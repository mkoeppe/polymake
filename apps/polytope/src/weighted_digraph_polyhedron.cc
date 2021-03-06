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
#include "polymake/SparseVector.h"
#include "polymake/SparseMatrix.h"
#include "polymake/GenericMatrix.h"
#include "polymake/ListMatrix.h"

namespace polymake { namespace polytope {

template <typename Scalar, typename Matrix>
perl::Object weighted_digraph_polyhedron(const GenericMatrix<Matrix, Scalar>& W)
{
   const int k(W.rows());
   if (W.cols() != k)
         throw std::runtime_error("weighted_digraph_polyhedron: non-square matrix");

   ListMatrix< SparseVector<Scalar> > Ineq;
   for (int i=0; i<k; ++i)
      for (int j=0; j<k; ++j) {
         if (isfinite(W.top()(i,j)) && (i!=j || W.top()(i,j) != 0)) {
            SparseVector<Scalar> this_ineq(k+1); // initialized as zero vector
            this_ineq[0]=W.top()(i,j);
            if (i!=j) { this_ineq[i+1]=-1; this_ineq[j+1]=1; }
            Ineq /= this_ineq;
         }
      }
  
   perl::Object p_out(perl::ObjectType::construct<Scalar>("polytope::Polytope"));
   p_out.set_description() << "Weighted digraph polyhedron" <<endl;
   p_out.take("INEQUALITIES") << Ineq;
   return p_out;
}

UserFunctionTemplate4perl("# @category Producing a polytope from graphs"
                          "# Weighted digraph polyhedron of a directed graph with a weight function."
                          "# The graph and the weight function are combined into a matrix."
                          "# @param Matrix encoding weighted digraph"
                          "# @return polytope::Polytope",
                          "weighted_digraph_polyhedron<Scalar>(Matrix<Scalar,_>)");
} }

// Local Variables:
// mode:C++
// c-basic-offset:3
// indent-tabs-mode:nil
// End:
