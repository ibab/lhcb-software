#include "HltDataSvc.h"
#include "Event/HltSummary.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IDataManagerSvc.h" 
#include "GaudiKernel/SmartDataPtr.h" 

// Declaration of the Service Factory
DECLARE_SERVICE_FACTORY( DataSvc );

// Declaration of the Service Factory
DECLARE_SERVICE_FACTORY( HltDataSvc );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltDataSvc::HltDataSvc( const std::string& name,
                        ISvcLocator* pSvcLocator)
  : Service ( name , pSvcLocator )
  , m_hltMan(0)
  , m_hltSvc(0)
  , m_evtSvc(0)
  , m_hltConf(0)
{
  declareProperty("HltConfigurationLocation", m_hltConfigurationLocation = 
                         LHCb::HltSummaryLocation::Default+"/Configuration");
}

//=============================================================================
// Destructor
//=============================================================================
HltDataSvc::~HltDataSvc()
{

}

//=============================================================================
// queryInterface
//=============================================================================
StatusCode HltDataSvc::queryInterface(const InterfaceID& IID,
                                      void** iface) {
  if ( IHltDataSvc::interfaceID().versionMatch(IID) )   {
    *iface = (IHltDataSvc*)this;
    addRef();
    return StatusCode::SUCCESS;
  }
  return Service::queryInterface(IID,iface);
}



StatusCode HltDataSvc::initialize() {
//TODO: don't put Hlt::Configuration into DataSvc...
  info() << "Initialize" << endmsg;
  StatusCode status = Service::initialize();
  if ( !status.isSuccess() )   return status;
  // create the Hlt Data Svc
  if (!m_hltMan) {
    std::string name = "DataSvc/HltSvc";
    debug() << " creating hltSvc " << name << endreq;
    status = serviceLocator()->service(name,m_hltMan,true);
    if ( status.isFailure() ) return status; 
  } else {
    status = m_hltMan->clearStore();
    if ( status.isFailure() ) return status; 
  }
  if (!m_hltMan) fatal() << " not able to create HltSvc " << endreq;
  info() << " created HltSvc " << endreq;
  status = m_hltMan->setRoot("/Event", new DataObject());
  if (status.isFailure()) return status;
  
  status = hltSvc().registerObject( "/Event/"+hltConfigurationLocation(), new Hlt::Configuration() );
  if (status.isFailure()) return status;
  info() << " stored Hlt::Configuration in HltSvc at " << hltConfigurationLocation() << endreq;

  // get incident Svc...
  IIncidentSvc*                incidentSvc(0);     ///<
  if (!service( "IncidentSvc", incidentSvc).isSuccess()) return StatusCode::FAILURE;
  // add listener to be triggered by BeginEvent
  bool rethrow = false; bool oneShot = false; long priority = 0;
  incidentSvc->addListener(this,IncidentType::BeginEvent,priority,rethrow,oneShot);
  incidentSvc->release();

  return status;
}

StatusCode HltDataSvc::finalize() {
  StatusCode status = Service::finalize();
  if (m_hltMan) {
    status = m_hltMan->clearStore();  // this deletes HLTData and HLTConf...
    if (status.isFailure()) return status;
    m_hltMan->release();
    m_hltMan=0;
  }
  if (m_evtSvc) { m_evtSvc->release(); m_evtSvc=0; }
  if (m_hltSvc) { m_hltSvc->release(); m_hltSvc=0; }
  return status;
}

IDataProviderSvc& HltDataSvc::hltSvc() const {
  if (m_hltSvc == 0) {
    StatusCode sc = serviceLocator()->service("HltSvc", m_hltSvc);
    if (sc.isFailure() || m_hltSvc == 0) { 
        throw GaudiException( name()+"::hltSvc() no HltDataSvc","",StatusCode::FAILURE);
    }
  }
  return *m_hltSvc;
}

IDataProviderSvc& HltDataSvc::evtSvc() const {
  if (m_evtSvc == 0) {
    StatusCode sc = serviceLocator()->service("EventDataSvc", m_evtSvc);
    if (sc.isFailure() || m_evtSvc == 0) { 
        throw GaudiException( name()+"::evtSvc() no EventDataSvc","",StatusCode::FAILURE);
    }
  }
  return *m_evtSvc;
}


StatusCode 
HltDataSvc::addSelection(Hlt::Selection* sel) {
    typedef std::map<stringKey,Hlt::Selection*>::iterator iter_t;
    std::pair<iter_t,iter_t> p = m_mapselections.equal_range(sel->id());
    if (p.first!=p.second) return StatusCode::FAILURE; // already there...
    m_mapselections.insert(p.first,std::make_pair(sel->id(),sel));
    return StatusCode::SUCCESS;
}

bool 
HltDataSvc::hasSelection(const stringKey& id) const {   
    return (m_mapselections.find(id) != m_mapselections.end());
}
    

Hlt::Selection& 
HltDataSvc::selection(const stringKey& id) {
    // don't use hasSelection here to avoid doing 'find' twice...
    std::map<stringKey,Hlt::Selection*>::const_iterator i = m_mapselections.find(id);
    if (i == m_mapselections.end()) throw GaudiException( name()+"::selection() not present ",id.str(),StatusCode::FAILURE);
    return *(i->second);
}

void 
HltDataSvc::clean() {
    for ( std::map<stringKey,Hlt::Selection*>::iterator i  = m_mapselections.begin();
                                                        i != m_mapselections.end(); ++i)
        i->second->clean();
}
    
std::vector<stringKey> 
HltDataSvc::selectionKeys()
{
    std::vector<stringKey> keys; keys.reserve(m_mapselections.size());
    for (std::map<stringKey,Hlt::Selection*>::const_iterator i = m_mapselections.begin();
         i!=m_mapselections.end();++i)  keys.push_back(i->first);
    return keys;
}

void 
HltDataSvc::resetData() {
    assert(1==0);
    // m_hltData = 0;
}

Hlt::Configuration& 
HltDataSvc::config() {
    if (m_hltConf != 0) return *m_hltConf;
    std::string loca = "/Event/"+m_hltConfigurationLocation;
    SmartDataPtr<Hlt::Configuration> obj(&hltSvc(),loca);
    m_hltConf = obj;
    if ( m_hltConf == 0) { 
        throw GaudiException( name()+"::config() no Hlt::Configuration","",StatusCode::FAILURE);
    }
    return *m_hltConf;
};

void HltDataSvc::handle( const Incident& ) {
    clean();
};

MsgStream& HltDataSvc::msg(MSG::Level level) const {
    if (m_msg.get()==0) m_msg.reset( new MsgStream( msgSvc(), name() ));
    return *m_msg << level;
}
