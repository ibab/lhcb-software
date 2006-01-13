// $Id: State.cpp,v 1.12 2006-01-13 17:11:59 erodrigu Exp $

#include <math.h>
#include <gsl/gsl_math.h>

// local
#include "Event/State.h"

#include "Kernel/TrackTypes.h"
#include "Kernel/GenericMatrixTypes.h"
#include "Kernel/GenericVectorTypes.h"
#include "Kernel/SymmetricMatrixTypes.h"
#include "Event/SHacks.h"
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
State::State() {
  setLocation( State::LocationUnknown );
  m_z           = 0.;
  m_stateVector = TrackVector();
  m_covariance  = TrackMatrix();
}

//=============================================================================
// Retrieve the charge-over-momentum Q/P of the state
//=============================================================================
double State::qOverP() const
{
  return m_stateVector[4];
};

//=============================================================================
// Retrieve the momentum of the state
//=============================================================================
double State::p() const
{
  return (fabs(m_stateVector[4]) > TrackParameters::lowTolerance ? fabs( 1./m_stateVector[4]) : HUGE_VAL) ;
};

//=============================================================================
// Retrieve the transverse momentum of the state
//=============================================================================
double State::pt() const
{
  if (fabs(m_stateVector[4]) > TrackParameters::lowTolerance) {
    const double txy2 =   m_stateVector[2]*m_stateVector[2]
                  + m_stateVector[3]*m_stateVector[3];
    return sqrt( txy2/(1.+txy2) ) / fabs( m_stateVector[4] );
  }
  return HUGE_VAL;
};

//=============================================================================
// Retrieve the 6D covariance matrix (x,y,z,px,py,pz) of the state
//=============================================================================
SymMatrix6x6 State::posMomCovariance() const
{
  // Transformation done in 2 steps:
  // 1) "convert" first from (x,y,tx,ty,Q/p) to (x,y,z,tx,ty,Q/p)
  const TrackMatrix cov5D = covariance();
  SymMatrix6x6 cov6Dtmp = SymMatrix6x6();
  cov6Dtmp.Place_at(cov5D.Sub<2,2>(0,0),0,0);
  cov6Dtmp.Place_at(cov5D.Sub<3,3>(0,2),0,3);
  cov6Dtmp.Place_at(cov5D.Sub<3,3>(2,0),3,0);
  cov6Dtmp.Place_at(cov5D.Sub<3,3>(2,2),3,3);
 
  // 2) transformation from (x,y,z,tx,ty,Q/p) to (x,y,z,px,py,pz)
  // jacobian J = I 0
  //              0 j
  //  -> covariance matrix C = C_A  C_B.T()
  //                           C_B  C_D
  //     becomes C' = C_A   (j.C_B).T()  after similarity transformation
  //                  j.C_B j.C_D.(j.T())
  const double Tx     = tx();
  const double Ty     = ty();
  const double QOverP = qOverP();
  const double Q   = ( QOverP != 0. ? (fabs(QOverP)/QOverP) : 0. );
  const double Tx2 = Tx * Tx;
  const double Ty2 = Ty * Ty;
  const double Qp  = Q * p();
  const double N   = 1. / sqrt( 1 + Tx2 + Ty2 );
  const double N2  = N*N;

  SymMatrix6x6 cov6D = SymMatrix6x6();
  SymMatrix3x3 C_A = cov6Dtmp.Sub<3,3>(0,2);
  SymMatrix3x3 C_D = cov6Dtmp.Sub<3,3>(3,5);
  Matrix3x3    C_B = Matrix3x3();
  Matrix3x3    jmat = Matrix3x3();

  jmat(0,0) = ( 1 + Ty2 ) * N2;
  jmat(0,1) = - Tx * Ty * N2;
  jmat(0,2) = - Qp * Tx;
  jmat(1,0) = - Tx * Ty * N2;
  jmat(1,1) = ( 1 + Tx2 ) * N2;
  jmat(1,2) = - Qp * Ty;
  jmat(2,0) = - Tx * N2;
  jmat(2,1) = - Ty * N2;
  jmat(2,2) = - Qp;

  C_B(0,0) = cov6Dtmp(3,0);
  C_B(1,0) = cov6Dtmp(4,0);
  C_B(1,1) = cov6Dtmp(4,1);
  C_B(2,0) = cov6Dtmp(5,0);
  C_B(2,1) = cov6Dtmp(5,1);
  C_B(2,2) = cov6Dtmp(5,2);

  C_B = jmat * C_B;

  cov6D.Place_at(C_A,0,0);
  cov6D.Place_at(SHacks::Similarity<Matrix3x3,SymMatrix3x3>(jmat, C_D),3,3);

  cov6D(3,0) = C_B(0,0);
  cov6D(4,0) = C_B(1,0);
  cov6D(5,0) = C_B(2,0);
  cov6D(3,1) = C_B(0,1);
  cov6D(4,1) = C_B(1,1);
  cov6D(5,1) = C_B(2,1);
  cov6D(3,2) = C_B(0,2);
  cov6D(4,2) = C_B(1,2);
  cov6D(5,2) = C_B(2,2);

  return cov6D;
};

//=============================================================================
// Retrieve the squared error on the charge-over-momentum Q/P of the state
//=============================================================================
double State::errQOverP2() const
{
  return m_covariance(4,4);
};

//=============================================================================
// Retrieve the squared error on the momentum of the state
//=============================================================================
double State::errP2() const
{
 return ( fabs(m_stateVector[4]) > TrackParameters::lowTolerance ? 
          errQOverP2() / gsl_pow_4( m_stateVector[4]): 0. );
};

//=============================================================================
// Retrieve the errors on the momentum vector of the state
//=============================================================================
SymMatrix3x3 State::errMomentum() const
{
  const SymMatrix6x6 temp = posMomCovariance(); 
  return temp.Sub<3,3>(3,5);
};

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
};

//=============================================================================
// Clone the state
//=============================================================================
State* State::clone() const
{
  return new State(*this);
};

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
};

//=============================================================================
// Update the Q/P value of the state
//=============================================================================
void State::setQOverP( double value )
{
  m_stateVector[4] = value;
};

//=============================================================================

Gaudi::SymMatrix3x3 State::errPosition() const{
 const Gaudi::SymMatrix6x6 temp = posMomCovariance();
 return temp.Sub<3,3>(0,0);
};


