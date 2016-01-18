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
#include "polymake/Matrix.h"
#include "polymake/Integer.h"
#include "polymake/Array.h"
#include "polymake/IncidenceMatrix.h"
#include "polymake/Bitset.h"
#include "polymake/hash_map"
#include "polymake/vector"
#include "polymake/pair.h"
#include "polymake/numerical_functions.h"
#include <algorithm>

namespace polymake { namespace polytope {
  
namespace {

//calculates the legal edges, i.e. the edges containing no other points in their interior
template <typename Scalar>
IncidenceMatrix<> calc_illegal_edges(const Array< Vector<Scalar> >& points) 
{
   const int n = points.size();
   IncidenceMatrix<> illegal_edges(n,n);
   typedef typename pm::algebraic_traits<Scalar>::field_type field;    
   for (int i=0; i<n; ++i)
      for (int j=i+2; j<n; ++j)
         for (int k=i+1; k<j; ++k) {
            field x_ik = points[k][1]-points[i][1];
            field x_ij = points[j][1]-points[i][1];
            field y_ik = points[k][2]-points[i][2];
            field y_ij = points[j][2]-points[i][2];
            if (((0==x_ik) && (0==x_ij)) || ((0!=x_ik) && (0!=x_ij) && y_ik/x_ik==y_ij/x_ij))
               illegal_edges(i,j) = 1;
         }
   return illegal_edges;
}
  
//container class for precalculated pieces of information about an edge
struct EdgeData{
   Bitset adv1, adv2, vis;
   bool monotony;
   bool trivial;
   EdgeData() : trivial(true) {}
   EdgeData(const Bitset& a1, const Bitset& a2, const Bitset& v, const bool& m) : adv1(a1), adv2(a2), vis(v), monotony(m), trivial(false) {}
};

//calculates the following informations for all edges: possible advances, upper visible points to the right of the left endpoint, monotony
template <typename Scalar> 
Array<Array<EdgeData > > edge_precalc(const Array< Vector<Scalar> > points) 
{
   typedef typename pm::algebraic_traits<Scalar>::field_type field;   
   const int n = points.size();    
   Array<Array<EdgeData > > edges(n-2);
   for (int i=0; i<n-2; i++) {
      Array<EdgeData > tmp(n-i-1);
      edges[i] = tmp;
   }
   Matrix<field> V(3,3);
   bool candidate = true;
   perl::Object::Schedule s;
   const IncidenceMatrix<>& illegal_edges = calc_illegal_edges<Scalar>(points);
   for (int i=0; i<n-2; ++i) {
      for (int j=i+1; j<n; ++j) {
         if (0==illegal_edges(i,j)) {
            Bitset adv_type1(j-i-1);
            Bitset adv_type2(j-i-1); 
            Bitset vis_from_above(n-i-2);
            for (int k=i+1; k<n; ++k) {
               if (k!=j && (0==illegal_edges(i,k)) && (0==illegal_edges(j,k))) {
                  perl::Object p = perl::Object(perl::ObjectType::construct<field>("Polytope")); 
                  V.row(0) = points[i];
                  V.row(1) = points[k];
                  V.row(2) = points[j];
                  p.take("VERTICES") << V;
                  if (!s.valid()) {
                     p.take("FEASIBLE") << true;
                     p.take("BOUNDED") << true;
                     s = p.CallPolymakeMethod("get_schedule","FACETS","VERTICES_IN_FACETS");
                  }
                  s.apply(p);  

                  const Matrix<field> facets = p.give("FACETS");
                  const IncidenceMatrix<> VIF = p.give("VERTICES_IN_FACETS");
                  
                  //for each point in the segment wrt. the x-coordinate check if the corresponding triangle constitutes an advance triangle
                  if (k < j) {
                     candidate = true;
                     //the triangle is an advance triangle iff there is no other point of the input in its interior 
                     for (int l= i+1; l<j; ++l) {
                        if (l!=k) {
                           Vector<field> eval = facets*points[l];
                           if (eval[0]>0 && eval[1]>0 && eval[2]>0)
                              candidate = false;
                        }
                     }
                  }
                  //calculate the index of the facet which contains the points i and j
                  int index = 0;
                  for (int l=0; l<3; ++l) { 
                     if (VIF.exists(l,0) && VIF.exists(l,2))
                        index = l;
                  }
                  //check if the additional point satifies the visibility condition and determine the advance type in the case of constituting an advance triangle 
                  if (facets(index,2)>0) {
                     if (k<j) {
                        if (candidate) 
                           adv_type1 += k-i-1;
                        vis_from_above += k-i-1;
                     }
                     else
                        vis_from_above += k-i-2;
                  }
                  else 
                     if (candidate && (k<j)) 
                        adv_type2 += k-i-1;
               }
            }
            EdgeData ed(adv_type1,adv_type2,vis_from_above, Rational(points[j][2]-points[i][2]) < 0 ? true : false);
            edges[i][j-i-1] = ed;
         }
      }
   }
   return edges;
}

//calculates the lower and the upper boundary for the input polytope (including all such points specified in POINTS)
template <typename Scalar> 
int lower_upper_boundary(Bitset& lower, Bitset& upper, perl::Object p)
{
   typedef typename pm::algebraic_traits<Scalar>::field_type field;
   const Matrix<Scalar>& P = p.give("POINTS");
   const int n = P.rows()-1;
   Set<int> boundary_points;
   const IncidenceMatrix<> PIF = p.give("POINTS_IN_FACETS");
   const Matrix<field> F = p.give("FACETS");
   
   upper+= n;
   for (Entire<Rows<IncidenceMatrix<> > >::const_iterator rit = entire(rows(PIF)); !rit.at_end(); ++rit) {
      const Set<int>& ps((*rit));	
      for (Entire<Set<int> >::const_iterator sit = entire(ps); !sit.at_end(); ++sit) {
         boundary_points+= *sit;
         if (F(rit.index(),2)>0 || P(n,1)==P(*sit,1))
            lower+= *sit;
         else
            upper+= *sit;
      }
   }
   return boundary_points.size();
}

}// end anonymous namespace

//calculate the number of triangulations in the input points
template <typename Scalar>
Integer n_triangulations(const Matrix<Scalar>& points, perl::OptionSet options)
{
   const bool opt(options["optimization"]);    
   const int n = points.rows();

   //throw exception if n < 3
   if (n < 3)
      throw std::runtime_error("insufficient number of points");

   Array< Vector<Scalar> > ordered_points(rows(points));
   std::sort(ordered_points.begin(),ordered_points.end());
   
   //precalculations for the edges
   Array<Array<EdgeData > > advance_triangles = edge_precalc<Scalar>(ordered_points);
   perl::Object p = perl::Object(perl::ObjectType::construct<Scalar>("Polytope"));
   Matrix<Scalar> ordered_points_matrix(n,points.cols(),entire(ordered_points));
   
   p.take("FEASIBLE") << true;
   p.take("BOUNDED") << true;
   p.take("POINTS") << ordered_points_matrix;

   //calculate the upper boundary and the source for an DAG
   const int size = n+log2_ceil(n);
   Bitset upper_boundary(n);
   Bitset source(size);
   source+= size-1;
   const int n_boundary_points = lower_upper_boundary<Scalar>(source,upper_boundary,p);

   hash_map<Bitset, Integer> unmap1;
   hash_map<Bitset, Integer> unmap2;
   hash_map<Bitset, Integer> *current_lvl = &unmap1, *next_lvl = &unmap2;
   Integer counter(1);
   unmap1[source] = counter;

   //while keeping at most two hashtables for consecutive levels in memory, calculate the number of triangulations for each level one after another
   for (int i=0; i<2*n-n_boundary_points-2; ++i) {	
      for (Entire<hash_map<Bitset, Integer> >::iterator chain = (*current_lvl).begin(); chain != (*current_lvl).end(); chain = (*current_lvl).erase(chain)) {
         Bitset current_mchain = (*chain).first;
         Bitset on_or_above(n);
         if (opt) {
            //calculate all points on or above the current chain and relate them to their visible edges
            on_or_above += 0;
            Entire<Bitset>::const_iterator right_end = current_mchain.begin();
            right_end++;
            for (int j=0; j!=n-1; j = *right_end, right_end++) {
               on_or_above += *right_end;
               for (int l=j+1; l < *right_end; l++)
                  if (advance_triangles[j][*right_end-j-1].vis.contains(l-j-1))
                     on_or_above += l;
            }
         }
         //calculate the marking from the corresponding bitset
         int mark(0); 
         Entire<Bitset>::const_iterator leading_one = current_mchain.begin();
         while (*leading_one < n) 
            leading_one++;
         for (int j = size-1; j >= *leading_one; --j) {
            if (current_mchain.contains(j)) {
               mark+= pow(2,size-j-1);
            }
         }
         int left(0), mid(0), right(0);
         //calculate the start nodes according to the mark of the chain
         Entire<Bitset>::const_iterator unmarked = current_mchain.begin();
         for (int j=0; j+2<mark; ++j) {
            unmarked++;
         }
         mid = *unmarked;

         if (mark > 1) {
            unmarked++;
            right = *(unmarked);
            //decrement the mark in binary of the chain 
            int j = size-1;
            while (!current_mchain.contains(j)) {
              current_mchain += j;
              j--;
            }
            current_mchain -= j;
         }

         //check each edge in the current chain for legal successors
         do {
            left = mid;
            mid = right;
            Entire<Bitset>::const_iterator legal_edge = current_mchain.begin();
            while (*legal_edge < mid+1)
               legal_edge++;
            right = *legal_edge;
            //check for advances of type 2 which exclude a point from the chain
            if (mid > 0) {
               if (!advance_triangles[left][right-left-1].trivial && advance_triangles[left][right-left-1].adv2.contains(mid-left-1)) {
                  //check if the visibility condition is met for the advanced marked chain in question
                  current_mchain -= mid;
                  if (opt) {
                     on_or_above -= mid;
                     bool visible = true;  
                     Entire<Bitset>::const_iterator untreated = current_mchain.begin();
                     untreated++;
                     int tmp(0);
                     for (size_t m=0; m<left && visible; m=tmp) {
                        tmp = *untreated;
                        untreated++;
                        if ((upper_boundary.contains(m) && upper_boundary.contains(tmp)) || advance_triangles[m][tmp-m-1].monotony)
                           continue;
                        else
                           if (upper_boundary.contains(tmp))
                              visible = false;
                           else
                              if (tmp < left && !(advance_triangles[tmp][*untreated-tmp-1].monotony))
                                 continue;
                              else {
                                 bool retrace = false;
                                 //check for a possible retrace
                                 for (int s = tmp+1; s < n && !retrace; ++s) 
                                    if (advance_triangles[m][tmp-m-1].vis.contains(s-m-2) && on_or_above.contains(s) && 
                                       advance_triangles[m][s-m-1].adv2.contains(tmp-m-1)) 
                                       retrace = true;
                                 if (retrace)
                                    continue;
                                 else
                                    visible = false;
                              }
                     }
                     if (visible) {
                        (*next_lvl)[current_mchain] += (*chain).second;
                        on_or_above += mid;
                     }
                  }
                  if (!opt)
                     (*next_lvl)[current_mchain] += (*chain).second;
                  current_mchain += mid;
               }
               //increment the mark in binary of the chain
               int j = size-1;
               while (current_mchain.contains(j)) {
                  current_mchain -= j;
                  j--;
               }
               current_mchain += j;
            }

            //check for advances of type 1 which include a new point into the chain
            if (right-mid > 1) {
               for (int j = mid+1; j<right; j++) {
                  if (!advance_triangles[mid][right-mid-1].trivial && advance_triangles[mid][right-mid-1].adv1.contains(j-mid-1)) {
                     //check if the visibility condition is met
                     current_mchain += j;
                     if (opt) {
                        bool visible = true;
                        Entire<Bitset>::const_iterator untreated = current_mchain.begin();
                        untreated++;
                        int tmp(0);
                        for (int m=0; m<mid && visible; m=*untreated, untreated++) {
                           tmp = *untreated;
                           untreated++;
                           if ((upper_boundary.contains(m) && upper_boundary.contains(tmp)) || advance_triangles[m][tmp-m-1].monotony)
                              continue;
                           else
                              if (upper_boundary.contains(tmp)) 
                                 visible = false;
                              else
                                 if (tmp < mid && !(advance_triangles[tmp][*untreated-tmp-1].monotony))
                                    continue;
                                 else {
                                    bool retrace = false;
                                    //check for a possible retrace
                                    for (int s = tmp+1; s < n && !retrace; ++s) 
                                       if (advance_triangles[m][tmp-m-1].vis.contains(s-m-2) && on_or_above.contains(s) && 
                                          advance_triangles[m][s-m-1].adv2.contains(tmp-m-1)) 
                                          retrace = true;
                                    if (retrace)
                                       continue;
                                    else 
                                       visible = false;
                                 }
                        }
                        if (visible)
                           (*next_lvl)[current_mchain] += (*chain).second;
                     }
                     if (!opt)
                        (*next_lvl)[current_mchain] += (*chain).second;
                     current_mchain -=j;
                  }
               }
            }
         } while (right < n-1);
      }
      std::swap(current_lvl,next_lvl);
   }
   //calculate the number of triangulations from the counters of the last iteration
   Integer tr(0);
   for (Entire<hash_map<Bitset,Integer> >::iterator chain = (*current_lvl).begin(); chain != (*current_lvl).end(); ++chain) {
      tr += (*chain).second;
   }
   return tr;
}

/* The implementation is based on the following paper: 
"Victor Alvarez, Raimund Seidel. A Simple Aggregative Algorithm for Counting Triangulations of Planar Point Sets and Related Problems. In Proc. of the 29th Symposium on Computational Geometry (SoCG '13), pages 1 – 8, Rio de Janeiro, Brazil, 2013"
*/

UserFunctionTemplate4perl("# @category Triangulations, subdivisions and volume"
                          "# Calculates the number of triangulations of the input points given as rows of a matrix. This can be space intensive."
                          "# @param Matrix M points in the projective plane"
                          "# @param Bool optimization defaults to 1, where 1 includes optimization and 0 excludes it"
                          "# @return Integer number of triangulations"
                          "# @example To print the number of possible triangulations of a square, do this:"
                          "# > print n_triangulations(cube(2)->VERTICES);"
                          "# | 2","n_triangulations(Matrix { optimization => 1})");

} }

// Local Variables:
// mode:C++
// c-basic-offset:3
// indent-tabs-mode:nil
// End:
