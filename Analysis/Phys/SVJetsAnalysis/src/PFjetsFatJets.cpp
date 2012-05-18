//Created by Murilo Rangel

// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/LoKi.h"
#include "LoKi/LoKiAlgoMC.h"
#include "LoKi/Algo.h"
#include "LoKi/CoreCuts.h"
#include "LoKi/GenTypes.h"
#include "LoKi/GenTypes.h"
#include "LoKi/GenParticleCuts.h"
#include "LoKi/Vertices.h"
#include "LoKi/VertexCast.h"
#include "LoKi/Particles.h"
#include "LoKi/ParticleCuts.h"
#include "LoKi/VertexCuts.h"
#include "LoKi/ParticleContextCuts.h"
#include "Kernel/IParticle2MCAssociator.h"
#include "Kernel/DVAlgorithm.h"
#include <iostream>
#include "stdio.h"
#include "stdlib.h"

#include "JetUtils.h"

#include <Kernel/IJetMaker.h>

using namespace LHCb;
using namespace LoKi;
using namespace LoKi::GenTypes;
using namespace LoKi::Types;
using namespace LoKi::Cuts;
using namespace LoKi::Particles;
using namespace LoKi::Vertices;

LOKI_ALGORITHM(PFJetsFatJets)
{
	info() << " Begin " << endmsg;

	const IJetMaker* jetMaker = tool<IJetMaker> ( "LoKi::FastJetMaker");//FastJetMaker",this ) ;                                                                          
	IJetMaker::Jets   mcjets,hepgenjets,vbpvhepgenjets,mcjetsinLHCb; 

  Tuple tuple_stdpfjets = nTuple("StdPFJets","StdPFJets");
  //Tuple tuple_mcpfjets = nTuple("MCPFJets","MCPFJets");

  const double MaxRdistance = 0.6;
  unsigned int jetcounter = 0;

	info() << " 50 " << endmsg;

	info() << "Exist FatJets " << exist<LHCb::Particle::Range>("Phys/FatJets/Particles") << endmsg;

  LHCb::Particle::Range rstdjets	= get<LHCb::Particle::Range>("Phys/StdJets/Particles");

  LHCb::Particle::Range fatjets;
  if(exist<LHCb::Particle::Range>("Phys/FatJets/Particles")) fatjets = get<LHCb::Particle::Range>("Phys/FatJets/Particles");

  LHCb::Particle::Range stdloosemuons	= get<LHCb::Particle::Range>("Phys/StdLooseMuons/Particles");

  Range fattest = select ( "fattest" , INTES("Phys/FatJets/Particles", false) ) ;

  info() << "Fattest size " << fattest.size() << endmsg;
  //if(fattest.empty()) return StatusCode::SUCCESS ;

	info() << " 56 " << endmsg;
	info() << " fatsize " << fatjets.size() << endmsg;

  if(rstdjets.size() < 1) return StatusCode::SUCCESS ;

	info() << " 63 " << endmsg;

	if(exist<LHCb::Particle::Range>("Phys/FatJets/Particles"))info() << "Ok it exists" << endmsg;
  if(fatjets.size()) return StatusCode::SUCCESS ;

	info() << " 60 " << endmsg;

  LHCb::RecVertex::Range pvs = primaryVertices();
  tuple_stdpfjets->column("NRecPVs",pvs.size());

	info() << " 65 " << endmsg;

  tuple_stdpfjets->farray("stdloosemuon_px",PX,
				"stdloosemuon_py",PY,
				"stdloosemuon_pz",PZ,
				"stdloosemuon_e",E,stdloosemuons.begin(),stdloosemuons.end(),"NMaxMuons",10000);

	info() << " 75 " << endmsg;

  tuple_stdpfjets->farray("fatjet_px",PX,
				"fatjet_py",PY,
				"fatjet_pz",PZ,
				"fatjet_e",E,fatjets.begin(),fatjets.end(),"NMaxFatjets",10000);

  //tuple_stdpfjets->farray("stdjet_pt",PT,rstdjets.begin(),rstdjets.end(),"NMaxjets",10000);
  //tuple_stdpfjets->farray("stdjet_p",P,rstdjets.begin(),rstdjets.end(),"NMaxjets",10000);
  tuple_stdpfjets->farray("stdjet_px",PX,
				"stdjet_py",PY,
				"stdjet_pz",PZ,
				"stdjet_e",E,rstdjets.begin(),rstdjets.end(),"NMaxjets",10000);
  tuple_stdpfjets->farray("stdjet_pdgid",ID,rstdjets.begin(),rstdjets.end(),"NMaxjets",10000);
  //tuple_stdpfjets->farray("stdjet_eta",ETA,rstdjets.begin(),rstdjets.end(),"NMaxjets",10000);
  //tuple_stdpfjets->farray("stdjet_phi",PHI,rstdjets.begin(),rstdjets.end(),"NMaxjets",10000);
  tuple_stdpfjets->farray("stdjet_ndaughters",NDAUGHTERS,rstdjets.begin(),rstdjets.end(),"NMaxjets",10000);

  tuple_stdpfjets->column("NumberOfstdjets",rstdjets.size());
   
  std::vector<const LHCb::Particle *> jetdaughters;
  std::vector<unsigned int> jetcounterdaughters;

  std::vector<float> trackchi2ofdaughters;

  std::vector<float> stdjet_bestpvz;
  //std::vector<const LHCb::Particle *> genjets;

  jetcounter = 0;
  DVAlgorithm* dvAlg = 0;
// The tool service doesn't understand typedefs, so we have to pass this long string
const std::string finderType = "GenericParticle2PVRelator__p2PVWithIPChi2_OfflineDistanceCalculatorName_";

// get it from the tool service.
const IRelatedPVFinder* pFinder = tool<IRelatedPVFinder>(finderType,this);

// get a particle
// get some primary vertices
  LHCb::RecVertex::Range m_vertices;
//  const LHCb::RecVertex::Container* pVertices = ....;
  m_vertices = get<LHCb::RecVertex::Range>(LHCb::RecVertexLocation::Primary);
// get the best PV for the particle. See the IRelatedPVFinder doxygen for full interface.
  IRelatedPVFinder* m_pvRelator ;
  m_pvRelator = tool<IRelatedPVFinder>("GenericParticle2PVRelator__p2PVWithIPChi2_OfflineDistanceCalculatorName_/P2PVWithIPChi2", this);

  std::vector<int> number_clusters;
  std::vector<float> clusters_e;

  for (LHCb::Particle::Range::iterator ijet = rstdjets.begin() ; ijet != rstdjets.end() ; ++ijet)
  {
	  const LHCb::Particle * jet = * ijet;
	  const LHCb::VertexBase *bestPV = m_pvRelator->relatedPV(jet,m_vertices);
	  if(bestPV!=NULL)
	  stdjet_bestpvz.push_back(bestPV->position().Z());
          else stdjet_bestpvz.push_back(-9999.);
	  info() << "Line 116" << endmsg;

	  std::vector<const LHCb::Particle *> daughtersvector = jet->daughtersVector();
	  jetcounter++;

	  for (std::vector<const LHCb::Particle *>::iterator idaughter = daughtersvector.begin() ; idaughter != daughtersvector.end() ; ++idaughter){

		  const LHCb::Particle * daughter = *idaughter;
		  info() << "Line 124" << endmsg;

		  jetdaughters.push_back(daughter);
		  jetcounterdaughters.push_back(jetcounter);

		  if (daughter == NULL)continue;// ?????

		  if (daughter->proto() != NULL){
			  if (daughter->proto()->track() != NULL){
			  //info() << daughter->proto() << endmsg; 
			  //info() << daughter->proto()->track() << endmsg; 
			  //info() << daughter->proto()->track()->chi2() << endmsg; 
			  trackchi2ofdaughters.push_back(daughter->proto()->track()->chi2());
			  }
			  else trackchi2ofdaughters.push_back(-10000);
		  }
		  else trackchi2ofdaughters.push_back(-10000);

		  if(daughter->proto() == NULL) {number_clusters.push_back(0);continue;}
		  if(daughter->proto()->calo().size() < 1) {number_clusters.push_back(0);continue;}

		  const SmartRefVector < LHCb::CaloHypo > daughtercalo = daughter->proto()->calo();

		  //for (SmartRefVector<LHCb::CaloHypo>::const_iterator icalodaughter = daughter->proto()->calo().begin() ; icalodaughter != daughter->proto()->calo().end() ; ++icalodaughter){
		  const LHCb::CaloHypo *calodaughter = * (daughter->proto()->calo().begin());//* icalodaughter;

		  number_clusters.push_back(calodaughter->clusters().size());
		  info() << "Line 151" << endmsg;

		  SmartRefVector<LHCb::CaloCluster>::const_iterator icluster = calodaughter->clusters().begin();

		  for (; icluster != calodaughter->clusters().end() ; ++icluster){
			  const LHCb::CaloCluster *cluster = * icluster;
			  clusters_e.push_back(cluster->e());

		  }
		  info() << "Line 160" << endmsg;
		  //break;// only the first Hypo 
		  //}
	  }

	  info() << "Line 165" << endmsg;



  }
  
		  info() << "Line 171" << endmsg;

  //tuple_stdpfjets->farray("jetdaughters_pt",PT,jetdaughters.begin(),jetdaughters.end(),"NMaxdaughters",10000);
  //tuple_stdpfjets->farray("jetdaughters_p",P,jetdaughters.begin(),jetdaughters.end(),"NMaxdaughters",10000);
  tuple_stdpfjets->farray("jetdaughters_trackchi2",trackchi2ofdaughters,"NMaxdaughters",10000);
  tuple_stdpfjets->farray("jetdaughters_px",PX,
				"jetdaughters_py",PY,
				"jetdaughters_pz",PZ,
				"jetdaughters_e",E,jetdaughters.begin(),jetdaughters.end(),"NMaxdaughters",10000);
  //tuple_stdpfjets->farray("jetdaughters_jetid",jetcounterdaughters,"NMaxdaughters",10000);
  tuple_stdpfjets->farray("jetdaughters_pdgid",ID,
			  "jetdaughters_info",INFO4JET,jetdaughters.begin(),jetdaughters.end(),"NMaxdaughters",10000);
  tuple_stdpfjets->farray("stdjet_nclusters",number_clusters,"NMaxjets",10000);
  tuple_stdpfjets->farray("stdjet_clusters_e",clusters_e,"NMaxjets",10000);
  //tuple_stdpfjets->farray("jetdaughters_eta",ETA,jetdaughters.begin(),jetdaughters.end(),"NMaxdaughters",10000);
  //tuple_stdpfjets->farray("jetdaughters_phi",PHI,jetdaughters.begin(),jetdaughters.end(),"NMaxdaughters",10000);

  tuple_stdpfjets->farray("stdjet_bestpvz",stdjet_bestpvz,"NMaxjets",10000);

  tuple_stdpfjets->write();

  info() << "Line 192" << endmsg;

  return StatusCode::SUCCESS ;

}
