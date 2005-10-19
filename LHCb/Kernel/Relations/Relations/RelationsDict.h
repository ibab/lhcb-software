// $Id: RelationsDict.h,v 1.4 2005-10-19 12:50:54 cattanem Exp $
//====================================================================
//	RelationsDict.h
//--------------------------------------------------------------------
//
//  Description: Compiler generated SEAL relation table dictionaries
//
//	Author     : M.Frank
//====================================================================
#ifndef EVENT_RELATIONDICT_H
#define EVENT_RELATIONDICT_H 1
#include <iostream>
#include "GaudiKernel/DataObject.h"
#include "Reflex/Builder/ReflexBuilder.h"

namespace seal { namespace reflect {}}

#define __template_method__ inline

#define private public
#define protected public
#include "GaudiKernel/SmartRefVector.h"
#include "GaudiKernel/SmartRef.h"
#undef private
#undef protected

namespace GaudiDict {

  static int PUBLIC      = seal::reflex::PUBLIC;
  static int PRIVATE     = seal::reflex::PRIVATE;
  static int TRANSIENT   = seal::reflex::TRANSIENT;
  static int VIRTUAL     = seal::reflex::VIRTUAL;
  static int CLASS       = seal::reflex::CLASS;
  static int CONSTRUCTOR = seal::reflex::CONSTRUCTOR;
  static int DESTRUCTOR  = seal::reflex::DESTRUCTOR;

  typedef const std::vector<void*>& Args;
  using seal::reflex::Type;
  using seal::reflex::TypeBuilder;
  using seal::reflex::FunctionTypeBuilder;
  using seal::reflex::ConstBuilder;
  using seal::reflex::PointerBuilder;
  using seal::reflex::ClassBuilderImpl;

  class CBImpl {
  public:
    ClassBuilderImpl m_impl;
  };

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

  inline seal::reflex::Type void_func() {
    static seal::reflex::Type t = FunctionTypeBuilder(TypeBuilder("void"));
    return t;
  }
  inline seal::reflex::Type voids_func() {
    static seal::reflex::Type t = FunctionTypeBuilder(PointerBuilder(TypeBuilder("void")));
    return t;
  }
  inline seal::reflex::Type func_type(const std::string& s) {
    return seal::reflex::FunctionTypeBuilder (seal::reflex::TypeBuilder(s.c_str()));
  }
  inline seal::reflex::Type type(const std::string& s) {
    return seal::reflex::TypeBuilder(s.c_str());
  }
  seal::reflex::Type pointer(const std::string& s) {
    return seal::reflex::PointerBuilder(type(s));
  }

  using seal::reflex::Base;
  using seal::reflex::baseOffset;
}

#include <utility>
namespace GaudiDict {

  template<class A, class B> struct PairDict { 
    typedef std::pair<A,B> _Me_;
    PairDict();
    static void* Ctor (void*, Args, void*);
    static void* Ctor2(void*, Args, void*);
    static void* Ctor3(void*, Args, void*);
    static void* Dtor (void* o, Args, void*) { ((_Me_*)o)->~pair(); return 0; }
  };
}

template<class A, class B> __template_method__ 
GaudiDict::PairDict<A,B>::PairDict() {
  std::string _me_   = templateName2("std::pair", typeid(A),typeid(B));
  if ( !Type::byName(_me_) )  {
    Type _frst_ = type(typeName(typeid(A)));
    Type _scnd_ = type(typeName(typeid(B)));
    Type void_typ  = type("void");
    Type myref_typ = ReferenceBuilder(type(_me_));
    seal::reflex::ClassBuilder<_Me_>(_me_.c_str(),PUBLIC|CLASS)
      .addDataMember(_frst_, "first",  OffsetOf(_Me_, first),  PUBLIC)
      .addDataMember(_scnd_, "second", OffsetOf(_Me_, second), PUBLIC)
      .addFunctionMember(FunctionTypeBuilder(void_typ),               "pair", Ctor,  0,0,PUBLIC|CONSTRUCTOR)
      .addFunctionMember(FunctionTypeBuilder(void_typ,myref_typ),     "pair", Ctor2, 0,0,PUBLIC|CONSTRUCTOR)
      .addFunctionMember(FunctionTypeBuilder(void_typ,_frst_,_scnd_), "pair", Ctor3, 0,0,PUBLIC|CONSTRUCTOR)
      .addFunctionMember(FunctionTypeBuilder(void_typ),              "~pair", Dtor,  0,0,PUBLIC|DESTRUCTOR);
  }
}

template<class A, class B> __template_method__ 
void* GaudiDict::PairDict<A,B>::Ctor( void* mem, Args, void*) {
  return new(mem) _Me_();
}

template<class A, class B> __template_method__ 
void* GaudiDict::PairDict<A,B>::Ctor2( void* mem, Args arg, void*) {
  return new(mem) _Me_(*(const _Me_*)arg[0]);
}

template<class A, class B> __template_method__ 
void* GaudiDict::PairDict<A,B>::Ctor3( void* mem, Args arg, void*) {
  return new(mem) _Me_(*(const typename _Me_::first_type*)arg[0], *(const typename _Me_::second_type*)arg[1]);
}


#include <vector>

namespace GaudiDict {

  template<class T> struct VectorDict {
    typedef std::vector<T> _Me_;
    typedef typename _Me_::value_type value_type;

    VectorDict();
    VectorDict(const std::string& name);
    void init(const std::string& name);
    static void* size( void*, Args, void* );
    static void* max_size( void*, Args, void* );
    static void* capacity( void*, Args, void* );
    static void* empty( void*, Args, void* );
    static void* op_Bracket1( void*, Args, void* );
    static void* op_Bracket2( void*, Args, void* );
    static void* at_1( void*, Args, void* );
    static void* at_2( void*, Args, void* );
    static void* sizeInitCtor( void*, Args, void* );
    static void* sizeCtor( void*, Args, void* );
    static void* copyCtor( void*, Args, void* );
    static void* Dtor( void* o, Args, void* ) { ((_Me_*)o)->~vector(); return 0; }
    static void* op_equal( void*, Args, void* );
    static void* reserve( void*, Args, void* );
    static void* assign( void*, Args, void* );
    static void* front_1( void*, Args, void* );
    static void* front_2( void*, Args, void* );
    static void* back_1( void*, Args, void* );
    static void* back_2( void*, Args, void* );
    static void* push_back( void*, Args, void* );
    static void* swap( void*, Args, void* );
    static void* pop_back( void*, Args, void* );
    static void* resize_1( void*, Args, void* );
    static void* resize_2( void*, Args, void* );
    static void* clear( void*, Args, void* );
    static void* Ctor( void*, Args, void* );
    static void* createCollFuncTable(void*, Args, void*);
  };
}

template<class T> __template_method__ 
GaudiDict::VectorDict<T>::VectorDict() {
  std::string _me_ = vectorName(typeid(T));
  init(_me_);
}

template<class T> __template_method__ 
GaudiDict::VectorDict<T>::VectorDict(const std::string& name) {
  std::string _me_ = vectorName(name);
  init(_me_);
}

template<class T> __template_method__ 
void GaudiDict::VectorDict<T>::init(const std::string& _me_) {
  if ( 0 == Type::byName(_me_) )  {
    std::string _t_  = typeName(typeid(T));
    std::string _tr_ = _t_+"&";
    std::string _mr_ = _me_+"&";
    Type uint_typ  = type("unsigned int");
    Type vref_typ  = ReferenceBuilder(type(_t_));
    Type vcref_typ = ConstBuilder(vref_typ);
    Type void_typ  = type("void");
    Type myref_typ = ReferenceBuilder(type(_me_));
    Type uint_fun  = func_type("unsigned int");
    Type vref_fun  = func_type(_tr_);
    Type vcref_fun = FunctionTypeBuilder(vref_typ);
    Type void_fcn  = void_func();
    //std::cout << "Creating dictionary for " << _me_ << std::endl;
    seal::reflex::ClassBuilder<_Me_>(_me_.c_str(),PUBLIC|CLASS)
      .addFunctionMember(void_fcn,                                         "vector",   Ctor,        0,0,PUBLIC|CONSTRUCTOR)
      .addFunctionMember(FunctionTypeBuilder(void_typ,uint_typ),           "vector",   sizeCtor,    0,0,PUBLIC|CONSTRUCTOR)
      .addFunctionMember(FunctionTypeBuilder(void_typ,uint_typ,vcref_typ), "vector",   sizeInitCtor,0,0,PUBLIC|CONSTRUCTOR)
      .addFunctionMember(FunctionTypeBuilder(void_typ,myref_typ),          "vector",   copyCtor,    0,0,PUBLIC|CONSTRUCTOR)
      .addFunctionMember(void_fcn,                                         "~vector",  Dtor,        0,0,PUBLIC|DESTRUCTOR)
      .addFunctionMember(uint_fun,                                         "size",     size,        0,0,PUBLIC)
      .addFunctionMember(uint_fun,                                         "max_size", max_size,    0,0,PUBLIC)
      .addFunctionMember(uint_fun,                                         "capacity", capacity,    0,0,PUBLIC)
      .addFunctionMember(FunctionTypeBuilder(myref_typ,myref_typ),         "equal",    op_equal,    0,0,PUBLIC)
      .addFunctionMember(FunctionTypeBuilder(void_typ,uint_typ),           "reserve",  reserve,     0,0,PUBLIC)
      .addFunctionMember(FunctionTypeBuilder(void_typ,uint_typ, vref_typ), "assign",   assign,      0,0,PUBLIC)
      .addFunctionMember(FunctionTypeBuilder(type("bool")),                "empty",    empty,       0,0,PUBLIC)
      .addFunctionMember(FunctionTypeBuilder(vcref_typ,uint_typ),          "[]",       op_Bracket1, 0,0,PUBLIC)
      .addFunctionMember(FunctionTypeBuilder(vref_typ,uint_typ),           "[]",       op_Bracket2, 0,0,PUBLIC)
      .addFunctionMember(FunctionTypeBuilder(vcref_typ,uint_typ),          "at",       at_1,        0,0,PUBLIC)
      .addFunctionMember(FunctionTypeBuilder(vref_typ,uint_typ),           "at",       at_2,        0,0,PUBLIC)
      .addFunctionMember(vcref_fun,                                        "front",    front_1,     0,0,PUBLIC)
      .addFunctionMember(vref_fun,                                         "front",    front_2,     0,0,PUBLIC)
      .addFunctionMember(vcref_fun,                                        "back",     back_1,      0,0,PUBLIC)
      .addFunctionMember(vref_fun,                                         "back",     back_2,      0,0,PUBLIC)
      .addFunctionMember(FunctionTypeBuilder(void_typ,vref_typ),           "push_back",push_back,   0,0,PUBLIC)
      .addFunctionMember(FunctionTypeBuilder(void_typ, myref_typ),         "swap",     swap,        0,0,PUBLIC)
      .addFunctionMember(void_fcn,                                         "pop_back", pop_back,    0,0,PUBLIC)
      .addFunctionMember(FunctionTypeBuilder(void_typ,uint_typ, vref_typ), "resize",   resize_1,    0,0,PUBLIC)
      .addFunctionMember(FunctionTypeBuilder(void_typ,uint_typ),           "resize",   resize_2,    0,0,PUBLIC)
      .addFunctionMember(void_fcn,                                         "clear",    clear,       0,0,PUBLIC)
      .addFunctionMember(voids_func(),"createCollFuncTable", createCollFuncTable,0,0,PUBLIC);
  }
}

template<class T> __template_method__ 
void* GaudiDict::VectorDict<T>::size( void* o, Args, void*)    {
  static unsigned int ret;
  ret = ((_Me_*)o)->size();
  return &ret;
}

template<class T> __template_method__ 
void* GaudiDict::VectorDict<T>::max_size( void* o, Args, void*){
  static unsigned int ret;
  ret = ((_Me_*)o)->max_size();
  return &ret;
}

template<class T> __template_method__ 
void* GaudiDict::VectorDict<T>::capacity( void* o, Args, void*){
  static unsigned int ret;
  ret = ((_Me_*)o)->capacity();
  return &ret;
}

template<class T> __template_method__ 
void* GaudiDict::VectorDict<T>::empty( void* o, Args, void*){
  static bool ret;
  ret = ((_Me_*)o)->empty();
  return &ret;
}

