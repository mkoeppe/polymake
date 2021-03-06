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

#ifndef POLYMAKE_PERL_TYPES_H
#define POLYMAKE_PERL_TYPES_H

namespace pm { namespace perl {

typedef SV* (*wrapper_type)(SV**, char*);
typedef SV* (*indirect_wrapper_type)(void*, SV**, char*);
typedef void (*destructor_type)(char*);
typedef void (*copy_constructor_type)(void*, const char*);
typedef void (*assignment_type)(char*, SV*, value_flags);
typedef SV* (*conv_to_SV_type)(const char*, const char*);
typedef int (*conv_to_int_type)(const char*);
typedef double (*conv_to_double_type)(const char*);
typedef void (*container_resize_type)(char*, int);
typedef void (*container_begin_type)(void*, char*);
typedef void (*container_access_type)(char*, char*, int, SV*, SV*, const char*);
typedef void (*container_store_type)(char*, char*, int, SV*);
typedef void (*composite_access_type)(char*, SV*, SV*, const char*);
typedef void (*composite_store_type)(char*, SV*);
typedef SV* (*iterator_deref_type)(const char*, const char*);
typedef void (*iterator_incr_type)(char*);
typedef SV* (*provide_type)();

struct composite_access_vtbl {
   composite_access_type get[2];
   composite_store_type store;
};

} }
namespace polymake { namespace perl_bindings {

struct bait { int dummy[1]; };

template <bool exact_match>
struct recognized : bait { int dummies[1+exact_match]; };

inline bait* recognize(SV**, bait*, ...) { return NULL; }
inline SV* field_names(bait*, ...) { return pm::perl::Scalar::undef(); }

template <typename T> class Class;

// don't try to create any magic glue for these guys
inline bait* recognize(SV**, bait*, pm::perl::Object*, pm::perl::Object*) { return NULL; }
inline bait* recognize(SV**, bait*, pm::perl::ObjectType*, pm::perl::ObjectType*) { return NULL; }
inline bait* recognize(SV**, bait*, pm::Array<pm::perl::Object>*, pm::Array<pm::perl::Object>*) { return NULL; }

} }
namespace pm { namespace perl {

typedef polymake::perl_bindings::bait* recognizer_bait;

struct type_infos {
   SV* descr;
   SV* proto;
   bool magic_allowed;

   type_infos() : descr(NULL), proto(NULL), magic_allowed(false) {}

   bool allow_magic_storage() const;
   void set_proto(SV* prescribed_pkg, const std::type_info&, SV* super_proto=NULL);
   void set_proto(SV* known_proto=NULL);
   bool set_descr();
   bool set_descr(const std::type_info&);
};

template <typename T>
struct known_type {
   static const size_t recog_size=sizeof(*recognize((SV**)0, recognizer_bait(0), (T*)0, (T*)0));
   static const bool value = recog_size >= sizeof(polymake::perl_bindings::recognized<false>),
               exact_match = recog_size == sizeof(polymake::perl_bindings::recognized<true>);
};

template <typename Base, typename E, typename Params>
struct known_type< sparse_elem_proxy<Base,E,Params> > {
   static const bool value=known_type<E>::value || list_contains<primitive_lvalues,E>::value,
               exact_match=false;
};

template <typename T,
          bool _known=known_type<T>::value, bool _exact_match=known_type<T>::exact_match,
          bool _has_persistent=known_type<typename object_traits<T>::persistent_type>::value,
          bool _has_generic=known_type<typename generic_representative<T>::type>::value,
          bool _is_sparse_proxy=is_instance3_of<T,sparse_elem_proxy>::value>
class type_cache_helper {
protected:
   // primary template: nothing known
   // either a special class with prescribed perl package, or a built-in type, or something really undeclared

   typedef pm::list prohibited_magic_for(Object, pm::Array<Object>, ObjectType);

   static type_infos get(SV* known_proto)
   {
      type_infos _infos;
      if (!list_contains<prohibited_magic_for, T>::value &&
          _infos.set_descr(typeid(typename remove_unsigned<T>::type))) {
         _infos.set_proto(known_proto);
         _infos.magic_allowed=_infos.allow_magic_storage();
      }
      return _infos;
   }

   static type_infos get_with_prescribed_pkg(SV* prescribed_pkg, bool accept_magic)
   {
      type_infos _infos;
      _infos.set_proto(prescribed_pkg, typeid(T));
      _infos.magic_allowed= accept_magic && _infos.allow_magic_storage();
      _infos.descr= _infos.magic_allowed ? polymake::perl_bindings::Class<T>::register_it(0,1,_infos.proto) : 0;
      return _infos;
   }
};

class type_cache_base {
protected:
   static wrapper_type get_function_wrapper(SV* src, SV* descr, int auto_func_index);
   static wrapper_type get_assignment_operator(SV* src, SV* descr);
   static wrapper_type get_conversion_operator(SV* src, SV* descr);
   static wrapper_type get_conversion_constructor(SV* src, SV* descr);
};

template <typename T>
class type_cache
   : protected type_cache_base
   , protected type_cache_helper<T> {
protected:
   typedef type_cache_helper<T> super;

   static type_infos& get(SV* known_proto=NULL)
   {
      static type_infos _infos=super::get(known_proto);
      return _infos;
   }

   static type_infos& get_with_prescribed_pkg(SV* prescribed_pkg)
   {
      static type_infos _infos=super::get_with_prescribed_pkg(prescribed_pkg, !object_traits<T>::is_lazy);
      return _infos;
   }
public:
   static SV* get_descr(SV* known_proto=NULL) { return get(known_proto).descr; }
   static SV* get_proto(SV* known_proto=NULL) { return get(known_proto).proto; }
   static SV* provide() { return get_proto(); } // for ClassRegistrator
   static bool magic_allowed(int)
   {
      return get().magic_allowed;
   }
   static bool magic_allowed(SV* prescribed_pkg)
   {
      return get_with_prescribed_pkg(prescribed_pkg).magic_allowed;
   }

   static wrapper_type get_assignment_operator(SV* src)
   {
      return type_cache_base::get_assignment_operator(src, get_descr());
   }

   static wrapper_type get_conversion_operator(SV* src)
   {
      return type_cache_base::get_conversion_operator(src, get_descr());
   }

   static wrapper_type get_conversion_constructor(SV* src)
   {
      return type_cache_base::get_conversion_constructor(src, get_descr());
   }
};

template <typename T, typename Representative>
class type_cache_via {
protected:
   static SV* _get_descr(SV* proto, False)
   {
      return polymake::perl_bindings::Class<T>::register_it(0,0,proto);
   }
   static SV* _get_descr(SV*, True) { return NULL; }

   static type_infos get(SV* known_proto)
   {
      assert(known_proto==NULL);
      type_infos _infos;
      _infos.proto=type_cache<Representative>::get_proto();
      _infos.magic_allowed=type_cache<Representative>::magic_allowed(0);
      _infos.descr=_infos.proto ? _get_descr(_infos.proto, bool2type<object_traits<T>::is_lazy>()) : 0;
      return _infos;
   }

   static type_infos get_with_prescribed_pkg(SV* prescribed_pkg, bool accept_magic)
   {
      type_infos _infos;
      _infos.set_proto(prescribed_pkg, typeid(T), type_cache<Representative>::get_proto());
      _infos.magic_allowed= accept_magic && _infos.allow_magic_storage();
      _infos.descr= _infos.magic_allowed ? polymake::perl_bindings::Class<T>::register_it(0,1,_infos.proto) : 0;
      return _infos;
   }
};

template <typename T, bool _has_generic>
class type_cache_helper<T, false, false, true, _has_generic, false>
   : public type_cache_via<T, typename object_traits<T>::persistent_type> {};

template <typename T>
class type_cache_helper<T, false, false, false, true, false>
   :  public type_cache_via<T, typename generic_representative<T>::type> {};

template <typename T, bool _has_persistent, bool _has_generic>
class type_cache_helper<T, true, false, _has_persistent, _has_generic, true> {
protected:
   static type_infos get(SV* known_proto)
   {
      assert(known_proto==NULL);
      type_infos _infos;
      _infos.proto=type_cache<typename T::value_type>::get_proto();
      _infos.magic_allowed=true;
      _infos.descr=polymake::perl_bindings::Class<T>::register_it(0, 0, _infos.proto);
      return _infos;
   }
};

template <typename T, bool _has_persistent, bool _has_generic>
class type_cache_helper<T, true, false, _has_persistent, _has_generic, false> {
protected:
   static type_infos get(SV* known_proto)
   {
      assert(known_proto==NULL);
      type_infos _infos;
      recognize(&_infos.proto, recognizer_bait(0), (T*)0, (T*)0);
      _infos.magic_allowed=_infos.allow_magic_storage();
      _infos.descr=polymake::perl_bindings::Class<T>::register_it(0, 0, _infos.proto);
      return _infos;
   }
};

template <typename T, bool _has_persistent, bool _has_generic>
class type_cache_helper<T, true, true, _has_persistent, _has_generic, false> {
protected:
   static type_infos get(SV* known_proto)
   {
      type_infos _infos;
      if (known_proto
          ? (_infos.set_proto(known_proto),
             true)
          : (recognize(&_infos.proto, recognizer_bait(0), (T*)0, (T*)0),
             _infos.proto != NULL)) {
         if ((_infos.magic_allowed = _infos.allow_magic_storage()))
            _infos.set_descr();
      }
      return _infos;
   }
};

template <typename T>
struct is_lvalue : bool2type<attrib<T>::is_reference && !attrib<T>::is_const> {};

template <typename T>
struct is_mutable : bool2type<pm::is_mutable<T>::value && !object_traits<T>::is_always_const> {};

template <typename type_list, int i>
struct TypeList_helper {
   static const int next= i+1 < list_length<type_list>::value ? i+1 : i;
   typedef TypeList_helper<type_list,next> recurse_down;
   typedef typename n_th<type_list,i>::type T;

