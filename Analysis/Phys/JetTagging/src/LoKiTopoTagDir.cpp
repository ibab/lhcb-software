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
#include "LoKi/ParticleContextCuts.h"
#include "LoKi/ParticleCuts.h"
// ============================================================================
// Declaration of the Tool Factory
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi,TopoTagDir);


// ============================================================================
// Standard Initialization
StatusCode LoKi::TopoTagDir::initialize()
{
  if(msgLevel(MSG::VERBOSE))
    verbose() << "Initialising algorithm" << endmsg;

  StatusCode sc = GaudiTool::initialize();
  if(sc.isFailure())
    return sc;


  svc<LoKi::ILoKiSvc>("LoKiSvc", true);

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
  jetWeight.clear();

  // Set member variables
  m_Jet = jet;
  // m_JetInputs = jet->daughtersVector();

 
  LHCb::Particle::Range topos;
  bool locseed = exist<LHCb::Particle::Range>( m_tinputs );
  if(!locseed){
    debug()<< "Unable to get location at '"
	   << m_tinputs  << "'" << endreq;
  }else if(locseed){
    topos = get<LHCb::Particle::Range>(  m_tinputs );
    if( !topos ){
      debug()<< "Unable to get particles at '"
	     << m_tinputs  << "'" << endreq;
    }
  }
  



  LHCb::Particle::Range::iterator itopo;

  std::pair<double,std::string> tmp;
  double maxTopoBDT = -1;
  double maxTopoR = -1;
  double maxTopoFD = -1;
  double maxTopoPtRel = -1;
  double maxTopoPt = -1;
  double maxTopoM = -1;
  double maxTopoMCorr = -1;
  double sumTopo = 0;
  double multTopo = 0;

  if(locseed){


    const LHCb::VertexBase* JetOriVtx = m_dva->bestVertex ( jet );

    for(itopo = topos.begin(); itopo!=topos.end(); itopo++){
      

      //      std::cout<<"helllllo cedric2"<<std::endl;

      LHCb::Track::ConstVector tracks;
      LHCb::Particle::ConstVector parts;
      LHCb::Particle::ConstVector::iterator iparts;
      LHCb::Track::ConstVector::iterator itrack;
      const LHCb::Particle* mypartS = *itopo;
      const LHCb::VertexBase* evtx = mypartS->endVertex();
      const LHCb::VertexBase* oriVtx = m_dva->bestVertex ( mypartS );

      if(oriVtx->key()!=JetOriVtx->key()) continue;

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
      



      //  double distance = std::sqrt(deltaRFunction((mypartS)));
      if (distance < 0.5){
	
	double defVal = -1;
	double topoVal = mypartS->info(9999,defVal);
	
	Gaudi::XYZVector B2 =  mypartS->momentum().Vect();
	
	//   copying code from Perp2 from ROOT
	double tot = A.Mag2();
	double ss = B2.Dot(A);
	double per = B2.Mag2();
	if(tot > 0.0) per -= ss*ss/tot;
	if(per < 0)   per = 0;
	
	double ptRel = std::sqrt(per);
	
	if(topoVal > maxTopoBDT){
	  maxTopoBDT = topoVal;
	  maxTopoPtRel = ptRel;
	  maxTopoR = distance;
	  maxTopoFD = std::sqrt(B.Mag2());
	  maxTopoPt = mypartS->pt();
	  maxTopoM = mypartS->momentum().M();
	  // m_BPVCORRM.setVertex(JetOriVtx);
	  //	  maxTopoMCorr = LOKI::CUTS::BPVCORRM(mypartS);
	}

	sumTopo+=topoVal;
        multTopo+=1;

      }
    }
  } 


  jetWeight["sumTopo"]=sumTopo;
  jetWeight["multTopo"]=multTopo;
  jetWeight["maxTopoBDT"]=maxTopoBDT;
  jetWeight["maxTopoPtRel"]=maxTopoPtRel;
  jetWeight["maxTopoR"]=maxTopoR;
  jetWeight["maxTopoPt"]=maxTopoPt;
  jetWeight["maxTopoM"]=maxTopoM;
  jetWeight["maxTopoMCorr"]=maxTopoMCorr;
  jetWeight["maxTopoFD"]=maxTopoFD;
  

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