template<class T> __template_method__ 
void* GaudiDict::VectorDict<T>::op_Bracket1( void* o, Args arg, void*)
{  return (void*)&((_Me_*)o)->operator [](*(unsigned int*)arg[0]);}

template<class T> __template_method__ 
void* GaudiDict::VectorDict<T>::op_Bracket2( void* o, Args arg, void*)
{  return (void*)&((_Me_*)o)->operator [](*(unsigned int*)arg[0]);}

template<class T> __template_method__ 
void* GaudiDict::VectorDict<T>::at_1( void* o, Args arg, void*)
{  return (void*)&((_Me_*)o)->at(*(unsigned int*)arg[0]);}

template<class T> __template_method__ 
void* GaudiDict::VectorDict<T>::at_2( void* o, Args arg, void*)
{  return (void*)&((_Me_*)o)->at(*(unsigned int*)arg[0]);}

template<class T> __template_method__ 
void* GaudiDict::VectorDict<T>::sizeInitCtor( void* mem, Args arg, void*) {
  if ( arg.size() == 2 ) {
    return new(mem) _Me_(*(unsigned int*)arg[0],*(const value_type*)arg[1]);
  }
  return 0;
}

template<class T> __template_method__ 
void* GaudiDict::VectorDict<T>::sizeCtor( void* mem, Args arg, void*) {
  return new(mem) _Me_(*(unsigned int*)arg[0]);
}

template<class T> __template_method__ 
void* GaudiDict::VectorDict<T>::copyCtor( void* mem, Args arg, void*) {
  return new(mem) _Me_(*(const _Me_*)arg[0]);
}

template<class T> __template_method__ 
void* GaudiDict::VectorDict<T>::op_equal( void* o, Args arg, void*)
{  return (void*)&((_Me_*)o)->operator =(*(const _Me_*)arg[0]);                 }

template<class T> __template_method__ 
void* GaudiDict::VectorDict<T>::reserve( void* o, Args arg, void*)
{  ((_Me_*)o)->reserve(*(unsigned int*)arg[0]);      return 0;                          }

template<class T> __template_method__ 
void* GaudiDict::VectorDict<T>::assign( void* o, Args arg, void*)
{  ((_Me_*)o)->assign(*(unsigned int*)arg[0],*(const value_type*)arg[1]); return 0;     }

template<class T> __template_method__ 
void* GaudiDict::VectorDict<T>::front_1( void* o, Args, void*)
{  return (void*)&((_Me_*)o)->front();                                         }

template<class T> __template_method__ 
void* GaudiDict::VectorDict<T>::front_2( void* o, Args, void*)
{  return (void*)&((_Me_*)o)->front();                                         }

template<class T> __template_method__ 
void* GaudiDict::VectorDict<T>::back_1( void* o, Args, void*)
{  return (void*)&((_Me_*)o)->back();                                          }

template<class T> __template_method__ 
void* GaudiDict::VectorDict<T>::back_2( void* o, Args, void*)
{  return (void*)&((_Me_*)o)->back();                                          }

template<class T> __template_method__ 
void* GaudiDict::VectorDict<T>::push_back( void* o, Args arg, void*)
{  ((_Me_*)o)->push_back(*(const value_type*)arg[0]);   return 0;              }

template<class T> __template_method__ 
void* GaudiDict::VectorDict<T>::swap( void* o, Args arg, void*)
{  ((_Me_*)o)->swap(*(_Me_*)arg[0]);     return 0;                              }

template<class T> __template_method__ 
void* GaudiDict::VectorDict<T>::pop_back( void* o, Args, void*)
{  ((_Me_*)o)->pop_back();      return 0;                                      }

template<class T> __template_method__ 
void* GaudiDict::VectorDict<T>::resize_1( void* o, Args arg, void*)
{  ((_Me_*)o)->resize(*(unsigned int*)arg[0], *(const value_type*)arg[1]);  return 0; }

template<class T> __template_method__ 
void* GaudiDict::VectorDict<T>::resize_2( void* o, Args arg, void*)
{  ((_Me_*)o)->resize(*(unsigned int*)arg[0]);  return 0;                      }

template<class T> __template_method__ 
void* GaudiDict::VectorDict<T>::clear( void* o, Args, void*)
{  ((_Me_*)o)->clear();      return 0;                                         }

template<class T> __template_method__ 
void* GaudiDict::VectorDict<T>::Ctor( void* mem, Args, void*) 
{  return new(mem) _Me_();                                                     }

template<class T> __template_method__ 
void* GaudiDict::VectorDict<T>::createCollFuncTable( void*, Args, void* ) {
  return seal::reflex::Proxy<_Me_ >::generate();
}

#ifndef private
#define private public
#endif
#ifndef protected
#define protected public
#endif
#include "Relations/Relation1D.h"
#include "Relations/RelationWeighted1D.h"
#include "Relations/Relation2D.h"
#include "Relations/RelationWeighted2D.h"
#undef private
#undef public

namespace GaudiDict  {

  /** @class RelationTypeTraitsDict
    *
    *   @author   M.Frank
    *   @version  1.0
    */
  template <class FROM, class TO> struct RelationTypeTraitsDict { 
    /// Type definition of the dictionary class
    typedef typename Relations::RelationTypeTraits<FROM,TO> _Me_;
    /// Standard Constructor
    RelationTypeTraitsDict();
    /// Static function to execute object destructor
    static void* dtor( void* o, Args, void* ) { ((_Me_*)o)->~Entry(); return 0; }
    /// Static function to execute object constructor
    static void* ctor( void*, Args, void* );
  };
}


template <class FROM, class TO>
GaudiDict::RelationTypeTraitsDict<FROM,TO>::RelationTypeTraitsDict() {
  std::string _me_ = templateName2("Relations::RelationTypeTraits", typeid(FROM),typeid(TO));
  if ( !(Type::byName(_me_)) )  {
    //std::cout << "Creating dictionary for " << _me_ << std::endl;
    seal::reflex::ClassBuilder<_Me_>(_me_.c_str(), PUBLIC|CLASS|VIRTUAL);
  }
}

namespace GaudiDict  {

  /** @class RelationTypeTraitsEntryDict
    *
    *   @author   M.Frank
    *   @version  1.0
    */
  template <class FROM, class TO> struct RelationTypeTraitsEntryDict { 
    /// Type definition of the dictionary class
    typedef typename Relations::RelationTypeTraits<FROM,TO>::Entry _Me_;
    /// Standard Constructor
    RelationTypeTraitsEntryDict();
    /// Static function to execute object destructor
    static void* dtor( void* o, Args, void* ) { ((_Me_*)o)->~Entry(); return 0; }
    /// Static function to execute object constructor
    static void* ctor( void*, Args, void* );
  };
}

template <class FROM, class TO>
GaudiDict::RelationTypeTraitsEntryDict<FROM,TO>::RelationTypeTraitsEntryDict() {
  RelationTypeTraitsDict<FROM,TO>();
  typedef typename Relations::RelationTypeTraits<FROM,TO>::From  _From_;
  typedef typename Relations::RelationTypeTraits<FROM,TO>::To    _To_;
  std::string _me_ = templateName2("Relations::RelationTypeTraits", typeid(FROM),typeid(TO));
  _me_ += "::Entry";
  if ( !Type::byName(_me_) )  {
    //std::cout << "Creating dictionary for " << _me_ << std::endl;
    seal::reflex::ClassBuilder<_Me_>(_me_.c_str(),PUBLIC|CLASS)
      .addDataMember(type(typeName(typeid(_From_))), "_from" , OffsetOf(_Me_, first), PUBLIC)
      .addDataMember(type(typeName(typeid(_To_))),   "_to" ,   OffsetOf(_Me_, second), PUBLIC)
      .addFunctionMember(void_func(), "Entry", ctor, 0, 0, PUBLIC|CONSTRUCTOR)
      .addFunctionMember(void_func(), "~Entry",dtor, 0, 0, PUBLIC|DESTRUCTOR );
  }
}

template <class FROM, class TO>
void* GaudiDict::RelationTypeTraitsEntryDict<FROM,TO>::ctor(void* mem, Args, void*) {
  return new(mem) _Me_();
}

namespace GaudiDict {

  /** @class RelationBaseDict
    *
    *   @author   M.Frank
    *   @version  1.0
    */
  template <class FROM, class TO> struct RelationBaseDict { 
    /// Type definition of the dictionary class
    typedef Relations::RelationBase<FROM,TO> _Me_;
    /// Standard Constructor
    RelationBaseDict();
    /// Static function to execute object destructor
    static void* dtor( void* o, Args, void* ) { ((_Me_*)o)->~RelationBase(); return 0;}
    /// Static function to execute object constructor
    static void* ctor( void*, Args, void* );
  };
}

template <class FROM, class TO> 
GaudiDict::RelationBaseDict<FROM,TO>::RelationBaseDict() {
  RelationTypeTraitsDict<FROM,TO>();
  RelationTypeTraitsEntryDict<FROM,TO>();
  VectorDict<typename Relations::RelationTypeTraits<FROM, TO>::Entry >();

  std::string _me_ = templateName2("Relations::RelationBase", typeid(FROM),typeid(TO));
  if ( !Type::byName(_me_) )  {
    //std::cout << "Creating dictionary for " << _me_ << std::endl;
    std::string m = "std::vector<"+templateName2("Relations::RelationTypeTraits", typeid(FROM),typeid(TO))+"::Entry>";
    seal::reflex::ClassBuilder<_Me_>(_me_.c_str(),PUBLIC|CLASS)
      .addDataMember(type(m), "m_entries",OffsetOf(_Me_,m_entries), PUBLIC)
      .addFunctionMember(void_func(),"RelationBase", ctor, 0, 0, PUBLIC|CONSTRUCTOR)
      .addFunctionMember(void_func(),"~RelationBase",dtor,0,0,PUBLIC|DESTRUCTOR);
  }
}

template <class FROM, class TO> 
void* GaudiDict::RelationBaseDict<FROM,TO>::ctor( void* mem, Args, void*) {
  return new(mem) _Me_();
}

namespace GaudiDict {

  /** @class RelationDict
    *
    *   @author   M.Frank
    *   @version  1.0
    */
  template <class FROM, class TO> struct RelationDict { 
    /// Type definition of the dictionary class
    typedef Relations::Relation<FROM,TO> _Me_;
    /// Standard Constructor
    RelationDict();
    /// Static function to execute object destructor
    static void* dtor( void* o, Args, void* ) { ((_Me_*)o)->~Relation(); return 0; }
    /// Static function to execute object constructor
    static void* ctor( void*, Args, void* );
  };
}

template <class FROM, class TO> 
GaudiDict::RelationDict<FROM,TO>::RelationDict() {
  std::string _me_ = templateName2("Relations::Relation", typeid(FROM),typeid(TO));
  RelationBaseDict<FROM,TO>();
  if ( !seal::reflex::Type::byName(_me_) )  {
    //std::cout << "Creating dictionary for " << _me_ << std::endl;
    std::string b = templateName2("Relations::RelationBase", typeid(FROM),typeid(TO));
    seal::reflex::ClassBuilder<_Me_>(_me_.c_str(), PUBLIC|CLASS|VIRTUAL)
      .addDataMember(type(b),   "m_direct", OffsetOf(_Me_,m_direct), PRIVATE)
      .addDataMember(pointer(b),"m_inverse",OffsetOf(_Me_,m_inverse), PRIVATE|TRANSIENT)
      .addFunctionMember(void_func(), "Relation",ctor,0,0, PUBLIC|CONSTRUCTOR)
      .addFunctionMember(void_func(),"~Relation",dtor,0,0,PUBLIC|DESTRUCTOR|VIRTUAL);
  }
}

template <class FROM, class TO> 
void* GaudiDict::RelationDict<FROM,TO>::ctor( void* mem, Args, void*) {
  return new(mem) _Me_();
}

namespace GaudiDict {

