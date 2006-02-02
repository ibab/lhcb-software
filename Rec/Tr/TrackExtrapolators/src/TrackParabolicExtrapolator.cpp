// Include files
// -------------
// from Gaudi
#include "GaudiKernel/IMagneticFieldSvc.h"
#include "GaudiKernel/ToolFactory.h"

// from GSL
#include "gsl/gsl_math.h"
#include "gsl/gsl_poly.h"

// from LHCbDefinitions
#include "Kernel/Vector3DTypes.h"
#include "Kernel/Point3DTypes.h"
#include "Kernel/PhysicalConstants.h"

// from TrackEvent
#include "Event/TrackParameters.h"
#include "Event/SHacks.h"

// Local 
#include "TrackParabolicExtrapolator.h"

// Needed for the creation of TrackParabolicExtrapolator objects.
static const ToolFactory<TrackParabolicExtrapolator> s_factory;
const IToolFactory& TrackParabolicExtrapolatorFactory = s_factory;

//=============================================================================
// TrackParabolicExtrapolator constructor.
//=============================================================================
TrackParabolicExtrapolator::TrackParabolicExtrapolator
( const std::string& type,
  const std::string& name,
  const IInterface* parent )
  : TrackExtrapolator(type, name, parent) {}

//=============================================================================
// TrackParabolicExtrapolator destructor.
//=============================================================================
TrackParabolicExtrapolator::~TrackParabolicExtrapolator() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode TrackParabolicExtrapolator::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() )
    {
      return Error("Failed to initialize", sc);
    }
  
  m_pIMF = svc<IMagneticFieldSvc>("MagneticFieldSvc", true);
 
  return StatusCode::SUCCESS;
}

//=============================================================================
// Propagate a state to a given z-position
//=============================================================================
StatusCode TrackParabolicExtrapolator::propagate( State& state, 
                                                  double zNew, 
                                                  ParticleID )
{
  // Reset the transport matrix
  //TODO: remove the work-around as soon as MathCore has something to replace
  //      m_F = HepMatrix(5, 5, 1);
  m_F = TransportMatrix();
  for ( unsigned int ind = 0; ind < 5; ++ind ) m_F(ind,ind) = 1.;
  
  // check current z-position
  double dz = zNew - state.z();
  if (fabs(dz) < TrackParameters::hiTolerance) { 
    // already at required z position
    debug() << "already at required z position" << endreq;
    return StatusCode::SUCCESS; 
  }

  //get the B field at midpoint
  double xMid = state.x() + (0.5*state.tx()*dz);
  double yMid = state.y() + (0.5*state.ty()*dz);
  XYZPoint P( xMid, yMid, state.z()+(0.5*dz) );
  m_pIMF -> fieldVector( P, m_B );

  // to save some typing...
  double Tx   = state.tx();
  double Ty   = state.ty();
  double nTx  = sqrt( 1.0+gsl_pow_2(Tx) );
  double nTy  = sqrt( 1.0+gsl_pow_2(Ty) );
  double norm = sqrt( 1.0+gsl_pow_2(Tx)+gsl_pow_2(Ty) );
 
  // calculate the A factors 
  m_ax = norm*( Ty*(Tx*m_B.x()+m_B.z())-(gsl_pow_2(nTx)*m_B.y()));
  m_ay = norm*(-Tx*(Ty*m_B.y()+m_B.z())+(gsl_pow_2(nTy)*m_B.x()));
   
  // set non-zero diagonal elements
  updateTransportMatrix(dz, state); 

  // Update the state parameters (exact extrapolation)
  const TrackVector& tState = state.stateVector();
  state.setState( tState[0] + dz*(Tx + 0.5*m_ax*tState[4]*eplus*c_light*dz),
                  tState[1] + dz*(Ty + 0.5*m_ay*tState[4]*eplus*c_light*dz),
                  zNew,
                  tState[2] + m_ax*tState[4]*eplus*c_light*dz,
                  tState[3] + m_ay*tState[4]*eplus*c_light*dz,
                  tState[4] );
  state.setCovariance( SHacks::Similarity<TransportMatrix,TrackMatrix>
                       ( m_F, state.covariance() ) );

  return StatusCode::SUCCESS;
};

//=============================================================================
// Propagate a state to the closest position to the specified point
//=============================================================================
StatusCode TrackParabolicExtrapolator::propagate( State& state,
                                                  const XYZPoint& point,
                                                  ParticleID pid )
{
  StatusCode sc = StatusCode::SUCCESS;

  // Check whether not already at reference point
  XYZPoint P     = state.position();
  XYZVector diff = point - P;
  if ( diff.R() < TrackParameters::hiTolerance ) { return StatusCode::SUCCESS; }
  
  // The distance between the reference point and a point on the parabola
  // can be minimized by taking the derivative wrt Z and equal that to zero.
  // This implies solving a cubic equation, resulting in 1 or 3 solutions.

  double Tx   = state.tx();
  double Ty   = state.ty();
  double nTx  = sqrt(1.0+gsl_pow_2(Tx));
  double nTy  = sqrt(1.0+gsl_pow_2(Ty));
  double norm = sqrt(1.0+gsl_pow_2(Tx)+gsl_pow_2(Ty));
  m_ax = norm*( Ty*(Tx*m_B.x()+m_B.z())-(gsl_pow_2(nTx)*m_B.y()));
  m_ay = norm*(-Tx*(Ty*m_B.y()+m_B.z())+(gsl_pow_2(nTy)*m_B.x()));
  double varA     = 0.5*m_ax*state.qOverP()*eplus*c_light;
  double varB     = 0.5*m_ay*state.qOverP()*eplus*c_light;
  double alpha    = 2*( varA*varA + varB*varB );
  double beta     = 3*( Tx*varA + Ty*varB );
  
  double gamma    = 2*P.x()*varA + Tx*Tx + 1 -2*point.x()*varA + 
    2*P.y()*varB + Ty*Ty - 2*point.y()*varB;
  
  double delta    = 
    P.x()*Tx - point.x()*Tx + P.y()*Ty - point.y()*Ty + P.z() - point.z();
  
  // The coefficients of the cubic equation
  double coefA = beta  / alpha;
  double coefB = gamma / alpha;
  double coefC = delta / alpha;

  // Create pointers in which to store the solutions (zNew = zOld+sol)
  double so1 = 999.;
  double so2 = 999.;
  double so3 = 999.;

  double* sol1 = &so1;
  double* sol2 = &so2;
  double* sol3 = &so3;

  gsl_poly_solve_cubic( coefA, coefB, coefC, sol1, sol2, sol3 );

  // Choose the solution closest to the present position
  if( ((*sol1) >= 0.) && ((*sol1) != 999.) )
  {
    sc = propagate(state, P.z()+(*sol1), pid);
  }
  else { sc = propagate(state, P.z()+*sol3, pid); }
  
  return sc;
};

//=============================================================================
// Update the transport matrix
//=============================================================================
void TrackParabolicExtrapolator::updateTransportMatrix( const double dz, 
                                                        State& state )
{
  //create the transport matrix dX/dX_0 for ptState's
  //has to be separate to allow fast transport
  
  // to save some typing...
  double Tx = state.tx();
  double Ty = state.ty();
  double norm = sqrt(1.+gsl_pow_2(Tx)+gsl_pow_2(Ty));
  
  //calculate derivatives of Ax, Ay
  double dAx_dTx = (Tx*m_ax/gsl_pow_2(norm)) + norm*(Ty*m_B.x()-(2.*Tx*m_B.y())); 
  double dAx_dTy = (Ty*m_ax/gsl_pow_2(norm)) + norm*(Tx*m_B.x()+m_B.z());
  double dAy_dTx = (Tx*m_ay/gsl_pow_2(norm)) + norm*(-Ty*m_B.y()-m_B.z());
  double dAy_dTy = (Ty*m_ay/gsl_pow_2(norm)) + norm*(-Tx*m_B.y()+(2.*Ty*m_B.x()));
  
  // fill transport matrix 
  m_F(0,2) = dz + 0.5*dAx_dTx*state.qOverP() * eplus*c_light*gsl_pow_2(dz);
  m_F(0,3) = 0.5*dAx_dTy*state.qOverP()*eplus*c_light*gsl_pow_2(dz);
  m_F(0,4) = 0.5*m_ax*eplus*c_light*gsl_pow_2(dz);
  
  m_F(1,2) = 0.5*dAy_dTx*state.qOverP()*eplus*c_light*gsl_pow_2(dz);
  m_F(1,3) = dz + 0.5*dAy_dTy*state.qOverP()*eplus*c_light*gsl_pow_2(dz);
  m_F(1,4) = 0.5*m_ay*eplus*c_light*gsl_pow_2(dz);
  
  m_F(2,2) = 1.0+ dAx_dTx*state.qOverP()*eplus*c_light*dz;
  m_F(2,3) = dAx_dTy*state.qOverP()*eplus*c_light*dz;
  m_F(2,4) = m_ax*eplus*c_light*dz;
  
  m_F(3,2) = dAy_dTx*state.qOverP()*eplus*c_light*dz;
  m_F(3,3) = 1.0 + dAy_dTy*state.qOverP()*eplus*c_light*dz;
  m_F(3,4) = m_ay*eplus*c_light*dz;
  
};

//=============================================================================
