#ifndef ROOTCNV_ROOTREFS_H
#define ROOTCNV_ROOTREFS_H

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

  /** @struct ObjectRefs RootRefs.h RootCnv/RootRefs.h
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
    RootObjectRefs(const ObjectRefs& r) : links(r.links), refs(r.refs) {}
    /// Default destructor
    ~ObjectRefs() {}
    /// Assignment operator
    ObjectRefs& operator=(const ObjectRefs& r) {
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

#endif // ROOTCNV_ROOTREFS_H

