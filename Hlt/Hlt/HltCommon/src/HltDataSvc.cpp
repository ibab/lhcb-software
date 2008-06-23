#include "HltDataSvc.h"
#include "Event/HltSummary.h"
#include "GaudiKernel/SvcFactory.h"
// #include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IDataManagerSvc.h" 
#include "GaudiKernel/IAlgorithm.h" 
#include "GaudiKernel/SmartDataPtr.h" 
#include "boost/lambda/lambda.hpp"
#include "boost/lambda/construct.hpp"

// Declaration of the Service Factory
// DECLARE_SERVICE_FACTORY( DataSvc );


///////////TODO:
/////////// if input selection is from TES, copy into 'private' container
/////////// when to trigger? Needs support from algorithm... -- return callback
/////////// on register to trigger update, require algo to use callback...
/////////// i.e. this needs a handshake between algorithm and service....
/// usecase: see HltCommon/src/HltTFilter, the 'prepare' variant...

// Declaration of the Service Factory
DECLARE_SERVICE_FACTORY( HltDataSvc );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltDataSvc::HltDataSvc( const std::string& name,
                        ISvcLocator* pSvcLocator)
  : Service ( name , pSvcLocator )
  , m_evtSvc(0)
  , m_hltConf(0)
{
    declareProperty("OutputPrefix" ,m_TESOutputPrefix = "Hlt/Selection");
}

//=============================================================================
// Destructor
//=============================================================================
HltDataSvc::~HltDataSvc()
{
 std::for_each( m_ownedSelections.begin(),
                m_ownedSelections.end(),
                boost::lambda::delete_ptr() );
 m_ownedSelections.clear();
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
  info() << "Initialize" << endmsg;
  StatusCode status = Service::initialize();
  if ( !status.isSuccess() )   return status;
  // create the Hlt Data Svc
  m_hltConf.reset( new Hlt::Configuration() );
  
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
  m_hltConf.reset( (Hlt::Configuration*)0 );
  if (m_evtSvc) { m_evtSvc->release(); m_evtSvc=0; }
  return status;
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
HltDataSvc::addSelection(Hlt::Selection* sel,IAlgorithm* parent,bool useTES) {
//@TODO: record dependency of id on parent
//    if (parent==0) std::cout << "don't have parent..." << std::endl;
//    else std::cout << "HltDataSvc("<<name()<<"):addSelection called by " << parent->name() << " for " << sel->id() << std::endl;
    typedef std::map<stringKey,Hlt::Selection*>::iterator iter_t;
    std::pair<iter_t,iter_t> p = m_mapselections.equal_range(sel->id());
    if (p.first!=p.second) return StatusCode::FAILURE; // already there...
    m_mapselections.insert(p.first,std::make_pair(sel->id(),sel));
    if (useTES) {
        StatusCode sc = evtSvc().registerObject(m_TESOutputPrefix + "/" + sel->id().str(),sel);
        if (sc.isFailure()) return sc;
    } else {
        m_ownedSelections.push_back(sel);
    }
    return StatusCode::SUCCESS;
}

bool 
HltDataSvc::hasSelection(const stringKey& id) const {   
    return (m_mapselections.find(id) != m_mapselections.end());
}
    

Hlt::Selection& 
HltDataSvc::selection(const stringKey& id,IAlgorithm* parent) {
//@TODO: record dependency of parent on id
    //if (parent==0) std::cout << "don't have parent..." << std::endl;
    // else std::cout << "HltDataSvc("<<name()<<"):selection called by " << parent->name() << " for " << id << std::endl;
    // don't use hasSelection here to avoid doing 'find' twice...
    std::map<stringKey,Hlt::Selection*>::const_iterator i = m_mapselections.find(id);
    if (i == m_mapselections.end()) throw GaudiException( name()+"::selection() not present ",id.str(),StatusCode::FAILURE);
    return *(i->second);
}

void 
HltDataSvc::clean() {
    // shouldn't we re-register TES based selections in the TES???
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
    if ( m_hltConf.get() == 0) { 
        throw GaudiException( name()+"::config() no Hlt::Configuration","",StatusCode::FAILURE);
    }
    return *m_hltConf;
};

void HltDataSvc::handle( const Incident& ) {
    // what to do with selections in TES???
    clean();
};

MsgStream& HltDataSvc::msg(MSG::Level level) const {
    if (m_msg.get()==0) m_msg.reset( new MsgStream( msgSvc(), name() ));
    return *m_msg << level;
}
