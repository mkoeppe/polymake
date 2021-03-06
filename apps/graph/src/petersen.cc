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
#include "polymake/Graph.h"

namespace polymake { namespace graph {

perl::Object petersen()
{
   Graph<> g(10);
   for (int i=0; i<5; ++i) {
      g.edge(i,i+5);
      g.edge(i,(i+1)%5);
      g.edge(i+5,(i+2)%5+5);
   }
   perl::Object G("Graph<>");
   G.take("N_NODES")<<10;
   G.take("N_EDGES")<<15;
   G.take("CONNECTED")<<true;
   G.take("BIPARTITE")<<false;
   G.take("ADJACENCY")<<g;
   G.set_description()<<"Petersen graph"<<endl;
   return G;

}
UserFunction4perl("# @category Producing a graph\n"
                  "# Constructs the __Petersen graph__."
                  "# @return Graph",
                  &petersen, "petersen");


} }

// Local Variables:
// mode:C++
// c-basic-offset:3
// indent-tabs-mode:nil
// End:
