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

namespace polymake { namespace polytope { namespace {
///==== Automatically generated contents follow.    Please do not delete this line. ====
   template <typename T0>
   FunctionInterface4perl( integer_points_bbox_T_x, T0 ) {
      perl::Value arg0(stack[0]);
      WrapperReturn( (integer_points_bbox<T0>(arg0)) );
   };

   FunctionInstance4perl(integer_points_bbox_T_x, double);
   FunctionInstance4perl(integer_points_bbox_T_x, Rational);
///==== Automatically generated contents end here.  Please do not delete this line. ====
} } }
