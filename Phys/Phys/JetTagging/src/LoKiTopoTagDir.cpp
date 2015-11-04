
// $Id: LokiSVTag.cpp,v 1.1 2011-06-24 10:10:10 aphan Exp $

// ============================================================================
// Includes

// Local
#include "LoKiTopoTagDir.h"
#include "LoKi/Kinematics.h"

#include "LoKi/Geometry.h"
#include <sstream>

#include "Kernel/IDVAlgorithm.h"
#include <Kernel/GetIDVAlgorithm.h>


// LoKi
// #include "LoKi/ILoKiSvc.h"
// #include "LoKi/ParticleContextCuts.h"
// #include "LoKi/ParticleCuts.h"
// #include "LoKi/ParticleContextCuts.h"
// #include "LoKi/ParticleCuts.h"

// ============================================================================
// Declaration of the Tool Factory
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi,TopoTagDir)

// ============================================================================
// Standard Initialization
  StatusCode LoKi::TopoTagDir::initialize()
{
  if(msgLevel(MSG::VERBOSE))
    verbose() << "Initialising algorithm" << endmsg;

  StatusCode sc = GaudiTool::initialize();
  if(sc.isFailure())
    return sc;


  // svc<LoKi::ILoKiSvc>("LoKiSvc", true);

  m_distCalc = tool<IDistanceCalculator>("LoKi::DistanceCalculator",this);

  m_dva = Gaudi::Utils::getIDVAlgorithm ( contextSvc(), this ) ;
  if (0==m_dva) return Error("Couldn't get parent DVAlgorithm",
                             StatusCode::FAILURE);


  //  m_MCORR = LoKi::Cuts::CORRM();
  //  std::cout<<" "<<std::endl;
  //std::cout<<m_lifetimeFitterName<<std::endl;
  //std::cout<<m_tinputs<<std::endl;

  //std::cout<<" "<<std::endl;
  return StatusCode::SUCCESS;
}


// ============================================================================
// Standard Finalization
StatusCode LoKi::TopoTagDir::finalize()
{
  if(msgLevel(MSG::VERBOSE))
    verbose() << "Finalising algorithm" << endmsg;

  return GaudiTool::finalize();
}



