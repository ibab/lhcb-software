// $Id: UpdateManagerSvc.h,v 1.6 2005-07-08 15:16:34 marcocle Exp $
#ifndef UPDATEMANAGERSVC_H 
#define UPDATEMANAGERSVC_H 1

// Include files
#include "GaudiKernel/Service.h"
#include "GaudiKernel/TimePoint.h"
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IIncidentListener.h"

#include "DetDesc/IUpdateManagerSvc.h"

#include <string>
#include <map>
#include <exception>
#include <algorithm>

// Forward declarations
template <class TYPE> class SvcFactory;
class IIncidentSvc;

/** @class UpdateManagerSvc UpdateManagerSvc.h
 *  
 *
 *  @author Marco Clemencic
 *  @date   2005-03-30
 */
class UpdateManagerSvc: public virtual Service, public virtual IUpdateManagerSvc, public virtual IIncidentListener {
public: 
  /// Standard constructor
  UpdateManagerSvc(const std::string& name, ISvcLocator* svcloc); 

  virtual ~UpdateManagerSvc( ); ///< Destructor

  /** Query interfaces (\see{IInterface})
      @param riid       ID of Interface to be retrieved
      @param ppvUnknown Pointer to Location for interface pointer
  */
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvUnknown);

  /// Initialize Service
  virtual StatusCode initialize();
  
  /// Finalize Service
  virtual StatusCode finalize();

  /// Return the pointer to the data provider service, used to retrieve objects.
  virtual IDataProviderSvc *dataProvider() const;
  /// Return the pointer to the detector data service, used to obtain the event time..
  virtual IDetDataSvc *detDataSvc() const;
  
  /// Start a the update loop getting the time to use from the detector data service.
  virtual StatusCode newEvent();
  /// Start a the update loop using the provided time to decide if an item is valid or not.
  /// \warning{The time used to retrieve an object from the condition database is the one obtained from 
  /// the detector data service.}
  virtual StatusCode newEvent(const ITime &evtTime);

  //virtual StatusCode runAll() const;

  virtual bool getValidity(const std::string path, TimePoint& since, TimePoint &until, bool path_to_db = false);
  virtual void setValidity(const std::string path, const TimePoint& since, const TimePoint &until, bool path_to_db = false);

  /// Debug method: it dumps the dependency network through the message service (not very readable, for experts only).
  virtual void dump();

  // ---- Implement IIncidentListener interface ----
  /// Handle BeginEvent incident.
  virtual void handle(const Incident &inc);

protected:

  /// Register a condition for an object
  virtual StatusCode registerCondition(const std::string &condition, BaseObjectMemberFunction *mf);

  /// Register a condition for an object
  virtual StatusCode registerCondition(void *obj, BaseObjectMemberFunction *mf);

  /// Used to force an update of the given instance (ex. when the object is created during an event).
  virtual StatusCode update(void *instance);

  /// Used to remove an object from the dependency network. 
  /// \warning{Removing an object is dangerous}
  virtual StatusCode unregister(void *instance);

  /// Force an update of all the object depending on the given one for the next event.
  virtual void       invalidate(void *instance);

private:

#include "UpdateManagerSvc_Item.icpp"

  /// Connects two items in a parent-child relationship through the give member function.
  inline void link(Item* parent, BaseObjectMemberFunction *mf, Item *child);

  /// Finds the item matching the given path.
  inline Item *findItem(const std::string &path, bool is_path_to_db = false) const;
   
  /// Finds the item matching the given pointer.
  inline Item *findItem(void *p) const;
  
  /// Finds the item containing the given member function.
  inline Item *findItem(BaseObjectMemberFunction *mf) const;

  /// Removes an item from the list of head items.
  inline void removeFromHead(Item *item);

  /// Allow SvcFactory to instantiate the service.
  friend class SvcFactory<UpdateManagerSvc>;

  // ---------- data members ----------
  /// Handle to the Data Provider (where to find conditions).
  IDataProviderSvc *m_dataProvider;
  /// Name of the Data Provider (set by the option DataProviderSvc, by default "DetectorDataSvc").
  std::string       m_dataProviderName;
  /// Handle to the IDetDataSvc interface (used to get the event time).
  /// If the service is not found it is not fatal, but you cannot use the method newEvent()
  /// without the event time parameter (will always fail).
  IDetDataSvc      *m_detDataSvc;
  /// Name of the DetDataSvc (set by the option DetDataSvc, by default empty, which means <i>the same as data provider</i>).
  std::string       m_detDataSvcName;

  /// Pointer to the incident service;
  IIncidentSvc     *m_incidentSvc;

  /// List used to keep track of all the registered items.
  Item::ItemList    m_all_items;
  /// List used to record all teh objects without parents. (for fast access)
  Item::ItemList    m_head_items;
  /// Lower bound of intersection of head IOVs.
  TimePoint         m_head_since;
  /// Higher bound of intersection of head IOVs.
  TimePoint         m_head_until;
};

#include "UpdateManagerSvc.icpp"

#endif // UPDATEMANAGERSVC_H
