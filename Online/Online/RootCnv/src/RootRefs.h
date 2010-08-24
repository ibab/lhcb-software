// $Id: RootRefs.h,v 1.4 2010-08-24 23:30:32 frankb Exp $
//====================================================================
// Dictionary classes for internal classes used to write ROOT files.
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef GAUDIROOTCNV_ROOTREFS_H
#define GAUDIROOTCNV_ROOTREFS_H

// Framework include files
#include "GaudiKernel/Kernel.h"

// C++ include files
#include <string>
#include <vector>

/*
 *   Gaudi namespace declaration
 */
namespace Gaudi {

  /** @struct RootRef RootRefs.h RootCnv/RootRefs.h
   *
   * Persistent reference object.
   * 
   * @author  M.Frank
   * @version 1.0
   */
  struct RootRef {
    /// Data members to define object location in the persistent world
    int      dbase,container,link,clid,svc,entry;
    /// Standard constructor
    RootRef()     {  this->reset(); }
    /// Copy constructor
    RootRef(const RootRef& c) 
      : dbase(c.dbase),container(c.container),link(c.link),clid(c.clid),svc(c.svc),entry(c.entry)
    {
    }
    /// Assignment operator
    RootRef& operator=(const RootRef& c)  {
      dbase     = c.dbase;
      container = c.container;
      link      = c.link;
      clid      = c.clid;
      svc       = c.svc;
      entry     = c.entry;
      return *this;
    }
    /// Reset data content
    void reset() {
      dbase     = -1;
      container = -1;
      link      = -1;
      entry     = -1;
      svc       = 1;
      clid      = 1;
    }
  };

  /** @struct RootObjectRefs RootRefs.h RootCnv/RootRefs.h
   *
   * Persistent reference object containing all leafs and links
   * corresponding to a Gaudi DataObject.
   * 
   * @author  M.Frank
   * @version 1.0
   */
  struct RootObjectRefs {
    /// The links of the link manager
    std::vector<int>       links;
    /// The references corresponding to the next layer of items in the data store
    std::vector<RootRef>   refs;
    
    /// Default constructor
    RootObjectRefs() {}
    /// Copy constructor
    RootObjectRefs(const RootObjectRefs& r) : links(r.links), refs(r.refs) {}
    /// Default destructor
    ~RootObjectRefs() {}
    /// Assignment operator
    RootObjectRefs& operator=(const RootObjectRefs& r) {
      links = r.links;
      refs = r.refs; 
      return *this;
    }
  };

  /** @class RootNTupleDescriptor RootRefs.h RootCnv/RootRefs.h
   *
   * Description:
   *
   * @author  M.Frank
   * @version 1.0
   */
  struct RootNTupleDescriptor {
    /// Description string
    std::string   description;
    /// Optional description 
    std::string   optional;
    /// Identifier of description
    std::string   container;
    /// Class ID of the described object
    unsigned long clid;
    /// Standard constructor
    RootNTupleDescriptor() {}
    /// Standard destructor
    virtual ~RootNTupleDescriptor() {}
  };
}

// The following two classes are the interfaces to facilitate reading POOL formatted ROOT files:

typedef Gaudi::RootNTupleDescriptor PoolDbNTupleDescriptor;

/*
 *   POOL namespace declaration
 */
namespace pool  {

  /** @class Token
   *
   *  Shadow class to mimik POOL tokens.
   *
   */
  class Token {
  public:
    /// POOL OID data member
    std::pair<int, int> m_oid;
    bool operator==(const Token& t) const { return m_oid.first==t.m_oid.first && m_oid.second==t.m_oid.second;}
  };
}

  /** @class UCharDbArray
   *
   *  Shadow class to mimik POOL blobs.
   *
   */
struct UCharDbArray {
  public: 
  /// Size of buffer
  int    m_size; 
  /// Buffer with object content
  unsigned char *m_buffer;//[m_size]
  /// Default constructor
  UCharDbArray() : m_size(0), m_buffer(0)  {}
  /// Standard destructor
  virtual ~UCharDbArray() {
    if ( m_buffer ) delete [] m_buffer;
    m_buffer = 0;
  }
}; 

/** @class PoolDbTokenWrap PoolDbNTupleDescriptor.h PoolDb/PoolDbNTupleDescriptor.h
  *
  * Description:
  *
  * @author  M.Frank
  * @version 1.0
  */
struct PoolDbTokenWrap {
  /// Aggregated token object
  pool::Token token;
  /// Standard constructor
  PoolDbTokenWrap() {}
  /// Copy constructor
  PoolDbTokenWrap(const PoolDbTokenWrap& wrp)  {    token = wrp.token;  }
  /// Standard destructor
  virtual ~PoolDbTokenWrap() {}
  /// Equality operator
  bool operator==(const PoolDbTokenWrap& c) const {    return token == c.token;  }
  /// Assignment operator
  PoolDbTokenWrap& operator=(const PoolDbTokenWrap& wrp)  {    token = wrp.token;    return *this;  }
};

/** @class PoolDbLinkManager PoolDbLinkManager.h GaudiPoolDb/PoolDbLinkManager.h
  *
  * Description:
  * PoolDbLinkManager class implementation definition.
  *
  * @author Markus Frank
  * @version 1.0
  */
class GAUDI_API PoolDbLinkManager {
protected:
  /// Directory links
  std::vector<pool::Token*> m_refs;
  /// Logical links
  std::vector<std::string>  m_links;

public:
  /// Standard constructor
  PoolDbLinkManager() {}
  /// Standard destructor
  virtual ~PoolDbLinkManager() {
    for(std::vector<pool::Token*>::iterator i=m_refs.begin(); i!=m_refs.end();++i)
      delete (*i);
  }
  /// Access to token array
  std::vector<pool::Token*>& references()  {    return m_refs;   }
  /// Access to link array
  std::vector<std::string>& links()        {    return m_links;  }
};

#endif // GAUDIROOTCNV_ROOTREFS_H

