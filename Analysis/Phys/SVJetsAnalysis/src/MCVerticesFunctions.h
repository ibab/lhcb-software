// $Id: $
#ifndef MCVERTICESFUNCTIONS_H
#define MCVERTICESFUNCTIONS_H 1
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/LoKi.h"
#include "Kernel/IParticle2MCAssociator.h"
#include "LoKi/Algo.h"
#include "LoKi/Vertices.h"
#include "LoKi/VertexCast.h"
#include "LoKi/Particles.h"
#include "LoKi/ParticleCuts.h"
#include "LoKi/VertexCuts.h"
//#include <Kernel/IPhysDesktop.h>
#include "LoKi/ParticleContextCuts.h"
#include "LoKi/LoKiAlgoMC.h"
#include "LoKi/GenTypes.h"
#include "LoKi/GenTypes.h"
#include "LoKi/GenVertexCuts.h"
#include "LoKi/GenParticleCuts.h"

#include "Kernel/IParticle2MCAssociator.h"
#include "Kernel/DVAlgorithm.h"
#include "Kernel/IEventTupleTool.h"
// ============================================================
// JetMaker.h
// ============================================================
//#include "LoKiFastJetMaker.h"
#include <Kernel/IJetMaker.h>
// Trigger Tool (Tistos)
#include "Kernel/ITriggerTisTos.h"
// ===========================================================================

/// avoid long names:
using namespace LoKi        ;
using namespace LoKi::Types ;
using namespace LoKi::Cuts  ;
using namespace LoKi::Particles  ;
using namespace LoKi::Vertices  ;
using namespace LHCb;

using namespace LoKi::GenTypes;
using namespace LoKi::Cuts;

namespace oscar {
	//IParticle2MCAssociator * m_assoc = tool<IParticle2MCAssociator>("MCMatchObjP2MCRelator");

	bool IsAtMaterial(const LHCb::Particle* p);

	bool IsAtMaterial(LHCb::Particle* p);

	bool IsInAcceptance(double px, double py, double pz);

	bool IsInAcceptance(const LHCb::MCParticle* testmcparticle);
	bool IsInAcceptance(const LHCb::Particle* testparticle);
	bool IsInAcceptance(LHCb::MCParticle* testmcparticle);
	bool IsInAcceptance(LHCb::Particle* testparticle);
	bool IsInAcceptance(LHCb::MCParticle testmcparticle);
	bool IsInAcceptance(LHCb::Particle testparticle);
	//bool IsMCVertexReconstructed(LHCb::MCVertex* mctestvertex,std::vector<const LHCb::Vertex*> vertices);

	bool IsInVeloVz(double vz);
	bool IsInVeloVz(float vz);
	bool IsInVeloVz(LHCb::MCParticle* testmcparticle);
	bool IsInVeloVz(LHCb::Particle* testparticle);
	bool IsInVeloVz(LHCb::MCParticle testmcparticle);
	bool IsInVeloVz(LHCb::Particle testparticle);
	bool IsInVeloVz(const LHCb::MCParticle* testmcparticle);
	bool IsInVeloVz(const LHCb::Particle* testparticle);

	double MCIPtoPrim(const LHCb::MCParticle *testparticle);

	bool Hlt2GlobalFlag(ITriggerTisTos* m_TriggerTisTosTool);
	double GetRFromBL(const Gaudi::XYZPoint &p , LHCb::Particle *m_BeamLine);
}

#endif /*MCVERTICESFUNCTIONS_H*/ 
