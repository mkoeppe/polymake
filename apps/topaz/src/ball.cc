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
#include "polymake/SparseMatrix.h"
#include "polymake/SparseVector.h"
#include "polymake/Array.h"
#include "polymake/Set.h"
#include "polymake/Rational.h"

namespace polymake { namespace topaz {
  
perl::Object ball(const int d)
{
   perl::Object p("GeometricSimplicialComplex<Rational>");
   p.set_description() << "The " << d << "-dimensional ball.\n"
                          "Realized as the " << d << "-simplex.\n";

   Array< Set<int> > F(1);
   F[0]=range(0,d);

   p.take("FACETS") << F;
   p.take("DIM") << d;
   p.take("PURE") << true;
   p.take("MANIFOLD") << true;
   p.take("BALL") << true;
   p.take("ORIENTED_PSEUDO_MANIFOLD") << true;
   if (d>0)
      p.take("COORDINATES") << zero_vector<Rational>(d) / unit_matrix<Rational>(d);
   return p;
}
   
UserFunction4perl("# @category Producing from scratch\n"
                  "# A //d//-dimensional __ball__, realized as the //d//-simplex.\n"
                  "# @param Int d dimension"
                  "# @return GeometricSimplicialComplex",
                  &ball, "ball($)");
} }

// Local Variables:
// mode:C++
// c-basic-offset:3
// indent-tabs-mode:nil
// End:
