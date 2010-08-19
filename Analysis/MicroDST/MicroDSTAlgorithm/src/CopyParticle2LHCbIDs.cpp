// $Id: CopyParticle2LHCbIDs.cpp,v 1.1 2010-08-19 15:34:31 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include <GaudiKernel/ObjectVector.h>
// LHCb 
#include "Kernel/ITriggerTisTos.h" 
#include "Kernel/LHCbID.h"
#include "Event/Particle.h"
// DaVinci
#include "Kernel/DaVinciStringUtils.h"
// local
#include "CopyParticle2LHCbIDs.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CopyParticle2LHCbIDs
//
// 2010-08-18 : Juan Palacios
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CopyParticle2LHCbIDs );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CopyParticle2LHCbIDs::CopyParticle2LHCbIDs( const std::string& name,
                                            ISvcLocator* pSvcLocator)
  : MicroDSTAlgorithm ( name , pSvcLocator )
{

}
//=============================================================================
// Destructor
//=============================================================================
CopyParticle2LHCbIDs::~CopyParticle2LHCbIDs() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CopyParticle2LHCbIDs::initialize() {
  StatusCode sc = MicroDSTAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_iTisTos = tool<ITriggerTisTos>("TriggerTisTos", this);

  return m_iTisTos ? StatusCode::SUCCESS : StatusCode::FAILURE;

}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CopyParticle2LHCbIDs::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;


  typedef std::vector<std::string>::const_iterator stringIter;
  stringIter locBegin = this->inputTESLocations().begin();
  stringIter locEnd = this->inputTESLocations().end();

      
  for (stringIter iLoc = locBegin; iLoc != locEnd; ++iLoc) {
    
    const std::string inputLocation = MicroDST::niceLocationName(*iLoc);

    executeLocation(inputLocation);

  }
    
  return StatusCode::SUCCESS;

}
//=============================================================================
void CopyParticle2LHCbIDs::executeLocation(const std::string& particleLocation) 
{

  std::string outputLocation = particleLocation;
  
  DaVinci::StringUtils::removeEnding(outputLocation, "/Particles");

  outputLocation += "/Particle2LHCbIDMap";

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

  Particle2LHCbIDVectorMap* p2LHCbID = new Particle2LHCbIDVectorMap;

  put(p2LHCbID, outputLocation);

  LHCb::Particle::Range::const_iterator iPart = particles.begin();
  LHCb::Particle::Range::const_iterator iPartEnd = particles.end();

  for ( ; iPart != iPartEnd; ++iPart) {
    const LHCb::Particle* clone = getStoredClone<LHCb::Particle>(*iPart);
    if (clone) {
      m_iTisTos->setOfflineInput(); 
      m_iTisTos->addToOfflineInput(**iPart);
      std::vector<LHCb::LHCbID> signalHits = m_iTisTos->offlineLHCbIDs();
      p2LHCbID->insert(clone, signalHits);
    }
  }

  // test that we find the stuff.
  
  Particle2LHCbIDVectorMap* test = get<Particle2LHCbIDVectorMap>(outputLocation);
  if (test) {
    debug() << "Test passed, found P2LHCbID map with " 
            << test->size() <<" entries!" << endmsg;
    if (!test->empty()) {
      for (Particle2LHCbIDVectorMap::const_iterator iMap = test->begin();
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
StatusCode CopyParticle2LHCbIDs::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
