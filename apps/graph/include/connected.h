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

#ifndef POLYMAKE_GRAPH_CONNECTED_H
#define POLYMAKE_GRAPH_CONNECTED_H

#include "polymake/graph/BFSiterator.h"
#include "polymake/PowerSet.h"

namespace polymake { namespace graph {

/// Determine whether a graph is connected.
template <typename Graph>
bool is_connected(const GenericGraph<Graph>& G)
{
   if (!G.nodes()) return true;
   BFSiterator<Graph> it(G.top(), nodes(G).front());
   while (!it.at_end()) {
      if (it.unvisited_nodes()==0) return true;
      ++it;
   }
   return false;
}

template <typename Graph>
class connected_components_iterator : protected BFSiterator< Graph, Visitor< BoolNodeVisitor<true> > > {
protected:
   typedef BFSiterator< Graph, Visitor< BoolNodeVisitor<true> > > BFS;
   Set<int> component;

   void fill()
   {
      do {
         component+=this->queue.front();
         BFS::operator++();
      } while (!BFS::at_end());
   }

   void next()
   {
      const int n=this->visitor.get_visited_nodes().front();
      this->queue.push_back(n);
      this->visitor.add(n);
      --this->unvisited;
   }

public:
   typedef std::forward_iterator_tag iterator_category;
   typedef Set<int> value_type;
   typedef value_type reference;
   typedef const value_type* pointer;
   typedef ptrdiff_t difference_type;

   typedef connected_components_iterator iterator;
   typedef iterator const_iterator;

   connected_components_iterator() {}
   connected_components_iterator(const Graph& graph_arg)
      : BFS(graph_arg, graph_arg.nodes() ? nodes(graph_arg).front() : -1)
   {
      if (this->unvisited>=0) fill();
   }

   reference operator* () const { return component; }
   pointer operator-> () const { return &component; }

   iterator& operator++ ()
   {
      component.clear();
      if (BFS::unvisited>0) { next(); fill(); }
      return *this;
   }
   const iterator operator++ (int) { iterator copy(*this);  operator++();  return copy; }

   bool operator== (const iterator& it) const { return component==it.component; }
   bool operator!= (const iterator& it) const { return !operator==(it); }

   bool at_end() const { return component.empty(); }

   void rewind()
   {
      if (this->graph->nodes()) {
         BFS::reset(nodes(*this->graph).front());
         component.clear();
         fill();
      }
   }
};

/// Compute the connected components
template <typename Graph> inline
pm::GraphComponents<const Graph&, connected_components_iterator>
connected_components(const GenericGraph<Graph>& G) { return G.top(); }

namespace {

template<typename Container, typename E>
void add_to(Container& C, const E& e) 
{
   C.push_back(e);
}

template<typename E>
void add_to(Set<E>& C, const E& e)
{
   C += e;
}

} // end anonymous namespace

// compute a spanning tree rooted at vertex root_node,
// optionally restricted to a support set
template<typename GraphType, typename DirType, typename Container>
void
connected_component(const GenericGraph<GraphType, DirType>& G, 
                    Container& C,
                    int root_node=0,
                    const Set<int> support=Set<int>())
{
   std::list<int> unprocessed_leaves;
   Bitset marked;  // nodes already included in the tree

   unprocessed_leaves.push_back(root_node); // we start the tree at the given node
   add_to(C, root_node);
   marked.insert(root_node);

   while ( !unprocessed_leaves.empty() ) {      
      const int current = unprocessed_leaves.front();
      unprocessed_leaves.pop_front();
      const Set<int> neighbours = G.top().adjacent_nodes(current);  
      for (Entire< Set< int > >::const_iterator v = entire(neighbours); !v.at_end(); ++v) 
         if ((!support.size() || support.contains(*v)) 
             && !marked.contains(*v)) {
            unprocessed_leaves.push_back(*v);
            marked.insert(*v);
            add_to(C, *v);
         }
   }
}


} }

namespace pm {

template <typename Graph>
struct check_iterator_feature< polymake::graph::connected_components_iterator<Graph>, end_sensitive > : True {};

template <typename Graph>
struct check_iterator_feature< polymake::graph::connected_components_iterator<Graph>, rewindable > : True {};

template <typename GraphRef>
class generic_of_GraphComponents<GraphRef, polymake::graph::connected_components_iterator>
   : public GenericSet< GraphComponents<GraphRef,polymake::graph::connected_components_iterator>, Set<int>, operations::cmp > {};
}

#endif // POLYMAKE_GRAPH_CONNECTED_H

// Local Variables:
// mode:C++
// c-basic-offset:3
// indent-tabs-mode:nil
// End:
