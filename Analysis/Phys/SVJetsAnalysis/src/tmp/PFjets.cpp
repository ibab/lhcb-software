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
#include <iostream>
#include "stdio.h"
#include "stdlib.h"

#include "JetUtils.h"

using namespace LHCb;
using namespace LoKi;
using namespace LoKi::GenTypes;
using namespace LoKi::Types;
using namespace LoKi::Cuts;
using namespace LoKi::Particles;
using namespace LoKi::Vertices;

LOKI_MCALGORITHM(PFJetsMC)
{

  Tuple tuple_stdpfjets = nTuple("StdPFJets","StdPFJets");
  Tuple tuple_stdpfjets_daughters = nTuple("StdPFJetsDaughters","StdPFJetsDaughters");
  Tuple tuple_stdpfjets_gendaughters = nTuple("StdPFJetsGenDaughters","StdPFJetsGenDaughters");
  Tuple tuple_stdpfjets_genjets = nTuple("StdPFJetsGenJets","StdPFJetsGenJets");
  Tuple tuple_stdpfjets_qgmothers = nTuple("StdPFJetsQGMothers","StdPFJetsQGMpthers");

  const double MaxRdistance = 0.6;

  LoKi::GenTypes::GRange GFinal		= gselect( "GFinal" , 1 == GSTATUS );

  LoKi::GenTypes::GRange GQuarks	= gselect( "GQuarks" , (4 == GABSID || 2 == GABSID || 1 == GABSID || 3 == GABSID || 21 == GABSID || 5 == GABSID || 6 == GABSID ) && GPT > 2000); 

  LoKi::GenTypes::GRange GVBON		= gselect( "GVBON" , GABSID == 1000022 || 24 == GABSID || 23 == GABSID );

  LHCb::Particle::Range rstdjets	= get<LHCb::Particle::Range>("Phys/StdJets/Particles");

  tuple_stdpfjets->farray("stdjet_pt",PT,rstdjets.begin(),rstdjets.end(),"NMaxjets",10000);
  tuple_stdpfjets->farray("stdjet_p",P,rstdjets.begin(),rstdjets.end(),"NMaxjets",10000);
  tuple_stdpfjets->farray("stdjet_px",PX,rstdjets.begin(),rstdjets.end(),"NMaxjets",10000);
  tuple_stdpfjets->farray("stdjet_py",PY,rstdjets.begin(),rstdjets.end(),"NMaxjets",10000);
  tuple_stdpfjets->farray("stdjet_pz",PZ,rstdjets.begin(),rstdjets.end(),"NMaxjets",10000);
  tuple_stdpfjets->farray("stdjet_e",E,rstdjets.begin(),rstdjets.end(),"NMaxjets",10000);
  tuple_stdpfjets->farray("stdjet_pdgid",ID,rstdjets.begin(),rstdjets.end(),"NMaxjets",10000);
  tuple_stdpfjets->farray("stdjet_eta",ETA,rstdjets.begin(),rstdjets.end(),"NMaxjets",10000);
  tuple_stdpfjets->farray("stdjet_phi",PHI,rstdjets.begin(),rstdjets.end(),"NMaxjets",10000);
  tuple_stdpfjets->farray("stdjet_ndaughters",NDAUGHTERS,rstdjets.begin(),rstdjets.end(),"NMaxjets",10000);

  tuple_stdpfjets->column("NumberOfstdjets",rstdjets.size());
   
  std::vector<const LHCb::Particle *> jetdaughters;

  for (LHCb::Particle::Range::iterator ijet = rstdjets.begin() ; ijet != rstdjets.end() ; ++ijet)
  {
	  const LHCb::Particle * jet = * ijet;
	  std::vector<const LHCb::Particle *> daughtersvector = jet->daughtersVector();

	  for (std::vector<const LHCb::Particle *>::iterator idaughter = daughtersvector.begin() ; idaughter != daughtersvector.end() ; ++idaughter){

		  const LHCb::Particle * daughter = *idaughter;
		  jetdaughters.push_back(daughter);

	  }
  }
  

  tuple_stdpfjets_daughters->farray("jetdaughters_pt",PT,jetdaughters.begin(),jetdaughters.end(),"NMaxdaughters",10000);
  tuple_stdpfjets_daughters->farray("jetdaughters_p",P,jetdaughters.begin(),jetdaughters.end(),"NMaxdaughters",10000);
  tuple_stdpfjets_daughters->farray("jetdaughters_px",PX,jetdaughters.begin(),jetdaughters.end(),"NMaxdaughters",10000);
  tuple_stdpfjets_daughters->farray("jetdaughters_py",PY,jetdaughters.begin(),jetdaughters.end(),"NMaxdaughters",10000);
  tuple_stdpfjets_daughters->farray("jetdaughters_pz",PZ,jetdaughters.begin(),jetdaughters.end(),"NMaxdaughters",10000);
  tuple_stdpfjets_daughters->farray("jetdaughters_e",E,jetdaughters.begin(),jetdaughters.end(),"NMaxdaughters",10000);
  tuple_stdpfjets_daughters->farray("jetdaughters_pdgid",ID,jetdaughters.begin(),jetdaughters.end(),"NMaxdaughters",10000);
  tuple_stdpfjets_daughters->farray("jetdaughters_eta",ETA,jetdaughters.begin(),jetdaughters.end(),"NMaxdaughters",10000);
  tuple_stdpfjets_daughters->farray("jetdaughters_phi",PHI,jetdaughters.begin(),jetdaughters.end(),"NMaxdaughters",10000);
  tuple_stdpfjets_daughters->farray("jetdaughters_info",INFO4JET,jetdaughters.begin(),jetdaughters.end(),"NMaxdaughters",10000);

  std::vector<int> Ngendaughters;
  std::vector<const HepMC::GenParticle *> jetgendaughters;
  std::vector<const LHCb::Particle *> genjets;

  for (LHCb::Particle::Range::iterator ijet = rstdjets.begin() ; ijet != rstdjets.end() ; ++ijet)
   {
           const LHCb::Particle * jet = * ijet;
	   int numberofgendaughters = 0;
	   LHCb::Particle * GenJet = new LHCb::Particle() ;//98 = jetid
	   LorentzVector * genjet4vector = new LorentzVector(0,0,0,0);

	   for (LoKi::GenTypes::GRange::iterator igen = GFinal.begin() ; igen != GFinal.end() ; ++igen)
	   {

	           const HepMC::GenParticle * gen = * igen;

		   double deltaPhi = std::abs(jet->momentum().phi() - gen->momentum().phi());
		   if(deltaPhi > std::acos(-1)) deltaPhi = 2*std::acos(-1) - deltaPhi;
		   double deltaEta = jet->momentum().eta() - gen->momentum().eta(); 

	           if (std::sqrt( std::pow(deltaPhi,2) + std::pow(deltaEta,2)) < MaxRdistance)
	           {
	        	   jetgendaughters.push_back(gen);
	        	   numberofgendaughters++;
			   *genjet4vector += (LorentzVector(gen->momentum().px(),gen->momentum().py(),gen->momentum().pz(),gen->momentum().e()));
	           }


	   }

	   GenJet->setMomentum(*genjet4vector);
	   Ngendaughters.push_back(numberofgendaughters);
	   genjets.push_back(GenJet);
   }

  tuple_stdpfjets->farray("stdjet_ngendaughters",Ngendaughters,"NMaxjets",10000);

  if(jetgendaughters.size()){
  tuple_stdpfjets_gendaughters->farray("jetgendaughters_pt",GPT,jetgendaughters.begin(),jetgendaughters.end(),"NMaxgendaughters",10000);
  tuple_stdpfjets_gendaughters->farray("jetgendaughters_p",GP,jetgendaughters.begin(),jetgendaughters.end(),"NMaxgendaughters",10000);
  tuple_stdpfjets_gendaughters->farray("jetgendaughters_px",GPX,jetgendaughters.begin(),jetgendaughters.end(),"NMaxgendaughters",10000);
  tuple_stdpfjets_gendaughters->farray("jetgendaughters_py",GPY,jetgendaughters.begin(),jetgendaughters.end(),"NMaxgendaughters",10000);
  tuple_stdpfjets_gendaughters->farray("jetgendaughters_pz",GPZ,jetgendaughters.begin(),jetgendaughters.end(),"NMaxgendaughters",10000);
  tuple_stdpfjets_gendaughters->farray("jetgendaughters_e",GE,jetgendaughters.begin(),jetgendaughters.end(),"NMaxgendaughters",10000);
  tuple_stdpfjets_gendaughters->farray("jetgendaughters_pdgid",GID,jetgendaughters.begin(),jetgendaughters.end(),"NMaxgendaughters",10000);
  tuple_stdpfjets_gendaughters->farray("jetgendaughters_eta",GETA,jetgendaughters.begin(),jetgendaughters.end(),"NMaxgendaughters",10000);
  tuple_stdpfjets_gendaughters->farray("jetgendaughters_phi",GPHI,jetgendaughters.begin(),jetgendaughters.end(),"NMaxgendaughters",10000);
  tuple_stdpfjets_gendaughters->write();
  }

  if(genjets.size()){
  tuple_stdpfjets_genjets->farray("genjet_p",PT,genjets.begin(),genjets.end(),"NMaxgenjets",10000);
  tuple_stdpfjets_genjets->farray("genjet_pt",P,genjets.begin(),genjets.end(),"NMaxgenjets",10000);
  tuple_stdpfjets_genjets->farray("genjet_px",PX,genjets.begin(),genjets.end(),"NMaxgenjets",10000);
  tuple_stdpfjets_genjets->farray("genjet_py",PY,genjets.begin(),genjets.end(),"NMaxgenjets",10000);
  tuple_stdpfjets_genjets->farray("genjet_pz",PZ,genjets.begin(),genjets.end(),"NMaxgenjets",10000);
  tuple_stdpfjets_genjets->farray("genjet_e",E,genjets.begin(),genjets.end(),"NMaxgenjets",10000);
  tuple_stdpfjets_genjets->farray("genjet_eta",ETA,genjets.begin(),genjets.end(),"NMaxgenjets",10000);
  tuple_stdpfjets_genjets->farray("genjet_phi",PHI,genjets.begin(),genjets.end(),"NMaxgenjets",10000);
  tuple_stdpfjets_genjets->write();
  }

  if(rstdjets.size()){
  tuple_stdpfjets->write();
  tuple_stdpfjets_daughters->write();
  }


  std::vector<const HepMC::GenParticle *> jetqgmothers;
  //std::vector<const LHCb::Particle *> genjets;

  for (LHCb::Particle::Range::iterator ijet = rstdjets.begin() ; ijet != rstdjets.end() ; ++ijet)
   {
           const LHCb::Particle * jet = * ijet;

	   for (LoKi::GenTypes::GRange::iterator iquark = GQuarks.begin() ; iquark != GQuarks.end() ; ++iquark)
	   {

	           const HepMC::GenParticle * quark = * iquark;

		   double deltaPhi = std::abs(jet->momentum().phi() - quark->momentum().phi());
		   if(deltaPhi > std::acos(-1)) deltaPhi = 2*std::acos(-1) - deltaPhi;
		   double deltaEta = jet->momentum().eta() - quark->momentum().eta(); 

	           if (std::sqrt( std::pow(deltaPhi,2) + std::pow(deltaEta,2)) < MaxRdistance)
	           {
	        	   jetqgmothers.push_back(quark);
	           }


	   }

   }


  if(jetqgmothers.size()){
  tuple_stdpfjets_qgmothers->farray("jetqgmothers_pt",GPT,jetqgmothers.begin(),jetqgmothers.end(),"NMaxqgmothers",10000);
  tuple_stdpfjets_qgmothers->farray("jetqgmothers_p",GP,jetqgmothers.begin(),jetqgmothers.end(),"NMaxqgmothers",10000);
  tuple_stdpfjets_qgmothers->farray("jetqgmothers_px",GPX,jetqgmothers.begin(),jetqgmothers.end(),"NMaxqgmothers",10000);
  tuple_stdpfjets_qgmothers->farray("jetqgmothers_py",GPY,jetqgmothers.begin(),jetqgmothers.end(),"NMaxqgmothers",10000);
  tuple_stdpfjets_qgmothers->farray("jetqgmothers_pz",GPZ,jetqgmothers.begin(),jetqgmothers.end(),"NMaxqgmothers",10000);
  tuple_stdpfjets_qgmothers->farray("jetqgmothers_e",GE,jetqgmothers.begin(),jetqgmothers.end(),"NMaxqgmothers",10000);
  tuple_stdpfjets_qgmothers->farray("jetqgmothers_pdgid",GID,jetqgmothers.begin(),jetqgmothers.end(),"NMaxqgmothers",10000);
  tuple_stdpfjets_qgmothers->farray("jetqgmothers_eta",GETA,jetqgmothers.begin(),jetqgmothers.end(),"NMaxqgmothers",10000);
  tuple_stdpfjets_qgmothers->farray("jetqgmothers_phi",GPHI,jetqgmothers.begin(),jetqgmothers.end(),"NMaxqgmothers",10000);
  tuple_stdpfjets_qgmothers->write();
  }

  return StatusCode::SUCCESS ;

}