  /** @class Relation1DDict
    *
    *   @author   M.Frank
    *   @version  1.0
    */
  template <class FROM, class TO> struct Relation1DDict { 
    /// Type definition of the dictionary class
    typedef Relation1D<FROM,TO> _Me_;
    /// Standard Constructor
    Relation1DDict();
    /// Static function to execute object destructor
    static void* dtor( void* o, Args, void* ) { ((_Me_*)o)->~Relation1D(); return 0; }
    /// Static function to execute object constructor
    static void* ctor( void*,Args,void* );
    /// Access table of bases
    static void* getBases( void*, Args, void* ); 
  };
}

template <class FROM, class TO>
GaudiDict::Relation1DDict<FROM,TO>::Relation1DDict() {
  std::string _me_ = templateName2("Relation1D", typeid(FROM),typeid(TO));
  RelationDict<FROM,TO>();
  if ( !Type::byName(_me_) )  {
    static char clid_txt[64];
    sprintf(clid_txt,"%08X-0000-0000-0000-000000000000",(int)_Me_::classID());
    std::cout << "Creating dictionary for " << _me_ << " GUID=" << clid_txt << std::endl;
    std::string b = templateName2("Relations::Relation", typeid(FROM),typeid(TO));
    seal::reflex::ClassBuilder<_Me_> c(_me_.c_str(),CLASS|VIRTUAL|PUBLIC);
    c//.addBase< ::DataObject >(PUBLIC)
      .addProperty("ClassID",clid_txt)
      .addDataMember(type(b),"m_base",OffsetOf(_Me_,m_base),  PRIVATE)
      .addFunctionMember(void_func(), "Relation1D",ctor,0,0,PUBLIC|CONSTRUCTOR)
      .addFunctionMember(void_func(),"~Relation1D",dtor,0,0,PUBLIC|DESTRUCTOR|VIRTUAL)
      .addFunctionMember(voids_func(),"getBasesTable", getBases, 0, 0, PUBLIC);
    std::cout << "Creating bases for " << _me_ << " GUID=" << clid_txt << std::endl;
    ClassBuilderImpl* imp = &((CBImpl*)&c)->m_impl;
    imp->addBase(type("DataObject"),seal::reflex::baseOffset<_Me_,DataObject>::get(),PUBLIC);
  }
}

template <class FROM, class TO> 
void* GaudiDict::Relation1DDict<FROM,TO>::ctor(void* mem, Args, void*) {
  return new(mem) _Me_();
}
template <class FROM, class TO> 
void* GaudiDict::Relation1DDict<FROM,TO>::getBases(void*, Args, void*) {
  static std::vector<std::pair<Base, int> > s_bases;
  if ( s_bases.size() == 0 ) {
    s_bases.push_back(std::make_pair(Base(type("DataObject"),seal::reflex::baseOffset<_Me_,DataObject>::get(),PUBLIC),0));
  }
  return &s_bases;
}

// =========================================================================

namespace GaudiDict  {

  /** @class RelationWeightedTypeTraitsDict
    *
    *   @author   M.Frank
    *   @version  1.0
    */
  template <class FROM, class TO, class WEIGHT> struct RelationWeightedTypeTraitsDict { 
    /// Type definition of the dictionary class
    typedef typename Relations::RelationWeightedTypeTraits<FROM,TO,WEIGHT> _Me_;
    /// Standard Constructor
    RelationWeightedTypeTraitsDict();
  };
}

template <class FROM, class TO, class WEIGHT>
GaudiDict::RelationWeightedTypeTraitsDict<FROM,TO,WEIGHT>::RelationWeightedTypeTraitsDict() {
  std::string _me_ = templateName3("Relations::RelationWeightedTypeTraits", typeid(FROM),typeid(TO),typeid(WEIGHT));
  if ( !Type::byName(_me_) )  {
    // std::cout << "Creating dictionary for " << _me_ << std::endl;
    seal::reflex::ClassBuilder<_Me_> (_me_.c_str(),PUBLIC|CLASS);
  }
}

namespace GaudiDict  {

  /** @class RelationWeightedTypeTraitsEntryDict
    *
    *   @author   M.Frank
    *   @version  1.0
    */
  template <class FROM, class TO, class WEIGHT> struct RelationWeightedTypeTraitsEntryDict { 
    /// Type definition of the dictionary class
    typedef typename Relations::RelationWeightedTypeTraits<FROM,TO,WEIGHT>::Entry _Me_;
    /// Standard Constructor
    RelationWeightedTypeTraitsEntryDict();
    /// Static function to execute object destructor
    static void* dtor( void* o, Args, void* ) { ((_Me_*)o)->~Entry(); return 0; }
    /// Static function to execute object constructor
    static void* ctor( void*, Args, void* );
  };
}

template <class FROM, class TO, class WEIGHT>
GaudiDict::RelationWeightedTypeTraitsEntryDict<FROM,TO,WEIGHT>::RelationWeightedTypeTraitsEntryDict() {
  RelationWeightedTypeTraitsDict<FROM,TO,WEIGHT>();
  typedef typename Relations::RelationWeightedTypeTraits<FROM,TO,WEIGHT>::From    _From_;
  typedef typename Relations::RelationWeightedTypeTraits<FROM,TO,WEIGHT>::To      _To_;
  typedef typename Relations::RelationWeightedTypeTraits<FROM,TO,WEIGHT>::Weight  _Weight_;
  std::string _me_ = templateName3("Relations::RelationWeightedTypeTraits", typeid(FROM),typeid(TO),typeid(WEIGHT));
  _me_ += "::Entry";
  if ( !Type::byName(_me_) )  {
    // std::cout << "Creating dictionary for " << _me_ << std::endl;
    seal::reflex::ClassBuilder<_Me_>(_me_.c_str(),PUBLIC|CLASS)
      .addDataMember(type(typeName(typeid(_From_))),  "_from",   OffsetOf(_Me_,first.first), PUBLIC)
      .addDataMember(type(typeName(typeid(_Weight_))),"_weight", OffsetOf(_Me_,first.second), PUBLIC)
      .addDataMember(type(typeName(typeid(_To_))),    "_to",     OffsetOf(_Me_,second), PUBLIC)
      .addFunctionMember(void_func(),"Entry", ctor, 0, 0, PUBLIC|CONSTRUCTOR)
      .addFunctionMember(void_func(),"~Entry",dtor,0,0,PUBLIC|DESTRUCTOR);
  }
}

