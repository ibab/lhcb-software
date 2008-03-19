// $Id: Fidel.cpp,v 1.1.1.1 2008-03-19 11:11:46 pkoppenb Exp $ // Include files
// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "GaudiKernel/ToolFactory.h"

// from Event
#include "Event/Particle.h"
#include "Event/Vertex.h"
#include "Event/RecHeader.h"
#include "Event/ProcessHeader.h"
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"

// from DaVinci
#include "Kernel/ICheckOverlap.h"
#include "Kernel/Particle2MCLinker.h"
#include "Kernel/IRelatedPV.h"

#include "Event/HltSummary.h"
#include "Event/HltEnums.h"
#include "Event/L0DUReport.h"

#include "gsl/gsl_cdf.h"

using namespace LHCb;
using namespace Gaudi::Units;


class Matrix{
public:            
  Matrix(double a1,double a2,double a3, double b1,double b2, double b3, double c1, double c2, double c3){
    a[0]=a1;
    a[1]=a2;
    a[2]=a3;
    b[0]=b1;
    b[1]=b2;
    b[2]=b3;
    c[0]=c1;
    c[1]=c2;
    c[2]=c3;
  }
  double a[3]; 
  double b[3];
  double c[3];
  
  double det(){
    double det1=b[1]*c[2]-b[2]*c[1];
    double det2=-1*(b[0]*c[2]-b[2]*c[0]);
    double det3=b[0]*c[1]-b[1]*c[0];    
    double d=a[0]*det1+a[1]*det2+a[2]*det3;
    return d;
  }			
};//class

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
  
  ,m_pLinker(0)
  
{
  declareProperty("minMass", m_minMass = 5000); 
  declareProperty("maxMass", m_maxMass = 7000);  
  declareProperty("minPt",   m_minPt = -999);
  declareProperty("maxChi2", m_maxChi2 = -999);
  declareProperty("maxPointing",m_maxPointing = -999);  
  declareProperty("pid",m_pid = 511);
  declareProperty("maxIps",m_maxIps = -999);
  declareProperty("maxDist",m_maxDist = -999);
  declareProperty("minCts",m_minCts = -999);  
  declareProperty("minProb",m_minProb = -999);
  declareProperty("inputParticles",m_inputParticles = 1000);
  declareProperty("basicparticle",m_basicparticle = false);
  declareProperty("fillTupla",m_fillTupla = false );
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
  debug()<<"max distance between 2 secondary Vtx "<<m_maxDist<<" mm, pointing "<<m_maxPointing<<" rad"<<endmsg;

  m_pLinker = new Particle2MCLinker(this,Particle2MCMethod::Chi2,"");

  m_relatedPV = tool<IRelatedPV>("RelatedPV",this );
  if (!m_relatedPV) {
    fatal() << "    Unable to retrieve RelatedPV " ;
    return StatusCode::FAILURE;  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode Fidel::execute() {

  debug() << "==> Execute" << endmsg;

  setFilterPassed(false);

  const Particle::ConstVector& parts = desktop()->particles();
  const RecVertex::ConstVector& PVs = desktop()->primaryVertices();
  
  LHCb::Particle::ConstVector::const_iterator ip1;
  LHCb::Particle::ConstVector::const_iterator ip2;

  LHCb::Particle::ConstVector  Candidates;
  StatusCode sc = particleFilter() -> filter (parts,Candidates);
  
  LHCb::Particle::ConstVector  Muons;
  StatusCode scMu = particleFilter() -> filterByPID(parts,Muons,13,true);
  
  Gaudi::LorentzVector Cand1Cand2Comb;
  
  double dist=0;
  double pointing=1000;
  double a = 0;
  double Acos = 0;
  double minflight = 0;
  double ips=0;
  double impCand=0;double iperr=0;
  double dist_B=0; double errdist_B=0; double fs_B=0;
  double ipB_geo=0;
  double angleP=0;
  double Pchi2=0;
  int    muonFlag=0;
  
  
  //============================================
  // Loop on particles
  //============================================
  
  for(ip1 = Candidates.begin();ip1 != Candidates.end(); ip1++){
    for(ip2 = ip1+1 ;ip2 != Candidates.end(); ip2++){
      
      if(Candidates.size()>m_inputParticles)continue;

      if(Muons.size()==0)muonFlag = 1;
      if(m_muonReq && Muons.size()==0)continue;
      
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
      if(checkOverlap()->foundOverlap(*ip1,*ip2))continue;
      
      if(!m_basicparticle && (*ip1)->isBasicParticle() && (*ip2)->isBasicParticle())continue;
      
      //---------------------//
      //   Standard  Vertex  //
      //---------------------//
      LHCb::Vertex BCandVertex;      
      LHCb::ParticleID m_MothID(pid);      
      LHCb::Particle::Particle BCand(m_MothID);
      StatusCode fit = vertexFitter()->fit(*(*ip1),*(*ip2),BCand,BCandVertex);
      
      //-----------------------//
      //    Geometrical Tool  //
      //-----------------------//
      const Gaudi::XYZPoint pos1 = (*ip1)->referencePoint();
      const Gaudi::XYZPoint pos2 = (*ip2)->referencePoint();      
      double p1x = (*ip1)->momentum().x();double p1y =(*ip1)->momentum().y();double p1z = (*ip1)->momentum().z();
      double p2x = (*ip2)->momentum().x();double p2y =(*ip2)->momentum().y();double p2z = (*ip2)->momentum().z();
      Gaudi::XYZVector p1(p1x,p1y,p1z);
      Gaudi::XYZVector p2(p2x,p2y,p2z);
      Gaudi::XYZVector Psum = p1 + p2;
      
      //=====================================
      //  simple distance between 2 vertex
      //  Implemented by N.Serra 
      //  Adapted by S.Furcas
      //=====================================
      
      //=====================================
      // Distance between  2 skew line
      //=====================================
      Gaudi::XYZVector v1;
      v1=p1.Cross(p2);
      v1=v1.Unit();
      double m = (v1.X()*(pos1.X()-pos2.X()) + v1.Y()*(pos1.Y()-pos2.Y()) +v1.Z()*(pos1.Z()-pos2.Z()));
      double x3 = pos1.X() + m * v1.X();
      double y3 = pos1.Y() + m * v1.Y();
      double z3 = pos1.Z() + m * v1.Z();
      
      dist = sqrt((pos1.X()-x3)*(pos1.X()-x3) + (pos1.Y()-y3)*(pos1.Y()-y3) + (pos1.Z()-z3)*(pos1.Z()-z3));
      
      //====================================
      //   minimal approch line
      //====================================
      
      Gaudi::XYZVector n1 = v1.Cross(p1);
      n1=n1.Unit();
      double d1 = -1*(n1.X()*pos1.X() + n1.Y()*pos1.Y() +n1.Z()*pos1.Z()) ;
      Gaudi::XYZVector n2 = v1.Cross(p2);
      n2=n2.Unit();
      double d2 = -1*(n2.X()*pos2.X() + n2.Y()*pos2.Y() +n2.Z()*pos2.Z()) ;
      Gaudi::XYZVector pos1v(pos1.X(),pos1.Y(),pos1.Z());
      Gaudi::XYZVector n = pos1v.Cross(p2);
      n=n.Unit();
      double d3 = -1*((n.X()*pos1.X())+(n.Y()*pos1.Y())+(n.Z()*pos1.Z()));
      double d4 = -1*((n.X()*pos2.X())+(n.Y()*pos2.Y())+(n.Z()*pos2.Z()));
      double DetDen = n1.X()*((n2.Y()*n.Z()-n2.Z()*n.Y()))-n1.Y()*((n2.X()*n.Z()-n2.Z()*n.X()))+
        n1.Z()*((n2.X()*n.Y()-n2.Y()*n.X()));
      Matrix Det(n1.X(),n1.Y(),n1.Z(),n2.X(),n2.Y(),n2.Z(),n.X(),n.Y(),n.Z());
      double den = Det.det();
      Matrix X1_matrix(-1*d1, n1.Y(), n1.Z(), -1*d2, n2.Y(), n2.Z(), -1*d3, n.Y(), n.Z());
      Matrix Y1_matrix(n1.X(), -1*d1, n1.Z(), n2.X(), -1*d2, n2.Z(), n.X(), -1*d3, n.Z());
      Matrix Z1_matrix(n1.X(), n1.Y(), -1*d1, n2.X(), n2.Y(),  -1*d2, n.X(), n.Y(), -1*d3);
      double X1=X1_matrix.det()/den;
      double Y1=Y1_matrix.det()/den;
      double Z1=Z1_matrix.det()/den;
      Matrix X2_matrix(-1*d1, n1.Y(), n1.Z(), -1*d2, n2.Y(), n2.Z(), -1*d4, n.Y(), n.Z());
      Matrix Y2_matrix(n1.X(), -1*d1, n1.Z(), n2.X(), -1*d2, n2.Z(), n.X(), -1*d4, n.Z());
      Matrix Z2_matrix(n1.X(), n1.Y(), -1*d1, n2.X(), n2.Y(),  -1*d2, n.X(), n.Y(), -1*d4);
      double X2=X2_matrix.det()/den;
      double Y2=Y2_matrix.det()/den;
      double Z2=Z2_matrix.det()/den;
      double co1=X2-X1;
      double co2=Y2-Y1;
      double co3=Z2-Z1;
      
      Gaudi::XYZPoint VtxCand(co1+X1, co2+Y1, co3+Z1);
      LHCb::Vertex BCandVtx(Gaudi::XYZPoint& VtxCand);
      
      const LHCb::VertexBase* bestPV  = m_relatedPV->bestPV(&BCand);//BCand
      
      if(bestPV!=NULL){        
        //geometrical variables 
        Gaudi::XYZPoint Origin = bestPV->position();
        Gaudi::XYZVector flight = VtxCand - Origin;
        double FD_geom = sqrt(flight.Mag2());
        flight = flight.Unit();
        Psum = Psum.Unit();
        a = flight.Dot(Psum);
        Acos = acos (a);
        if(a>0){
          pointing = Acos;
          minflight=FD_geom;
          ipB_geo=sin(pointing)*minflight;
          if(pointing==1000)continue;
        }        
        
        StatusCode sc = geomDispCalculator()->calcImpactPar(BCand,*bestPV,impCand,iperr);
        //impact parameter significance
        ips=impCand/iperr;            
        //flight distance 
        StatusCode sc_B = geomDispCalculator()->calcProjectedFlightDistance(*bestPV,BCand,dist_B,errdist_B);
        fs_B = dist_B / errdist_B;
        //pointing
        Gaudi::XYZPoint x = BCandVertex.position();
        Gaudi::XYZVector myDist = x - Origin;
        Gaudi::XYZVector p(BCand.momentum().Vect());
        double cosangle = p.Dot(myDist)/sqrt(p.mag2())/sqrt(myDist.mag2());
        if(cosangle>0){
          angleP = acos(cosangle);//angle in rad
        }
      }//best PV

      //--------------------//
      //  chi2Probability   //
      //--------------------//      
      Pchi2 = gsl_cdf_chisq_Q(BCandVertex.chi2(),BCandVertex.nDoF());     
      
      //============================================//
      //  Latest cuts and save  the B candidate     //
      //============================================//
      if(m_maxIps>0.0 && ips>m_maxIps)continue;
      if(m_maxChi2>0.0 && BCandVertex.chi2()>m_maxChi2)continue;
      if(m_maxDist>0.0 && dist>m_maxDist)continue;
      if(m_maxPointing>0.0 && pointing>m_maxPointing)continue;      
      if(m_minCts>-999.0 && fs_B<m_minCts)continue;
      if(m_minProb>-999.0 && Pchi2<m_minProb)continue;

      desktop()->save(&BCand); 
      setFilterPassed(true);
      sc = StatusCode::SUCCESS;
      
      //--------------//
      //  Ntupla      //
      //--------------//
      if(m_fillTupla) {
        
        int runNum, evtNum;
        SmartDataPtr<RecHeader> evt(eventSvc(), RecHeaderLocation::Default);
        runNum=evt->runNumber();
        evtNum=evt->evtNumber();
        
        LHCb::L0DUReport* report = get<LHCb::L0DUReport>(LHCb::L0DUReportLocation::Default);
        long Level0decision = report->decision();
        
        const LHCb::MCParticle* mc1;
        const LHCb::MCParticle* mc2;
        
        Tuple MyTupla = nTuple("MyTupla");
        MyTupla->column("evt",evtNum);
        MyTupla->column("run",runNum);          
        MyTupla->column("nPV",PVs.size());              
        MyTupla->column("L0",Level0decision);
        MyTupla->column("ch1",(*ip1)->charge());
        MyTupla->column("ch2",(*ip2)->charge());
        MyTupla->column("mass1",(*ip1)->momentum().mass());
        MyTupla->column("mass2",(*ip2)->momentum().mass());
        MyTupla->column("ID1",(*ip1)->particleID().pid());
        MyTupla->column("ID2",(*ip2)->particleID().pid());
        MyTupla->column("PT1",(*ip1)->pt());
        MyTupla->column("PT2",(*ip2)->pt());
        MyTupla->column("ipsB",ips);
        MyTupla->column("ipB",impCand);            
        MyTupla->column("muonFlag",muonFlag);      
        /////////////////
        // vertices    //
        /////////////////
        MyTupla->column("BCandVertex_ndof",BCandVertex.nDoF());
        MyTupla->column("BCandVertex_chi2",BCandVertex.chi2());
        MyTupla->column("ProbChi2",Pchi2);
        MyTupla->column("FD_geom",minflight);
        MyTupla->column("ipB_geo",ipB_geo);
        MyTupla->column("anglepointing",angleP);            
        ////////////
        //	MC    //
        ////////////
        mc1 = m_pLinker->firstMCP( *ip1 );
        mc2 = m_pLinker->firstMCP( *ip2 );
        
        if(mc1 && mc2){
          MyTupla->column("MCID1",mc1 -> particleID().pid());
          MyTupla->column("MCID2",mc2 -> particleID().pid());
          
          if(mc1->mother())MyTupla->column("MCMothID1",mc1 -> mother()-> particleID().pid());
          else{MyTupla->column("MCMothID1",-999);}
          if(mc2->mother())MyTupla->column("MCMothID2",mc2 -> mother()-> particleID().pid());
          else{MyTupla->column("MCMothID2",-999);}
        } 
        
        if(mc1 && mc2==0) {
          MyTupla->column("MCID1",mc1 -> particleID().pid());
          MyTupla->column("MCID2",-999);
          if(mc1->mother())MyTupla->column("MCMothID1",mc1 -> mother()-> particleID().pid());
          else{MyTupla->column("MCMothID1",-999);}  
          MyTupla->column("MCMothID2",-999);
        }
        
        if(mc2 && mc1==0) {
          MyTupla->column("MCID2",mc2 -> particleID().pid());
          MyTupla->column("MCID1",-999);
          MyTupla->column("MCMothID1",-999);
          if(mc2->mother())MyTupla->column("MCMothID2",mc2 -> mother()-> particleID().pid());
          else{MyTupla->column("MCMothID2",-999);}
        } 
        
        if(mc2==0 && mc1==0) {
          MyTupla->column("MCID2",-999);
          MyTupla->column("MCID1",-999);
          MyTupla->column("MCMothID1",-999);
          MyTupla->column("MCMothID2",-999);
        }
        
        MyTupla->column("ChargeB",sumq);
        MyTupla->column("InvMass",Cand1Cand2Comb.mass());
        MyTupla->column("PMoth",Cand1Cand2Comb.P());
        MyTupla->column("PTM",Cand1Cand2Comb.pt()); 
        MyTupla->column("Pointing",pointing);//rad
        MyTupla->column("dist",dist);//mm
        MyTupla->column("Fd_B",dist_B);//mm
        MyTupla->column("Fs_B",fs_B);//mm
        MyTupla->write();
      }//fill ntupla
      
      
      
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

  if(NULL!=m_pLinker) delete m_pLinker;

  return DVAlgorithm::finalize(); 
}

//=============================================================================
