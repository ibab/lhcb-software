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
#include "Kernel/IEventTupleTool.h"
#include "LoKi/ParticleContextCuts.h"
#include "Kernel/IParticle2MCAssociator.h"
#include "Kernel/DVAlgorithm.h"
#include <iostream>
#include "stdio.h"
#include "stdlib.h"
#include "TMath.h"
#include "DoubleTopoTool.h"

#include "JetUtils.h"

#include <Kernel/IJetMaker.h>

using namespace LHCb;
using namespace LoKi;
using namespace LoKi::GenTypes;
using namespace LoKi::Types;
using namespace LoKi::Cuts;
using namespace LoKi::Particles;
using namespace LoKi::Vertices;

LOKI_MCALGORITHM(PFbjets)
{
	//info() << "PFJets from Jet Analysis" << endreq;
	LoKi::Types::Fun ECAL_E = LoKi::Cuts::PPINFO(332,-10.);
	LoKi::Types::Fun HCAL_E = LoKi::Cuts::PPINFO(333,-10.);

	LoKi::Types::Fun cpf = LoKi::Cuts::INFO(9006,-10.);
	LoKi::Types::Fun width = LoKi::Cuts::INFO(9007,-10.);
	LoKi::Types::Fun mtf 		= LoKi::Cuts::INFO(9003,-10.); 
	LoKi::Types::Fun n90 		= LoKi::Cuts::INFO(9002,-10.); 
	LoKi::Types::Fun nPVInfo 	= LoKi::Cuts::INFO(9005,-10.); 
	LoKi::Types::Fun ntrk 		= LoKi::Cuts::INFO(9001,-10.);
	LoKi::Types::Fun NsatCells 	= LoKi::Cuts::INFO(9004,-10.);
	LoKi::Types::Fun N_HasPVInfo 	= LoKi::Cuts::INFO(9005,-10.);
	LoKi::Types::Fun JetCorrec 	= LoKi::Cuts::INFO(9100,-10.);
	LoKi::Types::Fun N_PVs 		= LoKi::Cuts::INFO(9101,-10.);//Number of PVs

	LoKi::Types::Fun bestVertexKey = BPV(VX);

	// Defining the JetMaker 
	IJetMaker* jetMaker = tool<IJetMaker> ( "LoKi::FastJetMaker");
	StatusCode setc = Gaudi::Utils::setProperty(jetMaker,"Recombination","1"); 
	StatusCode setc1 = Gaudi::Utils::setProperty(jetMaker,"Type","2"); 
	StatusCode setc2 = Gaudi::Utils::setProperty(jetMaker,"PtMin","10000."); 
	StatusCode setc3 = Gaudi::Utils::setProperty(jetMaker,"RParameter",".5"); 
	IJetMaker::Jets   vbpvhepgenjets; 

	// Defining tuple
	Tuple tuple_stdpfjets = nTuple("StdPFJets","StdPFJets");

	// Max Distance in etha x Phi space (R distance)
	const double MaxRdistance = 0.5;

	//jetcounter per Event
	unsigned int jetcounter = 0;

	// Standard Jets from Particles Flow (Reminder put the inputlocation of the stdjets for this loki program :) )
	LHCb::Particle::Range rstdjets = select("StdJets", ABSID == 98 && PT>5000); 
        //if(rstdjets.size()<1) return StatusCode::SUCCESS;
        LHCb::Particle::Range fatjets  = select("FatJets", ABSID == 99);
        LHCb::Particle::Range stdloosemuons  = select("StdLooseMuons", ABSID == 13);
        LHCb::Track::Range alltrks      = get<LHCb::Track::Range>("/Event/Rec/Track/Best") ;

	// Get all reconstructed primary vertices
	LHCb::RecVertex::Range pvs = primaryVertices();

	// Put the number of PVs on the ntuple
	tuple_stdpfjets->column("NRecPVs",pvs.size());

	tuple_stdpfjets->farray("stdloosemuon_px",PX,
			"stdloosemuon_py",PY,
			"stdloosemuon_pz",PZ,
			"stdloosemuon_e",E,stdloosemuons.begin(),stdloosemuons.end(),"NMaxMuons",10000);
	tuple_stdpfjets->farray("stdloosemuon_pdgid",ID,
			"stdloosemuon_bpvvz",BPV(VZ),
			"stdloosemuon_chi2dof",TRCHI2DOF,stdloosemuons.begin(),stdloosemuons.end(),"NMaxMuons",10000);

	tuple_stdpfjets->farray("stdloosemuon_ECAL_E",ECAL_E,stdloosemuons.begin(),stdloosemuons.end(),"NMaxMuons",10000);
	tuple_stdpfjets->farray("stdloosemuon_HCAL_E",HCAL_E,stdloosemuons.begin(),stdloosemuons.end(),"NMaxMuons",10000);

	tuple_stdpfjets->farray("fatjet_px",PX,
			"fatjet_py",PY,
			"fatjet_pz",PZ,
			"fatjet_e",E,fatjets.begin(),fatjets.end(),"NMaxFatjets",10000);
//	tuple_stdpfjets->farray("fatjet_pdgid",ID,fatjets.begin(),fatjets.end(),"NMaxFatjets",10000);
	tuple_stdpfjets->farray("fatjet_ndaughters",NDAUGHTERS,fatjets.begin(),fatjets.end(),"NMaxFatjets",10000);

	// Put some variables of the Standard Jets on the ntuple
	tuple_stdpfjets->farray("stdjet_px",PX,
			"stdjet_py",PY,
			"stdjet_pz",PZ,
			"stdjet_e",E,rstdjets.begin(),rstdjets.end(),"NMaxjets",10000);
//	tuple_stdpfjets->farray("stdjet_pdgid",ID,rstdjets.begin(),rstdjets.end(),"NMaxjets",10000);
	tuple_stdpfjets->farray("stdjet_ndaughters",NDAUGHTERS,rstdjets.begin(),rstdjets.end(),"NMaxjets",10000);

	tuple_stdpfjets->farray("stdjet_mtf",mtf,rstdjets.begin(),rstdjets.end(),"NMaxjets",10000);
	tuple_stdpfjets->farray("stdjet_width",width,rstdjets.begin(),rstdjets.end(),"NMaxjets",10000);

	tuple_stdpfjets->farray("stdjet_ECAL_E",ECAL_E,rstdjets.begin(),rstdjets.end(),"NMaxjets",10000);
	tuple_stdpfjets->farray("stdjet_HCAL_E",HCAL_E,rstdjets.begin(),rstdjets.end(),"NMaxjets",10000);

	tuple_stdpfjets->farray("stdjet_cpf",cpf,rstdjets.begin(),rstdjets.end(),"NMaxjets",10000);
	tuple_stdpfjets->farray("stdjet_n90",n90,rstdjets.begin(),rstdjets.end(),"NMaxjets",10000);  
	tuple_stdpfjets->farray("stdjet_nPVInfo",nPVInfo,rstdjets.begin(),rstdjets.end(),"NMaxjets",10000);  
//	tuple_stdpfjets->farray("stdjet_ntrk",ntrk,rstdjets.begin(),rstdjets.end(),"NMaxjets",10000);  
//	tuple_stdpfjets->farray("stdjet_NsatCells",NsatCells,rstdjets.begin(),rstdjets.end(),"NMaxjets",10000);  
	tuple_stdpfjets->farray("stdjet_N_HasPVInfo",N_HasPVInfo,rstdjets.begin(),rstdjets.end(),"NMaxjets",10000);  
//	tuple_stdpfjets->farray("stdjet_JetCorrec",JetCorrec,rstdjets.begin(),rstdjets.end(),"NMaxjets",10000);  
	tuple_stdpfjets->farray("stdjet_N_PVs",N_PVs,rstdjets.begin(),rstdjets.end(),"NMaxjets",10000);  


  std::vector<float> stdloosemuon_bestpvz;
  std::vector<float> fatjet_bestpvz;
  std::vector<float> stdjet_bestpvz;
  std::vector<float> pv_ntrkbkw;
  std::vector<float> pv_ntrkfw;

  for (LHCb::Particle::Range::iterator ijet = stdloosemuons.begin() ; ijet != stdloosemuons.end() ; ++ijet) {
    const LHCb::Particle * jet = * ijet;
    const LHCb::VertexBase* bestPVV = getStoredBestPV(jet);
    stdloosemuon_bestpvz.push_back(bestPVV != 0 ? bestPVV->position().Z() : -10000.);
  }
  tuple_stdpfjets->farray("stdloosemuon_bestpvz",stdloosemuon_bestpvz,"NMaxMuons",10000);

  for (LHCb::Particle::Range::iterator ijet = fatjets.begin() ; ijet != fatjets.end() ; ++ijet) {
    const LHCb::Particle * jet = * ijet;
    const LHCb::VertexBase* bestPVV = getStoredBestPV(jet);
    fatjet_bestpvz.push_back(bestPVV->position().Z());
  }
  tuple_stdpfjets->farray("fatjet_bestpvz",fatjet_bestpvz,"NMaxFatjets",10000);

//  for (LHCb::RecVertex::Range::iterator irec = pvs.begin() ; irec != pvs.end() ; ++irec) {
//    const LHCb::VertexBase* rec = *irec;
//    float auxz = rec->position().Z();
    int ntrkfw=0, ntrkbkw=0;
    for (LHCb::Track::Range::iterator itrk = alltrks.begin() ; itrk != alltrks.end() ; ++itrk) {
      const LHCb::Track *trk = * itrk;
      if(!trk->hasVelo()) continue;
//      const LHCb::VertexBase* bestPVV = getStoredBestPV(trk);
//      if(TMath::Abs(auxz-bestPVV->position().Z())>0.01) continue;
      if(trk->momentum().Eta()<0) ntrkbkw++;
      else ntrkfw++;
//      trk_bestpvz.push_back(bestPVV->position().Z());
//      trk_eta.push_back(trk->momentum().Eta());
    }
    pv_ntrkfw.push_back(ntrkfw);
    pv_ntrkbkw.push_back(ntrkbkw);
//  }
  tuple_stdpfjets->farray("pv_ntrkfw",pv_ntrkfw,"NMaxPVs",10000);
  tuple_stdpfjets->farray("pv_ntrkbkw",pv_ntrkbkw,"NMaxPVs",10000);
//  tuple_stdpfjets->farray("trk_bestpvz",trk_bestpvz,"NMaxTrks",10000);
//   tuple_stdpfjets->farray("trk_eta",trk_eta,"NMaxTrks",10000);

  ITriggerTisTos * m_TriggerTisTosTool = tool<ITriggerTisTos>( "TriggerTisTos",this);
  std::vector<std::string> NamesInHlt = m_TriggerTisTosTool->triggerSelectionNames("Hlt2Topo.*Decision");
  //std::vector<std::string>::iterator nameit = NamesInHlt.begin();
  //for(; nameit!=NamesInHlt.end(); ++nameit) info() << *nameit << endreq;
  
  IAccept * m_doubletopo = tool<IAccept>("DoubleTopoTool",this);
  //if(!Gaudi::Utils::setProperty(m_doubletopo,"minAngle","2./57"))error() << "Problem setting minAngle(DoubleTopo)" << endreq ; 
  //if(!Gaudi::Utils::setProperty(m_doubletopo,"minMass","19000"))error() << "Problem setting minMass(DoubleTopo)" << endreq ; 


  IEventTupleTool * m_eventtupletool = tool<IEventTupleTool>("TupleToolEventInfo",this);
  m_eventtupletool->fill(tuple_stdpfjets);
  IEventTupleTool * m_eventtupletooltrigger = tool<IEventTupleTool>("TupleToolTrigger",this);
  if(!Gaudi::Utils::setProperty(m_eventtupletooltrigger,"Verbose","True"))error() << "Problem setting Verbose to true" << endreq ; 
  if(!Gaudi::Utils::setProperty(m_eventtupletooltrigger,"FillStripping","True"))error() << "Problem setting FillStripping to true" << endreq ; 
  if(!Gaudi::Utils::setProperty(m_eventtupletooltrigger,"VerboseHlt2","True"))error() << "Problem setting FillStripping to true" << endreq ; 
  Gaudi::Utils::setProperty(m_eventtupletooltrigger,"TriggerList","Hlt2SingleHighPTMuon_Decision");
  if(!m_eventtupletooltrigger->fill(tuple_stdpfjets))error() << "Problem filling the trigger and stripping event information" << endreq ;

	std::vector<float> topo_px,topo_py,topo_pz,topo_e;


	std::vector<const LHCb::HltObjectSummary*> hltObjs
		= m_TriggerTisTosTool->hltObjectSummaries("Hlt2Topo.*BBDTDecision",2,2);
	unsigned int num = hltObjs.size();
	std::vector<std::vector<int> > lhcbIDs(num);
	for(unsigned int i = 0; i < num; i++){
		if(lhcbIDs[i].size() == 0) getLHCbIDs(hltObjs[i],lhcbIDs[i]);
		/*bool good=true;
		for(unsigned int j = 0; j < i; j++){
			if(j==i) continue; 
			if(lhcbIDs[j].size() == 0) getLHCbIDs(hltObjs[j],lhcbIDs[j]);
			if(doLHCbIDsOverlap(lhcbIDs[i],lhcbIDs[j])) good=false;
		}
		if(!good) continue;*/
		Gaudi::LorentzVector p4;
		getHltObjP4(hltObjs[i],p4);
		topo_px.push_back(p4.Px());
		topo_py.push_back(p4.Py());
		topo_pz.push_back(p4.Pz());
		topo_e.push_back(p4.E());
	}


	tuple_stdpfjets->column("doubletopo_accept",m_doubletopo->accept());
	tuple_stdpfjets->farray("topo_px",topo_px,"NMaxTopos",10000);
	tuple_stdpfjets->farray("topo_py",topo_py,"NMaxTopos",10000);
	tuple_stdpfjets->farray("topo_pz",topo_pz,"NMaxTopos",10000);
	tuple_stdpfjets->farray("topo_e",topo_e,"NMaxTopos",10000);

	std::vector<int> Ngendaughters;
	std::vector<int> Ntopotriggers;
	std::vector<const HepMC::GenParticle *> jetgendaughters;
	std::vector<const LHCb::Particle *> jetdaughters;
	std::vector<unsigned int> jetcountergenjetsdaughters;

	std::vector<const HepMC::GenParticle *> jetqgmothers;
	std::vector<int> numberofqgmothers;

	std::vector<int> number_clusters;
	std::vector<int> stdjet_daughter_tracktype, trackchi2ofdaughters;
	std::vector<float> clusters_e;
	std::vector<float> stdjet_bestdaughterPVz_FromPF,stdjet_bestdaughterPVz, stdjet_bestdaughterPVIP, stdjet_daughterIPerr, stdjet_daughterIPz;

	// get a particle
	// get some primary vertices
	LHCb::RecVertex::Range m_vertices;
	//  const LHCb::RecVertex::Container* pVertices = ....;
	m_vertices = get<LHCb::RecVertex::Range>(LHCb::RecVertexLocation::Primary);
	// get the best PV for the particle. See the IRelatedPVFinder doxygen for full interface.
	IRelatedPVFinder* m_pvRelator ;
	m_pvRelator = tool<IRelatedPVFinder>("GenericParticle2PVRelator__p2PVWithIPChi2_OfflineDistanceCalculatorName_/P2PVWithIPChi2", this);

        const IDistanceCalculator* idist = distanceCalculator();
        Gaudi::XYZVector *impact = new Gaudi::XYZVector();

	for (LHCb::Particle::Range::iterator ijet = rstdjets.begin() ; ijet != rstdjets.end() ; ++ijet) {
		// Loop on the stddjets 
		const LHCb::Particle * jet = * ijet;

		const LHCb::VertexBase* bestPVV = getStoredBestPV(jet);
                m_TriggerTisTosTool->setOfflineInput(*jet);
                std::vector<std::string>::iterator nameit = NamesInHlt.begin();
                int ito=0;
                for(; nameit!=NamesInHlt.end(); ++nameit) {if(m_TriggerTisTosTool->tosSelection(*nameit)) ito++;
                if(m_TriggerTisTosTool->tusSelection(*nameit)) ito+=100;}
                Ntopotriggers.push_back(ito);

		stdjet_bestpvz.push_back(bestPVV->position().Z());

		std::vector<const LHCb::Particle *> daughtersvector = jet->daughtersVector();
		jetcounter++;

		int numberofgendaughters = 0;
		LorentzVector * genjet4vector = new LorentzVector(0,0,0,0);


		int nqgmothers = 0;

		for (std::vector<const LHCb::Particle *>::iterator idaughter = daughtersvector.begin() ; idaughter != daughtersvector.end() ; ++idaughter){
			//Loop on jet daughters (particle used to make the jet)
			const LHCb::Particle * daughter = *idaughter;
			const LHCb::VertexBase *bestDaughterPV1 = bestPV(daughter);
			const LHCb::VertexBase *bestDaughterPV = m_pvRelator->relatedPV(daughter,m_vertices);

			jetdaughters.push_back(daughter);
                        double ippu=-1,ippuerr=-1;

			// A lot o cheks before get the tracktype
			(daughter->proto() == NULL) ? 
				stdjet_daughter_tracktype.push_back(0): 
				(daughter->proto()->track() == NULL) ? 
				stdjet_daughter_tracktype.push_back(0) : stdjet_daughter_tracktype.push_back(daughter->proto()->track()->type()); 

			if(bestDaughterPV!=NULL){
				Fun bdaughterpvip = IP (bestDaughterPV, geo() )  ;
				Fun bdaughterpvipchi2 = IPCHI2 (bestDaughterPV, geo() )  ;
                                idist->distance(daughter,bestDaughterPV,*impact); 
				stdjet_bestdaughterPVz.push_back(bestDaughterPV->position().Z());
				stdjet_bestdaughterPVz_FromPF.push_back(bestDaughterPV->position().Z());
			        stdjet_bestdaughterPVIP.push_back(bdaughterpvip(daughter));
			        stdjet_daughterIPerr.push_back(bdaughterpvipchi2(daughter));
			        stdjet_daughterIPz.push_back(impact->Z());
			}
			else {
				stdjet_bestdaughterPVz.push_back(-9999.);
				stdjet_bestdaughterPVz_FromPF.push_back(-9999.);
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

	}



	// Fill the tuple with jet daughters variables
//	tuple_stdpfjets->farray("jetdaughters_trackchi2",trackchi2ofdaughters,"NMaxdaughters",10000);
//	tuple_stdpfjets->farray("jetdaughters_bestPVz",stdjet_bestdaughterPVz,"NMaxdaughters",10000);
//	tuple_stdpfjets->farray("jetdaughters_bestPVz_FromPF",stdjet_bestdaughterPVz_FromPF,"NMaxdaughters",10000);
//	tuple_stdpfjets->farray("jetdaughters_bestPVIP",stdjet_bestdaughterPVIP,"NMaxdaughters",10000);
//	tuple_stdpfjets->farray("jetdaughters_bestPVIPchi2",stdjet_daughterIPerr,"NMaxdaughters",10000);
//	tuple_stdpfjets->farray("jetdaughters_bestPVIPz",stdjet_daughterIPz,"NMaxdaughters",10000);
	//tuple_stdpfjets->farray("jetdaughters_tracktype",stdjet_daughter_tracktype,"NMaxdaughters",10000);

	//tuple_stdpfjets->farray("jetdaughters_px",PX,
			//"jetdaughters_py",PY,
			//"jetdaughters_pz",PZ,
			//"jetdaughters_e",E,jetdaughters.begin(),jetdaughters.end(),"NMaxdaughters",10000);

	//tuple_stdpfjets->farray("jetdaughters_pdgid",ID,
			//"jetdaughters_info",INFO4JET,jetdaughters.begin(),jetdaughters.end(),"NMaxdaughters",10000);
	// Best PV (z component)
	tuple_stdpfjets->farray("stdjet_bestpvz",stdjet_bestpvz,"NMaxjets",10000);

	tuple_stdpfjets->farray("stdjet_Ntopotriggers",Ntopotriggers,"NMaxjets",10000);

	tuple_stdpfjets->write();

	return StatusCode::SUCCESS ;

}
