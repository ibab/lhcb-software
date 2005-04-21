// $Id: NewDirectionFitter.cpp,v 1.1 2005-04-21 17:04:02 xieyu Exp $

// Include files from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"

// Include files from Event
#include "Event/Particle.h"
#include "Event/Vertex.h"

// local file
#include "NewDirectionFitter.h"
#include "Kernel/IParticleTransporter.h"

//--------------------------------------------------------------------
//
//  ClassName  : NewDirectionFitter
// 
// Addapted from LifeTimeFitter written by G. Raven
// fit for lifetime and update  mass
// provide error matrix of (time, mass)
// provide the method for mass-constrained time
//
// @author Yuehong Xie
//
// @date 04/20/2005
//
//--------------------------------------------------------------------

//==================================================================
// Instantiation of a static factory class used by clients to create
// instances of this service
//==================================================================
static ToolFactory<NewDirectionFitter> s_factory;
const IToolFactory& NewDirectionFitterFactory = s_factory;

//==================================================================
// Standard Constructor
//==================================================================
NewDirectionFitter::NewDirectionFitter(const std::string& type, 
                               const std::string& name, 
                               const IInterface* parent) 
  : GaudiTool( type, name, parent )
{
  declareInterface<INewDirectionFitter>(this);
  declareProperty( "MaxIter", m_maxIter = 10);
  declareProperty( "MaxDeltaChiSq", m_maxDeltaChiSq = 0.01);

}
//==================================================================
// Perform fit
//==================================================================
StatusCode NewDirectionFitter::fit(const Vertex& productionVtx,
                               Particle& particle,
                               double& lifetime, double& mass,
                               HepSymMatrix& cov,
                               double& chisq) const
{

  debug() << " NewDirectionFitter starting " << endmsg;

  // problem is parameterized by 8 parameters: v,p4,tau
  HepVector p(8);
  HepSymMatrix Cp(8,0);
  // and we have 10 observables and the corresponding weight matrix
  HepVector O(10);
  HepSymMatrix W(10,0);

  if(!setup(particle,productionVtx,p,O,W)) return StatusCode::FAILURE;

  bool converged = false;
  double prev_chi_sq = 1.E20;
  for (unsigned i=0;i<m_maxIter&&!converged;++i) {
     double chi_sq = iterate( p, Cp, O, W );
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
     if(!transform(p,Cp,lifetime,mass,cov)) return StatusCode::FAILURE;
     debug() << " found lifetime " << lifetime 
         << " +- " << sqrt(cov(1,1)) <<  "   ( " << prev_chi_sq << " ) " << endmsg;
     return StatusCode::SUCCESS;
  } else {
     return StatusCode::FAILURE;
  }
}

bool 
NewDirectionFitter::setup(const Particle& part, const Vertex& vert, 
                      HepVector& p,HepVector& O, HepSymMatrix& W) const
{

  // initial guestimate of the 8 parameters:
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
  O[6] = p[6] = part.mass();
  O[7]= production.x();
  O[8]= production.y();
  O[9]= production.z();
  p[7] = part.mass()*mom.dot(decay-production)/mom.mag2();

  HepSymMatrix W7(7,0);
  W7.sub(1,decayVtx->positionErr());
  W7.sub(4,part.momentumErr());
  for (unsigned i=0;i<4;++i)
  for (unsigned j=0;j<3;++j) W7[3+i][j] = part.posMomCorr()[i][j];

  HepMatrix Te2m=MatrixE2M(part);
  W7=W7.similarity(Te2m);

  double massErr=part.massErr();
  if(massErr==0)  for(int i=1;i<=7;i++) W7(7,i)=0.;

  W*=0;
  W.sub(1,W7);
  W.sub(8,vert.positionErr());

  int ier=0;
  W.invert(ier);
  if (ier!=0) {
     err() << "could not invert matrix in NewDirectionFitter::setup" << endmsg;
     err() << "matrix W has determinant " << W.determinant() <<endmsg;
  }
  return (ier==0);
}

//==================================================================
// Method name: iterate
// This method performs one iteration
//==================================================================
double 
NewDirectionFitter::iterate(HepVector& p, HepSymMatrix& Cp, 
                        const HepVector& O, const HepSymMatrix& W)  const
{

  HepVector R(10);
  { for (unsigned i=0;i<7;++i) R[i] = O[i]-p[i]; }
  double mass=p[6];
  { for (unsigned i=0;i<3;++i) R[7+i] = O[7+i]-(p[i]-p[7]*p[3+i]/mass); }
  
  static const HepMatrix I7(  -1. * HepMatrix(7,7,1) );
  static const HepMatrix I3(  -1. * HepMatrix(3,3,1) );
  HepMatrix D(10,8,0);
  D.sub(1,1,I7);
  D.sub(8,1,I3);
  for (unsigned i=0;i<3;++i) D(8+i,8) = p[3+i]/mass;

  D(8,4)=p[7]/mass;
  D(8,7)=p[7]*p[3]*(-1/mass/mass);

  D(9,5)=p[7]/mass;
  D(9,7)=p[7]*p[4]*(-1/mass/mass);

  D(10,6)=p[7]/mass;
  D(10,7)=p[7]*p[5]*(-1/mass/mass);


  HepSymMatrix DWD = W.similarityT(D);
  // as long as we don't need the covariance 
  // matrix, 'solve' is faster and more robust
  // than inverting and multiplying...
  // but only HepMatrix has a solve, not HepSymMatrix...
  int ier=0;
  Cp = DWD.inverse(ier);
  if (ier!=0) {
     err() << "could not invert matrix in NewDirectionFitter::iterate" << endmsg;
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
// Method name: constrainMassForTime
// This method updates time using the (B) mass constraint
//==================================================================


StatusCode NewDirectionFitter::constrainMassForTime( double & time, double & mass,
                                                     HepSymMatrix& cov,
                                                     double newMass ) const
{
  double e11=cov(1,1);
  double e22=cov(2,2);
  double e21=cov(2,1);
  if(e22!=0) { 
    time-=(mass-newMass)*e21/e22;
    mass=newMass;
    cov(1,1)-=e21*e21/e22;
    cov(2,2)=0.;
    cov(2,1)=0.;
  }
  return StatusCode::SUCCESS;
}

//==================================================================
// Method name: transform
// This method pick out (time,mass) from the fitted parameters 
//==================================================================

bool NewDirectionFitter::transform(HepVector &p, HepSymMatrix& Cp,
               double& lifetime, double& mass,
               HepSymMatrix& cov) const 
{
  lifetime=p[7];
  mass=p[6];
  HepMatrix JA(2,8,0);
  JA(1,8)=1.;
  JA(2,7)=1.;
  cov=Cp.similarity(JA);  
  return true;
}

//==================================================================
// Method name: MatrixE2M
// Given a Particle, this method returns the Jaccobian matrix 
// from energy representation to mass representation 
//==================================================================

HepMatrix NewDirectionFitter::MatrixE2M(const Particle& part) const
{
  HepLorentzVector momentum = part.momentum();
  double px=momentum.vect().x();
  double py=momentum.vect().y();
  double pz=momentum.vect().z();
  double energy=momentum.e();
  double mass=momentum.mag();

  HepMatrix Te2m(7,7,1);
  Te2m(7,4)=-px/mass;
  Te2m(7,5)=-py/mass;
  Te2m(7,6)=-pz/mass;
  Te2m(7,7)=energy/mass;
  return Te2m;
}

//==================================================================

