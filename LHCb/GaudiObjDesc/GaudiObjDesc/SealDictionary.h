// $Header: /afs/cern.ch/project/cvs/reps/lhcb/GaudiObjDesc/GaudiObjDesc/SealDictionary.h,v 1.1 2003-12-17 17:31:16 mato Exp $
//====================================================================
//	SealDictionary.h
//--------------------------------------------------------------------
//
//  Description: Compiler generated SEAL dictionaries
//
//	Author     : M.Frank
//====================================================================
#ifndef GAUDIKERNEL_SEALDICTIONARY_H
#define GAUDIKERNEL_SEALDICTIONARY_H 1

#include "ReflectionBuilder/ReflectionBuilder.h"
#include <typeinfo>
#include <vector>
#include <string>
#include <cstdio>

#define __template_method__ inline
#include "GaudiKernel/SmartRefBase.h"
#include "GaudiKernel/ContainedObject.h"

#define private public
#define protected public
#include "GaudiKernel/SmartRefVector.h"
#include "GaudiKernel/SmartRef.h"
#undef private
#undef protected

namespace GaudiDict {
  enum {     PUBLIC      = seal::reflect::PUBLIC       };
  enum {     PROTECTED   = seal::reflect::PROTECTED    };
  enum {     PRIVATE     = seal::reflect::PRIVATE      };
  enum {     TRANSIENT   = seal::reflect::TRANSIENT    };
  enum {     VIRTUAL     = seal::reflect::VIRTUAL      };
  enum {     STATIC      = seal::reflect::STATIC       };

  typedef const std::vector<void*>& Args;

  std::string typeName(const std::type_info& typ);
  std::string vectorName(const std::type_info& typ);
  std::string vectorName(const std::string& typ);
  std::string pairName(const std::type_info& typ1, const std::type_info& typ2);
  std::string relationName(const std::string& prefix, const std::type_info& typ1, const std::type_info& typ2);
  std::string templateName1(const std::string& templ, const std::type_info& typ);
  std::string templateName1(const std::string& templ, const std::string& typ);
  std::string templateName2(const std::string& templ, const std::type_info& typ1, const std::type_info& typ2);
  std::string templateName3(const std::string& templ, const std::type_info& typ1, const std::type_info& typ2, const std::type_info& typ3);
  std::string keyedContainerName(const std::string& prefix, const std::type_info& typ1);
}

namespace GaudiDict {

  template<class A> struct EmptyDict { 
    EmptyDict(const std::string& n1, const std::string& n2="");
    static void* Ctor (void*);
    static void  Dtor (void* o) { ((A*)o)->~A(); }
  };
}

template<class A> __template_method__ 
GaudiDict::EmptyDict<A>::EmptyDict(const std::string& n1, const std::string& n2) {
  std::string _me_ = n1;
  std::string nn2 = (n2.length()==0) ? n1 : n2;
  if ( 0 == seal::reflect::Class::forName(n1) )  {
    seal::reflect::ClassBuilder _c_(n1.c_str(), "", typeid(A), 
      sizeof(A),std::vector<const std::type_info*>(), 
      false, seal::reflect::NOCONTAINER, 0, PUBLIC | VIRTUAL);
    _c_.addMethod(nn2.c_str(), "", _me_.c_str(), Ctor, PUBLIC);
    _c_.addMethod(std::string("~"+nn2).c_str(), "",   Dtor, PUBLIC | VIRTUAL);
    _c_.build();
  }
}

template<class A> __template_method__ 
void* GaudiDict::EmptyDict<A>::Ctor( void* mem) {  return new(mem) A();   }


#include <utility>
namespace GaudiDict {

  template<class A, class B> struct PairDict { 
    typedef std::pair<A,B> _Me_;
    PairDict();
    static void* Ctor (void*);
    static void* Ctor2(void*, Args);
    static void* Ctor3(void*, Args);
    static void  Dtor (void* o) { ((_Me_*)o)->~pair(); }
  };
}

template<class A, class B> __template_method__ 
GaudiDict::PairDict<A,B>::PairDict() {
  std::string _me_   = templateName2("std::pair", typeid(A),typeid(B));
  if ( 0 == seal::reflect::Class::forName(_me_) )  {
    std::string _frst_ = typeName(typeid(A));
    std::string _scnd_ = typeName(typeid(B));
    seal::reflect::ClassBuilder _c_(_me_.c_str(), "", typeid(_Me_), 
      sizeof(_Me_),std::vector<const std::type_info*>(), 
      false, seal::reflect::NOCONTAINER, 0, PUBLIC);
    _c_.addField("first", _frst_.c_str(), "", (int)(&((_Me_*)64)->first)-64,  PUBLIC);
    _c_.addField("second",_scnd_.c_str(), "", (int)(&((_Me_*)64)->second)-64, PUBLIC);
    _c_.addMethod("pair", "", _me_.c_str(), Ctor,                             PUBLIC);
    _c_.addMethod("pair", "", _me_.c_str(), std::string(_me_+"&").c_str(), Ctor2,            PUBLIC);
    _c_.addMethod("pair", "", _me_.c_str(), std::string(_frst_+"&; "+_scnd_).c_str(), Ctor3, PUBLIC);
    _c_.addMethod("~pair","", Dtor,                                   PUBLIC);
    _c_.build();
  }
}

template<class A, class B> __template_method__ 
void* GaudiDict::PairDict<A,B>::Ctor( void* mem) {
  return new(mem) _Me_();
}

template<class A, class B> __template_method__ 
void* GaudiDict::PairDict<A,B>::Ctor2( void* mem, Args arg) {
  return new(mem) _Me_(*(const _Me_*)arg[0]);
}

template<class A, class B> __template_method__ 
void* GaudiDict::PairDict<A,B>::Ctor3( void* mem, Args arg) {
  return new(mem) _Me_(*(const typename _Me_::first_type*)arg[0], *(const typename _Me_::second_type*)arg[1]);
}

#include <vector>

namespace GaudiDict {

  template<class T> struct VectorDict {
    typedef std::vector<T> _Me_;
    typedef typename _Me_::value_type value_type;

    VectorDict();
    VectorDict(const std::string& name);
    static void* size( void* );
    static void* max_size( void* );
    static void* capacity( void* );
    static void* empty( void* );
    static void* op_Bracket1( void*, Args );
    static void* op_Bracket2( void*, Args );
    static void* at_1( void*, Args );
    static void* at_2( void*, Args );
    static void* constructor_5626( void*, Args );
    static void* sizeInitCtor( void*, Args );
    static void* sizeCtor( void*, Args );
    static void* copyCtor( void*, Args );
    static void  Dtor( void* o ) { ((_Me_*)o)->~vector(); }
    static void* op_equal( void*, Args );
    static void  reserve( void*, Args );
    static void  assign( void*, Args );
    static void* front_1( void* );
    static void* front_2( void* );
    static void* back_1( void* );
    static void* back_2( void* );
    static void  push_back( void*, Args );
    static void  swap( void*, Args );
    static void  pop_back( void* );
    static void  resize_1( void*, Args );
    static void  resize_2( void*, Args );
    static void  clear( void* );
    static void* Ctor( void* );
    static void* writeData( void*, Args ); 
    static void* readData( void*, Args ); 
  };
}

template<class T> __template_method__ 
GaudiDict::VectorDict<T>::VectorDict() {
  std::string _me_ = vectorName(typeid(T));
  if ( 0 == seal::reflect::Class::forName(_me_) )  {
    std::string _t_  = typeName(typeid(T));
    std::string _tr_ = _t_+"&";
    std::string _mr_ = _me_+"&";
    seal::reflect::ClassBuilder _c_(_me_.c_str(), "", typeid(_Me_), sizeof(_Me_),
      std::vector<const std::type_info*>(), 
      false, seal::reflect::VECTOR, 0, 
      PUBLIC);
    _c_.addMethod("size",     "", "unsigned int",         size,         PUBLIC);
    _c_.addMethod("max_size", "", "unsigned int",         max_size,     PUBLIC);
    _c_.addMethod("capacity", "", "unsigned int",         capacity,     PUBLIC);
    _c_.addMethod("empty",    "", "bool",                 empty,        PUBLIC);
    _c_.addMethod("[]",       "", _tr_.c_str(), "unsigned int",  op_Bracket1,PUBLIC);
    _c_.addMethod("[]",       "", _tr_.c_str(), "unsigned int",  op_Bracket2,PUBLIC);
    _c_.addMethod("at",       "", _tr_.c_str(), "unsigned int",  at_1,       PUBLIC);
    _c_.addMethod("at",       "", _tr_.c_str(), "unsigned int",  at_2,       PUBLIC);
    _c_.addMethod("vector",   "", _me_.c_str(),             Ctor,       PUBLIC);
    _c_.addMethod("vector",   "", _me_.c_str(), std::string("unsigned int; "+_tr_).c_str(), sizeInitCtor, PUBLIC);
    _c_.addMethod("vector",   "", _me_.c_str(), "unsigned int",sizeCtor,PUBLIC);
    _c_.addMethod("vector",   "", _me_.c_str(), _mr_.c_str(), copyCtor, PUBLIC);
    _c_.addMethod("~vector",  "", Dtor,                                 PUBLIC);
    _c_.addMethod("equal",    "", _mr_.c_str(), _mr_.c_str(), op_equal, PUBLIC);
    _c_.addMethod("reserve",  "", "unsigned int", reserve,              PUBLIC);
    _c_.addMethod("assign",   "", std::string("unsigned int; "+_tr_).c_str(), assign,     PUBLIC);
    _c_.addMethod("front",    "", _tr_.c_str(), front_1,                PUBLIC);
    _c_.addMethod("front",    "", _tr_.c_str(), front_2,                PUBLIC);
    _c_.addMethod("back",     "", _tr_.c_str(), back_1,                 PUBLIC);
    _c_.addMethod("back",     "", _tr_.c_str(), back_2,                 PUBLIC);
    _c_.addMethod("push_back","", _tr_.c_str(), push_back,              PUBLIC);
    _c_.addMethod("swap",     "", _mr_.c_str(), swap,                   PUBLIC);
    _c_.addMethod("pop_back", "", pop_back,                             PUBLIC);
    _c_.addMethod("resize",   "", std::string("unsigned int; "+_tr_).c_str(), resize_1,   PUBLIC);
    _c_.addMethod("resize",   "", "unsigned int", resize_2,             PUBLIC);
    _c_.addMethod("clear",    "", clear,                                PUBLIC);
    _c_.addMethod("writeData","", "void*", "std::vector<void*>", writeData);
    _c_.addMethod("readData", "", "void*", "std::vector<void*>", readData);
    _c_.build();
  }
}

template<class T> __template_method__ 
GaudiDict::VectorDict<T>::VectorDict(const std::string& name) {
  std::string _me_ = vectorName(name);
  if ( 0 == seal::reflect::Class::forName(_me_) )  {
    std::string _t_  = typeName(typeid(T));
    std::string _tr_ = _t_+"&";
    std::string _mr_ = _me_+"&";
    seal::reflect::ClassBuilder _c_(_me_.c_str(), "", typeid(_Me_), sizeof(_Me_),
      std::vector<const std::type_info*>(), 
      false, seal::reflect::VECTOR, 0, 
      PUBLIC);
    _c_.addMethod("size",     "", "unsigned int",         size,         PUBLIC);
    _c_.addMethod("max_size", "", "unsigned int",         max_size,     PUBLIC);
    _c_.addMethod("capacity", "", "unsigned int",         capacity,     PUBLIC);
    _c_.addMethod("empty",    "", "bool",                 empty,        PUBLIC);
    _c_.addMethod("[]",       "", _tr_.c_str(), "unsigned int",  op_Bracket1,PUBLIC);
    _c_.addMethod("[]",       "", _tr_.c_str(), "unsigned int",  op_Bracket2,PUBLIC);
    _c_.addMethod("at",       "", _tr_.c_str(), "unsigned int",  at_1,       PUBLIC);
    _c_.addMethod("at",       "", _tr_.c_str(), "unsigned int",  at_2,       PUBLIC);
    _c_.addMethod("vector",   "", _me_.c_str(),             Ctor,       PUBLIC);
    _c_.addMethod("vector",   "", _me_.c_str(), std::string("unsigned int; "+_tr_).c_str(), sizeInitCtor, PUBLIC);
    _c_.addMethod("vector",   "", _me_.c_str(), "unsigned int",sizeCtor,PUBLIC);
    _c_.addMethod("vector",   "", _me_.c_str(), _mr_.c_str(), copyCtor, PUBLIC);
    _c_.addMethod("~vector",  "", Dtor,                                 PUBLIC);
    _c_.addMethod("equal",    "", _mr_.c_str(), _mr_.c_str(), op_equal, PUBLIC);
    _c_.addMethod("reserve",  "", "unsigned int", reserve,              PUBLIC);
    _c_.addMethod("assign",   "", std::string("unsigned int; "+_tr_).c_str(), assign,     PUBLIC);
    _c_.addMethod("front",    "", _tr_.c_str(), front_1,                PUBLIC);
    _c_.addMethod("front",    "", _tr_.c_str(), front_2,                PUBLIC);
    _c_.addMethod("back",     "", _tr_.c_str(), back_1,                 PUBLIC);
    _c_.addMethod("back",     "", _tr_.c_str(), back_2,                 PUBLIC);
    _c_.addMethod("push_back","", _tr_.c_str(), push_back,              PUBLIC);
    _c_.addMethod("swap",     "", _mr_.c_str(), swap,                   PUBLIC);
    _c_.addMethod("pop_back", "", pop_back,                             PUBLIC);
    _c_.addMethod("resize",   "", std::string("unsigned int; "+_tr_).c_str(), resize_1,   PUBLIC);
    _c_.addMethod("resize",   "", "unsigned int", resize_2,             PUBLIC);
    _c_.addMethod("clear",    "", clear,                                PUBLIC);
    _c_.addMethod("writeData","", "void*", "std::vector<void*>", writeData);
    _c_.addMethod("readData", "", "void*", "std::vector<void*>", readData);
    _c_.build();
  }
}

template<class T> __template_method__ 
void* GaudiDict::VectorDict<T>::size( void* o)    {
  static unsigned int ret;
  ret = ((_Me_*)o)->size();
  return &ret;
}

template<class T> __template_method__ 
void* GaudiDict::VectorDict<T>::max_size( void* o){
  static unsigned int ret;
  ret = ((_Me_*)o)->max_size();
  return &ret;
}

template<class T> __template_method__ 
void* GaudiDict::VectorDict<T>::capacity( void* o){
  static unsigned int ret;
  ret = ((_Me_*)o)->capacity();
  return &ret;
}

template<class T> __template_method__ 
void* GaudiDict::VectorDict<T>::empty( void* o){
  static bool ret;
  ret = ((_Me_*)o)->empty();
  return &ret;
}

template<class T> __template_method__ 
void* GaudiDict::VectorDict<T>::op_Bracket1( void* o, Args arg)
{  return (void*)&((_Me_*)o)->operator [](*(unsigned int*)arg[0]);}

template<class T> __template_method__ 
void* GaudiDict::VectorDict<T>::op_Bracket2( void* o, Args arg)
{  return (void*)&((_Me_*)o)->operator [](*(unsigned int*)arg[0]);}

template<class T> __template_method__ 
void* GaudiDict::VectorDict<T>::at_1( void* o, Args arg)
{  return (void*)&((_Me_*)o)->at(*(unsigned int*)arg[0]);}

template<class T> __template_method__ 
void* GaudiDict::VectorDict<T>::at_2( void* o, Args arg)
{  return (void*)&((_Me_*)o)->at(*(unsigned int*)arg[0]);}

template<class T> __template_method__ 
void* GaudiDict::VectorDict<T>::sizeInitCtor( void* mem, Args arg) {
  if ( arg.size() == 2 ) {
    return new(mem) _Me_(*(unsigned int*)arg[0],*(const value_type*)arg[1]);
  }
  return 0;
}

template<class T> __template_method__ 
void* GaudiDict::VectorDict<T>::sizeCtor( void* mem, Args arg) {
  return new(mem) _Me_(*(unsigned int*)arg[0]);
}

template<class T> __template_method__ 
void* GaudiDict::VectorDict<T>::copyCtor( void* mem, Args arg) {
  return new(mem) _Me_(*(const _Me_*)arg[0]);
}

template<class T> __template_method__ 
void* GaudiDict::VectorDict<T>::op_equal( void* o, Args arg)
{  return (void*)&((_Me_*)o)->operator =(*(const _Me_*)arg[0]);                 }

template<class T> __template_method__ 
void GaudiDict::VectorDict<T>::reserve( void* o, Args arg)
{  ((_Me_*)o)->reserve(*(unsigned int*)arg[0]);                                }

template<class T> __template_method__ 
void GaudiDict::VectorDict<T>::assign( void* o, Args arg)
{  ((_Me_*)o)->assign(*(unsigned int*)arg[0],*(const value_type*)arg[1]);      }

template<class T> __template_method__ 
void* GaudiDict::VectorDict<T>::front_1( void* o)
{  return (void*)&((_Me_*)o)->front();                                         }

template<class T> __template_method__ 
void* GaudiDict::VectorDict<T>::front_2( void* o)
{  return (void*)&((_Me_*)o)->front();                                         }

template<class T> __template_method__ 
void* GaudiDict::VectorDict<T>::back_1( void* o)
{  return (void*)&((_Me_*)o)->back();                                          }

template<class T> __template_method__ 
void* GaudiDict::VectorDict<T>::back_2( void* o)
{  return (void*)&((_Me_*)o)->back();                                          }

template<class T> __template_method__ 
void GaudiDict::VectorDict<T>::push_back( void* o, Args arg)
{  ((_Me_*)o)->push_back(*(const value_type*)arg[0]);                          }

template<class T> __template_method__ 
void GaudiDict::VectorDict<T>::swap( void* o, Args arg)
{  ((_Me_*)o)->swap(*(_Me_*)arg[0]);                                            }

template<class T> __template_method__ 
void GaudiDict::VectorDict<T>::pop_back( void* o)
{  ((_Me_*)o)->pop_back();                                                     }

template<class T> __template_method__ 
void GaudiDict::VectorDict<T>::resize_1( void* o, Args arg)
{  ((_Me_*)o)->resize(*(unsigned int*)arg[0], *(const value_type*)arg[1]);     }

template<class T> __template_method__ 
void GaudiDict::VectorDict<T>::resize_2( void* o, Args arg)
{  ((_Me_*)o)->resize(*(unsigned int*)arg[0]);                                 }

template<class T> __template_method__ 
void GaudiDict::VectorDict<T>::clear( void* o)
{  ((_Me_*)o)->clear();                                                        }

template<class T> __template_method__ 
void* GaudiDict::VectorDict<T>::Ctor( void* mem) 
{  return new(mem) _Me_();                                                     }

template<class T> __template_method__ 
void* GaudiDict::VectorDict<T>::writeData( void* o, Args args)  {
  _Me_* v = (_Me_*)o;
  size_t n = v->size();
  ((void (*)(size_t&, Args))args[1])(n, args);
  for(typename _Me_::iterator i = v->begin(); i != v->end(); i++) {
    ((void (*)(const void*, Args))args[2])(&(*i), args);
  }
  return 0;
}

template<class T> __template_method__ 
void* GaudiDict::VectorDict<T>::readData( void* o, Args args) {
  _Me_* v = (_Me_*)o;
  size_t n = *(size_t*)args[1];
  if (n > 0) {
    for(size_t i = 0; i < n; i++) {
      typename _Me_::value_type o;
      ((void (*)(const void*, Args))args[2])(&o, args);
      v->push_back(o);
    }
  }
  return 0;
}

namespace GaudiDict  {
  template <class T> struct SmartRefDict { 
    typedef SmartRef<T> _Me;
    SmartRefDict();
    static int to_SmartRefBase(void*);
    static void* Ctor   (void* );
    static void  Dtor   (void* o) { ((_Me*)o)->~SmartRef(); }
    static void* hintID (void* );
    static void* linkID (void* );
    static void* type   (void* );
    static void* data   (void* );
    static void* ptr    (void* );
    static void* equals (void*, Args );
    static void* non_equals( void*, Args );
  };
}

template <class T> __template_method__ 
GaudiDict::SmartRefDict<T>::SmartRefDict() {
  std::string _me_ = templateName1("SmartRef", typeid(T));
  if ( 0 == seal::reflect::Class::forName(_me_) )  {
    std::string _t_  = typeName(typeid(T));
    seal::reflect::ClassBuilder _c_(_me_.c_str(), "", typeid(_Me), sizeof(_Me), 
      std::vector<const std::type_info*>(), false, seal::reflect::NOCONTAINER, 0, PUBLIC);
    _c_.addSuperClass("SmartRefBase", 0, to_SmartRefBase);
    _c_.addField("m_target",    std::string(_t_+"*").c_str(), "", (int)(&((_Me*)64)->m_target)-64, PROTECTED | TRANSIENT );
    _c_.addMethod("SmartRef",   "", _me_.c_str(),   Ctor,               PUBLIC);
    _c_.addMethod("~SmartRef",  "",         Dtor,               PUBLIC);
    _c_.addMethod("hintID",     "", "long", hintID,             PUBLIC);
    _c_.addMethod("linkID",     "", "long", linkID,             PUBLIC);
    _c_.addMethod("type",       "", "std::type_info*", type,    PUBLIC);
    _c_.addMethod("data",       "", std::string(_t_+"*").c_str(),  data,             PUBLIC);
    _c_.addMethod("ptr",        "", std::string(_t_+"*").c_str(),  ptr,              PUBLIC);
    _c_.addMethod("equals",     "", "bool", std::string(_me_+"&").c_str(),equals,    PUBLIC);
    _c_.addMethod("non_equals", "", "bool", std::string(_me_+"&").c_str(),non_equals,PUBLIC);
    _c_.build();
  }
}

template <class T> __template_method__ 
int GaudiDict::SmartRefDict<T>::to_SmartRefBase(void* o ) {
  return (int)(SmartRefBase*)(_Me*)o - (int)(_Me*)o;
}

template <class T> __template_method__ 
void* GaudiDict::SmartRefDict<T>::Ctor(void* mem) {
  return new(mem) _Me();
}

template <class T> __template_method__ 
void* GaudiDict::SmartRefDict<T>::hintID(void* o) {
  static long ret;
  ret = ((_Me*)o)->hintID();
  return &ret;
}

template <class T> __template_method__ 
void* GaudiDict::SmartRefDict<T>::linkID(void* o)   {
  static long ret;
  ret = ((_Me*)o)->linkID();
  return &ret;
}

template <class T> __template_method__ 
void* GaudiDict::SmartRefDict<T>::type(void* o)   {
  return (void*)((_Me*)o)->type();
}

template <class T> __template_method__
void* GaudiDict::SmartRefDict<T>::equals(void* o, Args arg)  {
  static bool ret;
  ret = ((_Me*)o)->operator ==(*(const _Me*)arg[0]);
  return &ret;
}

template <class T> __template_method__
void* GaudiDict::SmartRefDict<T>::non_equals(void* o, Args arg)  {
  static bool ret;
  ret = ((_Me*)o)->operator !=(*(const _Me*)arg[0]);
  return &ret;
}

template <class T> __template_method__ 
void* GaudiDict::SmartRefDict<T>::ptr(void* o)  {
  return (void*)((_Me*)o)->operator->();
}

template <class T> __template_method__ 
void* GaudiDict::SmartRefDict<T>::data(void* o)  {
  return (void*)((_Me*)o)->data();
}


namespace GaudiDict  {

  template <class T> struct SmartRefVectorDict { 
    typedef SmartRefVector<T> _Me_;
    SmartRefVectorDict();
    static int   to_vector(void*);
    static void* Ctor( void* );
    static void* copyCtor( void*, Args );
    static void  Dtor( void* o ) { ((_Me_*)o)->~SmartRefVector(); }
    static void* type( void* );
  };
}

