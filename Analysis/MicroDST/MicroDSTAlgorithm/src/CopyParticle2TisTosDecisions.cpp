// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include <GaudiKernel/ObjectVector.h>
// LHCb 
#include "Kernel/ITriggerTisTos.h" 
#include "Event/Particle.h"
#include "Event/HltDecReports.h"
// DaVinci
#include "Kernel/DaVinciStringUtils.h"
#include "Kernel/Particle2UnsignedInts.h"
// local
#include "CopyParticle2TisTosDecisions.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CopyParticle2TisTosDecisions
//
// 2010-09-28 : Juan Palacios
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CopyParticle2TisTosDecisions );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CopyParticle2TisTosDecisions::CopyParticle2TisTosDecisions( const std::string& name,
                                                            ISvcLocator* pSvcLocator)
  : MicroDSTAlgorithm ( name , pSvcLocator ),
  m_decReports(0),
  m_decReportsLocation(LHCb::HltDecReportsLocation::Default)
{
  declareProperty("HltDecReportsLocation", m_decReportsLocation);
}
//=============================================================================
// Destructor
//=============================================================================
CopyParticle2TisTosDecisions::~CopyParticle2TisTosDecisions() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CopyParticle2TisTosDecisions::initialize() {
  StatusCode sc = MicroDSTAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_iTisTos = tool<ITriggerTisTos>("TriggerTisTos", this);

  return m_iTisTos ? StatusCode::SUCCESS : StatusCode::FAILURE;

}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CopyParticle2TisTosDecisions::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;


  m_decReports = get<LHCb::HltDecReports>(m_decReportsLocation);

  if (0==m_decReports) {
    return Warning("No LHCb::HltDecReports in " +
            m_decReportsLocation,
            StatusCode::SUCCESS, 
            0);
  }

  typedef std::vector<std::string>::const_iterator stringIter;
  stringIter iLoc = this->inputTESLocations().begin();
  stringIter locEnd = this->inputTESLocations().end();

      
  for (; iLoc != locEnd; ++iLoc) {
    
    const std::string inputLocation = niceLocationName(*iLoc);

    executeLocation(inputLocation);

  }
    
  return StatusCode::SUCCESS;

}
//=============================================================================
void CopyParticle2TisTosDecisions::executeLocation(const std::string& particleLocation) 
{

  std::string outputLocation = particleLocation;
  
  DaVinci::StringUtils::removeEnding(outputLocation, "/Particles");

  outputLocation += "/Particle2TisTos";

  outputLocation = this->outputTESLocation( outputLocation );

  verbose() << "Going to clone LHCbIDs from Particles in " << particleLocation
            << " into " << outputLocation << endmsg;

  if (!exist<LHCb::Particle::Range>(particleLocation) ) {
    Warning("No LHCb::Particle::Range found at " + 
            particleLocation,
            StatusCode::SUCCESS, 
            0).ignore();
    return;
  }

  const LHCb::Particle::Range particles = 
    get<LHCb::Particle::Range>(particleLocation);
  
  if (particles.empty()) {
    Warning("No LHCb::Particles in LHCb::Particle::Range " +
            particleLocation,
            StatusCode::SUCCESS, 
            0).ignore();
    return;
  }

  DaVinci::Map::Particle2UnsignedInts* p2TisTos = new DaVinci::Map::Particle2UnsignedInts;

  put(p2TisTos, outputLocation);

  LHCb::Particle::Range::const_iterator iPart = particles.begin();
  LHCb::Particle::Range::const_iterator iPartEnd = particles.end();

  for ( ; iPart != iPartEnd; ++iPart) {
    const LHCb::Particle* clone = getStoredClone<LHCb::Particle>(*iPart);
    if (clone) {

      m_iTisTos->setOfflineInput(); 
      m_iTisTos->addToOfflineInput(**iPart);

      std::vector<std::string> decNames = m_decReports->decisionNames();
      std::vector<unsigned int> decisions;

      std::vector<std::string>::const_iterator iName = decNames.begin();
      std::vector<std::string>::const_iterator nameEnd = decNames.end();

      for ( ; iName != nameEnd; ++ iName) {
        decisions.push_back(m_iTisTos->tisTosSelection(*iName));
      }

      p2TisTos->insert(clone, decisions);

    }
  }

  // test that we find the stuff.
  
  DaVinci::Map::Particle2UnsignedInts* test = get<DaVinci::Map::Particle2UnsignedInts>(outputLocation);
  if (test) {
    debug() << "Test passed, found P2LHCbID map with " 
            << test->size() <<" entries!" << endmsg;
    if (!test->empty()) {
      for (DaVinci::Map::Particle2UnsignedInts::const_iterator iMap = test->begin();
           iMap!=test->end();
           ++iMap) {
        debug() << "Found " << (*iMap).second.size() << " LHCbIDs" << endmsg;
      }
    }
    
  }

  else {
    error() << "Test failed, found no P2LHCbID map" << endmsg;
  }
  
  
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode CopyParticle2TisTosDecisions::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
