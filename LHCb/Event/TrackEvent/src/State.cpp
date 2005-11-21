// $Id: State.cpp,v 1.9 2005-11-21 10:38:02 jvantilb Exp $

#include <math.h>
#include <gsl/gsl_math.h>

// local
#include "Event/State.h"

#include "CLHEP/Matrix/Matrix.h"

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
  m_stateVector = HepVector(5,0);
  m_covariance  = HepSymMatrix(5,0);
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
  if ( m_stateVector[4] != 0. ) return fabs( 1./m_stateVector[4] );
  return HUGE_VAL;
};

//=============================================================================
// Retrieve the transverse momentum of the state
//=============================================================================
double State::pt() const
{
  if ( m_stateVector[4] != 0. ) {
    double txy2 =   m_stateVector[2]*m_stateVector[2]
                  + m_stateVector[3]*m_stateVector[3];
    return sqrt( txy2/(1.+txy2) ) / fabs( m_stateVector[4] );
  }
  return HUGE_VAL;
};

//=============================================================================
// Retrieve the 6D covariance matrix (x,y,z,px,py,pz) of the state
//=============================================================================
HepSymMatrix State::posMomCovariance() const
{
  // Transformation done in 2 steps:
  // 1) "convert" first from (x,y,tx,ty,Q/p) to (x,y,z,tx,ty,Q/p)
  const HepSymMatrix cov5D = covariance();
  HepSymMatrix cov6Dtmp    = HepSymMatrix(6,0);

  std::vector<int> index;
  index.push_back( 1 );
  index.push_back( 3 );
  index.push_back( 4 );
  index.push_back( 5 );

  for ( int jj=0 ; jj<5 ; ++jj ) {
    for ( int i=jj ; i<5 ; ++i ) {
      cov6Dtmp.fast(index[i]+1,index[jj]+1) = cov5D.fast(i+1,jj+1);
    }
  }
  cov6Dtmp.fast(3,1) = 0.;
  cov6Dtmp.fast(3,2) = 0.;
  cov6Dtmp.fast(3,3) = 0.;
  cov6Dtmp.fast(4,3) = 0.;
  cov6Dtmp.fast(5,3) = 0.;
  cov6Dtmp.fast(6,3) = 0.;

  // 2) transformation from (x,y,z,tx,ty,Q/p) to (x,y,z,px,py,pz)
  // jacobian J = I 0
  //              0 j
  //  -> covariance matrix C = C_A  C_B.T()
  //                           C_B  C_D
  //     becomes C' = C_A   (j.C_B).T()  after similarity transformation
  //                  j.C_B j.C_D.(j.T())
  double Tx     = tx();
  double Ty     = ty();
  double QOverP = qOverP();
  double Q   = ( QOverP != 0. ? (fabs(QOverP)/QOverP) : 0. );
  double Tx2 = Tx * Tx;
  double Ty2 = Ty * Ty;
  double Qp  = Q * p();
  double N   = 1. / sqrt( 1 + Tx2 + Ty2 );
  double N2  = N*N;

  HepSymMatrix cov6D = HepSymMatrix(6,0);
  HepSymMatrix C_A = cov6Dtmp.sub(1,3);
  HepSymMatrix C_D = cov6Dtmp.sub(4,6);
  HepMatrix    C_B = HepMatrix(3,3,0);
  HepMatrix    jmat   = HepMatrix(3,3,0);

  jmat[0][0] = ( 1 + Ty2 ) * N2;
  jmat[0][1] = - Tx * Ty * N2;
  jmat[0][2] = - Qp * Tx;
  jmat[1][0] = - Tx * Ty * N2;
  jmat[1][1] = ( 1 + Tx2 ) * N2;
  jmat[1][2] = - Qp * Ty;
  jmat[2][0] = - Tx * N2;
  jmat[2][1] = - Ty * N2;
  jmat[2][2] = - Qp;

  C_B(1,1) = cov6Dtmp.fast(4,1);
  C_B(2,1) = cov6Dtmp.fast(5,1);
  C_B(2,2) = cov6Dtmp.fast(5,2);
  C_B(3,1) = cov6Dtmp.fast(6,1);
  C_B(3,2) = cov6Dtmp.fast(6,2);
  C_B(3,3) = cov6Dtmp.fast(6,3);

  C_B = jmat * C_B;

  cov6D.sub(1,C_A);
  cov6D.sub(4,C_D.similarity(jmat));
  cov6D.fast(4,1) = C_B(1,1);
  cov6D.fast(5,1) = C_B(2,1);
  cov6D.fast(6,1) = C_B(3,1);
  cov6D.fast(4,2) = C_B(1,2);
  cov6D.fast(5,2) = C_B(2,2);
  cov6D.fast(6,2) = C_B(3,2);
  cov6D.fast(4,3) = C_B(1,3);
  cov6D.fast(5,3) = C_B(2,3);
  cov6D.fast(6,3) = C_B(3,3);

  return cov6D;
};

//=============================================================================
// Retrieve the squared error on the charge-over-momentum Q/P of the state
//=============================================================================
double State::errQOverP2() const
{
  return m_covariance.fast(5,5);
};

//=============================================================================
// Retrieve the squared error on the momentum of the state
//=============================================================================
double State::errP2() const
{
  if ( m_stateVector[4] != 0. )
    return errQOverP2() / gsl_pow_4( m_stateVector[4] );
  return 0.;
};

//=============================================================================
// Retrieve the errors on the momentum vector of the state
//=============================================================================
HepSymMatrix State::errMomentum() const
{
//  if ( m_stateVector[4] != 0. ) {
    const HepSymMatrix temp = posMomCovariance(); // CLHEP 1.9, must be const
    return temp.sub(4,6);
//  }
//  else {
//    return HepSymMatrix(3,0);
//  }
};

//=============================================================================
// Retrieve the squared error on the Q/Pperp of the state
//=============================================================================
double State::errQOverPperp2() const
{
  double tx2        = tx() * tx();
  double ty2        = ty() * ty();
  double qOverP2    = qOverP() * qOverP();
  double transSlope = 1. + tx2;
  double norm       = 1 + tx2 + ty2;

  double QOverPperpError = ( (norm/transSlope) * m_covariance[4][4] )

    + ( qOverP2 * tx2 * ty2*ty2 * m_covariance[2][2]/
       (gsl_pow_3(transSlope)*norm))

    + ( qOverP2 * ty2 * m_covariance[3][3] / (norm*transSlope) )

    - ( 2. * qOverP() * tx() * ty2 * m_covariance[2][4]
        / ( transSlope*transSlope ) )

    + 2. * qOverP() * ty() * m_covariance[3][4] / transSlope

    - 2. * ( qOverP2 * tx() * ty() * ty2 * m_covariance[2][3]
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
