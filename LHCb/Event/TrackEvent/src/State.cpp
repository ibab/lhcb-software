// $Id: State.cpp,v 1.1.1.1 2005-02-10 17:23:12 erodrigu Exp $

// local
#include "Event/State.h"

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
  HepSymMatrix cov = covariance();
  // to be written ...

  return cov;  
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