template <class T> __template_method__
GaudiDict::SmartRefVectorDict<T>::SmartRefVectorDict() {
  std::string _me_ = templateName1("SmartRefVector", typeid(T));
  std::string _o_  = templateName1("SmartRef", typeid(T));
  std::string _t_  = vectorName(typeid(SmartRef<T>));

  if ( 0 == seal::reflect::Class::forName(_t_) )  {
    VectorDict<SmartRef<T> >();
  }
  if ( 0 == seal::reflect::Class::forName(_o_) )  {
    SmartRefVectorDict<T>();
  }
  if ( 0 == seal::reflect::Class::forName(_me_) )  {
    std::string _t_  = vectorName(typeid(SmartRef<T>));
    seal::reflect::ClassBuilder _c_(_me_.c_str(), "", typeid(_Me_), sizeof(_Me_),
      std::vector<const std::type_info*>(), 
      false, seal::reflect::NOCONTAINER, 0, 
      PUBLIC);
    _c_.addSuperClass(_t_.c_str(), 0, to_vector);
    _c_.addField("m_data", "DataObject*", "", (int)(&((_Me_*)64)->m_data)-64, 
      PROTECTED | TRANSIENT);
    _c_.addField("m_contd", "ContainedObject*", "", (int)(&((_Me_*)64)->m_contd)-64, 
      PROTECTED | TRANSIENT);
    _c_.addMethod("~SmartRefVector", "", Dtor,                    PUBLIC);
    _c_.addMethod("SmartRefVector", "", _me_.c_str(), Ctor,               PUBLIC);
    _c_.addMethod("SmartRefVector", "", _me_.c_str(), std::string(_me_+"&").c_str(), copyCtor, PUBLIC);
    _c_.addMethod("type", "", "std::type_info*",        type,     PUBLIC);
    _c_.build();
  }
}

template <class T> __template_method__
int GaudiDict::SmartRefVectorDict<T>::to_vector(void* o ) {
  return (int)(std::vector<SmartRef<T> >*)(_Me_*)o - (int)(_Me_*)o;
}

template <class T> __template_method__
void* GaudiDict::SmartRefVectorDict<T>::Ctor(void* mem) {
  return new(mem) SmartRefVector<T>();
}

template <class T> __template_method__
void* GaudiDict::SmartRefVectorDict<T>::copyCtor(void* mem, Args arg) {
  return new(mem) SmartRefVector<T>(*(const _Me_*)arg[0]);
}

template <class T> __template_method__
void* GaudiDict::SmartRefVectorDict<T>::type(void* o) {
  return (void*)((_Me_*)o)->type();
}


#include "GaudiKernel/ObjectContainerBase.h"
#define private public
#define protected public
#include "GaudiKernel/ObjectVector.h"
#undef private
#undef protected

namespace GaudiDict {

  template <class T> struct ObjectVectorDict { 
    typedef ObjectVector<T>           _Me_;
    typedef std::vector<T*>           _Cnt_;
    typedef typename _Cnt_::iterator  _It_;
    ObjectVectorDict();
    static int to_base(void*);
    static void* Ctor( void* );
    static void* copyCtor( void*, Args );
    static void  destructor( void* o ) { ((_Me_*)o)->~ObjectVector(); }
    static void* clID( void* );
    static void* classID( void* );
    static void* equal( void*, Args );
    static void* begin( void* );
    static void* end( void* );
    static void* size( void* );
    static void* numberOfObjects( void* );
    static void* max_size( void* );
    static void* capacity( void* );
    static void  reserve( void*, Args );
    static void* empty( void* );
    static void* front( void* );
    static void* back( void* );
    static void  push_back( void*, Args );
    static void* add( void*, Args );
    static void  pop_back( void* );
    static void* remove( void*, Args );
    static void* insert( void*, Args );
    static void  erase( void*, Args );
    static void  eraseRange( void*, Args );
    static void  clear( void* );
    static void* at( void*, Args );
    static void* index( void*, Args );
    static void* containedObject( void*, Args );
  };
}

