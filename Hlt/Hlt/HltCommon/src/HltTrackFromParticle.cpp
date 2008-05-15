// $Id: HltTrackFromParticle.cpp,v 1.5 2008-05-15 08:56:55 graven Exp $
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

  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  m_outputTracks = &(registerTSelection<LHCb::Track>());

  saveConfiguration();
  info() << "HltAlgorithm initialized";

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltTrackFromParticle::execute() {

  StatusCode sc = HltAlgorithm::execute(); // must be executed first

  //Check if particles exist
  if (!exist<Particles>(m_particlesName)) return sc;

  Particles* pars = get<Particles>(m_particlesName);
  if (m_debug) {
    if (pars == 0) verbose() << " no particles found! " << endreq;
    else verbose() << " particles found " << pars->size() << endreq;
  }  
  if (pars == 0) return sc;
  
  m_outputTracks->clear();
  for (Particles::iterator it = pars->begin(); it != pars->end(); ++it) {
    loadParticle(**it);
  }
  
  if (m_debug) {
    debug() << " candidates found " << m_outputTracks->size() << endreq;
    printInfo(" tracks from particles ",*m_outputTracks);
  }
  
  return sc;
}

void HltTrackFromParticle::loadParticle(const Particle& par) {
  verbose() << " loading " << par.pt() << endreq;
  if (par.isBasicParticle()) {
    const Track* track = par.proto()->track();
    if (track)  m_outputTracks->push_back( const_cast<Track*>(track));
    verbose() << " loading particle " << par.pt() 
              << " as track " << track->pt() << endreq;
  } else {
    const SmartRefVector<Particle>& pars2 = par.daughters();
    for (SmartRefVector<Particle>::const_iterator it2 = pars2.begin(); 
         it2 != pars2.end(); ++it2) {
      loadParticle(**it2);
    }
  }  
}
