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
#include "polymake/Rational.h"
#include "polymake/Matrix.h"
#include "polymake/polytope/transform.h"

namespace polymake { namespace polytope {

template <typename Scalar>
perl::Object facet_to_infinity(perl::Object p_in, const int inf_facet)
{

  perl::Object p_out(perl::ObjectType::construct<Scalar>("Polytope"));

  const Matrix<Scalar> facets=p_in.give("FACETS");

  const Vector<Scalar> inf_f= facets.row(inf_facet);
  const Matrix<Scalar> tau=null_space(inf_f);
  transform_section(p_out, p_in, "FACETS | INEQUALITIES", T(inf_f/tau));
  transform_section(p_out, p_in, "AFFINE_HULL | EQUATIONS", T(inf_f/tau));
  transform_section(p_out, p_in, "VERTICES", inv(inf_f/tau));
  transform_section(p_out, p_in, "LINEALITY_SPACE", inv(inf_f/tau));
  
  p_out.take("BOUNDED") << false;

  return p_out;
}

  UserFunctionTemplate4perl("# @category Producing a polytope from polytopes"
                            "# Make an affine transformation such that the i-th facet is transformed to infinity"
                            "# @param Polytope P"
                            "# @param Int i the facet index"
                            "# @return Polytope"
                            "# @author Sven Herrmann"
                            "# @example This generates the polytope that is the positive quadrant in 2-space:"
                            "# > $p = new Polytope(VERTICES=>[[1,-1,-1],[1,0,1],[1,1,0]]);"
                            "# > $pf = facet_to_infinity($q,2);"
                            "# > print $pf->VERTICES;"
                            "# | 1 0 0"
                            "# | 0 0 1"
                            "# | 0 1 0",
                            "facet_to_infinity<Scalar>(Polytope<Scalar> $)");
} }

// Local Variables:
// mode:C++
// c-basic-offset:3
// indent-tabs-mode:nil
// End:
