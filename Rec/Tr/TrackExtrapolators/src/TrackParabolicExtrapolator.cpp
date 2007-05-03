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
    // already at required z position
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
  const double nTx = 1.0+gsl_pow_2(Tx);
  const double nTy = 1.0+gsl_pow_2(Ty);
  const double norm = sqrt( nTx + nTy - 1.0 );

  // calculate the A factors 
  m_ax = norm*( Ty*(Tx*m_B.x()+m_B.z())-(nTx*m_B.y()));
  m_ay = norm*(-Tx*(Ty*m_B.y()+m_B.z())+(nTy*m_B.x()));

  const double fac = eplus*c_light*dz;
  const double fact = fac * stateVec[4];

  // Update the state parameters (exact extrapolation)
  stateVec[0] += dz*( Tx + 0.5 * m_ax * fact );
  stateVec[1] += dz*( Ty + 0.5 * m_ay * fact );
  stateVec[2] += m_ax * fact;
  stateVec[3] += m_ay * fact;

  if( transMat != NULL ) {
    updateTransportMatrix( dz, stateVec, transMat );
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Propagate a state to a given z-position
//=============================================================================
StatusCode TrackParabolicExtrapolator::propagate( State& state, 
                                                  double zNew, 
                                                  ParticleID pid )
{
  // Reset the transport matrix
  m_F = TrackMatrix( ROOT::Math::SMatrixIdentity() );
  
  // check current z-position
  const double dz = zNew - state.z();
  if (fabs(dz) < TrackParameters::hiTolerance) { 
    // already at required z position
    debug() << "already at required z position" << endreq;
    return StatusCode::SUCCESS; 
  }

  //get the B field at midpoint
  const double xMid = state.x() + (0.5*state.tx()*dz);
  const double yMid = state.y() + (0.5*state.ty()*dz);
  XYZPoint P( xMid, yMid, state.z()+(0.5*dz) );
  m_pIMF -> fieldVector( P, m_B ).ignore();

  // to save some typing...
  const double Tx   = state.tx();
  const double Ty   = state.ty();
  const double nTx  = 1.0 + gsl_pow_2(Tx);
  const double nTy  = 1.0 + gsl_pow_2(Ty);
  const double norm = sqrt( nTx + nTy - 1.0 );
 
  // calculate the A factors 
  m_ax = norm*( Ty*(Tx*m_B.x()+m_B.z())-(nTx*m_B.y()));
  m_ay = norm*(-Tx*(Ty*m_B.y()+m_B.z())+(nTy*m_B.x()));
   
  // set non-zero diagonal elements
  TrackMatrix* tm = NULL;
  TrackVector& tState = state.stateVector();
  updateTransportMatrix( dz, tState, tm ); 

  // Update the state parameters (exact extrapolation)
  double fact = tState[4]*eplus*c_light*dz;
  state.setState( tState[0] + dz*( Tx + 0.5*m_ax*fact ),
                  tState[1] + dz*( Ty + 0.5*m_ay*fact ),
                  zNew,
                  tState[2] + m_ax*fact,
                  tState[3] + m_ay*fact,
                  tState[4] );

  state.setCovariance( ROOT::Math::Similarity<double,TrackMatrix::kRows,TrackMatrix::kCols>
                       ( m_F, state.covariance() ) );

  return StatusCode::SUCCESS;
};

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
  if ( diff.R() < TrackParameters::hiTolerance ) { return StatusCode::SUCCESS;}
  
  // The distance between the reference point and a point on the parabola
  // can be minimized by taking the derivative wrt Z and equal that to zero.
  // This implies solving a cubic equation, resulting in 1 or 3 solutions.

  double Tx   = state.tx();
  double Ty   = state.ty();
  double nTx  = 1.0+gsl_pow_2(Tx);
  double nTy  = 1.0+gsl_pow_2(Ty);
  double norm = sqrt( nTx + nTy - 1.0 );
  m_ax = norm*( Ty*(Tx*m_B.x()+m_B.z())-(nTx*m_B.y()));
  m_ay = norm*(-Tx*(Ty*m_B.y()+m_B.z())+(nTy*m_B.x()));
  double varA     = 0.5*m_ax*state.qOverP()*eplus*c_light;
  double varB     = 0.5*m_ay*state.qOverP()*eplus*c_light;
  double alpha    = 2*( gsl_pow_2(varA) + gsl_pow_2(varB) );
  double beta     = 3*( Tx*varA + Ty*varB );
  
  double gamma    = 2*P.x()*varA + gsl_pow_2(Tx) + 1 -2*point.x()*varA + 
    2*P.y()*varB + gsl_pow_2(Ty) - 2*point.y()*varB;
  
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
                                                        TrackVector& stateVec,
                                                        TrackMatrix* transMat )
{
  // Create the transport matrix, has to be separate to allow fast transport.
  
  // to save some typing...
  double Tx = stateVec[2];
  double Ty = stateVec[3];
  double norm = sqrt(1.+gsl_pow_2(Tx)+gsl_pow_2(Ty));
  double norm2 = gsl_pow_2(norm);
  
  //calculate derivatives of Ax, Ay
  double dAx_dTx = (Tx*m_ax/norm2) + norm*(Ty*m_B.x()-(2.*Tx*m_B.y())); 
  double dAx_dTy = (Ty*m_ax/norm2) + norm*(Tx*m_B.x()+m_B.z());
  double dAy_dTx = (Tx*m_ay/norm2) + norm*(-Ty*m_B.y()-m_B.z());
  double dAy_dTy = (Ty*m_ay/norm2) + norm*(-Tx*m_B.y()+(2.*Ty*m_B.x()));

  // fill transport matrix 
  double fac = eplus*c_light*dz;
  double fac2 = eplus*c_light*gsl_pow_2(dz);
  double fact = fac*stateVec[4];
  double fact2 = fac2*stateVec[4];

  m_F(0,2) = dz + 0.5 * dAx_dTx * fact2;
  m_F(0,3) = 0.5 * dAx_dTy * fact2;
  m_F(0,4) = 0.5 * m_ax * fac2;
  
  m_F(1,2) = 0.5 * dAy_dTx * fact2;
  m_F(1,3) = dz + 0.5 * dAy_dTy * fact2;
  m_F(1,4) = 0.5 * m_ay * fac2;
  
  m_F(2,2) = 1.0 + dAx_dTx * fact;
  m_F(2,3) = dAx_dTy * fact;
  m_F(2,4) = m_ax * fac;
  
  m_F(3,2) = dAy_dTx * fact;
  m_F(3,3) = 1.0 + dAy_dTy * fact;
  m_F(3,4) = m_ay * fac;

  if( transMat != NULL ) {
    (*transMat) = ROOT::Math::SMatrixIdentity();
    (*transMat)(0,2) = m_F(0,2);
    (*transMat)(0,3) = m_F(0,3);
    (*transMat)(0,4) = m_F(0,4);
    (*transMat)(1,2) = m_F(1,2);
    (*transMat)(1,3) = m_F(1,3);
    (*transMat)(1,4) = m_F(1,4);
    (*transMat)(2,2) = m_F(2,2);
    (*transMat)(2,3) = m_F(2,3);
    (*transMat)(2,4) = m_F(2,4);
    (*transMat)(3,2) = m_F(3,2);
    (*transMat)(3,3) = m_F(3,3);
    (*transMat)(3,4) = m_F(3,4);
  }
  
  return;
};

//=============================================================================
