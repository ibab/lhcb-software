// $Header: /afs/cern.ch/project/cvs/reps/lhcb/GaudiObjDesc/GaudiObjDesc/KeyedDictionary.h,v 1.2 2004-01-22 15:47:50 mato Exp $
//====================================================================
//	KeyedDictionary.h
//--------------------------------------------------------------------
//
//  Description: Compiler generated SEAL dictionaries
//
//	Author     : M.Frank
//====================================================================
#ifndef GAUDIKERNEL_KEYEDDICTIONARY_H
#define GAUDIKERNEL_KEYEDDICTIONARY_H 1

#include "GaudiObjDesc/SealDictionary.h"

#define private public
#define protected public
#include "GaudiKernel/KeyedObject.h"
#undef private
#undef protected

namespace GaudiDict {

template <class T> struct KeyedObjectDict { 
  typedef KeyedObject<T> _Me_;
  KeyedObjectDict();
  static int to_ContainedObject(void*);
  static void* Ctor       (void*);
  static void  destructor (void* o) { ((_Me_*)o)->~KeyedObject(); }
  static void* key        (void*);
  static void* hasKey     (void*);
};

template <class T> __template_method__
KeyedObjectDict<T>::KeyedObjectDict() {
  std::string _me_ = typeName(typeid(KeyedObject<T>));
  if ( 0 == seal::reflect::Class::forName(_me_) )  {
    std::string _k_  = typeName(typeid(T));
    seal::reflect::ClassBuilder _c_(_me_.c_str(), "", typeid(_Me_), sizeof(_Me_),
      std::vector<const std::type_info*>(), false, 
      seal::reflect::NOCONTAINER, 0, 
      PUBLIC | VIRTUAL);
    _c_.addSuperClass("ContainedObject", 0, to_ContainedObject);
    _c_.addField ("m_key", _k_.c_str(), "", (int)(&((_Me_*)64)->m_key)-64,      PROTECTED );
    //_c_.addField ("m_refCount",   "long", "", (int)(&((_Me_*)64)->m_refCount)-64, PROTECTED | TRANSIENT );
    //_c_.addField ("m_hasKey",     "bool", "", (int)(&((_Me_*)64)->m_hasKey)-64,   PROTECTED | TRANSIENT );
    _c_.addMethod("KeyedObject",  "", _me_.c_str(),           Ctor,PUBLIC);
    _c_.addMethod("~KeyedObject", "", destructor,          PUBLIC | VIRTUAL);
    _c_.addMethod("key",          "", std::string(_k_+"&").c_str(), key,        PUBLIC);
    _c_.addMethod("hasKey",       "", "bool",  hasKey,     PUBLIC);
    _c_.build();
  }
}

template <class T> __template_method__
int KeyedObjectDict<T>::to_ContainedObject(void* o) {
  return (int)(ContainedObject*)(_Me_*)o - (int)(_Me_*)o;
}

template <class T> __template_method__
void* KeyedObjectDict<T>::Ctor(void* mem) {
  return new(mem) _Me_();
}

template <class T> __template_method__
void* KeyedObjectDict<T>::key(void* o)  {
  return (void*)&((_Me_*)o)->key();
}

template <class T> __template_method__
void* KeyedObjectDict<T>::hasKey(void* o) {
  static bool ret;
  ret = ((_Me_*)o)->hasKey();
  return &ret;
}

}

#define private public
#define protected public
#include "GaudiKernel/KeyedContainer.h"
#undef private
#undef protected

namespace GaudiDict {

  template <class T> struct KeyedContainerDict { 
    typedef KeyedContainer<T> _Me_;
    KeyedContainerDict();
    static int to_base(void*);
    static void* copyCtor( void*, Args );
    static void* Ctor( void* );
    static void  Dtor( void* o ) { ((_Me_*)o)->~KeyedContainer(); }
    static void* clID( void* );
    static void* classID( void* );
    static void* numberOfObjects( void* );
    static void* add( void*, Args );
    static void* remove( void*, Args );
    static void* containedObject( void*, Args );
    static void* index( void*, Args );
    static void* containedObjects( void*, Args );
    static void* size( void* );
    static void* empty( void* );
    static void  reserve( void*, Args );
    static void  clear( void* );
    static void* containedObjects( void* );
    static void  configureDirectAccess( void* );
    static void* begin( void* );
    static void* begin_const( void* );
    static void* end( void* );
    static void* end_const( void* );
    static void* object( void*, Args );
    static void* operator_1746( void*, Args );
    static void* eraseKey( void*, Args );
    static void* eraseObj( void*, Args );
    static void* eraseIter( void*, Args );
    static void  eraseRange( void*, Args );
    static void* insertWithKey( void*, Args );
    static void* insert( void*, Args );
  };
}

template<class T> __template_method__ 
GaudiDict::KeyedContainerDict<T>::KeyedContainerDict() {
  std::string _me_ = keyedContainerName("Containers::KeyedObjectManager<Containers::hashmap> >", typeid(T));
  if ( 0 == seal::reflect::Class::forName(_me_) )  {
    static char clid_txt[64];
    std::string _v_  = vectorName(typeid(T*));
    std::string _k_  = typeName(typeid(typename T::key_type));
    std::string _t_  = typeName(typeid(T));
    seal::reflect::ClassBuilder _c_(_me_.c_str(), "", typeid(_Me_), sizeof(_Me_),
      std::vector<const std::type_info*>(), false, 
      seal::reflect::NOCONTAINER, 0, 
      PUBLIC | VIRTUAL);
    _c_.addSuperClass("ObjectContainerBase", 0, to_base);
    //_c_.addField("m_cont", "Containers::KeyedObjectManager<Containers::hashmap>", "", (int)(&((_Me_*)64)->m_cont)-64, PRIVATE | TRANSIENT );
    _c_.addField("m_sequential", _v_.c_str(), "", (int)(&((_Me_*)64)->m_sequential)-64, PRIVATE );
    //_c_.addField("m_random",  _v_+"*", "", (int)(&((_Me_*)64)->m_random)-64, PRIVATE | TRANSIENT );
    _c_.addMethod("KeyedContainer", "", _me_.c_str(),           Ctor,     PUBLIC);
    _c_.addMethod("KeyedContainer", "", _me_.c_str(), std::string(_me_+"&").c_str(), copyCtor, PUBLIC);
    _c_.addMethod("~KeyedContainer", "",                Dtor,     PUBLIC | VIRTUAL);
    _c_.addMethod("clID",           "", "long unsigned int&", clID,     PUBLIC | VIRTUAL);
    _c_.addMethod("classID",        "", "long unsigned int&", classID,  PUBLIC | STATIC);
    _c_.addMethod("numberOfObjects","", "long", numberOfObjects, PUBLIC | VIRTUAL);
    _c_.addMethod("add",            "", "long", "ContainedObject*", add,PUBLIC | VIRTUAL);
    _c_.addMethod("remove",         "", "long", "ContainedObject*", remove, PUBLIC | VIRTUAL);
    _c_.addMethod("containedObject","", "ContainedObject*", "long", containedObject, PUBLIC | VIRTUAL);
    _c_.addMethod("index",          "", "long", "ContainedObject*", index, PUBLIC | VIRTUAL);
    _c_.addMethod("containedObjects","","long", "std::vector<ContainedObject*>&", containedObjects, PUBLIC | VIRTUAL);
    _c_.addMethod("size",           "", "long", size, PUBLIC);
    _c_.addMethod("empty",          "", "bool", empty, PUBLIC);
    _c_.addMethod("reserve",        "", "long", reserve, PUBLIC);
    _c_.addMethod("clear",          "",  clear, PUBLIC);
    _c_.addMethod("containedObjects","","std::vector<const ContainedObject*>*", containedObjects, PUBLIC | VIRTUAL);
    _c_.addMethod("configureDirectAccess", "",                configureDirectAccess, PUBLIC | VIRTUAL);
    _c_.addMethod("begin",          "", std::string(_v_+"::iterator").c_str(),                            begin, PUBLIC);
    _c_.addMethod("begin",          "", std::string(_v_+"::const_iterator").c_str(),                      begin_const, PUBLIC);
    _c_.addMethod("end",            "", std::string(_v_+"::iterator").c_str(),                            end, PUBLIC);
    _c_.addMethod("end",            "", std::string(_v_+"::const_iterator").c_str(),                      end_const, PUBLIC);
    _c_.addMethod("object",         "", std::string(_t_+"*").c_str(), std::string(_k_+"&").c_str(),       object, PUBLIC);
    _c_.addMethod("eraseKey",       "", std::string(_k_).c_str(), std::string(_k_+"&").c_str(),           eraseKey, PUBLIC);
    _c_.addMethod("eraseObj",       "", std::string(_k_).c_str(), std::string(_t_+"*").c_str(),           eraseObj, PUBLIC);
    _c_.addMethod("eraseIter",      "", std::string(_k_).c_str(), std::string(_v_+"::iterator").c_str(),  eraseIter, PUBLIC);
    _c_.addMethod("eraseRange",     "", std::string(_v_+"::iterator; "+_v_+"::iterator").c_str(),         eraseRange, PUBLIC);
    _c_.addMethod("insertWithKey",  "", std::string(_k_+"&").c_str(), std::string(_t_+"*; "+_k_+"&").c_str(),                  insertWithKey, PUBLIC);
    _c_.addMethod("insert",         "", std::string(_k_+"&").c_str(), std::string(_t_+"*").c_str(),                            insert, PUBLIC);
    sprintf(clid_txt,"%08X-0000-0000-0000-000000000000",(int)_Me_::classID());
    _c_.addProperty("ClassID", (char*)clid_txt);
    _c_.build();
  }
}