template <class FROM, class TO, class WEIGHT>
void* GaudiDict::RelationWeightedTypeTraitsEntryDict<FROM,TO,WEIGHT>::ctor(void* mem, Args, void*) {
  return new(mem) _Me_();
}

namespace GaudiDict {
  /** @class RelationWeightedBaseDict
    *
    *   @author   M.Frank
    *   @version  1.0
    */
  template <class FROM, class TO, class WEIGHT> struct RelationWeightedBaseDict { 
    /// Type definition of the dictionary class
    typedef Relations::RelationWeightedBase<FROM,TO,WEIGHT> _Me_;
    /// Standard Constructor
    RelationWeightedBaseDict();
    /// Static function to execute object destructor
    static void* dtor( void* o, Args, void* ) { ((_Me_*)o)->~RelationWeightedBase(); return 0;}
    /// Static function to execute object constructor
    static void* ctor( void*, Args, void* );
  };
}

template <class FROM, class TO, class WEIGHT> 
GaudiDict::RelationWeightedBaseDict<FROM,TO,WEIGHT>::RelationWeightedBaseDict() {
  RelationWeightedTypeTraitsDict<FROM,TO,WEIGHT>();
  RelationWeightedTypeTraitsEntryDict<FROM,TO,WEIGHT>();
  VectorDict<typename Relations::RelationWeightedTypeTraits<FROM,TO,WEIGHT>::Entry >();
  std::string _me_ = templateName3("Relations::RelationWeightedBase", typeid(FROM),typeid(TO),typeid(WEIGHT));
  if ( !Type::byName(_me_) )  {
    //std::cout << "Creating dictionary for " << _me_ << std::endl;
    std::string _v_ = "std::vector<"+templateName3("Relations::RelationWeightedTypeTraits", typeid(FROM),typeid(TO),typeid(WEIGHT));
    _v_ += "::Entry>";
    seal::reflex::ClassBuilder<_Me_>(_me_.c_str(), CLASS|PUBLIC|VIRTUAL)
      .addDataMember(type(_v_), "m_entries",OffsetOf(_Me_,m_entries), PRIVATE)
      .addFunctionMember(void_func(),"RelationWeightedBase", ctor, 0, 0, PUBLIC|CONSTRUCTOR)
      .addFunctionMember(void_func(),"~RelationWeightedBase",dtor,0,0,PUBLIC|DESTRUCTOR);
  }
}

template <class FROM, class TO, class WEIGHT> 
void* GaudiDict::RelationWeightedBaseDict<FROM,TO,WEIGHT>::ctor(void* mem, Args, void*) {
  return new(mem) _Me_();
}

namespace GaudiDict {

  /** @class RelationWeightedDict
    *
    *   @author   M.Frank
    *   @version  1.0
    */
  template <class FROM, class TO, class WEIGHT> struct RelationWeightedDict { 
    /// Type definition of the dictionary class
    typedef Relations::RelationWeighted<FROM,TO,WEIGHT> _Me_;
    /// Standard Constructor
    RelationWeightedDict();
    /// Static function to execute object destructor
    static void* dtor( void* o, Args, void* ) { ((_Me_*)o)->~RelationWeighted(); return 0;}
    /// Static function to execute object constructor
    static void* ctor( void*, Args, void* );
  };
}

template <class FROM, class TO, class WEIGHT> 
GaudiDict::RelationWeightedDict<FROM,TO,WEIGHT>::RelationWeightedDict() {
  std::string _me_ = templateName3("Relations::RelationWeighted", typeid(FROM),typeid(TO),typeid(WEIGHT));
  RelationWeightedBaseDict<FROM,TO,WEIGHT>();
  if ( !Type::byName(_me_) )  {
    std::string  _b_ = templateName3("Relations::RelationWeightedBase", typeid(FROM),typeid(TO),typeid(WEIGHT));
    std::cout << "Creating dictionary for " << _me_ << std::endl;
    seal::reflex::ClassBuilder<_Me_>(_me_.c_str(), PUBLIC|CLASS|VIRTUAL)
      .addDataMember(type(_b_), "m_direct", OffsetOf(_Me_,m_direct), PRIVATE)
      .addDataMember(pointer(_b_), "m_inverse",OffsetOf(_Me_,m_inverse), PRIVATE|TRANSIENT)
      .addFunctionMember(void_func(),"RelationWeighted", ctor, 0, 0, PUBLIC|CONSTRUCTOR)
      .addFunctionMember(void_func(),"~RelationWeighted",dtor,0,0,PUBLIC|DESTRUCTOR|VIRTUAL);
  }
}

template <class FROM, class TO, class WEIGHT> 
void* GaudiDict::RelationWeightedDict<FROM,TO,WEIGHT>::ctor( void* mem, Args, void*) {
  return new(mem) _Me_();
}

namespace GaudiDict {
  /** @class RelationWeighted1DDict
    *
    *   @author   M.Frank
    *   @version  1.0
    */
  template <class FROM, class TO, class WEIGHT> struct RelationWeighted1DDict { 
    /// Type definition of the dictionary class
    typedef RelationWeighted1D<FROM,TO,WEIGHT> _Me_;
    /// Standard Constructor
    RelationWeighted1DDict();
    /// Static function to execute destructor
    static void* dtor( void* o, Args, void* ) { ((_Me_*)o)->~RelationWeighted1D(); return 0;}
    /// Static function to execute object constructor
    static void* ctor( void*, Args, void* );
    /// Access table of bases
    static void* getBases( void*, Args, void* ); 
  };
}

