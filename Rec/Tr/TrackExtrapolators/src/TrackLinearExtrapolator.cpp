// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// from TrackEvent
#include "Event/TrackParameters.h"

// local
#include "TrackLinearExtrapolator.h"

// Declaration of the Tool Factory
static const  ToolFactory<TrackLinearExtrapolator>          s_factory ;
const        IToolFactory& TrackLinearExtrapolatorFactory = s_factory ;

//=============================================================================
// Propagate a State to a given z-position
//=============================================================================
StatusCode TrackLinearExtrapolator::propagate( State& state,
                                               double zNew,
                                               ParticleID pid )
{
  // Reset and update the transport matrix
  m_F = HepMatrix(5, 5, 1);
  double dz = zNew - state.z();
  m_F(1,3) = dz;
  m_F(2,4) = dz;

  debug() << "Transport matrix F =" << m_F << endreq;
  
  // Update the State
  HepVector& tState = state.stateVector();
  tState[0] += tState[2]*dz;
  tState[1] += tState[3]*dz;
  state.setZ( zNew );
  
  // update covariance
  HepSymMatrix& tStateCov = state.covariance();
  tStateCov = tStateCov.similarity(m_F); // F*C*F.T()

  debug() << " z propagation " << zNew
          << " propagated state " << state.stateVector()
          << " propagated cov   " << state.covariance()
          << " of particle pid " << pid.pid() << endreq;

  return StatusCode::SUCCESS;
}

StatusCode TrackLinearExtrapolator::predict(const State& state,
                                            const HepPlane3D& plane,
                                            double& dZ) 
{
  
  StatusCode sc = StatusCode::SUCCESS;
  
  // calculation of the z-position by linear extrapolation to the plane
  // ------------------------------------------------------------------
  // given the plane defined by (a,b,c,d) and the input state of
  // position (x0,y0,z0) and slopes (Tx,Ty),
  // the (x,y,z) of the intersection point verify the set of equations
  //    a*x + b*y + c*z + d = 0
  //    x = x0 + (z-z0)*Tx
  //    x = y0 + (z-z0)*Ty
  HepNormal3D nVec   = plane.normal();
  HepPoint3D  posVec = state.position();
  HepVector3D slpVec = state.slopes();

  // denominator 'den'
  double den = nVec.dot(slpVec);

  // devide by zero protection, indicates curling Track
  if( den < TrackParameters::looseTolerance )
  {
    return StatusCode::FAILURE;
    warning() << "Linear propagation requested devision by zero." << endreq;
  }

  // nominator 'nom'  
  double nom = nVec.dot(posVec) + plane.d();
  
  // z-value of intersection point
  double zNew = - (nom / den) + state.z();
  dZ = zNew - state.z();

  debug() << " z propagation " << zNew 
          << " delta-z " << dZ << endreq;

  return sc;

}

//=============================================================================
// Propagate a State to the closest position to the specified point
//=============================================================================
StatusCode TrackLinearExtrapolator::propagate( State& state,
                                               const HepPoint3D& point,
                                               ParticleID pid )
{
  // Distance = sqrt((x'-x0-Tx*dz)^2+(y'-y0-Ty*dz)^2+(z'-z0-dz)^2)
  // Find dz by solving: d(distance)/dz = 0
  HepPoint3D  pos = state.position();
  HepVector3D slo = state.slopes();
  HepPoint3D  dif = pos - point;
  
  // Remember that slo[2]==1 by definition
  double zNew = -2*( (dif[0]+dif[1]+dif[2]) / (slo[0]+slo[1]+1) );
  
  // Propagate to the point
  StatusCode sc = propagate(state, zNew, pid);
  
  return sc;
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackLinearExtrapolator::TrackLinearExtrapolator(const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent )
  : TrackExtrapolator ( type, name, parent ) {}

//=============================================================================
// Destructor
//=============================================================================
TrackLinearExtrapolator::~TrackLinearExtrapolator() {};
