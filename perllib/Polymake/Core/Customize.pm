#  Copyright (c) 1997-2015
#  Ewgenij Gawrilow, Michael Joswig (Technische Universitaet Berlin, Germany)
#  http://www.polymake.org
#
#  This program is free software; you can redistribute it and/or modify it
#  under the terms of the GNU General Public License as published by the
#  Free Software Foundation; either version 2, or (at your option) any
#  later version: http://www.gnu.org/licenses/gpl.txt.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#-------------------------------------------------------------------------------

use strict;
use namespaces;

package Polymake::Core::Customize;

use Polymake::Ext;
use Polymake::Enum qw( state: undefined private=1 default=2 global=4 saved=1+4 config=8 tied=16 hidden=32 accumulating=64 noexport=128 );

my $file_state;
my $var_state=0;
my $sep_line="########################\n";
my $print_state=$state_private;
my $export_state;

sub ARCH {
   $_[0] eq $Arch
   and $var_state=$state_config
}

sub help2comment {
   my ($text)=@_;
   $text =~ s/^\s*$//mg;
   $text =~ s/^/\# /mg;
   enforce_nl($text) if length($text);
   $text;
}

#################################################################################
package Polymake::Core::Customize::Var;
use Polymake::Struct (
   [ new => '$$;$' ],
   [ '$name' => '#1' ],
   [ '$state' => '#2' ],
   [ '$help' => '#3' ],
   '$appendix',
   [ '$extension' => 'undef' ],
);

sub state_after_set {
   my $self=shift;
   $self->state &= ~$state_default;
   $self->state |= $state_private;
}

#################################################################################
package Polymake::Core::Customize::Scalar;
use Polymake::Struct (
   [ '@ISA' => 'Var' ],
   [ '$private_value' => 'undef' ],
   [ '$default_value' => 'undef' ],
   [ '$global_value' => 'undef' ],
);

sub TIESCALAR {
   my $self=shift;
   if (is_object($self)) {
      $self->state |= $state_tied;
   } else {
      $self=_new($self, undef, $var_state | $file_state | $state_tied);
      $var_state=0;
      $self->global_value=$_[0] if $file_state & $state_global;
      $self->private_value=$_[0];
   }
   $self;
}

sub re_tie {
   my $self=shift;
   unless ($self->state & $state_tied) {
      no strict 'refs';
      tie $ {$self->name}, $self;
   }
}

sub un_tie {
   my $self=shift;
   $self->state &= ~$state_tied;
   if ($self->state & $state_config) {
      $self->default_value=$self->private_value;
   }
   no strict 'refs';
   untie $ {$self->name};
   $ {$self->name}=$self->private_value;
}

sub FETCH { $_[0]->private_value }

sub STORE {
   my $self=shift;
   if (defined $file_state) {
      $self->state |= $var_state | $file_state;
      $var_state=0;
      $self->global_value=$_[0] if $file_state & $state_global;
      $self->private_value=$_[0];
   } elsif ($self->state & $state_config) {
      $self->state |= $state_private;
      $self->private_value=$_[0];
   } else {
      $self->default_value=$_[0];
   }
}

sub new {
   no strict 'refs';
   my $self=tied ${$_[1]};
   if (defined $self) {
      $self->name=$_[1];
      $self->state|=$_[2];
      $self->help=$_[3];
   } else {
      $self=&_new;
      if (declared_scalar(*{$self->name})) {
         if (!($self->state & $state_config) and defined ($self->default_value=$ {$self->name})) {
            $self->state |= $state_default;
         } else {
            tie $ {$self->name}, $self;
            $self->state |= $state_tied;
         }
      } else {
         croak( "undefined custom variable \$", $self->name );
      }
   }
   $self;
}

sub set {
   my $self=$_[0];
   &state_after_set;
   no strict 'refs';
   if (@_) {
      $ {$self->name}=$self->private_value=$_[0];
   } else {
      $self->private_value=$ {$self->name};
   }
}

sub reset {
   my $self=shift;
   my $val=$self->default_value;
   if ($self->state & $state_config) {
      $self->private_value=$val;
   } elsif ($self->state & $state_global) {
      $self->private_value=$self->global_value;
      $self->state &= ~$state_private;
      $val=$self->global_value;
   } elsif ($self->state & $state_private) {
      $self->state=$state_default;
      undef $self->private_value;
   }
   no strict 'refs';
   $ {$self->name}=$val;
}

sub unset {
   my $self=shift;
   if (($self->state & ($state_config | $state_private)) == ($state_config | $state_private)) {
      $self->state=$state_undefined;
      undef $self->default_value;
      undef $self->private_value;
      undef $self->global_value;
      no strict 'refs';
      undef $ {$self->name};
   }
}

sub prefix { "\$" }

sub printable_value {
   my $self=$_[0];
   printable_scalar($self->state & $print_state ? $self->private_value : $self->default_value);
}

#################################################################################
package Polymake::Core::Customize::Array;
use Polymake::Struct (
   [ '@ISA' => 'Var' ],
   '@private_value',
   '@default_value',
   [ '@global_value' => 'undef' ],
);

sub TIEARRAY {
   my $self=shift;
   if (is_object($self)) {
      $self->state |= $state_tied;
   } else {
      $self=_new($self, undef, $var_state | $file_state | $state_tied);
      $var_state=0;
      @{$self->private_value}=@{$_[0]};
      $self->global_value=$self->private_value if $file_state & $state_global;
   }
   $self;
}


sub re_tie {
   my $self=shift;
   unless ($self->state & $state_tied) {
      no strict 'refs';
      tie @{$self->name}, $self;
   }
}

sub un_tie {
   my $self=shift;
   $self->state &= ~$state_tied;
   if ($self->state & $state_config) {
      @{$self->default_value}=@{$self->private_value};
   }
   no strict 'refs';
   untie @{$self->name};
   @{$self->name}=@{$self->private_value};
}

sub _changed_value {
   my $self=shift;
   if (defined $file_state) {
      $self->state |= $var_state | $file_state;
      $var_state=0;
      $self->private_value;
   } elsif ($self->state & $state_config) {
      $self->state |= $state_private;
      $self->private_value;
   } else {
      $self->default_value;
   }
}

sub FETCH { $_[0]->private_value->[$_[1]] }

