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

package Polymake::Core::Permutation;

use Polymake::Struct (
   [ '@ISA' => 'Property' ],
   [ new => '$$' ],
   [ '$flags' => '$is_subobject | $is_locally_derived | $is_permutation' ],
   [ '$type' => '#1' ],
   [ '$name' => '#2' ],
   [ '$belongs_to' => '#1' ],

   '%sensitive_props',                  # Property->key => [ Rule ]  or  { $sub_key }->{ Property }-> ... ->[ Rule ]
                                        # rules transferring Property from the permuted subobject back into the basis
   '%sub_permutations',                 # Property->key (subobject) => Property (permutation)
   '%parent_permutations',              # Property->key (parent object) => ... => $sub_key => [ Property (permutation) ]
   [ '$sensitivity_check' => 'undef' ], # pseudo-rule for canned schedules
);

sub new {
   my $self=&_new;
   $self->mixin=new ObjectType::LocalDerivationMixin($self, $self->type);
   $self->inst_cache={ };
   if ($self->type->abstract) {
      $self->accept=sub : method {
         my $self=shift;
         clone($self, $_[1])->accept->(@_);
      };
   } else {
      $self->accept=sub : method {
         my $self=shift;
         clone_locally_derived($self, $_[1]->type)->accept->(@_);
      };
   }

   $self;
}
####################################################################################
sub create_locally_derived_type : method {
   my ($self, $parent_proto)=@_;
   _clone($self, new ObjectType::Permuted($parent_proto, $self->mixin));
}
####################################################################################

declare $sub_key=\(1);      # singular key value used in sensitive_props

# Rule => true if it is a recovering rule for a permuted property
sub analyze_rule {
   my ($self, $rule)=@_;
   my ($regular_out_seen, $perm_out_seen);
   foreach my $output (@{$rule->output}) {
      if (@$output == 1) {
         my $prop=$output->[0];
         if ($rule->code==\&Rule::nonexistent) {
            if ($Application::plausibility_checks && ref($self->sensitive_props->{$prop->key})) {
               croak( "recovery of property ", $prop->name, " was enabled prior to this rule definition" );
            }
            $self->sensitive_props->{$prop->key}=[ ];
         } else {
            if ($Application::plausibility_checks && exists($self->sensitive_props->{$prop->key}) &&
                @{$self->sensitive_props->{$prop->key}}==0) {
               croak( "recovery of property ", $prop->name, " was disabled prior to this rule definition" );
            }
            push @{$self->sensitive_props->{$prop->key}}, $rule;
         }
         $regular_out_seen=1;
      } else {
         if ($Application::plausibility_checks && get_array_flags($output) & $is_multiple_new) {
            croak( "Rule dealing with permutations may not create new multiple subobjects" );
         }
         my $perm_pos=find_first_in_path($output, $is_permutation);
         if ($perm_pos>=0) {
            if ($perm_pos==0) {
               if ($Application::plausibility_checks && $output->[0] != $self) {
                  croak( "dependence between two different permutation subobjects on the same level is not allowed" );
               }
            } else {
               add_sub_permutation($self, @$output[0..$perm_pos]);
            }
            $perm_out_seen=1;
         } else {
            add_sensitive_sub_property($self, @$output, $rule);
            $regular_out_seen=1;
         }
      }
   }
   if ($perm_out_seen && $regular_out_seen) {
      croak( "A production rule can't create properties in the base object and permutation subobject at the same time" );
   }
   $regular_out_seen
}

####################################################################################
# private:
sub add_sensitive_sub_property {
   my $self=shift;
   my $rule=pop;
   my $prop=pop;
   my $hash=$self->sensitive_props;
   foreach $prop (@_) {
      $hash=($hash->{$sub_key} ||= { });
      $hash=($hash->{$prop->key} ||= { });
   }
   if ($rule->code==\&Rule::nonexistent) {
      if ($Application::plausibility_checks && ref($hash->{$prop->key})) {
         croak( "recovery of property ", $prop->name, " was enabled prior to this rule definition" );
      }
      $hash->{$prop->key}=[ ];
   } else {
      if ($Application::plausibility_checks && exists($hash->{$prop->key}) && @{$hash->{$prop->key}}==0) {
         croak( "recovery of property ", $prop->name, " was disabled prior to this rule definition" );
      }
      push @{$hash->{$prop->key}}, $rule;
   }
}
####################################################################################
# parent Permutation, (path to subobject permutation Property) =>
sub add_sub_permutation {
   my $self=shift;
   my $sub_permutation=pop;
   my $prop=pop;
   my $hash=$self->sub_permutations;
   foreach $prop (@_) {
      $hash=($hash->{$sub_key} ||= { });
      $hash=($hash->{$prop->key} ||= { });
   }
   if (exists $hash->{$prop->key}) {
      $hash->{$prop->key}==$sub_permutation or
      croak( "ambiguous permutation propagation into ", join(".", map { $_->name } @_,$prop), " : ",
             $sub_permutation->name, " vs. ", $hash->{$prop->key}->name );
   }

   $hash->{$prop->key}=$sub_permutation;
   # provide for transitive closure
   my $deeper_sub_perms=$sub_permutation->sub_permutations;
   if (keys %$deeper_sub_perms) {
      $hash=($hash->{$sub_key} ||= { });
      $hash->{$prop->key}=$deeper_sub_perms;
   }
   # establish reversed relation
   $hash=$sub_permutation->parent_permutations;
   foreach my $parent_prop ($prop, reverse @_) {
      $hash=($hash->{$parent_prop->key} ||= { });
   }
   push @{$hash->{$sub_key}}, $self;
}
####################################################################################
sub find_sensitive_sub_property {
   my $self=shift;
   my $prop=pop;
   my $hash=$self->sensitive_props;
   foreach $prop (@_) {
      defined( $hash=$hash->{$sub_key} ) or return;
      defined( $hash=$hash->{$prop->key} ) or return;
   }
   $hash->{$prop->key}
}
####################################################################################
sub find_sub_permutation {
   my $self=shift;
   my $prop=pop;
   my $hash=$self->sub_permutations;
   foreach $prop (@_) {
      defined( $hash=$hash->{$sub_key} ) or return;
      defined( $hash=$hash->{$prop->key} ) or return;
   }
   $hash->{$prop->key}
}

1

# Local Variables:
# cperl-indent-level: 3
# indent-tabs-mode:nil
# End:
