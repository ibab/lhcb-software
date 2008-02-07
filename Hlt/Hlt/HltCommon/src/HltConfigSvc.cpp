// $Id: HltConfigSvc.cpp,v 1.1 2008-02-07 16:36:58 graven Exp $
// Include files 

#include <algorithm>

#include "boost/lexical_cast.hpp"

// from Gaudi
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/SvcFactory.h"

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
{
  declareProperty("TCK2ConfigMap", m_tck2config);
  declareProperty("initialTCK", m_initialTCK = TCK_t(1));
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
  return PropertyConfigSvc::finalize();
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltConfigSvc::initialize() {
  StatusCode status = PropertyConfigSvc::initialize();
  if ( !status.isSuccess() ) return status;

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
  if (m_initialTCK!=0) {
     status = configure( m_initialTCK );
     if (status.isSuccess()) m_configuredTCK = m_initialTCK;
  }
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
    return PropertyConfigSvc::configure( PropertyConfig::createDigest(i->second) );
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
    return PropertyConfigSvc::reconfigure( PropertyConfig::createDigest( i->second) );
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
    return PropertyConfigSvc::loadConfig( PropertyConfig::createDigest(i->second) );
}

//=============================================================================
// Check TCK on 'beginEvent' incident
//=============================================================================
void HltConfigSvc::handle(const Incident& /*incident*/) {
  info() << "HltConfigSvc::handle: currently configured TCK: " << m_configuredTCK << endl;
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
      info() << "updating config from TCK " << m_configuredTCK << " to TCK " << currentTCK << endl;
      StatusCode sc = reconfigure( currentTCK);
      if (sc.isSuccess()) m_configuredTCK = currentTCK;
  }
}
