// $Id: $
#ifndef MCVERTICESFUNCTIONS_H
#define MCVERTICESFUNCTIONS_H 1
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/LoKi.h"
#include "Kernel/IParticle2MCAssociator.h"

// ============================================================
// JetMaker.h
// ============================================================
//#include "LoKiFastJetMaker.h"
#include <Kernel/IJetMaker.h>
// Trigger Tool
#include "Kernel/ITriggerTisTos.h"
// ===========================================================================

/// avoid long names:
using namespace LoKi        ;
using namespace LoKi::Types ;
using namespace LoKi::Cuts  ;
using namespace LoKi::Particles  ;
using namespace LoKi::Vertices  ;
using namespace LHCb;


namespace oscar {
	//IParticle2MCAssociator * m_assoc = tool<IParticle2MCAssociator>("MCMatchObjP2MCRelator");

	bool IsAtMaterial(const LHCb::Particle* p){return p->info(51,-1000.)>-900;}

	bool IsAtMaterial(LHCb::Particle* p){return p->info(51,-1000.)>-900;}

	bool IsInAcceptance(double px, double py, double pz){return pz > 0 ? (std::abs(px/pz) < std::tan(0.3) && std::abs(py/pz) < std::tan(0.25)) ? true : false : false;}
	bool IsInVeloVz(double vz){return vz > -175 && vz < 750 ? true : false;}
	bool IsInVeloVz(float vz){return vz > -175 && vz < 750 ? true : false;}

	bool IsInAcceptance(const LHCb::MCParticle* testmcparticle){return IsInAcceptance(testmcparticle->momentum().X() ,testmcparticle->momentum().Y() ,testmcparticle->momentum().Z()) ;}
	bool IsInAcceptance(const LHCb::Particle* testparticle){return IsInAcceptance(testparticle->momentum().X() ,testparticle->momentum().Y() ,testparticle->momentum().Z()) ;}


	bool IsInAcceptance(LHCb::MCParticle* testmcparticle){return IsInAcceptance(testmcparticle->momentum().X() ,testmcparticle->momentum().Y() ,testmcparticle->momentum().Z()) ;}
	bool IsInAcceptance(LHCb::Particle* testparticle){return IsInAcceptance(testparticle->momentum().X() ,testparticle->momentum().Y() ,testparticle->momentum().Z()) ;}
	bool IsInAcceptance(LHCb::MCParticle testmcparticle){return IsInAcceptance(testmcparticle.momentum().X() ,testmcparticle.momentum().Y() ,testmcparticle.momentum().Z()) ;}
	bool IsInAcceptance(LHCb::Particle testparticle){return IsInAcceptance(testparticle.momentum().X() ,testparticle.momentum().Y() ,testparticle.momentum().Z()) ;}


	bool IsInVeloVz(const LHCb::MCParticle* testmcparticle){return IsInVeloVz(testmcparticle->originVertex()->position().Z()) ;}
	//bool IsInVeloVz(const LHCb::Particle* testparticle){return IsInVeloVz(testparticle->originVertex()->position().Z()) ;}
	bool IsInVeloVz(LHCb::MCParticle* testmcparticle){return IsInVeloVz(testmcparticle->originVertex()->position().Z()) ;}
	//bool IsInVeloVz(LHCb::Particle* testparticle){return IsInVeloVz(testparticle->originVertex()->position().Z()) ;}
	bool IsInVeloVz(LHCb::MCParticle testmcparticle){return IsInVeloVz(testmcparticle.originVertex()->position().Z()) ;}
	//bool IsInVeloVz(LHCb::Particle testparticle){return IsInVeloVz(testparticle.originVertex().Z()) ;}
//	bool IsMCVertexReconstructed(LHCb::MCVertex* mctestvertex,std::vector<const LHCb::Vertex*> vertices);


	double MCIPtoPrim(const LHCb::MCParticle *testparticle){
		double primvX = testparticle->primaryVertex()->position().X();
		double primvY = testparticle->primaryVertex()->position().Y();
		double primvZ = testparticle->primaryVertex()->position().Z();

		double ovX = testparticle->originVertex()->position().X();
		double ovY = testparticle->originVertex()->position().Y();
		double ovZ = testparticle->originVertex()->position().Z();

		double diffX = ovX - primvX;
		double diffY = ovY - primvY;
		double diffZ = ovZ - primvZ;

		double 	scalardiffprim = (diffX * primvX) + (diffY * primvY) + (diffZ * primvZ);

		double primv2 = (primvX * primvX) + (primvY * primvY) + (primvZ * primvZ) ;

		double IPX = diffX - ( primvX * (scalardiffprim) / primv2 ); 
		double IPY = diffY - ( primvY * (scalardiffprim) / primv2 ); 
		double IPZ = diffZ - ( primvZ * (scalardiffprim) / primv2 ); 

		double IP = std::sqrt( (IPX * IPX) +(IPY * IPY) +(IPZ * IPZ) ) ;

		return IP;}


	bool Hlt2GlobalFlag(ITriggerTisTos* m_TriggerTisTosTool){//Do the Hlt2
	m_TriggerTisTosTool->setTriggerInput("Hlt2Global");
	//Fill the decision, tis and tos parametres for the Hlt2 as a whole   
	bool decision, tis, tos;
	m_TriggerTisTosTool->triggerTisTos("Hlt2Global",decision,tis,tos);
	return decision;
	}


	double GetRFromBL(const Gaudi::XYZPoint &p , LHCb::Particle *m_BeamLine){
		//intersection of the beam line with the XY plane, 
		//find the lambda parameter of the line.
		double lambda = (p.z() - m_BeamLine->referencePoint().z()) /
			m_BeamLine->momentum().z();

		//find x and y of intersection point
		double x = m_BeamLine->referencePoint().x() 
			+ lambda * m_BeamLine->momentum().x();
		double y = m_BeamLine->referencePoint().y() 
			+ lambda * m_BeamLine->momentum().y();

		//return distance to the intersection point 
		x -= p.x(); y -= p.y();
		return sqrt( x*x + y*y );
	}

}

#endif /*MCVERTICESFUNCTIONS_H*/ 
