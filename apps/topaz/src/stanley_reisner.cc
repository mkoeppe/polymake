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
#include "polymake/Array.h"
#include "polymake/Set.h"
#include "polymake/Vector.h"
#include "polymake/Polynomial.h"
#include "polymake/Rational.h"


namespace polymake { namespace topaz {

perl::Object stanley_reisner(perl::Object C, perl::OptionSet options) {
  const Array< Set<int> > non_faces=C.give("MINIMAL_NON_FACES");
  const int n_non_faces=non_faces.size();
  const int n_vertices=C.give("N_VERTICES");

  Ring<Rational,int> R;
  if ((options["ring"] >> R)) {
     if (R.n_vars()!=n_vertices)
        throw std::runtime_error("stanley_reisner: number of variables do not match");
  } else {
     Ring<Rational,int> newR(n_vertices);
     R=newR;
  }
  Array< Polynomial<Rational,int> > gens(n_non_faces);

  for (int k=0; k<n_non_faces; ++k) {
     Monomial<Rational,int> M(same_element_sparse_vector(non_faces[k],1,n_vertices),R);
     gens[k]=M;
  }

  perl::Object I("ideal::Ideal");
  I.take("GENERATORS") << gens;
  I.take("MONOMIAL") << true;
  I.take("RING") << R;
  I.set_description() << "Stanley-Reisner ideal of " << C.name();

  return I;
}

UserFunction4perl("# @category Other"
                  "# Creates the __Stanley-Reisner ideal__ of a simplicial complex."
                  "# Optional //ring// parameter is required to have precisely as many variables as //C// has vertices."
                  "# @param  SimplicialComplex complex"
                  "# @option Ring<Rational,int> ring"
                  "# @return ideal::Ideal",
                  &stanley_reisner, "stanley_reisner(SimplicialComplex { ring => undef })");
} }

// Local Variables:
// mode:C++
// c-basic-offset:3
// indent-tabs-mode:nil
// End:
