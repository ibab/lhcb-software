// $Id: MCParticleLinkerMaker.cpp,v 1.2 2010-04-16 14:17:32 jpalac Exp $
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
DECLARE_ALGORITHM_FACTORY( MCParticleLinkerMaker )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  MCParticleLinkerMaker::MCParticleLinkerMaker( const std::string& name,
                                                ISvcLocator* pSvcLocator)
    : DaVinciTupleAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputMCLocation", m_inputMC = LHCb::MCParticleLocation::Default );
  declareProperty( "InputParticleLocation", m_inputParticle = LHCb::ParticleLocation::User );
  declareProperty( "writeTuple", m_writeTuple = false );
}
//=============================================================================
// Destructor
//=============================================================================
MCParticleLinkerMaker::~MCParticleLinkerMaker() {}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MCParticleLinkerMaker::execute()
{

  LHCb::MCParticle::Container* mcparticles = get<LHCb::MCParticle::Container>(m_inputMC);
  const LHCb::Particle::Range particles = get<LHCb::Particle::Range>(m_inputParticle);

  if ( (!mcparticles || mcparticles->empty()) || particles.empty() ) {
    debug() << "    No MCParticles/Particles retrieved from" << m_inputMC << "/" <<m_inputParticle<< endmsg;
    return StatusCode::SUCCESS;
  }
  LinkerWithKey<LHCb::MCParticle,  LHCb::Particle>  myLink(evtSvc(), msgSvc(),m_inputParticle);

  bool linked;
  for ( LHCb::Particle::Range::const_iterator ip = particles.begin(); ip != particles.end() ; ++ip ){
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

              Tuple PullTuple = nTuple("MCparticleMakerPull");

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


