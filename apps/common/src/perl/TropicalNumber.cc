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

#include "polymake/Integer.h"
#include "polymake/Rational.h"
#include "polymake/TropicalNumber.h"
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

   ClassTemplate4perl("Polymake::common::TropicalNumber");
   Class4perl("Polymake::common::TropicalNumber_A_Min_I_Rational_Z", TropicalNumber< Min, Rational >);
   FunctionInstance4perl(new_X, TropicalNumber< Min, Rational >, int);
   OperatorInstance4perl(Binary_add, perl::Canned< const TropicalNumber< Min, Rational > >, perl::Canned< const TropicalNumber< Min, Rational > >);
   OperatorInstance4perl(Binary_mul, perl::Canned< const TropicalNumber< Min, Rational > >, perl::Canned< const TropicalNumber< Min, Rational > >);
   Class4perl("Polymake::common::TropicalNumber_A_Max_I_Integer_Z", TropicalNumber< Max, Integer >);
   FunctionInstance4perl(new_X, TropicalNumber< Max, Integer >, int);
   OperatorInstance4perl(Binary_add, perl::Canned< const TropicalNumber< Max, Integer > >, perl::Canned< const TropicalNumber< Max, Integer > >);
   OperatorInstance4perl(Binary_mul, perl::Canned< const TropicalNumber< Max, Integer > >, perl::Canned< const TropicalNumber< Max, Integer > >);
   FunctionInstance4perl(new, TropicalNumber< Min, Rational >);
   FunctionInstance4perl(new, TropicalNumber< Max, Integer >);
   OperatorInstance4perl(Binary_add, perl::Canned< const TropicalNumber< Min, Rational > >, perl::Canned< const Rational >);
   Class4perl("Polymake::common::TropicalNumber_A_Max_I_Rational_Z", TropicalNumber< Max, Rational >);
   FunctionInstance4perl(new_X, TropicalNumber< Max, Rational >, int);
   FunctionInstance4perl(new_X, TropicalNumber< Max, Rational >, perl::Canned< const Rational >);
   FunctionInstance4perl(new_X, TropicalNumber< Min, Rational >, perl::Canned< const Rational >);
   OperatorInstance4perl(BinaryAssign_mul, perl::Canned< TropicalNumber< Min, Rational > >, perl::Canned< const TropicalNumber< Min, Rational > >);
   OperatorInstance4perl(BinaryAssign_mul, perl::Canned< TropicalNumber< Max, Rational > >, perl::Canned< const TropicalNumber< Max, Rational > >);
   OperatorInstance4perl(Binary__eq, perl::Canned< const TropicalNumber< Min, Rational > >, perl::Canned< const TropicalNumber< Min, Rational > >);
   OperatorInstance4perl(Binary__eq, perl::Canned< const TropicalNumber< Max, Rational > >, perl::Canned< const TropicalNumber< Max, Rational > >);
///==== Automatically generated contents end here.  Please do not delete this line. ====
} } }
