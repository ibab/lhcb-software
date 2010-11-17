// $Id: CopyParticle2LHCbIDs.cpp,v 1.2 2010-08-20 09:47:07 jpalac Exp $
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
#include "Kernel/Particle2LHCbIDs.h"
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
  : MicroDSTAlgorithm ( name , pSvcLocator ),
    m_fullTree(false),
    m_iTisTos(0)
{

  declareProperty("FullDecayTree", m_fullTree);

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
  stringIter iLoc = this->inputTESLocations().begin();
  stringIter locEnd = this->inputTESLocations().end();

      
  for ( ; iLoc != locEnd; ++iLoc) {
    
    const std::string inputLocation = niceLocationName(*iLoc);

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

  DaVinci::Map::Particle2LHCbIDs* p2LHCbID = new DaVinci::Map::Particle2LHCbIDs;

  put(p2LHCbID, outputLocation);

  LHCb::Particle::Range::const_iterator iPart = particles.begin();
  LHCb::Particle::Range::const_iterator iPartEnd = particles.end();

  for ( ; iPart != iPartEnd; ++iPart) {
    storeLHCbIDs(*iPart, p2LHCbID);
  }

  // test that we find the stuff.

  if ( msgLevel(MSG::DEBUG) ) {
    
  
    DaVinci::Map::Particle2LHCbIDs* test = get<DaVinci::Map::Particle2LHCbIDs>(outputLocation);
    if (test) {
      debug() << "Test passed, found P2LHCbID map with " 
              << test->size() <<" entries!" << endmsg;
      if (!test->empty()) {
        for (DaVinci::Map::Particle2LHCbIDs::const_iterator iMap = test->begin();
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
  
}
//=============================================================================
void CopyParticle2LHCbIDs::storeLHCbIDs(const LHCb::Particle* part,
                                        DaVinci::Map::Particle2LHCbIDs* p2LHCbID)
{
  const LHCb::Particle* clone = getStoredClone<LHCb::Particle>(part);
  if (0==clone) return;

  m_iTisTos->setOfflineInput(); 
  m_iTisTos->addToOfflineInput(*part);
  std::vector<LHCb::LHCbID> signalHits = m_iTisTos->offlineLHCbIDs();
  p2LHCbID->insert(clone, signalHits);

  if (m_fullTree) {
    const SmartRefVector<LHCb::Particle>& daughters = part->daughters();
    if (daughters.empty()) return;
    SmartRefVector<LHCb::Particle>::const_iterator iDau = daughters.begin();
    SmartRefVector<LHCb::Particle>::const_iterator iDauEnd = daughters.end();
    for (; iDau != iDauEnd; ++iDau) {
      storeLHCbIDs(*iDau, p2LHCbID);
    }
    
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