// ===========================================================================
// Calculate Tag Weight
bool LoKi::TopoTagDir::calculateJetProperty
(const LHCb::Particle *jet,  std::map <std::string,double> &jetWeight)
{



  

  StatusCode sc = StatusCode::SUCCESS;






  std::pair<double,std::string> tmp;
  double maxTopoBDT = -1;
  double maxTopoR = -1;
  double maxTopoFD = -1;
  double maxTopoFDChi2 = -1;
  double maxTopoPtRel = -1;
  double maxTopoPt = -1;
  double maxTopoPtMiss = -1;
  double maxTopoM = -1;
  double maxTopoMCorr = -1;
  double maxTopoMCorrNew = -1;
  double maxTopoCosDira = -9;
  double maxTopoMaxDoca = -1;
  double maxTopoHeavyChi2PerDOF = -1;
  double maxTopoIpBPV = -1;
  double maxTopoChi2BPV = -1;
  double maxTopoSumPt = 0;
  double maxTopoAlpha = -1;
  double maxTopoP = -1;
  double maxTopoPtFractionInJet = -1;

  double maxTopoPTBDT = -1;
  double maxTopoPTR = -1;
  double maxTopoPTFD = -1;
  double maxTopoPTFDChi2 = -1;
  double maxTopoPTPtRel = -1;
  double maxTopoPTPt = -1;
  double maxTopoPTPtMiss = -1;
  double maxTopoPTM = -1;
  double maxTopoPTMCorr = -1;
  double maxTopoPTMCorrNew = -1;
  double maxTopoPTCosDira = -9;
  double maxTopoPTMaxDoca = -1;
  double maxTopoPTHeavyChi2PerDOF = -1;
  double maxTopoPTIpBPV = -1;
  double maxTopoPTChi2BPV = -1;
  double maxTopoPTSumPt = 0;
  double maxTopoPTAlpha = -1;
  double maxTopoPTP = -1;
  double maxTopoPTPtFractionInJet = -1;


  double sumTopo = 0;
  double multTopo = 0;





  jetWeight.clear();

  LHCb::Particle::Range topos;

  if(jet != NULL){
  // Set member variables
  m_Jet = jet;
  // m_JetInputs = jet->daughtersVector();

 
  bool locseed = exist<LHCb::Particle::Range>( m_tinputs );
  if(!locseed){
    debug()<< "Unable to get location at '"
	   << m_tinputs  << "'" << endmsg;
  }else if(locseed){
    topos = get<LHCb::Particle::Range>(  m_tinputs );
    if( !topos ){
      debug()<< "Unable to get particles at '"
	     << m_tinputs  << "'" << endmsg;
    }
  }
  



  std::vector<const LHCb::Particle*>  jetDaus = extractDaugthers(jet);
  




  LHCb::Particle::Range::iterator itopo;

  if(locseed){

//       std::cout<<"topo size" << topos.size() << std::endl;

    //    const LHCb::VertexBase* JetOriVtx = m_dva->bestVertex ( jet );

    for(itopo = topos.begin(); itopo!=topos.end(); itopo++){
      

      //      std::cout<<"helllllo cedric2"<<std::endl;





      LHCb::Track::ConstVector tracks;
      LHCb::Particle::ConstVector parts;
      LHCb::Particle::ConstVector::iterator iparts;
      LHCb::Track::ConstVector::iterator itrack;
      const LHCb::Particle* mypartS = *itopo;
      const LHCb::VertexBase* evtx = mypartS->endVertex();
      const LHCb::VertexBase* oriVtx = m_dva->bestVertex ( mypartS );


      if((jet->endVertex()->position() - oriVtx->position()).Mag2() > 1e-4) continue;


      Gaudi::XYZVector A = jet->momentum().Vect();
      Gaudi::XYZVector B = evtx->position() - oriVtx->position();


      // LoKi::Cuts::DR2 deltaRFunction = Cuts::DR2(jet->momentum());

      const double PI=3.14159265;
      double Dphi,phi1,phi2,e1,e2;
      phi1 = A.phi();
      phi2 = B.phi();
      e1 = A.eta();
      e2 = B.eta();
      Dphi = std::fabs(phi1-phi2);
      if (Dphi>PI) { Dphi = 2*PI - Dphi; };
      double distance = std::sqrt( Dphi*Dphi + (e1-e2)*(e1-e2) );
      

      // std::cout<<"topo2: "<<distance << " "<<maxTopoPTCosDira<<" "<< maxTopoBDT<<std::endl;


      //  double distance = std::sqrt(deltaRFunction((mypartS)));
      if (distance < 0.5){
	



	std::vector<const LHCb::Particle*>  topoDaus = extractDaugthers(mypartS);
	
	double EnergyFractionDenom   = 0;
	double EnergyFraction = 0;
	
	for(int i_topo = 0; i_topo < (int) topoDaus.size();  i_topo++){
	  
	  if (topoDaus[i_topo]->proto() == NULL ) continue;
	  const LHCb::Track* topoTrack = topoDaus[i_topo]->proto()->track();
	  if (topoTrack == NULL ) continue;
	  
	  EnergyFractionDenom+=topoTrack->pt();

	  for(int i_jet = 0; i_jet < (int) jetDaus.size();  i_jet++){
	    
	    
	    if (jetDaus[i_jet]->proto() == NULL ) continue;
	    const    LHCb::Track* jetTrack = jetDaus[i_jet]->proto()->track();
	    if (jetTrack == NULL ) continue;
	    
	    if(topoTrack->key() == jetTrack->key()){
	      EnergyFraction+=topoTrack->pt();
	      break;
	    }

	  }
	}
	EnergyFraction /= EnergyFractionDenom;

	Gaudi::XYZVector B2 =  mypartS->momentum().Vect();
	



	// copying code from Perp2 from ROOT
	//
	// Missing Pt of the topo with respect to its flight direction
	double tot2 = B.Mag2();
	double ss2 = B2.Dot(B);
	double per2 = B2.Mag2();
	double dira = ss2/(std::sqrt(per2)*std::sqrt(tot2));
	if(tot2 > 0.0) per2 -= ss2*ss2/tot2;
	if(per2 < 0)   per2 = 0;
	


	double defVal = -1;
	double topoVal = mypartS->info(9999,defVal);






	if(EnergyFraction >= maxTopoPTPtFractionInJet){
	if(mypartS->pt() > maxTopoPTPt){

	  double  SumPt = 0;
	  //max doca:
	  LHCb::Particle::ConstVector daughters = mypartS->daughtersVector();
	  int size = daughters.size();
	  double maxDoca = 0;
	  for(int i = 0; i < size; i++){
	    SumPt += daughters[i]->pt();
	    if(daughters[i]->particleID().pid() == 22) continue;
	    for(int j = i+1; j < size; j++){
	      if(daughters[j]->particleID().pid() == 22) continue;
	      double doca;
	      m_distCalc->distance(daughters[i],daughters[j],doca);
	      if(doca > maxDoca) maxDoca = doca;
	    }
	  }

	  
	  
	  double chi2 = 0; int dof = 0;
	  double chipdof = -1;
	  heavyVChi2Dof(mypartS,chi2,dof);
	  if(dof > 0) chipdof= chi2/(double)dof;


	  double ipVtx=0, chi2Vtx=0;
	  m_distCalc->distance(mypartS, oriVtx, ipVtx, chi2Vtx);




	  // flight distance
	  double Fdist = 0;
	  double FDChi2 = 0 ;
	  m_distCalc->distance( oriVtx,evtx, Fdist, FDChi2 );


	  //   copying code from Perp2 from ROOT
	  //
	  //Relative Pt of the topo cand with respect to the jet
	  double tot = A.Mag2();
	  double ss = B2.Dot(A);
	  double per = B2.Mag2();
	  if(tot > 0.0) per -= ss*ss/tot;
	  if(per < 0)   per = 0;
	
	  double ptRel = std::sqrt(per);



	  double topoPtMiss = std::sqrt(per2);


	  //Corrected Mass
	  double topoM = mypartS->momentum().M();
	  double topoMCorr = std::sqrt((topoM*topoM)+(topoPtMiss*topoPtMiss)) + topoPtMiss;
	  double topoP =  mypartS->momentum().P();
	  double mcorr2    = std::sqrt((topoM*topoM) + 2*topoPtMiss*((std::sqrt(topoM*topoM+topoP*topoP))+topoPtMiss));
	  
	  maxTopoPTBDT = topoVal;
	  maxTopoPTPtRel = ptRel;
	  maxTopoPTR = distance;
	  maxTopoPTFD = Fdist;
	  maxTopoPTFDChi2 = FDChi2;
	  maxTopoPTPt = mypartS->pt();
	  maxTopoPTM = topoM;
	  maxTopoPTPtMiss = topoPtMiss;
	  maxTopoPTMCorr = topoMCorr;
	  maxTopoPTMCorrNew = mcorr2;
	  maxTopoPTCosDira = dira;
	  maxTopoPTMaxDoca = maxDoca;
	  maxTopoPTHeavyChi2PerDOF = chipdof;
	  maxTopoPTIpBPV = ipVtx;
	  maxTopoPTChi2BPV = chi2Vtx;
	  maxTopoPTSumPt = SumPt;
	  maxTopoPTAlpha = B.theta();
	  maxTopoPTP = mypartS->momentum().P();
	  maxTopoPTPtFractionInJet= EnergyFraction;

	}
	}



	if(EnergyFraction >= maxTopoPtFractionInJet){

	if(topoVal > maxTopoBDT){

	  double  SumPt = 0;
	  //max doca:
	  LHCb::Particle::ConstVector daughters = mypartS->daughtersVector();
	  int size = daughters.size();
	  double maxDoca = 0;
	  for(int i = 0; i < size; i++){
	    SumPt += daughters[i]->pt();
	    if(daughters[i]->particleID().pid() == 22) continue;
	    for(int j = i+1; j < size; j++){
	      if(daughters[j]->particleID().pid() == 22) continue;
	      double doca;
	      m_distCalc->distance(daughters[i],daughters[j],doca);
	      if(doca > maxDoca) maxDoca = doca;
	    }
	  }

	  
	  
	  double chi2 = 0; int dof = 0;
	  double chipdof = -1;
	  heavyVChi2Dof(mypartS,chi2,dof);
	  if(dof > 0) chipdof= chi2/(double)dof;


	  double ipVtx=0, chi2Vtx=0;
	  m_distCalc->distance(mypartS, oriVtx, ipVtx, chi2Vtx);




	  // flight distance
	  double Fdist = 0;
	  double FDChi2 = 0 ;
	  m_distCalc->distance( oriVtx,evtx, Fdist, FDChi2 );


	  // copying code from Perp2 from ROOT
	  //
	  // Relative Pt of the topo cand with respect to the jet
	  double tot = A.Mag2();
	  double ss = B2.Dot(A);
	  double per = B2.Mag2();
	  if(tot > 0.0) per -= ss*ss/tot;
	  if(per < 0)   per = 0;
	
	  double ptRel = std::sqrt(per);
	  double topoPtMiss = std::sqrt(per2);
	

	  //Corrected Mass
	  double topoM = mypartS->momentum().M();
	  double topoMCorr = std::sqrt((topoM*topoM)+(topoPtMiss*topoPtMiss)) + topoPtMiss;	  
	  double topoP =  mypartS->momentum().P();
	  double mcorr2    = std::sqrt((topoM*topoM) + 2*topoPtMiss*(std::sqrt(topoM*topoM+topoP*topoP)+topoPtMiss));
	  
	  maxTopoBDT = topoVal;
	  maxTopoPtRel = ptRel;
	  maxTopoR = distance;
	  maxTopoFD = Fdist;
	  maxTopoFDChi2 = FDChi2;
	  maxTopoPt = mypartS->pt();
	  maxTopoM = topoM;
	  maxTopoPtMiss = topoPtMiss;
	  maxTopoMCorr = topoMCorr;
	  maxTopoMCorrNew = mcorr2;
	  maxTopoCosDira = dira;
	  maxTopoMaxDoca = maxDoca;
	  maxTopoHeavyChi2PerDOF = chipdof;
	  maxTopoIpBPV = ipVtx;
	  maxTopoChi2BPV = chi2Vtx;
	  maxTopoSumPt = SumPt;
	  maxTopoAlpha = B.theta();
	  maxTopoP = mypartS->momentum().P();
	  maxTopoPtFractionInJet= EnergyFraction;
	}
	}

	sumTopo+=topoVal;
        multTopo+=1;

      }
    }
  } 
  }

  jetWeight["sumTopo"]=sumTopo;
  jetWeight["multTopo"]=multTopo;

  jetWeight["maxTopoPTBDT"]=maxTopoPTBDT;
  jetWeight["maxTopoPTPtRel"]=maxTopoPTPtRel;
  jetWeight["maxTopoPTR"]=maxTopoPTR;
  jetWeight["maxTopoPTPt"]=maxTopoPTPt;
  jetWeight["maxTopoPTPtMiss"]=maxTopoPTPtMiss;
  jetWeight["maxTopoPTM"]=maxTopoPTM;
  jetWeight["maxTopoPTMCorr"]=maxTopoPTMCorr;
  jetWeight["maxTopoPTMCorrNew"]=maxTopoPTMCorrNew;
  jetWeight["maxTopoPTCosDira"]=maxTopoPTCosDira;
  jetWeight["maxTopoPTFD"]=maxTopoPTFD;
  jetWeight["maxTopoPTFDChi2"]=maxTopoPTFDChi2;
  jetWeight["maxTopoPTMaxDoca"]=maxTopoPTMaxDoca;
  jetWeight["maxTopoPTHeavyChi2PerDOF"]=maxTopoPTHeavyChi2PerDOF;
  jetWeight["maxTopoPTIpBPV"]=maxTopoPTIpBPV;
  jetWeight["maxTopoPTChi2BPV"]=maxTopoPTChi2BPV;
  jetWeight["maxTopoPTSumPt"]=maxTopoPTSumPt;
  jetWeight["maxTopoPTAlpha"]=maxTopoPTAlpha;
  jetWeight["maxTopoPTP"]=maxTopoPTP;
  jetWeight["maxTopoPTPtFractionInJet"]=maxTopoPTPtFractionInJet;

  jetWeight["maxTopoBDT"]=maxTopoBDT;
  jetWeight["maxTopoPtRel"]=maxTopoPtRel;
  jetWeight["maxTopoR"]=maxTopoR;
  jetWeight["maxTopoPt"]=maxTopoPt;
  jetWeight["maxTopoPtMiss"]=maxTopoPtMiss;
  jetWeight["maxTopoM"]=maxTopoM;
  jetWeight["maxTopoMCorr"]=maxTopoMCorr;
  jetWeight["maxTopoMCorrNew"]=maxTopoMCorrNew;
  jetWeight["maxTopoCosDira"]=maxTopoCosDira;
  jetWeight["maxTopoFD"]=maxTopoFD;
  jetWeight["maxTopoFDChi2"]=maxTopoFDChi2;
  jetWeight["maxTopoMaxDoca"]=maxTopoMaxDoca;
  jetWeight["maxTopoHeavyChi2PerDOF"]=maxTopoHeavyChi2PerDOF;
  jetWeight["maxTopoIpBPV"]=maxTopoIpBPV;
  jetWeight["maxTopoChi2BPV"]=maxTopoChi2BPV;
  jetWeight["maxTopoSumPt"]=maxTopoSumPt;
  jetWeight["maxTopoAlpha"]=maxTopoAlpha;
  jetWeight["maxTopoP"]=maxTopoP;
  jetWeight["maxTopoPtFractionInJet"]=maxTopoPtFractionInJet;


  double tag_global = 0;
  if (maxTopoBDT > 0) 
    tag_global = 1;
  if (!topos)
    tag_global = -1;
  
  //IT HAS TO BEHTE LAST ONE: GLOBAL DECISTION
  jetWeight["Tag"] = tag_global;
  
  
  jetWeight["extraInfo"] = 5100;
  if(msgLevel(MSG::DEBUG)) debug() << "Tag weight: " << jetWeight << endmsg;
  

  if(tag_global>0.1)
    return true;
  else
    return false;
}


// ============================================================================
void  LoKi::TopoTagDir::heavyVChi2Dof(const LHCb::Particle *p, double &chi2, int &dof){
  LHCb::Particle::ConstVector daughters = p->daughtersVector();
  int size = daughters.size();
  if(!p->particleID().hasCharm() && !p->particleID().hasBottom()) return;
  if(size == 0 || p->endVertex() == 0) return;
  chi2 += p->endVertex()->chi2();
  dof += p->endVertex()->nDoF();
  for(int i = 0; i < size; i++) heavyVChi2Dof(daughters[i],chi2,dof);
}


// ============================================================================
std::vector<const LHCb::Particle*> LoKi::TopoTagDir::extractDaugthers(const LHCb::Particle *p){
  std::vector<const LHCb::Particle*> tmp;
  LHCb::Particle::ConstVector daughters = p->daughtersVector();
  int size = daughters.size();
  if(size == 0 || p->endVertex() == 0) return tmp;
  for(int i = 0; i < size; i++){
    std::vector<const LHCb::Particle*> tmp2 = extractDaugthers(daughters[i]);
    if(tmp2.size() == 0){
      tmp.push_back(daughters[i]);
    }else{
      for(int j = 0; j < (int) tmp2.size(); j++){
	tmp.push_back(tmp2[j]);
      }
    }
  }
  
  return tmp;
}
