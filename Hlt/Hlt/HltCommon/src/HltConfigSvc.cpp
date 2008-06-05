// $Id: HltConfigSvc.cpp,v 1.8 2008-06-05 11:20:16 graven Exp $
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
{
  declareProperty("TCK2ConfigMap", m_tck2config);
  declareProperty("initialTCK", m_initialTCK = TCK_t(0));
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
  m_evtSvc->release();
  return PropertyConfigSvc::finalize();
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltConfigSvc::initialize() {
  StatusCode status = PropertyConfigSvc::initialize();
  if ( !status.isSuccess() ) return status;

  if (!service( "EventDataSvc", m_evtSvc).isSuccess()) return StatusCode::FAILURE;


  IIncidentSvc*                incidentSvc;     ///< 
  if (!service( "IncidentSvc", incidentSvc).isSuccess()) return StatusCode::FAILURE;
  // add listener to be triggered by first BeginEvent with low priority
  // so it gets called first
  bool rethrow = false;
  bool oneShot = false;
  incidentSvc->addListener(this,IncidentType::BeginEvent,
                             std::numeric_limits<long>::min(),rethrow,oneShot);
  incidentSvc->release();

  //TODO:
  // pre-fetch all configurations needed for specified set of TCKs
  // verify that tools do not change from one TCK to the next...

  // configure everyone from an a-priori specified TCK
  status = configure( m_initialTCK );
  if (status.isSuccess()) m_configuredTCK = m_initialTCK;
  return status;
}


//=============================================================================
// Configure
//=============================================================================
StatusCode 
HltConfigSvc::configure(const TCK_t& tck) const {
    TCKMap_t::const_iterator i = m_tck2config.find(lexical_cast<string>(tck));
    if (i == m_tck2config.end()) {
        error() << " could not resolve " << tck << " to a configID " << endl;
        return StatusCode::FAILURE;
    }
    debug() << "mapping TCK" << lexical_cast<string>(tck) << " to configuration ID" << i->second << endmsg;
    info() << "Invoking PropertyConfigSvc::configure" << endmsg;
    return PropertyConfigSvc::configure( ConfigTreeNode::digest_type::createFromStringRep(i->second) );
}

//=============================================================================
// Reconfigure
//=============================================================================
StatusCode 
HltConfigSvc::reconfigure(const TCK_t& tck) const {
    TCKMap_t::const_iterator i = m_tck2config.find(lexical_cast<string>(tck));
    if (i == m_tck2config.end()) {
        error() << " could not resolve " << tck << " to a configID " << endl;
        return StatusCode::FAILURE;
    }
    return PropertyConfigSvc::reconfigure( ConfigTreeNode::digest_type::createFromStringRep( i->second) );
}

//=============================================================================
// populate cache with this configuration
//=============================================================================
bool 
HltConfigSvc::loadConfig(const TCK_t& tck) {
    TCKMap_t::const_iterator i = m_tck2config.find(lexical_cast<string>(tck));
    if (i == m_tck2config.end()) {
        error() << " could not resolve " << tck << " to a configID " << endl;
        return StatusCode::FAILURE;
    }
    return PropertyConfigSvc::loadConfig( ConfigTreeNode::digest_type::createFromStringRep(i->second) );
}

//=============================================================================
// Check TCK on 'beginEvent' incident
//=============================================================================
void HltConfigSvc::dummyVerifyTCK() {
  // check if TCK still the same -- if not, reconfigure... 
  TCK_t currentTCK = m_configuredTCK;
  static unsigned nEvent(0);
  info() << "nEvent: " << nEvent << endl;
  if (++nEvent%100==0) { 
      info()   << " ********************************************\n"
               << " *********INCREASING TCK !!!!****************\n"
               << " ********************************************\n" << endl;
      ++currentTCK;
  }
  if (m_configuredTCK != currentTCK) {
      info() << "updating configuration from TCK " << m_configuredTCK << " to TCK " << currentTCK << endl;
      StatusCode sc = reconfigure( currentTCK );
      if (sc.isSuccess()) m_configuredTCK = currentTCK;
  }
}

void HltConfigSvc::verifyTCK() {

    SmartDataPtr<LHCb::ODIN> odin( m_evtSvc , LHCb::ODINLocation::Default );
    unsigned int TCK = odin->triggerConfigurationKey();

//    cout << " warning : " << (warning().isActive()?"yes":"no") << endl;

//    std::cout << "verifyTCK: currently configured TCK: " << m_configuredTCK << endl;
    debug() << "verifyTCK: currently configured TCK: " << m_configuredTCK << endmsg;
//    std::cout << "verifyTCK: TCK in ODIN bank: " << TCK << endl;
    debug() << "verifyTCK: TCK in ODIN bank: " << TCK << endmsg;

    if ( m_configuredTCK == TCK ) return;

    info() << "requesting update of from TCK " << m_configuredTCK << " to TCK " << TCK << endmsg;
    if (reconfigure( TCK ).isSuccess()) { 
        m_configuredTCK = TCK;
    } else {
        warning() << " reconfigure failed... " << endmsg;
    }
}

void HltConfigSvc::handle(const Incident& /*incident*/) {
 // std::cout << "received an Incident!" << std::endl;

  debug() << "received an Incident!" << endmsg;
  // dummyVerifyTCK();
  verifyTCK();
}
