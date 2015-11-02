#ifdef __INTEL_COMPILER // Disable ICC remark from CORAL MessageStream and Boost
  #pragma warning(disable:2259)
#endif

// Include files
#include "RelationalAccess/ConnectionService.h"
#include "RelationalAccess/IConnectionServiceConfiguration.h"
#include "RelationalAccess/IReplicaSortingAlgorithm.h"
#include "RelationalAccess/IDatabaseServiceDescription.h"

#include "CoolKernel/IDatabaseSvc.h"
#include "CoolApplication/Application.h"
#include "CoolApplication/DatabaseSvcFactory.h"

#include "COOLConfSvc.h"

#ifdef WIN32
#define NOMSG
#define NOGDI
#endif

// For the case insensitive string comparison (boost::algorithm::icontains).
#include "boost/algorithm/string/predicate.hpp"
// For random numbers not affecting simulation.
#include "boost/random/linear_congruential.hpp"
//#include "boost/random/uniform_smallint.hpp"
#include "boost/date_time/posix_time/posix_time_types.hpp"

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
      typedef boost::rand48 RandomGenType;
      typedef RandomGenType::result_type WeightType;
      typedef std::map<std::string,WeightType> WeightMap;

      /// Site that have to be used before the others
      std::string site;
      /// Map used to remember the priority of the sites.
      /// the local site has weight -1, the other are randomly chosen the first
      /// time they are encountered.
      mutable WeightMap weights;
      /// Random number generator. Using Boost to avoid interactions with the
      /// random generator services.
      mutable RandomGenType gen;

      WeightType getWeight(const std::string& key) const {
        WeightMap::iterator i = weights.find(key);
        if ( weights.end() == i ) { // not found
          WeightType newWeight = 0;
          if (boost::algorithm::icontains(key,site)) {
            // it means that this is the site with highest priority
            newWeight = gen.max();
          } else {
            // all other sites are distributed randomly
            newWeight = gen();
          }
          weights[key] = newWeight;
          return newWeight;
        }
        return i->second;
      }

    public:

      /// Constructor.
      /// @param theSite the local LHCb Production Site (<i>SITE</i>.<i>country</i>)
      Comparator(std::string theSite):
        site(std::move(theSite)),
        gen(// this is the rather longish Boost way of getting the current number of
            // seconds since the beginning of the day... that I want to use as seed
            // for the local random number generator (I do not use "seconds since epoch"
            // because the boosted way of getting it is too long).
            boost::posix_time::second_clock::universal_time().time_of_day().total_seconds())
      {}

      /// Main function
      result_type operator() (first_argument_type a, second_argument_type b) const
      {
        return getWeight(a->serviceParameter(a->serverNameParam()))
               <
               getWeight(b->serviceParameter(b->serverNameParam()));
      }

    };

    std::string localSite;
    MsgStream log;

  public:

    /// Constructor.
    /// @param theSite the local LHCb Production Site (LCG.<i>SITE</i>.<i>country</i>)
    ReplicaSortAlg(std::string theSite, IMessageSvc *msgSvc):
      localSite(std::move(theSite)),
      log(msgSvc,"ReplicaSortAlg")
    {
      if( UNLIKELY( log.level() <= MSG::VERBOSE ) )
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
      if( UNLIKELY( log.level() <= MSG::VERBOSE ) ) {
        log << MSG::VERBOSE << "Original list" << endmsg;
        replicaSet_t::iterator i;
        for ( i = replicaSet.begin(); i != replicaSet.end(); ++i ) {
          log << MSG::VERBOSE << " " << (*i)->serviceParameter((*i)->serverNameParam()) << endmsg;
        }

        log << MSG::VERBOSE << "Sorting..." << endmsg;
      }
      std::sort(replicaSet.begin(),replicaSet.end(),Comparator(localSite));

      if( UNLIKELY( log.level() <= MSG::VERBOSE ) ) {
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
  base_class(name,svcloc)
{
  declareProperty("UseLFCReplicaSvc", m_useLFCReplicaSvc = false );
  declareProperty("LocalSite", m_localSite = "",
                  "The name of the site we are running on, used to order the "
                  "list of replicas in LFC.");
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
coral::IConnectionService& COOLConfSvc::connectionSvc() {
  return coolApplication()->connectionSvc();
}

//=============================================================================
// initialize
//=============================================================================
StatusCode COOLConfSvc::initialize(){
  StatusCode sc = base_class::initialize();
  if (sc.isFailure()) return sc;

  MsgStream log(msgSvc(), name() );

  if( UNLIKELY( log.level() <= MSG::DEBUG ) )
    log << MSG::DEBUG << "Initialize" << endmsg;

  if ( ! m_coolApplication.get() ) {

    if( UNLIKELY( log.level() <= MSG::DEBUG ) )
      log << MSG::DEBUG << "Initializing COOL Application" << endmsg;
    m_coolApplication.reset(new cool::Application);

    if( UNLIKELY( log.level() <= MSG::DEBUG ) )
      log << MSG::DEBUG << "Getting CORAL Connection Service configurator" << endmsg;
    coral::IConnectionServiceConfiguration &connSvcConf =
      m_coolApplication->connectionSvc().configuration();

    if ( m_useLFCReplicaSvc ) {

      log << MSG::INFO << "Using CORAL LFCReplicaService" << endmsg;
      connSvcConf.setLookupService( "CORAL/Services/LFCReplicaService" );
      connSvcConf.setAuthenticationService( "CORAL/Services/LFCReplicaService" );

      if ( m_localSite.empty() ) {
        // if we didn't get a site from options, we try the environment var DIRACSITE
        m_localSite = System::getEnv("DIRACSITE");
        if ( m_localSite.empty() || m_localSite == "UNKNOWN" ) {
          // if DIRACSITE is not defined, we try, for backward compatibility, LHCBPRODSITE
          m_localSite = System::getEnv("LHCBPRODSITE");
          if ( m_localSite.empty() || m_localSite == "UNKNOWN" ) {
            // if none of the env. vars is set, let's stick to a "sensible" default
            m_localSite = "CERN.ch";
          }
        }
      }
      log << MSG::INFO << "Using '" << m_localSite << "' as preferred site" << endmsg;

      m_replicaSortAlg.reset(new ReplicaSortAlg(m_localSite,msgSvc()));
      connSvcConf.setReplicaSortingAlgorithm(*m_replicaSortAlg);
    }

    if ( ! m_coralConnCleanUp ) {

      if( UNLIKELY( log.level() <= MSG::DEBUG ) )
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
  if( UNLIKELY( log.level() <= MSG::DEBUG ) )
    log << MSG::DEBUG << "Finalize" << endmsg;
  m_coolApplication.reset();
  m_replicaSortAlg.reset();
  return base_class::finalize();
}
