// $Id: LifetimeFitter.cpp,v 1.2 2004-11-08 14:37:18 xieyu Exp $

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
#include "LifetimeFitter.h"
#include "DaVinciTools/IParticleTransporter.h"

//--------------------------------------------------------------------
//
//  ClassName  : LifetimeFitter
// 
//  Description: Performs a fit to determine proper time and
//               and error
//                           
//  @Author     : Gerhard Raven
//
//--------------------------------------------------------------------

//==================================================================
// Instantiation of a static factory class used by clients to create
// instances of this service
//==================================================================
static ToolFactory<LifetimeFitter> s_factory;
const IToolFactory& LifetimeFitterFactory = s_factory;

//==================================================================
// Standard Constructor
//==================================================================
LifetimeFitter::LifetimeFitter(const std::string& type, 
                               const std::string& name, 
                               const IInterface* parent) 
  : AlgTool( type, name, parent )
{
  declareInterface<ILifetimeFitter>(this);
  declareProperty( "MaxIter", m_maxIter = 10);
  declareProperty( "MaxDeltaChiSq", m_maxDeltaChiSq = 0.01);

}

//==================================================================
// Initialize
//==================================================================
StatusCode LifetimeFitter::initialize() {
  return StatusCode::SUCCESS;
  
}


//==================================================================
// Perform fit
//==================================================================
StatusCode LifetimeFitter::fit(const Vertex& productionVtx,
                               const Particle& particle,
                               double& lifetime, double& lifetimeError,
                               double& chisq) const
{

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << " lifetime fitter starting " << endmsg;

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
     log<< MSG::DEBUG << " iteration " << i << " chisq: " << chi_sq << endmsg;
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
     log << MSG::DEBUG << " found lifetime " << lifetime 
         << " +- " << lifetimeError <<  "   ( " << prev_chi_sq << " ) " << endmsg;

     return StatusCode::SUCCESS;
  } else {
     return StatusCode::FAILURE;
  }
}

bool 
LifetimeFitter::setup(const Particle& part, const Vertex& vert, 
                      double &mass,
                      HepVector& p,HepVector& O, HepSymMatrix& W) const
{
  MsgStream log(msgSvc(), name());

  mass = part.mass();
  // initial guestimate of the 7 parameters:
  const Hep3Vector& mom = part.momentum().vect();
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
     log << MSG::ERROR << "could not invert matrix in LifetimeFitter::setup" << endmsg;
     log << MSG::ERROR << "matrix W has determinant " << W.determinant() <<endmsg;
  }
  return (ier==0);
}

//==================================================================
// Method name: iterate
// This method performs one iteration
//==================================================================
double 
LifetimeFitter::iterate(HepVector& p, HepSymMatrix& Cp, 
                        const double& mass,const HepVector& O, const HepSymMatrix& W)  const
{
  MsgStream log(msgSvc(), name());
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
     log << MSG::ERROR << "could not invert matrix in LifetimeFitter::iterate" << endmsg;
     log << MSG::ERROR << "matrix DWD has determinant " << DWD.determinant() <<endmsg;
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
