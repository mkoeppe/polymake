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
#include "polymake/Set.h"
#include "polymake/PowerSet.h"
#include "polymake/SparseMatrix.h"
#include "polymake/IncidenceMatrix.h"
#include "polymake/Graph.h"
#include "polymake/Bitset.h"
#include "polymake/graph/connected.h"
#include "polymake/vector"
#include "polymake/map"
#include "polymake/list"

namespace polymake { namespace fan {

namespace {

class Tubing {
protected:
   Graph<Directed> spine; // a directed rooted spanning forest
   int root;

public:
   Tubing() {}

   Tubing(const Graph<Undirected>& G) // produce from scratch
      : spine(G.nodes())
      , root(0) 
   {
      std::vector<int> nodes;
      nodes.reserve(G.nodes());
      graph::connected_component(G, nodes);
      for (unsigned int i=0; i<nodes.size()-1; ++i)
         spine.edge(nodes[i], nodes[i+1]);
      root = nodes[G.nodes()-1];
   }

   Tubing(const Graph<Undirected>& G, const Graph<Directed>& _spine) // produce from scratch
      : spine(_spine)
      , root(0) 
   {
      for (int i=0; i<spine.nodes(); ++i)
         if (spine.out_degree(i) == 0) {
            root = i;
            break;
         }
   }

   // we name a tube (a principal order ideal in the tubing) by its generator
   Tubing(const Graph<Undirected>& G, const Tubing& old, int t)  // produce via flip in t
      : spine(old.spine.nodes())
      , root(old.root)
   {
      const Set<int> out_nodes(old.spine.out_adjacent_nodes(t));
      if (!out_nodes.size()) {
         cerr << "attempting to flip tube generated by " << t << endl;
         throw std::runtime_error("cannot flip the tube that contains the entire component.");
      }
      const int t_bar = out_nodes.front();
      const Set<int> new_tube_support(old.downset_of_tubing_from(t_bar) - scalar2set(t));
      Set<int> cc;
      graph::connected_component(G, cc, t_bar, new_tube_support);
      
      // now flip in two steps.
      // (1) all edges from new_tube to t are bent to t_bar
      // (2) flip t -> t_bar  to  t_bar -> t
      // (3) flip t_bar -> parent  to  t -> parent 
      // All other edges are left intact
      for (Entire<Edges<Graph<Directed> > >::const_iterator eit = entire(edges(old.spine)); !eit.at_end(); ++eit) {
         const int a(eit.from_node()), b(eit.to_node());
         if (b == t) {
            if (cc.contains(a)) {
               spine.edge(a, t_bar);
            } else {
               spine.edge(a, b);
            } 
         } else if (a == t && b == t_bar) {
            spine.edge(t_bar, t);
         } else if (a == t_bar) {
            spine.edge(t, b);
         } else {
            spine.edge(a, b);
         }
      }
      if (t_bar == root) root = t;
   }

   Set<int> downset_of_tubing_from(int t) const {
      Set<int> support;
      std::list<int> queue;
      queue.push_back(t);
      while (queue.size()) {
         const int n = queue.front(); queue.pop_front();
         support += n;
         for (Entire<Graph<Directed>::in_adjacent_node_list>::const_iterator e=entire(spine.in_adjacent_nodes(n)); !e.at_end(); ++e) {
            queue.push_back(*e);
         }
      }
      return support;
   }

   SparseMatrix<Rational> inequalities() const {
      SparseMatrix<Rational> inequalities(spine.edges(), spine.nodes());
      Entire<Rows<SparseMatrix<Rational> > >::iterator rit = entire(rows(inequalities));
      for (Entire<Edges<Graph<Directed> > >::const_iterator eit = entire(edges(spine)); !eit.at_end(); ++eit, ++rit) {
         (*rit)[eit.from_node()] = 1;
         (*rit)[eit.to_node()] = -1;
      }
      return inequalities;
   }

   Matrix<Rational> rays() const {
      const int n(spine.nodes());
      Matrix<Rational> rays(n-1, n);
      Entire<Rows<Matrix<Rational> > >::iterator rit = entire(rows(rays));
      for (int i=0; i<n; ++i) {
         if (i==root) continue;
         const Set<int> support(downset_of_tubing_from(i));
         for (Entire<Set<int> >::const_iterator sit = entire(support); !sit.at_end(); ++sit)
            (*rit)[*sit] = 1;
         ++rit;
      }
      return rays;
   }

protected:

   void representation_impl(std::ostream& os, int n) const {
      os << "(" << n;
      for (Entire<Graph<Directed>::in_edge_list>::const_iterator e=entire(spine.in_edges(n)); !e.at_end(); ++e)
         representation_impl(os, e.from_node());
      os << ")";
   }

public:

   std::string representation() const {
      std::stringstream ss;
      representation_impl(ss, root);
      return ss.str();
   }

   friend
   std::ostream& operator<< (std::ostream& os, const Tubing& T) {
      return os << T.representation();
   }

   const Graph<Directed>& get_spine() const { return spine; }
   int get_root() const { return root; }

   Set<Set<int> > tubes() const {
      const int n(spine.nodes());
      Set<Set<int> > tubes;
      for (int i=0; i<n; ++i) {
         if (i==root) continue;
         tubes += downset_of_tubing_from(i);
      }
      return tubes;
   }
};

void process_tubing(const Tubing& T, 
                    std::map<Vector<Rational>, int>& index_of_ray,
                    int& max_ray_index,
                    std::vector<Set<int> >& VIF_list,
                    std::vector<std::string>& cone_label_list)
{
   Set<int> VIF;
   for (Entire<Rows<Matrix<Rational> > >::const_iterator rit = entire(rows(T.rays())); !rit.at_end(); ++rit) {
      const int index = (index_of_ray.find(*rit) == index_of_ray.end())
         ? max_ray_index++
         : index_of_ray[*rit];
      index_of_ray[*rit] = index;
      VIF += index;
   }
   VIF_list.push_back(VIF);
   cone_label_list.push_back(T.representation());
}

} // end anonymous namespace

perl::Object graph_associahedron_fan(const perl::Object& g)
{
   const Graph<Undirected> G = g.give("ADJACENCY");
   Set<std::string> seen;
   std::list<int> tubing_queue;
   std::vector<Tubing> tubing_list;
   std::map<std::string, int> index_of_tubing;
   int max_tubing_index(0);

   std::map<Vector<Rational>, int> index_of_ray;
   int max_ray_index(0);

   std::vector<Set<int> > VIF_list;
   std::vector<std::string> cone_label_list;
   std::vector<std::pair<int,int> > dual_graph_edges;
   const int n(G.nodes());

   tubing_list.push_back(Tubing(G));
   tubing_queue.push_back(max_tubing_index++);
   seen += tubing_list[0].representation();
   index_of_tubing[seen.front()] = 0;
   process_tubing(tubing_list[0], index_of_ray, max_ray_index, VIF_list, cone_label_list);
   
   while(tubing_queue.size()) {
      const int Tind = tubing_queue.front(); tubing_queue.pop_front();
      for (int i=0; i<n; ++i) {
         if (i == tubing_list[Tind].get_root()) continue;
         const Tubing flipped(G, tubing_list[Tind], i);
         const std::string flipped_rep(flipped.representation());
         if (seen.contains(flipped_rep)) {
            dual_graph_edges.push_back(std::make_pair(Tind, index_of_tubing[flipped_rep]));
            continue;
         }
         seen += flipped_rep;
         tubing_list.push_back(flipped);
         tubing_queue.push_back(max_tubing_index);
         index_of_tubing[flipped_rep] = max_tubing_index;
         process_tubing(flipped, index_of_ray, max_ray_index, VIF_list, cone_label_list);
         dual_graph_edges.push_back(std::make_pair(Tind, max_tubing_index));
         max_tubing_index++;
      }
   }
   const IncidenceMatrix<> VIF(VIF_list.size(), max_ray_index, entire(VIF_list));
   Matrix<Rational> rays(max_ray_index, n);
   for (std::map<Vector<Rational>, int>::const_iterator mit = index_of_ray.begin(), mend = index_of_ray.end(); mit != mend; ++mit)
      rays[mit->second] = mit->first;

   Graph<Undirected> dual_graph(VIF_list.size());
   for (Entire<std::vector<std::pair<int,int> > >::const_iterator pit = entire(dual_graph_edges); !pit.at_end(); ++pit)
      dual_graph.edge(pit->first, pit->second);

   perl::Object fan("PolyhedralFan");
   fan.take("RAYS") << rays;
   fan.take("MAXIMAL_CONES") << VIF;
   fan.take("MAXIMAL_CONE_LABELS") << cone_label_list;
   fan.take("LINEALITY_SPACE") << ones_matrix<Rational>(1, n);
   fan.take("FAN_AMBIENT_DIM") << n;
   fan.take("FAN_DIM") << n;
   fan.take("FULL_DIM") << true;
   fan.take("POINTED") << false;
   fan.take("COMPLETE") << true;
   fan.take("PURE") << true;
   fan.take("SIMPLICIAL") << true;
   fan.take("DUAL_GRAPH.ADJACENCY") << dual_graph;
   return fan;
}

perl::Object flip_tube(const perl::Object& g, const perl::Object& spine, int t)
{
   const Graph<Undirected> G = g.give("ADJACENCY");
   const Graph<Directed> Spine = spine.give("ADJACENCY");
   const Tubing T(G, Spine);
   const Tubing T1(G, T, t);
   perl::Object new_spine("Graph<Directed>");
   new_spine.take("ADJACENCY") << T1.get_spine();
   return new_spine;
}

perl::Object cone_of_tubing(const perl::Object& g, const perl::Object& spine)
{
   const Graph<Undirected> G = g.give("ADJACENCY");
   const Graph<Directed> Spine = spine.give("ADJACENCY");
   const Tubing T(G, Spine);
   perl::Object cone("Cone<Rational>");
   cone.take("RAYS") << T.rays();
   cone.take("INEQUALITIES") << T.inequalities();
   cone.take("LINEALITY_SPACE") << ones_matrix<Rational>(1, G.nodes());
   return cone;
}

Set<Set<int> > tubes_of_tubing(const perl::Object& g, const perl::Object& spine)
{
   const Graph<Undirected> G = g.give("ADJACENCY");
   const Graph<Directed> Spine = spine.give("ADJACENCY");
   const Tubing T(G, Spine);
   return T.tubes();
}

// a tube is a non-empty subset of the vertices that induces a connected graph
PowerSet<int> tubes_of_graph(const perl::Object& g)
{
   const Graph<Undirected> G = g.give("ADJACENCY");
   const int n(G.nodes());
   PowerSet<int> tubes;
   for (int i=0; i<n; ++i)
      tubes += scalar2set(i);

   for (int k=2; k <= n; ++k) {
      for (Entire<Subsets_of_k<const sequence&> >::const_iterator p=entire(all_subsets_of_k(sequence(0,n), k)); !p.at_end(); ++p) {
         const Set<int> support(*p);
         Set<int> cc;
         graph::connected_component(G, cc, support.front(), support);
         if (cc.size() == support.size())
            tubes += support;
      }
   }
   return tubes;
}

// a tube is a non-empty subset of the vertices that induces a connected graph
Set<Set<int> > tubing_of_graph(const perl::Object& g)
{
   const Graph<Undirected> G = g.give("ADJACENCY");
   const Tubing T(G);
   return T.tubes();
}


UserFunction4perl("# @category Producing a fan"
                  "# Produce the dual fan of a graph associahedron."
                  "# @param Graph G the input graph"
                  "# @return PolyhedralFan",
                  &graph_associahedron_fan, "graph_associahedron_fan(Graph)");

UserFunction4perl("# @category Other"
                  "# Flip a tubing in a tube"
                  "# @param Graph G the input graph"
                  "# @param Graph T the input tubing"
                  "# @param Int t the tube to flip, identified by its root"
                  "# @return Graph",
                  &flip_tube, "flip_tube(Graph,Graph,$)");

UserFunction4perl("# @category Other"
                  "# Output the cone of a tubing"
                  "# @param Graph G the input graph"
                  "# @param Graph T the input tubing"
                  "# @return Cone",
                  &cone_of_tubing, "cone_of_tubing(Graph,Graph)");

UserFunction4perl("# @category Other"
                  "# Output the tubes of a tubing"
                  "# @param Graph G the input graph"
                  "# @param Graph T the input tubing"
                  "# @return Set<Set>",
                  &tubes_of_tubing, "tubes_of_tubing(Graph,Graph)");

UserFunction4perl("# @category Other"
                  "# Output the set of all tubes of a graph"
                  "# @param Graph G the input graph"
                  "# @return Set<Set>",
                  &tubes_of_graph, "tubes_of_graph(Graph)");

UserFunction4perl("# @category Other"
                  "# Output one tubing of a graph"
                  "# @param Graph G the input graph"
                  "# @return Set<Set>",
                  &tubing_of_graph, "tubing_of_graph(Graph)");

} }

// Local Variables:
// mode:C++
// c-basic-offset:3
// indent-tabs-mode:nil
// End:
