// $Id: $
#include "GaudiKernel/AlgFactory.h"

#include "ParticleRefitter.h"
#include "Kernel/IParticle2State.h"
#include "Kernel/IVertexFit.h"


//-----------------------------------------------------------------------------
// Implementation file for class : ParticleRefitter
//
// 2013-02-19 : Paul Seyfert <pseyfert@cern.ch>
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( ParticleRefitter )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ParticleRefitter::ParticleRefitter(const std::string& name, ISvcLocator* pSvcLocator)
  : DaVinciAlgorithm(name, pSvcLocator)
{
  declareProperty("FitTool", m_fittoolname = "OfflineVertexFitter");
}

//=============================================================================
// Destructor
//=============================================================================
ParticleRefitter::~ParticleRefitter() {;}

//=============================================================================
// Initialization
//=============================================================================
StatusCode ParticleRefitter::initialize()
{
  StatusCode sc = DaVinciAlgorithm::initialize();
  if (sc.isFailure()) return sc;

  if ( UNLIKELY(msgLevel(MSG::DEBUG)) ) debug() << "==> Initialize" << endmsg;

  m_stateToParticle = tool<IParticle2State>("Particle2State",this);
  m_fit = tool<IVertexFit>(m_fittoolname,this);


  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ParticleRefitter::execute()
{
  for (unsigned k = 0 ; k < m_vt.size() ; ++k) {
    delete m_vt[k];
  }
  m_vt.clear();
  setFilterPassed(false);
  if ( UNLIKELY(msgLevel(MSG::DEBUG)) ) debug() << "==> Execute" << endmsg;

  const LHCb::Particle::Range inputParticles = particles();

  if ( UNLIKELY(msgLevel(MSG::DEBUG)) ) verbose() << inputParticles.size() << "input particles" << endmsg;

  if (0==inputParticles.size()) return StatusCode::SUCCESS;

  LHCb::Particle::Range::iterator Piter = inputParticles.begin();

  StatusCode sc(StatusCode::SUCCESS);
  for ( ; inputParticles.end() != Piter ; ++Piter) {
    sc = execute(const_cast<LHCb::Particle*>(*Piter));
    if (sc.isFailure()) {
      return Warning("Particle Refitting failed, aborting event. (still returning SUCCESS, but not setting FilterPassed true)",StatusCode::SUCCESS,30);
    }
  }

  setFilterPassed(true);
  return StatusCode::SUCCESS;
}

StatusCode ParticleRefitter::execute(LHCb::Particle* part) {
  StatusCode mainsc(StatusCode::SUCCESS);
  StatusCode subsc(StatusCode::SUCCESS);
  if (part->proto() && part->proto()->track()) {
    mainsc = m_stateToParticle->state2Particle(part->proto()->track()->firstState(),*part);
  } else {
    LHCb::Particle::ConstVector daughters = part->daughtersVector();
    part->clearDaughters();
    LHCb::Particle::ConstVector::iterator dau_iter;
    for(dau_iter = daughters.begin(); dau_iter != daughters.end(); ++dau_iter)
    {
      subsc = ParticleRefitter::execute(const_cast<LHCb::Particle*>(*dau_iter));
      part->addToDaughters(*dau_iter);
      if (subsc.isFailure()) mainsc = subsc;
    }
    LHCb::Vertex* Vtx = new LHCb::Vertex(); // @todo: leave them on TES in future versions
    subsc = m_fit->fit(daughters, *Vtx, *part);
    if (subsc.isFailure()) mainsc = subsc;
    m_vt.push_back(Vtx);
  }

  return sc;
}



