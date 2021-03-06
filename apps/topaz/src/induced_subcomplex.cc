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
#include "polymake/topaz/complex_tools.h"
#include "polymake/FacetList.h"
#include "polymake/Matrix.h"
#include "polymake/Rational.h"
#include "polymake/hash_map"

namespace polymake { namespace topaz {

perl::Object induced_subcomplex(perl::Object p_in,const Set<int>& V_in, perl::OptionSet options)
{
   const Array< Set<int> > C = p_in.give("FACETS");
   const int n_vert = p_in.give("N_VERTICES");
   
   // checking input
   if (V_in.front()<0 || V_in.back()>n_vert-1)
      throw std::runtime_error("induced_subcomplex: Specified vertices are not contained in VERTICES.");
   
   // computing subcomplex
   FacetList sub(n_vert);
   for (Entire< Array< Set<int> > >::const_iterator it=entire(C);
        !it.at_end(); ++it)
      sub.replaceMax(V_in*(*it));
   
   // adjust numbering
   sub.squeeze();
   
   perl::Object p_out("SimplicialComplex");
   p_out.set_description()<<"Subcomplex of " <<p_in.name() << " induced by the vertices " << V_in<<"."<<endl;

   p_out.take("FACETS") << sub;

   if (!options["no_labels"]) {
      const Array<std::string> L = p_in.give("VERTEX_LABELS");
      Array<std::string> new_L(V_in.size());
      Array<std::string>::iterator l = new_L.begin();
      for (Entire< Set<int> >::const_iterator v=entire(V_in);
           !v.at_end(); ++v, ++l)
         *l = L[*v];
      
      p_out.take("VERTEX_LABELS") << new_L;
   }
   
   if (options["geom_real"]) {
      const Matrix<Rational> Coord = p_in.give("COORDINATES");
      p_out.take("COORDINATES") << Coord.minor(V_in,All);
   }

   return p_out;
}

UserFunction4perl("# @category Producing a new simplicial complex from others"
                  "# Produce the subcomplex consisting of all faces which are contained in the given set of //vertices//." 
                  "# @option Bool no_label tells the client not to create any labels."
                  "# @option Bool geom_real  tells the client to inherit the [[COORDINATES]]."
                  "# @param SimplicialComplex complex"
                  "# @param Set<Int> vertices"
                  "# @return SimplicialComplex",
                  &induced_subcomplex,"induced_subcomplex(SimplicialComplex,$;{ no_labels => 0, geom_real => 0})");
} }

// Local Variables:
// mode:C++
// c-basic-offset:3
// indent-tabs-mode:nil
// End:
