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
#include "polymake/Set.h"
#include "polymake/Array.h"
#include "polymake/Matrix.h"
#include "polymake/TropicalNumber.h"
#include "polymake/tropical/arithmetic.h"

namespace polymake { namespace tropical {
  
	//FIXME Adapt to tropical numbers. -- only finite coordinates
      
      template <typename Addition, typename Scalar>
      Vector<TropicalNumber<Addition, Scalar> > nearest_point(perl::Object t_in, const Vector<TropicalNumber<Addition, Scalar> >& point)
   {
      typedef TropicalNumber<Addition, Scalar> TNumber;
      Matrix<TNumber> V = t_in.give("POINTS");
      //Matrix<TNumber> V = t_in.give("VERTICES");
      
      Vector<TNumber> lambda = principal_solution(T(V), point);

      return T(V) * lambda;

      //should this output be homogenized?

   } 
      UserFunctionTemplate4perl("# @category Tropical operations"
                             "# Compute the projection of a point //x// in  tropical projective space onto a tropical cone //C//."
                             "# Cf."
                             "# \t Develin & Sturmfels math.MG/0308254v2, Proposition 9."
                             "# @param Cone<Addition,Scalar> C"
                             "# @param Vector<TropicalNumber<Addition,Scalar> > x"
                             "# @return Vector<TropicalNumber<Addition,Scalar> >"
                             "# @author Katja Kulas",
                             "nearest_point<Addition, Scalar>(Cone<Addition, Scalar>, Vector<TropicalNumber<Addition, Scalar> >)"); 


      UserFunctionTemplate4perl("# @category Tropical operations"
                                "# Compute the solution of the tropical equation //A// * //x// = //b//."
                                "# If there is no solution, the return value is 'near' a solution."
                                "# Cf. "
                                "# Butkovič 'Max-linear systems: theory and algorithms' (MR2681232), "
                                "# Theorem 3.1.1"
                                "# @param Matrix<TropicalNumber<Addition, Scalar> > A"
                                "# @param Vector<TropicalNumber<Addition, Scalar> > b"
                                "# @return Vector<TropicalNumber<Addition, Scalar> > ",
                                "principal_solution(Matrix, Vector)"); 

      FunctionTemplate4perl("rel_coord(Vector, Vector)");
} }

// Local Variables:
// mode:C++
// c-basic-offset:3
// indent-tabs-mode:nil
// End:
