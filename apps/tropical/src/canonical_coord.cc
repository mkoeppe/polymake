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
#include "polymake/Vector.h"
#include "polymake/Matrix.h"
#include "polymake/linalg.h"
#include "polymake/TropicalNumber.h"
#include "polymake/polytope/canonicalize.h"

namespace polymake { namespace tropical {
	namespace {

		template <typename Iterator> inline
			bool leading_non_zero(const Iterator& it, pm::False) { return !is_zero(*it); }

		template <typename Iterator> inline
			bool leading_non_zero(const Iterator& it, pm::True) { return !it.at_end() && it.index()==0; }
	}

	//The following all canonicalize a matrix/vector of tropical numbers such that the first entry
	// is zero.

	template <typename Vector, typename Addition, typename Scalar>
		void canonicalize_to_leading_zero(GenericVector<Vector, TropicalNumber<Addition, Scalar> >& V)
		{
			typename Vector::iterator it=V.top().begin();
			if (leading_non_zero(it, bool2type<pm::check_container_feature<Vector,pm::sparse>::value>())) {
				const typename Vector::element_type first=*it;
				V/=first;
			}
		}

	template <typename Matrix, typename Addition, typename Scalar>
		void canonicalize_to_leading_zero(GenericMatrix<Matrix, TropicalNumber<Addition,Scalar> >& M)
		{
			if (!M.rows())
				throw std::runtime_error("point matrix may not be empty");
			for (typename Entire< Rows<Matrix> >::iterator r=entire(rows(M)); !r.at_end();  ++r)
				canonicalize_to_leading_zero(r->top());
		}

	//Does the same as canonicalize_to_leading_zero, and checks and complains if there is a zero column
	template <typename Matrix, typename Addition, typename Scalar>
		void canonicalize_to_leading_zero_and_check_columns(GenericMatrix<Matrix, TropicalNumber<Addition,Scalar> >&M) {
			for(typename Entire<Rows<Transposed<Matrix> > >::iterator c = entire(rows(T(M))); !c.at_end(); c++) {
				if(support(*c).size() == 0) 
					throw std::runtime_error("The points can't all lie in the same boundary stratum of projective space. Maybe use a projection?");
			}
			return canonicalize_to_leading_zero(M);
		}
/*
	template <typename Vector, typename Addition, typename Scalar>
		void canonicalize_to_nonnegative(GenericVector<Vector, TropicalNumber<Addition,Scalar> >& V)
		{
			const typename Vector::element_type x_min=accumulate(V.top(), operations::min());
			if (pm::check_container_feature<Vector,pm::sparse>::value
					? x_min<0 || V.top().size()==V.dim()
					: !is_zero(x_min))
				V/=x_min;
		}

	template <typename Matrix, typename Addition, typename Scalar>
		void canonicalize_to_nonnegative(GenericMatrix<Matrix, TropicalNumber<Addition,Scalar> >& M)
		{
			if (!M.rows())
				throw std::runtime_error("point matrix may not be empty");
			for (typename Entire< Rows<Matrix> >::iterator r=entire(rows(M)); !r.at_end();  ++r)
				canonicalize_to_nonnegative(r->top());
		}

*/
	FunctionTemplate4perl("canonicalize_to_leading_zero(Vector&) : void");
	FunctionTemplate4perl("canonicalize_to_leading_zero(Matrix&) : void");
	//FunctionTemplate4perl("canonicalize_to_nonnegative(Vector&) : void");
	//FunctionTemplate4perl("canonicalize_to_nonnegative(Matrix&) : void");

	//The following canonicalize matrices/vectors of scalars, but representing (finite) 
	//tropical homogeneous coordinates.

	template <typename Vector, typename Scalar>
		void canonicalize_scalar_to_leading_zero(GenericVector<Vector,Scalar>& V)
		{
			typename Vector::iterator it=V.top().begin();
			if (leading_non_zero(it, bool2type<pm::check_container_feature<Vector,pm::sparse>::value>())) {
				const typename Vector::element_type first=*it;
				V-=same_element_vector(first, V.dim());
			}
		}

	template <typename Matrix, typename Scalar>
		void canonicalize_scalar_to_leading_zero(GenericMatrix<Matrix,Scalar>& M)
		{
			if (!M.rows())
				throw std::runtime_error("point matrix may not be empty");
			for (typename Entire< Rows<Matrix> >::iterator r=entire(rows(M)); !r.at_end();  ++r)
				canonicalize_scalar_to_leading_zero(r->top());
		}

	//FIXME Should this be swapped for min/max, i.e. should there be a nonpositive - or rather,
	// a templated version of this?

	template <typename Vector, typename Scalar>
		void canonicalize_scalar_to_nonnegative(GenericVector<Vector,Scalar>& V)
		{
			const typename Vector::element_type x_min=accumulate(V.top(), operations::min());
			if (pm::check_container_feature<Vector,pm::sparse>::value
					? x_min<0 || V.top().size()==V.dim()
					: !is_zero(x_min))
				V-=same_element_vector(x_min,V.dim());
		}

	template <typename Matrix, typename Scalar>
		void canonicalize_scalar_to_nonnegative(GenericMatrix<Matrix,Scalar>& M)
		{
			if (!M.rows())
				throw std::runtime_error("point matrix may not be empty");
			for (typename Entire< Rows<Matrix> >::iterator r=entire(rows(M)); !r.at_end();  ++r)
				canonicalize_scalar_to_nonnegative(r->top());
		}

	// Assumes as input a matrix of tropically homogeneous vectors with a leading 1/0 indicating
	// vertex or far vertex. Will canonicalize M.minor(All,~[0]) to have first coordinate 0. 
	// Then will canonicalize the full matrix as a usual vertex matrix.
	template <typename Matrix, typename Scalar>
		void canonicalize_vertices_to_leading_zero(GenericMatrix<Matrix,Scalar> &M) {
			canonicalize_scalar_to_leading_zero(M.minor(All,~scalar2set(0)).top());
			for(typename Entire< Rows<Matrix> >::iterator r = entire(rows(M)); !r.at_end(); ++r) {
				polymake::polytope::canonicalize_oriented( find_if(entire(r->top()), operations::non_zero()) );
			}
		}

	template <typename Vector, typename Scalar>
		void canonicalize_vertex_to_leading_zero(GenericVector<Vector,Scalar> &V) {
			canonicalize_scalar_to_leading_zero(V.slice(~scalar2set(0)).top());
			polymake::polytope::canonicalize_oriented( find_if(entire(V.top()), operations::non_zero()) );
		}

	FunctionTemplate4perl("canonicalize_scalar_to_leading_zero(Vector&) : void");
	FunctionTemplate4perl("canonicalize_scalar_to_leading_zero(Matrix&) : void");
	FunctionTemplate4perl("canonicalize_to_leading_zero_and_check_columns(Matrix&) : void");
	FunctionTemplate4perl("canonicalize_scalar_to_nonnegative(Vector&) : void");
	FunctionTemplate4perl("canonicalize_scalar_to_nonnegative(Matrix&) : void");
	FunctionTemplate4perl("canonicalize_vertices_to_leading_zero(Matrix&) : void");
	FunctionTemplate4perl("canonicalize_vertex_to_leading_zero(Vector&) : void");



	//FunctionTemplate4perl("dehomogenize_trop(Vector)");
	//FunctionTemplate4perl("dehomogenize_trop(Matrix)");

} }

// Local Variables:
// mode:C++
// c-basic-offset:3
// indent-tabs-mode:nil
// End:
