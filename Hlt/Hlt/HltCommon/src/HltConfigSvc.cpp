// $Id: HltConfigSvc.cpp,v 1.18 2008-09-01 14:15:47 graven Exp $
// Include files 

#include <algorithm>

#include "boost/lexical_cast.hpp"
#include "boost/format.hpp"

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
  declareProperty("initialTCK", m_initialTCK = TCK_t(0)); // TODO: replace by hex string
  declareProperty("prefetchTCK", m_prefetchTCK); // TODO: load all TCK of same type as initialTCK
  declareProperty("checkOdin", m_checkOdin = true);
  declareProperty("maskL0TCK", m_maskL0TCK = false);
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
  if (m_incidentSvc!=0) { m_incidentSvc->release(); m_incidentSvc=0; }
  return PropertyConfigSvc::finalize();
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltConfigSvc::initialize() {
  StatusCode status = PropertyConfigSvc::initialize();
  if ( !status.isSuccess() ) return status;

  if (!service( "EventDataSvc", m_evtSvc).isSuccess()) return StatusCode::FAILURE;


  if (m_checkOdin) {
      if (!service( "IncidentSvc", m_incidentSvc).isSuccess()) return StatusCode::FAILURE;
      // add listener to be triggered by first BeginEvent with low priority
      // so it gets called first
      bool rethrow = false;
      bool oneShot = false;
      m_incidentSvc->addListener(this,IncidentType::BeginEvent,
                                 std::numeric_limits<long>::min(),rethrow,oneShot);
  }

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
    std::string tckRep = boost::str( boost::format("0x%08x")%tck ) ;
    TCKMap_t::const_iterator i = m_tck2config.find( tck );
    if (i != m_tck2config.end()) {
        ConfigTreeNode::digest_type id = ConfigTreeNode::digest_type::createFromStringRep(i->second);
        debug() << " TCK " << tckRep << " mapped (by explicit option) to " << id << endmsg;
        return id;
    }

    // NOTE: we need to access to the IConfigAccessSvc of the parent to make
    // sure we are consistent...

    //TODO: make dedicated type of TCK (containing an 'unsigned int') which defines
    //      amongst others its text representation, and allows strong typing in 
    //      interfaces...

    i = m_tck2configCache.find( tck );
    if ( i!=m_tck2configCache.end() )  {
        id = ConfigTreeNode::digest_type::createFromStringRep(i->second);
    } else {
        ConfigTreeNodeAlias::alias_type alias( std::string("TCK/") +  tckRep  );
        boost::optional<ConfigTreeNode> n = cas()->readConfigTreeNodeAlias( alias );
        if (!n) {
            error() << "Could not resolve TCK " <<  tckRep  << " : no alias found " << endmsg;
            return id;
        }
        id = n->digest(); // need a digest, not an object itself...
        // add to cache...
        m_tck2configCache.insert( make_pair(  tck , id.str() ) );
    }
    debug() << "mapping TCK" <<  tckRep  << " to configuration ID" << id << endmsg;
    return id;
}

//=============================================================================
// Check TCK on 'beginEvent' incident
//=============================================================================
void HltConfigSvc::dummyCheckOdin() {
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

void HltConfigSvc::checkOdin() {

    SmartDataPtr<LHCb::ODIN> odin( m_evtSvc , LHCb::ODINLocation::Default );
    if (!odin) {
        error() << " Could not locate ODIN... " << endmsg;
        m_incidentSvc->fireIncident(Incident(name(),IncidentType::AbortEvent));
        return;
    }
    unsigned int TCK = odin->triggerConfigurationKey();

    debug() << "checkOdin: TCK in ODIN bank: " << TCK << endmsg;
    debug() << "checkOdin: currently configured TCK: " << m_configuredTCK << endmsg;

    // if upper bit set, ignore L0... in that case, the L0 TCK is generated by PVSS
    // and not by us, and hence we only know it with the L0 part blanked out...
    if ( m_maskL0TCK || (TCK & 0x80000000)!=0 ) TCK &= 0xFFFF0000;
    debug() << "checkOdin: requested TCK: " << TCK << endmsg;

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
     //dummyCheckOdin();
    checkOdin();
}
