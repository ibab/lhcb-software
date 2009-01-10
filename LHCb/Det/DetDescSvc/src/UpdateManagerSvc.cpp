// $Id: UpdateManagerSvc.cpp,v 1.22 2009-01-10 23:01:00 marcocle Exp $
// Include files

#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/IDetDataSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IEventProcessor.h"
#include "GaudiKernel/System.h"

#include "DetDesc/ValidDataObject.h"
#include "DetDesc/Condition.h"

#include <set>
#include <sstream>
#include <fstream>
#include <memory> // for auto_ptr with gcc 4.3

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
  declareProperty("DataProviderSvc",    m_dataProviderName = "DetectorDataSvc");
  declareProperty("DetDataSvc",         m_detDataSvcName);
  declareProperty("ConditionsOverride", m_conditionsOveridesDesc);
  declareProperty("DiaDumpFile",        m_diaDumpFile = "");
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

  if ( m_outputLevel <= MSG::DEBUG || ! m_diaDumpFile.empty() ) dump();

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
  if ( FSMState() < Gaudi::StateMachine::INITIALIZED ){
    throw GaudiException("Service offline","UpdateManagerSvc::registerCondition",StatusCode::FAILURE);
  }

  MsgStream log(msgSvc(),name());

  std::string cond_copy(condition);

  if (!cond_copy.empty()) {
    // remove the root name if present
    if ( cond_copy[0] == '/'
         && cond_copy.compare(0,m_dataProviderRootName.size(),m_dataProviderRootName) == 0 ){
      cond_copy.erase(0,m_dataProviderRootName.size());
    }
    log << MSG::DEBUG << "registering condition \"" << cond_copy
        << "\" for object of type " << System::typeinfoName(mf->type()) << " at " << std::hex << mf->castToVoid() << endmsg;
  }
  else {
    log << MSG::DEBUG << "registering object of type " << System::typeinfoName(mf->type())
        << " (without condition)" << endmsg;
  }

  // find the object
  Item *mf_item = findItem(mf);
  if (!mf_item){ // a new OMF
    mf_item = new Item(mf, m_dataProviderRootName);
    m_all_items.push_back(mf_item);
    m_head_items.push_back(mf_item); // since it is new, it has no parents
  } else {
    if ( ! mf_item->ptr ) { // the item is know but not its pointer (e.g. after a purge)
      mf_item->vdo = mf->castToValidDataObject();
      mf_item->ptr = mf->castToVoid();
    }
  }

  if (!cond_copy.empty()) {
    // find the condition
    Item *cond_item = findItem(cond_copy);
    if (!cond_item){ // a new condition

      // Check if the requested condition is in the override list.
      GaudiUtils::Map<std::string,Condition*>::iterator cond_ov = m_conditionsOverides.find(cond_copy);
      if ( cond_ov != m_conditionsOverides.end() ) {
        // yes, it is!
        cond_item = new Item(cond_copy,Item::UserPtrType(ptr_dest,mf_item->ptr),
                             cond_ov->second);
        // I do not need it anymore in the list
        m_conditionsOverides.erase(cond_ov);
      } else {
        // no override
        cond_item = new Item(cond_copy,Item::UserPtrType(ptr_dest,mf_item->ptr));
      }

      m_all_items.push_back(cond_item);

    } else {
      if (ptr_dest){
        // I already have this condition registered, but a new user wants to set the pointer to it.
        cond_item->user_dest_ptrs.push_back(Item::UserPtrType(ptr_dest,mf_item->ptr));
        // Let's check if the object is already loaded (the pointers are set by Item only when it loads them)
        if (cond_item->vdo) {
          ptr_dest->set(cond_item->vdo);
          if ( ptr_dest->isNull() ) { // the dynamic cast failed
            throw GaudiException("A condition in memory cannot be casted to the requested type",
                "UpdateManagerSvc::i_registerCondition", StatusCode::FAILURE );
          }
        }
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
  if ( FSMState() < Gaudi::StateMachine::INITIALIZED ){
    throw GaudiException("Service offline","UpdateManagerSvc::registerCondition",StatusCode::FAILURE);
  }
  MsgStream log(msgSvc(),name());
  log << MSG::DEBUG << "registering object at " << std::hex << obj << std::dec
      << " for object of type " << System::typeinfoName(mf->type()) << " at " << std::hex << mf->castToVoid() << endmsg;
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
    mf_item = new Item(mf, m_dataProviderRootName);
    m_all_items.push_back(mf_item);
    m_head_items.push_back(mf_item); // since it is new, it has no parents
  }
  if ( ! mf_item->ptr ) { // the item is know but not its pointer (e.g. after a purge)
    mf_item->vdo = mf->castToValidDataObject();
    mf_item->ptr = mf->castToVoid();
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
  if ( FSMState() < Gaudi::StateMachine::INITIALIZED ){
    throw GaudiException("Service offline","UpdateManagerSvc::newEvent",StatusCode::FAILURE);
  }

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
  if ( FSMState() < Gaudi::StateMachine::INITIALIZED ){
    throw GaudiException("Service offline","UpdateManagerSvc::update",StatusCode::FAILURE);
  }

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
      } else {
        MsgStream log(msgSvc(),name());
        log << MSG::WARNING << "Cannot find object at " << instance << endmsg;
      }
    } else {
      return StatusCode::SUCCESS;
    }
  }
  return StatusCode::FAILURE;
}
void UpdateManagerSvc::i_invalidate(void *instance){
  if ( FSMState() < Gaudi::StateMachine::INITIALIZED ){
    throw GaudiException("Service not initialized","UpdateManagerSvc::invalidate",StatusCode::FAILURE);
  }

  if ( m_outputLevel <= MSG::DEBUG ) {
    MsgStream log(msgSvc(),name());
    log << MSG::DEBUG << "Invalidate object at " << instance << endmsg;
  }
  Item *item = findItem(instance);
  if (item) {
    item->invalidate();
    m_head_since = 1;
    m_head_until = 0;
  } else {
    MsgStream log(msgSvc(),name());
    log << MSG::WARNING << "Cannot find object at " << instance << endmsg;
  }
}

void UpdateManagerSvc::unlink(Item *parent, Item *child){

  // check if the parent knows about the child
  Item::ItemList::iterator childIt = std::find(parent->children.begin(),
                                               parent->children.end(),child);
  if ( parent->children.end() == childIt )
    return; // parent does not know about child

  // remove from child all the user pointers belonging to the parent
  Item::UserPtrList::iterator pi = child->user_dest_ptrs.begin();
  while ( pi != child->user_dest_ptrs.end() ) {
    if (pi->second != parent) {
      pi = child->user_dest_ptrs.erase(pi);
    } else {
      ++pi;
    }
  }

  // If the child is used by a MF that uses other Items, we need to disconnect
  // them too.
  std::set<Item*> siblings; // list of Items used together with "child"

  // loop over child parent's pairs (mf,parent) to disconnect from them
  Item::MembFuncList::iterator p_mf;
  Item::ParentList::iterator p = child->parents.begin();
  while ( p != child->parents.end() ) {
    if (p->first != parent) {
      ++p;
      continue; // skip to next one
    }

    // find the MF inside the parent
    p_mf = parent->find(p->second);

    // find iterator to child in MF list ...
    Item::ItemList *mfInternalList = p_mf->items;
    Item::ItemList::iterator entry = std::find(mfInternalList->begin(),
                                               mfInternalList->end(),child);
    // ... and remove it (if found)
    if ( mfInternalList->end() != entry )
      mfInternalList->erase(entry);

    // append then other Items in the MF (to unlink them too)
    siblings.insert(mfInternalList->begin(),mfInternalList->end());

    // remove the parent pair from child
    p = child->parents.erase(p);
  }

  // unlink the siblings
  std::set<Item*>::iterator s;
  for ( s = siblings.begin(); s != siblings.end(); ++s ) {
    unlink(parent,*s);
  }

  // Check in the parent if there are MF without children: they have to be
  // removed.
  p_mf = parent->memFuncs.begin();
  while ( p_mf != parent->memFuncs.end() ) {
    if ( p_mf->items->empty() ) p_mf = parent->memFuncs.erase(p_mf);
    else ++p_mf;
  }

  // Remove child from parent's list of all children.
  // we have to look again for it because the iterator may have been made
  // invalid by the "unlink" of siblings
  childIt = std::find(parent->children.begin(),
                      parent->children.end(),child);
  if (childIt != parent->children.end())
    parent->children.erase(childIt);

  // check if the child should be part of the head now
  if ( child->isHead() ) {
    m_head_items.push_back(child);
  }

  // Note: I do not need to touch the validity because the it can only increase

}

void UpdateManagerSvc::i_unregister(void *instance){
  if ( FSMState() < Gaudi::StateMachine::INITIALIZED ){
    // un-registration is allowed after service finalize (no-op).
    if ( m_outputLevel <= MSG::VERBOSE ) {
      MsgStream log(msgSvc(),name());
      log << MSG::VERBOSE << "Trying to unregister object at " << instance
          << ", with the service OFFLINE"<< endmsg;
    }
    return;
  }

  if ( m_outputLevel <= MSG::DEBUG ) {
    MsgStream log(msgSvc(),name());
    log << MSG::DEBUG << "Unregister object at " << instance << endmsg;
  }

  Item *item = findItem(instance);
  if (item){

    // unlink from parents
    Item::ParentList::iterator p = item->parents.begin();
    while ( p != item->parents.end() ) {
      unlink(p->first,item);
      p = item->parents.begin();
    }

    // unlink from children
    Item::ItemList::iterator c = item->children.begin();
    while ( c != item->children.end() ) {
      unlink(item,(*c));
      c = item->children.begin();
    }

    // update the lists of Items
    if ( item->isHead() ) removeFromHead(item);
    m_all_items.erase(std::find(m_all_items.begin(),m_all_items.end(),item));

    // finally we can delete the Item
    delete item;
  }
}

void UpdateManagerSvc::dump(){
  if ( FSMState() < Gaudi::StateMachine::INITIALIZED ){
    throw GaudiException("Service offline","UpdateManagerSvc::dump",StatusCode::FAILURE);
  }

  MsgStream log(msgSvc(),name());

  std::auto_ptr<std::ofstream> dia_file;
  int dia_lines_ctr = 0;
  if ( ! m_diaDumpFile.empty() ){
    dia_file.reset(new std::ofstream(m_diaDumpFile.c_str()));
  }

  if (dia_file.get() != NULL) {
    // DIA header
    (*dia_file)
      << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
      << "<dia:diagram xmlns:dia=\"http://www.lysator.liu.se/~alla/dia/\">"
      << "<dia:layer name=\"Background\" visible=\"true\">";
  }

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

    if (dia_file.get() != NULL) {
      // DIA Object for registered item (first part)
      (*dia_file)
        << "<dia:object type=\"Flowchart - Box\" version=\"0\""
        << " id=\"i" << std::hex << *i << "\">"
        << "<dia:attribute name=\"text\"><dia:composite type=\"text\">"
        << "<dia:attribute name=\"string\"><dia:string>#"
        << "(" << std::dec << cnt-1 << ") " << std::hex << *i << "\n"
        << "(" << (*i)->ptr << ")";
    }

    log << MSG::DEBUG << "       ptr  = " << std::hex << (*i)->ptr << std::dec << endmsg;
    if ( !(*i)->path.empty() ) {
      log << MSG::DEBUG << "       path = " << (*i)->path << endmsg;
      if (dia_file.get() != NULL) {
        // If we have the path, we can put it in the DIA Object
        (*dia_file) << "\n" << (*i)->path;
      }
    }

    if (dia_file.get() != NULL) {
      // DIA Object for registered item (closure)
      (*dia_file) << "#</dia:string></dia:attribute></dia:composite>"
                  << "</dia:attribute></dia:object>";
    }

    log << MSG::DEBUG << "        IOV = " << (*i)->since << " - " << (*i)->until << endmsg;
    if ((*i)->memFuncs.size()){
      log << MSG::DEBUG << "       depend on :" << endmsg;
      for (Item::MembFuncList::iterator mfIt = (*i)->memFuncs.begin(); mfIt != (*i)->memFuncs.end(); ++mfIt){
        log << MSG::DEBUG << std::hex << "                  ";
        for (Item::ItemList::iterator itemIt = mfIt->items->begin(); itemIt != mfIt->items->end(); ++itemIt){
          log << " " << *itemIt;
          if (dia_file.get() != NULL) {
            // Add an arrow to the diagram connecting the user Item to the
            // used Item
            (*dia_file)
              << "<dia:object type=\"Standard - Line\" version=\"0\" id=\"l" << std::dec << dia_lines_ctr++ << "\">"
              << "<dia:attribute name=\"end_arrow\"><dia:enum val=\"22\"/>"
              << "</dia:attribute>"
              << "<dia:connections>"
              << "<dia:connection handle=\"0\" to=\"i" << std::hex << *i << "\" connection=\"13\"/>"
              << "<dia:connection handle=\"1\" to=\"i" << std::hex << *itemIt << "\" connection=\"2\"/>"
              << "</dia:connections></dia:object>";
          }
        }
        log << std::dec << endmsg;
      }
    }
  }

  if (dia_file.get() != NULL) {
    // DIA header
    (*dia_file) << "</dia:layer></dia:diagram>\n";
    log << MSG::ALWAYS << "DIA file '" << m_diaDumpFile << "' written" << endmsg;
  }

  log << MSG::DEBUG << "Found " << head_cnt << " head items: ";
  if (m_head_items.size() == head_cnt){
    log << "OK";
  } else {
    log << "MISMATCH!!!!!";
  }
  log << endmsg;
}


void UpdateManagerSvc::purge() {
  if ( FSMState() < Gaudi::StateMachine::INITIALIZED ){
    throw GaudiException("Service offline","UpdateManagerSvc::purge",StatusCode::FAILURE);
  }

  MsgStream log(msgSvc(),name());

  log << MSG::INFO << "Purging dependencies network" << endmsg;

  // first I make a copy of the list of objects
  //Item::ItemList items_copy(m_all_items);
  //Item::ItemList items_copy(m_head_items);
  // Start from a clean IOV (I cannot use m_head_X because the head is not stable and they may change)
  //Gaudi::Time head_copy_since(Gaudi::Time::epoch());
  //Gaudi::Time head_copy_until(Gaudi::Time::max());

  Item::ItemList::iterator it = m_all_items.begin();
  for (it = m_all_items.begin(); it != m_all_items.end() ; ++it){
    (*it)->purge(&log);

    if ( ! (*it)->path.empty() ) {
      Item::ItemList &children = (*it)->children;
      // remove connections to children if the object is going to be reloaded
      Item::ItemList::iterator c = children.begin();
      while ( children.end() != c ) {
        unlink(*it,*c);
        c = children.begin();
      }
    }
  }

  m_head_since = 1;
  m_head_until = 0;

}

//=========================================================================
//  search the item with the given path and get its validity
//=========================================================================
bool UpdateManagerSvc::getValidity(const std::string path, Gaudi::Time& since, Gaudi::Time &until,
                                   bool path_to_db) {
  if ( FSMState() < Gaudi::StateMachine::INITIALIZED ){
    throw GaudiException("Service offline","UpdateManagerSvc::registerCondition",StatusCode::FAILURE);
  }

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
  if ( FSMState() < Gaudi::StateMachine::INITIALIZED ){
    throw GaudiException("Service offline","UpdateManagerSvc::registerCondition",StatusCode::FAILURE);
  }

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
