// $Id: HltTrackFromParticle.cpp,v 1.1 2007-06-20 12:17:38 hernando Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// from Event
#include "HltTrackFromParticle.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltMCChecker
//
// 2007-06-04 : Jose A. Hernando
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltTrackFromParticle );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltTrackFromParticle::HltTrackFromParticle( const std::string& name, ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{

  declareProperty( "ParticlesName" , m_particlesName = "");
}

//=============================================================================
// Destructor
//=============================================================================
HltTrackFromParticle::~HltTrackFromParticle() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltTrackFromParticle::initialize() {

  info() << "Starts" << endmsg;
  
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  checkInput(m_outputTracks," output tracks ");
  info() << "HltAlgorithm initialized";

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltTrackFromParticle::execute() {

  StatusCode sc = HltAlgorithm::execute(); // must be executed first

  Particles* pars = get<Particles>(m_particlesName);
  if (m_debug) {
    if (pars == NULL) verbose() << " no particles found! " << endreq;
    else verbose() << " particles found " << pars->size() << endreq;
  }  
  if (pars == NULL) return sc;
  
  m_outputTracks->clear();
  for (Particles::iterator it = pars->begin(); it != pars->end(); ++it) {
    Particle& par = *(*it);
    loadParticle(par);
  }
  
  int ncan = m_outputTracks->size();
  candidateFound(ncan);
  
  debug() << " candidates found " << ncan << endreq;
  if (m_debug)
    printInfo(" tracks from particles ",*m_outputTracks);
  
  return sc;
  
}

void HltTrackFromParticle::loadParticle(const Particle& par) {
  verbose() << " loading " << par.pt() << endreq;
  if (par.isBasicParticle()) {
    const Track* track = par.proto()->track();
    if (track)  m_outputTracks->push_back( (Track*) track);
    verbose() << " loading particle " << par.pt() 
              << " as track " << track->pt() << endreq;
  } else {
    const SmartRefVector<Particle>& pars2 = par.daughters();
    for (SmartRefVector<Particle>::const_iterator it2 = pars2.begin(); 
         it2 != pars2.end(); ++it2) {
      const Particle& par2 = *(*it2);      
      loadParticle(par2);
    }
  }  
}


//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltTrackFromParticle::finalize() {

  StatusCode sc = HltAlgorithm::finalize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  debug() << "==> Finalize" << endreq;

  return  StatusCode::SUCCESS;
}

