// $Id: Fidel.cpp,v 1.6 2008-07-10 16:23:30 pkoppenb Exp $ // Include files
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ToolFactory.h"
#include "Kernel/IGeomDispCalculator.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "gsl/gsl_cdf.h"

using namespace LHCb;
using namespace Gaudi::Units;

// local
#include "Fidel.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Fidel
//
// 2007-11-12 : Sara Furcas  && Rolf Oldeman
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( Fidel );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Fidel::Fidel( const std::string& name,
              ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator )
  
{
  declareProperty("minMass", m_minMass = 4000); 
  declareProperty("maxMass", m_maxMass = 7000);  
  declareProperty("minPt",   m_minPt = -999);
  declareProperty("minP",    m_minP = -999);
  declareProperty("maxChi2", m_maxChi2 = -999);
  declareProperty("maxPointing",m_maxPointing = -999);  
  declareProperty("pid",m_pid = 511);
  declareProperty("maxIps",m_maxIps = -999);
  declareProperty("minCts",m_minCts = -999);  
  declareProperty("minProb",m_minProb = -999);
  declareProperty("minFsB1",m_minFsB1 = -999);
  declareProperty("minFsB2",m_minFsB2 = -999);
  declareProperty("inputParticles",m_inputParticles = 1000);
  declareProperty("basicparticle",m_basicparticle = false);
  declareProperty("checkQ",m_checkQ = true );
  declareProperty("checkB",m_checkB = true );
  declareProperty("checkL",m_checkL = true );
  declareProperty("minQ",m_minQ = 0);
  declareProperty("maxQ",m_maxQ = 0);
  declareProperty("minB",m_minB = 0);
  declareProperty("maxB",m_maxB = 0);
  declareProperty("minL",m_minL = 0);
  declareProperty("maxL",m_maxL = 0);
  declareProperty("cc",m_cc = true );
  declareProperty("muonReq",m_muonReq = false);
  
}
//=============================================================================
// Destructor
//=============================================================================
Fidel::~Fidel() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode Fidel::initialize() {

  StatusCode sc = DVAlgorithm::initialize(); 
  if ( sc.isFailure() ) return sc;

  debug() << "==> Initialize" << endmsg;

  debug()<<"*************FIDEL CUTS************  "<<endmsg;
  debug()<<"minMass  "<<m_minMass<<", maxMass  "<<m_maxMass<<" MeV"<<endmsg;
  debug()<<"minPt "<<m_minPt<<"MeV, max Chi2 "<<m_maxChi2<<endmsg;
  debug()<<"pointing "<<m_maxPointing<<" rad"<<endmsg;
  
  /// @todo replce with IDistanceCalculator (from DVAlgorithm)
  m_geomDispCalculator = tool<IGeomDispCalculator>("GeomDispCalculator",this );
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode Fidel::execute() {

  debug() << "==> Execute" << endmsg;

  setFilterPassed(false);

  const Particle::ConstVector& parts = desktop()->particles();
  
  LHCb::Particle::ConstVector::const_iterator ip1;
  LHCb::Particle::ConstVector::const_iterator ip2;

  LHCb::Particle::ConstVector  Candidates;
  StatusCode sc = particleFilter() -> filter (parts,Candidates);
  
  LHCb::Particle::ConstVector  Muons;
  StatusCode scMu = particleFilter() -> filterByPID(parts,Muons,13,true);
  
  Gaudi::LorentzVector Cand1Cand2Comb;

  double ips=0;
  double impCand=0;double iperr=0;
  double dist_B=0; double errdist_B=0; double fs_B=0;
  double distB=0; double errdistB=0; double fsB=0;
  double distB1=-9; double errdistB1=-9;double FsB1=-999;
  double distB2=-9; double errdistB2=-9;double FsB2=-999;
  double angleP=0;
  double Pchi2=0;
  
  
  LHCb::Vertex BCandVertex; 


  //============================================
  // Loop on particles
  //============================================
  
  for(ip1 = Candidates.begin();ip1 != Candidates.end(); ip1++){
    for(ip2 = ip1+1 ;ip2 != Candidates.end(); ip2++){
      
      if(Candidates.size()>m_inputParticles)continue;

      if(m_muonReq && Muons.size()==0)continue;//to use to simulate Hlt single muon
      
      Gaudi::LorentzVector Cand1Cand2Comb = (*ip1)->momentum()+(*ip2)->momentum();
      
      // calculate quantum numbers
      int sumq=0, sumb=0, suml=0;
      sumq = (*ip1)->charge()+(*ip2)->charge();
      
      LHCb::ParticleID id1 = (*ip1)->particleID();
      LHCb::ParticleID id2 = (*ip2)->particleID();      
      sumb = (id1.isBaryon()*id1.pid()/abs(id1.pid()))+(id2.isBaryon()*id2.pid()/abs(id2.pid()));
      suml = (id1.isLepton()*id1.pid()/abs(id1.pid()))+(id2.isLepton()*id2.pid()/abs(id2.pid()));
      
      bool pass = true;
      if(m_checkQ &&(sumq<m_minQ || sumq>m_maxQ))pass=false;
      if(m_checkB &&(sumb<m_minB || sumb>m_maxB))pass=false;
      if(m_checkL &&(suml<m_minL || suml>m_maxL))pass=false;
      bool passcc=true;      
      if(m_checkQ && ( -sumq<m_minQ || -sumq>m_maxQ))passcc=false;
      if(m_checkB && ( -sumb<m_minB || -sumb>m_maxB))passcc=false;
      if(m_checkL && ( -suml<m_minL || -suml>m_maxL))passcc=false;
      bool accept=pass;
      if(m_cc)accept=pass||passcc;
      if(!accept)continue;
      int pid=m_pid;
      if(passcc && !pass)pid=-m_pid;

      //***************//
      //  First Cuts   //
      //***************//
      if(Cand1Cand2Comb.mass()< m_minMass || Cand1Cand2Comb.mass()>m_maxMass )continue;
      if(Cand1Cand2Comb.Pt()<m_minPt)continue;
      if(Cand1Cand2Comb.P()<m_minP)continue;
      
      if(checkOverlap()->foundOverlap(*ip1,*ip2))continue;
      
      if(!m_basicparticle && (*ip1)->isBasicParticle() && (*ip2)->isBasicParticle())continue;
      
      //---------------//
      //    Vertex     //
      //---------------//
      LHCb::ParticleID m_MothID(pid);      
      LHCb::Particle BCand(m_MothID);
      
      StatusCode fit = vertexFitter()->fit(*(*ip1),*(*ip2),BCand,BCandVertex);      
      if(!fit) {      
        Warning("Fit error");
        continue;        
      }

      const LHCb::VertexBase* bestPV  = desktop()->relatedVertex(&BCand);
      Gaudi::XYZPoint Origin = bestPV->position();
        
      StatusCode sc = m_geomDispCalculator->calcImpactPar(BCand,*bestPV,impCand,iperr);
      //impact parameter significance
      ips=impCand/iperr;
      //flight distance Projected B
      StatusCode sc_B = m_geomDispCalculator->calcProjectedFlightDistance(*bestPV,BCand,dist_B,errdist_B);
      fs_B = dist_B / errdist_B;
      //flight distance Signed B
      StatusCode scB = m_geomDispCalculator->calcSignedFlightDistance(*bestPV,BCand,distB,errdistB);
      fsB = distB / errdistB;
      //flight distance Projected B - Res 1
      StatusCode scB1 = m_geomDispCalculator->calcProjectedFlightDistance(BCandVertex,*(*ip1),distB1,errdistB1);
      //flight distance Projected B - Res 2
      StatusCode scB2 = m_geomDispCalculator->calcProjectedFlightDistance(BCandVertex,*(*ip2),distB2,errdistB2);

      if(scB1)FsB1 = distB1/errdistB1;
      else{
        FsB1=-999; 
      }
      if(scB2)FsB2 = distB2/errdistB2;
      else{
        FsB2=-999; 
      }
      

      //pointing
      Gaudi::XYZPoint x = BCandVertex.position();
      Gaudi::XYZVector myDist = x - Origin;
      Gaudi::XYZVector p(BCand.momentum().Vect());
      double cosangle = p.Dot(myDist)/sqrt(p.mag2())/sqrt(myDist.mag2());
      if(cosangle>0){
        angleP = acos(cosangle);//angle in rad
      }

      //--------------------//
      //  chi2Probability   //
      //--------------------//      
      Pchi2 = gsl_cdf_chisq_Q(BCandVertex.chi2(),BCandVertex.nDoF());     
      
      //============================================//
      //  Latest cuts and save  the B candidate     //
      //============================================//
      if(m_maxIps>0.0 && ips>m_maxIps)continue;
      if(m_maxChi2>0.0 && BCandVertex.chi2()>m_maxChi2)continue;
      if(m_maxPointing>0.0 && angleP>m_maxPointing)continue;      
      if(m_minCts>-999.0 && fs_B<m_minCts)continue;
      if(m_minProb>-999.0 && Pchi2<m_minProb)continue;

      if(FsB1>-999 && FsB1<m_minFsB1)continue;
      if(FsB2>-999 && FsB2<m_minFsB2)continue;


      desktop()->keep(&BCand); 
      setFilterPassed(true);
      sc = StatusCode::SUCCESS;

    }//ip2
  }//ip1

  return desktop()->saveDesktop();
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode Fidel::finalize() {

  debug() << "==> Finalize" << endmsg;

  return DVAlgorithm::finalize(); 
}

//=============================================================================
