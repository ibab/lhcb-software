
// ============================================================================
// LoKi
// ============================================================================
//#include "LoKi/LoKi.h"
#include "MCVerticesFunctions.h"
// ============================================================
// JetMaker.h
// ============================================================
//#include "LoKiFastJetMaker.h"
//#include <Kernel/IJetMaker.h>
// ===========================================================================

/// avoid long names:
  using namespace LoKi        ;
//  using namespace LoKi::Types ;
  using namespace LoKi::Cuts  ;
  using namespace LoKi::Particles  ;
//  using namespace LoKi::Vertices  ;
//  using namespace LHCb;
  using namespace oscar;



LOKI_ALGORITHM(VerticesAnalysis) 
{
  ITriggerTisTos* m_TriggerTisTosTool;
  m_TriggerTisTosTool = tool<ITriggerTisTos>( "TriggerTisTos",this);
  bool Hlt2Global_Dec = Hlt2GlobalFlag(m_TriggerTisTosTool);
  
  Tuple tuple_verticesanalysis = nTuple("VerticesAnalysis");
  Tuple tuple_deltaphipermu = nTuple("DeltaPhiPerMu");
  Tuple tuple_displrv = nTuple("DisplRV");
 
  Tuple tuple_eventreco = nTuple("EventReco");
  bool Event_HasMuon = false , Event_AtMaterial = false;  
  unsigned int Event_NbDisplRV = 0;
  float Event_InvariantMassMin = 140000000, Event_InvariantMassMax = 0;
  float Event_SumptMin = 140000000, Event_SumptMax=0;

  Fun bpvip = BPVIP ("")  ;

  const StatusCode OK = StatusCode::SUCCESS ;
  //Range allparticles = select("AllParticles",ALL);
  std::vector<const LHCb::Vertex*> vsecondaries;

  /*for ( Range::iterator ip = allparticles.begin() ; allparticles.end() != ip ; ++ip ) 
  {
	  const LHCb::Particle* p = *ip ;
	  // get the secondaries vertices:
	  vsecondaries.push_back(p->endVertex());
	  info() << "Debug So far so good" << endmsg;
	  plot2D(p->endVertex()->position().X(),p->endVertex()->position().Y(),"Transversal distance of SVs",-3,3,-3,3,120,120);
	  info() << "Debug So far so good" << endmsg;
  }*/

  // get all SECONDARY  particles from desktop
  //LHCb::RecVertex::Range     secs  = get<LHCb::RecVertex::Range>("Rec/Vertices/DisplRV");
  //VRange secondaries = vselect ( "Secodaries" , secs.begin() , secs.end() , VALL ) ;
  //warning() << "Secondary vertices size " << secondaries.size() << endmsg; 
  //LHCb::Particle::Range    rangedisplrvparticles  = get<LHCb::Particle::Range>("/Event/EW/Phys/SingleHighMassDisplVertices/Particles");
  LHCb::Particle::Range    rangedisplrvparticles  = select("rangedisplrvparticles",ALL);
  //LHCb::Particle::Range     rangedisplrvparticles  = get<LHCb::Particle::Range>("/Event/Phys/PreselDisplVertices/Particles");
  plot(rangedisplrvparticles.size() > 0 ? 1:0,"PreSelDisplRVSize",0,2);
  //LHCb::Particle::Range     rangedisplrvparticlesstrip  = get<LHCb::Particle::Range>("/Event/Phys/SingleDVHighMass/Particles");
  //LHCb::Particle::Range    rangedisplrvparticlesstrip  = get<LHCb::Particle::Range>("/Event/EW/Phys/SingleHighMassDisplVertices/Particles");
  LHCb::Particle::Range    rangedisplrvparticlesstrip  = select("rangedisplrvparticlesstrip",ALL);

  plot(rangedisplrvparticlesstrip.size() > 0 ? 1:0,"StripCandidatesPerEvent",0,2);
  plot(Hlt2Global_Dec,"Hlt2Global_DecPerEvent",0,2);
  plot(Hlt2Global_Dec * rangedisplrvparticlesstrip.size() > 0 ? 1:0,"StripAndTrigger_DecPerEvent",0,2);

  LHCb::Particle* m_BeamLine = new Particle();

  Particle::Range BL;      

  if(exist<Particle::Range>("/Event/BeamLine")){

	  BL = get<Particle::Range>("/Event/BeamLine");      
	  const LHCb::Particle* tmp = *(BL.begin());
	  m_BeamLine->setReferencePoint(tmp->referencePoint());
	  m_BeamLine->setMomentum( tmp->momentum() );

	  //info() << "BL.size() " << BL.size() << endmsg;

  }
  //else{info() << "Couldn't get the BeamLine info" << endmsg;}

  //if(BL.size() != 0)info() << "Fooooooooooooo works" << endmsg;

  //LHCb::Particle::Range     rangedisplrvparticles  = get<LHCb::Particle::Range>("/Event/Phys/PreselDisplVertices/Particles");
  Range displrvparticles = select ( "DisplRVParticles" , rangedisplrvparticles.begin() , rangedisplrvparticles.end() , ALL ) ;
  //warning() << "Secondary vertices particles size " << displrvparticles.size() << endmsg; 

  if ( rangedisplrvparticles.empty() )
  { return Warning ( "No Displaced vertices are found!" , SUCCESS ) ; } // RETURN 
  if ( displrvparticles.empty() )
  { return Warning ( "No Displaced vertices Particles are found! PreselDisplVertices is Empty!!" , SUCCESS ) ; } // RETURN 
  
  unsigned int counterreconeutralinos = 0;
  for (Range::iterator idisplvtxparticle = displrvparticles.begin() ; idisplvtxparticle != displrvparticles.end() ; ++idisplvtxparticle)
  {
	  //info() << "Setting variable" << endmsg;
	  const LHCb::Particle * displvtxparticle = *idisplvtxparticle;
          if(displvtxparticle->particleID().abspid()!=1000022)continue;
	  counterreconeutralinos++; 
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

	  const double displcovx = displvtxparticle->posCovMatrix()(0,0);
	  const double displcovy = displvtxparticle->posCovMatrix()(1,1);
	  const double displcovxy = displvtxparticle->posCovMatrix()(0,1);
	  const double rerror = std::sqrt( std::pow(2*vx,2)*displcovx + std::pow(2*vy,2)*displcovy + 8*std::abs(vx*vy)*displcovxy );
	  const double zerror = std::sqrt( displvtxparticle->posCovMatrix()(2,2));
	  tuple_displrv->column("RError",rerror);
	  tuple_displrv->column("ZError",zerror);
	  //const double r2 = (std::pow(vx,2)+std::pow(vy,2));
	  plot2D(vx,vy,"RecDisplRV50mm",-50,50,-50,50);
	  plot2D(vx,vy,"RecDisplRV3mm",-3,3,-3,3);
	  plot2D(vx,vz,"RecDisplXZ300mm",-20,20,0,300);
	  plot2D(vx,vz,"RecDisplXZ100mm",-20,20,0,10,400,1000);
	  bool HasMuon = false;
	  float sumpt=0;
	  double MuHighestPT=0;
	  double ippvmuhighestpt = -10000 ;


	  for (SmartRefVector<LHCb::Particle>::const_iterator jmudaughtersneut = displvtxparticle->daughters().begin() ; jmudaughtersneut != displvtxparticle->daughters().end() ; ++jmudaughtersneut){
		  const LHCb::Particle * mudaughtersneut = * jmudaughtersneut;
		  if(mudaughtersneut == NULL)continue;
		  //info() << mudaughtersneut << " PID estranho " << mudaughtersneut->particleID().abspid() << " " << mudaughtersneut->momentum().energy()<< endmsg;
		  sumpt+=mudaughtersneut->momentum().Pt();	
		  if (mudaughtersneut->particleID().abspid() != 13)continue;

		  HasMuon = true;

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
	  tuple_displrv->column("MuHighestPT",MuHighestPT);
	  tuple_displrv->column("BestIPPVMuHighestPT",ippvmuhighestpt);
	  tuple_displrv->column("IsAtMaterial",IsAtMaterial(displvtxparticle));
	  tuple_displrv->column("Hlt2Global_Dec",Hlt2Global_Dec);
	  tuple_displrv->column("sumpt",sumpt);
	  tuple_displrv->write();

	Event_HasMuon = HasMuon;  
	if (Event_InvariantMassMin > displvtxparticle->momentum().mass())Event_InvariantMassMin = displvtxparticle->momentum().mass();
	if (Event_InvariantMassMax < displvtxparticle->momentum().mass())Event_InvariantMassMax = displvtxparticle->momentum().mass();
	if (Event_SumptMin > sumpt) Event_SumptMin = sumpt;
	if (Event_SumptMax < sumpt) Event_SumptMax = sumpt; 

  }
	Event_NbDisplRV = counterreconeutralinos;

	tuple_eventreco->column("Event_NbDisplRV",Event_NbDisplRV);
	tuple_eventreco->column("Event_SumptMin",Event_SumptMin);
	tuple_eventreco->column("Event_SumptMax",Event_SumptMax);
	tuple_eventreco->column("Event_InvariantMassMin",Event_InvariantMassMin);
	tuple_eventreco->column("Event_InvariantMassMax",Event_InvariantMassMax);
	tuple_eventreco->column("Event_AtMaterial",Event_AtMaterial);
	tuple_eventreco->write();

  return StatusCode::SUCCESS ;
  // ==========================================================================
} //                                                       end of the algorithm
// ============================================================================
// The END 
// ============================================================================
