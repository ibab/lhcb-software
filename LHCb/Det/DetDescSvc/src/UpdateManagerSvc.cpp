// $Id: UpdateManagerSvc.cpp,v 1.10 2007-01-23 07:16:24 cattanem Exp $
// Include files 

#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/IDetDataSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IEventProcessor.h"

#include "DetDesc/ValidDataObject.h"
#include "DetDesc/Condition.h"

// local
#include "UpdateManagerSvc.h"

#include "ConditionParser.h"

DECLARE_SERVICE_FACTORY( UpdateManagerSvc );

//-----------------------------------------------------------------------------
// Implementation file for class : UpdateManagerSvc
//
// 2005-03-30 : Marco Clemencic
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
UpdateManagerSvc::UpdateManagerSvc(const std::string& name, ISvcLocator* svcloc):
  Service(name,svcloc),m_dataProvider(NULL),m_detDataSvc(NULL),m_incidentSvc(NULL),m_evtProc(NULL),
  m_head_since(1),m_head_until(0)
{
#ifndef WIN32
  pthread_mutex_t tmp_lock = PTHREAD_MUTEX_INITIALIZER;
  m_busy = tmp_lock;
#endif
  declareProperty("DataProviderSvc", m_dataProviderName = "DetectorDataSvc");
  declareProperty("DetDataSvc",      m_detDataSvcName);
  declareProperty("ConditionsOverride", m_conditionsOveridesDesc);
}
//=============================================================================
// Destructor
//=============================================================================
UpdateManagerSvc::~UpdateManagerSvc() {
  // delete objects in the container
  for (Item::ItemList::const_iterator i = m_all_items.begin(); i != m_all_items.end(); ++i){
    delete *i;
  }
}

//=============================================================================
// IInterface implementation
//=============================================================================
StatusCode UpdateManagerSvc::queryInterface(const InterfaceID& riid, void** ppvUnknown){
  if ( IID_IUpdateManagerSvc.versionMatch(riid) ) {
    *ppvUnknown = (IUpdateManagerSvc*)this;
    addRef();
    return StatusCode::SUCCESS;
  } else if ( IID_IIncidentListener.versionMatch(riid) ) {
    *ppvUnknown = (IIncidentListener*)this;
    addRef();
    return StatusCode::SUCCESS;
  }
  return Service::queryInterface(riid,ppvUnknown);
}

//=============================================================================
// IService implementation
//=============================================================================
StatusCode UpdateManagerSvc::initialize(){
  // base class initialization
  StatusCode sc = Service::initialize();
  if (!sc.isSuccess()) return sc;
  // local initialization
  MsgStream log(msgSvc(),name());
  log << MSG::DEBUG << "--- initialize ---" << endmsg;

  // find the data provider
  sc = serviceLocator()->service<IDataProviderSvc>(m_dataProviderName,m_dataProvider,true);
  if (!sc.isSuccess()) {
    log << MSG::ERROR << "Unable to get a handle to the data provider" << endmsg;
    return sc;
  } else {
    log << MSG::DEBUG << "Got pointer to IDataProviderSvc \"" << m_dataProviderName << '"' << endmsg;
    IDataManagerSvc * dMgr;
    sc = m_dataProvider->queryInterface(IDataManagerSvc::interfaceID(),(void **) &dMgr);
    if ( sc.isSuccess() ) {
      m_dataProviderRootName = dMgr->rootName() + "/";
      dMgr->release();
      if (!sc.isSuccess()) {
        return sc;
      }
    }
    else {
      log << MSG::WARNING << "Cannot access IDataManagerSvc interface of \"" << m_dataProviderName
          << "\": using empty RootName" << endmsg;
      m_dataProviderRootName = "";
    } 
  }

  // find the detector data service
  if (m_detDataSvcName == "") m_detDataSvcName = m_dataProviderName;
  sc = serviceLocator()->service(m_detDataSvcName,m_detDataSvc,true);
  if (!sc.isSuccess()) {
    log << MSG::WARNING << "Unable to get a handle to the detector data service interface:"
      " all the calls to newEvent(void) will fail!" << endmsg;
    m_detDataSvc = NULL;
  } else {
    log << MSG::DEBUG << "Got pointer to IDetDataSvc \"" << m_detDataSvcName << '"' << endmsg;
  }
  
  // before registering to the incident service I have to be sure that the EventClockSvc is ready
  IService *evtClockSvc;
  sc = service("EventClockSvc", evtClockSvc, true);
  if ( sc.isSuccess() ) {
    log << MSG::DEBUG << "Good: EventClockSvc found" << endmsg;
    evtClockSvc->release();
  } else {
    log << MSG::WARNING << "Unable find EventClockSvc, probably I'll not work." << endmsg;
  }  

  // register to the incident service for BeginEvent incidents
  sc = service("IncidentSvc", m_incidentSvc, false);
  if ( sc.isSuccess() ) {
    m_incidentSvc->addListener(this,IncidentType::BeginEvent);
    log << MSG::DEBUG << "Got pointer to IncidentSvc" << endmsg;
  } else {
    log << MSG::ERROR << "Unable to register to the incident service." << endmsg;
    m_incidentSvc = NULL;
    return sc;
  }

  sc = serviceLocator()->service("ApplicationMgr",m_evtProc);
  if ( !sc.isSuccess() ) {
    log << MSG::ERROR << "Cannot find an event processor." << endmsg;
    return sc;
  }

  // Loop over overridden conditions
  for ( std::vector<std::string>::iterator co = m_conditionsOveridesDesc.begin();
        co != m_conditionsOveridesDesc.end(); ++co ) {
    std::string name;
    Condition *cond = new Condition();
    if (ConditionParser(*co,name,*cond)) {
      
      // Remove TS root name from the path
      if ( name[0] == '/'
           && name.compare(0,m_dataProviderRootName.size(),m_dataProviderRootName) == 0 ){
        name.erase(0,m_dataProviderRootName.size());
      }
      
      // If a condition override with that name already exists, delete it
      Condition * dest = m_conditionsOverides[name];
      if ( dest ) {
        log << MSG::WARNING << "Override condition for path '" << name
            << "' is defined more than once (I use the last one)." << endmsg;
        delete dest;
      }
      
      // Add the condition to internal list
      m_conditionsOverides[name] = cond;
      log << MSG::DEBUG << "Added condition: " << name << "\n" << cond->printParams() << endmsg;
      
    } else {
      // something went wrong while parsing: delete the temporary
      delete cond;
      log << MSG::ERROR << "Cannot understand condition:" << endmsg;
      log << MSG::ERROR << *co << endmsg;
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode UpdateManagerSvc::finalize(){
  // local finalization
  
  MsgStream log(msgSvc(),name());
  log << MSG::DEBUG << "--- finalize ---" << endmsg;

  if (msgSvc()->outputLevel() <= 1) dump();

  // release the interfaces used
  if (m_dataProvider != NULL) m_dataProvider->release();
  if (m_detDataSvc != NULL) m_detDataSvc->release();
  if (m_incidentSvc != NULL) {
    // unregister from the incident svc
    m_incidentSvc->removeListener(this,IncidentType::BeginEvent);
    m_incidentSvc->release();
  }
  if (m_evtProc != NULL) m_evtProc->release();

  // delete unused overridden conditions (the others are deleted together with the T.S.)
  if ( ! m_conditionsOverides.empty() ) {
    log << MSG::WARNING << "Few overrided conditions were not used:" << endmsg;
    for (GaudiUtils::Map<std::string,Condition*>::iterator c = m_conditionsOverides.begin();
         c != m_conditionsOverides.end(); ++c ) {
      log << MSG::WARNING << c->first << endmsg;
      delete c->second;
    }
  }

  // base class finalization
  return Service::finalize();
}
//=============================================================================
// IUpdateManagerSvc implementation
//=============================================================================
IDataProviderSvc *UpdateManagerSvc::dataProvider() const {
  return m_dataProvider;
}
IDetDataSvc *UpdateManagerSvc::detDataSvc() const {
  return m_detDataSvc;
}
void UpdateManagerSvc::i_registerCondition(const std::string &condition, BaseObjectMemberFunction *mf,
                                           BasePtrSetter *ptr_dest){
  MsgStream log(msgSvc(),name());

  std::string cond_copy(condition);

  if (!cond_copy.empty()) {
    // remove the root name if present
    if ( cond_copy[0] == '/'
         && cond_copy.compare(0,m_dataProviderRootName.size(),m_dataProviderRootName) == 0 ){
      cond_copy.erase(0,m_dataProviderRootName.size());
    }
    log << MSG::DEBUG << "registering condition \"" << cond_copy
        << "\" for object of type " << mf->type().name() << endmsg;
  }
  else {
    log << MSG::DEBUG << "registering object of type " << mf->type().name()
        << " (without condition)" << endmsg;
  }
  
  // find the object
  Item *mf_item = findItem(mf);
  if (!mf_item){ // a new OMF
    mf_item = new Item(mf);
    m_all_items.push_back(mf_item);
    m_head_items.push_back(mf_item); // since it is new, it has no parents
  }
  if (!cond_copy.empty()) {
    // find the condition
    Item *cond_item = findItem(cond_copy);
    if (!cond_item){ // a new condition

      // Check if the requested condition is in the override list.
      GaudiUtils::Map<std::string,Condition*>::iterator cond_ov = m_conditionsOverides.find(cond_copy);
      if ( cond_ov != m_conditionsOverides.end() ) {
        // yes, it is!
        cond_item = new Item(cond_copy,ptr_dest,cond_ov->second);
        // I do not need it anymore in the list
        m_conditionsOverides.erase(cond_ov);
      } else {
        // no override
        cond_item = new Item(cond_copy,ptr_dest);
      }

      m_all_items.push_back(cond_item);
      
    } else {
      if (ptr_dest){
        // I already have this condition registered, but a new user wants to set the pointer to it.
        cond_item->user_dest_ptrs.push_back(ptr_dest);
        // Let's check if the object is already loaded (the pointers are set by Item only when it loads them)
        if (cond_item->vdo) { ptr_dest->set(cond_item->vdo); }
      }
      if (cond_item->isHead()) removeFromHead(cond_item);
    }
    link(mf_item,mf,cond_item);
  } else {
    // this is usually done inside Item::addChild (called by "link")
    Item::MembFuncList::iterator mfIt = mf_item->find(mf);
    if (mfIt == mf_item->memFuncs.end()) {
      // I do not have the MF registered inside the item
      // so I add it
      mf_item->memFuncs.insert(mfIt,Item::MembFunc(mf));
    } else {
      // the MF is already there
      if (mfIt->mf != mf) // but it has a different pointer
        // so I do not need to keep the copy I have
        delete mf;
    }
  }
  // a new item means that we need an update
  m_head_since = 1;
  m_head_until = 0;
}
void UpdateManagerSvc::i_registerCondition(void *obj, BaseObjectMemberFunction *mf){
  MsgStream log(msgSvc(),name());
  log << MSG::DEBUG << "registering object at " << std::hex << obj << std::dec
      << " for object of type " << mf->type().name() << endmsg;
  // find the "condition"
  Item *cond_item = findItem(obj);
  if (!cond_item){ // Error!!!
    throw UpdateManagerException("tried to register for an object not in the UpdateManagerSvc");
  } else {
    if (cond_item->isHead()) removeFromHead(cond_item);
    }
  // find the OMF (Object Member Function)
  Item *mf_item = findItem(mf);
  if (!mf_item){ // a new OMF
    mf_item = new Item(mf);
    m_all_items.push_back(mf_item);
    m_head_items.push_back(mf_item); // since it is new, it has no parents
  }
  link(mf_item,mf,cond_item);
  // a new item means that we need an update
  m_head_since = 1;
  m_head_until = 0;
}
StatusCode UpdateManagerSvc::newEvent(){
  if (detDataSvc() != NULL){
    if (detDataSvc()->validEventTime()) {
      return newEvent(detDataSvc()->eventTime());
    } else {
      MsgStream log(msgSvc(),name());
      log << MSG::WARNING << "newEvent(): the event time is not defined!" << endmsg;
    }
  }
  return StatusCode::FAILURE;
}
StatusCode UpdateManagerSvc::newEvent(const Gaudi::Time &evtTime){
  StatusCode sc = StatusCode::SUCCESS;

#ifndef WIN32
  MsgStream log(msgSvc(),name());
  
  log << MSG::VERBOSE << "newEvent(evtTime): acquiring mutex lock" << endmsg;
  acquireLock();
#endif

  // Check head validity
  if ( evtTime >= m_head_since && evtTime < m_head_until ) {
#ifndef WIN32
    log << MSG::VERBOSE << "newEvent(evtTime): releasing mutex lock" << endmsg;
    releaseLock();
#endif    
    return sc; // no need to update
  }

#ifndef WIN32
  try {
#endif
  Item::ItemList::iterator it;

  // The head list may change while updating, I'll loop until it's stable (or a problem occurs)
  bool head_has_changed = false;
  do {
    if ( m_outputLevel <= MSG::DEBUG ) {
      MsgStream log(msgSvc(),name());
      log << MSG::DEBUG << "newEvent(evtTime): loop over head items" << endmsg;
    }
    // first I make a copy of the current head
    Item::ItemList head_copy(m_head_items);
    // Start from a clean IOV (I cannot use m_head_X because the head is not stable and they may change)
    Gaudi::Time head_copy_since(Gaudi::Time::epoch());
    Gaudi::Time head_copy_until(Gaudi::Time::max());
    for (it = head_copy.begin(); it != head_copy.end() && sc.isSuccess(); ++it){
      if ( m_outputLevel <= MSG::DEBUG ) {
        MsgStream item_log(msgSvc(),name()+"::Item");
        sc = (*it)->update(dataProvider(),evtTime,&item_log);
      } else {
        sc = (*it)->update(dataProvider(),evtTime);       
      }
      if (sc.isSuccess()) {
        if ( head_copy_since < (*it)->since )  head_copy_since = (*it)->since;
        if ( head_copy_until > (*it)->until )  head_copy_until = (*it)->until;
      }
    }
    // now it is safe to set m_head_X
    m_head_since = head_copy_since;
    m_head_until = head_copy_until;

    // check if we need to re-do the loop (success and a change in the head)
    head_has_changed = sc.isSuccess() && (head_copy != m_head_items);
  } while ( head_has_changed );

#ifndef WIN32
  } catch (...) {
    log << MSG::VERBOSE << "newEvent(evtTime): releasing mutex lock (exception occurred)" << endmsg;
    releaseLock();
    throw;
  }

  log << MSG::VERBOSE << "newEvent(evtTime): releasing mutex lock" << endmsg;
  releaseLock();
#endif
  
  return sc;
}
StatusCode UpdateManagerSvc::i_update(void *instance){
  if ( m_outputLevel <= MSG::DEBUG ) {
    MsgStream log(msgSvc(),name());
    log << MSG::DEBUG << "Update specific object at " << instance << endmsg;
  }
  if (detDataSvc() != NULL){
    if (detDataSvc()->validEventTime()) {
      Item *item = findItem(instance);
      if (item) {
        StatusCode sc;
        if ( m_outputLevel <= MSG::DEBUG ) {
          MsgStream item_log(msgSvc(),name()+"::Item");
          sc = item->update(dataProvider(),detDataSvc()->eventTime(),&item_log);
        } else {
          sc = item->update(dataProvider(),detDataSvc()->eventTime());
        }
        if (sc.isSuccess()) {
          if ( m_head_since < item->since )  m_head_since = item->since;
          if ( m_head_until > item->until )  m_head_until = item->until;
        }
        return sc;
      }
    } else {
      return StatusCode::SUCCESS;
    }
  }
  return StatusCode::FAILURE;
}
void UpdateManagerSvc::i_invalidate(void *instance){
  if ( m_outputLevel <= MSG::DEBUG ) {
    MsgStream log(msgSvc(),name());
    log << MSG::DEBUG << "Invalidate object at " << instance << endmsg;
  }
  Item *item = findItem(instance);
  if (item) {
    item->invalidate();
    m_head_since = 1;
    m_head_until = 0;
  }
}

void UpdateManagerSvc::i_unregister(void *instance){
  if ( m_outputLevel <= MSG::DEBUG ) {
    MsgStream log(msgSvc(),name());
    log << MSG::DEBUG << "Unregister object at " << instance << endmsg;
  }
  Item *item = findItem(instance);
  if (item){
    // remove from parents
    for ( Item::ParentList::iterator p = item->parents.begin(); p != item->parents.end(); ++p ){
      Item::MembFuncList::iterator p_mf = p->first->find(p->second);
      Item::ItemList *mfInternalList = p_mf->items;
      Item::ItemList::iterator entry = std::find(mfInternalList->begin(),mfInternalList->end(),item);
      mfInternalList->erase(entry);
      p->first->children.erase(std::find(p->first->children.begin(),p->first->children.end(),item));
    }

    //  loop over unique children
    for (Item::ItemList::iterator c = item->children.begin(); c != item->children.end(); ++c){
      bool was_head = (*c)->isHead();
      for (Item::ParentList::iterator c_p = (*c)->parents.begin(); c_p != (*c)->parents.end(); ){
        if (c_p->first == item) { // removing connections to "item"
          c_p = (*c)->parents.erase(c_p);
        } else {
          ++c_p;
        }
      }
      if ((*c)->isHead() && !was_head){
        m_head_items.push_back(*c);
      }
    }
    if (item->isHead()) removeFromHead(item);
    m_all_items.erase(std::find(m_all_items.begin(),m_all_items.end(),item));
    delete item;
  }
}

void UpdateManagerSvc::dump(){
  MsgStream log(msgSvc(),name());

  log << MSG::DEBUG << "--- Dump" << endmsg;
  log << MSG::DEBUG << "    " << m_all_items.size() << " items registered" << endmsg;
  log << MSG::DEBUG << "     of which " << m_head_items.size() << " in the head" << endmsg;
  log << MSG::DEBUG << "         head IOV = " << m_head_since << " - " << m_head_until << endmsg;
  
  size_t cnt = 0, head_cnt = 0;
  for (Item::ItemList::iterator i = m_all_items.begin(); i != m_all_items.end(); ++i){
    log << MSG::DEBUG << "--item " << cnt++ << " " << std::hex << *i << std::dec;
    if ((*i)->isHead()){
      log << " (head)";
      ++head_cnt;
    }
    log << endmsg;
    log << MSG::DEBUG << "       ptr  = " << std::hex << (*i)->ptr << std::dec << endmsg;
    if ( !(*i)->path.empty() )
      log << MSG::DEBUG << "       path = " << (*i)->path << endmsg;
    log << MSG::DEBUG << "        IOV = " << (*i)->since << " - " << (*i)->until << endmsg;
    if ((*i)->memFuncs.size()){
      log << MSG::DEBUG << "       depend on :" << endmsg;
      for (Item::MembFuncList::iterator mfIt = (*i)->memFuncs.begin(); mfIt != (*i)->memFuncs.end(); ++mfIt){
        log << MSG::DEBUG << std::hex << "                  ";
        for (Item::ItemList::iterator itemIt = mfIt->items->begin(); itemIt != mfIt->items->end(); ++itemIt){
          log << " " << *itemIt;
        }
        log << std::dec << endmsg;
      }
    }
  }
  log << MSG::DEBUG << "Found " << head_cnt << " head items: ";
  if (m_head_items.size() == head_cnt){
    log << "OK";
  } else {
    log << "MISMATCH!!!!!";
  }
  log << endmsg;
}

//=========================================================================
//  search the item with the given path and get its validity
//=========================================================================
bool UpdateManagerSvc::getValidity(const std::string path, Gaudi::Time& since, Gaudi::Time &until,
                                   bool path_to_db) {
  // search
  Item *item = findItem(path,path_to_db);
  if (item) {
    // copy IOV limits
    since = item->since;
    until = item->until;
    return true;
  }
  return false;
}
//=========================================================================
//  search the item with the given path and change its validity
//=========================================================================
void UpdateManagerSvc::setValidity(const std::string path, const Gaudi::Time& since, const Gaudi::Time &until,
                                   bool path_to_db) {
  if (!path_to_db) { // the DDS path is unique
    // search
    Item *item = findItem(path,path_to_db);
    if (item) {
      // set the validity and propagate up
      item->changeValidity(since,until);
      // if the object has already been loaded we should also change its validity
      if (item->vdo) item->vdo->setValidity(since,until);
    }
  } else { // a CondDB path can contain many objects
    Item::ItemList::iterator i = m_all_items.begin();
    while ( i !=  m_all_items.end() ) {
      if ( (*i)->match(path,path_to_db) ) {
        // set the validity and propagate up
        (*i)->changeValidity(since,until);
        // if the object has already been loaded we should also change its validity
        if ((*i)->vdo) (*i)->vdo->setValidity(since,until);
      }
      ++i;
    }
  }
  // adjust head validity
  if ( m_head_since < since ) m_head_since = since;
  if ( m_head_until > until ) m_head_until = until;
}

//=========================================================================
//  Handle BeginEvent incident
//=========================================================================
void UpdateManagerSvc::handle(const Incident &inc) {
  if ( inc.type() == IncidentType::BeginEvent ) {
    MsgStream log( msgSvc(), name() );
    log << MSG::DEBUG << "New BeginEvent incident received" << endmsg;
    StatusCode sc = UpdateManagerSvc::newEvent();
    if (!sc.isSuccess()) {
      log << MSG::FATAL << "***** The update failed. I schedule a stop of the run *****" << endmsg;
      m_evtProc->stopRun();
      // The exception is ignored by the IncidentSvc
      // throw UpdateManagerException("Failed to preform the update","*UpdateManagerSvc*",sc);
    }
  }
}

//=========================================================================
//  Locking functionalities
//=========================================================================
void UpdateManagerSvc::acquireLock(){
#ifndef WIN32
  pthread_mutex_lock(&m_busy);
#endif
}
void UpdateManagerSvc::releaseLock(){
#ifndef WIN32
  pthread_mutex_unlock(&m_busy);
#endif
}

//=============================================================================
