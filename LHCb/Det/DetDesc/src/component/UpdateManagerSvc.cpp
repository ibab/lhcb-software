// $Id: UpdateManagerSvc.cpp,v 1.4 2005-06-23 15:14:14 marcocle Exp $
// Include files 
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include <GaudiKernel/IDetDataSvc.h>
#include <GaudiKernel/IDataProviderSvc.h>

#include <GaudiKernel/IRegistry.h>

#include <DetDesc/ValidDataObject.h>
// local
#include "UpdateManagerSvc.h"

// Factory implementation
static SvcFactory<UpdateManagerSvc> s_factory;
const ISvcFactory &UpdateManagerSvcFactory = s_factory;

//-----------------------------------------------------------------------------
// Implementation file for class : UpdateManagerSvc
//
// 2005-03-30 : Marco CLEMENCIC
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
UpdateManagerSvc::UpdateManagerSvc(const std::string& name, ISvcLocator* svcloc):
  Service(name,svcloc),m_dataProvider(NULL),m_detDataSvc(NULL),m_head_since(1),m_head_until(-1)
{
  declareProperty("DataProviderSvc", m_dataProviderName = "DetectorDataSvc");
  declareProperty("DetDataSvc",      m_detDataSvcName);
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
  if ( IID_IUpdateManagerSvc.versionMatch(riid) )   {
    *ppvUnknown = (IUpdateManagerSvc*)this;
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
	// TODO: initialization to be implemented
	MsgStream log(msgSvc(),name());
	log << MSG::DEBUG << "--- initialize ---" << endmsg;

  // find the data provider
  sc = serviceLocator()->service<IDataProviderSvc>(m_dataProviderName,m_dataProvider,true);
	if (!sc.isSuccess()) {
		log << MSG::ERROR << "Unable to get a handle to the data provider" << endmsg;
		return sc;
	} else {
		log << MSG::DEBUG << "Got pointer to IDataProviderSvc \"" << m_dataProviderName << '"' << endmsg;
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
	return StatusCode::SUCCESS;
}

StatusCode UpdateManagerSvc::finalize(){
	// local finalization
	// TODO: finalization to be implemented
	
	MsgStream log(msgSvc(),name());
	log << MSG::DEBUG << "--- finalize ---" << endmsg;

  if (msgSvc()->outputLevel() <= 1) dump();

	// release the interfaces used
	if (m_dataProvider != NULL) m_dataProvider->release();
	if (m_detDataSvc != NULL) m_detDataSvc->release();
	
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
StatusCode UpdateManagerSvc::registerCondition(const std::string &condition, BaseObjectMemberFunction *mf){
	MsgStream log(msgSvc(),name());
	log << MSG::DEBUG << "registering condition \"" << condition
	    << "\" for object of type " << mf->type().name() << endmsg;
  // find the object
  Item *mf_item = findItem(mf);
  if (!mf_item){ // a new OMF
    mf_item = new Item(mf);
    m_all_items.push_back(mf_item);
    m_head_items.push_back(mf_item); // since it is new, it has no parents
  }
  // find the condition
  Item *cond_item = findItem(condition);
  if (!cond_item){ // a new condition
    cond_item = new Item(condition);
    m_all_items.push_back(cond_item);
  } else {
    if (cond_item->isHead()) removeFromHead(cond_item);
  }
  link(mf_item,mf,cond_item);
  // a new item means that we need an update
  m_head_since = 1;
  m_head_until = -1;
	return StatusCode::SUCCESS;
}
StatusCode UpdateManagerSvc::registerCondition(void *obj, BaseObjectMemberFunction *mf){
	MsgStream log(msgSvc(),name());
	log << MSG::DEBUG << "registering object at " << std::hex << obj << std::dec
	    << " for object of type " << mf->type().name() << endmsg;
  // find the "condition"
  Item *cond_item = findItem(obj);
  if (!cond_item){ // Error!!!
    throw std::runtime_error("tried to register for an object not in the UpdateManagerSvc");
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
  m_head_until = -1;
	return StatusCode::SUCCESS;
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
StatusCode UpdateManagerSvc::newEvent(const ITime &evtTime){
  StatusCode sc = StatusCode::SUCCESS;
  // Check head validity
  if ( evtTime >= m_head_since && evtTime < m_head_until ) return sc; // no need to update
  // Reset head IOV
  m_head_since = time_absolutepast;
  m_head_until = time_absolutefuture;
  // loop over the registered items of the head (unless a problem occurs)
  Item::ItemList::iterator it;
  for (it = m_head_items.begin(); it != m_head_items.end() && sc.isSuccess(); ++it){
    sc = (*it)->update(dataProvider(),evtTime);
    if (sc.isSuccess()) {
      if ( m_head_since < (*it)->since )  m_head_since = (*it)->since;
      if ( m_head_until > (*it)->until )  m_head_until = (*it)->until;
    }
  }
  return sc;
}
StatusCode UpdateManagerSvc::update(void *instance){
  if (detDataSvc() != NULL){
		if (detDataSvc()->validEventTime()) {
      Item *item = findItem(instance);
      if (item) {
        StatusCode sc = item->update(dataProvider(),detDataSvc()->eventTime());
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
void UpdateManagerSvc::invalidate(void *instance){
  Item *item = findItem(instance);
  if (item) {
    item->invalidate();
    m_head_since = 1;
    m_head_until = -1;
  }
}

StatusCode UpdateManagerSvc::unregister(void *instance){
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
  return StatusCode::SUCCESS;
}

void UpdateManagerSvc::dump(){
  MsgStream log(msgSvc(),name());

  log << MSG::DEBUG << "--- Dump" << endmsg;
  log << MSG::DEBUG << "    " << m_all_items.size() << " items registered" << endmsg;
  log << MSG::DEBUG << "     of which " << m_head_items.size() << " in the head" << endmsg;
  log << MSG::DEBUG << "         head IOV = " << m_head_since.absoluteTime() << " - " << m_head_until.absoluteTime() << endmsg;
  
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
    log << MSG::DEBUG << "        IOV = " << (*i)->since.absoluteTime() << " - " << (*i)->until.absoluteTime() << endmsg;
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
//  search the item with the given path and change its validity
//=========================================================================
bool UpdateManagerSvc::getValidity(const std::string path, TimePoint& since, TimePoint &until,
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
void UpdateManagerSvc::setValidity(const std::string path, const TimePoint& since, const TimePoint &until,
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
//=============================================================================
