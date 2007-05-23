// Include files
// -------------
// from Gaudi
#include "GaudiKernel/IMagneticFieldSvc.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/PhysicalConstants.h"
using namespace Gaudi::Units;

// from GSL
#include "gsl/gsl_math.h"
#include "gsl/gsl_poly.h"

// Math Definitions
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Point3DTypes.h"

// from TrackEvent
#include "Event/TrackParameters.h"

// Local 
#include "TrackParabolicExtrapolator.h"

using namespace LHCb;
using namespace Gaudi;

DECLARE_TOOL_FACTORY( TrackParabolicExtrapolator );

//=============================================================================
// TrackParabolicExtrapolator constructor.
//=============================================================================
TrackParabolicExtrapolator::TrackParabolicExtrapolator
( const std::string& type,
  const std::string& name,
  const IInterface* parent )
  : TrackExtrapolator(type, name, parent) {

  declareProperty("FieldSvc", m_fieldSvc = "MagneticFieldSvc");
}

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
  
  m_pIMF = svc<IMagneticFieldSvc>(m_fieldSvc, true);
 
  return StatusCode::SUCCESS;
}

//=============================================================================
// Propagate a state vector from zOld to zNew
// Transport matrix is calulated when transMat pointer is not NULL
//=============================================================================
StatusCode TrackParabolicExtrapolator::propagate( TrackVector& stateVec,
                                                  double zOld,
                                                  double zNew,
                                                  TrackMatrix* transMat,
                                                  ParticleID pid )
{
  const double dz = zNew - zOld;
  if( fabs(dz) < TrackParameters::hiTolerance ) { 
    debug() << "already at required z position" << endreq;
    return StatusCode::SUCCESS; 
  }
  
  // get the B field at midpoint
  const double xMid = stateVec[0] + (0.5*stateVec[2]*dz);
  const double yMid = stateVec[1] + (0.5*stateVec[3]*dz);
  XYZPoint P( xMid, yMid, zOld+(0.5*dz) );
  m_pIMF -> fieldVector( P, m_B ).ignore();

  // to save some typing...
  const double Tx   = stateVec[2];
  const double Ty   = stateVec[3];
  const double nTx2 = 1.0+Tx*Tx;
  const double nTy2 = 1.0+Ty*Ty;
  const double norm = sqrt( nTx2 + nTy2 - 1.0 );

  // calculate the A factors 
  m_ax = norm*( Ty*(Tx*m_B.x()+m_B.z())-(nTx2*m_B.y()));
  m_ay = norm*(-Tx*(Ty*m_B.y()+m_B.z())+(nTy2*m_B.x()));

  const double fac = eplus*c_light*dz;
  const double fact = fac * stateVec[4];

  // Update the state parameters (exact extrapolation)
  stateVec[0] += dz*( Tx + 0.5 * m_ax * fact );
  stateVec[1] += dz*( Ty + 0.5 * m_ay * fact );
  stateVec[2] += m_ax * fact;
  stateVec[3] += m_ay * fact;

  if( transMat != NULL ) {
    updateTransportMatrix( dz, stateVec, *transMat );
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Propagate a state to the closest position to the specified point
//=============================================================================
StatusCode TrackParabolicExtrapolator::propagate( State& state,
                                                  const Gaudi::XYZPoint& point,
                                                  ParticleID pid )
{
  StatusCode sc = StatusCode::SUCCESS;

  // Check whether not already at reference point
  XYZPoint P     = state.position();
  XYZVector diff = point - P;
  if( diff.R() < TrackParameters::hiTolerance ) { return StatusCode::SUCCESS; }
  
  XYZPoint midP = P + 0.5*diff;
  m_pIMF -> fieldVector( midP, m_B ).ignore();

  // The distance between the reference point and a point on the parabola
  // can be minimized by taking the derivative wrt Z and equal that to zero.
  // This implies solving a cubic equation, resulting in 1 or 3 solutions.

  double Tx   = state.tx();
  double Ty   = state.ty();
  double nTx2 = 1.0+Tx*Tx;
  double nTy2 = 1.0+Ty*Ty;
  double norm = sqrt( nTx2 + nTy2 - 1.0 );
  m_ax = norm*( Ty*(Tx*m_B.x()+m_B.z())-(nTx2*m_B.y()));
  m_ay = norm*(-Tx*(Ty*m_B.y()+m_B.z())+(nTy2*m_B.x()));
  double varA  = 0.5*m_ax*state.qOverP()*eplus*c_light;
  double varB  = 0.5*m_ay*state.qOverP()*eplus*c_light;
  double alpha = 2.*( varA*varA + varB*varB );
  double beta  = 3.*( Tx*varA + Ty*varB );
  
  double gamma = 2.*P.x()*varA + Tx*Tx + 1. - 2.*point.x()*varA + 
    2.*P.y()*varB + Ty*Ty - 2.*point.y()*varB;
  
  double delta = P.x()*Tx-point.x()*Tx+P.y()*Ty-point.y()*Ty+P.z()-point.z();
  
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
    sc = TrackExtrapolator::propagate(state, P.z()+(*sol1), pid);
  }
  else { sc = TrackExtrapolator::propagate(state, P.z()+*sol3, pid); }
  
  return sc;
};

//=============================================================================
// Update the transport matrix
//=============================================================================
void TrackParabolicExtrapolator::updateTransportMatrix( const double dz, 
                                                        TrackVector& stateVec,
                                                        TrackMatrix& transMat )
{
  // Reset the transport matrix
  transMat = TrackMatrix( ROOT::Math::SMatrixIdentity() );
  
  // to save some typing...
  double Tx = stateVec[2];
  double Ty = stateVec[3];
  double norm2 = 1. + Tx*Tx + Ty*Ty;
  double norm = sqrt( norm2 );

  //calculate derivatives of Ax, Ay
  double dAx_dTx = (Tx*m_ax/norm2) + norm*(Ty*m_B.x()-(2.*Tx*m_B.y())); 
  double dAx_dTy = (Ty*m_ax/norm2) + norm*(Tx*m_B.x()+m_B.z());
  double dAy_dTx = (Tx*m_ay/norm2) + norm*(-Ty*m_B.y()-m_B.z());
  double dAy_dTy = (Ty*m_ay/norm2) + norm*(-Tx*m_B.y()+(2.*Ty*m_B.x()));

  // fill transport matrix 
  double fac  = eplus*c_light*dz;
  double fact = fac*stateVec[4];

  transMat(0,2) = dz + 0.5 * dAx_dTx * fact*dz;
  transMat(0,3) = 0.5 * dAx_dTy * fact*dz;
  transMat(0,4) = 0.5 * m_ax * fac*dz;
  
  transMat(1,2) = 0.5 * dAy_dTx * fact*dz;
  transMat(1,3) = dz + 0.5 * dAy_dTy * fact*dz;
  transMat(1,4) = 0.5 * m_ay * fac*dz;
  
  transMat(2,2) = 1.0 + dAx_dTx * fact;
  transMat(2,3) = dAx_dTy * fact;
  transMat(2,4) = m_ax * fac;
  
  transMat(3,2) = dAy_dTx * fact;
  transMat(3,3) = 1.0 + dAy_dTy * fact;
  transMat(3,4) = m_ay * fac;
  
  return;
};
