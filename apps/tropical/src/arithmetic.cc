/* Copyright (c) 1997-2015
   Ewgenij Gawrilow, Michael Joswig (Technische Universitaet Berlin, Germany)
   http://www.polymake.org

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version: http://www.gnu.org/licenses/gpl.txt.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
--------------------------------------------------------------------------------
*/

#include "polymake/client.h"
#include "polymake/Matrix.h"
#include "polymake/Array.h"
#include "polymake/tropical/arithmetic.h"

namespace polymake { namespace tropical {


  
UserFunctionTemplate4perl("# @category Tropical operations"
                          "# The __tropical determinant__ of a matrix."
                          "# See also [tdet_and_perm] and [second_tdet_and_perm]."
                          "# @param Matrix<TropicalNumber<Addition,Scalar> > matrix"
                          "# @return TropicalNumber<Addition,Scalar>"
                          "# @example > print tdet(new Matrix<TropicalNumber<Max>>([[1,0,0],[0,1,0],[0,0,1]]));"
                          "# | 3" ,
                          "tdet(Matrix)");

UserFunctionTemplate4perl("# @category Tropical operations"
                          "# The __tropical determinant__ of a matrix and one optimal permutation."
                          "# @param Matrix< TropicalNumber<Addition,Scalar> > matrix"
                          "# @return Pair< TropicalNumber<Addition,Scalar>, Array<Int> >"
                          "# @example > print tdet_and_perm(new Matrix<TropicalNumber<Min>>([[1,0,0],[0,1,0],[0,0,1]]));"
                          "# | 0 <2 0 1>" ,
                          "tdet_and_perm(Matrix)");
      
UserFunctionTemplate4perl("# @category Tropical operations"
                          "# The __second tropical optimum__ of a matrix and one corresponding permutation."
                          "# @param Matrix< TropicalNumber<Addition,Scalar> > matrix"
                          "# @return Pair< TropicalNumber<Addition,Scalar>, Array<Int> >"
                          "# @example > print second_tdet_and_perm(new Matrix<TropicalNumber<Min>>([[1,0,0],[0,1,0],[0,0,1]]));"
                          "# | 0 <1 2 0>" ,
                          "second_tdet_and_perm(Matrix)");      
      
UserFunctionTemplate4perl("# @category Tropical operations"
                          "# The __solution vector__ of an unsigned tropical matrix equation."
                          "# For more details and background see"
                          "Akian, Gaubert & Guterman: Tropical Cramer determinants revisited."
                          "Tropical and idempotent mathematics and applications, Contemp. Math., 616, AMS, 2014"
                          "Preprint http://arxiv.org/abs/1309.6298"
                          "# @param Matrix<TropicalNumber<Addition,Scalar> > matrix"
                          "# @return Vector<TropicalNumber<Addition,Scalar> >" ,
                          "cramer(Matrix)");

UserFunctionTemplate4perl("# @category Tropical operations"
                          "# Tropical distance function.  This is a metric on the tropical projective torus."
                          "# The same for Min and Max."
                          "# @param Vector<TropicalNumber<Addition,Scalar> > v"
                          "# @param Vector<TropicalNumber<Addition,Scalar> > w"
                          "# @return Scalar"
                          "# @example > $v=new Vector<TropicalNumber<Min>>([1,0]);"
                          "# > $w=new Vector<TropicalNumber<Min>>([0,1]);"
                          "# > print tdist($v,$w);"
                          "# | 2" ,
                          "tdist(Vector,Vector)");

UserFunctionTemplate4perl("# @category Tropical operations"
                          "# Tropical diameter of a simplex, defined by the columns of a matrix.  This is the maximum over the pairwise tropical distances."
                          "# The same for Min and Max."
                          "# @param Matrix<TropicalNumber<Addition,Scalar> > matrix"
                          "# @return Scalar" 
                          "# @example > print tdiam(new Matrix<TropicalNumber<Max>>([[1,0,0],[0,1,0],[0,0,1]]));"
                          "# | 2" ,
                          "tdiam(Matrix)");


} }

// Local Variables:
// mode:C++
// c-basic-offset:3
// indent-tabs-mode:nil
// End: