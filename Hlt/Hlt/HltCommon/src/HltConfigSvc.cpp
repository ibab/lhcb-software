// $Id: HltConfigSvc.cpp,v 1.13 2008-07-30 13:37:32 graven Exp $
// Include files 

#include <algorithm>

#include "boost/lexical_cast.hpp"

// from Gaudi
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "Event/ODIN.h"

// local
#include "HltConfigSvc.h"

using namespace std;
using boost::lexical_cast;

//-----------------------------------------------------------------------------
// Implementation file for class : HltConfigSvc
//
// 2007-10-24 : Gerhard Raven
//-----------------------------------------------------------------------------

// Declaration of the Service Factory
DECLARE_SERVICE_FACTORY( HltConfigSvc );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltConfigSvc::HltConfigSvc( const string& name, ISvcLocator* pSvcLocator)
  : PropertyConfigSvc( name , pSvcLocator )
  , m_configuredTCK(0)
  , m_evtSvc(0)
  , m_incidentSvc(0)
{
  declareProperty("TCK2ConfigMap", m_tck2config);
  declareProperty("initialTCK", m_initialTCK = TCK_t(0));
  declareProperty("prefetchTCK", m_prefetchTCK);
}
//=============================================================================
// Destructor
//=============================================================================
HltConfigSvc::~HltConfigSvc() {
   // if not released already by now, then do it now...
} 


//=============================================================================
// Finalization
//=============================================================================
StatusCode HltConfigSvc::finalize() {
  m_evtSvc->release();      m_evtSvc=0;
  m_incidentSvc->release(); m_incidentSvc=0;
  return PropertyConfigSvc::finalize();
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltConfigSvc::initialize() {
  StatusCode status = PropertyConfigSvc::initialize();
  if ( !status.isSuccess() ) return status;

  if (!service( "EventDataSvc", m_evtSvc).isSuccess()) return StatusCode::FAILURE;


  if (!service( "IncidentSvc", m_incidentSvc).isSuccess()) return StatusCode::FAILURE;
  // add listener to be triggered by first BeginEvent with low priority
  // so it gets called first
  bool rethrow = false;
  bool oneShot = false;
  m_incidentSvc->addListener(this,IncidentType::BeginEvent,
                             std::numeric_limits<long>::min(),rethrow,oneShot);

  //TODO:
  // verify that tools do not change from one TCK to the next...
  for (vector<TCK_t>::const_iterator i = m_prefetchTCK.begin(); i!=m_prefetchTCK.end(); ++i ) {
     info() << " loading TCK " << *i << endmsg; 
     if ( !loadConfig( tck2id(*i) ) ) {
        error() << " failed to load TCK " << *i << endmsg; 
        return StatusCode::FAILURE;
     }
  }
  // configure everyone from an a-priori specified TCK
  status = configure( tck2id(m_initialTCK), false );
  if (status.isSuccess()) m_configuredTCK = m_initialTCK;
  return status;
}


//=============================================================================
// Perform mapping from TCK to onfiguration ID
//=============================================================================
ConfigTreeNode::digest_type
HltConfigSvc::tck2id(const TCK_t& tck) const {
    ConfigTreeNode::digest_type id = ConfigTreeNode::digest_type::createInvalid();
    TCKMap_t::const_iterator i = m_tck2config.find(lexical_cast<string>(tck));
    if (i != m_tck2config.end()) {
        ConfigTreeNode::digest_type id = ConfigTreeNode::digest_type::createFromStringRep(i->second);
        debug() << " TCK " << lexical_cast<string>(tck) << " mapped (by explicit option) to " << id << endmsg;
        return id;
    }

    // NOTE: we need to access to the IConfigAccessSvc of the parent to make
    // sure we are consistent...

    i = m_tck2configCache.find(lexical_cast<string>(tck));
    if ( i!=m_tck2configCache.end() )  {
        id = ConfigTreeNode::digest_type::createFromStringRep(i->second);
    } else {
        ConfigTreeNodeAlias::alias_type alias( std::string("TCK/") + lexical_cast<string>(tck) );
        boost::optional<ConfigTreeNode> n = cas()->readConfigTreeNodeAlias( alias );
        if (!n) {
            error() << "Could not resolve TCK " << lexical_cast<string>(tck) << " : no alias found " << endmsg;
            return id;
        }
        id = n->digest(); // need a digest, not an object itself...
        // add to cache...
        m_tck2configCache.insert( make_pair( lexical_cast<string>(tck), lexical_cast<string>(id) ) );
    }
    debug() << "mapping TCK" << lexical_cast<string>(tck) << " to configuration ID" << id << endmsg;
    return id;
}

//=============================================================================
// Check TCK on 'beginEvent' incident
//=============================================================================
void HltConfigSvc::dummyVerifyTCK() {
  // check if TCK still the same -- if not, reconfigure... 
  TCK_t currentTCK = m_configuredTCK;
  static unsigned nEvent(0);
  debug() << "nEvent: " << nEvent << endmsg;
  if (++nEvent%100==0) { 
      always()   << "\n ********************************************\n"
               << " *********INCREASING TCK !!!!****************\n"
               << " ********************************************\n" << endl;
      ++currentTCK;
  }
  if (m_configuredTCK != currentTCK) {
      info() << "updating configuration from TCK " << m_configuredTCK << " to TCK " << currentTCK << endl;
      StatusCode sc = reconfigure( tck2id(currentTCK) );
      if (sc.isSuccess()) m_configuredTCK = currentTCK;
  }
}

void HltConfigSvc::verifyTCK() {

    SmartDataPtr<LHCb::ODIN> odin( m_evtSvc , LHCb::ODINLocation::Default );
    if (!odin) {
        error() << " Could not locate ODIN... " << endmsg;
        m_incidentSvc->fireIncident(Incident(name(),IncidentType::AbortEvent));
        return;
    }
    unsigned int TCK = odin->triggerConfigurationKey();

    debug() << "verifyTCK: TCK in ODIN bank: " << TCK << endmsg;
    debug() << "verifyTCK: currently configured TCK: " << m_configuredTCK << endmsg;

    if ( m_configuredTCK == TCK ) return;

    info() << "requesting configuration update from TCK " 
           << m_configuredTCK 
           << " to TCK " << TCK << endmsg;
    if (reconfigure( tck2id(TCK) ).isSuccess()) { 
        m_configuredTCK = TCK;
    } else {
        error()   << "\n\n\n\n\n"
                  << "            ****************************************\n"
                  << "            ****************************************\n"
                  << "            ****************************************\n"
                  << "            ********                        ********\n"
                  << "            ********   RECONFIGURE FAILED   ********\n"
                  << "            ********                        ********\n"
                  << "            ****************************************\n"
                  << "            ****************************************\n"
                  << "            ****************************************\n"
                  << "\n\n\n\n\n"
                  << endmsg;
        m_incidentSvc->fireIncident(Incident(name(),IncidentType::AbortEvent));
        return;
    }
}

void HltConfigSvc::handle(const Incident& /*incident*/) {
     dummyVerifyTCK();
    // verifyTCK();
}
