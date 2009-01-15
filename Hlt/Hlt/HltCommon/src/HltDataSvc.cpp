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

///////////TODO; verify that the output for a given algorithm has all inputs for
///////////      that algorithm registered in the selection (and nothing more/less)
///////////TODO: don't allow changes after first beginEvent...

// Declaration of the Service Factory
DECLARE_SERVICE_FACTORY( HltDataSvc );

namespace  {
    template <typename T>
    class _cmpFirst {
        public:
            _cmpFirst(const T& t) : m_t(t) {}
            template < typename X> bool operator()(const X& x) const  { return x.first == m_t; }
        private:
            T m_t;
    };
    
    template <typename T> _cmpFirst<T> cmpFirst(const T& t) {
        return _cmpFirst<T>(t);
    };

};

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltDataSvc::HltDataSvc( const std::string& name,
                        ISvcLocator* pSvcLocator)
  : Service ( name , pSvcLocator )
  , m_evtSvc(0)
  , m_annSvc(0)
  //
  , m_mapselections   () 
  , m_ownedSelections () 
  , m_parents ()
  , m_beginEventCalled(false)
{
   declareProperty("Producer",m_producers,"Map selection name to the name of the Algorithm which produced it");
   declareProperty("Dependencies",m_dependencies,"Map selection name to the names of the selections it depends on");
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
  if ( IHltDataSvc::interfaceID().versionMatch(IID) )   
  {
    *iface = static_cast<IHltDataSvc*> ( this ) ;
    addRef();
    return StatusCode::SUCCESS;
  }
  return Service::queryInterface(IID,iface);
}



StatusCode HltDataSvc::initialize() {
  info() << "Initialize" << endmsg;
  StatusCode status = Service::initialize();
  if ( !status.isSuccess() )   return status;
  
  // get incident Svc...
  IIncidentSvc*                incidentSvc(0);     ///<
  if (!service( "IncidentSvc", incidentSvc).isSuccess()) return StatusCode::FAILURE;
  // add listener to be triggered by BeginEvent
  bool rethrow = false; bool oneShot = false; long priority = 0;
  incidentSvc->addListener(this,IncidentType::BeginEvent,priority,rethrow,oneShot);
  incidentSvc->addListener(this,IncidentType::BeginRun,priority,rethrow,oneShot);
  incidentSvc->release();

  return status;
}

StatusCode HltDataSvc::finalize() {
  StatusCode status = Service::finalize();
  if (m_evtSvc) { m_evtSvc->release(); m_evtSvc=0; }
  if (m_annSvc) { m_annSvc->release(); m_annSvc=0; }
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

IANNSvc& HltDataSvc::annSvc() const {
  if (m_annSvc == 0) {
    StatusCode sc = serviceLocator()->service("HltANNSvc", m_annSvc);
    if (sc.isFailure() || m_annSvc == 0) { 
        throw GaudiException( name()+"::annSvc() no service found","",StatusCode::FAILURE);
    }
  }
  return *m_annSvc;
}

StatusCode 
HltDataSvc::addSelection ( Hlt::Selection*   sel               ,
                           const IAlgorithm* parent            ,
                           const bool        originatesFromTES ) 
{
  //@TODO: record dependency of id on parent
  //@TODO: verify that is a valid name by going to the ANNSvc...
  if (parent==0) {
    error() << " did not specify parent... " << endmsg;
    return StatusCode::FAILURE;
  }
  if (sel->id().empty()) {
    error() << " attempt by " << parent->name() << " to register an unnamed selection... " << endmsg;
    return StatusCode::FAILURE;
  }
  typedef std::map<stringKey,Hlt::Selection*>::iterator iter_t;
  std::pair<iter_t,iter_t> p = m_mapselections.equal_range(sel->id());
  if (p.first!=p.second) {
    error() << " attempt by " << parent->name() << " to register selection " << sel->id() << " which has already been registered" << endmsg;
    return StatusCode::FAILURE; // already there...
  }
  
  if (!originatesFromTES) { 
    //FIXME: what about the fact that we know also register Hlt2 selections???
    //if (!annSvc().value("Hlt1SelectionID",sel->id().str())) {
      //error() << "attempt by " << parent->name() << " to register a selection, " << sel->id() 
      //        << "unknown to HltANNSvc" << endmsg;
      //return StatusCode::FAILURE;
    //}
    if (std::find_if(m_parents.begin(),m_parents.end(), cmpFirst(parent))!=m_parents.end()) {
      error() << " parent already registerd an output selection... " << endmsg;
      return StatusCode::FAILURE;
    }
    m_parents.push_back(std::make_pair(parent,sel) ); // register that a parent generated an output selection...
    debug() << "adding output selection " << sel->id() << " for " << parent->name() << endmsg;
  } else {
    debug() << "adding TES input selection " << sel->id() << " requested by " << parent->name() << endmsg;
  }
  m_ownedSelections.push_back(sel);
  m_mapselections.insert(p.first,std::make_pair(sel->id(),sel));
  return StatusCode::SUCCESS;
}

bool 
HltDataSvc::hasSelection(const stringKey& id) const {   
  return (m_mapselections.find(id) != m_mapselections.end());
}


Hlt::Selection*
HltDataSvc::selection ( const stringKey&  id     , 
                        const IAlgorithm* parent ) 
{
  //@TODO: record dependency of parent on id
  if (parent==0) {
    throw GaudiException(" did not specify parent... ", id.str(),StatusCode::FAILURE);
  }
  if (std::find_if(m_parents.begin(),m_parents.end(), cmpFirst(parent))!=m_parents.end()) {
    throw GaudiException( " parent requests additional input after declaring output!",parent->name()+":"+id.str(),StatusCode::FAILURE);
  }
  std::map<stringKey,Hlt::Selection*>::const_iterator i = m_mapselections.find(id);
  return i != m_mapselections.end() ? i->second : (Hlt::Selection*) 0 ;
}

std::vector<stringKey> 
HltDataSvc::selectionKeys() const
{
  std::vector<stringKey> keys; keys.reserve(m_mapselections.size());
  for (std::map<stringKey,Hlt::Selection*>::const_iterator i = m_mapselections.begin();
       i!=m_mapselections.end();++i)  keys.push_back(i->first);
  return keys;
}

StatusCode 
HltDataSvc::inputUsedBy(const stringKey& key, std::vector<std::string>& inserter) const {
  std::map<stringKey,Hlt::Selection*>::const_iterator sel = m_mapselections.find(key);
  if (sel == m_mapselections.end()) return StatusCode::FAILURE;
  std::vector<stringKey>::const_iterator i=sel->second->inputSelectionsIDs().begin();
  while (i!=sel->second->inputSelectionsIDs().end()) inserter.push_back( *i++ );
  return StatusCode::SUCCESS;
}


void HltDataSvc::handle( const Incident& i) {
  // info() << " got incident "  << i.type() << " " << i.source() << endmsg;
  if (i.type()==IncidentType::BeginRun) {
        // TODO: record dependency graph in property to allow querries offline
        // FIXME: this incident is never generated ;-(
  } else if (i.type()==IncidentType::BeginEvent) {
      if (!m_beginEventCalled) {
        debug() << " first event seen, locking and storing dependency graph " << endmsg;
        for ( std::vector<std::pair<const IAlgorithm*,const Hlt::Selection*> >::const_iterator i  = m_parents.begin();
              i != m_parents.end(); ++i) {
          std::string selName = i->second->id().str();
          m_producers[ selName ] = i->first->name();
          debug() << " producer[ " << selName << " ] = " << i->first->name()  << endmsg;
          
          std::vector<stringKey>::const_iterator d= i->second->inputSelectionsIDs().begin();
          std::vector<std::string>& deps = m_dependencies[ selName ];
          while (d!=i->second->inputSelectionsIDs().end()) deps.push_back( *d++ );

          debug() << " dependencies[ " << selName << " ] = [" ;
          for ( std::vector<std::string>::const_iterator ii  = m_dependencies[ selName ].begin();
                                                         ii != m_dependencies[ selName ].end(); ++ii ) {
                debug() << " " << *ii ;
          }
          debug() << " ] " << endmsg;
        }

        m_beginEventCalled = true;
      }
      for ( std::map<stringKey,Hlt::Selection*>::iterator i  = m_mapselections.begin();
            i != m_mapselections.end(); ++i) {
        i->second->clean(); // invalidates all selections, resets decision to 'no', i.e. reject
      }
  } else { 
      //OOPS ...
  }

};

MsgStream& HltDataSvc::msg(MSG::Level level) const {
  if (m_msg.get()==0) m_msg.reset( new MsgStream( msgSvc(), name() ));
  return *m_msg << level;
}