template<class T> __template_method__ 
int GaudiDict::KeyedContainerDict<T>::to_base( void* o ) {
  return (int)(ObjectContainerBase*)(_Me_*)o - (int)(_Me_*)o;
}

template<class T> __template_method__ 
void* GaudiDict::KeyedContainerDict<T>::copyCtor( void* mem, Args arg) {
  return new(mem) _Me_(*(const _Me_*)arg[0]);
}

template<class T> __template_method__ 
void* GaudiDict::KeyedContainerDict<T>::Ctor( void* mem) {
  return new(mem) _Me_();
}

template<class T> __template_method__ 
void* GaudiDict::KeyedContainerDict<T>::clID( void* o) {
  return (void*)&((_Me_*)o)->clID();
}

template<class T> __template_method__ 
void* GaudiDict::KeyedContainerDict<T>::classID( void* o)  {
  return (void*)&((_Me_*)o)->classID();
}

template<class T> __template_method__ 
void* GaudiDict::KeyedContainerDict<T>::numberOfObjects( void* o)  {
  static long ret;
  ret = ((_Me_*)o)->numberOfObjects();
  return &ret;
}

template<class T> __template_method__ 
void* GaudiDict::KeyedContainerDict<T>::add( void* o, Args arg) {
  static long ret;
  ret = ((_Me_*)o)->add((ContainedObject*)arg[0]);
  return &ret;
}

template<class T> __template_method__ 
void* GaudiDict::KeyedContainerDict<T>::remove( void* o, Args arg)  {
  static long ret;
  ret = ((_Me_*)o)->remove((ContainedObject*)arg[0]);
  return &ret;
}

template<class T> __template_method__ 
void* GaudiDict::KeyedContainerDict<T>::containedObject( void* o, Args arg) {
  return (void*)((_Me_*)o)->containedObject(*(long*)arg[0]);
}

template<class T> __template_method__ 
void* GaudiDict::KeyedContainerDict<T>::index( void* o, Args arg) {
  static long ret;
  ret = ((_Me_*)o)->index((const ContainedObject*)arg[0]);
  return &ret;
}

template<class T> __template_method__ 
void* GaudiDict::KeyedContainerDict<T>::containedObjects( void* o, Args arg)  {
  static long ret;
  ret = ((_Me_*)o)->containedObjects(*(std::vector<ContainedObject*>*)arg[0]);
  return &ret;
}

template<class T> __template_method__ 
void* GaudiDict::KeyedContainerDict<T>::size( void* o) {
  static long ret;
  ret = ((_Me_*)o)->size();
  return &ret;
}

template<class T> __template_method__ 
void* GaudiDict::KeyedContainerDict<T>::empty( void* o)  {
  static bool ret;
  ret = ((_Me_*)o)->empty();
  return &ret;
}

template<class T> __template_method__ 
void GaudiDict::KeyedContainerDict<T>::reserve( void* o, Args arg)  {
  ((_Me_*)o)->reserve(*(long*)arg[0]);
}

template<class T> __template_method__ 
void GaudiDict::KeyedContainerDict<T>::clear( void* o) {
  ((_Me_*)o)->clear();
}

template<class T> __template_method__ 
void* GaudiDict::KeyedContainerDict<T>::containedObjects( void* o) {
  return (void*)((_Me_*)o)->containedObjects();
}