   static bool push_types(Stack& stack)
   {
      if (SV* const Tproto=type_cache<T>::get_proto()) {
         stack.push(Tproto);
         return next==i || recurse_down::push_types(stack);
      } else {
         return false;
      }
   }

   static void gather_flags(ArrayHolder& arr)
   {
      type_cache<typename attrib<T>::minus_const_ref>::get_proto();
      if (is_lvalue<T>::value) {
         Value lvalue_arg;
         lvalue_arg << i;
         arr.push(lvalue_arg);
      }
      if (next>i) recurse_down::gather_flags(arr);
   }

   static void gather_types(ArrayHolder& arr)
   {
      typedef typename access<T>::type Type;
      arr.push(Scalar::const_string_with_int(typeid(Type).name(), attrib<Type>::is_const));
      if (next>i) recurse_down::gather_types(arr);
   }

   static void gather_descriptors(ArrayHolder& arr)
   {
      SV* arg_proto=type_cache<typename attrib<T>::minus_const_ref>::get_proto();
      arr.push(arg_proto ? arg_proto : Scalar::undef());
      if (next>i) recurse_down::gather_descriptors(arr);
   }
};

template <int i>
struct TypeList_helper<void,i> {
   static bool push_types(Stack&) { return true; }
   static void gather_flags(ArrayHolder&) {}
   static void gather_types(ArrayHolder&) {}
   static void gather_descriptors(ArrayHolder&) {}
};

template <typename Fptr>
class TypeListUtils {
   typedef typename list2cons<Fptr>::type type_list;

   static SV* gather_flags()
   {
      ArrayHolder arr(1+list_accumulate_unary<list_count, is_lvalue, type_list>::value);
      Value void_result;
      void_result << (identical<typename list2cons<Fptr>::return_type, void>::value ||
                      identical<typename list2cons<Fptr>::return_type, ListReturn>::value);
      arr.push(void_result);
      TypeList_helper<type_list,0>::gather_flags(arr);
      return arr.get();
   }

   static SV* gather_types()
   {
      ArrayHolder arr(list_length<type_list>::value);
      TypeList_helper<type_list,0>::gather_types(arr);
      return arr.get();
   }

   static SV* gather_descriptors()
   {
      ArrayHolder arr(list_length<type_list>::value);
      TypeList_helper<type_list,0>::gather_descriptors(arr);
      arr.set_contains_aliases();
      return arr.get();
   }

public:
   static const int type_cnt=list_length<type_list>::value;

   static bool push_types(Stack& stack)
   {
      return TypeList_helper<type_list,0>::push_types(stack);
   }

   // build an array with void-return and arg-is-lvalue flags;
   // the signature of this function has to be compatible with wrapper_type
   static SV* get_flags(SV**, char*)
   {
      static SV* ret=gather_flags();
      return ret;
   }

   static SV* get_types()
   {
      static SV* types=gather_types();
      return types;
   }

   // the same, but with prescribed text for the first element
   static SV* get_types(const char* first_arg_name)
   {
      ArrayHolder arr(list_length<type_list>::value);
      arr.push(Scalar::const_string(first_arg_name));
      static const int next= list_length<type_list>::value > 1;
      if (next) TypeList_helper<type_list,next>::gather_types(arr);
      return arr.get();
   }

   // instantiate the perl-side descriptors of all types in type_list
   static SV* provide_types()
   {
      static SV* types=gather_descriptors();
      return types;
   }
};

} }

#endif // POLYMAKE_PERL_TYPES_H

// Local Variables:
// mode:C++
// c-basic-offset:3
// indent-tabs-mode:nil
// End:
