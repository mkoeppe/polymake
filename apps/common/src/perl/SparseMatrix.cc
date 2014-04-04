/* Copyright (c) 1997-2014
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

///==== this line controls the automatic file splitting: max.instances=20

#include "polymake/client.h"
#include "polymake/SparseMatrix.h"
#include "polymake/IncidenceMatrix.h"
#include "polymake/Matrix.h"
#include "polymake/Rational.h"
#include "polymake/QuadraticExtension.h"
#include "polymake/linalg.h"
#include "polymake/Vector.h"
#include "polymake/Integer.h"

namespace polymake { namespace common { namespace {
///==== Automatically generated contents follow.    Please do not delete this line. ====
   template <typename T0>
   FunctionInterface4perl( new_int_int, T0 ) {
      perl::Value arg0(stack[1]), arg1(stack[2]);
      WrapperReturnNew(T0, (arg0.get<int>(), arg1.get<int>()) );
   };

   FunctionInstance4perl(new_int_int, SparseMatrix< double, NonSymmetric >);
   OperatorInstance4perl(Binary_diva, perl::Canned< const Wary< pm::DiagMatrix<pm::SameElementVector<pm::Rational const&>, true> > >, perl::Canned< const pm::RepeatedRow<pm::SameElementVector<pm::Rational const&> > >);
   OperatorInstance4perl(Binary_mul, perl::Canned< const Wary< SparseMatrix< Integer, NonSymmetric > > >, perl::Canned< const pm::ColChain<pm::DiagMatrix<pm::SameElementVector<pm::Rational const&>, true> const&, pm::RepeatedRow<pm::SameElementVector<pm::Rational const&> > const&> >);
   OperatorInstance4perl(Binary_mul, perl::Canned< const Wary< SparseMatrix< Integer, NonSymmetric > > >, perl::Canned< const pm::RowChain<pm::DiagMatrix<pm::SameElementVector<pm::Rational const&>, true> const&, pm::RepeatedRow<pm::SameElementVector<pm::Rational const&> > const&> >);
   OperatorInstance4perl(Binary_mul, perl::Canned< const Wary< pm::RowChain<pm::DiagMatrix<pm::SameElementVector<pm::Rational const&>, true> const&, pm::RepeatedRow<pm::SameElementVector<pm::Rational const&> > const&> > >, perl::Canned< const Matrix< Integer > >);
   OperatorInstance4perl(Binary_diva, perl::Canned< const Wary< pm::DiagMatrix<pm::SameElementVector<pm::Rational const&>, true> > >, perl::Canned< const Vector< Rational > >);
   OperatorInstance4perl(Unary_neg, perl::Canned< const Wary< pm::DiagMatrix<pm::SameElementVector<pm::Rational const&>, true> > >);
   OperatorInstance4perl(Binary_diva, perl::Canned< const Wary< pm::DiagMatrix<pm::SameElementVector<pm::Rational const&>, true> > >, perl::Canned< const SparseMatrix< Rational, Symmetric > >);
   OperatorInstance4perl(Unary_neg, perl::Canned< const Wary< SparseMatrix< Rational, NonSymmetric > > >);
   FunctionInstance4perl(new_int_int, SparseMatrix< QuadraticExtension< Rational >, NonSymmetric >);
   Class4perl("Polymake::common::SparseMatrix_A_Float_I_Symmetric_Z", SparseMatrix< double, Symmetric >);
   OperatorInstance4perl(Binary_mul, perl::Canned< const Wary< SparseMatrix< double, NonSymmetric > > >, perl::Canned< const Matrix< double > >);
///==== Automatically generated contents end here.  Please do not delete this line. ====
} } }