/* Copyright (c) 1997-2014
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
#include "polymake/SparseMatrix.h"
#include "polymake/polytope/CubeFacets.h"
#include "polymake/IncidenceMatrix.h"
#include "polymake/group/group_domain.h"

namespace polymake { namespace polytope {

template <typename Scalar>
perl::Object cube(int d, Scalar x_up, Scalar x_low, perl::OptionSet options)
{
   if (d < 1)
      throw std::runtime_error("cube: dimension d >= 1 required");
   
   if (d > std::numeric_limits<int>::digits-1)
      throw std::runtime_error("cube: in this dimension the number of vertices exceeds the machine int size ");
   
   if (x_up==0 && x_low==0) {
      x_up=1;
   } else {
      if (x_up <= x_low)
         throw std::runtime_error("cube: x_up > x_low required");
      x_low.negate();
   }

   perl::Object p(perl::ObjectType::construct<Scalar>("Polytope"));
   p.set_description() << "cube of dimension " << d << endl;

   SparseMatrix<Scalar> F(2*d,d+1);
   typename Rows< SparseMatrix<Scalar> >::iterator f=rows(F).begin();
   for (int i=1; i<=d; ++i) { // Facet 2*i and Facet 2*i+1 are parallel  
      (*f)[0]=x_low;
      (*f)[i]=1;
      ++f;
      (*f)[0]=x_up;
      (*f)[i]=-1;
      ++f;
   }

   IncidenceMatrix<> VIF(2*d, 1<<d, CubeFacets<int>(d).begin());

   // generate the combinatorial symmetry group on the facets
   bool group_flag = options["group"];
   if ( group_flag ) {
      perl::Object g("group::GroupOfPolytope");
      g.set_description() << "full combinatorial group on facets of " << d << "-dim cube" << endl;
      g.set_name("fullCombinatorialGroupOnFacets");
      g.take("DOMAIN") << polymake::group::OnFacets;
      Array< Array< int > > gens(d);
      if ( d==1 ) {
         Array< int > gen(2);
         gen[0]=1;
         gen[1]=0;
         gens[0]=gen;
      } else {
         Array< int > gen = sequence(0,2*d);
         gen[0]=1;
         gen[1]=0;

         gens[0]=gen;

         // restore gen (=> gen=[0..2d-1])
         gen[0]=0;
         gen[1]=1;

         for ( int i=1; i<d; ++i ) { 
            gen[2*i-2] = 2*i;
            gen[2*i] = 2*i-2;
            gen[2*i-1] = 2*i+1;
            gen[2*i+1] = 2*i-1;
            gens[i]=gen;

            // restore gen (=> gen=[0..2d-1])
            gen[2*i-2] = 2*i-2;
            gen[2*i] = 2*i;
            gen[2*i-1] = 2*i-1;
            gen[2*i+1] = 2*i+1;
         }
      }

      g.take("GENERATORS") << gens;
      p.take("GROUP") << g;

   }

   p.take("CONE_AMBIENT_DIM") << d+1;
   p.take("CONE_DIM") << d+1;
   p.take("FACETS") << F;
   p.take("AFFINE_HULL") << Matrix<Scalar>();
   p.take("VERTICES_IN_FACETS") << VIF;
   p.take("BOUNDED") << true;

   return p;
}

UserFunctionTemplate4perl("# @category Producing a polytope from scratch"
                          "# Produce a //d//-dimensional cube."
                          "# Regular polytope corresponding to the Coxeter group of type B<sub>//d//-1</sub> = C<sub>//d//-1</sub>."
                          "# "
                          "# The bounding hyperplanes are x<sub>i</sub> <= //x_up// and x<sub>i</sub> >= //x_low//."
                          "# @tparam Scalar Coordinate type of the resulting polytope.  Unless specified explicitly, deduced from the type of bound values, defaults to Rational."
                          "# @param Int d the dimension"
                          "# @param Scalar x_up upper bound in each dimension"
                          "# @param Scalar x_low lower bound in each dimension"
                          "# @option Bool group add a symmetry group description to the resulting polytope"
                          "# @return Polytope<Scalar>",
                          "cube<_BoundType, Scalar={ typechecks::is_ordered_field(_BoundType) ? _BoundType : Rational }>(Int; _BoundType=1, _BoundType=(-$_[-1]), { group => undef } )");
} }

// Local Variables:
// mode:C++
// c-basic-offset:3
// indent-tabs-mode:nil
// End: