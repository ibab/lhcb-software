// $Id: State.cpp,v 1.2 2005-02-23 18:13:23 erodrigu Exp $

// local
#include "Event/State.h"

#include "CLHEP/Matrix/Matrix.h"

//-----------------------------------------------------------------------------
// Implementation file for class : State
//
// 2004-12-14 : Jose Hernando, Eduardo Rodrigues
//-----------------------------------------------------------------------------

//=============================================================================
// Default constructor. State defined to be of type State::HasMomentum
//=============================================================================
State::State() {
  setType( State::HasMomentum );
  setLocation( State::Unknown );
  m_z          = 0.;
  m_state      = HepVector(5,0);
  m_covariance = HepSymMatrix(5,0);
}

//=============================================================================
// Retrieve the charge-over-momentum Q/P of the state
//=============================================================================
double State::qOverP() const
{
  return m_state[4];
};

//=============================================================================
// Retrieve the momentum of the state
//=============================================================================
double State::p() const
{
  if ( m_state[4] != 0. ) return fabs( 1./m_state[4] );
  return 0.;
};

//=============================================================================
// Retrieve the transverse momentum of the state
//=============================================================================
double State::pt() const
{
  if ( m_state[4] != 0. ) {
    double txy2 = m_state[2]*m_state[2] + m_state[3]*m_state[3];
    return sqrt( txy2/(1.+txy2) ) / fabs( m_state[4] );
  }
  return 0.;
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

  for ( int j=0 ; j<5 ; j++ ) {
    for ( int i=j ; i<5 ; i++ ) {
      cov6Dtmp.fast(index[i]+1,index[j]+1) = cov5D.fast(i+1,j+1);
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
  HepMatrix    j   = HepMatrix(3,3,0);

  j[0][0] = ( 1 + Ty2 ) * N2;
  j[0][1] = - Tx * Ty * N2;
  j[0][2] = - Qp * Tx;
  j[1][0] = - Tx * Ty * N2;
  j[1][1] = ( 1 + Tx2 ) * N2;
  j[1][2] = - Qp * Ty;
  j[2][0] = - Tx * N2;
  j[2][1] = - Ty * N2;
  j[2][2] = - Qp;

  C_B(1,1) = cov6Dtmp.fast(4,1);
  C_B(2,1) = cov6Dtmp.fast(5,1);
  C_B(2,2) = cov6Dtmp.fast(5,2);
  C_B(3,1) = cov6Dtmp.fast(6,1);
  C_B(3,2) = cov6Dtmp.fast(6,2);
  C_B(3,3) = cov6Dtmp.fast(6,3);

  C_B = j * C_B;

  cov6D.sub(1,C_A);
  cov6D.sub(4,C_D.similarity(j));
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
  if ( m_state[4] != 0. ) return errQOverP2() / pow( m_state[4], 4. );
  return 0.;
};

//=============================================================================
// Retrieve the errors on the momentum vector of the state
//=============================================================================
HepSymMatrix State::errMomentum() const
{
  if ( checkType( State::HasMomentum ) ) {
    const HepSymMatrix temp = posMomCovariance(); // CLHEP 1.9, must be const
    return temp.sub(4,6);
  }
  else {
    return HepSymMatrix(3,0);
  }
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
       (pow(transSlope,3.)*norm))

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
// Clear the state before re-using it
//=============================================================================
void State::reset()
{
  m_z          = 0.;
  m_state      = HepVector(5,0);
  m_covariance = HepSymMatrix(5,0);
};

//=============================================================================
// Update the state vector (presumably of type State::HasMomentum)
//=============================================================================
void State::setState( double x, double y, double z,
                        double tx, double ty,
                        double qOverP )
{
  m_state[0] = x;
  m_state[1] = y;
  m_state[2] = tx;
  m_state[3] = ty;
  m_z        = z;
  if ( checkType( State::StraightLine ) ) {
    std::cerr
      << "ERROR   You're trying to set the Q/P value for a state of type State::StraightLine!"
      << "ERROR   This value will be discarded." << std::endl;
  }
  else {
     m_state[4] = qOverP;
  }
};

//=============================================================================
// Update the Q/P value of the state
//=============================================================================
void State::setQOverP( double value )
{
  m_state[4] = value;
};

//=============================================================================
// Update State type
//=============================================================================
void State::setType( const Type& value)
{
  unsigned int val = (unsigned int)value;
  m_flags &= ~typeMask;
  m_flags |= ((((unsigned int)val) << typeBits) & typeMask);
};

//=============================================================================
