// $Id: UpdateManagerSvc.h,v 1.13 2009-01-16 12:03:46 ocallot Exp $
#ifndef UPDATEMANAGERSVC_H
#define UPDATEMANAGERSVC_H 1

// Include files
#include "GaudiKernel/Service.h"
#include "GaudiKernel/Time.h"
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/MsgStream.h"

#include "GaudiKernel/IUpdateManagerSvc.h"
#include "GaudiKernel/UpdateManagerException.h"

#include "GaudiKernel/Map.h"

#include "GaudiKernel/HashMap.h"

#include <string>
#include <map>
#include <exception>
#include <algorithm>

#ifndef WIN32
#include <pthread.h>
#endif

// Forward declarations
template <class TYPE> class SvcFactory;
class IIncidentSvc;
class IEventProcessor;
class Condition;

/** @class UpdateManagerSvc UpdateManagerSvc.h
 *
 *
 *  @author Marco Clemencic
 *  @date   2005-03-30
 */
class UpdateManagerSvc: public virtual extends2<Service,
                        						IUpdateManagerSvc,
                        						IIncidentListener> {
public:
  /// Standard constructor
  UpdateManagerSvc(const std::string& name, ISvcLocator* svcloc);

  virtual ~UpdateManagerSvc( ); ///< Destructor

  /// Initialize Service
  virtual StatusCode initialize();

  /// Stop Service.
  /// Dump the status of the network of dependencies.
  virtual StatusCode stop();

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
  virtual StatusCode newEvent(const Gaudi::Time &evtTime);

  //virtual StatusCode runAll() const;

  virtual bool getValidity(const std::string path, Gaudi::Time& since, Gaudi::Time &until, bool path_to_db = false);
  virtual void setValidity(const std::string path, const Gaudi::Time& since, const Gaudi::Time &until, bool path_to_db = false);

  /// Debug method: it dumps the dependency network through the message service (not very readable, for experts only).
  virtual void dump();

  /// Force the update manager service to wait before entering the newEvent loop.
  virtual void acquireLock();
  /// Let the update manager service enter the newEvent loop.
  virtual void releaseLock();

  /// Remove all the items referring to objects present in the transient store.
  /// This is needed when the Detector Transient Store is purged, otherwise we
  /// will keep pointers to not existing objects.
  virtual void purge();

  // ---- Implement IIncidentListener interface ----
  /// Handle BeginEvent incident.
  virtual void handle(const Incident &inc);

protected:

  /// Register a condition for an object
  //  virtual StatusCode i_registerCondition(const std::string &condition, BaseObjectMemberFunction *mf);

  /// Register a condition for an object together with the destination for the pointer to the condition object.
  virtual void i_registerCondition(const std::string &condition, BaseObjectMemberFunction *mf,
                                         BasePtrSetter *ptr_dest = NULL);

  /// Register a condition for an object
  virtual void i_registerCondition(void *obj, BaseObjectMemberFunction *mf);

  /// Used to force an update of the given instance (e.g. when the object is created during an event).
  virtual StatusCode i_update(void *instance);

  /// Used to remove an object from the dependency network.
  /// \warning{Removing an object is dangerous}
  virtual void i_unregister(void *instance);

  /// Force an update of all the object depending on the given one for the next event.
  virtual void i_invalidate(void *instance);

private:

#include "UpdateManagerSvc_Item.icpp"

  /// Hashmap for fast string access
  GaudiUtils::HashMap<std::string, Item*> m_pathMap;

  void insertInMap( Item* it ) {
    const std::pair<const std::string,Item*> tempS( it->path, it );
    m_pathMap.insert( tempS );
  }

  /// Connects two items in a parent-child relationship through the give member function.
  inline void link(Item* parent, BaseObjectMemberFunction *mf, Item *child);

  // Disconnect a parent from the child.
  void unlink(Item *parent, Item *child);

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

  /// Name of the root node of the Transient Store.
  std::string       m_dataProviderRootName;

  /// Handle to the IDetDataSvc interface (used to get the event time).
  /// If the service is not found it is not fatal, but you cannot use the method newEvent()
  /// without the event time parameter (will always fail).
  IDetDataSvc      *m_detDataSvc;

  /// Name of the DetDataSvc (set by the option DetDataSvc, by default empty, which means <i>the same as data provider</i>).
  std::string       m_detDataSvcName;

  /// Pointer to the incident service;
  IIncidentSvc     *m_incidentSvc;

  /// Pointer to the event processor in order to be able to top the run if something goes wrpong during an update.
  IEventProcessor  *m_evtProc;

  /// List used to keep track of all the registered items.
  Item::ItemList    m_all_items;
  /// List used to record all the objects without parents. (for fast access)
  Item::ItemList    m_head_items;
  /// Lower bound of intersection of head IOVs.
  Gaudi::Time       m_head_since;
  /// Higher bound of intersection of head IOVs.
  Gaudi::Time       m_head_until;

  /// List of condition definitions to override the ones in the transient store (option ConditionsOverride).
  /// The syntax to define a condition is:<BR>
  /// path := type1 name1 = value1; type2 name2 = value2; ...
  std::vector<std::string> m_conditionsOveridesDesc;
  /// Map containing the list of parsed condition definitions
  GaudiUtils::Map<std::string,Condition*> m_conditionsOverides;

  /// Name of the dot (graphviz) file into which write the dump (http://www.graphviz.org)
  /// (property DotDumpFile).
  std::string m_dotDumpFile;

#ifndef WIN32
  /// mutex lock used to avoid dependencies corruptions in a multi-thread environment.
  pthread_mutex_t m_busy;
#endif

};

#include "UpdateManagerSvc.icpp"

#endif // UPDATEMANAGERSVC_H
