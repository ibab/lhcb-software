// $Id: RelationsDict.h,v 1.3 2005-02-08 20:55:11 ibelyaev Exp $
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

#include "GaudiKernel/SealDictionary.h"

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
    static void  destructor( void* o ) { ((_Me_*)o)->~Entry(); }
    /// Static function to execute object constructor
    static void* ctor( void* );
  };
}

template <class FROM, class TO>
GaudiDict::RelationTypeTraitsDict<FROM,TO>::RelationTypeTraitsDict() {
  std::string _me_ = templateName2("Relations::RelationTypeTraits", typeid(FROM),typeid(TO));
  if ( 0 == seal::reflect::Class::forName(_me_) )  {
    seal::reflect::ClassBuilder _c_(_me_.c_str(), "", typeid(_Me_), sizeof(_Me_),
      std::vector<const std::type_info*>(), false, 
      seal::reflect::NOCONTAINER, 0, PUBLIC);
    _c_.build();
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
    static int to_pair(void*);
    /// Static function to execute object destructor
    static void  destructor( void* o ) { ((_Me_*)o)->~Entry(); }
    /// Static function to execute object constructor
    static void* ctor( void* );
  };
}

template <class FROM, class TO>
GaudiDict::RelationTypeTraitsEntryDict<FROM,TO>::RelationTypeTraitsEntryDict() {
  RelationTypeTraitsDict<FROM,TO>();
  typedef typename Relations::RelationTypeTraits<FROM,TO>::From  _From_;
  typedef typename Relations::RelationTypeTraits<FROM,TO>::To    _To_;
  std::string _me_ = templateName2("Relations::RelationTypeTraits", typeid(FROM),typeid(TO));
  _me_ += "::Entry";
  if ( 0 == seal::reflect::Class::forName(_me_) )  {
    seal::reflect::ClassBuilder _c_(_me_.c_str(), "", typeid(_Me_), sizeof(_Me_),
      std::vector<const std::type_info*>(), false, 
      seal::reflect::NOCONTAINER, 0, PUBLIC);
    std::string _frst_ = typeName(typeid(_From_));
    std::string _scnd_ = typeName(typeid(_To_));
    //
    _c_.addField( "_from" , _frst_.c_str() , "" , (int)(&((_Me_*)64)->first)-64  , PUBLIC ) ; 
    _c_.addField( "_to"   , _scnd_.c_str() , "" , (int)(&((_Me_*)64)->second)-64 , PUBLIC ) ;
    //
    _c_.addMethod("Entry",  "", _me_.c_str(),             ctor,       PUBLIC);
    _c_.addMethod("~Entry", "",                   destructor, PUBLIC);
    _c_.build();
  }
}

template <class FROM, class TO>
void* GaudiDict::RelationTypeTraitsEntryDict<FROM,TO>::ctor( void* mem) {
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
    static void  destructor( void* o ) { ((_Me_*)o)->~RelationBase(); }
    /// Static function to execute object constructor
    static void* ctor( void* );
  };
}

template <class FROM, class TO> 
GaudiDict::RelationBaseDict<FROM,TO>::RelationBaseDict() {
  RelationTypeTraitsDict<FROM,TO>();
  RelationTypeTraitsEntryDict<FROM,TO>();
  VectorDict<typename Relations::RelationTypeTraits<FROM, TO>::Entry >();
  std::string _me_ = templateName2("Relations::RelationBase", typeid(FROM),typeid(TO));
  if ( 0 == seal::reflect::Class::forName(_me_) )  {
    std::string _v_ = "std::vector<";
    _v_ += templateName2("Relations::RelationTypeTraits", typeid(FROM),typeid(TO));
    _v_ += "::Entry>";
    seal::reflect::ClassBuilder _c_(_me_.c_str(), "", typeid(_Me_), sizeof(_Me_),
      std::vector<const std::type_info*>(), false, 
      seal::reflect::NOCONTAINER, 0, 
      PUBLIC | VIRTUAL);
    _c_.addField("m_entries", _v_.c_str(), "", (int)(&((_Me_*)64)->m_entries)-64, PRIVATE );
    _c_.addMethod("RelationBase",  "", _me_.c_str(), ctor, PUBLIC);
    _c_.addMethod("~RelationBase", "", destructor, PUBLIC | VIRTUAL);
    _c_.build();
  }
}

template <class FROM, class TO> 
void* GaudiDict::RelationBaseDict<FROM,TO>::ctor( void* mem) {
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
    static void  destructor( void* o ) { ((_Me_*)o)->~Relation(); }
    /// Static function to execute object constructor
    static void* ctor( void* );
  };
}

template <class FROM, class TO> 
GaudiDict::RelationDict<FROM,TO>::RelationDict() {
  std::string _me_ = templateName2("Relations::Relation", typeid(FROM),typeid(TO));
  RelationBaseDict<FROM,TO>();
  if ( 0 == seal::reflect::Class::forName(_me_) )  {
    std::string  _b_ = templateName2("Relations::RelationBase", typeid(FROM),typeid(TO));
    seal::reflect::ClassBuilder _c_(_me_.c_str(), "", typeid(_Me_), sizeof(_Me_),
      std::vector<const std::type_info*>(), false, 
      seal::reflect::NOCONTAINER, 0, 
      PUBLIC | VIRTUAL);
    _c_.addField("m_direct",   _b_.c_str(), "", (int)(&((_Me_*)64)->m_direct)-64, PRIVATE );
    _c_.addField("m_inverse",  std::string(_b_+"*").c_str(), "", (int)(&((_Me_*)64)->m_inverse)-64, PRIVATE | TRANSIENT );
    _c_.addMethod("Relation",  "", _me_.c_str(), ctor, PUBLIC);
    _c_.addMethod("~Relation", "", destructor, PUBLIC | VIRTUAL);
    _c_.build();
  }
}

template <class FROM, class TO> 
void* GaudiDict::RelationDict<FROM,TO>::ctor( void* mem) {
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
    /// Cast function to DataObject
    static int to_DataObject(void*);
    /// Static function to execute object destructor
    static void  destructor( void* o ) { ((_Me_*)o)->~Relation1D(); }
    /// Static function to execute object constructor
    static void* ctor( void* );
  };
}

template <class FROM, class TO>
GaudiDict::Relation1DDict<FROM,TO>::Relation1DDict() {
  std::string _me_ = templateName2("Relation1D", typeid(FROM),typeid(TO));
  RelationDict<FROM,TO>();
  if ( 0 == seal::reflect::Class::forName(_me_) )  {
    std::string  _b_ = templateName2("Relations::Relation", typeid(FROM),typeid(TO));
    seal::reflect::ClassBuilder _c_(_me_.c_str(), "", typeid(_Me_), sizeof(_Me_),
      std::vector<const std::type_info*>(), false, 
      seal::reflect::NOCONTAINER, 0, 
      PUBLIC | VIRTUAL);
    _c_.addSuperClass("DataObject", 0, to_DataObject);
    _c_.addMethod("Relation1D",  "", _me_.c_str(), ctor, PUBLIC);
    _c_.addMethod("~Relation1D", "", destructor, PUBLIC | VIRTUAL);
    _c_.addField("m_base",   _b_.c_str(), "", (int)(&((_Me_*)64)->m_base)-64, PRIVATE );
    static char clid_txt[64];
    sprintf(clid_txt,"%08X-0000-0000-0000-000000000000",(int)_Me_::classID());
    _c_.addProperty("ClassID", (char*)clid_txt);
    _c_.build();
  }
}

template <class FROM, class TO> 
int GaudiDict::Relation1DDict<FROM,TO>::to_DataObject( void* o ) {
  return (int)(DataObject*)(_Me_*)o - (int)(_Me_*)o;
}

template <class FROM, class TO> 
void* GaudiDict::Relation1DDict<FROM,TO>::ctor( void* mem) {
  return new(mem) _Me_();
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
    /// Static function to execute object destructor
    static void  destructor( void* o ) { ((_Me_*)o)->~RelationWeightedTypeTraits(); }
    /// Static function to execute object constructor
    static void* ctor( void* );
  };
}

template <class FROM, class TO, class WEIGHT>
GaudiDict::RelationWeightedTypeTraitsDict<FROM,TO,WEIGHT>::RelationWeightedTypeTraitsDict() {
  std::string _me_ = templateName3("Relations::RelationWeightedTypeTraits", typeid(FROM),typeid(TO),typeid(WEIGHT));
  if ( 0 == seal::reflect::Class::forName(_me_) )  {
    seal::reflect::ClassBuilder _c_(_me_.c_str(), "", typeid(_Me_), sizeof(_Me_),
      std::vector<const std::type_info*>(), false, 
      seal::reflect::NOCONTAINER, 0, PUBLIC);
    _c_.build();
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
    static void  destructor( void* o ) { ((_Me_*)o)->~Entry(); }
    /// Static function to execute object constructor
    static void* ctor( void* );
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
  if ( 0 == seal::reflect::Class::forName(_me_) )  {
    seal::reflect::ClassBuilder _c_(_me_.c_str(), "", typeid(_Me_), sizeof(_Me_),
      std::vector<const std::type_info*>(), false, 
      seal::reflect::NOCONTAINER, 0, PUBLIC);
    std::string _frst_   = typeName(typeid(_From_));
    std::string _weight_ = typeName(typeid(_Weight_));
    std::string _scnd_   = typeName(typeid(_To_));
    //
    _c_.addField( "_from"   , _frst_   .c_str() , "" , (int)(&((_Me_*)64)->first.first)-64  , PUBLIC );
    _c_.addField( "_weight" , _weight_ .c_str() , "" , (int)(&((_Me_*)64)->first.second)-64 , PUBLIC );
    _c_.addField( "_to"     , _scnd_   .c_str() , "" , (int)(&((_Me_*)64)->second)-64       , PUBLIC );
    //
    _c_.addMethod("Entry", "", _me_  .c_str() , ctor,       PUBLIC);
    _c_.addMethod("~Entry","",       destructor, PUBLIC);
    _c_.build();
  }
}

template <class FROM, class TO, class WEIGHT>
void* GaudiDict::RelationWeightedTypeTraitsEntryDict<FROM,TO,WEIGHT>::ctor( void* mem) {
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
    static void  destructor( void* o ) { ((_Me_*)o)->~RelationWeightedBase(); }
    /// Static function to execute object constructor
    static void* ctor( void* );
  };
}

template <class FROM, class TO, class WEIGHT> 
GaudiDict::RelationWeightedBaseDict<FROM,TO,WEIGHT>::RelationWeightedBaseDict() {
  RelationWeightedTypeTraitsDict<FROM,TO,WEIGHT>();
  RelationWeightedTypeTraitsEntryDict<FROM,TO,WEIGHT>();
  VectorDict<typename Relations::RelationWeightedTypeTraits<FROM,TO,WEIGHT>::Entry >();
  std::string _me_ = templateName3("Relations::RelationWeightedBase", typeid(FROM),typeid(TO),typeid(WEIGHT));
  if ( 0 == seal::reflect::Class::forName(_me_) )  {
    std::string _v_ = "std::vector<";
    _v_ += templateName3("Relations::RelationWeightedTypeTraits", typeid(FROM),typeid(TO),typeid(WEIGHT));
    _v_ += "::Entry>";
    seal::reflect::ClassBuilder _c_(_me_.c_str(), "", typeid(_Me_), sizeof(_Me_),
      std::vector<const std::type_info*>(), false, 
      seal::reflect::NOCONTAINER, 0, 
      PUBLIC | VIRTUAL);
    _c_.addField("m_entries", _v_.c_str(), "", (int)(&((_Me_*)64)->m_entries)-64, PRIVATE );
    _c_.addMethod("RelationWeightedBase",  "", _me_.c_str(), ctor, PUBLIC);
    _c_.addMethod("~RelationWeightedBase", "", destructor, PUBLIC | VIRTUAL);
    _c_.build();
  }
}

template <class FROM, class TO, class WEIGHT> 
void* GaudiDict::RelationWeightedBaseDict<FROM,TO,WEIGHT>::ctor( void* mem) {
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
    static void  destructor( void* o ) { ((_Me_*)o)->~RelationWeighted(); }
    /// Static function to execute object constructor
    static void* ctor( void* );
  };
}

template <class FROM, class TO, class WEIGHT> 
GaudiDict::RelationWeightedDict<FROM,TO,WEIGHT>::RelationWeightedDict() {
  std::string _me_ = templateName3("Relations::RelationWeighted", typeid(FROM),typeid(TO),typeid(WEIGHT));
  RelationWeightedBaseDict<FROM,TO,WEIGHT>();
  if ( 0 == seal::reflect::Class::forName(_me_) )  {
    std::string  _b_ = templateName3("Relations::RelationWeightedBase", typeid(FROM),typeid(TO),typeid(WEIGHT));
    seal::reflect::ClassBuilder _c_(_me_.c_str(), "", typeid(_Me_), sizeof(_Me_),
      std::vector<const std::type_info*>(), false,
      seal::reflect::NOCONTAINER, 0, 
      PUBLIC | VIRTUAL);
    _c_.addField("m_direct",   _b_.c_str(), "", (int)(&((_Me_*)64)->m_direct)-64, PRIVATE );
    _c_.addField("m_inverse",  std::string(_b_+"*").c_str(), "", (int)(&((_Me_*)64)->m_inverse)-64, PRIVATE | TRANSIENT );
    _c_.addMethod("RelationWeighted",  "", _me_.c_str(), ctor, PUBLIC);
    _c_.addMethod("~RelationWeighted", "", destructor, PUBLIC | VIRTUAL);
    _c_.build();
  }
}

template <class FROM, class TO, class WEIGHT> 
void* GaudiDict::RelationWeightedDict<FROM,TO,WEIGHT>::ctor( void* mem) {
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
    /// Cast function to DataObject
    static int   to_DataObject(void*);
    /// Static function to execute destructor
    static void  destructor( void* o ) { ((_Me_*)o)->~RelationWeighted1D(); }
    /// Static function to execute object constructor
    static void* ctor( void* );
  };
}

template <class FROM, class TO, class WEIGHT>
GaudiDict::RelationWeighted1DDict<FROM,TO,WEIGHT>::RelationWeighted1DDict() {
  std::string _me_ = templateName3("RelationWeighted1D", typeid(FROM),typeid(TO),typeid(WEIGHT));
  RelationWeightedDict<FROM,TO,WEIGHT>();
  if ( 0 == seal::reflect::Class::forName(_me_) )  {
    std::string  _b_ = templateName3("Relations::RelationWeighted", typeid(FROM),typeid(TO),typeid(WEIGHT));
    seal::reflect::ClassBuilder _c_(_me_.c_str(), "", typeid(_Me_), sizeof(_Me_),
      std::vector<const std::type_info*>(), false, 
      seal::reflect::NOCONTAINER, 0, 
      PUBLIC | VIRTUAL);
    _c_.addSuperClass("DataObject", 0, to_DataObject);
    _c_.addMethod("RelationWeighted1D",  "", _me_.c_str(), ctor, PUBLIC);
    _c_.addMethod("~RelationWeighted1D", "", destructor, PUBLIC | VIRTUAL);
    _c_.addField("m_base",   _b_.c_str(), "", (int)(&((_Me_*)64)->m_base)-64, PRIVATE );
    static char clid_txt[64];
    sprintf(clid_txt,"%08X-0000-0000-0000-000000000000", (int)_Me_::classID());
    _c_.addProperty("ClassID", (char*)clid_txt);
    _c_.build();
  }
}

template <class FROM, class TO, class WEIGHT> 
int GaudiDict::RelationWeighted1DDict<FROM,TO,WEIGHT>::to_DataObject( void* o ) {
  return (int)(DataObject*)(_Me_*)o - (int)(_Me_*)o;
}

template <class FROM, class TO, class WEIGHT> 
void* GaudiDict::RelationWeighted1DDict<FROM,TO,WEIGHT>::ctor( void* mem) {
  return new(mem) _Me_();
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
    static void  destructor( void* o ) { ((_Me_*)o)->~Relation2(); }
    /// Static function to execute object constructor
    static void* ctor( void* );
  };
}

template <class FROM, class TO> 
GaudiDict::Relation2Dict<FROM,TO>::Relation2Dict() {
  std::string _me_ = templateName2("Relations::Relation2", typeid(FROM),typeid(TO));
  RelationDict<FROM,TO>();
  RelationDict<TO,FROM>();
  if ( 0 == seal::reflect::Class::forName(_me_) )  {
    std::string  _p_ = templateName2("Relations::Relation", typeid(FROM),typeid(TO));
    std::string  _q_ = templateName2("Relations::Relation", typeid(TO),typeid(FROM));
    seal::reflect::ClassBuilder _c_(_me_.c_str(), "", typeid(_Me_), sizeof(_Me_),
      std::vector<const std::type_info*>(), false,
      seal::reflect::NOCONTAINER, 0, 
      PUBLIC | VIRTUAL);
    _c_.addMethod("Relation2",  "", _me_.c_str(), ctor, PUBLIC);
    _c_.addMethod("~Relation2", "", destructor, PUBLIC | VIRTUAL);
    _c_.addField("m_direct",  _p_.c_str(), "", (int)(&((_Me_*)64)->m_direct)-64, PRIVATE );
    _c_.addField("m_inverse", _q_.c_str(), "", (int)(&((_Me_*)64)->m_inverse)-64, PRIVATE );
    _c_.build();
  }
}

template <class FROM, class TO> 
void* GaudiDict::Relation2Dict<FROM,TO>::ctor( void* mem) {
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
    /// Cast function to DataObject
    static int to_DataObject(void*);
    /// Static function to execute object destructor
    static void  destructor( void* o ) { ((_Me_*)o)->~Relation2D(); }
    /// Static function to execute object constructor
    static void* ctor( void* );
  };
}

template <class FROM, class TO>
GaudiDict::Relation2DDict<FROM,TO>::Relation2DDict() {
  std::string _me_ = templateName2("Relation2D", typeid(FROM),typeid(TO));
  Relation2Dict<FROM,TO>();
  if ( 0 == seal::reflect::Class::forName(_me_) )  {
    std::string  _p_ = templateName2("Relations::Relation2", typeid(FROM),typeid(TO));
    seal::reflect::ClassBuilder _c_(_me_.c_str(), "", typeid(_Me_), sizeof(_Me_),
      std::vector<const std::type_info*>(), false, 
      seal::reflect::NOCONTAINER, 0, 
      PUBLIC | VIRTUAL);
    _c_.addSuperClass("DataObject", 0, to_DataObject);
    _c_.addMethod("Relation2D",  "", _me_.c_str(), ctor, PUBLIC);
    _c_.addMethod("~Relation2D", "", destructor, PUBLIC | VIRTUAL);
    _c_.addField("m_base",  _p_.c_str(), "", (int)(&((_Me_*)64)->m_base-64), PRIVATE );
    static char clid_txt[64];
    sprintf(clid_txt,"%08X-0000-0000-0000-000000000000",(int)_Me_::classID());
    _c_.addProperty("ClassID", (char*)clid_txt);
    _c_.build();
  }
}

template <class FROM, class TO> 
int GaudiDict::Relation2DDict<FROM,TO>::to_DataObject( void* o ) {
  return (int)(DataObject*)(_Me_*)o - (int)(_Me_*)o;
}

template <class FROM, class TO> 
void* GaudiDict::Relation2DDict<FROM,TO>::ctor( void* mem) {
  return new(mem) _Me_();
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
    static void  destructor( void* o ) { ((_Me_*)o)->~Relation2Weighted(); }
    /// Static function to execute object constructor
    static void* ctor( void* );
  };
}

template <class FROM, class TO, class WEIGHT> 
GaudiDict::Relation2WeightedDict<FROM,TO,WEIGHT>::Relation2WeightedDict() {
  std::string _me_ = templateName3("Relations::Relation2Weighted", typeid(FROM),typeid(TO),typeid(WEIGHT));
  RelationWeightedDict<FROM,TO,WEIGHT>();
  RelationWeightedDict<TO,FROM,WEIGHT>();
  if ( 0 == seal::reflect::Class::forName(_me_) )  {
    std::string  _p_ = templateName3("Relations::RelationWeighted", typeid(FROM),typeid(TO),typeid(WEIGHT));
    std::string  _q_ = templateName3("Relations::RelationWeighted", typeid(TO),typeid(FROM),typeid(WEIGHT));
    seal::reflect::ClassBuilder _c_(_me_.c_str(), "", typeid(_Me_), sizeof(_Me_),
      std::vector<const std::type_info*>(), false,
      seal::reflect::NOCONTAINER, 0, 
      PUBLIC | VIRTUAL);
    _c_.addField("m_direct",  _p_.c_str(), "", (int)(&((_Me_*)64)->m_direct)-64, PRIVATE );
    _c_.addField("m_inverse", _q_.c_str(), "", (int)(&((_Me_*)64)->m_inverse)-64, PRIVATE );
    _c_.addMethod("Relation2Weighted",  "", _me_.c_str(), ctor, PUBLIC);
    _c_.addMethod("~Relation2Weighted", "", destructor, PUBLIC | VIRTUAL);
    _c_.build();
  }
}

template <class FROM, class TO, class WEIGHT> 
void* GaudiDict::Relation2WeightedDict<FROM,TO,WEIGHT>::ctor( void* mem) {
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
    /// Cast function to DataObject
    static int   to_DataObject(void*);
    /// Static function to execute destructor
    static void  destructor( void* o ) { ((_Me_*)o)->~RelationWeighted2D(); }
    /// Static function to execute object constructor
    static void* ctor( void* );
  };
}

template <class FROM, class TO, class WEIGHT>
GaudiDict::RelationWeighted2DDict<FROM,TO,WEIGHT>::RelationWeighted2DDict() {
  std::string _me_ = templateName3("RelationWeighted2D", typeid(FROM), typeid(TO), typeid(WEIGHT));
  Relation2WeightedDict<FROM,TO,WEIGHT>();
  if ( 0 == seal::reflect::Class::forName(_me_) )  {
    std::string  _p_ = templateName3("Relations::Relation2Weighted", typeid(FROM), typeid(TO), typeid(WEIGHT));
    seal::reflect::ClassBuilder _c_(_me_.c_str(), "", typeid(_Me_), sizeof(_Me_),
      std::vector<const std::type_info*>(), false, 
      seal::reflect::NOCONTAINER, 0, 
      PUBLIC | VIRTUAL);
    _c_.addSuperClass("DataObject", 0, to_DataObject);
    _c_.addMethod("RelationWeighted2D",  "", _me_.c_str(), ctor, PUBLIC);
    _c_.addMethod("~RelationWeighted2D", "", destructor, PUBLIC | VIRTUAL);
    _c_.addField("m_base",  _p_.c_str(), "", (int)(&((_Me_*)64)->m_base)-64, PRIVATE );
    static char clid_txt[64];
    sprintf(clid_txt,"%08X-0000-0000-0000-000000000000", (int)_Me_::classID());
    _c_.addProperty("ClassID", (char*)clid_txt);
    _c_.build();
  }
}

template <class FROM, class TO, class WEIGHT> 
int GaudiDict::RelationWeighted2DDict<FROM,TO,WEIGHT>::to_DataObject( void* o ) {
  return (int)(DataObject*)(_Me_*)o - (int)(_Me_*)o;
}

template <class FROM, class TO, class WEIGHT> 
void* GaudiDict::RelationWeighted2DDict<FROM,TO,WEIGHT>::ctor( void* mem) {
  return new(mem) _Me_();
}
#endif // EVENT_RELATIONDICT_H
