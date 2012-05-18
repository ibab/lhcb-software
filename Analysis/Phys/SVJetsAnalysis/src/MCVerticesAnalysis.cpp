#include "MCVerticesFunctions.h"
//To associate a particle to the true one.
#include "Kernel/Particle2MCLinker.h"
#include "LoKi/LoKiAlgoMC.h"
#include "LoKi/CoreCuts.h"

#include "LoKi/MCParticleCuts.h"
#include "LoKi/PhysMCParticleCuts.h"

#include "Event/HltDecReports.h"

using namespace oscar ;
using namespace LoKi::Cuts ;
using namespace LoKi ;

LOKI_MCALGORITHM(MCVerticesAnalysis)
{

	//ITriggerTisTos* m_TriggerTisTosTool;
	//m_TriggerTisTosTool = tool<ITriggerTisTos>( "TriggerTisTos",this);
	//bool Hlt2Global_Dec = Hlt2GlobalFlag(m_TriggerTisTosTool);
	bool Hlt1Global_Dec = false;
	bool Hlt2Global_Dec = false;

	if( exist<LHCb::HltDecReports>( LHCb::HltDecReportsLocation::Default ) )
	{ 
		const LHCb::HltDecReports* decReports = 
			get<LHCb::HltDecReports>( LHCb::HltDecReportsLocation::Default );
		//Get the HLT1 global
		Hlt1Global_Dec = (decReports->decReport("Hlt1Global"))?(decReports->decReport("Hlt1Global")->decision()):0;
		//Get the HLT2 global
		Hlt2Global_Dec = (decReports->decReport("Hlt2Global"))?(decReports->decReport("Hlt2Global")->decision()):0;

	} 

	Tuple tuple_verticesanalysis = nTuple("VerticesAnalysis");
	Tuple tuple_mcdaughters = nTuple("MCDaughters");
	Tuple tuple_mcverticesanalysis = nTuple("MCVerticesAnalysis");
	Tuple tuple_mcdeltaphipermu = nTuple("MCDeltaPhiPerMu");
	Tuple tuple_deltaphipermu = nTuple("DeltaPhiPerMu");
	Tuple tuple_mcverticescharge2pidsanalysis = nTuple("MCVerticesCharge2PIDsAnalysis");
	Tuple tuple_eventreco = nTuple("EventReco");
	bool Event_HasMuon = false , Event_AtMaterial = false;  
	unsigned int Event_NbDisplRV = 0;
	float Event_InvariantMassMin = 140000000, Event_InvariantMassMax = 0;
	float Event_SumptMin = 140000000, Event_SumptMax=0;
	Fun bpvip = BPVIP ("")  ;

	Tuple tuple_cylindercut = nTuple("CylinderCut");
	Tuple tuple_cylindercutevents = nTuple("CylinderCutEvents");
	Tuple tuple_displrv = nTuple("DisplRV");

	bool Event_Rec5Trks = false;
	bool Event_Rec4Trks = false;
	bool Event_Rec3Trks = false;
	bool Event_Rec2Trks = false;
	bool Event_Rec1Trks = false;

	IParticle2MCAssociator* m_assoc;
	m_assoc = tool<IParticle2MCAssociator >("MCMatchObjP2MCRelator");
	Particle2MCLinker* m_pLinker;
	//Retrieve MC linker tools
	m_pLinker = new Particle2MCLinker(this,Particle2MCMethod::Links,std::vector<std::string>(1,"") );

	// get all SECONDARY  particles from desktop
	//LHCb::RecVertex::Range     secs  = get<LHCb::RecVertex::Range>("Rec/Vertices/DisplRV");
	//VRange secondaries = vselect ( "Secodaries" , secs.begin() , secs.end() , VALL ) ;
	//warning() << "Secondary vertices size " << secondaries.size() << endmsg; 
	//LHCb::Particle::Range     rangedisplrvparticles  = get<LHCb::Particle::Range>("/Event/Phys/SingleDVHighMass/Particles");
	//LHCb::Particle::Range     rangedisplrvparticlesstrip  = get<LHCb::Particle::Range>("/Event/Phys/SingleDVHighMass/Particles");
	LHCb::Particle::Range     rangedisplrvparticlesstrip  = select("rangedisplrvparticlesstrip",ALL);

	plot(rangedisplrvparticlesstrip.size(),"StripCandidatesPerEvent",0,15);
	plot(rangedisplrvparticlesstrip.size() > 0 ? 1:0,"StripEvent_Dec",0,2);
	//LHCb::Particle::Range     rangedisplrvparticles  = get<LHCb::Particle::Range>("/Event/Phys/PreselDisplVertices/Particles");
	LHCb::Particle::Range     rangedisplrvparticles  = select("rangedisplrvparticles",ABSID == 1000022);

	plot(rangedisplrvparticles.size(),"PreSelCandidatesPerEvent",0,50);
	plot(rangedisplrvparticles.size() > 0 ? 1:0,"PreSelEvent_Dec",0,2);

	Range displrvparticles = select ( "DisplRVParticles" , rangedisplrvparticles.begin() , rangedisplrvparticles.end() , ABSID == 1000022 ) ;

	plot(Hlt2Global_Dec,"Hlt2Global_DecPerEvent",0,2);
	plot(Hlt2Global_Dec * rangedisplrvparticlesstrip.size() > 0 ? 1:0,"StripAndTrigger_DecPerEvent",0,2);
	plot(Hlt2Global_Dec * rangedisplrvparticlesstrip.size(),"StripAndTriggerCandidatesPerEvent",0,5);

	//warning() << "Secondary vertices particles size " << displrvparticles.size() << endmsg; 
	//LHCb::RecVertex::Range preselverticesrange = get<LHCb::RecVertex::Range>("/Event/Phys/PreselDisplVertices/decayVertices");
	//VRange preseldisplvertices = vselect("PreSelDisplVertices",preselverticesrange.begin(),preselverticesrange.end(),VALL);
	std::vector<const LHCb::Vertex*> preseldisplvertices;
	std::vector<const LHCb::Particle*> recneutralinos;

	LHCb::Particle* m_BeamLine = new Particle();

	Particle::Range BL;      

	if(exist<Particle::Range>("/Event/BeamLine")){

		BL = get<Particle::Range>("/Event/BeamLine");      
		const LHCb::Particle* tmp = *(BL.begin());
		m_BeamLine->setReferencePoint(tmp->referencePoint());
		m_BeamLine->setMomentum( tmp->momentum() );

		//info() << "BL.size() " << BL.size() << endmsg;

	}

	if ( displrvparticles.size() > 0 ) {for (Range::iterator idisplvtxparticle = displrvparticles.begin() ; idisplvtxparticle != displrvparticles.end() ; ++idisplvtxparticle) {
		//info() << "Setting variable" << endmsg;
		const LHCb::Particle * displvtxparticle = *idisplvtxparticle;
		//info() << "Scan on particle displ pids " << displvtxparticle->particleID().abspid() << endmsg;
		if(displvtxparticle->particleID().abspid()!=1000022)continue;
		preseldisplvertices.push_back(displvtxparticle->endVertex());
		recneutralinos.push_back(displvtxparticle);
	}}

	const StatusCode OK = StatusCode::SUCCESS ;
	MCRange mcparticles = mcselect("MCParticles", MCALL);
	//Range allparticles = select("AllParticles",ALL);
	std::vector<const LHCb::Vertex*> vfrommc;

	VFun mindisttorecvpresel = MINVVD( preseldisplvertices ) ;
	//VFun mindisttorecv = MINVVD( secondaries ) ;

	for ( MCRange::const_iterator ip = mcparticles.begin() ; mcparticles.end() != ip ; ++ip ) {
		const LHCb::MCParticle* mcp = *ip;
		if(mcp->particleID().abspid() == 13 && mcp->mother())if(mcp->mother()->particleID().abspid() == 1000022){
			//mcverticesanalysis->column("MCChargedNbtracks",mcp->originVertex()->products()->size());
			const SmartRefVector<LHCb::MCParticle>& products = mcp->originVertex()->products();
			SmartRefVector<LHCb::MCParticle>::const_iterator imcdaughtersneut,jmcmudaughtersneut;
			unsigned int countercharged=0,countermuons=0,countercharge2=0;
			Gaudi::LorentzVector mcneutvertexmomentum(0,0,0,0);
			Gaudi::LorentzVector mcneutvertexonlychargedmomentum(0,0,0,0);
			double highestmomentum=0;
			LHCb::MCParticle highestmomentumparticle;
			std::vector<const LHCb::MCParticle*> mcchargeddaughtersinacceptance;
			float mcsumpt = 0;

			for (jmcmudaughtersneut =  products.begin() ; jmcmudaughtersneut != products.end() ; jmcmudaughtersneut++){
				//SmartRef<LHCb::MCParticle> smartrefmcmudaughtersneut = *jmcmudaughtersneut;
				const LHCb::MCParticle * mcmudaughtersneut = * jmcmudaughtersneut;//new LHCb::MCParticle;
				//mcdaughtersneut = smartrefmcmudaughtersneut;
				tuple_mcdaughters->column("PhiofMCDaughters",((*jmcmudaughtersneut)->momentum().Phi())) ;
				tuple_mcdaughters->write();
				mcsumpt+=mcmudaughtersneut->momentum().Pt();

				if((*jmcmudaughtersneut)->momentum().P() > highestmomentum){highestmomentum = (*jmcmudaughtersneut)->momentum().P();highestmomentumparticle = (LHCb::MCParticle)*jmcmudaughtersneut;}


				if(IsInAcceptance(mcmudaughtersneut) && mcmudaughtersneut->particleID().threeCharge() != 0)mcchargeddaughtersinacceptance.push_back(mcmudaughtersneut);

				if ((*jmcmudaughtersneut)->particleID().abspid() != 13) continue;
				Gaudi::LorentzVector mcneutvertexmomentumnotmu(0,0,0,0);
				double mcdeltaphimumax=0, mcdeltaphimumin=std::acos(-1), mcdeltaphimu4vec=-1, mcdeltaphi=-1;


				for (imcdaughtersneut =  products.begin() ; imcdaughtersneut != products.end() ; imcdaughtersneut++){
					if ((*imcdaughtersneut)->particleID().abspid() == 13) continue;
//					const LHCb::MCParticle * mcdaughtersnotmuneut = * imcdaughtersneut;//new LHCb::MCParticle;
					mcdeltaphi = (*imcdaughtersneut)->momentum().Phi() - (*jmcmudaughtersneut)->momentum().Phi();
					if(mcdeltaphi < 0)mcdeltaphi = mcdeltaphi + 2*std::acos(-1);
					if(mcdeltaphi > std::acos(-1))mcdeltaphi = mcdeltaphi - std::acos(-1);
					if(mcdeltaphimumax < mcdeltaphi) mcdeltaphimumax = mcdeltaphi;
					if(mcdeltaphimumin > mcdeltaphi) mcdeltaphimumin = mcdeltaphi;
					mcneutvertexmomentumnotmu+=(*imcdaughtersneut)->momentum();
				}
				tuple_mcdeltaphipermu->column("DeltaPhiMax",mcdeltaphimumax);
				tuple_mcdeltaphipermu->column("DeltaPhiMin",mcdeltaphimumin);

				if (mcneutvertexmomentumnotmu.P() > 0){
					//info() << "Phi4vec" << mcneutvertexmomentumnotmu.Phi() << endmsg;
					//info() << "PhiMu" <<  (*jmcmudaughtersneut)->momentum().Phi() << endmsg;
					mcdeltaphimu4vec = (*jmcmudaughtersneut)->momentum().Phi() - mcneutvertexmomentumnotmu.Phi();
					if(mcdeltaphimu4vec < 0)mcdeltaphimu4vec = mcdeltaphimu4vec + 2*std::acos(-1);
					if(mcdeltaphimu4vec > std::acos(-1))mcdeltaphimu4vec = mcdeltaphimu4vec - std::acos(-1);
				}

				tuple_mcdeltaphipermu->column("DeltaPhiBy4Vec",mcdeltaphimu4vec);
				tuple_mcdeltaphipermu->write();


			}

			for (imcdaughtersneut =  products.begin() ; imcdaughtersneut != products.end() ; imcdaughtersneut++){

				mcneutvertexmomentum+=(*imcdaughtersneut)->momentum();

				if((*imcdaughtersneut)->particleID().threeCharge()!=0)countercharged++;
				if(std::abs((*imcdaughtersneut)->particleID().threeCharge())>3)
					if(std::abs((*imcdaughtersneut)->particleID().threeCharge())>3){
						countercharge2++;
						tuple_mcverticescharge2pidsanalysis->column("Charge2AbsPIDs",(*imcdaughtersneut)->particleID().abspid());
						tuple_mcverticescharge2pidsanalysis->write();
					}
				if((*imcdaughtersneut)->particleID().abspid() == 13)countermuons++;
				if((*imcdaughtersneut)->particleID().threeCharge()!=0)mcneutvertexonlychargedmomentum+=(*imcdaughtersneut)->momentum();

			}

			tuple_mcverticesanalysis->column("MCNbtracks",mcp->originVertex()->products().size());
			tuple_mcverticesanalysis->column("MCChargedNbtracks",countercharged);
			tuple_mcverticesanalysis->column("MCNeutralNbtracks",mcp->originVertex()->products().size()-countercharged);
			tuple_mcverticesanalysis->column("MCChargeMoreThan2Nbtracks",countercharge2);
			tuple_mcverticesanalysis->column("MuonsperVertex",countermuons);
			tuple_mcverticesanalysis->column("HighestMomentumParticleID",highestmomentumparticle.particleID().abspid());
			tuple_mcverticesanalysis->column("VZ",mcp->originVertex()->position().Z());
			tuple_mcverticesanalysis->column("VY",mcp->originVertex()->position().Y());
			tuple_mcverticesanalysis->column("VX",mcp->originVertex()->position().X());
			tuple_mcverticesanalysis->column("FD",(mcp->originVertex()->position() - mcp->mother()->originVertex()->position()).R());
			tuple_mcverticesanalysis->column("SumPt",mcsumpt);

			/*for (std::vector<const LHCb::Particle>::const_iterator idaughterrec; idaughterrec != ;){
			  const LHCb::MCParticle* muassocMCP = m_assoc->relatedMCP(Muons(1), LHCb::MCParticleLocation::Default);
			  }*/

			bool IsVertexReconstructed = false;
			const unsigned int minnumberofmcpartsinrecvertex = 2; 
			unsigned int NbofRecVerticesFromMCVtx2 = 0,NbofRecVerticesFromMCVtx3 = 0;; 
			for (std::vector<const LHCb::Particle*>::const_iterator irecneut = recneutralinos.begin() ; irecneut!=recneutralinos.end() ; ++irecneut){
				std::vector<const LHCb::Particle*>matchedparticles;
				const LHCb::Vertex* vertex = (*irecneut)->endVertex();
				const LHCb::Particle* recneut = *irecneut;

				//if( (mcp->originVertex()->position() - vertex->position()).R() > 0.1 )continue;
				//NbofRecVerticesFromMCVtx++; 
				//info() << "vertex.size " << vertex->outgoingParticlesVector().size() << endmsg;
				//info() << "vertex.split? " << NbofRecVerticesFromMCVtx << endmsg;

				for (imcdaughtersneut =  products.begin() ; imcdaughtersneut != products.end() ; imcdaughtersneut++){
					for (SmartRefVector<LHCb::Particle>::const_iterator irecpartinvertex = recneut->daughters().begin() ; irecpartinvertex!=recneut->daughters().end() ; ++irecpartinvertex){
						const LHCb::Particle* recpartinvertex = *irecpartinvertex;
						if(recpartinvertex == NULL)continue;
						//if((*irecpartinvertex)->proto()->track() == NULL)continue;
						//info() << "recpartinvertex " << recpartinvertex << endmsg;
						//info() << "recpartinvertex " << recpartinvertex << "ParticleID "<< recpartinvertex->particleID().pid() << endmsg;
						//if(m_assoc->relatedMCP(recpartinvertex, LHCb::MCParticleLocation::Default) != NULL)if( (*imcdaughtersneut)->momentum() == (m_assoc->relatedMCP(recpartinvertex, LHCb::MCParticleLocation::Default))->momentum() ){info() << "Goal" << endmsg;matchedparticles.push_back(recpartinvertex);}
						//info() << "Part " << recpartinvertex << endmsg;
						//info() << "Part id " << recpartinvertex->particleID().abspid() << endmsg;
						//info() << "Part track " << recpartinvertex->proto()->track() << " " << NULL << endmsg;
						//info() << "Before Link P2MC " << endmsg;
						const LHCb::MCParticle *MCPart = m_pLinker->firstMCP( (recpartinvertex) );
						//info() << "After Link P2MC " << endmsg;
						//info() << "MCPart Associated " << MCPart << endmsg;
						//info() << "MCPart isValid " << MCPart->particleID().isValid() << endmsg;
						//info() << "MCPart Mother " << MCPart->mother() << endmsg;

						if(MCPart != NULL)if( (*imcdaughtersneut)->momentum() == MCPart->momentum() ){
							//info() << "Goal" << endmsg;
							matchedparticles.push_back(recpartinvertex);
						}
//						if(matchedparticles.size() == minnumberofmcpartsinrecvertex)break;
					}
//					if(matchedparticles.size() == minnumberofmcpartsinrecvertex)break;
				}
//				if(matchedparticles.size() == minnumberofmcpartsinrecvertex)IsVertexReconstructed = true;
//				if(IsVertexReconstructed)break;
				if(matchedparticles.size() >= 2)NbofRecVerticesFromMCVtx2++; 
				if(matchedparticles.size() >= 3)NbofRecVerticesFromMCVtx3++; 
				plot(matchedparticles.size(),"NbofRecTrueTracksRecVtx",0,10);
				if(matchedparticles.size() >= 2) plot((mcp->originVertex()->position() - vertex->position()).R(), "DistMCVtx2RecVtx2TrueTracksAtLeast",0,2);
				if(matchedparticles.size() >= 3) plot((mcp->originVertex()->position() - vertex->position()).R(), "DistMCVtx2RecVtx3TrueTracksAtLeast",0,2);
			}
			bool mcmuinlhcb = IsInAcceptance(mcp);
			tuple_mcverticesanalysis->column("NbofRecVerticesFromMCVtx2", NbofRecVerticesFromMCVtx2);
			tuple_mcverticesanalysis->column("NbofRecVerticesFromMCVtx3", NbofRecVerticesFromMCVtx3);
			//tuple_mcverticesanalysis->column("NbRecTrueTracks", displrvparticles.size() == 0 ? 0 : matchedparticles.size() );
			tuple_mcverticesanalysis->column("IsReconstructible", mcchargeddaughtersinacceptance.size() >= 3 && mcmuinlhcb ? 1:0 );
			tuple_mcverticesanalysis->column("IsReconstructedMaxDist01",displrvparticles.size() > 0 ? (mindisttorecvpresel((LHCb::Vertex*)mcp->originVertex()) < 0.1 ? 1:0 ): 0);
			//tuple_mcverticesanalysis->column("IsReconstructed",secondaries.size() > 0 ? (mindisttorecv((LHCb::Vertex*)mcp->originVertex()) < 0.1 ? 1:0 ): 0);
			tuple_mcverticesanalysis->column("InvariantMass",mcneutvertexmomentum.mass());
			tuple_mcverticesanalysis->column("Energy",mcneutvertexmomentum.energy());
			tuple_mcverticesanalysis->column("Pt",mcneutvertexmomentum.pt());
			tuple_mcverticesanalysis->column("P",mcneutvertexmomentum.P());
			tuple_mcverticesanalysis->column("OnlyChargedInvariantMass",mcneutvertexonlychargedmomentum.mass());
			tuple_mcverticesanalysis->column("OnlyChargedEnergy",mcneutvertexonlychargedmomentum.energy());
			tuple_mcverticesanalysis->column("OnlyChargedPt",mcneutvertexonlychargedmomentum.pt());
			tuple_mcverticesanalysis->column("OnlyChargedP",mcneutvertexonlychargedmomentum.P());
			tuple_mcverticesanalysis->column("Hlt2Global_Dec",Hlt2Global_Dec);
			tuple_mcverticesanalysis->write();
			vfrommc.push_back((LHCb::Vertex *) mcp->originVertex());

		}
	}

	/*for ( Range::iterator ip = allparticles.begin() ; allparticles.end() != ip ; ++ip ) 
	  {
	  const LHCb::Particle* p = *ip ;
	// get the secondaries vertices:
	vsecondaries.push_back(p->endVertex());
	info() << "Debug So far so good" << endmsg;
	plot2D(p->endVertex()->position().X(),p->endVertex()->position().Y(),"Transversal distance of SVs",-3,3,-3,3,120,120);
	info() << "Debug So far so good" << endmsg;
	}*/


	VFun mindisttovfrommc = MINVVD( vfrommc ) ;

	if ( !(rangedisplrvparticles.empty()) )
	{info() << "Goalllllllllllllllllllllllllllllllllllllllllllllllllllllllllllll" << endmsg;}
	//{ return Warning ( "No Displaced vertices are found!" , SUCCESS ) ; } // RETURN 
	//if ( displrvparticles.empty() )
	//{ return Warning ( "No Displaced vertices Particles are found! PreselDisplVertices is Empty!!" , SUCCESS ) ; } // RETURN 

	for (Range::iterator idisplvtxparticle = displrvparticles.begin() ; idisplvtxparticle != displrvparticles.end() ; ++idisplvtxparticle) {
		//info() << "Setting variable" << endmsg;
		const LHCb::Particle * displvtxparticle = *idisplvtxparticle;
		if(displvtxparticle->particleID().abspid()!=1000022)continue;
		//warning() << "how many children? " << displvtxparticle->daughtersVector().size() << endmsg;
		//warning() << "ID please? " << displvtxparticle->particleID().toString() << endmsg;
		//warning() << "Have u an endVertex? " << displvtxparticle->endVertex() << endmsg;

		const double vx = displvtxparticle->endVertex()->position().X();
		const double vy = displvtxparticle->endVertex()->position().Y();
		const double vz = displvtxparticle->endVertex()->position().Z();
		if(BL.size()>0){
			const double R_FromBeamLine = oscar::GetRFromBL(displvtxparticle->endVertex()->position(),m_BeamLine);
			//info() << "R false " << displvtxparticle->endVertex()->position().R() << " R corrected " << R_FromBeamLine << endmsg ;
			tuple_displrv->column("RFromBeamLine",R_FromBeamLine);
		};
		//warning() << "Position of vertex" << endmsg;
		tuple_displrv->column("VZ",vz);
		tuple_displrv->column("VY",vy);
		tuple_displrv->column("VX",vx);
		tuple_displrv->column("R",std::sqrt((vx*vx)+(vy*vy)));
		//warning() << "Position" << endmsg;
		tuple_displrv->column("CovMatrix11",displvtxparticle->posCovMatrix()(0,0));
		tuple_displrv->column("CovMatrix12",displvtxparticle->posCovMatrix()(0,1));
		tuple_displrv->column("CovMatrix13",displvtxparticle->posCovMatrix()(0,2));
		tuple_displrv->column("CovMatrix21",displvtxparticle->posCovMatrix()(1,0));
		tuple_displrv->column("CovMatrix22",displvtxparticle->posCovMatrix()(1,1));
		tuple_displrv->column("CovMatrix23",displvtxparticle->posCovMatrix()(1,2));
		tuple_displrv->column("CovMatrix31",displvtxparticle->posCovMatrix()(2,0));
		tuple_displrv->column("CovMatrix32",displvtxparticle->posCovMatrix()(2,1));
		tuple_displrv->column("CovMatrix33",displvtxparticle->posCovMatrix()(2,2));
		tuple_displrv->column("Nbtracks",displvtxparticle->daughtersVector().size());
		//warning() << "CovMatrix" << endmsg;
		//tuple_displrv->column("Chi2PerDof",base->chi2PerDoF());

		/*const SmartRefVector<LHCb::Track>& producttracks = vtx->tracks();
		  SmartRefVector<LHCb::Track>::const_iterator idaughtertrack;
		  Gaudi::XYZVector displrvmomentum(0,0,0);

		  for (idaughtertrack =  producttracks.begin() ; idaughtertrack != producttracks.end() ; idaughtertrack++){

		  displrvmomentum += (*idaughtertrack)->momentum();

		  }
		 */
		tuple_displrv->column("PX",displvtxparticle->momentum().X());
		tuple_displrv->column("PY",displvtxparticle->momentum().Y());
		tuple_displrv->column("PZ",displvtxparticle->momentum().Z());
		tuple_displrv->column("PT",displvtxparticle->momentum().Pt());
		tuple_displrv->column("P",displvtxparticle->momentum().P());
		tuple_displrv->column("InvariantMass",displvtxparticle->momentum().mass());
		tuple_displrv->column("IsTrueMCVertex",mindisttovfrommc(displvtxparticle->endVertex()) < 0.1 ? 1 : 0);

		const double displcovx = displvtxparticle->posCovMatrix()(0,0);
		const double displcovy = displvtxparticle->posCovMatrix()(1,1);
		const double displcovxy = displvtxparticle->posCovMatrix()(0,1);
		const double rerror = std::sqrt( std::pow(2*vx,2)*displcovx + std::pow(2*vy,2)*displcovy + 8*std::abs(vx*vy)*displcovxy );
		const double zerror = std::sqrt( displvtxparticle->posCovMatrix()(2,2));
		tuple_displrv->column("RError",rerror);
		tuple_displrv->column("ZError",zerror);
		float sumpt=0;
		double MuHighestPT=0;
		double ippvmuhighestpt = -10000 ;

		//const double r2 = (std::pow(vx,2)+std::pow(vy,2));
		plot2D(vx,vy,"RecDisplRV50mm",-50,50,-50,50);
		plot2D(vx,vy,"RecDisplRV3mm",-3,3,-3,3);
		plot2D(vx,vz,"RecDisplXZ300mm",-20,20,0,300);
		plot2D(vx,vz,"RecDisplXZ100mm",-20,20,0,10,400,1000);
		bool HasMuon = false;
		bool IsMuonMCMuon = false;
		unsigned int nbRecMCParts = 0;

		//info() << "fine1" << endmsg;
		for (SmartRefVector<LHCb::Particle>::const_iterator jmudaughtersneut = displvtxparticle->daughters().begin() ; jmudaughtersneut != displvtxparticle->daughters().end() ; ++jmudaughtersneut){
			const LHCb::Particle * mudaughtersneut = * jmudaughtersneut;
			if(mudaughtersneut == NULL)continue;
			sumpt+=mudaughtersneut->momentum().Pt();	
			//info() << mudaughtersneut << " PID estranho " << mudaughtersneut->particleID().abspid() << " " << mudaughtersneut->momentum().energy()<< endmsg;
			const LHCb::MCParticle *MCPartRec = m_pLinker->firstMCP( (mudaughtersneut) );
			if (MCPartRec != NULL) if (MCPartRec->mother()) if (MCPartRec->mother()->particleID().abspid() == 1000022)nbRecMCParts++;
			if (mudaughtersneut->particleID().abspid() != 13)continue;
			HasMuon = true;
			const LHCb::MCParticle *MCPartforMu = m_pLinker->firstMCP( (mudaughtersneut) );
			if (MCPartforMu != NULL) if (MCPartforMu->mother()) if (MCPartforMu->mother()->particleID().abspid() == 1000022 && MCPartforMu->particleID().abspid() == 13)IsMuonMCMuon = true;

			Gaudi::LorentzVector neutvertexmomentumnotmu(0,0,0,0);
			double deltaphimumax=0, deltaphimumin=std::acos(-1), deltaphimu4vec=-1, deltaphi=-1;

			if (mudaughtersneut->momentum().Pt() > MuHighestPT){
				ippvmuhighestpt = bpvip( mudaughtersneut ) ;
				MuHighestPT = mudaughtersneut->momentum().Pt();
			}
			//info() << "Start DeltaPhiMuMin" << endmsg;

			for (SmartRefVector<LHCb::Particle>::const_iterator idaughtersneutnotmu = displvtxparticle->daughters().begin() ; idaughtersneutnotmu != displvtxparticle->daughters().end() ; ++idaughtersneutnotmu){

				const LHCb::Particle * daughtersneutnotmu= * idaughtersneutnotmu;
				//info() << daughtersneutnotmu << " PID estranho " << daughtersneutnotmu->particleID().abspid() << " " << daughtersneutnotmu->momentum().energy()<< endmsg;

				if (daughtersneutnotmu == NULL)continue;
				if (daughtersneutnotmu->particleID().abspid() == 13 ) continue;

				deltaphi = daughtersneutnotmu->momentum().Phi() - mudaughtersneut->momentum().Phi();
				if(deltaphi < 0)deltaphi = deltaphi + 2*std::acos(-1);
				if(deltaphi > std::acos(-1))deltaphi = deltaphi - std::acos(-1);
				if(deltaphimumax < deltaphi) deltaphimumax = deltaphi;
				if(deltaphimumin > deltaphi) deltaphimumin = deltaphi;
				//info() << "DeltaPhiMin " << deltaphimumin << " PID not mu " << daughtersneutnotmu->particleID().abspid() << endmsg; 
				neutvertexmomentumnotmu+=daughtersneutnotmu->momentum();
			}
			tuple_deltaphipermu->column("DeltaPhiMax",deltaphimumax);
			if(deltaphimumin == 0)info() << "Problema com o deltaphimin esta sendo nuloooooooooooooooo" << endmsg;
			tuple_deltaphipermu->column("DeltaPhiMin",deltaphimumin);

			if (neutvertexmomentumnotmu.P() > 0){
				deltaphimu4vec = mudaughtersneut->momentum().Phi() - neutvertexmomentumnotmu.Phi();
				if(deltaphimu4vec < 0)deltaphimu4vec = deltaphimu4vec + 2*std::acos(-1);
				if(deltaphimu4vec > std::acos(-1))deltaphimu4vec = deltaphimu4vec - std::acos(-1);
			}

			tuple_deltaphipermu->column("DeltaPhiBy4Vec",deltaphimu4vec);
			tuple_deltaphipermu->write();


		}

		tuple_displrv->column("HasMuon",HasMuon);
		tuple_displrv->column("IsMuonMCMuon",IsMuonMCMuon);
		tuple_displrv->column("nbRecMCParts",nbRecMCParts);
		tuple_displrv->column("IsAssoc2MCVertex5Tracks", nbRecMCParts >= 5 && IsMuonMCMuon > 0 ? 1 : 0);
		if ( nbRecMCParts >= 5 && IsMuonMCMuon > 0 ? 1 : 0 )Event_Rec5Trks = true;
		tuple_displrv->column("IsAssoc2MCVertex4Tracks", nbRecMCParts >= 4 && IsMuonMCMuon > 0 ? 1 : 0);
		if ( nbRecMCParts >= 4 && IsMuonMCMuon > 0 ? 1 : 0 )Event_Rec4Trks = true;
		tuple_displrv->column("IsAssoc2MCVertex3Tracks", nbRecMCParts >= 3 && IsMuonMCMuon > 0 ? 1 : 0);
		if ( nbRecMCParts >= 3 && IsMuonMCMuon > 0 ? 1 : 0 )Event_Rec3Trks = true;
		tuple_displrv->column("IsAssoc2MCVertex2Tracks", nbRecMCParts >= 2 && IsMuonMCMuon > 0 ? 1 : 0);
		if ( nbRecMCParts >= 2 && IsMuonMCMuon > 0 ? 1 : 0 )Event_Rec2Trks = true;
		//tuple_displrv->column("IsAssoc2MCVertex1Tracks", nbRecMCParts >= 1 && IsMuonMCMuon > 0 ? 1 : 0);
		//if ( nbRecMCParts >= 1 && IsMuonMCMuon > 0 ? 1 : 0 )Event_Rec1Trks = true;
		tuple_displrv->column("MuHighestPT",MuHighestPT);
		tuple_displrv->column("BestIPPVMuHighestPT",ippvmuhighestpt);
		tuple_displrv->column("sumpt",sumpt);
		tuple_displrv->column("IsAtMaterial",IsAtMaterial(displvtxparticle));Event_AtMaterial|=IsAtMaterial(displvtxparticle);
		tuple_displrv->column("Hlt2Global_Dec",Hlt2Global_Dec);
		tuple_displrv->write();



	Event_HasMuon = HasMuon;  
	if (Event_InvariantMassMin > displvtxparticle->momentum().mass())Event_InvariantMassMin = displvtxparticle->momentum().mass();
	if (Event_InvariantMassMax < displvtxparticle->momentum().mass())Event_InvariantMassMax = displvtxparticle->momentum().mass();
	if (Event_SumptMin > sumpt) Event_SumptMin = sumpt;
	if (Event_SumptMax < sumpt) Event_SumptMax = sumpt; 

	}

	Event_NbDisplRV = recneutralinos.size();

	tuple_eventreco->column("Event_NbDisplRV",Event_NbDisplRV);
	tuple_eventreco->column("Event_SumptMin",Event_SumptMin);
	tuple_eventreco->column("Event_SumptMax",Event_SumptMax);
	tuple_eventreco->column("Event_InvariantMassMin",Event_InvariantMassMin);
	tuple_eventreco->column("Event_InvariantMassMax",Event_InvariantMassMax);
	tuple_eventreco->column("Event_AtMaterial",Event_AtMaterial);
	tuple_eventreco->write();

	/*
	   for ( VRange::iterator displvtx = secondaries.begin() ; secondaries.end() != displvtx ; ++displvtx ){ 
	   const LHCb::VertexBase *base = *displvtx;
	   const LHCb::RecVertex* vtx = (const LHCb::RecVertex*) base;

	   const double vx = vtx->position().X();
	   const double vy = vtx->position().Y();
	   const double vz = vtx->position().Z();
	   tuple_displrv->column("VZ",vz);
	   tuple_displrv->column("VY",vy);
	   tuple_displrv->column("VX",vx);
	   tuple_displrv->column("CovMatrix11",base->covMatrix()(0,0));
	   tuple_displrv->column("CovMatrix12",base->covMatrix()(0,1));
	   tuple_displrv->column("CovMatrix13",base->covMatrix()(0,2));
	   tuple_displrv->column("CovMatrix21",base->covMatrix()(1,0));
	   tuple_displrv->column("CovMatrix22",base->covMatrix()(1,1));
	   tuple_displrv->column("CovMatrix23",base->covMatrix()(1,2));
	   tuple_displrv->column("CovMatrix31",base->covMatrix()(2,0));
	   tuple_displrv->column("CovMatrix32",base->covMatrix()(2,1));
	   tuple_displrv->column("CovMatrix33",base->covMatrix()(2,2));
	   tuple_displrv->column("Nbtracks",vtx->tracks().size());
	   tuple_displrv->column("Chi2PerDof",base->chi2PerDoF());

	   const SmartRefVector<LHCb::Track>& producttracks = vtx->tracks();
	   SmartRefVector<LHCb::Track>::const_iterator idaughtertrack;
	   Gaudi::XYZVector displrvmomentum(0,0,0);

	   for (idaughtertrack =  producttracks.begin() ; idaughtertrack != producttracks.end() ; idaughtertrack++){

	   displrvmomentum += (*idaughtertrack)->momentum();

	   }

	   tuple_displrv->column("PX",displrvmomentum.X());
	   tuple_displrv->column("PY",displrvmomentum.Y());
	   tuple_displrv->column("PZ",displrvmomentum.Z());

	   const double displcovx = base->covMatrix()(0,0);
	   const double displcovy = base->covMatrix()(1,1);
	   const double displcovxy = base->covMatrix()(0,1);
	   const double rerror = std::sqrt( std::pow(2*vx,2)*displcovx + std::pow(2*vy,2)*displcovy + 8*std::abs(vx*vy)*displcovxy );
	   const double zerror = std::sqrt(base->covMatrix()(2,2));
	   tuple_displrv->column("RError",rerror);
	   tuple_displrv->column("ZError",zerror);
	   tuple_displrv->write();
	   const double r2 = (std::pow(vx,2)+std::pow(vy,2));
	   plot2D(vx,vy,"RecDisplRV50mm",-50,50,-50,50);
	   plot2D(vx,vy,"RecDisplRV3mm",-3,3,-3,3);
	   plot2D(vx,vz,"RecDisplXZ300mm",-20,20,0,300);
	   plot2D(vx,vz,"RecDisplXZ100mm",-20,20,0,10,400,1000);
	   }
	 */

	plot(Event_Rec2Trks * rangedisplrvparticlesstrip.size() > 0 ? 1:0,"StripCandidatesPerEventRec2Trks_Dec",0,2);
	plot(Event_Rec2Trks * rangedisplrvparticles.size() > 0 ? 1:0,"PreSelCandidatesPerEventRec2Trks_Dec",0,2);
	plot(Event_Rec2Trks * Hlt2Global_Dec,"Hlt2Global_DecPerEventRec2Trks_Dec",0,2);
	plot(Event_Rec2Trks * Hlt2Global_Dec * rangedisplrvparticlesstrip.size() > 0 ? 1:0,"StripAndTrigger_DecPerEventRec2Trks",0,2);

	plot(Event_Rec3Trks * rangedisplrvparticlesstrip.size() > 0 ? 1:0,"StripCandidatesPerEventRec3Trks_Dec",0,2);
	plot(Event_Rec3Trks * rangedisplrvparticles.size() > 0 ? 1:0,"PreSelCandidatesPerEventRec3Trks_Dec",0,2);
	plot(Event_Rec3Trks * Hlt2Global_Dec,"Hlt2Global_DecPerEventRec3Trks_Dec",0,2);
	plot(Event_Rec3Trks * Hlt2Global_Dec * rangedisplrvparticlesstrip.size() > 0 ? 1:0,"StripAndTrigger_DecPerEventRec3Trks",0,2);

	plot(Event_Rec4Trks * rangedisplrvparticlesstrip.size() > 0 ? 1:0,"StripCandidatesPerEventRec4Trks_Dec",0,2);
	plot(Event_Rec4Trks * rangedisplrvparticles.size() > 0 ? 1:0,"PreSelCandidatesPerEventRec4Trks_Dec",0,2);
	plot(Event_Rec4Trks * Hlt2Global_Dec,"Hlt2Global_DecPerEventRec4Trks_Dec",0,2);
	plot(Event_Rec4Trks * Hlt2Global_Dec * rangedisplrvparticlesstrip.size() > 0 ? 1:0,"StripAndTrigger_DecPerEventRec4Trks",0,2);

	plot(Event_Rec5Trks * rangedisplrvparticlesstrip.size() > 0 ? 1:0,"StripCandidatesPerEventRec5Trks_Dec",0,2);
	plot(Event_Rec5Trks * rangedisplrvparticles.size() > 0 ? 1:0,"PreSelCandidatesPerEventRec5Trks_Dec",0,2);
	plot(Event_Rec5Trks * Hlt2Global_Dec,"Hlt2Global_DecPerEventRec5Trks_Dec",0,2);
	plot(Event_Rec5Trks * Hlt2Global_Dec * rangedisplrvparticlesstrip.size() > 0 ? 1:0,"StripAndTrigger_DecPerEventRec5Trks",0,2);

	return StatusCode::SUCCESS ;
// ==========================================================================
} //                                                       end of the algorithm
// ============================================================================
// The END 
// ============================================================================
