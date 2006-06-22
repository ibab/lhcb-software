// $Id: MCParticleLinkerMaker.cpp,v 1.1 2006-06-22 12:38:48 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
// ============================================================================
// local
#include "Event/Particle.h"
#include "Event/MCParticle.h"
#include "MCParticleLinkerMaker.h"
#include "Linker/LinkerWithKey.h"
// ============================================================================
//-----------------------------------------------------------------------------
// Implementation file for class : MCParticleLinkerMaker
//
// 2006-06-09 : Stefania Vecchi
//-----------------------------------------------------------------------------
using namespace Gaudi::Units;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MCParticleLinkerMaker );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCParticleLinkerMaker::MCParticleLinkerMaker( const std::string& name,
                                              ISvcLocator* pSvcLocator) 
  : DVAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputMCLocation", m_inputMC = LHCb::MCParticleLocation::Default );
  declareProperty( "InputParticleLocation", m_inputParticle = LHCb::ParticleLocation::User );  
  declareProperty( "writeTuple", m_writeTuple = false );  
}
//=============================================================================
// Destructor
//=============================================================================
MCParticleLinkerMaker::~MCParticleLinkerMaker() {} 

//=================================w============================================
// Initialization
//=============================================================================
StatusCode MCParticleLinkerMaker::initialize() {
  //=== The following two lines should be commented for DC04 algorithms ! ===
  StatusCode sc = DVAlgorithm::initialize(); 
  if ( sc.isFailure() ) return sc;

  debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MCParticleLinkerMaker::execute() {

  debug() << "==> Execute" << endmsg;
  Tuple PullTuple = nTuple("MCparticleMakerPull");
  LHCb::MCParticle::Container* mcparticles = get<LHCb::MCParticle::Container>(m_inputMC);

  // code goes here  

  LHCb::Particles*     particles = get<LHCb::Particles>(m_inputParticle);
  //  LHCb::MCParticles* mcparticles = get<LHCb::MCParticles>(m_inputParticle);

  if ( !mcparticles || (0 == mcparticles->size()) ||!particles || (0 == particles->size()) ){
    debug() << "    No MCParticles/Particles retrieved from" << m_inputMC << "/" <<m_inputParticle<< endmsg;
    return StatusCode::SUCCESS;
  }
  LinkerWithKey<LHCb::MCParticle,  LHCb::Particle>  myLink(evtSvc(), msgSvc(),m_inputParticle);

  bool linked;
  for ( LHCb::Particles::const_iterator ip = particles->begin(); ip != particles->end() ; ++ip ){
    linked=false;
    if((*ip)->hasKey()) {
      for ( LHCb::MCParticle::Container::const_iterator imc = mcparticles->begin() ;
            imc != mcparticles->end() ; ++imc ){
        if((*imc)->hasKey()){ 
          if((*imc)->key()==(*ip)->key()){
            linked=true;
            myLink.link((*ip),(*imc), 1.);
            debug()<< " Created Linker MCParticle-Particle"<<endmsg;
            if(m_writeTuple) {
              double Pullx=((*imc)->originVertex()->position().x() - (*ip)->referencePoint().x())/
                sqrt((*ip)->posCovMatrix()(0,0));
              double Pully=((*imc)->originVertex()->position().y() - (*ip)->referencePoint().y())/
                sqrt((*ip)->posCovMatrix()(1,1));
              double Pullz=((*imc)->originVertex()->position().z() - (*ip)->referencePoint().z())/
                sqrt((*ip)->posCovMatrix()(2,2));
              double PullPx=((*imc)->momentum().Px() - (*ip)->momentum().Px())/
                sqrt((*ip)->momCovMatrix()(0,0));
              double PullPy=((*imc)->momentum().Py() - (*ip)->momentum().Py())/
                sqrt((*ip)->momCovMatrix()(1,1));
              double PullPz=((*imc)->momentum().Pz() - (*ip)->momentum().Pz())/
                sqrt((*ip)->momCovMatrix()(2,2));
              double mom=(*ip)->p()/GeV;

              PullTuple->fill("Pullx",Pullx);
              PullTuple->fill("Pully",Pully);
              PullTuple->fill("Pullz",Pullz);
              PullTuple->fill("PullPx",PullPx);
              PullTuple->fill("PullPy",PullPy);
              PullTuple->fill("PullPz",PullPz);
              PullTuple->fill("P",mom);

              PullTuple->write();
            }
            
          }
          
        }
      }      
    }
    if(!linked) debug() << "Particle not linked to MCparticle"<<endmsg;
  }
  
  setFilterPassed(true);   // Mandatory. Set to true if event is accepted.
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode MCParticleLinkerMaker::finalize() {

  debug() << "==> Finalize" << endmsg;

  return DVAlgorithm::finalize(); //=== For DC04, return StatusCode::SUCCESS;
}

//=============================================================================


