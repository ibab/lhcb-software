// $Id: Fidel.cpp,v 1.10 2008-08-04 17:23:27 pkoppenb Exp $ // Include files
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ToolFactory.h"
#include "Kernel/IDistanceCalculator.h"
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
  declareProperty("minMass",    m_minMass = 4000,  "Minimum B candidate mass"); 
  declareProperty("maxMass",    m_maxMass = 7000,  "Maximum B candidate mass");  
  declareProperty("minPt",      m_minPt = -999,    "Minimum Pt" );
  declareProperty("minP",       m_minP = -999,     "Minimum P");
  declareProperty("maxChi2",    m_maxChi2 = -999,  "Max Chi2 Vtx" );
  declareProperty("maxPointing",m_maxPointing = -999,"Max pointing angle");  
  declareProperty("pid",        m_pid = 511,       "Default Pid");
  declareProperty("maxIpchi2", m_maxIpchi2 = -999, "Maximum Chi2 Impact parameter");
  declareProperty("minCts",    m_minCts = -999,    "Minimum Flight Significance");  
  declareProperty("minProb",   m_minProb = -999,   "Minimum Probability of Chi2 vtx");
  declareProperty("minFsB1Res",m_minFsB1Res = -999,"Minimum Flight Significance B-Res1");
  declareProperty("minFsB2Res",m_minFsB2Res = -999,"Minimum Flight Significance B-Res2");
  declareProperty("minFsB1ch", m_minFsB1ch = -999, "Minimum Flight Significance B-Ch1");
  declareProperty("minFsB2ch", m_minFsB2ch = -999, "Minimum Flight Significance B-Ch2");
  declareProperty("minFsB1KS0",m_minFsB1KS0 = -999,"Minimum Flight Significance B-KS01");
  declareProperty("minFsB2KS0",m_minFsB2KS0 = -999,"Minimum Flight Significance B-KS02");
  declareProperty("minFs",     m_minFs=-999,       "Minimum Flight Significance between PV and Res");
  declareProperty("minChFs",   m_minChFs=-999,     "Minimum Flight Significance between PV and Charmed particle");
 
  declareProperty("inputParticles",m_inputParticles = 1000,"Max number particles in the loop");
  declareProperty("basicparticle",m_basicparticle = false,"Enable particles couple in loop");
  declareProperty("checkQ",m_checkQ = true,"Check quantum numbers-Charge");
  declareProperty("checkB",m_checkB = true,"Check quantum numbers-baryon number");
  declareProperty("checkL",m_checkL = true,"Check quantum numbers-lepton number");
  declareProperty("minQ",m_minQ = 0,       "Minimum charge");
  declareProperty("maxQ",m_maxQ = 0,       "Maximum charge");
  declareProperty("minB",m_minB = 0,       "Minimum baryon number");
  declareProperty("maxB",m_maxB = 0,       "Maximum baryon number");
  declareProperty("minL",m_minL = 0,       "Minimum lepton number");
  declareProperty("maxL",m_maxL = 0,       "Maximum lepton number");
  declareProperty("cc",m_cc = true,        "Check quantum number for charge coniugate");
  declareProperty("muonReq",m_muonReq = false,"Enable to simulate Hlt single muon");
  
  

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

  if (msgLevel(MSG::DEBUG)) debug() << "==> Initialize" << endmsg;

  if (msgLevel(MSG::DEBUG)) debug()<<"*************FIDEL CUTS************  "<<endmsg;
  if (msgLevel(MSG::DEBUG)) debug()<<"minMass  "<<m_minMass<<", maxMass  "<<m_maxMass<<" MeV"<<endmsg;
  if (msgLevel(MSG::DEBUG)) debug()<<"minPt "<<m_minPt<<"MeV, max Chi2 "<<m_maxChi2<<endmsg;
  if (msgLevel(MSG::DEBUG)) debug()<<"pointing "<<m_maxPointing<<" rad"<<endmsg;
  
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode Fidel::execute() {

  if (msgLevel(MSG::DEBUG)) debug() << "==> Execute" << endmsg;

  setFilterPassed(false);

  const Particle::ConstVector& parts = desktop()->particles();
  
  LHCb::Particle::ConstVector::const_iterator ip1;
  LHCb::Particle::ConstVector::const_iterator ip2;

  LHCb::Particle::ConstVector  Candidates;
  StatusCode sc = particleFilter() -> filter (parts,Candidates);
  if (!sc) return sc;
  
  LHCb::Particle::ConstVector  Muons;
  StatusCode scMu = particleFilter() -> filterByPID(parts,Muons,13,true);
  if(!scMu)return scMu;
  

  Gaudi::LorentzVector Cand1Cand2Comb;

  double impCand=0;
  double ipchi2=0;
  double dist_B=0; double errdist_B=0; double fs_B=0;
  double distB1=-9; double errdistB1=-9;double FsB1=-999;
  double distB2=-9; double errdistB2=-9;double FsB2=-999;
  double angleP=0;
  double Pchi2=0;
  double dist1,errdist1,fs1;
  double dist2,errdist2,fs2;
  
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
        if (msgLevel(MSG::DEBUG)) debug()<<"Fit error"<<endmsg;
        continue;        
      }

      const LHCb::VertexBase* bestPV  = desktop()->relatedVertex(&BCand);
      Gaudi::XYZPoint Origin = bestPV->position();

      const LHCb::Particle *myBCand = BCand.clone();
      const LHCb::VertexBase *BVtx = BCand.endVertex();
      
      StatusCode sc = distanceCalculator()->distance(myBCand,bestPV,impCand,ipchi2);
      if (!sc) return sc;
      
      //flight distance Projected B
      StatusCode sc_B = distanceCalculator()->projectedDistance(myBCand,bestPV,dist_B,errdist_B);
      if (!sc_B) return sc_B ;
      fs_B = dist_B / errdist_B;
      
      //flight distance Projected PV - Res 1
      StatusCode scfs1 = distanceCalculator()->projectedDistance((*ip1),bestPV,dist1,errdist1);
      if(scfs1)fs1 = dist1/errdist1;
      else{
        fs1=-999; 
      }
      //flight distance Projected PV - Res 2
      StatusCode scfs2 = distanceCalculator()->projectedDistance((*ip2),bestPV,dist2,errdist2);
      if(scfs2)fs2 = dist2/errdist2;
      else{
        fs2=-999; 
      }

      //flight distance Projected B - Res 1
      StatusCode scB1 = distanceCalculator()->projectedDistance((*ip1),BVtx,distB1,errdistB1);
      if(scB1)FsB1 = distB1/errdistB1;
      else{
        FsB1=-999; 
      }
      //flight distance Projected B - Res 2
      StatusCode scB2 = distanceCalculator()->projectedDistance((*ip2),BVtx,distB2,errdistB2);      

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
      //pointing>0
      if(cosangle<0)continue;
      
      //--------------------//
      //  chi2Probability   //
      //--------------------//      
      Pchi2 = gsl_cdf_chisq_Q(BCandVertex.chi2(),BCandVertex.nDoF());     
      
      //============================================//
      //  Latest cuts and save  the B candidate     //
      //============================================//
      if(m_maxIpchi2>0.0 && ipchi2>m_maxIpchi2)continue;
      if(m_maxChi2>0.0 && BCandVertex.chi2()>m_maxChi2)continue;
      if(m_maxPointing>0.0 && angleP>m_maxPointing)continue;      
      if(m_minCts>-999.0 && fs_B<m_minCts)continue;
      if(m_minProb>-999.0 && Pchi2<m_minProb)continue;
      
      if((abs(id1.pid())==411 || abs(id1.pid())==431 || abs(id1.pid())==421) && fs1>-999 && fs1<m_minChFs)continue;
      else if (fs1>-999 && fs1<m_minFs)continue;
      
      if((abs(id2.pid())==411 || abs(id2.pid())==431 || abs(id2.pid())==421) && fs2>-999 && fs2<m_minChFs)continue;
      else if (fs2>-999 && fs2<m_minFs)continue;
      
      //cuts particle 1
      if((abs(id1.pid())==443 || abs(id1.pid())==333 || abs(id1.pid())==313 || abs(id1.pid())==413) &&
         FsB1>-999 && FsB1<m_minFsB1Res)continue;
      else if((abs(id1.pid())==411 || abs(id1.pid())==431 || abs(id1.pid())==421) && FsB1>-999 && FsB1<m_minFsB1ch)continue;
      else if(abs(id1.pid())==310 && FsB1>-999 && FsB1<m_minFsB1KS0)continue;
      
      //cuts particle 2
      if((abs(id2.pid())==443 || abs(id2.pid())==333 || abs(id2.pid())==313 || abs(id2.pid())==413) &&
         FsB2>-999 && FsB2<m_minFsB2Res)continue;
      else if((abs(id2.pid())==411 || abs(id2.pid())==431 || abs(id2.pid())==421) && FsB2>-999 && FsB2<m_minFsB2ch)continue;
      else if(abs(id2.pid())==310 && FsB2>-999 && FsB2<m_minFsB2KS0)continue;


      desktop()->keep(&BCand); 
      setFilterPassed(true);

    }//ip2
  }//ip1

  return desktop()->saveDesktop();
  
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode Fidel::finalize() {

  if (msgLevel(MSG::DEBUG)) debug() << "==> Finalize" << endmsg;

  return DVAlgorithm::finalize(); 
}

//=============================================================================