template <class FROM, class TO, class WEIGHT>
GaudiDict::RelationWeighted1DDict<FROM,TO,WEIGHT>::RelationWeighted1DDict() {
  std::string _me_ = templateName3("RelationWeighted1D", typeid(FROM),typeid(TO),typeid(WEIGHT));
  RelationWeightedDict<FROM,TO,WEIGHT>();
  if ( 0 == seal::reflex::Type::byName(_me_) )  {
    static char clid_txt[64];
    std::string m = templateName3("Relations::RelationWeighted", typeid(FROM),typeid(TO),typeid(WEIGHT));
    sprintf(clid_txt,"%08X-0000-0000-0000-000000000000",(int)_Me_::classID());
    std::cout << "Creating dictionary for " << _me_ << " GUID=" << clid_txt << std::endl;
    seal::reflex::ClassBuilder<_Me_> c(_me_.c_str(),CLASS|VIRTUAL|PUBLIC);
      c//.addBase< ::DataObject >(PUBLIC)
      .addProperty("ClassID", clid_txt)
      .addDataMember(type(m), "m_base",OffsetOf(_Me_,m_base), PRIVATE)
      .addFunctionMember(void_func(), "RelationWeighted1D", ctor, 0, 0, PUBLIC|CONSTRUCTOR)
      .addFunctionMember(void_func(), "~RelationWeighted1D",dtor,0,0,PUBLIC|DESTRUCTOR|VIRTUAL)
      .addFunctionMember(voids_func(),"getBasesTable", getBases,0,0,PUBLIC);
    std::cout << "Instaniated classbuilder for " << _me_ << " GUID=" << clid_txt << std::endl;
    ClassBuilderImpl* imp = &((CBImpl*)&c)->m_impl;
    imp->addBase(type("DataObject"),seal::reflex::baseOffset<_Me_,DataObject>::get(),PUBLIC);
  }
}

template <class FROM, class TO, class WEIGHT> 
void* GaudiDict::RelationWeighted1DDict<FROM,TO,WEIGHT>::ctor( void* mem, Args, void*) {
  return new(mem) _Me_();
}
template <class FROM, class TO, class WEIGHT> 
void* GaudiDict::RelationWeighted1DDict<FROM,TO,WEIGHT>::getBases(void*,Args, void*) {
  static std::vector<std::pair<Base, int> > s_bases;
  if ( s_bases.size() == 0 ) {
    s_bases.push_back(std::make_pair(Base(type("DataObject"),seal::reflex::baseOffset<_Me_,DataObject>::get(),PUBLIC),0));
  }
  return &s_bases;
}

// =========================================================================
//
//        2 D   Relations
//
// =========================================================================

namespace GaudiDict {

  /** @class Relation2Dict
    *
    *   @author   M.Frank
    *   @version  1.0
    */
  template <class FROM, class TO> struct Relation2Dict { 
    /// Type definition of the dictionary class
    typedef Relations::Relation2<FROM,TO> _Me_;
    /// Standard Constructor
    Relation2Dict();
    /// Static function to execute object destructor
    static void* dtor( void* o, Args, void* ) { ((_Me_*)o)->~Relation2(); return 0; }
    /// Static function to execute object constructor
    static void* ctor( void*, Args, void* );
  };
}

template <class FROM, class TO> 
GaudiDict::Relation2Dict<FROM,TO>::Relation2Dict() {
  std::string _me_ = templateName2("Relations::Relation2", typeid(FROM),typeid(TO));
  RelationDict<FROM,TO>();
  RelationDict<TO,FROM>();
  if ( !Type::byName(_me_) )  {
    std::cout << "Creating dictionary for " << _me_ << std::endl;
    std::string  _p_ = templateName2("Relations::Relation", typeid(FROM),typeid(TO));
    std::string  _q_ = templateName2("Relations::Relation", typeid(TO),typeid(FROM));
    seal::reflex::ClassBuilder<_Me_>(_me_.c_str(), PUBLIC|CLASS|VIRTUAL)
      .addDataMember(type(_p_),    "m_direct", OffsetOf(_Me_,m_direct), PRIVATE)
      .addDataMember(pointer(_q_), "m_inverse",OffsetOf(_Me_,m_inverse), PRIVATE)
      .addFunctionMember(void_func(),"Relation2", ctor, 0, 0, PUBLIC|CONSTRUCTOR)
      .addFunctionMember(void_func(),"~Relation2",dtor,0,0,PUBLIC|DESTRUCTOR|VIRTUAL);
  }
}

template <class FROM, class TO> 
void* GaudiDict::Relation2Dict<FROM,TO>::ctor( void* mem, Args, void*) {
  return new(mem) _Me_();
}


namespace GaudiDict {

  /** @class Relation2DDict
    *
    *   @author   M.Frank
    *   @version  1.0
    */
  template <class FROM, class TO> struct Relation2DDict { 
    /// Type definition of the dictionary class
    typedef Relation2D<FROM,TO> _Me_;
    /// Standard Constructor
    Relation2DDict();
    /// Static function to execute object destructor
    static void* dtor( void* o, Args, void* ) { ((_Me_*)o)->~Relation2D(); return 0;}
    /// Static function to execute object constructor
    static void* ctor( void*, Args, void* );
    /// Access table of bases
    static void* getBases( void*, Args, void* ); 
  };
}

template <class FROM, class TO>
GaudiDict::Relation2DDict<FROM,TO>::Relation2DDict() {
  std::string _me_ = templateName2("Relation2D", typeid(FROM),typeid(TO));
  Relation2Dict<FROM,TO>();
  if ( !Type::byName(_me_) )  {
    static char clid_txt[64];
    sprintf(clid_txt,"%08X-0000-0000-0000-000000000000",(int)_Me_::classID());
    std::cout << "Creating dictionary for " << _me_ << " GUID=" << clid_txt << std::endl;
    std::string b = templateName2("Relations::Relation2", typeid(FROM),typeid(TO));
    seal::reflex::ClassBuilder<_Me_> c(_me_.c_str(),CLASS|VIRTUAL|PUBLIC);
      c//.addBase< ::DataObject >(PUBLIC)
      .addProperty("ClassID", (char*)clid_txt)
      .addDataMember(type(b), "m_base",OffsetOf(_Me_,m_base), PRIVATE)
      .addFunctionMember(void_func(),"Relation2D", ctor, 0, 0, PUBLIC|CONSTRUCTOR)
      .addFunctionMember(void_func(),"~Relation2D",dtor,0,0,PUBLIC|DESTRUCTOR|VIRTUAL)
      .addFunctionMember(voids_func(), "getBasesTable", getBases,0,0,PUBLIC);
    ClassBuilderImpl* imp = &((CBImpl*)&c)->m_impl;
    imp->addBase(type("DataObject"),seal::reflex::baseOffset<_Me_,DataObject>::get(),PUBLIC);
  }
}

