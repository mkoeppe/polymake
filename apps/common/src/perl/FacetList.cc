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

///==== this line controls the automatic file splitting: max.instances=40

#include "polymake/Array.h"
#include "polymake/FacetList.h"
#include "polymake/IncidenceMatrix.h"
#include "polymake/Set.h"
#include "polymake/client.h"

namespace polymake { namespace common { namespace {
///==== Automatically generated contents follow.    Please do not delete this line. ====
   template <typename T0>
   FunctionInterface4perl( new, T0 ) {
      WrapperReturnNew(T0, () );
   };

   template <typename T0, typename T1>
   FunctionInterface4perl( new_X, T0,T1 ) {
      perl::Value arg0(stack[1]);
      WrapperReturnNew(T0, (arg0.get<T1>()) );
   };

   Class4perl("Polymake::common::FacetList", FacetList);
   FunctionInstance4perl(new_X, FacetList, perl::Canned< const Array< Set< int > > >);
   OperatorInstance4perl(convert, FacetList, perl::Canned< const Array< Set< int > > >);
   OperatorInstance4perl(convert, FacetList, perl::Canned< const IncidenceMatrix< NonSymmetric > >);
   FunctionInstance4perl(new, FacetList);
   FunctionInstance4perl(new_X, FacetList, perl::Canned< const FacetList >);
///==== Automatically generated contents end here.  Please do not delete this line. ====
} } }