template <class T> __template_method__
GaudiDict::ObjectVectorDict<T>::ObjectVectorDict() {
  std::string _me_ = typeName(typeid(ObjectVector<T>));
  if ( 0 == seal::reflect::Class::forName(_me_) )  {
    static char clid_txt[64];
    std::string _v_  = vectorName(typeid(T*));
    std::string _t_  = vectorName(typeid(T));
    std::string _tr_ = _t_+"&";
    std::string _mr_ = _me_+"&";
    seal::reflect::ClassBuilder _c_(_me_.c_str(), "", typeid(_Me_), sizeof(_Me_),
      std::vector<const std::type_info*>(), false, 
      seal::reflect::NOCONTAINER, 0, 
      PUBLIC | VIRTUAL);
    _c_.addSuperClass("ObjectContainerBase", 0, to_base);
    _c_.addField("m_vector", _v_.c_str(), "", (int)(&((_Me_*)64)->m_vector)-64, seal::reflect::PRIVATE );
    _c_.addMethod("ObjectVector", "", _me_.c_str(),                   Ctor,       PUBLIC);
    _c_.addMethod("ObjectVector", "", _me_.c_str(), _mr_.c_str(),         copyCtor,   PUBLIC);
    _c_.addMethod("~ObjectVector","",                         destructor, PUBLIC | VIRTUAL);
    _c_.addMethod("clID",         "", "long unsigned int&",   clID,       PUBLIC | VIRTUAL);
    _c_.addMethod("classID",      "", "long unsigned int&",   classID,    PUBLIC | STATIC);
    _c_.addMethod("equal",        "", _mr_.c_str(), _mr_.c_str(),     equal,      PUBLIC);
    _c_.addMethod("begin",        "", std::string(_v_+"::iterator").c_str(),       begin,      PUBLIC);
    _c_.addMethod("end",          "", std::string(_v_+"::const_iterator").c_str(), end,        PUBLIC);
    _c_.addMethod("size",         "", "unsigned int",         size,       PUBLIC);
    _c_.addMethod("numberOfObjects", "", "long",              numberOfObjects, PUBLIC | VIRTUAL);
    _c_.addMethod("max_size",     "", "unsigned int",         max_size,   PUBLIC);
    _c_.addMethod("capacity",     "", "unsigned int",         capacity,   PUBLIC);
    _c_.addMethod("reserve",      "", "unsigned int",         reserve,    PUBLIC);
    _c_.addMethod("empty",        "", "bool",                 empty,      PUBLIC);
    _c_.addMethod("front",        "", std::string(_t_+"*&").c_str(),               front,      PUBLIC);
    _c_.addMethod("back",         "", std::string(_t_+"*&").c_str(),               back,       PUBLIC);
    _c_.addMethod("push_back",    "", std::string(_t_+"*&").c_str(),               push_back,  PUBLIC);
    _c_.addMethod("add",          "", "long", "ContainedObject*", add,    PUBLIC | VIRTUAL);
    _c_.addMethod("pop_back",     "",                         pop_back,   PUBLIC);
    _c_.addMethod("remove",       "", "long", "ContainedObject*", remove, PUBLIC | VIRTUAL);
    _c_.addMethod("insert",       "", std::string(_v_+"::iterator").c_str(), std::string(_v_+"::iterator; "+_t_+"*&").c_str(), insert, PUBLIC);
    _c_.addMethod("erase",        "", std::string(_v_+"::iterator").c_str(),       erase,      PUBLIC);
    _c_.addMethod("erase",        "", std::string(_v_+"::iterator; "+_v_+"::iterator").c_str(),eraseRange, PUBLIC);
    _c_.addMethod("clear",        "",                         clear,      PUBLIC);
    _c_.addMethod("at",           "", std::string(_t_+"*&").c_str(), "unsigned int", at,       PUBLIC);
    _c_.addMethod("index",        "", "long", "ContainedObject*", index,  PUBLIC | VIRTUAL);
    _c_.addMethod("containedObject", "", "ContainedObject*", "long", containedObject, PUBLIC | VIRTUAL);
    sprintf(clid_txt,"%08X-0000-0000-0000-000000000000",(int)_Me_::classID());
    _c_.addProperty("ClassID", (char*)clid_txt);
    _c_.build();
  }
}

template <class T> __template_method__
int GaudiDict::ObjectVectorDict<T>::to_base( void* o ) {
  return (int)(ObjectContainerBase*)(_Me_*)o - (int)(_Me_*)o;
}

template <class T> __template_method__
void* GaudiDict::ObjectVectorDict<T>::Ctor( void* mem) {
  return new(mem) _Me_();
}

template <class T> __template_method__
void* GaudiDict::ObjectVectorDict<T>::copyCtor( void* mem, Args arg) {
  return new(mem) _Me_(*(const _Me_*)arg[0]);
}

template <class T> __template_method__
void* GaudiDict::ObjectVectorDict<T>::clID( void* o)   {
  return (void*)&((_Me_*)o)->clID();
}

template <class T> __template_method__
void* GaudiDict::ObjectVectorDict<T>::classID( void* o)  {
  return (void*)&((_Me_*)o)->classID();
}

template <class T> __template_method__
void* GaudiDict::ObjectVectorDict<T>::equal( void* o, Args arg) {
  return (void*)&((_Me_*)o)->operator =(*(const _Me_*)arg[0]);
}

template <class T> __template_method__
void* GaudiDict::ObjectVectorDict<T>::begin( void* o)    {
  return new _It_(((_Me_*)o)->begin());
}

template <class T> __template_method__
void* GaudiDict::ObjectVectorDict<T>::end( void* o)    {
  return new typename _Cnt_::const_iterator(((_Me_*)o)->end());
}

