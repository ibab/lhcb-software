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
#include "LoKi/GenVertexCuts.h"
#include "LoKi/GenParticleCuts.h"
#include "LoKi/Vertices.h"
#include "LoKi/VertexCast.h"
#include "LoKi/Particles.h"
#include "LoKi/ParticleCuts.h"
#include "LoKi/VertexCuts.h"
#include "LoKi/ParticleContextCuts.h"
#include "Kernel/IParticle2MCAssociator.h"
#include "Kernel/DVAlgorithm.h"
#include "Kernel/IEventTupleTool.h"
#include <iostream>
#include "stdio.h"
#include "stdlib.h"

#include "JetUtils.h"

#include <Kernel/IJetMaker.h>
#include "MCVerticesFunctions.h"

using namespace oscar;
using namespace LHCb;
using namespace LoKi;
using namespace LoKi::GenTypes;
using namespace LoKi::Types;
using namespace LoKi::Cuts;
using namespace LoKi::Particles;
using namespace LoKi::Vertices;

LOKI_MCALGORITHM(PFJetsMuqqMCPerMCNeut)
{

	Tuple tuple_mcneut = nTuple("Per_MCNeut","Per_MCNeut");

	ITriggerTisTos* m_TriggerTisTosTool;
	m_TriggerTisTosTool = tool<ITriggerTisTos>( "TriggerTisTos",this);
	bool Hlt2Global_Dec = Hlt2GlobalFlag(m_TriggerTisTosTool);

	//info() << "PFJets from Jet Analysis" << endreq;
	LoKi::Types::Fun bestVertexKey = BPV(VX);
	LoKi::Types::Fun mtf 		= LoKi::Cuts::INFO(9003,-10.); 
	LoKi::Types::Fun n90 		= LoKi::Cuts::INFO(9002,-10.); 
	LoKi::Types::Fun nPVInfo 	= LoKi::Cuts::INFO(9005,-10.); 
	LoKi::Types::Fun ntrk 		= LoKi::Cuts::INFO(9001,-10.);
	LoKi::Types::Fun NsatCells 	= LoKi::Cuts::INFO(9004,-10.);
	LoKi::Types::Fun N_HasPVInfo 	= LoKi::Cuts::INFO(9005,-10.);
	LoKi::Types::Fun JetCorrec 	= LoKi::Cuts::INFO(9100,-10.);
	LoKi::Types::Fun N_PVs 		= LoKi::Cuts::INFO(9101,-10.);//Number of PVs

	// Defining the JetMaker 
	IJetMaker* jetMaker = tool<IJetMaker> ( "LoKi::FastJetMaker");
	StatusCode setc = Gaudi::Utils::setProperty(jetMaker,"Recombination","1"); 
	StatusCode setc1 = Gaudi::Utils::setProperty(jetMaker,"Type","2"); 
	StatusCode setc2 = Gaudi::Utils::setProperty(jetMaker,"PtMin","5000."); 
	StatusCode setc3 = Gaudi::Utils::setProperty(jetMaker,"RParameter",".5"); 

	//IEventTupleTool * m_eventtupletool = tool<IEventTupleTool>("LoKi::Hybrid::EvtTupleTool",this);
	IEventTupleTool * m_eventtupletool = tool<IEventTupleTool>("TupleToolEventInfo",this);

	IEventTupleTool * m_eventtupletooltrigger = tool<IEventTupleTool>("TupleToolTrigger",this);
	if(!Gaudi::Utils::setProperty(m_eventtupletooltrigger,"Verbose","True"))error() << "Problem setting Verbose to true" << endreq ; 
	if(!Gaudi::Utils::setProperty(m_eventtupletooltrigger,"FillStripping","True"))error() << "Problem setting FillStripping to true" << endreq ; 

	// Defining tuple

	// Max Distance in etha x Phi space (R distance)
	const double MaxRdistance = 0.5;

	//jetcounter per Event
	unsigned int jetcounter = 0;

	//Get HepMC Particles with the final status (1 or 999)
	//LoKi::GenTypes::GRange GFinal		= gselect( "GFinal" , 1 == GSTATUS || 999 == GSTATUS);
	LoKi::GenTypes::GRange GFinal		= gselect( "GFinal" , (1 == GSTATUS || 999 == GSTATUS) && GABSID != 13);
	LoKi::GenTypes::GRange GMuons		= gselect( "GMuons" , (1 == GSTATUS || 999 == GSTATUS) && GABSID == 13);

	// Get the Partons of the events with a cut of 2 GeV on the PT
	LoKi::GenTypes::GRange GPartons	= gselect( "GPartons" , (4 == GABSID || 2 == GABSID || 1 == GABSID || 3 == GABSID || 21 == GABSID || 5 == GABSID || 6 == GABSID ) && GPT > 2000); 

	// Get the Vector Bosons W and Z (And if possible the neutralino)
	LoKi::GenTypes::GRange GVB		= gselect( "GVB" , (GABSID == 1000022)  && GNDAUGHTERS > 1);

	// Put the number of PVs on the ntuple

	// Put some variables of the Standard Jets on the ntuple

	for(LoKi::GenTypes::GRange::iterator ik1 = GVB.begin(); GVB.end() != ik1 ; ++ik1 ) {
		const HepMC::GenParticle *mcneutralino = *ik1;
		std::vector<bool> mcmuoninLHCb;
		std::vector<const Particle*> mc_neut_mcdaughters_particles;
		std::vector<const Particle*> mc_neut_mcdaughters_particles_InLHCb;
		std::vector<const HepMC::GenParticle*> mc_neut_gendaughters;

		if(!m_eventtupletool->fill(tuple_mcneut))error() << "Problem filling the event information" << endreq ;
		if(!m_eventtupletooltrigger->fill(tuple_mcneut))error() << "Problem filling the trigger and stripping event information" << endreq ;

		for (LoKi::GenTypes::GRange::const_iterator igenp = GFinal.begin() ; igenp != GFinal.end() ; ++igenp) {

			const HepMC::GenParticle * genp = * igenp;

			if(genp->production_vertex()){
				const HepMC::GenParticle *genmother = (*(genp->production_vertex()->particles_in_const_begin()));
				while(genmother->production_vertex()){
					genmother = (*(genmother->production_vertex()->particles_in_const_begin()));
					if(genmother->pdg_id() == 1000022){
						if(genmother != mcneutralino)continue;

						LHCb::Particle * part = new LHCb::Particle();
						LorentzVector gen4vectortmp(genp->momentum().px(),genp->momentum().py(),genp->momentum().pz(),genp->momentum().e());
						part->setMomentum(gen4vectortmp);
						part->setParticleID(LHCb::ParticleID(genp->pdg_id()));
						mc_neut_mcdaughters_particles.push_back(part);
						if(IsInAcceptance(part))mc_neut_mcdaughters_particles_InLHCb.push_back(part);
						mc_neut_gendaughters.push_back(genp);
					}
				}
			}
		}

		std::vector<const HepMC::GenParticle*> mc_neut_genmuons;
		for (LoKi::GenTypes::GRange::const_iterator igenp = GMuons.begin() ; igenp != GMuons.end() ; ++igenp) {

			const HepMC::GenParticle * genmuon = * igenp;

			if(genmuon->production_vertex()){
				const HepMC::GenParticle *genmother = (*(genmuon->production_vertex()->particles_in_const_begin()));
				if(genmother != mcneutralino)continue;
				mc_neut_genmuons.push_back(genmuon);
				mcmuoninLHCb.push_back(IsInAcceptance(genmuon->momentum().px(),genmuon->momentum().py(),genmuon->momentum().pz()));

			}

		}

	IJetMaker::Jets   hepmcjets, hepmcjetsinLHCb; 
	StatusCode sc = jetMaker -> makeJets ( mc_neut_mcdaughters_particles , hepmcjets ) ;
	sc = jetMaker -> makeJets (  mc_neut_mcdaughters_particles_InLHCb, hepmcjetsinLHCb ) ;

	//tuple_mcneut->farray("mcjet_mtf",mtf,hepmcjets.begin(),hepmcjets.end(),"NMaxMCJets",10000);
	//tuple_mcneut->farray("mcjet_n90",n90,hepmcjets.begin(),hepmcjets.end(),"NMaxMCJets",10000);  
	//tuple_mcneut->farray("mcjet_ntrk",ntrk,hepmcjets.begin(),hepmcjets.end(),"NMaxMCJets",10000);  

	// Put some variables of MC Jets with all daughters inside LHCb on the ntuple
	tuple_mcneut->farray("mcjet_inLHCb_px",PX,
			"mcjet_inLHCb_py",PY,
			"mcjet_inLHCb_pz",PZ,
			"mcjet_inLHCb_e",E,hepmcjetsinLHCb.begin(),hepmcjetsinLHCb.end(),"NMaxMCJetsInLHCb",10000);
	tuple_mcneut->farray("mcjet_inLHCb_pdgid",ID,hepmcjetsinLHCb.begin(), hepmcjetsinLHCb.end(),"NMaxMCJetsInLHCb",10000);

	// Put some variables of MC Jets on the ntuple
	tuple_mcneut->farray("mcjet_px",PX,
			"mcjet_py",PY,
			"mcjet_pz",PZ,
			"mcjet_e",E,hepmcjets.begin(),hepmcjets.end(),"NMaxMCJets",10000);
	tuple_mcneut->farray("mcjet_pdgid",ID,hepmcjets.begin(), hepmcjets.end(),"NMaxMCJets",10000);

	// Put some variables of MC Muons
	tuple_mcneut->farray("mcmu_px",GPX,
			"mcmu_py",GPY,
			"mcmu_pz",GPZ,
			"mcmu_e",GE,mc_neut_genmuons.begin(),mc_neut_genmuons.end(),"NMaxMCMuons",10000);
	tuple_mcneut->farray("mcmu_pdgid",GID,mc_neut_genmuons.begin(), mc_neut_genmuons.end(),"NMaxMCMuons",10000);
	tuple_mcneut->farray("mcmu_inLHCb",mcmuoninLHCb,"NMaxMCMuons",10000);

	tuple_mcneut->write();

	}

	return StatusCode::SUCCESS ;

}