template<class T> __template_method__ 
void GaudiDict::KeyedContainerDict<T>::configureDirectAccess( void* o) {
  ((_Me_*)o)->configureDirectAccess();
}

template<class T> __template_method__ 
void* GaudiDict::KeyedContainerDict<T>::begin( void* o)  {
  return new typename std::vector<T*>::iterator(((_Me_*)o)->begin());
}

template<class T> __template_method__ 
void* GaudiDict::KeyedContainerDict<T>::begin_const( void* o)  {
  return new typename std::vector<T*>::const_iterator(((_Me_*)o)->begin());
}

template<class T> __template_method__ 
void* GaudiDict::KeyedContainerDict<T>::end( void* o)  {
  return new typename std::vector<T*>::iterator(((_Me_*)o)->end());
}

template<class T> __template_method__ 
void* GaudiDict::KeyedContainerDict<T>::end_const( void* o)  {
  return new typename std::vector<T*>::const_iterator(((_Me_*)o)->end());
}

template<class T> __template_method__ 
void* GaudiDict::KeyedContainerDict<T>::object( void* o, Args arg)  {
  return (void*)((_Me_*)o)->object(*(const long*)arg[0]);
}

template<class T> __template_method__ 
void* GaudiDict::KeyedContainerDict<T>::eraseKey( void* o, Args arg)  {
  static long ret;
  ret = ((_Me_*)o)->erase(*(const long*)arg[0]);
  return &ret;
}

template<class T> __template_method__ 
void* GaudiDict::KeyedContainerDict<T>::eraseObj( void* o, Args arg)  {
  static long ret;
  ret = ((_Me_*)o)->erase(*(T* const*)arg[0]);
  return &ret;
}

template<class T> __template_method__ 
void* GaudiDict::KeyedContainerDict<T>::eraseIter( void* o, Args arg) {
  static long ret;
  ret = ((_Me_*)o)->erase(*(typename std::vector<T*>::iterator*)arg[0]);
  return &ret;
}

template<class T> __template_method__ 
void GaudiDict::KeyedContainerDict<T>::eraseRange( void* o, Args arg) {
  ((_Me_*)o)->erase(*(typename std::vector<T*>::iterator*)arg[0], *(typename std::vector<T*>::iterator*)arg[1]);
}

template<class T> __template_method__ 
void* GaudiDict::KeyedContainerDict<T>::insertWithKey( void* o, Args arg) {
  return (void*)&((_Me_*)o)->insert(*(T* const*)arg[0],*(const long*)arg[1]);
}

template<class T> __template_method__ 
void* GaudiDict::KeyedContainerDict<T>::insert( void* o, Args arg)  {
  return (void*)&((_Me_*)o)->insert(*(T* const*)arg[0]);
}

namespace GaudiDict  {

  /** @class KeyedObjectDictionaries
    * 
    *  Helper class to generate all dictionaries for keyed
    *  object derived classes in one step.
    * 
    *   @author  M.Frank
    *   @version 1.0
    */
  template<class T> struct KeyedObjectDictionaries  {
    KeyedObjectDictionaries()    {
      KeyedObjectDict<typename T::key_type>();
      VectorDict<T*>();
      SmartRefDict<T>();
      VectorDict<SmartRef<T> >();
      SmartRefVectorDict<T>();
      ObjectVectorDict<T>();
      KeyedContainerDict<T>();
    }
  };
}

#define KEYEDOBJECTDICTIONARIES(CLASS)                          \
namespace {                                                     \
  struct _InitDict##CLASS {                                     \
    _InitDict##CLASS() {                                        \
      GaudiDict::KeyedObjectDict<CLASS::key_type>();            \
      GaudiDict::SmartRefDict<CLASS>();                         \
      GaudiDict::SmartRefVectorDict<CLASS>();                   \
      GaudiDict::VectorDict<CLASS*>();                          \
      GaudiDict::ObjectVectorDict<CLASS>();                     \
      GaudiDict::KeyedContainerDict<CLASS>();                   \
    }                                                           \
  };                                                            \
  static _InitDict##CLASS __init##CLASS;                        \
}                                                               \
void* __init_InitDict(CLASS* /* dummy */) {                     \
  return &__init##CLASS;                                        \
}

#endif // GAUDIKERNEL_KEYEDDICTIONARY_H
