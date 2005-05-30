// $Id: CheatedLifetimeFitter.cpp,v 1.3 2005-05-30 14:09:32 pkoppenb Exp $

// Include files from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/GaudiException.h"
//

#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/DiagMatrix.h"

// Include files from Event
#include "Event/Particle.h"
#include "Event/Vertex.h"

// local file
#include "CheatedLifetimeFitter.h"
#include "Kernel/IParticleTransporter.h"

//--------------------------------------------------------------------
//
//  ClassName  : CheatedLifetimeFitter
// 
//  Description: Performs lifetime fit with MC primary vertex,
//  seconday vertex or momentum.
//
//  addapted from LifetimeFitter written by G. Raven.
//                           
//  @Author     : Yuehong Xie
//
//--------------------------------------------------------------------

//==================================================================
// Instantiation of a static factory class used by clients to create
// instances of this service
//==================================================================
static ToolFactory<CheatedLifetimeFitter> s_factory;
const IToolFactory& CheatedLifetimeFitterFactory = s_factory;

//==================================================================
// Standard Constructor
//==================================================================
CheatedLifetimeFitter::CheatedLifetimeFitter(const std::string& type, 
                               const std::string& name, 
                               const IInterface* parent) 
  : GaudiTool( type, name, parent )
{
  declareInterface<ICheatedLifetimeFitter>(this);
  declareProperty( "MaxIter", m_maxIter = 10);
  declareProperty( "MaxDeltaChiSq", m_maxDeltaChiSq = 0.01);

}

//==================================================================
// Initialize
//==================================================================
StatusCode CheatedLifetimeFitter::initialize() {
  return StatusCode::SUCCESS;
  
}


//==================================================================
// Perform fit
//==================================================================
StatusCode CheatedLifetimeFitter::fit(const Vertex& productionVtx,
                               const Particle& particle,
                               double& lifetime, double& lifetimeError,
                               double& chisq) const
{

  debug() << " lifetime fitter starting " << endmsg;

  // problem is parameterized by 7 parameters: v,p,tau
  HepVector p(7);
  HepSymMatrix Cp(7,0);
  // and we have 9 observables and the corresponding weight matrix
  HepVector O(9);
  HepSymMatrix W(9,0);
  double mass;

  if(!setup(particle,productionVtx,mass,p,O,W)) return StatusCode::FAILURE;

  bool converged = false;
  double prev_chi_sq = 1.E20;
  for (unsigned i=0;i<m_maxIter&&!converged;++i) {
     double chi_sq = iterate( p, Cp, mass, O, W );
     debug()  << " iteration " << i << " chisq: " << chi_sq << endmsg;
     if(chi_sq<-9999.) {
       converged=false;
       break;
     }
     converged = ( fabs(prev_chi_sq-chi_sq) < m_maxDeltaChiSq );
     prev_chi_sq = chi_sq;
  }
  if (converged) {
     chisq = prev_chi_sq;
     lifetime = p[6];
     lifetimeError = sqrt(Cp[6][6]);
     debug()  << " found lifetime " << lifetime 
         << " +- " << lifetimeError <<  "   ( " << prev_chi_sq << " ) " << endmsg;

     return StatusCode::SUCCESS;
  } else {
     return StatusCode::FAILURE;
  }
}

bool 
CheatedLifetimeFitter::setup(const Particle& part, const Vertex& vert, 
                      double &mass,
                      HepVector& p,HepVector& O, HepSymMatrix& W) const
{

  mass = part.mass();
  // initial guestimate of the 7 parameters:
  const Hep3Vector& momV = part.momentum().vect();
  const HepPoint3D mom(momV.x(),momV.y(),momV.z());

  const Vertex* decayVtx = part.endVertex();
  const HepPoint3D& decay = decayVtx->position();
  const HepPoint3D& production = vert.position();
  // [] start from [0], () start from (1)...
  O[0] = p[0] = decay.x();
  O[1] = p[1] = decay.y();
  O[2] = p[2] = decay.z();
  O[3] = p[3] = mom.x();
  O[4] = p[4] = mom.y();
  O[5] = p[5] = mom.z();
  O[6]= production.x();
  O[7]= production.y();
  O[8]= production.z();
  p[6] = part.mass()*mom.dot(decay-production)/mom.mag2();

  // create the weight of the observables
  W*=0;
  W.sub(1,decayVtx->positionErr());
  W.sub(4,part.momentumErr().sub(1,3));
  W.sub(7,vert.positionErr());
  // FIXME: why doesn't HepSymMatrix allow one to put a HepMatrix 
  //        'above' its diagonal???
  //for (unsigned i=0;i<3;++i) 
  //for (unsigned j=0;j<3;++j) W[i][3+j] = part.posMomCorr()[i][j];
  
  //fixed by Y.Xie: posMomCorr is the lower square. 
  for (unsigned i=0;i<3;++i)
  for (unsigned j=0;j<3;++j) W[3+i][j] = part.posMomCorr()[i][j];

  int ier=0;
  W.invert(ier);
  if (ier!=0) {
     err() << "could not invert matrix in CheatedLifetimeFitter::setup" << endmsg;
     err() << "matrix W has determinant " << W.determinant() <<endmsg;
  }
  return (ier==0);
}

//==================================================================
// Method name: iterate
// This method performs one iteration
//==================================================================
double 
CheatedLifetimeFitter::iterate(HepVector& p, HepSymMatrix& Cp, 
                        const double& mass,const HepVector& O, const HepSymMatrix& W)  const
{
  // [] start from [0], () start from (1)...

  HepVector R(9);
  { for (unsigned i=0;i<6;++i) R[i] = O[i]-p[i]; }
  { for (unsigned i=0;i<3;++i) R[6+i] = O[6+i]-(p[i]-p[6]*p[3+i]/mass); }
  
  static const HepMatrix I6(  -1. * HepMatrix(6,6,1) );
  static const HepMatrix I3(  -1. * HepMatrix(3,3,1) );
  HepMatrix D(9,7,0);
  D.sub(1,1,I6);
  D.sub(7,1,I3);
  D.sub(7,4,(-p[6]/mass)*I3); 
  for (unsigned i=0;i<3;++i) D(7+i,7) = p[3+i]/mass;

  HepSymMatrix DWD = W.similarityT(D);
  // as long as we don't need the covariance 
  // matrix, 'solve' is faster and more robust
  // than inverting and multiplying...
  // but only HepMatrix has a solve, not HepSymMatrix...
  int ier=0;
  Cp = DWD.inverse(ier);
  if (ier!=0) {
     err() << "could not invert matrix in CheatedLifetimeFitter::iterate" << endmsg;
     err() << "matrix DWD has determinant " << DWD.determinant() <<endmsg;
     return -999999.;
  }

  HepVector DTWR = D.T() * ( W * R );
  HepVector delta = Cp * DTWR;
  p-=delta;

  HepSymMatrix x = W.similarityT(R);
  double chi1 = x[0][0] - dot(DTWR,delta);
  return chi1;

}
//==================================================================

//==================================================================
// Perform fit with MCVertex
//==================================================================
StatusCode CheatedLifetimeFitter::fitWithMCPV(const MCVertex& productionVtx,
                               const Particle& particle,
                               double& lifetime, double& lifetimeError,
                               double& chisq) const
{

  debug() << " lifetime fitter with MCPV starting " << endmsg;

  // problem is parameterized by 4 parameters: p,tau
  HepVector p(4);
  HepSymMatrix Cp(4,0);
  // and we have 6 observables and the corresponding weight matrix
  HepVector O(6);
  HepSymMatrix W(6,0);
  double mass;

  if(!setupWithMCPV(particle,productionVtx,mass,p,O,W)) return StatusCode::FAILURE;
  bool converged = false;
  double prev_chi_sq = 1.E20;
  for (unsigned i=0;i<m_maxIter&&!converged;++i) {
     double chi_sq = iterateWithMCPV( p, Cp, mass, O, W );
     debug() << " iteration " << i << " chisq: " << chi_sq << endmsg;
     if(chi_sq<-9999.) {
       converged=false;
       break;
     }
     converged = ( fabs(prev_chi_sq-chi_sq) < m_maxDeltaChiSq );
     prev_chi_sq = chi_sq;
  }
  if (converged) {
     chisq = prev_chi_sq;
     lifetime = p[3];
     lifetimeError = sqrt(Cp[3][3]);
     debug() << " found lifetime with MC PV " << lifetime 
         << " +- " << lifetimeError <<  "   ( " << prev_chi_sq << " ) " << endmsg;

     return StatusCode::SUCCESS;
  } else {
     return StatusCode::FAILURE;
  }
}

//==================================================================
// Method name: setupWithMCPV
//==================================================================

bool 
CheatedLifetimeFitter::setupWithMCPV(const Particle& part, const MCVertex& vert, 
                      double &mass,
                      HepVector& p,HepVector& O, HepSymMatrix& W) const
{
  mass = part.mass();
  // initial guestimate of the 4 parameters:
  const Hep3Vector& momV = part.momentum().vect();
  const HepPoint3D mom(momV.x(),momV.y(),momV.z());

  const Vertex* decayVtx = part.endVertex();
  const HepPoint3D& decay = decayVtx->position();
  const HepPoint3D& production = vert.position();
  // [] start from [0], () start from (1)...
  O[0] = decay.x()-production.x();
  O[1] = decay.y()-production.y();
  O[2] = decay.z()-production.z();
  O[3] = p[0] = mom.x();
  O[4] = p[1] = mom.y();
  O[5] = p[2] = mom.z();
  p[3] = part.mass()*mom.dot(decay-production)/mom.mag2();

  // create the weight of the observables
  W*=0;
  W.sub(1,decayVtx->positionErr());
  W.sub(4,part.momentumErr().sub(1,3));
  
  for (unsigned i=0;i<3;++i)
  for (unsigned j=0;j<3;++j) W[3+i][j] = part.posMomCorr()[i][j];

  int ier=0;
  W.invert(ier);
  if (ier!=0) {
     err() << "could not invert matrix in LifetimeFitter::setupWithMCPV" << endmsg;
  }
  return (ier==0);
}

//==================================================================
// Method name: iterateWithMCPV
// This method performs one iteration for fit with MCVertex
//==================================================================
double 
CheatedLifetimeFitter::iterateWithMCPV(HepVector& p, HepSymMatrix& Cp, 
                        const double& mass,const HepVector& O, const HepSymMatrix& W)  const
{
  // [] start from [0], () start from (1)...

  HepVector R(6);
  { for (unsigned i=0;i<3;++i) R[i] = O[i]-p[i]/mass*p[3]; }
  { for (unsigned i=3;i<6;++i) R[i] = O[i]-p[i-3]; }
  
  static const HepMatrix I3(  -1. * HepMatrix(3,3,1) );
  HepMatrix D(6,4,0);
  D.sub(1,1, p[3]/mass*I3); 
  D.sub(4,1, I3);
  for (unsigned i=1;i<4;++i) D(i,4) = -p[i-1]/mass;

  HepSymMatrix DWD = W.similarityT(D);
  // as long as we don't need the covariance 
  // matrix, 'solve' is faster and more robust
  // than inverting and multiplying...
  // but only HepMatrix has a solve, not HepSymMatrix...
  int ier=0;
  Cp = DWD.inverse(ier);
  if (ier!=0) {
     err() << "could not invert matrix in LifetimeFitter::iterateWithMCPV" << endmsg;
     err() << "matrix DWD has determinant " << DWD.determinant() <<endmsg;
     return -999999.;
  }

  HepVector DTWR = D.T() * ( W * R );
  HepVector delta = Cp * DTWR;
  p-=delta;

  HepSymMatrix x = W.similarityT(R);
  double chi1 = x[0][0] - dot(DTWR,delta);
  return chi1;

}

//==================================================================
// Perform fit with cheated decay vertex 
//==================================================================
StatusCode CheatedLifetimeFitter::fitWithMCVB(const Vertex& productionVtx,
                               const Particle& particle,
                               const MCParticle& mcparticle,
                               double& lifetime, double& lifetimeError,
                               double& chisq) const
{

  debug() << " lifetime fitter with MCVB starting " << endmsg;

  // problem is parameterized by 4 parameters: p,tau
  HepVector p(4);
  HepSymMatrix Cp(4,0);
  // and we have 6 observables and the corresponding weight matrix
  HepVector O(6);
  HepSymMatrix W(6,0);
  double mass;

  if(!setupWithMCVB(particle,mcparticle,productionVtx,mass,p,O,W)) return StatusCode::FAILURE;
  bool converged = false;
  double prev_chi_sq = 1.E20;
  for (unsigned i=0;i<m_maxIter&&!converged;++i) {
     double chi_sq = iterateWithMCVB( p, Cp, mass, O, W );
     debug() << " iteration " << i << " chisq: " << chi_sq << endmsg;
     if(chi_sq<-9999.) {
       converged=false;
       break;
     }
     converged = ( fabs(prev_chi_sq-chi_sq) < m_maxDeltaChiSq );
     prev_chi_sq = chi_sq;
  }
  if (converged) {
     chisq = prev_chi_sq;
     lifetime = p[3];
     lifetimeError = sqrt(Cp[3][3]);
     debug() << " found lifetime with MCB Vertex " << lifetime 
         << " +- " << lifetimeError <<  "   ( " << prev_chi_sq << " ) " << endmsg;

     return StatusCode::SUCCESS;
  } else {
     return StatusCode::FAILURE;
  }
}

//==================================================================
// Method name: setupWithMCVB
//==================================================================

bool 
CheatedLifetimeFitter::setupWithMCVB(const Particle& part, 
                      const MCParticle& mcpart,
                      const Vertex& vert, 
                      double &mass,
                      HepVector& p,HepVector& O, HepSymMatrix& W) const
{
  mass = part.mass();
  // initial guestimate of the 4 parameters:
  const Hep3Vector& momV = part.momentum().vect();
  const HepPoint3D mom(momV.x(),momV.y(),momV.z());
  const HepPoint3D& decay = mcpart.endVertices().front()->position();
  const HepPoint3D& production = vert.position();
  // [] start from [0], () start from (1)...
  O[0] = p[0] = mom.x();
  O[1] = p[1] = mom.y();
  O[2] = p[2] = mom.z();
  O[3] = production.x()-decay.x();
  O[4] = production.y()-decay.y();
  O[5] = production.z()-decay.z();
  p[3] = part.mass()*mom.dot(decay-production)/mom.mag2();

  // create the weight of the observables
  W*=0;
  W.sub(1,part.momentumErr().sub(1,3));
  W.sub(4, vert.positionErr());

  int ier=0;
  W.invert(ier);
  if (ier!=0) {
     err() << "could not invert matrix in LifetimeFitter::setupWithMCVB" << endmsg;
  }
  return (ier==0);
}

//==================================================================
// Method name: iterateWithMCVB
// This method performs one iteration for fit with cheated decay vertex
//==================================================================
double 
CheatedLifetimeFitter::iterateWithMCVB(HepVector& p, HepSymMatrix& Cp, 
                        const double& mass,const HepVector& O, const HepSymMatrix& W)  const
{
  // [] start from [0], () start from (1)...

  HepVector R(6);
  { for (unsigned i=0;i<3;++i) R[i] = O[i]-p[i]; }
  { for (unsigned i=3;i<6;++i) R[i] = O[i]+p[i-3]/mass*p[3]; }
  

  static const HepMatrix I3(  -1. * HepMatrix(3,3,1) );
  HepMatrix D(6,4,0);
  D.sub(1,1, I3);
  D.sub(4,1, -p[3]/mass*I3);
  for (unsigned i=4;i<7;++i) D(i,4) = p[i-4]/mass;

  HepSymMatrix DWD = W.similarityT(D);
  // as long as we don't need the covariance 
  // matrix, 'solve' is faster and more robust
  // than inverting and multiplying...
  // but only HepMatrix has a solve, not HepSymMatrix...
  int ier=0;
  Cp = DWD.inverse(ier);
  if (ier!=0) {
     err() << "could not invert matrix in LifetimeFitter::iterateWithMCVB" << endmsg;
     err() << "matrix DWD has determinant " << DWD.determinant() <<endmsg;
     return -999999.;
  }

  HepVector DTWR = D.T() * ( W * R );
  HepVector delta = Cp * DTWR;
  p-=delta;

  HepSymMatrix x = W.similarityT(R);
  double chi1 = x[0][0] - dot(DTWR,delta);
  return chi1;

}

//==================================================================
// Perform fit with cheated momentum
//==================================================================
StatusCode CheatedLifetimeFitter::fitWithMCPB(const Vertex& productionVtx,
                               const Particle& particle,
                               const MCParticle& mcparticle,
                               double& lifetime, double& lifetimeError,
                               double& chisq) const
{

  debug() << " lifetime fitter with MCPB starting " << endmsg;

  // problem is parameterized by 4 parameters: pv,tau
  HepVector p(4);
  HepSymMatrix Cp(4,0);
  // and we have 6 observables and the corresponding weight matrix
  HepVector O(6);
  HepSymMatrix W(6,0);
  double mass;
  HepVector3D vmom;

  if(!setupWithMCPB(particle,mcparticle,productionVtx, vmom, mass,p,O,W)) return StatusCode::FAILURE;
  bool converged = false;
  double prev_chi_sq = 1.E20;
  for (unsigned i=0;i<m_maxIter&&!converged;++i) {
     double chi_sq = iterateWithMCPB( p, Cp, vmom, mass, O, W );
     debug() << " iteration " << i << " chisq: " << chi_sq << endmsg;
     if(chi_sq<-9999.) {
       converged=false;
       break;
     }
     converged = ( fabs(prev_chi_sq-chi_sq) < m_maxDeltaChiSq );
     prev_chi_sq = chi_sq;
  }
  if (converged) {
     chisq = prev_chi_sq;
     lifetime = p[3];
     lifetimeError = sqrt(Cp[3][3]);
     debug() << " found lifetime with MCB momentum " << lifetime 
         << " +- " << lifetimeError <<  "   ( " << prev_chi_sq << " ) " << endmsg;

     return StatusCode::SUCCESS;
  } else {
     return StatusCode::FAILURE;
  }
}

//==================================================================
// Method name: setupWithMCPB
//==================================================================

bool 
CheatedLifetimeFitter::setupWithMCPB(const Particle& part, 
                      const MCParticle& mcpart,
                      const Vertex& vert, 
                      HepVector3D & vmom,
                      double &mass,
                      HepVector& p,HepVector& O, HepSymMatrix& W) const
{

  mass = mcpart.virtualMass();
  vmom = mcpart.momentum().vect();
  // initial guestimate of the 4 parameters:
  const Hep3Vector& momV = part.momentum().vect();
  const HepPoint3D mom(momV.x(),momV.y(),momV.z());
  const Vertex* decayVtx = part.endVertex();
  const HepPoint3D& decay = decayVtx->position();
  const HepPoint3D& production = vert.position();
  // [] start from [0], () start from (1)...
  O[0] = p[0] = production.x();
  O[1] = p[1] = production.y();
  O[2] = p[2] = production.z();
  O[3] = decay.x();
  O[4] = decay.y();
  O[5] = decay.z();
  p[3] = part.mass()*mom.dot(decay-production)/mom.mag2();

  // create the weight of the observables
  W*=0;
  W.sub(1,vert.positionErr());
  W.sub(4,decayVtx->positionErr());
  
  int ier=0;
  W.invert(ier);
  if (ier!=0) {
     err() << "could not invert matrix in LifetimeFitter::setupWithMCPB" << endmsg;
  }
  return (ier==0);
}

//==================================================================
// Method name: iterateWithMCPB
// This method performs one iteration for fit with cheated momentum
//==================================================================
double 
CheatedLifetimeFitter::iterateWithMCPB(HepVector& p, HepSymMatrix& Cp, 
                        const HepVector3D & vmom,
                        const double& mass,const HepVector& O, const HepSymMatrix& W)  const
{
  // [] start from [0], () start from (1)...

  HepVector R(6);
  HepVector TMP(3); // stupid thing needed for windows
  TMP[0] = vmom.x(); TMP[1] = vmom.y(); TMP[2] = vmom.z();
  { for (unsigned i=0;i<3;++i) R[i] = O[i]-p[i]; }
  { for (unsigned i=3;i<6;++i){ 
    R[i] = O[i]
      -(p[i-3]
        +TMP[i-3]
        /mass*p[3]); }
  }
  
  
  static const HepMatrix I3(  -1. * HepMatrix(3,3,1) );
  HepMatrix D(6,4,0);
  D.sub(1,1, I3); 
  D.sub(4,1, I3);
  for (unsigned i=4;i<7;++i) D(i,4) = -TMP[i-4]/mass;

  HepSymMatrix DWD = W.similarityT(D);
  // as long as we don't need the covariance 
  // matrix, 'solve' is faster and more robust
  // than inverting and multiplying...
  // but only HepMatrix has a solve, not HepSymMatrix...
  int ier=0;
  Cp = DWD.inverse(ier);
  if (ier!=0) {
     err() << "could not invert matrix in LifetimeFitter::iterateWithMCPB" << endmsg;
     err() << "matrix DWD has determinant " << DWD.determinant() <<endmsg;
     return -999999.;
  }

  HepVector DTWR = D.T() * ( W * R );
  HepVector delta = Cp * DTWR;
  p-=delta;

  HepSymMatrix x = W.similarityT(R);
  double chi1 = x[0][0] - dot(DTWR,delta);
  return chi1;

}


