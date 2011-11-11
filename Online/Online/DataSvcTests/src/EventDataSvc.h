// $Id: EventDataSvc.h,v 1.6 2005/01/19 08:23:16 mato Exp $
#ifndef EVENTDATASVC__EVENTDATASVC_H
#define EVENTDATASVC__EVENTDATASVC_H

// Framework include files
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IEventDataSvc.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/Service.h"
#include "AVX.h"

// Forward declarations
class IConversionSvc;
class IIncidentSvc;

/*
 *    Gaudi namespace declaration
 */
namespace Gaudi {

  using Framework::AVXMemory;
  using Framework::AVXHeap;

  /** @class EventDataSvc EventDataSvc.h GaudiKernel/EventDataSvc.h
   *
   *  Event Data servive implementation.
   *  See the interface header file for more details.
   *
   *  @author Markus Frank
   *  @version 1.0
   */
  class GAUDI_API EventDataSvc : public extends2<Service,IEventDataSvc,IDataManagerSvc>    {
  protected:

    struct Entry : public IRegistry  {
      IEventDataSvc*  m_service;
      unsigned int    m_tag;
      DataObject*     m_object;
      IOpaqueAddress* m_address;
      mutable std::string     m_name;

      Entry(IEventDataSvc* svc) : m_service(svc), m_tag(0), m_object(0), m_address(0) {}
      /// Defautl destructor
      virtual ~Entry();

      /// Clear entry and release objects
      void clear();
      /// Setup entry structure
      void set(const EventTag_t& tag, DataObject* pObject, IOpaqueAddress* pAddr);
      /// Set/Update Object reference
      void setObject(DataObject* pObject);
      /// Retrieve object behind the link. Non-virtual for speed reasons
      DataObject*     _object    () const { return m_object;  }
      /// Retrieve opaque storage address. Non-virtual for speed reasons
      IOpaqueAddress* _address   () const { return m_address; }

      /// Add reference to object
      virtual       unsigned long   addRef    ()       { return 1;         }
      /// release reference to object
      virtual       unsigned long   release   ()       { return 1;         }
      /// Retrieve pointer to Transient Store
      virtual IDataProviderSvc*     dataSvc   () const { return 0;         }
      /// Retrieve object behind the link
      virtual DataObject*           object    () const { return m_object;  }
      /// Retrieve opaque storage address
      virtual IOpaqueAddress*       address   () const { return m_address; }
      /// Name of the directory (or key)
      virtual const name_type&      name      () const;
      /// Full identifier (or key)
      virtual const id_type&        identifier() const;
      /// Set/Update Opaque storage address
      virtual void setAddress (IOpaqueAddress* pAddr);      
    };

    /// Pointer to data loader service
    IConversionSvc*                 m_dataLoader;
    /// Pointer to incident service
    IIncidentSvc*                   m_incidentSvc;
    /// Property: Integer corresponding to CLID of root entry
    CLID                            m_rootCLID;
    /// Property: Name of root event
    std::string                     m_rootName;
    /// Property: Width of the SSE4 bit mask in bits
    int                             m_maskWidth;
    /// Property: Flag to enable interrupts on data access requests
    bool                            m_enableAccessHdlr;
    /// Property: Flag to enable interrupts on data creation requests
    bool                            m_enableFaultHdlr;
    /// Property: Name of the data access incident
    std::string                     m_accessName;
    /// Property: Name of the data fault incident
    std::string                     m_faultName;

    AVXHeap                         m_bits;
    std::vector<Entry>              m_registry;
    EventTag_t                      m_root;

    /** Invoke data fault handling if enabled
     * @param entry  [IN]   Missing registry entry
     *
     * @return Object corresponding to the specified leaf
     */
    DataObject* handleDataFault(Entry& entry);

    /// Shortcut to handle unsupported calls: Throws exception
    StatusCode unsupportedCall(const std::string& call) const;

  public:
    /// Standard Constructor
    EventDataSvc(const std::string& name, ISvcLocator* svc);

    /// Standard Destructor
    virtual ~EventDataSvc();

    /// Service initialization
    virtual StatusCode initialize();

    /// Service initialization
    virtual StatusCode finalize();

    /// IDataManagerSvc: IDataManagerSvc: Pass a default data loader to the service.
    virtual StatusCode setDataLoader(IConversionSvc* svc);

    /// IDataManagerSvc: Remove all data objects in the data store.
    virtual StatusCode clearStore();

    /// Retrieve customizable data loader according to registry entry to be retrieved
    virtual IConversionSvc* getDataLoader(IRegistry* pReg);

    /** Invoke Persistency service to create transient object from its
     *  persistent representation
     */
    int loadObject(Entry& entry);

    /// Register object with the data store.
    virtual StatusCode put(const EventTag_t& tag, DataObject* pObject);

    /// Register object address with the data store.
    virtual StatusCode put(const EventTag_t& tag, IOpaqueAddress* pObject);

    /// Unregister object from the data store.
    virtual StatusCode unregister(const EventTag_t& tag);

    /// Add a link to another object.
    virtual StatusCode link(const EventTag_t& from, const EventTag_t& to);

    /// Remove a link to another object.
    virtual StatusCode unlink(const EventTag_t& tag);

    /// Retrieve object from data store.
    virtual StatusCode retrieve(const EventTag_t& tag, DataObject*& pObject);

    void* get(const EventTag_t& tag, Cast_t cast);
    void* retrieve(const EventTag_t& tag, Cast_t cast);

    /** 
     *  Supported calls from the IDataManagerSvc interface
     */

    /// Analyse by traversing all data objects in the data store.
    virtual StatusCode traverseTree(IDataStoreAgent* pAgent);

    /// Initialize data store for new event by giving new event path.
    virtual StatusCode setRoot(const std::string& root_name,DataObject* pObject);

    /// Initialize data store for new event by giving new event path.
    virtual StatusCode setRoot(const std::string& root_path,IOpaqueAddress* pRootAddr);

    /// Get class ID of root Event
    virtual CLID rootCLID() const;

    /// Get Name of root Event
    virtual std::string rootName() const;

    /**
     *  Unsupported calls from the IDataManagerSvc interface
     */    

    /// IDataManagerSvc: Explore the object store: retrieve the object's parent.
    virtual StatusCode objectParent(const DataObject*,IRegistry*&)
    {  return unsupportedCall("StatusCode objectParent(const DataObject*,IRegistry*&)"); }

    /// IDataManagerSvc: Explore the object store: retrieve the object's parent.
    virtual StatusCode objectParent(const IRegistry*,IRegistry*&)
    {  return unsupportedCall("StatusCode objectParent(const IRegistry*,IRegistry*&)");  }

    /// Explore the object store: retrieve all leaves attached to the object
    virtual StatusCode objectLeaves(const DataObject*,std::vector<IRegistry*>&)
    {  return unsupportedCall("StatusCode objectLeaves(const DataObject*,vector<IRegistry*>&)");  }

    /// Explore the object store: retrieve all leaves attached to the object
    virtual StatusCode objectLeaves(const IRegistry*,std::vector<IRegistry*>&)
    {  return unsupportedCall("StatusCode objectLeaves(const IRegistry*,vector<IRegistry*>&)");  }

    /// Remove all data objects below the sub tree identified by its full path name.
    virtual StatusCode clearSubTree(const std::string&)
    {  return unsupportedCall("StatusCode clearSubTree(const string&)");  }

    /// Remove all data objects below the sub tree
    virtual StatusCode clearSubTree(DataObject*)
    {  return unsupportedCall("StatusCode clearSubTree(DataObject*)");  }

    /// Analyse by traversing all data objects below the sub tree identified by its full path name.
    virtual StatusCode traverseSubTree(const std::string&,IDataStoreAgent*)
    {  return unsupportedCall("StatusCode traverseSubTree(const std::string&,IDataStoreAgent*)");  }

    /// Analyse by traversing all data objects below the sub tree
    virtual StatusCode traverseSubTree(DataObject*,IDataStoreAgent*)
    {  return unsupportedCall("StatusCode traverseSubTree(DataObject*,IDataStoreAgent*)");  }

    /// Register object address with the data store.
    virtual StatusCode registerAddress(const std::string&,IOpaqueAddress*)
    {  return unsupportedCall("StatusCode registerAddress(const string&,IOpaqueAddress*)");  }

    /// Register object address with the data store.
    virtual StatusCode registerAddress( DataObject*,const std::string&,IOpaqueAddress*)
    {  return unsupportedCall("StatusCode registerAddress(DataObject*,const string&,IOpaqueAddress*)");  }

    /// Register object address with the data store.
    virtual StatusCode registerAddress( IRegistry*,const std::string&,IOpaqueAddress*)
    {  return unsupportedCall("StatusCode registerAddress(IRegistry*,const string&,IOpaqueAddress*)");  }

    /// Unregister object address from the data store.
    virtual StatusCode unregisterAddress(const std::string&)
    {  return unsupportedCall("StatusCode unregisterAddress(const string&)");  }

    /// Unregister object address from the data store.
    virtual StatusCode unregisterAddress(DataObject*,const std::string&)
    {  return unsupportedCall("StatusCode unregisterAddress(DataObject*,const string&)");  }

    /// Unregister object address from the data store.
    virtual StatusCode unregisterAddress(IRegistry*,const std::string&)
    {  return unsupportedCall("StatusCode unregisterAddress(IRegistry*,const string&)");  }

  };
}      // End namespace Gaudi
#endif // EVENTDATASVC__EVENTDATASVC_H
