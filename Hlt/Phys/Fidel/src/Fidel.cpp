// $Id: Fidel.cpp,v 1.15 2010-05-13 16:27:22 jpalac Exp $ // Include files
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ToolFactory.h"
#include "Kernel/IDistanceCalculator.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "LoKi/PhysTypes.h"
#include "LoKi/ATypes.h"
#include "LoKi/IHybridFactory.h"
#include "LoKi/LoKiPhys.h"
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
    // the actual cut for combination of good daughters
  , m_acut ( LoKi::BasicFunctors<LoKi::ATypes::Combination>::BooleanConstant ( true  ) )
    // the final cut the constructed mother particle
  , m_cut  ( LoKi::BasicFunctors<const LHCb::Particle*>::BooleanConstant     ( false ) )
{
  declareProperty ( "CombinationCut"   , m_combinationCut = "ATRUE",
                    "The cut to be applied for combination of good daughters"   ) ;
  declareProperty ( "MotherCut"        , m_motherCut = "NONE" ,
                    "The cut to be applied for the constructed mother particle" ) ;
  declareProperty ( "Factory"          , m_factory =   "LoKi::Hybrid::Tool/HybridFactory:PUBLIC" ,
                    "Type/Name for C++/Python Hybrid Factory"                   ) ;

  declareProperty("pid",        m_pid = 511,       "Default Pid");
  declareProperty("minFsB1Res", m_minFsB1Res = -999,"Minimum Flight Significance B-Res1");
  declareProperty("minFsB2Res", m_minFsB2Res = -999,"Minimum Flight Significance B-Res2");
  declareProperty("minFsB1ch",  m_minFsB1ch = -999, "Minimum Flight Significance B-Ch1");
  declareProperty("minFsB2ch",  m_minFsB2ch = -999, "Minimum Flight Significance B-Ch2");
  declareProperty("minFsB1KS0", m_minFsB1KS0 = -999,"Minimum Flight Significance B-KS01");
  declareProperty("minFsB2KS0", m_minFsB2KS0 = -999,"Minimum Flight Significance B-KS02");
  declareProperty("minFs",      m_minFs=-999,       "Minimum Flight Significance between PV and Res");
  declareProperty("minChFs",    m_minChFs=-999,     "Minimum Flight Significance between PV and Charmed particle");
 
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
  
  // locate the factory
  LoKi::IHybridFactory* factory = tool<LoKi::IHybridFactory> ( m_factory , this ) ;
  sc = factory -> get ( m_combinationCut , m_acut ) ;
  if ( sc.isFailure () )
    { return Error ( "Unable to  decode the 'A'-cut: " + m_combinationCut  , sc ) ; }
  sc = factory -> get ( m_motherCut , m_cut ) ;
  if ( sc.isFailure () )
  { return Error ( "Unable to  decode 'mother'-cut: " + m_motherCut  , sc ) ; }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode Fidel::execute() {

  using namespace LoKi::Cuts;

  if (msgLevel(MSG::DEBUG)) debug() << "==> Execute" << endmsg;

  setFilterPassed(false);

  const Particle::ConstVector& parts = this->i_particles();
  
  LHCb::Particle::ConstVector::const_iterator ip1;
  LHCb::Particle::ConstVector::const_iterator ip2;

  LHCb::Particle::ConstVector  Candidates;
  //  StatusCode sc = particleFilter() -> filter (parts,Candidates);
  //  if (!sc) return sc;
  
  LHCb::Particle::ConstVector  Muons;
  //  StatusCode scMu = particleFilter() -> filterByPID(parts,Muons,13,true);
  LoKi::select(parts.begin(), parts.end(), std::back_inserter(Muons), "mu+" == ABSID);
  
  //  if(!scMu)return scMu;
  

  double impCand=0;
  double ipchi2=0;
  double dist_B=0; double errdist_B=0; double fs_B=0;
  //double distB1=-9; double errdistB1=-9;double FsB1=-999;
  //double distB2=-9; double errdistB2=-9;double FsB2=-999;
  //double angleP=0;
  //double Pchi2=0;
  //double dist1,errdist1,fs1;
  //double dist2,errdist2,fs2;
  

  //============================================
  // Loop on particles
  //============================================
  
  for(ip1 = Candidates.begin();ip1 != Candidates.end(); ip1++){
    for(ip2 = ip1+1 ;ip2 != Candidates.end(); ip2++){
      
      if(Candidates.size()>m_inputParticles)continue;

      if(m_muonReq && Muons.size()==0)continue;//to use to simulate Hlt single muon
      
      // calculate quantum numbers
      int sumq=0, sumb=0, suml=0;
      sumq = (*ip1)->charge()+(*ip2)->charge();
      
      const LHCb::ParticleID* id1 = &((*ip1)->particleID());
      const LHCb::ParticleID* id2 = &((*ip2)->particleID());      
      sumb = (id1->isBaryon()*id1->pid()/abs(id1->pid()))+(id2->isBaryon()*id2->pid()/abs(id2->pid()));
      suml = (id1->isLepton()*id1->pid()/abs(id1->pid()))+(id2->isLepton()*id2->pid()/abs(id2->pid()));
      
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
      LHCb::Particle::ConstVector Combination ;
      Combination.push_back(*ip1);
      Combination.push_back(*ip2);

      if ( !m_acut ( Combination ) )  {
        if (msgLevel(MSG::VERBOSE)) verbose() << "    Failed Cut!" << endmsg ;
        continue ; }               // CONTINUE
      if (msgLevel(MSG::VERBOSE)) verbose() << "    Good combination" << endmsg ;

      if(checkOverlap()->foundOverlap(*ip1,*ip2))continue;
      
      if(!m_basicparticle && (*ip1)->isBasicParticle() && (*ip2)->isBasicParticle())continue;
      
      //---------------//
      //    Vertex     //
      //---------------//
      LHCb::ParticleID m_MothID(pid);      
      LHCb::Particle BCand(m_MothID);
      LHCb::Vertex BCandVertex;
      
      StatusCode fit = vertexFitter()->fit(*(*ip1),*(*ip2),BCandVertex,BCand);      
      if(!fit) {      
        if (msgLevel(MSG::DEBUG)) debug()<<"Fit error"<<endmsg;
        continue;        
      }

      if ( !m_cut  ( &BCand ) )  { continue ; }                    // CONTINUE

      const LHCb::VertexBase* bestPV  = this->i_bestVertex(&BCand);
      Gaudi::XYZPoint Origin = bestPV->position();

      const LHCb::Particle myBCand(BCand);
      //const LHCb::VertexBase *BVtx = BCand.endVertex();
      
      StatusCode sc = distanceCalculator()->distance(&myBCand,bestPV,impCand,ipchi2);
      if (!sc) return sc;
      
      //flight distance Projected B
      StatusCode sc_B = distanceCalculator()->projectedDistance(&myBCand,bestPV,dist_B,errdist_B);
      if (!sc_B) return sc_B ;
      fs_B = dist_B / errdist_B;
      
      /* /// @todo This is wrong. Could it be moved to daughters cuts?

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

      if((abs(id1->pid())==411 || abs(id1->pid())==431 || abs(id1->pid())==421) && fs1>-999 && fs1<m_minChFs)continue;
      else if (fs1>-999 && fs1<m_minFs)continue;
      
      if((abs(id2->pid())==411 || abs(id2->pid())==431 || abs(id2->pid())==421) && fs2>-999 && fs2<m_minChFs)continue;
      else if (fs2>-999 && fs2<m_minFs)continue;

      //cuts particle 1
      if((abs(id1->pid())==443 || abs(id1->pid())==333 || abs(id1->pid())==313 || abs(id1->pid())==413) &&
         FsB1>-999 && FsB1<m_minFsB1Res)continue;
      else if((abs(id1->pid())==411 || abs(id1->pid())==431 || abs(id1->pid())==421) && FsB1>-999 && FsB1<m_minFsB1ch)continue;
      else if(abs(id1->pid())==310 && FsB1>-999 && FsB1<m_minFsB1KS0)continue;
      
      //cuts particle 2
      if((abs(id2->pid())==443 || abs(id2->pid())==333 || abs(id2->pid())==313 || abs(id2->pid())==413) &&
         FsB2>-999 && FsB2<m_minFsB2Res)continue;
      else if((abs(id2->pid())==411 || abs(id2->pid())==431 || abs(id2->pid())==421) && FsB2>-999 && FsB2<m_minFsB2ch)continue;
      else if(abs(id2->pid())==310 && FsB2>-999 && FsB2<m_minFsB2KS0)continue;

      */      

      this->markTree(&BCand); 
      setFilterPassed(true);

    }//ip2
  }//ip1

  return StatusCode::SUCCESS;
  
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode Fidel::finalize() {

  if (msgLevel(MSG::DEBUG)) debug() << "==> Finalize" << endmsg;

  return DVAlgorithm::finalize(); 
}

//=============================================================================
