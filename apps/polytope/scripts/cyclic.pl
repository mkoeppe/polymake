#########################################################
# ------------------------------------------------------
# This script contains functions for working with
# series of core points with respect to 
# cyclic groups of even order.
# ---
# A circulant matrix is a matrix generated by one 
# vector which is shifted in each row by one position.
# The resulting vectors are the elements of the orbit
# of the generating vector under the cyclic group
# acting on the coordinates.
#
# Cyclic groups (of even order) have infinite core sets.
# This can be shown by describing an infinite series of
# core points in one 1-layer, for instance, in H_{1,1}.
# Two different constructions of such series are
# realized by the functions 
#  - infinite_cp_series_one_dir
#  - infinite_cp_series_mult_dir
#
# The function 'nested_C4' produces a certain point
# whose NOP-graph (see object OrbitPolytope) is a
# path ending in some core point with respect to C4.
#
# ------------------------------------------------------
# subs:
# - circulant($vec)
# - circ_vec($l, $params_ref)
# - random_vec($l,$limit)
# - circ_rank_test($l_low, $l_high, $limit)
# - infinite_cp_series_one_dir($l, $m_low, $m_up)
# - infinite_cp_series_mult_dir($even_params_ref)
# - nested_C4($a, $b, $r)
# ------------------------------------------------------
#########################################################


# Produces a circulant matrix from the given vector //vec//.
# @param Vector vec the generating vector for the circulant matrix
# @return Matrix the circulant matrix corresponding to //vec//
sub circulant {
    my $vec = $_[0];
    my $n = $vec->dim;
    my $circ = new Matrix ($n,$n);
    for ( my $i=0; $i < $n; ++$i ) {
	for ( my $j=0; $j < $n; ++$j ) {
	    my $ind =  ($j-$i) % $n;
	    $circ->[$i]->[$j] = $vec->[$ind];
	}	
    }
    return $circ;
}


# Help function.
# Constructs a vector of the form
# z=e_1+\sum_{j_1}^{2l-2} k_jw_j
# where w_j = e_j-e_{j+2}.
# The parameters k_j are stored in the array params.
# @param Int l defines the dimension 2*l of the vector (no homogenization!)
# @param arrayref params_ref the parameters k_j
# @return Vector the generated vector 
sub circ_vec {
    my ($l, $params_ref) = @_;
    if ( $l < 2 ) {
	die "circ_vec: l must be greater than 1!";
    }
    if ( scalar(@$params_ref) != 2*($l-1) ) {
	die "circ_vec: wrong number of parameters!";
    }
    my $vec = new Vector(2*$l);
    $vec->[0] = 1 + $params_ref->[0];
    $vec->[1] = $params_ref->[1];
    for (my $i = 2; $i < 2*($l-1); ++$i) {
	$vec->[$i] = $params_ref->[$i] - $params_ref->[$i-2];
    }
    $vec->[2*$l-2] = -$params_ref->[2*$l-4];
    $vec->[2*$l-1] = -$params_ref->[2*$l-3];
    return $vec;
}


# Help function.
# Constructs a list of random parameters k_j
# e.g. for the function circ_vec for a given length of
# 2//l//-2. The random number between 0 and 1 is scaled 
# by //limit//.
# @param Int l defines the dimension 2*l-2 of the list
# @param Int limit the factor by which the random numbers are scaled
# @return arrayref a reference to an array of random parameters
sub random_vec {
    my ($l,$limit) = @_;
    my @params = ();
    for (my $i=0; $i<2*($l-1); ++$i) {
	my $random_number = rand();
	push(@params,(new Int(rand()*$limit)));
    }
    return \@params;
}


# Tests the ranks of circulant matrices of randomly generated
# vectors of the form described in the function circ_vec.
# The dimension of the generating vector varies between
# //l_low// and //l_high//. The parameter //limit// refers
# to the scaling factor for the randomly generated
# parameters k_j, see Function random_vec.
# @param Int l_low defines the lower limit of the dimension (l>1)
# @param Int l_high defines the upper limit of the dimension
# @param Int limit the factor by which the random numbers are scaled
# @return OUTPUT produces a warning if a matrix does not have full rank;
#                otherwise it prints "." for every successful test
sub circ_rank_test {
    if ( $l < 2 ) {
	die "circ_rank_test: l must be greater than 1!";
    }
    my ($l_low, $l_high, $limit) = @_;
    for (my $l=$l_low; $l<=$l_high; ++$l) {
	my $v=circ_vec($l,random_vec($l,$limit));
	my $m=circulant($v);
	if (rank($m)==2*$l) {
	    print ".";
	} else {
	    print "\n Circulant of [$v] does not have full rank! \n";	    
	}
	if ($l % 10 == 0) {
	    print "\n";
	}
    }
}


#---------------------------------------
# Orbit polytopes with respect to C_2l
#---------------------------------------

# Produces members of an infinite series of core points
# 'in one direction' with respect to the cyclic group of order 2//l//.
# The points are of the form
# z=e_1+m*w
# where w=sum_{i=1}^l(e_{2i-1}-e_{2i}).
# All points for m between //m_low// and //m_up// are produced.
# @param Int l defines the dimension 2*l of the core points
# @param Int m_low the lower limit for the parameter m
# @param Int m_up the upper limit for the parameter m
# @return Matrix a matrix of core points for C_2l
sub infinite_cp_series_one_dir {
    my ($l, $m_low, $m_up) = @_;
    my $z0 = new Vector(1|(new Vector(unit_vector<Rational>(2*$l,0))));
    my $w = new Vector(2*$l+1);
    $w->[0] = 0;
    foreach (1..$l) {
	$w->[2*$_-1] = 1;
	$w->[2*$_] = -1;
    }
    my @cps = ();
    for (my $m = $m_low; $m <= $m_up; ++$m) {
	push(@cps,($z0 + $m*$w));
    }
    return new Matrix(\@cps);
}


# Produces a point of an infinite series of core points
# 'in multiple directions' with respect to the cyclic group of order 2//l//.
# The point is of the form
# z=e_1+\sum_{j_1}^{2l-2} k_jw_j
# where w_j = e_j-e_{j+2} (compare circ_vec). For odd j, the parameter k_j
# is set to zero. The list of even_params only specifies the parameters k_j
# for even j.
# @param arrayref even_params_ref the parameters k_j for even j
# @return Vector a core point in the infinite series
sub infinite_cp_series_mult_dir {
    my ($even_params_ref) = @_;
    my @even_params = @$even_params_ref;
    my @params = ();
    foreach (@even_params) {
	push(@params,0);
	push(@params,$_);
    }
    return new Vector(1|circ_vec(scalar(@even_params)+1,\@params));
}


# Generates a point of the form 
# z=e_1+m_1*w_1+m_2*w_2
# where w_j = e_j-e_{j+2} (compare circ_vec) and the m_i  
# are dependent of the input parameters //a//, //b//, and //r//. 
# Note that the NOP-graph of z is a path, and the orbit polytopes
# of all points corresponding to (a,b,x) with x<r are nested
# in the orbit polytope of z with respect to C4.
# @param Int a the parameter 'a'
# @param Int b the parameter 'b'
# @param Int r the parameter 'r'
# @return Vector the constructed point z
sub nested_C4 {
    my ($a, $b, $r) = @_;
    my $m1 = (2*$r+1)*$a + $r;
    my $m2 = (2*$r+1)*$b;
    my @params = ($m1, $m2);
    return new Vector(1|circ_vec(2,\@params)); 
}