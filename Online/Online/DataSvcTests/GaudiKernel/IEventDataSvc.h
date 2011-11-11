// $Id: IEventDataSvc.h,v 1.6 2005/01/19 08:23:16 mato Exp $
#ifndef GAUDIKERNEL_IEVENTDATASVC_H
#define GAUDIKERNEL_IEVENTDATASVC_H

// Framework include files
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/EventItemID.h"

// Forward declarations
class DataObject;
class IOpaqueAddress;

/*
 *    Gaudi namespace declaration
 */
namespace Gaudi {

  /** @class IEventDataSvc IEventDataSvc.h GaudiKernel/IEventDataSvc.h
   *
   *  Event Data servive interface definition.
   *  The data provider interface allows to:
   *  <UL>
   *  <LI> Register objects from the data store. Once objects are registered
   *       with the data store the client gives up ownership.
   *  <LI> Unregister objects from the data store. Unregistering an object
   *       means to actually delete all entries hanging below, not the object
   *       itself - since the client claims back ownership. Note that this is a
   *       very delicate operation - any reference to the object will be invalid.
   *       Even worse: invalid references cannot be detected.
   *  <LI> Retrieve objects from the data store. Depending ont he
   *       availibility of the requested object in the data store the
   *       represented object will be loaded if possible.
   *  <LI> Create 'soft' links between objects in the transient store.
   *  </UL>
   *
   *  @author   Markus Frank
   *  @version  1.0
   */
  class GAUDI_API IEventDataSvc: virtual public IInterface {
  public:
    /// InterfaceID
    DeclareInterfaceID(IEventDataSvc,1,0);

    /// Declaration of cast function type
    typedef void* (*Cast_t)(void*);

    /// Static cast function
    template <class T> static void* cast_obj(void* obj) {  return dynamic_cast<T*>((DataObject*)obj);  }

    /** Register object with the data store.
     *  Connect the object identified by its pointer to the node object
     *  identified by its path.
     *  @param      tag         Object identifier within the transient store
     *  @param      pObject     Pointer to the object to be registered.
     *
     *  @return                 Status code indicating success or failure.
     */
    virtual StatusCode put(const EventTag_t& tag, DataObject* pObject) = 0;

    /** Register object address with the data store.
     *  Connect the object address identified by its pointer to the 
     *  node object identified by its path.
     *  @param      tag         Object identifier within the transient store
     *  @param      pObject     Pointer to the object to be registered.
     *
     *  @return                 Status code indicating success or failure.
     */
    virtual StatusCode put(const EventTag_t& tag, IOpaqueAddress* pObject) = 0;

    /** Unregister object from the data store.
     *  On registration the client gives up ownership of the object and may no
     *  longer delete the object. unregistering the object is the opposite.
     *
     *  @param      tag         Path identifier within the transient store
     *
     *  @return                 Status code indicating success or failure.
     */
    virtual StatusCode unregister(const EventTag_t& tag) = 0;

    /** Add a link to another object.
     *  Both objects must already be registered with the data store.
     *  Once linked, the object can only be unregistered after unlinking.
     *
     *  @param      from        Tag to the object the link originates.
     *  @param      to          Tag to the object the link points to.
     *
     *  @return                 Status code indicating success or failure.
     */
    virtual StatusCode link(const EventTag_t& from, const EventTag_t& to) = 0;

    /** Remove a link to another object.
     *  Both objects must be registered with the data store.
     *  This entry point can be used to unlink objects e.g. in order to unregister
     *  them.
     *
     *  @param      tag         Link identifier to be removed from the transient store
     *
     *  @return                 Status code indicating success or failure.
     */
    virtual StatusCode unlink(const EventTag_t& tag) = 0;

    /** Retrieve object from data store.
     *  The object to be retrieved is identified by the pointer to the parent
     *  object and the relative path with respect to the node.
     *  In case the object is not present it will be loaded and converted
     *  if possible.
     *
     *  @param  tag         Object identifier
     *  @param  pObject     Reference to the pointer of the object to be returned.
     *
     *  @return             Status code indicating success or failure.
     */
    virtual StatusCode retrieve(const EventTag_t& tag, DataObject*& pObject) = 0;

    virtual void* get(const EventTag_t& tag, Cast_t cast) = 0;
    virtual void* retrieve(const EventTag_t& tag, Cast_t cast) = 0;

    template <typename T> T* get(const EventTag_t& tag) {
      return (T*)get(tag,cast_obj<T>);
    }

    template <typename T> T* retrieve(const EventTag_t& tag) {
      return (T*)retrieve(tag,cast_obj<T>);
    }

    /// Status code definitions
    enum Status  {
      /// Success
      SUCCESS = StatusCode::SUCCESS,
      /// Failure
      FAILURE = StatusCode::FAILURE,
      /// The path for this objects is already in use
      INVALID_OBJ_PATH = 2,
      /// Object pointer is invalid
      INVALID_OBJECT   = 4,
      /// Sorry, the requested object is not loaded
      OBJ_NOT_LOADED   = 6,
      /// No data loader available
      NO_DATA_LOADER   = 8,
      /// Invalid object address
      INVALID_OBJ_ADDR = 10,
      /// Invalid ROOT name
      INVALID_ROOT     = 12,
      /// Terminator
      LAST
    };
  };
}      // End namespace Gaudi
#endif // GAUDIKERNEL_IEVENTDATASVC_H
