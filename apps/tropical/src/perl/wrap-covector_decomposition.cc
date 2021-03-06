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

#include "polymake/Rational.h"
#include "polymake/TropicalNumber.h"

namespace polymake { namespace tropical { namespace {
///==== Automatically generated contents follow.    Please do not delete this line. ====
   template <typename T0, typename T1>
   FunctionInterface4perl( compute_covector_decomposition_T_x_x_f16, T0,T1 ) {
      perl::Value arg0(stack[0]), arg1(stack[1]);
      WrapperReturnVoid( (compute_covector_decomposition<T0,T1>(arg0, arg1)) );
   };

   FunctionWrapper4perl( bool (pm::IncidenceMatrix<pm::NonSymmetric> const&) ) {
      perl::Value arg0(stack[0]);
      IndirectWrapperReturn( arg0.get< perl::TryCanned< const IncidenceMatrix< NonSymmetric > > >() );
   }
   FunctionWrapperInstance4perl( bool (pm::IncidenceMatrix<pm::NonSymmetric> const&) );

   FunctionInstance4perl(compute_covector_decomposition_T_x_x_f16, Min, Rational);
   FunctionWrapper4perl( pm::IncidenceMatrix<pm::NonSymmetric> (pm::Array<pm::IncidenceMatrix<pm::NonSymmetric>, void> const&, pm::Set<int, pm::operations::cmp> const&) ) {
      perl::Value arg0(stack[0]), arg1(stack[1]);
      IndirectWrapperReturn( arg0.get< perl::TryCanned< const Array< IncidenceMatrix< NonSymmetric > > > >(), arg1.get< perl::TryCanned< const Set< int > > >() );
   }
   FunctionWrapperInstance4perl( pm::IncidenceMatrix<pm::NonSymmetric> (pm::Array<pm::IncidenceMatrix<pm::NonSymmetric>, void> const&, pm::Set<int, pm::operations::cmp> const&) );

   FunctionInstance4perl(compute_covector_decomposition_T_x_x_f16, Max, Rational);
///==== Automatically generated contents end here.  Please do not delete this line. ====
} } }
