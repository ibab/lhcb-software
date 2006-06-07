// $Id: PropertimeFitter.cpp,v 1.1 2006-06-07 14:34:41 xieyu Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

#include "Event/Vertex.h"
#include "Event/Particle.h"

#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"

// local
#include "PropertimeFitter.h"

using namespace LHCb ;
using namespace Gaudi::Units;
//-----------------------------------------------------------------------------
// Implementation file for class : PropertimeFitter
//
// 2006-06-07 : Yuehong Xie
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<PropertimeFitter>          s_factory ;
const        IToolFactory& PropertimeFitterFactory = s_factory ; 

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PropertimeFitter::PropertimeFitter( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<ILifetimeFitter>(this);

  declareProperty( "applyBMassConstraint", m_applyBMassConstraint = false);
  declareProperty( "maxIter", m_maxIter = 10);
  declareProperty( "MaxDeltaChi2", m_maxDeltaChi2 = 0.001);

}

//=============================================================================
// Destructor
//=============================================================================
PropertimeFitter::~PropertimeFitter() {};

//=============================================================================
// Initialize
//=============================================================================
StatusCode PropertimeFitter::initialize(){
  StatusCode sc = GaudiTool::initialize();
  if (!sc) return sc;

  m_ppSvc = svc<IParticlePropertySvc>("ParticlePropertySvc");
 
  return sc;
};

//=============================================================================
// Get propertime
// inputs: Vertex corresponding to the assumed production point
//         LHCb::Particle itself
// output: resulting propertime and error, chisq.
//=============================================================================
StatusCode PropertimeFitter::fit( const LHCb::Vertex& PV, const LHCb::Particle& B,
                                  double& propertime, double& error,
                                  double& chisq) const 
{
  StatusCode sc = StatusCode::SUCCESS;

  Gaudi::XYZPoint PVposition = PV.position();
  Gaudi::SymMatrix3x3 CovPV = PV.covMatrix();

  Gaudi::XYZPoint BPosition= B.referencePoint();
  Gaudi::LorentzVector BLmom= B.momentum();

  double BMeasuredMass = sqrt(BLmom.T()*BLmom.T()-BLmom.X()*BLmom.X()-BLmom.Y()*BLmom.Y()-BLmom.Z()*BLmom.Z());

  Gaudi::SymMatrix7x7 CovB = B.covMatrix(); 

  Gaudi::Matrix7x7 Te2m = ROOT::Math::SMatrixIdentity();
  Te2m(6,3) = -BLmom.X()/BMeasuredMass;
  Te2m(6,4) = -BLmom.Y()/BMeasuredMass;
  Te2m(6,5) = -BLmom.Z()/BMeasuredMass;
  Te2m(6,6) =  BLmom.T()/BMeasuredMass;

  Gaudi::SymMatrix7x7 CovBm = ROOT::Math::Similarity<double,7,7>(Te2m, CovB);

  ROOT::Math::SVector<double, 10> X;
  X[0] = PVposition.x();
  X[1] = PVposition.y();
  X[2] = PVposition.z();
  X[3] = BPosition.x();
  X[4] = BPosition.y();
  X[5] = BPosition.z();
  X[6] = BLmom.X();
  X[7] = BLmom.y();
  X[8] = BLmom.z();
  X[9] = BMeasuredMass;

  SymMatrix10x10 Cx;

  for(int l1=0;l1<=2; l1++) 
    for(int l2=0;l2<=l1; l2++) Cx(l1,l2) = CovPV(l1,l2);

  for(int l1=0;l1<=6; l1++)
    for(int l2=0;l2<=l1; l2++) Cx(l1+3,l2+3) = CovBm(l1,l2);

  for(int l1=3;l1<=9; l1++)
    for(int l2=0;l2<=2; l2++) Cx(l1,l2) = 0.0;
  
  // xpv, ypv, zpv, xb, yb, zb, pxb, pyb, pzb, mb 
  ROOT::Math::SVector<double, 10> vfit=X;

  SymMatrix10x10 cfit = Cx;

  bool converged=false;
  int iter=0;

  double chi2PreviousFit=9999.;
  double chi2Fit=999.;

  while(!converged && iter< m_maxIter)  {
    iter++;
    verbose() << ":-) Iteration   " << iter << endreq;

    //f(0)=(xb-xpv)*pzb-(zb-zpv)*pxb
    //f(1)=(yb-ypv)*pzb-(zb-zpv)*pyb
    
    ROOT::Math::SVector<double, 2> f;
    f(0)= (vfit[3]-vfit[0])*vfit[8] - (vfit[5]-vfit[2])*vfit[6];
    f(1)= (vfit[4]-vfit[1])*vfit[8] - (vfit[5]-vfit[2])*vfit[7];

    //D is the derivative matrix
    ROOT::Math::SMatrix<double, 2, 10> D;

    D(0,0) = - vfit[8];
    D(0,1) = 0.0;
    D(0,2) = vfit[6];
    D(0,3) = vfit[8];
    D(0,4) = 0.0;
    D(0,5) = -vfit[6];
    D(0,6) = -(vfit[5]-vfit[2]);
    D(0,7) = 0.0;
    D(0,8) = (vfit[3]-vfit[0]);
    D(0,9) = 0.0;

    D(1,0) = 0.0;
    D(1,1) = -vfit[8];
    D(1,2) = vfit[7];
    D(1,3) = 0.0;
    D(1,4) = vfit[8];
    D(1,5) = -vfit[7];
    D(1,6) = 0.0;
    D(1,7) = -(vfit[5]-vfit[2]);
    D(1,8) = (vfit[4]-vfit[1]);
    D(1,9) = 0.0;
 
  }

  if(m_applyBMassConstraint) {
    int Bpid = B.particleID().pid();
    ParticleProperty*  partProp = m_ppSvc->findByStdHepID(Bpid  );
    double nominalBMass = partProp->mass();
    ROOT::Math::SMatrix<double,1,10> DD;
    Gaudi::Vector1 dd;
    DD(0,9)=1.;
    dd[0]=-nominalBMass;
    Gaudi::SymMatrix1x1 Cd = ROOT::Math::Similarity<double, 1, 10>(DD,cfit);
    if ( !Cd.Invert() ) {
      debug() << "could not invert matrix Cd in fit" << endmsg;
      return StatusCode::FAILURE;
    }
    vfit-= cfit*ROOT::Math::Transpose(DD)*Cd*(DD*vfit+dd);
    SymMatrix10x10 deltacfit1=ROOT::Math::SimilarityT<double,1,10>(DD,Cd);
    SymMatrix10x10 deltacfit2=ROOT::Math::Similarity<double,10,10>(cfit,deltacfit1);
    cfit -= deltacfit2;

    for(int i=0;i<=9;i++) cfit(9,i)=0.0;

  }

  //propertime = (xb-xpv)*mb/pxb ;
  propertime =  (vfit[3]-vfit[0])*vfit[9]/vfit[6];  

  ROOT::Math::SMatrix<double, 1, 10> JA;
  JA(0,0) = -vfit[9]/vfit[6];
  JA(0,1) = 0.0;
  JA(0,2) = 0.0; 
  JA(0,3) = vfit[9]/vfit[6];
  JA(0,4) = 0.0;
  JA(0,5) = 0.0;
  JA(0,6) = -(vfit[3]-vfit[0])*vfit[9]/vfit[6]/vfit[6];
  JA(0,7) = 0.0;
  JA(0,8) = 0.0;
  JA(0,9) = (vfit[3]-vfit[0])/vfit[6];

  Gaudi::SymMatrix1x1 CovTau = ROOT::Math::Similarity<double,1,10>(JA, cfit);

  error = CovTau(0,0);

  // convert to fs  
  propertime /=  mm*c_light;   
  error /= mm*c_light;

  chisq =  chi2Fit;

  return sc;
}

