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

#include "polymake/IncidenceMatrix.h"
#include "polymake/Integer.h"
#include "polymake/Matrix.h"
#include "polymake/PuiseuxFraction.h"
#include "polymake/QuadraticExtension.h"
#include "polymake/Rational.h"
#include "polymake/Set.h"
#include "polymake/SparseMatrix.h"
#include "polymake/SparseVector.h"
#include "polymake/TropicalNumber.h"
#include "polymake/Vector.h"
#include "polymake/client.h"
#include "polymake/linalg.h"

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

   template <typename T0>
   FunctionInterface4perl( new_int, T0 ) {
      perl::Value arg0(stack[1]);
      WrapperReturnNew(T0, (arg0.get<int>()) );
   };

   FunctionInstance4perl(new_int, Vector< Set< int > >);
   FunctionInstance4perl(new_X, Vector< TropicalNumber< Min, Rational > >, perl::Canned< const pm::SameElementVector<pm::TropicalNumber<pm::Min, pm::Rational> const&> >);
   FunctionInstance4perl(new_X, Vector< TropicalNumber< Min, Rational > >, perl::Canned< const pm::SameElementSparseVector<pm::SingleElementSet<int>, pm::TropicalNumber<pm::Min, pm::Rational> > >);
   OperatorInstance4perl(Binary_mul, perl::Canned< const Wary< Vector< Integer > > >, perl::Canned< const pm::SameElementVector<pm::Integer const&> >);
   FunctionInstance4perl(new_X, Vector< TropicalNumber< Max, Rational > >, perl::Canned< const pm::SameElementVector<pm::TropicalNumber<pm::Max, pm::Rational> const&> >);
   FunctionInstance4perl(new_X, Vector< TropicalNumber< Max, Rational > >, perl::Canned< const Vector< TropicalNumber< Max, Rational > > >);
   OperatorInstance4perl(BinaryAssign_add, perl::Canned< Wary< Vector< Rational > > >, perl::Canned< const pm::IndexedSlice<pm::masquerade<pm::ConcatRows, pm::Matrix_base<pm::Rational>&>, pm::Series<int, true>, void> >);
   FunctionInstance4perl(new_X, Vector< int >, perl::Canned< const Vector< int > >);
   FunctionInstance4perl(new_X, Vector< Integer >, perl::Canned< const Vector< Integer > >);
   FunctionInstance4perl(new_X, Vector< Rational >, perl::Canned< const pm::IndexedSlice<pm::masquerade<pm::ConcatRows, pm::Matrix_base<pm::Rational> const&>, pm::Series<int, false>, void> >);
   OperatorInstance4perl(Binary_add, perl::Canned< const Wary< pm::IndexedSlice<pm::masquerade<pm::ConcatRows, pm::Matrix_base<pm::Rational> const&>, pm::Series<int, false>, void> > >, perl::Canned< const Vector< Rational > >);
   OperatorInstance4perl(Binary__ora, perl::Canned< const Vector< Rational > >, perl::Canned< const SparseMatrix< Rational, NonSymmetric > >);
   FunctionInstance4perl(new_X, Vector< bool >, perl::Canned< const Vector< bool > >);
   OperatorInstance4perl(Binary__eq, perl::Canned< const Wary< Vector< bool > > >, perl::Canned< const Vector< bool > >);
   OperatorInstance4perl(Binary_add, perl::Canned< const Wary< Vector< Rational > > >, perl::Canned< const Vector< Rational > >);
   FunctionInstance4perl(new, Vector< Set< int > >);
   OperatorInstance4perl(convert, Vector< int >, perl::Canned< const Vector< Rational > >);
   OperatorInstance4perl(BinaryAssign__or, perl::Canned< Vector< Rational > >, perl::Canned< const Rational >);
   OperatorInstance4perl(BinaryAssign__or, perl::Canned< Vector< int > >, int);
   FunctionInstance4perl(new_X, Vector< TropicalNumber< Min, Rational > >, perl::Canned< const SparseVector< TropicalNumber< Min, Rational > > >);
   FunctionInstance4perl(new_X, Vector< TropicalNumber< Min, Rational > >, perl::Canned< const pm::SameElementSparseVector<pm::SingleElementSet<int>, pm::TropicalNumber<pm::Min, pm::Rational> const&> >);
   Class4perl("Polymake::common::Vector__PuiseuxFraction_A_Min_I_Rational_I_Rational_Z", Vector< PuiseuxFraction< Min, Rational, Rational > >);
   FunctionInstance4perl(new, Vector< PuiseuxFraction< Min, Rational, Rational > >);
   OperatorInstance4perl(Binary__eq, perl::Canned< const Wary< Vector< PuiseuxFraction< Min, Rational, Rational > > > >, perl::Canned< const Vector< PuiseuxFraction< Min, Rational, Rational > > >);
   Class4perl("Polymake::common::Vector__PuiseuxFraction_A_Max_I_Rational_I_Rational_Z", Vector< PuiseuxFraction< Max, Rational, Rational > >);
   OperatorInstance4perl(convert, Vector< PuiseuxFraction< Min, Rational, Rational > >, perl::Canned< const SparseVector< PuiseuxFraction< Min, Rational, Rational > > >);
   FunctionInstance4perl(new_X, Vector< PuiseuxFraction< Min, Rational, Rational > >, perl::Canned< const Vector< PuiseuxFraction< Min, Rational, Rational > > >);
   FunctionInstance4perl(new, Vector< PuiseuxFraction< Max, Rational, Rational > >);
   OperatorInstance4perl(Unary_neg, perl::Canned< const Wary< pm::IndexedSlice<pm::IndexedSlice<pm::masquerade<pm::ConcatRows, pm::Matrix_base<pm::Rational> const&>, pm::Series<int, true>, void> const&, pm::Series<int, true>, void> > >);
   OperatorInstance4perl(assign, pm::IndexedSlice<pm::masquerade<pm::ConcatRows, pm::Matrix_base<pm::Rational>&>, pm::Series<int, true>, void>, perl::Canned< const pm::sparse_matrix_line<pm::AVL::tree<pm::sparse2d::traits<pm::sparse2d::traits_base<pm::QuadraticExtension<pm::Rational>, true, false, (pm::sparse2d::restriction_kind)0>, false, (pm::sparse2d::restriction_kind)0> > const&, pm::NonSymmetric> >);
   OperatorInstance4perl(assign, pm::IndexedSlice<pm::masquerade<pm::ConcatRows, pm::Matrix_base<pm::QuadraticExtension<pm::Rational> >&>, pm::Series<int, true>, void>, perl::Canned< const pm::sparse_matrix_line<pm::AVL::tree<pm::sparse2d::traits<pm::sparse2d::traits_base<pm::QuadraticExtension<pm::Rational>, true, false, (pm::sparse2d::restriction_kind)0>, false, (pm::sparse2d::restriction_kind)0> > const&, pm::NonSymmetric> >);
   FunctionInstance4perl(new_X, Vector< PuiseuxFraction< Min, Rational, Rational > >, perl::Canned< const pm::IndexedSlice<pm::masquerade<pm::ConcatRows, pm::Matrix_base<pm::PuiseuxFraction<pm::Min, pm::Rational, pm::Rational> > const&>, pm::Series<int, true>, void> >);
   OperatorInstance4perl(Binary__ora, perl::Canned< const Vector< int > >, perl::Canned< const pm::MatrixMinor<pm::SparseMatrix<int, pm::NonSymmetric> const&, pm::Complement<pm::Set<int, pm::operations::cmp>, int, pm::operations::cmp> const&, pm::all_selector const&> >);
   Class4perl("Polymake::common::Vector__IncidenceMatrix__NonSymmetric", Vector< IncidenceMatrix< NonSymmetric > >);
   OperatorInstance4perl(Binary__gt, perl::Canned< const Wary< Vector< int > > >, perl::Canned< const Vector< int > >);
   OperatorInstance4perl(Binary__eq, perl::Canned< const Wary< Vector< Integer > > >, perl::Canned< const pm::SameElementSparseVector<pm::SingleElementSet<int>, pm::Integer> >);
   FunctionInstance4perl(new_int, Vector< PuiseuxFraction< Min, Rational, Rational > >);
   OperatorInstance4perl(Binary_sub, perl::Canned< const Wary< pm::IndexedSlice<pm::masquerade<pm::ConcatRows, pm::Matrix_base<pm::QuadraticExtension<pm::Rational> >&>, pm::Series<int, true>, void> > >, perl::Canned< const pm::IndexedSlice<pm::masquerade<pm::ConcatRows, pm::Matrix_base<pm::QuadraticExtension<pm::Rational> >&>, pm::Series<int, true>, void> >);
   OperatorInstance4perl(Binary_sub, perl::Canned< const Wary< pm::IndexedSlice<pm::masquerade<pm::ConcatRows, pm::Matrix_base<double>&>, pm::Series<int, true>, void> > >, perl::Canned< const pm::IndexedSlice<pm::masquerade<pm::ConcatRows, pm::Matrix_base<double>&>, pm::Series<int, true>, void> >);
   OperatorInstance4perl(Binary__ora, perl::Canned< const pm::SameElementVector<pm::Rational const&> >, perl::Canned< const pm::RowChain<pm::RowChain<pm::RowChain<pm::Matrix<pm::Rational> const&, pm::Matrix<pm::Rational> const&> const&, pm::Matrix<pm::Rational> const&> const&, pm::Matrix<pm::Rational> const&> >);
   OperatorInstance4perl(Binary__ora, perl::Canned< const pm::SameElementVector<pm::Rational const&> >, perl::Canned< const pm::RowChain<pm::RowChain<pm::RowChain<pm::RowChain<pm::RowChain<pm::RowChain<pm::Matrix<pm::Rational> const&, pm::Matrix<pm::Rational> const&> const&, pm::Matrix<pm::Rational> const&> const&, pm::Matrix<pm::Rational> const&> const&, pm::Matrix<pm::Rational> const&> const&, pm::Matrix<pm::Rational> const&> const&, pm::Matrix<pm::Rational> const&> >);
   FunctionInstance4perl(new_X, Vector< Rational >, perl::Canned< const pm::IndexedSlice<pm::Vector<pm::Integer> const&, pm::Series<int, true>, void> >);
   FunctionInstance4perl(new_X, Vector< Rational >, perl::Canned< const pm::IndexedSlice<pm::Vector<pm::Rational> const&, pm::Series<int, true>, void> >);
///==== Automatically generated contents end here.  Please do not delete this line. ====
} } }