template <class FROM, class TO> 
void* GaudiDict::Relation2DDict<FROM,TO>::ctor( void* mem, Args, void*) {
  return new(mem) _Me_();
}
template <class FROM, class TO> 
void* GaudiDict::Relation2DDict<FROM,TO>::getBases(void*,Args, void*) {
  static std::vector<std::pair<Base, int> > s_bases;
  if ( s_bases.size() == 0 ) {
    s_bases.push_back(std::make_pair(Base(type("DataObject"),seal::reflex::baseOffset<_Me_,DataObject>::get(),PUBLIC),0));
  }
  return &s_bases;
}

// =========================================================================
//
//        2 D   Weighted relations
//
// =========================================================================

namespace GaudiDict {

  /** @class Relation2WeightedDict
    *
    *   @author   M.Frank
    *   @version  1.0
    */
  template <class FROM, class TO, class WEIGHT> struct Relation2WeightedDict { 
    /// Type definition of the dictionary class
    typedef Relations::Relation2Weighted<FROM,TO,WEIGHT> _Me_;
    /// Standard Constructor
    Relation2WeightedDict();
    /// Static function to execute object destructor
    static void* dtor( void* o, Args, void* ) { ((_Me_*)o)->~Relation2Weighted(); return 0; }
    /// Static function to execute object constructor
    static void* ctor( void*, Args, void* );
  };
}

template <class FROM, class TO, class WEIGHT> 
GaudiDict::Relation2WeightedDict<FROM,TO,WEIGHT>::Relation2WeightedDict() {
  std::string _me_ = templateName3("Relations::Relation2Weighted", typeid(FROM),typeid(TO),typeid(WEIGHT));
  RelationWeightedDict<FROM,TO,WEIGHT>();
  RelationWeightedDict<TO,FROM,WEIGHT>();
  if ( !Type::byName(_me_) )  {
    std::cout << "Creating dictionary for " << _me_ << std::endl;
    std::string  _p_ = templateName3("Relations::RelationWeighted",typeid(FROM),typeid(TO),typeid(WEIGHT));
    std::string  _q_ = templateName3("Relations::RelationWeighted",typeid(TO),typeid(FROM),typeid(WEIGHT));
    seal::reflex::ClassBuilder<_Me_>(_me_.c_str(), PUBLIC|CLASS|VIRTUAL)
      .addDataMember(type(_p_),"m_direct", OffsetOf(_Me_,m_direct), PRIVATE)
      .addDataMember(type(_q_),"m_inverse",OffsetOf(_Me_,m_inverse),PRIVATE)
      .addFunctionMember(void_func(),"Relation2Weighted", ctor,0,0,PUBLIC|CONSTRUCTOR)
      .addFunctionMember(void_func(),"~Relation2Weighted",dtor,0,0,PUBLIC|DESTRUCTOR|VIRTUAL);
  }
}

template <class FROM, class TO, class WEIGHT> 
void* GaudiDict::Relation2WeightedDict<FROM,TO,WEIGHT>::ctor( void* mem, Args, void*) {
  return new(mem) _Me_();
}

namespace GaudiDict {

  /** @class RelationWeighted2DDict
    *
    *   @author   M.Frank
    *   @version  1.0
    */
  template <class FROM, class TO, class WEIGHT> struct RelationWeighted2DDict { 
    /// Type definition of the dictionary class
    typedef RelationWeighted2D<FROM,TO,WEIGHT> _Me_;
    /// Standard Constructor
    RelationWeighted2DDict();
    /// Static function to execute destructor
    static void* dtor( void* o, Args, void* ) { ((_Me_*)o)->~RelationWeighted2D(); return 0;}
    /// Static function to execute object constructor
    static void* ctor( void*, Args, void* );
    /// Access table of bases
    static void* getBases( void*, Args, void* ); 
  };
}

template <class FROM, class TO, class WEIGHT>
GaudiDict::RelationWeighted2DDict<FROM,TO,WEIGHT>::RelationWeighted2DDict() {
  std::string _me_ = templateName3("RelationWeighted2D", typeid(FROM), typeid(TO), typeid(WEIGHT));
  Relation2WeightedDict<FROM,TO,WEIGHT>();
  if ( !Type::byName(_me_) )  {
    static char clid_txt[64];
    sprintf(clid_txt,"%08X-0000-0000-0000-000000000000",(int)_Me_::classID());
    std::cout << "Creating dictionary for " << _me_ << " GUID=" << clid_txt << std::endl;
    std::string b = templateName3("Relations::Relation2Weighted", typeid(FROM), typeid(TO), typeid(WEIGHT));
    seal::reflex::ClassBuilder<_Me_> c(_me_.c_str(),CLASS|VIRTUAL|PUBLIC);
      c//.addBase< ::DataObject >(PUBLIC)
      .addProperty("ClassID", (char*)clid_txt)
      .addDataMember(type(b), "m_base",OffsetOf(_Me_,m_base), PRIVATE)
      .addFunctionMember(void_func(),"RelationWeighted2D", ctor, 0, 0, PUBLIC|CONSTRUCTOR)
      .addFunctionMember(void_func(),"~RelationWeighted2D",dtor,0,0,PUBLIC|DESTRUCTOR|VIRTUAL)
      .addFunctionMember(voids_func(),"getBasesTable", getBases,0,0,PUBLIC);
    ClassBuilderImpl* imp = &((CBImpl*)&c)->m_impl;
    imp->addBase(type("DataObject"),seal::reflex::baseOffset<_Me_,DataObject>::get(),PUBLIC);
  }
}

template <class FROM, class TO, class WEIGHT> 
void* GaudiDict::RelationWeighted2DDict<FROM,TO,WEIGHT>::ctor( void* mem, Args, void*) {
  return new(mem) _Me_();
}

template <class FROM, class TO, class WEIGHT> 
void* GaudiDict::RelationWeighted2DDict<FROM,TO,WEIGHT>::getBases(void*,Args, void*) {
  static std::vector<std::pair<Base, int> > s_bases;
  if ( s_bases.size() == 0 ) {
    s_bases.push_back(std::make_pair(Base(type("DataObject"),seal::reflex::baseOffset<_Me_,DataObject>::get(),PUBLIC),0));
  }
  return &s_bases;
}
#endif // EVENT_RELATIONDICT_H
