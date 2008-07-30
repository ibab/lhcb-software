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
#include <cassert>

// Declaration of the Service Factory
// DECLARE_SERVICE_FACTORY( DataSvc );


///////////TODO:
/////////// if input selection is from TES, copy into 'private' container
/////////// when to trigger? Needs support from algorithm... -- return callback
/////////// on register to trigger update, require algo to use callback...
/////////// i.e. this needs a handshake between algorithm and service....
///////////  alternative: register ourselves as auditor (for parent algo only?)
/// usecase: see HltCommon/src/HltTFilter, the 'prepare' variant...

///////////TODO: once an algorithm has registered an output, don't allow it to
///////////      pick up another input, nor to register another output
///////////TODO; verify that the output for a given algorithm has all inputs for
///////////      that algorithm registered in the selection (and nothing more/less)
///////////TODO: don't allow changes after first beginEvent...

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
HltDataSvc::addSelection(Hlt::Selection* sel,IAlgorithm* parent,bool originatesFromTES) {
//@TODO: record dependency of id on parent
//@TODO: verify that is a valid name by going to the ANNSvc...
    if (parent==0) {
        error() << " did not specify parent... " << endmsg;
        return StatusCode::FAILURE;
    }
    if (sel->id().empty()) {
        error() << " attempt by " << parent->name() << " to register an unnamed selection... " << endmsg;
        assert(1==0);
        return StatusCode::FAILURE;
    }
    if (!originatesFromTES) { 
        if (std::find(m_parents.begin(),m_parents.end(),parent)!=m_parents.end()) {
            error() << " parent already registerd an output selection... " << endmsg;
            return StatusCode::FAILURE;
        }
        m_parents.push_back(parent); // register that a parent generated an output selection...
        debug() << "adding output selection " << sel->id() << " for " << parent->name() << endmsg;
    } else {
        debug() << "adding TES input selection " << sel->id() << " requested by " << parent->name() << endmsg;
    }
    typedef std::map<stringKey,Hlt::Selection*>::iterator iter_t;
    std::pair<iter_t,iter_t> p = m_mapselections.equal_range(sel->id());
    if (p.first!=p.second) return StatusCode::FAILURE; // already there...
    m_mapselections.insert(p.first,std::make_pair(sel->id(),sel));
    if (!originatesFromTES) {
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
    if (parent==0) {
        throw GaudiException(" did not specify parent... ", id.str(),StatusCode::FAILURE);
    }
    if (std::find(m_parents.begin(),m_parents.end(),parent)!=m_parents.end()) {
        throw GaudiException( " parent requests input after declaring output!",parent->name()+":"+id.str(),StatusCode::FAILURE);
    }
    std::map<stringKey,Hlt::Selection*>::const_iterator i = m_mapselections.find(id);
    if (i == m_mapselections.end()) throw GaudiException( name()+"::selection() not present ",id.str(),StatusCode::FAILURE);
    return *(i->second);
}

std::vector<stringKey> 
HltDataSvc::selectionKeys()
{
    std::vector<stringKey> keys; keys.reserve(m_mapselections.size());
    for (std::map<stringKey,Hlt::Selection*>::const_iterator i = m_mapselections.begin();
         i!=m_mapselections.end();++i)  keys.push_back(i->first);
    return keys;
}

Hlt::Configuration& 
HltDataSvc::config() {
    if ( m_hltConf.get() == 0) { 
        throw GaudiException( name()+"::config() no Hlt::Configuration","",StatusCode::FAILURE);
    }
    return *m_hltConf;
};

void HltDataSvc::handle( const Incident& ) {
    for ( std::map<stringKey,Hlt::Selection*>::iterator i  = m_mapselections.begin();
                                                        i != m_mapselections.end(); ++i)
        i->second->clean(); // invalidates all selections, resets decision to 'no', i.e. reject
};

MsgStream& HltDataSvc::msg(MSG::Level level) const {
    if (m_msg.get()==0) m_msg.reset( new MsgStream( msgSvc(), name() ));
    return *m_msg << level;
}
