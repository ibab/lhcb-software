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

LOKI_MCALGORITHM(PFJetsMC)
{

	// Defining the JetMaker 
	IJetMaker* jetMaker = tool<IJetMaker> ( "LoKi::FastJetMaker");
	StatusCode setc = Gaudi::Utils::setProperty(jetMaker,"Recombination","1"); 
	StatusCode setc1 = Gaudi::Utils::setProperty(jetMaker,"Type","2"); 
	StatusCode setc2 = Gaudi::Utils::setProperty(jetMaker,"PtMin","5000."); 
	StatusCode setc3 = Gaudi::Utils::setProperty(jetMaker,"RParameter",".5"); 
	IJetMaker::Jets   mcjets,hepgenjets,vbpvhepgenjets,mcjetsinLHCb; 

	// Defining tuple
	Tuple tuple_stdpfjets = nTuple("StdPFJets","StdPFJets");

	// Max Distance in etha x Phi space (R distance)
	const double MaxRdistance = 0.6;

	//jetcounter per Event
	unsigned int jetcounter = 0;

	//Get HepMC Particles with the final status (1 or 999)
	LoKi::GenTypes::GRange GFinal		= gselect( "GFinal" , 1 == GSTATUS || 999 == GSTATUS);

	// Get the Partons of the events with a cut of 2 GeV on the PT
	LoKi::GenTypes::GRange GPartons	= gselect( "GPartons" , (4 == GABSID || 2 == GABSID || 1 == GABSID || 3 == GABSID || 21 == GABSID || 5 == GABSID || 6 == GABSID ) && GPT > 2000); 

	// Get the Vector Bosons W and Z (And if possible the neutralino)
	LoKi::GenTypes::GRange GVB		= gselect( "GVB" , GABSID == 1000022 || 24 == GABSID || 23 == GABSID );

	//Get all mcparticle without any endvertices 
	MCRange mcparticles = mcselect("mcparticles", MCNENDVERTICES == 0);

	// Standard Jets from Particles Flow (Reminder put the inputlocation of the stdjets for this loki program :) )
	LHCb::Particle::Range rstdjets = select("StdJets", ABSID == 98); 

	// Get all reconstructed primary vertices
	LHCb::RecVertex::Range pvs = primaryVertices();

	// Put the number of PVs on the ntuple
	tuple_stdpfjets->column("NRecPVs",pvs.size());

	// Put some variables of the Standard Jets on the ntuple
	tuple_stdpfjets->farray("stdjet_px",PX,
			"stdjet_py",PY,
			"stdjet_pz",PZ,
			"stdjet_e",E,rstdjets.begin(),rstdjets.end(),"NMaxjets",10000);
	tuple_stdpfjets->farray("stdjet_pdgid",ID,rstdjets.begin(),rstdjets.end(),"NMaxjets",10000);
	tuple_stdpfjets->farray("stdjet_ndaughters",NDAUGHTERS,rstdjets.begin(),rstdjets.end(),"NMaxjets",10000);

	// Number of PVs per Event
	tuple_stdpfjets->column("NumberOfstdjets",rstdjets.size());

	// A ton of vector of variables 
	std::vector<const LHCb::Particle *> jetdaughters;
	std::vector<unsigned int> jetcounterdaughters;
	std::vector<float> trackchi2ofdaughters;

	std::vector<int> Ngendaughters;
	std::vector<const HepMC::GenParticle *> jetgendaughters;
	std::vector<const LHCb::Particle *> genjets;
	std::vector<unsigned int> jetcountergenjetsdaughters;

	std::vector<const HepMC::GenParticle *> jetqgmothers;
	std::vector<unsigned int> jetcounterjetqgmothers;
	std::vector<int> numberofqgmothers;
	std::vector<float> stdjet_bestpvz;

	std::vector<int> number_clusters;
	std::vector<int> stdjet_daughter_tracktype;
	std::vector<float> clusters_e;
	std::vector<float> stdjet_bestdaughterPVz_FromPF,stdjet_bestdaughterPVz, stdjet_bestdaughterPVIP;

	// get a particle
	// get some primary vertices
	LHCb::RecVertex::Range m_vertices;
	//  const LHCb::RecVertex::Container* pVertices = ....;
	m_vertices = get<LHCb::RecVertex::Range>(LHCb::RecVertexLocation::Primary);
	// get the best PV for the particle. See the IRelatedPVFinder doxygen for full interface.
	IRelatedPVFinder* m_pvRelator ;
	m_pvRelator = tool<IRelatedPVFinder>("GenericParticle2PVRelator__p2PVWithIPChi2_OfflineDistanceCalculatorName_/P2PVWithIPChi2", this);


	for (LHCb::Particle::Range::iterator ijet = rstdjets.begin() ; ijet != rstdjets.end() ; ++ijet) {
		// Loop on the stddjets 
		const LHCb::Particle * jet = * ijet;

		const LHCb::VertexBase* bestPVV = getStoredBestPV(jet);

		stdjet_bestpvz.push_back(bestPVV->position().Z());

		std::vector<const LHCb::Particle *> daughtersvector = jet->daughtersVector();
		jetcounter++;

		int numberofgendaughters = 0;
		LHCb::Particle * GenJet = new LHCb::Particle() ;//98 = jetid
		LorentzVector * genjet4vector = new LorentzVector(0,0,0,0);


		int nqgmothers = 0;

		for (std::vector<const LHCb::Particle *>::iterator idaughter = daughtersvector.begin() ; idaughter != daughtersvector.end() ; ++idaughter){
			//Loop on jet daughters (particle used to make the jet)
			const LHCb::Particle * daughter = *idaughter;
			const LHCb::VertexBase *bestDaughterPV1 = bestPV(daughter);
			const LHCb::VertexBase *bestDaughterPV = m_pvRelator->relatedPV(daughter,m_vertices);

			jetdaughters.push_back(daughter);
			jetcounterdaughters.push_back(jetcounter);

			// A lot o cheks before get the tracktype
			(daughter->proto() == NULL) ? 
				stdjet_daughter_tracktype.push_back(0): 
				(daughter->proto()->track() == NULL) ? 
				stdjet_daughter_tracktype.push_back(0) : stdjet_daughter_tracktype.push_back(daughter->proto()->track()->type()); 

			if(bestDaughterPV!=NULL){
				Fun bdaughterpvip = IP (bestDaughterPV, geo() )  ;
				stdjet_bestdaughterPVz.push_back(bestDaughterPV->position().Z());
				stdjet_bestdaughterPVz_FromPF.push_back(bestDaughterPV->position().Z());
				stdjet_bestdaughterPVIP.push_back(bdaughterpvip(daughter));
			}
			else {
				stdjet_bestdaughterPVz.push_back(-9999.);
				stdjet_bestdaughterPVz_FromPF.push_back(-9999.);
				stdjet_bestdaughterPVIP.push_back(-9999.);
			}

			if (daughter == NULL)continue;// ?????

			if (daughter->proto() != NULL){
				if (daughter->proto()->track() != NULL){
					trackchi2ofdaughters.push_back(daughter->proto()->track()->chi2());
				}
				else trackchi2ofdaughters.push_back(-10000);
			}
			else trackchi2ofdaughters.push_back(-10000);

			continue;

			// Energy from the clusters (WITH PROBLEMS)
			if(daughter->proto() == NULL) {number_clusters.push_back(0);continue;}
			if(daughter->proto()->calo().size() < 1) {number_clusters.push_back(0);continue;}


			const SmartRefVector < LHCb::CaloHypo > daughtercalo = daughter->proto()->calo();

			const LHCb::CaloHypo *calodaughter = * (daughter->proto()->calo().begin());

			number_clusters.push_back(calodaughter->clusters().size());

			SmartRefVector<LHCb::CaloCluster>::const_iterator icluster = calodaughter->clusters().begin();

			for (; icluster != calodaughter->clusters().end() ; ++icluster){
				const LHCb::CaloCluster *cluster = * icluster;
				clusters_e.push_back(cluster->e());

			}

		}

		for (LoKi::GenTypes::GRange::iterator iquark = GPartons.begin() ; iquark != GPartons.end() ; ++iquark)
		{
			//Loop on quarks (HepMC)

			const HepMC::GenParticle * quark = * iquark;

			double deltaPhi = std::abs(jet->momentum().phi() - quark->momentum().phi());
			if(deltaPhi > std::acos(-1)) deltaPhi = 2*std::acos(-1) - deltaPhi;
			double deltaEta = jet->momentum().eta() - quark->momentum().eta(); 

			if (std::sqrt( std::pow(deltaPhi,2) + std::pow(deltaEta,2)) < MaxRdistance)
			{
				jetcounterjetqgmothers.push_back(jetcounter);
				nqgmothers++;
				jetqgmothers.push_back(quark);
			}


		}

		numberofqgmothers.push_back(nqgmothers);

		GenJet->setMomentum(*genjet4vector);
		Ngendaughters.push_back(numberofgendaughters);
		genjets.push_back(GenJet);
	}



	// Fill the tuple with jet daughters variables
	tuple_stdpfjets->farray("jetdaughters_trackchi2",trackchi2ofdaughters,"NMaxdaughters",10000);
	tuple_stdpfjets->farray("jetdaughters_bestPVz",stdjet_bestdaughterPVz,"NMaxdaughters",10000);
	tuple_stdpfjets->farray("jetdaughters_bestPVz_FromPF",stdjet_bestdaughterPVz_FromPF,"NMaxdaughters",10000);
	tuple_stdpfjets->farray("jetdaughters_bestPVIP",stdjet_bestdaughterPVIP,"NMaxdaughters",10000);
	tuple_stdpfjets->farray("jetdaughters_tracktype",stdjet_daughter_tracktype,"NMaxdaughters",10000);

	tuple_stdpfjets->farray("jetdaughters_px",PX,
			"jetdaughters_py",PY,
			"jetdaughters_pz",PZ,
			"jetdaughters_e",E,jetdaughters.begin(),jetdaughters.end(),"NMaxdaughters",10000);

	tuple_stdpfjets->farray("jetdaughters_pdgid",ID,
			"jetdaughters_info",INFO4JET,jetdaughters.begin(),jetdaughters.end(),"NMaxdaughters",10000);

	// Best PV (z component)
	tuple_stdpfjets->farray("stdjet_bestpvz",stdjet_bestpvz,"NMaxjets",10000);

	// Most probably q or gluon mother of the jet
	tuple_stdpfjets->farray("stdjet_nqgmothers",numberofqgmothers,"NMaxjets",10000);

	tuple_stdpfjets->farray("jetqgmothers_px",GPX,
			"jetqgmothers_py",GPY,
			"jetqgmothers_pz",GPZ,
			"jetqgmothers_e",GE,jetqgmothers.begin(),jetqgmothers.end(),"NMaxqgmothers",10000);
	tuple_stdpfjets->farray("jetqgmothers_pdgid",GID,jetqgmothers.begin(),jetqgmothers.end(),"NMaxqgmothers",10000);

	HepMC::GenVertex *GV;
	LHCb::Vertex *PV = new LHCb::Vertex();
	HepMC::GenEvent *VBosonEvent;

	for(LoKi::GenTypes::GRange::iterator ik1 = GVB.begin(); GVB.end() != ik1 ; ++ik1 ) {
		// Loop on Vector Bosons (HepMC)
		const HepMC::GenParticle *Vgen = *ik1;

		if(ik1==GVB.begin()) {
			// Obs.: Only for the first entry
			GV = Vgen->production_vertex(); VBosonEvent = Vgen->parent_event();
			const Gaudi::XYZPoint xyz(GV->point3d().x(),GV->point3d().y(),GV->point3d().z());
			PV->setPosition(xyz);
			tuple_stdpfjets->column("V_px",Vgen->momentum().px());
			tuple_stdpfjets->column("V_py",Vgen->momentum().py());
			tuple_stdpfjets->column("V_pz",Vgen->momentum().pz());
			tuple_stdpfjets->column("V_e",Vgen->momentum().e());
			tuple_stdpfjets->column("V_vtxz",GV->point3d().z());

			HepMC::GenVertex *final = Vgen->end_vertex();
			while(final->particles_out_size ()==1) final = (*(final->particles_out_const_begin()))->end_vertex(); 


			tuple_stdpfjets->farray("V_daughters_px",GPX,
					"V_daughters_py",GPY,
					"V_daughters_pz",GPZ,
					"V_daughters_e",GE, 
					final->particles_out_const_begin(),final->particles_out_const_end(),"NMaxVdaughters",10000);

			tuple_stdpfjets->farray("V_daughters_pdgid",GID,
					final->particles_out_const_begin(),final->particles_out_const_end(),"NMaxVdaughters",10000);

		}

		break;
	}

	std::vector<const HepMC::GenParticle *> genparts_from_pv_vboson;

	for(LoKi::GenTypes::GRange::iterator ik1 = GFinal.begin(); GFinal.end() != ik1 ; ++ik1 ) {
		const HepMC::GenParticle *gen = *ik1;
		if(gen->momentum().pz()<0) continue;
		if(VBosonEvent->signal_process_id() == gen->parent_event()->signal_process_id()) {

			// HepMC stable particles from the same process id of the vector boson
			genparts_from_pv_vboson.push_back(gen);

		}
	}

	std::vector<const LHCb::Particle *> vbpvgenp4jets;
	for (std::vector<const HepMC::GenParticle *>::iterator igenp = genparts_from_pv_vboson.begin() ; igenp != genparts_from_pv_vboson.end() ; ++igenp)
	{
		// Get the HEPMC particles to make a jet
		// Sum the 4-momentum of the previously selected HepMC particles

		const HepMC::GenParticle * genp = * igenp;
		LHCb::Particle * part = new LHCb::Particle();
		LorentzVector gen4vectortmp(genp->momentum().px(),genp->momentum().py(),genp->momentum().pz(),genp->momentum().e());
		part->setMomentum(gen4vectortmp);
		part->setParticleID(LHCb::ParticleID(genp->pdg_id()));
		vbpvgenp4jets.push_back(part);
	}

	StatusCode sc3 = jetMaker -> makeJets ( vbpvgenp4jets , vbpvhepgenjets ) ;

	// Fill some varibles of the jets made using the HepMC particles 
	tuple_stdpfjets->farray("vbpvhepgenjet_px",PX,
			"vbpvhepgenjet_py",PY,
			"vbpvhepgenjet_pz",PZ,
			"vbpvhepgenjet_e",E,vbpvhepgenjets.begin(),vbpvhepgenjets.end(),"NMaxvbpvhepgenjets",10000);
	tuple_stdpfjets->farray("vbpvhepgenjet_pdgid",ID,
			"vbpvhepgenjet_ndaughters",NDAUGHTERS,vbpvhepgenjets.begin(),vbpvhepgenjets.end(),"NMaxvbpvhepgenjets",10000);

	jetcounter = 0;
	std::vector<const LHCb::Particle *> vbpvjetdaughters;
	std::vector<int> vbpvjetcounterdaughters;
	std::vector<const HepMC::GenParticle *> vbpvjetpartons;
	std::vector<int> vbpvjetcounterpartons;

	for (std::vector<Particle *>::iterator ijet = vbpvhepgenjets.begin() ; ijet != vbpvhepgenjets.end() ; ++ijet)
	{
		const LHCb::Particle * jet = * ijet;
		std::vector<const LHCb::Particle *> daughtersvector = jet->daughtersVector();
		jetcounter++;
		LHCb::Particle * GenJet = new LHCb::Particle() ;//98 = jetid
		int quarkcounter = 0;


		for (LoKi::GenTypes::GRange::iterator iquark = GPartons.begin() ; iquark != GPartons.end() ; ++iquark)
		{

			const HepMC::GenParticle * quark = * iquark;

			double deltaPhi = std::abs(jet->momentum().phi() - quark->momentum().phi());
			if(deltaPhi > std::acos(-1)) deltaPhi = 2*std::acos(-1) - deltaPhi;
			double deltaEta = jet->momentum().eta() - quark->momentum().eta(); 

			if (std::sqrt( std::pow(deltaPhi,2) + std::pow(deltaEta,2)) < MaxRdistance)
			{
				//Check for partons inside the jet with a R distance lower than MaxRdistance
				vbpvjetpartons.push_back(quark);
				quarkcounter++;
			}


		}

		vbpvjetcounterpartons.push_back(quarkcounter);

		for (std::vector<const LHCb::Particle *>::iterator idaughter = daughtersvector.begin() ; idaughter != daughtersvector.end() ; ++idaughter){

			// Get the daughters of the HepMC Jets

			const LHCb::Particle * daughter = *idaughter;
			vbpvjetdaughters.push_back(daughter);
			vbpvjetcounterdaughters.push_back(jetcounter);

		}
	}

	// Fill variables of the HepMC particles (jets and daughters)
	tuple_stdpfjets->farray("vbpvjetdaughters_px",PX,
			"vbpvjetdaughters_py",PY,
			"vbpvjetdaughters_pz",PZ,
			"vbpvjetdaughters_e",E,vbpvjetdaughters.begin(),vbpvjetdaughters.end(),"NMaxvbpvjetdaughters",10000);

	tuple_stdpfjets->farray("vbpvjetpartons_px", GPX,
			"vbpvjetpartons_py",GPY,
			"vbpvjetpartons_pz",GPZ,
			"vbpvjetpartons_e",GE,vbpvjetpartons.begin(),vbpvjetpartons.end(),"NMaxvbpvjetpartons",10000);

	tuple_stdpfjets->farray("vbpvhepgenjet_pdgid",GID,vbpvjetpartons.begin(),vbpvjetpartons.end(),"NMaxvbpvjetpartons",10000);

	tuple_stdpfjets->farray("vbpvhepgenjet_npartons",vbpvjetcounterpartons,"NMaxvbpvhepgenjets",10000);

	tuple_stdpfjets->farray("vbpvjetdaughters_pdgid",ID,
			"vbpvjetdaughters_ndaughters",NDAUGHTERS,vbpvjetdaughters.begin(),vbpvjetdaughters.end(),"NMaxvbpvjetdaughters",10000);

	tuple_stdpfjets->write();

	return StatusCode::SUCCESS ;

}
