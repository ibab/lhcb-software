// $Id: COOLConfSvc.cpp,v 1.1 2007-12-20 15:48:52 marcocle Exp $

// Include files
#include "SealKernel/Context.h"
#include "SealKernel/ComponentLoader.h"

#include "RelationalAccess/IConnectionService.h"
#include "RelationalAccess/IConnectionServiceConfiguration.h"
#include "RelationalAccess/IReplicaSortingAlgorithm.h"
#include "RelationalAccess/IDatabaseServiceDescription.h"

#include "CoolKernel/IDatabaseSvc.h"
#include "CoolApplication/Application.h"
#include "CoolApplication/DatabaseSvcFactory.h"

#include "GaudiKernel/SvcFactory.h"

#include "COOLConfSvc.h"

namespace 
{

  /** @class ReplicaSortAlg
   *
   * Small class implementing coral::IReplicaSortingAlgorithm interface to allow dynamic sorting of
   * database replicas obtained from LFC.
   * 
   * When retrieving the list of DB replicas, LFCReplicaService obtains a list in an arbitrary order.
   * We have to provide to CORAL a class to be used to sort the list of replicas according to our
   * needs. First we want the closest DB, identified by the environment variable LHCBPRODSITE, then
   * the CERN server (LCG.CERN.ch), while the remaining one can be in any order (this implementation
   * uses the natural string ordering).
   *
   * @author Marco Clemencic
   * @date   2007-05-02
   */
  class ReplicaSortAlg: virtual public coral::IReplicaSortingAlgorithm 
  {
    typedef coral::IDatabaseServiceDescription dbDesc_t;
    typedef std::vector< const dbDesc_t * > replicaSet_t;
    
    /** @class  ReplicaSortAlg::Comparator
     *
     * Comparison function defining which replica comes before another.
     *
     * This class is used via the STL algorithm "sort" to order the list the way we need it.
     *
     * @author Marco Clemencic
     * @date   2007-05-02
     */
    class Comparator: public std::binary_function<const dbDesc_t*,const dbDesc_t*,bool>
    {

      std::string site;

    public:
      
      /// Constructor.
      /// @param theSite the local LHCb Production Site (LCG.<i>SITE</i>.<i>country</i>)
      Comparator(const std::string &theSite): site(theSite) {}

      /// Main function
      result_type operator() (first_argument_type a, second_argument_type b) const
      {
        std::string serverA =  a->serviceParameter(a->serverNameParam());
        std::string serverB =  b->serviceParameter(b->serverNameParam());
        
        if ( serverA == serverB )       return false; // equality                   => false
        if ( serverA == site )          return true;  // "SITE" < "anything"        => true
        if ( serverB == site )          return false; // "anything" < "SITE"        => false
        if ( serverA == "LCG.CERN.ch" ) return true;  // "LCG.CERN.ch" < "anything" => true
        if ( serverB == "LCG.CERN.ch" ) return false; // "anything" < "LCG.CERN.ch" => false
        return serverA < serverB;  // for any other case use, alphabetical order
      }

    };

    std::string localSite;
    MsgStream log;

  public:

    /// Constructor.
    /// @param theSite the local LHCb Production Site (LCG.<i>SITE</i>.<i>country</i>)
    ReplicaSortAlg(const std::string &theSite, IMessageSvc *msgSvc):
      localSite(theSite),
      log(msgSvc,"ReplicaSortAlg")
    {
      log << MSG::VERBOSE << "Constructor" << endmsg;
    }
    
    /// Destructor.
    virtual ~ReplicaSortAlg()
    {
      // log << MSG::VERBOSE << "ReplicaSortAlg --> destructor" <<std::endl;
    }

    /// Main function
    virtual void sort (std::vector< const coral::IDatabaseServiceDescription * > &replicaSet) 
    {
      if ( log.level() <= MSG::VERBOSE ) {
        log << MSG::VERBOSE << "Original list" << endmsg;
        replicaSet_t::iterator i;
        for ( i = replicaSet.begin(); i != replicaSet.end(); ++i ) {
          log << MSG::VERBOSE << " " << (*i)->serviceParameter((*i)->serverNameParam()) << endmsg;
        }
      }
      
      log << MSG::VERBOSE << "Sorting..." << endmsg;
      std::sort(replicaSet.begin(),replicaSet.end(),Comparator(localSite));

      if ( log.level() <= MSG::VERBOSE ) {
        log << MSG::VERBOSE << "Sorted list" << endmsg;
        replicaSet_t::iterator i;
        for ( i = replicaSet.begin(); i != replicaSet.end(); ++i ) {
          log << MSG::VERBOSE << " " << (*i)->serviceParameter((*i)->serverNameParam()) << endmsg;
        }
      }
    }
    
  };
  
}

// Factory implementation
DECLARE_SERVICE_FACTORY(COOLConfSvc)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
COOLConfSvc::COOLConfSvc(const std::string& name, ISvcLocator* svcloc):
  Service(name,svcloc),m_coolApplication(0)
{
  declareProperty("UseLFCReplicaSvc", m_useLFCReplicaSvc = false );
  declareProperty("EnableCoralConnectionCleanUp", m_coralConnCleanUp = false );
  declareProperty("CoralConnectionRetrialPeriod", m_retrialPeriod = 60,
                  "Time between two connection trials (in seconds).");
  declareProperty("CoralConnectionRetrialTimeOut", m_retrialTimeOut = 15*60,
                  "How long to keep retrying before giving up (in seconds).");
}
//=============================================================================
// Destructor
//=============================================================================
COOLConfSvc::~COOLConfSvc() {}

//=============================================================================
// Access to COOL DatabaseSvc
//=============================================================================
cool::IDatabaseSvc& COOLConfSvc::databaseSvc() {
  return coolApplication()->databaseService();
}

//=============================================================================
// Access to SEAL Context
//=============================================================================
seal::Context* COOLConfSvc::context() {
  return coolApplication()->context();
}

//=============================================================================
// queryInterface
//=============================================================================
StatusCode COOLConfSvc::queryInterface(const InterfaceID& riid,
                                       void** ppvUnknown){
  if ( IID_ICOOLConfSvc.versionMatch(riid) )   {
    *ppvUnknown = (ICOOLConfSvc*)this;
    addRef();
    return SUCCESS;
  }

  return Service::queryInterface(riid,ppvUnknown);
}

//=============================================================================
// initialize
//=============================================================================
StatusCode COOLConfSvc::initialize(){
  StatusCode sc = Service::initialize();
  if (sc.isFailure()) return sc;

  MsgStream log(msgSvc(), name() );

  log << MSG::DEBUG << "Initialize" << endmsg;

  if ( !m_coolApplication ) {

    log << MSG::DEBUG << "Initializing COOL Application" << endmsg;
    m_coolApplication = new cool::Application();

    seal::Handle<seal::ComponentLoader> loader = 
      m_coolApplication->context()->component<seal::ComponentLoader>();

    log << MSG::DEBUG << "Getting CORAL Connection Service configurator" << endmsg;

    loader->load( "CORAL/Services/ConnectionService" );
    std::vector< seal::IHandle< coral::IConnectionService > > loadedServices;
    m_coolApplication->context()->query( loadedServices );
    coral::IConnectionServiceConfiguration &connSvcConf = loadedServices.front()->configuration();

    if ( m_useLFCReplicaSvc ) {

      log << MSG::INFO << "Loading CORAL LFCReplicaService" << endmsg;
      loader->load( "CORAL/Services/LFCReplicaService" );
            
      std::string theSite = System::getEnv("LHCBPRODSITE");
      if ( theSite.empty() || theSite == "UNKNOWN" ) {
        theSite = "LCG.CERN.ch";
      }

      connSvcConf.setReplicaSortingAlgorithm(new ReplicaSortAlg(theSite,msgSvc()));
    }

    if ( ! m_coralConnCleanUp ) {

      log << MSG::DEBUG << "Disabling CORAL connection automatic clean up" << endmsg;
      connSvcConf.disablePoolAutomaticCleanUp();
      connSvcConf.setConnectionTimeOut( 0 );

    }        

    connSvcConf.setConnectionRetrialPeriod(m_retrialPeriod);
    log << MSG::INFO << "CORAL Connection Retrial Period set to "
        << connSvcConf.connectionRetrialPeriod() << "s" << endmsg;

    connSvcConf.setConnectionRetrialTimeOut(m_retrialTimeOut);
    log << MSG::INFO << "CORAL Connection Retrial Time-Out set to "
        << connSvcConf.connectionRetrialTimeOut() << "s" << endmsg;

  }

  return sc;
}

//=============================================================================
// finalize
//=============================================================================
StatusCode COOLConfSvc::finalize(){
  MsgStream log(msgSvc(), name() );
  log << MSG::DEBUG << "Finalize" << endmsg;
  if (m_coolApplication) {
    delete m_coolApplication;
    m_coolApplication = 0;
  }
  return Service::finalize();
}