sub FETCHSIZE { $#{$_[0]->private_value}+1 }

sub EXTEND {
   my $n=pop;
   $#{&_changed_value}=$n-1;
}

*STORESIZE=\&EXTEND;

sub CLEAR {
   my $self=shift;
   if (defined $file_state) {
      if ($file_state & $state_private) {
         if ($self->state & $state_global) {
            # divorce the global and private values
            $self->private_value=[ ];
         }
      } else {
         @{$self->global_value}=();
      }
      $self->state |= $var_state | $file_state;
      $var_state=0;
   } elsif ($self->state & $state_config) {
      @{$self->private_value}=();
   } else {
      @{$self->default_value}=();
   }
}

sub STORE {
   my $arr=&_changed_value;
   $arr->[$_[0]]=$_[1];
}

sub PUSH {
   my $arr=&_changed_value;
   push @$arr, @_;
}

sub UNSHIFT {
   my $arr=&_changed_value;
   unshift @$arr, @_;
}

sub SPLICE {
   my $arr=&_changed_value;
   splice @$arr, @_;
}

sub POP {
   pop @{&_changed_value};
}

sub SHIFT {
   shift @{&_changed_value};
}

sub new {
   no strict 'refs';
   my $self=tied @{$_[1]};
   if ($self) {
      $self->name=$_[1];
      $self->state|=$_[2];
      $self->help=$_[3];
      if ($self->state & $state_accumulating) {
         if ($self->state & $state_global) {
            if ($self->state & $state_private) {
               unshift @{$self->private_value}, @{$self->default_value}, @{$self->global_value};
            } else {
               $self->private_value=[ @{$self->default_value}, @{$self->global_value} ];
            }
         } else {
            unshift @{$self->private_value}, @{$self->default_value};
         }
      }
   } else {
      $self=&_new;
      if (defined *{$self->name}{ARRAY}) {
         if (!($self->state & $state_config) and @{$self->default_value}=@{$self->name}) {
            $self->state |= $state_default;
         } else {
            tie @{$self->name}, $self;
            $self->state |= $state_tied;
         }
      } else {
         croak( "undefined custom variable \@", $self->name );
      }
   }
   $self;
}

sub set {
   my $self=$_[0];
   $self->private_value=[ ] if ($self->state & $state_saved)==$state_global;
   &state_after_set;
   no strict 'refs';
   if (@_) {
      @{$self->name}=@_;
      @{$self->private_value}=@_;
   } else {
      @{$self->private_value}=@{$self->name};
   }
}

sub reset {
   my $self=shift;
   my $val=$self->default_value;
   if ($self->state & $state_config) {
      @{$self->private_value}=@$val;
   } elsif ($self->state & $state_global) {
      $val=$self->private_value=$self->global_value;
      $self->state &= ~$state_private;
   } elsif ($self->state & $state_private) {
      $self->state=$state_default;
      undef @{$self->private_value};
   }
   no strict 'refs';
   @{$self->name}=@$val;
}

sub unset {
   my $self=shift;
   if (($self->state & ($state_config | $state_private)) == ($state_config | $state_private)) {
      $self->state=$state_undefined;
      undef @{$self->default_value};
      undef @{$self->private_value};
      undef $self->global_value;
      no strict 'refs';
      undef @{$self->name};
   }
}

sub prefix { '@' }

sub printable_value {
   my $self=$_[0];
   my $comment_mark=
      $self->state & $print_state
      ? "" :
      $self->state & $state_global
      ? "#G# "
      : "# ";
   "(" .
   join(',', map { "\n$comment_mark   ".printable_scalar($_) }
        $self->state & $state_accumulating
        ? do { my $offset=@{$self->default_value} + (($self->state & $state_global) > ($self->state & $print_state) && @{$self->global_value});
               @{$self->private_value}[$offset..$#{$self->private_value}] }
        : @{ $self->state & $print_state ? $self->private_value : $self->default_value }) .
   "\n$comment_mark)"
}

#################################################################################
package Polymake::Core::Customize::Hash;
use Polymake::Struct (
   [ '@ISA' => 'Var' ],
   '%private_value',
   '%default_value',
   [ '%global_value' => 'undef' ],
);

sub TIEHASH {
   my $self=shift;
   if (is_object($self)) {
      $self->state |= $state_tied;
   } else {
      $self=_new($self, undef, $var_state | $file_state | $state_tied);
      $var_state=0;
      %{$self->private_value}=%{$_[0]};
      $self->global_value=$self->private_value if $file_state & $state_global;
   }
   $self;
}

sub re_tie {
   my $self=shift;
   unless ($self->state & $state_tied) {
      no strict 'refs';
      tie %{$self->name}, $self;
   }
}

sub un_tie {
   my $self=shift;
   $self->state &= ~$state_tied;
   if ($self->state & $state_config) {
      %{$self->default_value}=%{$self->private_value};
   }
   no strict 'refs';
   untie %{$self->name};
   %{$self->name}=%{$self->private_value};
}

sub FETCH { $_[0]->private_value->{$_[1]} }

sub EXISTS { exists $_[0]->private_value->{$_[1]} }

sub FIRSTKEY { each %{$_[0]->private_value} }

*NEXTKEY=\&FIRSTKEY;

sub CLEAR {
   my $self=shift;
   if (defined $file_state) {
      if ($file_state & $state_private  &&  $self->state & $state_global) {
         # divorce the global and private values
         $self->private_value={ %{$self->global_value} };
      }
      $self->state |= $var_state | $file_state;
      $var_state=0;
   } elsif ($self->state & $state_config) {
      $self->state |= $state_private;
      %{$self->private_value}=();
   } else {
      %{$self->default_value}=();
   }
}

sub STORE {
   my $self=shift;
   if (defined $file_state) {
      # state business already done in CLEAR
      $self->private_value->{$_[0]}=$_[1];
   } elsif ($self->state & $state_config) {
      $self->state |= $state_private;
      $self->private_value->{$_[0]}=$_[1];
   } else {
      $self->private_value->{$_[0]} //= $_[1] ;
      $self->default_value->{$_[0]}=$_[1];
   }
}

sub DELETE {
   my $self=shift;
   if ($self->state & $state_config) {
      $self->state |= $state_private;
      delete $self->private_value->{$_[0]};
   } else {
      delete $self->default_value->{$_[0]};
   }
}

sub new {
   no strict 'refs';
   my $self=tied %{$_[1]};
   if ($self) {
      $self->name=$_[1];
      $self->state|=$_[2];
      $self->help=$_[3];
   } else {
      $self=&_new;
      if (defined *{$self->name}{HASH}) {
         if (!($self->state & $state_config) and
             do { %{$self->default_value}=%{$self->name}; keys %{$self->default_value} }) {
            $self->state |= $state_default;
         } else {
            tie %{$self->name}, $self;
            $self->state |= $state_tied;
         }
      } elsif (!$self->state) {
         die "undefined custom variable %", $self->name, "\n";
      }
   }
   $self;
}

sub set {
   my $self=$_[0];
   my $divorce= ($self->state & $state_saved)==$state_global;
   &state_after_set;
   no strict 'refs';
   if (@_==0) {
      $self->private_value={ } if $divorce;
      %{$self->private_value}=%{$self->name};
   } elsif (@_==1) {
      $self->private_value={ %{$self->global_value} } if $divorce;
      $self->private_value->{$_[0]}=$self->name->{$_[0]};
   } elsif (@_==2) {
      $self->private_value={ %{$self->global_value} } if $divorce;
      $self->private_value->{$_[0]}=$_[1];
      $self->name->{$_[0]}=$_[1];
   } else {
      $self->private_value={ } if $divorce;
      %{$self->private_value}=@_;
      %{$self->name}=@_;
   }
}

sub reset {
   my $self=shift;
   if (@_) {
      foreach my $key (@_) {
         no strict 'refs';
         if ($self->state & $state_global && exists $self->global_value->{$key}) {
            ${$self->name}{$key}=$self->private_value->{$key}=$self->global_value->{$key};
         } elsif (exists $self->default_value->{$key}) {
            ${$self->name}{$key}=$self->private_value->{$key}=$self->default_value->{$key};
         } else {
            delete $self->private_value->{$key};
            delete ${$self->name}{$key};
         }
      }
   } else {
      my $val=$self->default_value;
      if ($self->state & $state_config) {
         %{$self->private_value}=%$val;
      } elsif ($self->state & $state_global) {
         $val=$self->private_value=$self->global_value;
         $self->state &= ~$state_private;
      } elsif ($self->state & $state_private) {
         $self->state=$state_default;
         undef %{$self->private_value};
      }
      no strict 'refs';
      %{$self->name}=%$val;
   }
}

sub unset {
   my $self=shift;
   if (($self->state & ($state_config | $state_private)) == ($state_config | $state_private)) {
      $self->state=$state_undefined;
      undef %{$self->default_value};
      undef %{$self->private_value};
      undef $self->global_value;
      no strict 'refs';
      undef %{$self->name};
   }
}

sub prefix { '%' }

sub printable_value {
   my $self=$_[0];
   my $help=$self->help;
   sanitize_help($help);
   my ($preface, %elements)=split /^\s*\@key\s+(\S+)\s+/m, $help;
   my $glob_vals= ($self->state & $state_global) ? $self->global_value : undef;
   if ($self->state & $print_state) {
      ( join("", map {
                    my $quoted= /\W/ ? "'$_'" : $_;
                    if ($export_state && ($self->state & ($state_config | $state_accumulating)) == ($state_config | $state_accumulating)
                        && !$self->private_value->{$_}) {
                       # don't show unconfigured values in export mode
                       ()
                    } else {
                       my $h=$elements{$_};
                       my $comment_mark="#  ";
                       ( ($glob_vals && $glob_vals->{$_} eq $self->private_value->{$_}  and  $comment_mark="#G#  ") ||
                         (!($self->state & $state_config) && $self->default_value->{$_} eq $self->private_value->{$_})
                         ? "$comment_mark  $quoted => " . printable_scalar($self->default_value->{$_})
                         : "   $quoted => " . printable_scalar($self->private_value->{$_}),
                         defined($h) ? " ,  " . help2comment($1) : " ,\n"
                       )
                    }
                 } sort keys %{$self->private_value} ),
        help2comment($preface)
      )
   } else {
      ( join("", map {
                    my $quoted= /\W/ ? "'$_'" : $_;
                    my $h=$elements{$_};
                    ( defined($glob_vals) && defined($glob_vals->{$_}) ? "#G#  " : "#  ",
                      $quoted, " => ", printable_scalar($self->default_value->{$_}),
                      defined($h) ? " ,  " . help2comment($1) : " ,\n"
                    )
                 } sorted_uniq(sort( keys(%{$self->default_value}), defined($glob_vals) ? keys(%$glob_vals) : () )) ),
        help2comment($preface)
      )
   }
}

sub printable {
   my $self=$_[0];
   my ($v, $h)=&printable_value;
   $h .= "# \n" if $h =~ /^.*[^\#\s]\s*\E/m;
   $h . &printable_decl . "(\n" . $v .
   ($self->state & $print_state ? "" : "# ") . ");\n"
}

#################################################################################
package Polymake::Core::Customize::Var;

# A custom variable is tied if it was loaded from the customization file.
# This way we can catch its default value which would otherwise overwrite
# the customized one.

my %prefix_dispatch=( '$' => 'Polymake::Core::Customize::Scalar',
                      '@' => 'Polymake::Core::Customize::Array',
                      '%' => 'Polymake::Core::Customize::Hash' );
sub new {
   $prefix_dispatch{substr($_[1],0,1)}->new(substr($_[1],1), $_[2], $_[3] || "undocumented");
}

sub full_name {
   my ($self, $default_pkg)=@_;
   my $l;
   defined($default_pkg)  &&  rindex($self->name,"::")==($l=length($default_pkg))  &&  substr($self->name,0,$l) eq $default_pkg
   ? $self->prefix . substr($self->name,$l+2)
   : $self->prefix . $self->name
}

#################################################################################
sub printable_scalar {
   defined($_[0]) ? is_numeric($_[0]) ? $_[0] : "'$_[0]'" : "undef";
}

my $arch_prefix="ARCH('$Arch') and\n";

sub printable_decl {
   my $self=$_[0];
   my $comment_mark=
      $self->state & $print_state
      ? "" :
      $self->state & $state_global
      ? "#G# "
      : "# ";
   ( $self->state & $state_config
     ? $comment_mark . $arch_prefix . $comment_mark
     : $comment_mark
   ) . &full_name . "=" 
}

sub printable {
   my $self=$_[0];
   my $help=$self->help;
   sanitize_help($help);
   help2comment($help) . &printable_decl . $self->printable_value . ";\n"
}

#################################################################################
package Polymake::Core::Customize::perApplication;

use Polymake::Struct (
   [ new => '$$' ],
   [ '$handler' => 'weak( #1 )' ],
   '%per_pkg',                  # package_name->{"$var_name"}->Var
   '%pkg_help',                 # package_name => help text
   [ '$default_pkg' => '#2' ],
   [ '$help' => 'undef' ],
   '@tied_vars',
);

sub new {
   my $self=&_new;
   my $global_text=delete $self->handler->global_pieces->{$self->default_pkg};
   my $private_text=$self->handler->private_pieces->{$self->default_pkg};
   if (defined($global_text) || defined($private_text)) {
      { no strict 'refs';
        *{$self->default_pkg."::ARCH"}=\&ARCH;
      }
      compile_start();
      if (defined $global_text) {
         $file_state=$state_global;
         eval $global_text;
         die $@ if $@;
      }
      if (defined $private_text) {
         $file_state=$state_private;
         eval $private_text;
         die $@ if $@;
      }
      compile_end();
      undef $file_state;
      { no strict 'refs';
        unimport_function(*{$self->default_pkg."::ARCH"});
      }
   } elsif (defined($self->handler->private_file)) {
      $self->handler->need_save=1;
   }
   $self;
}

#################################################################################
sub add {
   my ($self, $name, $help_text, $state, $pkg)=@_;
   if (defined $pkg) {
      substr($name,1,0).="$pkg\::";
   } elsif ((my $pkg_end=rindex($name, "::"))>0) {
      $pkg=substr($name,1,$pkg_end-1);
   } else {
      $pkg=caller;
      substr($name,1,0).="$pkg\::";
   }
   my $var= ( $self->per_pkg->{$pkg}->{$name} &&= croak( "multiple definition of custom variable '$name'" ) )
        ||= new Var($name,$state,$help_text);
   if ($var->state & $state_tied) {
      push @{$self->tied_vars}, $var;
   }
   $var;
}
#################################################################################
sub find {
   my ($self, $name, $skip_default)=@_;
   my $pkg_end=rindex($name, "::");
   my $dict;
   if ($pkg_end>0) {
      if (not $dict=$self->per_pkg->{substr($name,1,$pkg_end-1)} and
          !$skip_default and
          $dict=$self->per_pkg->{$self->default_pkg . "::" . substr($name,1,$pkg_end-1)}) {
         substr($name,1,0)=$self->default_pkg."::";
      }
   } elsif (!$skip_default) {
      $dict=$self->per_pkg->{$self->default_pkg};
      substr($name,1,0)=$self->default_pkg."::";
   }
   $dict && $dict->{$name};
}
#################################################################################
sub list_completions_in_pkg {
   my ($dict, $type, $prefix)=@_;
   my $var;
   # don't expose variables declared as hidden, as well as unconfigured variables; the latter shall be set via rule reconfiguration
   grep { $var=$dict->{$_}; not (($var->state & $state_hidden) || ($var->state & ($state_config | $state_saved)) == $state_config) }
      do {
         if ($type eq "\$") {
            grep { /^[%\$]\Q$prefix\E/ } keys %$dict;
         } else {
            grep { /^\Q$type$prefix\E/ } keys %$dict;
         }
      };
}

sub list_completions {
   my ($self, $type, $prefix, $skip_default)=@_;
   my @answer;
   my $l=length($self->default_pkg);
   if (!$skip_default) {
      my $last_qual=rindex($prefix,"::");
      if (defined (my $dict=$self->per_pkg->{$self->default_pkg . ($last_qual>0 && "::".substr($prefix,0,$last_qual))})) {
         push @answer, map { substr($_,$l+3) } list_completions_in_pkg($dict, $type, $self->default_pkg."::$prefix");
      }
   }
   while (my ($pkg, $dict)=each %{$self->per_pkg}) {
      if ($prefix =~ /^$pkg (?: : (?: : [^:]*)?)? $/x) {
         push @answer, map { substr($_,1) } list_completions_in_pkg($dict,$type,$prefix);
      }
      my $remove_default_prefix= !$skip_default && substr($pkg,0,$l) eq $self->default_pkg && substr($pkg,$l,2) eq "::";
      if ($remove_default_prefix ? substr($pkg,$l+2) =~ /^$prefix/ : $pkg ne $self->default_pkg && $pkg =~ /^$prefix/) {
         while ((undef, my $var)=each %$dict) {
            unless (($var->state & $state_hidden) || ($var->state & ($state_config | $state_saved)) == $state_config) {
               push @answer, ($remove_default_prefix ? substr($pkg,$l+2) : $pkg)."::";
               keys %$dict;  # reset the iterator!
               last;
            }
         }
      }
   }
   @answer;
}
#################################################################################
sub set {
   my ($self, $name)=splice @_, 0, 2;
   if (defined (my $var=$self->find($name))) {
      $var->set(@_);
      $self->handler->need_save=1;
   } else {
      croak( "unknown custom variable $name" );
   }
}
#################################################################################
sub reset {
   my ($self, $name)=splice @_, 0, 2;
   if (defined (my $var=$self->find($name))) {
      $var->reset(@_);
      $self->handler->need_save=1;
   } else {
      croak( "unknown custom variable $name" );
   }
}
#################################################################################
sub unset {
   my ($self, $name, $pkg)=@_;
   if (defined $pkg) {
      substr($name,1,0).="$pkg\::";
   } else {
      my $pkg_end=rindex($name, "::");
      $pkg=substr($name,1,$pkg_end-1);
   }
   if (defined (my $var=$self->per_pkg->{$pkg}->{$name})) {
      $var->unset;
      $self->handler->need_save=1;
   } else {
      croak( "unknown custom variable $name" );
   }
}
#################################################################################
sub delete_from_private_list {
   my ($self, $name, $pos)=@_;
   if (defined (my $var=$self->find($name))) {
      if ($var->state & $state_accumulating) {
         if ($pos >= @{$var->default_value} + ($var->state & $state_global && @{$var->global_value})) {
            splice @{$var->private_value}, $pos, 1;
            no strict 'refs';
            splice @{$var->name}, $pos, 1;
            $self->handler->need_save=1;
         } else {
            # not allowed
            0
         }
      } else {
         croak( "Customize::delete_from_private_list is only applicable to accumulating arrays" );
      }
   } else {
      croak( "unknown custom variable $name" );
   }
}
#################################################################################
sub cleanup {
   my $self=shift;
   $_->un_tie for @{$self->tied_vars};
   @{$self->tied_vars}=() if @_==0;     # called directly
}
#################################################################################
sub re_tie {
   my ($self, $name, $help_text, $state, $pkg)=@_;
   if (defined $pkg) {
      substr($name,1,0).="$pkg\::";
   } else {
      my $pkg_end=rindex($name, "::");
      $pkg=substr($name,1,$pkg_end-1);
   }
   if (defined (my $var=$self->per_pkg->{$pkg}->{$name})) {
      $var->re_tie;
      push @{$self->tied_vars}, $var;
      undef;
   } else {
      &add;
   }
}
#################################################################################
sub obliterate_extension {
   my ($self, $ext)=@_;
   while (my ($pkg, $per_pkg)=each %{$self->per_pkg}) {
      while (my ($name, $var)=each %$per_pkg) {
         if ($var->extension==$ext) {
            delete $per_pkg->{$name};
            $self->handler->need_save=1;
         }
      }
      unless (keys %$per_pkg) {
         delete $self->per_pkg->{$pkg};
      }
   }
}
#################################################################################
sub printMe {
   my ($self, $cf, $old_text)=@_;
   print $cf "package ".$self->default_pkg.";\n\n";
   if (length($old_text)) {
      local $_=$old_text;
      recycle_piece($cf,$self);
   }
   foreach my $pkg (sort { $a eq $self->default_pkg ? -1 : $b eq $self->default_pkg ? 1 : $a cmp $b } keys %{$self->per_pkg}) {
      if (defined (my $h=$self->pkg_help->{$pkg})) {
         sanitize_help($h);
         print $cf help2comment($h), "\n";
      }
      while (my ($name, $var)=each %{$self->per_pkg->{$pkg}}) {
         print $cf $var->printable($self->default_pkg), $var->appendix, "\n";
      }
   }
}
#################################################################################
sub export {
   my ($self, $cf)=@_;
   my $seen_in_app;
   foreach my $pkg (sort { $a eq $self->default_pkg ? -1 : $b eq $self->default_pkg ? 1 : $a cmp $b } keys %{$self->per_pkg}) {
      my $seen_in_pkg;
      while (my ($name, $var)=each %{$self->per_pkg->{$pkg}}) {
         if (($var->state & $print_state) && !($var->state & $state_noexport)) {
            $seen_in_app ||= print $cf $sep_line, "package ".$self->default_pkg.";\n\n";
            $seen_in_pkg ||= do {
               if (defined (my $h=$self->pkg_help->{$pkg})) {
                  sanitize_help($h);
                  print $cf help2comment($h), "\n";
               }
               1
            };
            print $cf $var->printable($self->default_pkg), "\n";
         }
      }
   }
}
#################################################################################
sub create_help_topics {
   my ($self, $help_root)=@_;
   $self->help=$help_root->add([ 'custom' ], <<'.');
# All customizable variables, ordered by packages.
# Please remember that the changes made by direct assignment will be lost after the session end.
# To make perpetual changes, please use the commands set_custom and reset_custom, or edit the
# file ~/.polymake/customize.pl manually (but only after finishing the interactive session!)
.
   my $l=length($self->default_pkg)+2;
   while (my ($pkg, $dict)=each %{$self->per_pkg}) {
      my $skip= $pkg eq $self->default_pkg || index($pkg, $self->default_pkg."::")==0 ? $l : 0;
      while (my ($name, $var)=each %{$self->per_pkg->{$pkg}}) {
         unless ($var->state & $state_hidden) {
            $self->help->add([ $var->prefix.substr($var->name,$skip) ], $var->help);
         }
      }
   }
}
#################################################################################
package Polymake::Core::Customize::File;
use Polymake::Struct (
   [ new => '$' ],
   [ '$filename' => '#1' ],     # where to save custom variables
   '%pieces',                   # package => file fragment
   '$version',                  # version of polymake which has created this custom file
);

sub new {
   my $self=&_new;
   my $filename=$self->filename;
   open my $cf, $filename
      or die "can't read file $filename: $!\n";
   local $_;
   my ($pkg, $body, $version)=("BEGIN", <<".");
no namespaces; no strict 'vars';
package main;
#line 1 "$filename"
.
   while (<$cf>) {
      if (/^\s* (?: (?:application|module) \s+ ($id_re) | package \s+ ($qual_id_re)); \s*$/xo) {
         $self->pieces->{$pkg}=$body;
         if (defined $2) {
            $pkg=$2;
            $body= <<".";
no namespaces; no strict 'vars';
#line $. "$filename";
.
         } else {
            $pkg=$1;
            my $l=$.+1;
            $body=<<".";
#line $l "$filename";
.
            next;
         }
      } else {
         # convert from old style
         s/\bCONFIGURED(?=\()/ARCH/ ||
         s/\bCONFIGURE:/ARCH('$Arch') and/;
      }
      $body.=$_;
   }
   $self->pieces->{$pkg}=$body;

   my $preamble=delete $self->pieces->{BEGIN};
   # delete old global flags
   $preamble =~ s/(?:CONFIGURED|ARCH)(?:\(.*?\))?/0/gm;
   my $version=v0.0;
   eval $preamble;
   if ($@) {
      err_print( beautify_error() );
      return;
   }
   $self->version=$version;

   if ($version le v2.9.10 && $version gt v2.3) {
      delete $self->pieces->{"Polymake::Core::Preference"};
      if (defined ($body=delete $self->pieces->{"Poly::User"})) {
         $body =~ s/\bPoly::User\b/Polymake::User/g;
         $self->pieces->{"Polymake::User"}=$body;
      }
      if (defined ($body=delete $self->pieces->{"Poly::CPlusPlus"})) {
         $body =~ s/\bPoly::CPlusPlus\b/Polymake::Core::CPlusPlus/g;
         $self->pieces->{"Polymake::Core::CPlusPlus"}=$body;
      }
   }

   $self;
}
#################################################################################
package Polymake::Core::Customize;

use Polymake::Struct (
   [ '$private_file' => 'undef' ],      # Customize::File
   '@per_app',                          # perApplication
   '%private_pieces',                   # package => private file fragment
   '%global_pieces',                    # package => concatenated global files fragments
   '$need_save',                        # boolean
);

sub load_private {
   my ($self, $filename)=@_;
   if (-f $filename) {
      $self->private_file=new File($filename) or return;
      $self->private_pieces=$self->private_file->pieces;
      $self->need_save= !defined($Version) || $self->private_file->version lt $VersionNumber;
      add AtEnd("Customize", sub { $self->save if $self->need_save });
   } else {
      $self->need_save=1;
      add AtEnd("Customize", sub { $self->save($filename) });
   }
}

sub load_global {
   my ($self, $filename)=@_;
   my $file=new File($filename) or return;
   if (defined $file->version) {
      if ($file->version lt $VersionNumber) {
         warn_print( "global configuration file $filename seems to be left over from an older version.\n",
                     "To get rid of this warning, please revise it and set the version number to $Version" );
      } elsif ($file->version gt $VersionNumber) {
         warn_print( "global configuration file $filename claims to be of a newer version than your polymake installation" );
         return;
      }
   }
   while (my ($pkg, $text)=each %{$file->pieces}) {
      $self->global_pieces->{$pkg} .= $text;
   }
}

sub app_handler {
   my ($self, $pkg)=@_;
   my $per_app=new perApplication($self,$pkg);
   push @{$self->per_app}, $per_app;
   $per_app
}

my $preface=<<".";
#########################################################################
#
#  This file contains copies of all customizable variables
#  scattered across the rule files.
#
#  The assignments can stay commented out as long as you are satisfied
#  with the default settings.  To change them, either edit this file
#  directly or call the user function 'set_custom'.
#  To revert to the default values later, simply comment out
#  the corresponding assignments or call the user function 'reset_custom'.
#
#  Elements of hash arrays can be activated selectively,
#  entries with default values may stay commented as long as needed.
#  But don't forget the last line with the closing parenthesis!
#
#  Be sure to edit this file only after you have finished the
#  interactive polymake session, otherwise your changes can be lost.
#
#  Some variables come from the rule autoconfiguration sections.
#  Their assignments are guarded by the `ARCH' condition, since they may
#  have different values for different architectures.
#
#  Variables commented out with special marker #G# are actually inherited
#  from some global configuration file.  The effective values can be queried
#  from the interactive shell only.  Activating these variables here
#  will override the global settings.
#
#  Please be aware that this file is loaded as the very first,
#  before any rule file.  It gives the settings made here a chance
#  to be compiled in the rule code.  But you cannot refer here to any
#  objects created in the course of parsing the rule files, that is,
#  applications, object types, or preference lists.
#  Please use another file in this directory: "prefer.pl" for these
#  purposes.
#
#########################################################################
#
# The rule files are rescanned for new custom variables as soon as you
# run a polymake version newer than recorded here, or use an application
# for the first time.
# If you have introduced new custom variables in the rules and want them
# to appear here right now, comment out the following line and run polymake.
\$version=v$Version;

.

# variable assignment as full statement
my $assignment_re=qr{ ([\$\@%] $qual_id_re) \s*= (?: [^\#\n]+; | .*\n (?: ^.*\n)*? ^ [^\#\n]+; ) }xom;

# protected:
sub recycle_piece {
   my ($cf, $ch)=@_;
   s/\A(?s:.*?)^\#line.*\n^\s*package\s+$qual_id_re;[ \t]*\n//mo;

   while (pos() < length()) {
      /\G (?: ^\s*\n )+ /xmgc;          # empty lines
      my $start_comments=pos();
      /\G (?: ^[ \t]* \#.*\n )+ /xmgc;  # leading comments

      if (/\G ^[ \t]* $assignment_re .*\n /xomgc) {
         # unconditional assignment - either stored in the application's dictionary or obsolete
         next;
      }

      my $print_nl="";
      my $start_stmt=pos();
      my $saved_comments="";

      while (/\G ^[ \t]* ARCH (?: \((['"]) (.*?) \1\) )? \s*and\s*
                 ^[ \t]* $assignment_re .*\n /xomgc) {
         my ($arch, $varname)=($2, $3);
         if ($arch) {
            if ($arch ne $Arch) {
               # value configured for other architecture
               if (defined($ch->default_pkg) && index($varname, "::")<0) {
                  substr($varname,0,1) .= $ch->default_pkg . "::";
               }
               if (my $var=$ch->find($varname)) {
                  $var->appendix .= substr($_,$start_stmt,pos()-$start_stmt);
               } else {
                  print $cf $saved_comments, substr($_,$start_comments,pos()-$start_comments);
                  $saved_comments="";
                  $print_nl="\n";
               }
            } else {
               $saved_comments=substr($_,$start_comments,$start_stmt-$start_comments);
            }
         }
         $start_comments=$start_stmt=pos();
      }
      print $cf $print_nl;

      if (/\G ^[ \t]* [^\#\s] .* \n/xmgc) {
         warn_print( "suspicious code in custom file: ", $& );
         print $cf $&;
      }
   }
}

sub save {
   my ($self, $filename)=@_;
   if (!defined($filename)) {
      die "no customization file to save\n" unless $self->private_file;
      $filename=$self->private_file->filename;
   }

   my ($cf, $cf_k)=new OverwriteFile($filename);
   print $cf $preface;

   foreach my $per_app (@{$self->per_app}) {
      $per_app->cleanup;
      print $cf $sep_line;
      $per_app->printMe($cf, delete $self->private_pieces->{$per_app->default_pkg});
   }

   my $sep=$sep_line;
   while (my ($pkg, $orphan)=each %{$self->private_pieces}) {
      if ($pkg !~ /^(?:Modules|Apps)::/) {
         $orphan =~ s/\A(?s:.*?)^\#line.*\n^\s*package\s+$qual_id_re;[ \t]*\n//mo;
         print $cf $sep, "package $pkg;\n", $orphan;
         undef $sep if length($orphan) > length($sep_line) && rindex($orphan, $sep_line) == length($orphan)-length($sep_line);
      }
   }

   close $cf;
   $self->need_save=0;
}
##################################################################################
sub export {
   my ($self, $filename, $merge_with_global, @also)=@_;
   my ($cf, $cf_k)=new OverwriteFile($filename);
   print $cf <<".";
#########################################################################
#
#  This file is an excerpt of the configuration base of polymake,
#  suitable to be included in the configuration path.
#  Put its full name in the environment variable
#    POLYMAKE_CONFIG_PATH="/absolute/path;user"
#  or specify it on the command line:
#    polymake --config-path "/absolute/path;user"
#
\$version=v$Version;

.
   local_scalar($export_state, 1);
   local_scalar($print_state, $state_private | $state_global) if $merge_with_global;
   foreach my $per_app (@{$self->per_app}, @also) {
      $per_app->cleanup;
      $per_app->export($cf);
   }
   close $cf;
}
##################################################################################
sub obliterate_application {
   my ($self, $per_app)=@_;
   if (delete_from_list($self->per_app, $per_app)) {
      delete $self->private_pieces->{$per_app->default_pkg};
      $self->need_save=1;
   }
}

1

# Local Variables:
# cperl-indent-level:3
# indent-tabs-mode:nil
# End:
