// $Id: State.cpp,v 1.32 2008-04-17 11:27:11 wouter Exp $

#include <math.h>
#include <gsl/gsl_math.h>

// from GaudiKernel
#include "GaudiKernel/GenericMatrixTypes.h"
#include "GaudiKernel/GenericVectorTypes.h"
#include "GaudiKernel/SymmetricMatrixTypes.h"

// local
#include "Event/TrackTypes.h"
#include "Event/State.h"
#include "Event/TrackParameters.h"

using namespace LHCb;
using namespace Gaudi;

//-----------------------------------------------------------------------------
// Implementation file for class : State
//
// 2004-12-14 : Jose Hernando, Eduardo Rodrigues
//-----------------------------------------------------------------------------

//=============================================================================
// Default constructor
//=============================================================================
State::State():
  m_flags(0),
  m_stateVector(TrackVector()),
  m_covariance(TrackSymMatrix()),
  m_z(0.0){
  setLocation( State::LocationUnknown );
}

//=============================================================================
// Retrieve the charge-over-momentum Q/P of the state
//=============================================================================
double State::qOverP() const
{
  return m_stateVector[4];
}

//=============================================================================
// Retrieve the momentum of the state
//=============================================================================
double State::p() const
{
  return ( fabs(m_stateVector[4]) > TrackParameters::lowTolerance ? fabs( 1./m_stateVector[4]) : HUGE_VAL );
}

//=============================================================================
// Retrieve the transverse momentum of the state
//=============================================================================
double State::pt() const
{
  if ( fabs(m_stateVector[4]) > TrackParameters::lowTolerance ) {
    const double txy2 =   m_stateVector[2]*m_stateVector[2]
      + m_stateVector[3]*m_stateVector[3];
    return sqrt( txy2/(1.+txy2) ) / fabs( m_stateVector[4] );
  }
  return HUGE_VAL;
}

//=============================================================================
// Retrieve the 6D covariance matrix (x,y,z,px,py,pz) of the state
//=============================================================================
SymMatrix6x6 State::posMomCovariance() const
{
  // This computes the jacobian from (x,y,tx,ty,Q/p) to
  // (x,y,z,px,py,pz). The math is the same as in
  // Gaudi::Math::JacobdP4dMom, so we should just merge that.

  const double qP = qOverP();
  const double qP2 = qP*qP;

  const double tX = tx();
  const double tX2 = tX*tX;

  const double tY = ty();
  const double tY2 = tY*tY;
  
  const double invNorm = 1. / sqrt( 1. + tX2 + tY2 );
  const double mom = p();
  const double invNorm3 = invNorm * invNorm * invNorm;
  const int q = fabs(qP) > TrackParameters::lowTolerance ? (qP > 0 ? 1 : -1) : 0;
  ROOT::Math::SMatrix<double, 6, 5> jmat;
  jmat(0,0) = jmat(1,1) = 1.;
  jmat(3,2) = mom * (1.+tY2)*invNorm3;
  jmat(3,3) = jmat(4,2) = -mom * tX * tY * invNorm3;
  jmat(3,4) = -q * tX * invNorm / (qP2);
  jmat(4,3) = mom * (1.+tX2)*invNorm3;
  jmat(4,4) = -q *tY * invNorm / (qP2);
  jmat(5,2) = -mom * tX * invNorm3;
  jmat(5,3) = -mom * tY * invNorm3;
  jmat(5,4) = -q * invNorm * mom * mom; // -q == -1/q
  
  return ROOT::Math::Similarity( jmat, covariance() );
}

//=============================================================================
// Retrieve the squared error on the charge-over-momentum Q/P of the state
//=============================================================================
double State::errQOverP2() const
{
  return m_covariance(4,4);
}

//=============================================================================
// Update the Q/P value of the state
//=============================================================================
void State::setQOverP( double value )
{
  m_stateVector[4] = value;
}

//=============================================================================
// Update the squared error on the Q/P of the state
//=============================================================================
void State::setErrQOverP2( double value )
{
  m_covariance(4,4) = value;
}

//=============================================================================
// Clone the state
//=============================================================================
State* State::clone() const
{
  return new State(*this);
}

//=============================================================================
// Retrieve the errors on the 3D-position vector of the state
//=============================================================================
Gaudi::SymMatrix3x3 State::errPosition() const
{
  const Gaudi::SymMatrix6x6 temp = posMomCovariance();
  return temp.Sub<SymMatrix3x3>(0,0);
}

//=============================================================================
// Retrieve the errors on the slopes of the state
//=============================================================================
Gaudi::SymMatrix3x3 State::errSlopes() const
{
  Gaudi::SymMatrix3x3 err = Gaudi::SymMatrix3x3();
  err.Place_at( m_covariance.Sub<SymMatrix2x2>(2,2), 0, 0 );
  return err;
}

//=============================================================================
// Retrieve the squared error on the momentum of the state
//=============================================================================
double State::errP2() const
{
  return ( fabs(m_stateVector[4]) > TrackParameters::lowTolerance ?
           errQOverP2() * gsl_pow_4( p() ): 0. );
}

//=============================================================================
// Retrieve the errors on the momentum vector of the state
//=============================================================================
SymMatrix3x3 State::errMomentum() const
{
  const SymMatrix6x6 temp = posMomCovariance();
  return temp.Sub<SymMatrix3x3>(3,3);
}

//=============================================================================
// Retrieve the squared error on the Q/Pperp of the state
//=============================================================================
double State::errQOverPperp2() const
{
  const double tx2        = tx() * tx();
  const double ty2        = ty() * ty();
  const double qOverP2    = qOverP() * qOverP();
  const double transSlope = 1. + tx2;
  const double norm       = 1 + tx2 + ty2;

  const double QOverPperpError = ( (norm/transSlope) * m_covariance(4,4) )

    + ( qOverP2 * tx2 * ty2*ty2 * m_covariance(2,2)/
        (gsl_pow_3(transSlope)*norm))

    + ( qOverP2 * ty2 * m_covariance(3,3) / (norm*transSlope) )

    - ( 2. * qOverP() * tx() * ty2 * m_covariance(2,4)
        / ( transSlope*transSlope ) )

    + 2. * qOverP() * ty() * m_covariance(3,4) / transSlope

    - 2. * ( qOverP2 * tx() * ty() * ty2 * m_covariance(2,3)
             / ( norm* transSlope*transSlope ) );

  return QOverPperpError;
}

//=============================================================================
// Update the state vector (presumably of type State::HasMomentum)
//=============================================================================
void State::setState( double x, double y, double z,
                      double tx, double ty,
                      double qOverP )
{
  m_stateVector[0] = x;
  m_stateVector[1] = y;
  m_stateVector[2] = tx;
  m_stateVector[3] = ty;
  m_stateVector[4] = qOverP;
  m_z              = z;
}

//=============================================================================
// fillstream
//=============================================================================
std::ostream& LHCb::State::fillStream(std::ostream& os) const 
{
  os << "*** State ***" << std::endl
     << " z        : " << z() << std::endl
     << " location : " << location() << std::endl
     << " p               : " << (float) p() <<std::endl
     << " pt              : " << (float) pt() <<std::endl
     << " (x,y,tx,ty,q/p) : " << stateVector() << std::endl
     << " covariance      : " << covariance() << std::endl;

  return os;
}

//=============================================================================