template <class T> __template_method__
void* GaudiDict::ObjectVectorDict<T>::size( void* o)   {
  static unsigned int ret;
  ret = ((_Me_*)o)->size();
  return &ret;
}

template <class T> __template_method__
void* GaudiDict::ObjectVectorDict<T>::numberOfObjects( void* o)  {
  static long ret;
  ret = ((_Me_*)o)->numberOfObjects();
  return &ret;
}

template <class T> __template_method__
void* GaudiDict::ObjectVectorDict<T>::max_size( void* o)   {
  static unsigned int ret;
  ret = ((_Me_*)o)->max_size();
  return &ret;
}

template <class T> __template_method__
void* GaudiDict::ObjectVectorDict<T>::capacity( void* o)   {
  static unsigned int ret;
  ret = ((_Me_*)o)->capacity();
  return &ret;
}

template <class T> __template_method__
void GaudiDict::ObjectVectorDict<T>::reserve( void* o, Args arg)  {
  ((_Me_*)o)->reserve(*(unsigned int*)arg[0]);
}

template <class T> __template_method__
void* GaudiDict::ObjectVectorDict<T>::empty( void* o)  {
  static bool ret;
  ret = ((_Me_*)o)->empty();
  return &ret;
}

template <class T> __template_method__
void* GaudiDict::ObjectVectorDict<T>::front( void* o)  {
  return (void*)&((_Me_*)o)->front();
}

template <class T> __template_method__
void* GaudiDict::ObjectVectorDict<T>::back( void* o) {
  return (void*)&((_Me_*)o)->back();
}

template <class T> __template_method__
void GaudiDict::ObjectVectorDict<T>::push_back( void* o, Args arg)  {
  ((_Me_*)o)->push_back(*(T* const*)arg[0]);
}

template <class T> __template_method__
void* GaudiDict::ObjectVectorDict<T>::add( void* o, Args arg)   {
  static long ret;
  ret = ((_Me_*)o)->add((ContainedObject*)arg[0]);
  return &ret;
}

template <class T> __template_method__
void GaudiDict::ObjectVectorDict<T>::pop_back( void* o)    {
  ((_Me_*)o)->pop_back();
}

template <class T> __template_method__
void* GaudiDict::ObjectVectorDict<T>::remove( void* o, Args arg)  {
  static long ret;
  ret = ((_Me_*)o)->remove((ContainedObject*)arg[0]);
  return &ret;
}

template <class T> __template_method__
void* GaudiDict::ObjectVectorDict<T>::insert( void* o, Args arg)  {
  return new _It_(((_Me_*)o)->insert(*(_It_*)arg[0], *(T* const*)arg[1]));
}

template <class T> __template_method__
void GaudiDict::ObjectVectorDict<T>::erase( void* o, Args arg)  {
  ((_Me_*)o)->erase(*(_It_*)arg[0]);
}

template <class T> __template_method__
void GaudiDict::ObjectVectorDict<T>::eraseRange( void* o, Args arg) {
  ((_Me_*)o)->erase(*(_It_*)arg[0], *(_It_*)arg[1]);
}

template <class T> __template_method__
void GaudiDict::ObjectVectorDict<T>::clear( void* o) {
  ((_Me_*)o)->clear();
}

template <class T> __template_method__
void* GaudiDict::ObjectVectorDict<T>::at( void* o, Args arg)  {
  return (void*)&((_Me_*)o)->operator [](*(unsigned int*)arg[0]);
}

template <class T> __template_method__
void* GaudiDict::ObjectVectorDict<T>::index( void* o, Args arg) {
  static long ret;
  ret = ((_Me_*)o)->index((const ContainedObject*)arg[0]);
  return &ret;
}

template <class T> __template_method__
void* GaudiDict::ObjectVectorDict<T>::containedObject( void* o, Args arg) {
  return (void*)((_Me_*)o)->containedObject(*(long*)arg[0]);
}

namespace GaudiDict  {

  /** @class ContainedObjectDictionaries
    * 
    *  Helper class to generate all dictionaries for contained
    *  object derived classes in one step.
    * 
    *   @author  M.Frank
    *   @version 1.0
    */
  template<class T> struct ContainedObjectDictionaries  {
    ContainedObjectDictionaries()    {
      VectorDict<T*>();
      SmartRefDict<T>();
      VectorDict<SmartRef<T> >();
      SmartRefVectorDict<T>();
      ObjectVectorDict<T>();
    }
  };

  /** @class DataObjectDictionaries
    * 
    *  Helper class to generate all dictionaries for data
    *  object derived classes in one step.
    * 
    *   @author  M.Frank
    *   @version 1.0
    */
  template<class T> struct DataObjectDictionaries  {
    DataObjectDictionaries()    {
      SmartRefDict<T>();
      VectorDict<SmartRef<T> >();
      SmartRefVectorDict<T>();
    }
  };
}
#endif // GAUDIKERNEL_SEALDICTIONARY_H
