// $Id: HltTrackFromParticle.cpp,v 1.11 2009-03-23 16:14:41 gligorov Exp $
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
  , m_selections(*this)
{

  declareProperty( "ParticlesName" , m_particlesName = "");
  m_selections.declareProperties();
}

//=============================================================================
// Destructor
//=============================================================================
HltTrackFromParticle::~HltTrackFromParticle() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltTrackFromParticle::initialize() {

  m_selections.registerSelection();

  info() << "HltAlgorithm initialized";
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltTrackFromParticle::execute() {

  StatusCode sc;

  //Check if particles exist
  if (!exist<Particles>(m_particlesName)) return sc;

  Particles* pars = get<Particles>(m_particlesName);
  if (msgLevel(MSG::DEBUG)) {
    if (pars == 0) verbose() << " no particles found! " << endreq;
    else           verbose() << " particles found " << pars->size() << endreq;
  }  
  if (pars == 0) return sc;
  
  m_selections.output()->clean();
//  std::for_each( boost::iterator::make_indirect_iterator(pars->begin()),
//                 boost::iterator::make_indirect_iterator(pars->end()),
//                 boost::bind(&HltTrackFromParticle::loadParticle,this,_1));
  for (Particles::iterator it = pars->begin(); it != pars->end(); ++it) {
    loadParticle(**it);
  }
  
  if (msgLevel(MSG::DEBUG)) {
    debug() << " candidates found " << m_selections.output()->size() << endreq;
    printInfo(" tracks from particles ",*m_selections.output());
  }
  
  return sc;
}

void HltTrackFromParticle::loadParticle(const Particle& par) {
  verbose() << " loading " << par.pt() << endreq;
  if (par.isBasicParticle()) {
    const Track* track = par.proto()->track();
    if (track) m_selections.output()->push_back( const_cast<Track*>(track));
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
