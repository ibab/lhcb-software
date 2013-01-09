// $Id: EventDataSvc.h,v 1.6 2005/01/19 08:23:16 mato Exp $

// Framework include files
#include "EventDataSvc.h"

#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/DataIncident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/PointerHelpers.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IDataStoreAgent.h"

using namespace Gaudi;
using namespace std;

static const string s_empty;

DECLARE_NAMESPACE_SERVICE_FACTORY(Gaudi,EventDataSvc)

#if 0
#define _TES_TRACE(svc,tag)  _calcHash32(tag)

static unsigned int hash32(const char* key) {
    unsigned int hash;
    const char* k;
    for (hash = 0, k = key; *k; k++) {
      hash += *k; hash += (hash << 10); hash ^= (hash >> 6);
    }
    hash += (hash << 3); hash ^= (hash >> 11); hash += (hash << 15);
    return hash;
  }

static void _calcHash32(const EventTag_t& tag) {
  const char* n = tag.name();
  unsigned int hash_code = n ? hash32(n) : 0;
  if ( hash_code == 0 ) {
    throw "Bad hash_code!";
  }
}
#else
#define _TES_TRACE(svc,tag) 
#endif

/// Defautl destructor
EventDataSvc::Entry::~Entry() {
  releasePtr(m_object);
  releasePtr(m_address);
  m_tag = 0;
}

/// Name of the directory (or key)
const string& EventDataSvc::Entry::name()   const {
  if ( m_name.empty() ) {
    const char* nam = EventTag_t::entry_name(m_tag);
    if ( nam ) m_name = nam;
  }
  return m_name;
}

/// Full identifier (or key)
const string& EventDataSvc::Entry::identifier()   const {
  return name();
}

/// Set/Update Opaque storage address
void EventDataSvc::Entry::setAddress (IOpaqueAddress* pAddress) {
  swapRefCounted(pAddress, m_address);
}

/// Set/Update Object reference
void EventDataSvc::Entry::setObject (DataObject* pObject) {
  swapRefCounted(pObject, m_object);
}

void EventDataSvc::Entry::clear() {
  releasePtr(m_object);
  releasePtr(m_address);
  m_name = "";
  m_tag = 0;
}

void EventDataSvc::Entry::set(const EventTag_t& tag, DataObject* pObject, IOpaqueAddress* pAddress) {
  swapRefCounted(pAddress, m_address);
  swapRefCounted(pObject, m_object);
  m_tag = tag.bit();
}

/// Standard Constructor
EventDataSvc::EventDataSvc(const std::string& name, ISvcLocator* svc)  
: base_class(name,svc), m_root(0)
{
  m_dataLoader       = 0;
  m_incidentSvc      = 0;
  m_enableFaultHdlr  = false;
  m_enableAccessHdlr = false;
  m_faultName        = "DataFault";
  m_accessName       = "DataAccess";

  declareProperty("RootCLID",            m_rootCLID = 110);
  declareProperty("RootName",            m_rootName = "/Event");
  declareProperty("MaskWidth",           m_maskWidth = 128);
  declareProperty("DataFaultName",       m_faultName);
  declareProperty("DataAccessName",      m_accessName);
  declareProperty("EnableFaultHandler",  m_enableFaultHdlr);
  declareProperty("EnableAccessHandler", m_enableAccessHdlr);
  m_bits.extend(m_maskWidth);
}

/// Standard Destructor
EventDataSvc::~EventDataSvc() {
  setDataLoader(0).ignore();
  clearStore().ignore();
}

/// Service initialization
StatusCode EventDataSvc::initialize() {
  StatusCode sc = Service::initialize();
  if ( sc.isSuccess() )  {
    sc = service("IncidentSvc", m_incidentSvc, true);
    if ( !sc.isSuccess() )  {
      MsgStream log(msgSvc(), name());
      log << MSG::ERROR << "Failed to access incident service." << endmsg;
    }
    m_root = EventTag_t(m_rootName.c_str());
    m_bits.extend(m_maskWidth);
    m_registry.resize(m_bits.second,Entry(this));
  }
  return sc;
}

/// Service initialization
StatusCode EventDataSvc::finalize() {
  setDataLoader(0).ignore();
  clearStore().ignore();
  releasePtr(m_incidentSvc);
  // Now call base class finalisation
  return Service::finalize();
}

/// Retrieve customizable data loader according to registry entry
IConversionSvc* EventDataSvc::getDataLoader(IRegistry* /* pReg */)   {
  return m_dataLoader;
}

/// IDataManagerSvc: Pass a default data loader to the service.
StatusCode EventDataSvc::setDataLoader(IConversionSvc* pDataLoader)    {
  if ( 0 != pDataLoader  ) pDataLoader->addRef();
  if ( 0 != m_dataLoader ) m_dataLoader->release();
  m_dataLoader = pDataLoader;
  if ( 0 != m_dataLoader )    {
    m_dataLoader->setDataProvider((IDataProviderSvc*)this).ignore();
  }
  return SUCCESS;
}

/// IDataManagerSvc: Remove all data objects in the data store.
StatusCode EventDataSvc::clearStore()   {
  for(vector<Entry>::iterator i=m_registry.begin(); i != m_registry.end(); ++i)
    (*i).clear();
  m_bits.zero();
  return SUCCESS;
}

/** Invoke Persistency service to create transient object from its
 *  persistent representation
 */
int EventDataSvc::loadObject(Entry& entry)   {
  StatusCode status = INVALID_OBJ_ADDR;
  IConversionSvc* pLoader  = getDataLoader(&entry);
  IOpaqueAddress* pAddress = entry._address();
  DataObject*     pObject  = 0;
  if ( 0 == pLoader )   {         // Precondition: Data loader must be present
    if (handleDataFault(entry) != 0) return  1;
    else                             return  0;
  }
  if ( 0 == pAddress )    {      // Precondition:  Address must be valid
    if (handleDataFault(entry) != 0) return  1;
    else                             return  0;
  }
  const string& identifier = entry.identifier();
  if ( m_enableAccessHdlr )  {
    // Fire data access incident
    DataIncident incident(name(), m_accessName, identifier);
    m_incidentSvc->fireIncident(incident);
  }
  try   {
    status = pLoader->createObj(pAddress,pObject);  // Call data loader
    if ( status.isSuccess() )    {
      entry.setObject(pObject);
      status = pLoader->fillObjRefs(pAddress, pObject);
    }
  }
  catch( const GaudiException& exc ) {
    if ( handleDataFault(entry) != 0 ) return 1;
    throw GaudiException("GaudiException in loadObject("+identifier+")",name(),FAILURE,exc);
  }
  catch( const std::exception& x) {
    if ( handleDataFault(entry) != 0 ) return 1;
    throw GaudiException("std::exception in loadObject("+identifier+"): "+
			 System::typeinfoName(typeid(x))+", "+x.what(),name(),FAILURE);
  }
  catch(...) {
    if ( handleDataFault(entry) != 0 ) return 1;
    throw GaudiException("UNKNOWN exception in loadObject() "+identifier,name(),FAILURE);
  }
  if ( !status.isSuccess() )  {
    if ( handleDataFault(entry) != 0 ) return 1;
  }
  return status;
}

/// Invoke data fault handling if enabled
DataObject* EventDataSvc::handleDataFault(Entry& entry)   {
  if ( m_enableFaultHdlr )  {
    DataIncident incident(name(), m_faultName, entry.identifier());
    m_incidentSvc->fireIncident(incident);
    return entry._object();
  }
  return 0;
}

/// Register object with the data store.
StatusCode EventDataSvc::put(const EventTag_t& tag, DataObject* pObject) {
  size_t bit = tag.bit();
  if ( bit < m_bits.second ) {
    m_registry[bit].set(tag,pObject,0);
    m_bits.set_bit(bit);
    _TES_TRACE(this,tag);
    return SUCCESS;
  }
  return INVALID_OBJ_PATH;
}

/// Register object address with the data store.
StatusCode EventDataSvc::put(const EventTag_t& tag, IOpaqueAddress* pAddress) {
  size_t bit = tag.bit();
  if ( bit < m_bits.second ) {
    m_registry[bit].set(tag,0,pAddress);
    m_bits.set_bit(bit);
    _TES_TRACE(this,tag);
    return SUCCESS;
  }
  return INVALID_OBJ_PATH;
}

/// Unregister object from the data store.
StatusCode EventDataSvc::unregister(const EventTag_t& tag) {
  size_t bit = tag.bit();
  if ( bit < m_bits.second ) {
    m_registry[bit].clear();
    m_bits.clear_bit(bit);
    _TES_TRACE(this,tag);
    return SUCCESS;
  }
  return INVALID_OBJ_PATH;
}

/// Add a link to another object.
StatusCode EventDataSvc::link(const EventTag_t& from, const EventTag_t& to) {
  size_t from_bit = from.bit(), to_bit = to.bit();
  if ( from_bit < m_bits.second && to_bit < m_bits.second ) {
    Entry& reg = m_registry[from_bit];
    m_registry[to_bit].set(to, reg._object(), reg._address());
    m_bits.set_bit(to_bit);
    return StatusCode::FAILURE;
  }
  return INVALID_OBJ_PATH;
}

/// Remove a link to another object.
StatusCode EventDataSvc::unlink(const EventTag_t& tag) {
  size_t bit = tag.bit();
  if ( bit < m_bits.second ) {
    m_registry[bit].clear();
    m_bits.clear_bit(bit);
    return SUCCESS;
  }
  return INVALID_OBJ_PATH;
}

/// Retrieve object from data store.
StatusCode EventDataSvc::retrieve(const EventTag_t& tag, DataObject*& pObject) {
  size_t bit = tag.bit();
  if ( bit < m_bits.second ) {
    if ( m_bits.test_bit(bit) )  {
      Entry& reg = m_registry[bit];
      pObject = reg._object();
      if ( pObject )   {
	_TES_TRACE(this,tag);
	return SUCCESS;
      }
      if ( loadObject(reg) ) {
	pObject = reg._object();
	_TES_TRACE(this,tag);
	return pObject ? SUCCESS : INVALID_OBJECT;
      }
      _TES_TRACE(this,tag);
      return OBJ_NOT_LOADED;
    }
    _TES_TRACE(this,tag);
    return INVALID_OBJ_ADDR;
  }
  return INVALID_OBJ_PATH;
}

void* EventDataSvc::get(const EventTag_t& tag, Cast_t cast) {
  DataObject* pObj = 0;
  StatusCode sc = retrieve(tag,pObj);
  if ( sc.isSuccess() ) {
    void* ptr = cast(pObj);
    if ( ptr ) {
      return ptr;
    }
  }
  throw std::runtime_error("Failed to access object.");
  return 0;
}

void* EventDataSvc::retrieve(const EventTag_t& tag, Cast_t cast) {
  DataObject* pObj = 0;
  return retrieve(tag,pObj).isSuccess() ? cast(pObj) : 0;
}

/// Shortcut to handle unsupported calls: Throws exception
StatusCode EventDataSvc::unsupportedCall(const string& call) const {
  throw std::runtime_error("The EventDataSvc call:"+call+" is not supported!");
}

/** 
 *  Supported calls from the IDataManagerSvc interface
 */
/// Analyse by traversing all data objects in the data store.
StatusCode EventDataSvc::traverseTree(IDataStoreAgent* pAgent)   {
  int level = 1;
  for(vector<Entry>::iterator i=m_registry.begin(); i != m_registry.end(); ++i) {
    Entry& e = *i;
    if ( e._object() || e._address() ) {
      pAgent->analyse(&e, level);
    }
  }
  return SUCCESS;
}

/// Initialize data store for new event by giving new event path.
StatusCode EventDataSvc::setRoot(const string& root_name,DataObject* pObject)  {
  if ( root_name == m_rootName ) {
    return put(m_root,pObject);
  }
  return INVALID_ROOT;
}

/// Initialize data store for new event by giving new event path.
StatusCode EventDataSvc::setRoot(const string& root_name,IOpaqueAddress* pRootAddr) {
  if ( root_name == m_rootName ) {
    return put(m_root,pRootAddr);
  }
  return INVALID_ROOT;
}

/// Get class ID of root Event
CLID EventDataSvc::rootCLID() const {
  return m_rootCLID;
}

/// Get Name of root Event
string EventDataSvc::rootName() const {
  return m_rootName;
}
