// $Id: Track.cpp,v 1.4 2005-04-13 17:24:56 hernando Exp $ // Include files

// local
#include "Event/Track.h"
#include "Event/TrackKeys.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Track
//
// 2004-12-14 : Jose Hernando, Eduardo Rodrigues
//-----------------------------------------------------------------------------


//=============================================================================
// Retrieve the position and momentum vectors and the corresponding
// 6D covariance matrix (pos:1->3,mom:4-6) at the physics state
//=============================================================================
StatusCode Track::positionAndMomentum( HepPoint3D &pos,
                                       HepVector3D &mom,
                                       HepSymMatrix &cov6D ) const
{
  m_physicsState.positionAndMomentum( pos, mom, cov6D );

  return StatusCode::SUCCESS;
};

//=============================================================================
// Retrieve the position and momentum vectors at the physics state
//=============================================================================
StatusCode Track::positionAndMomentum( HepPoint3D &pos,
                                       HepVector3D &mom ) const
{
  m_physicsState.positionAndMomentum( pos, mom );

  return StatusCode::SUCCESS;
};

//=============================================================================
// Retrieve the 3D-position (+ errors) at the physics state
//=============================================================================
StatusCode Track::position( HepPoint3D &pos,
                            HepSymMatrix &errPos ) const
{
  pos    = m_physicsState.position();
  errPos = m_physicsState.errPosition();

  return StatusCode::SUCCESS;
};

//=============================================================================
// Retrieve the slopes (dx/dz,dy/dz,1) at the physics state
//=============================================================================
StatusCode Track::slopes( HepVector3D &slopes,
                          HepSymMatrix &errSlopes ) const
{
  slopes    = m_physicsState.slopes();
  errSlopes = m_physicsState.errSlopes();

  return StatusCode::SUCCESS;
};

//=============================================================================
// Retrieve the momentum at the physics state
//=============================================================================
double Track::p() const
{
  return m_physicsState.p();
};

//=============================================================================
// Retrieve the transverse momentum at the physics state
//=============================================================================
double Track::pt() const
{
  return m_physicsState.pt();
};

//=============================================================================
// Retrieve the momentum vector (+ errors) at the physics state
//=============================================================================
StatusCode Track::momentum( HepVector3D &mom,
                            HepSymMatrix &errMom ) const
{
  mom    = m_physicsState.momentum();
  errMom = m_physicsState.errMomentum();

  return StatusCode::SUCCESS;
};

//=============================================================================
// Retrieve the 6D covariance matrix (x,y,z,px,py,pz) at the physics state
//=============================================================================
StatusCode Track::posMomCovariance( HepSymMatrix &cov6D ) const
{
  cov6D = m_physicsState.posMomCovariance();

  return StatusCode::SUCCESS;
};

//=============================================================================
// Retrieve the reference to the state closest to the given z-position
//=============================================================================
State & Track::closestState( double z )
{
  double minDist = 999999999.;
  State* best = 0;
  for ( std::vector<State*>::iterator it = m_states.begin() ;
        m_states.end() != it; it++ ) {
    if ( minDist > fabs( z - (*it)->z() ) ) {
      minDist = fabs( z-(*it)->z() );
      best    = *it;
    }
  }
  if ( fabs(z - m_physicsState.z()) < minDist ) return m_physicsState;
  return *best;
};

//=============================================================================
// Retrieve the (const) reference to the state closest to the given z-position
//=============================================================================
const State & Track::closestState( double z ) const
{
  double minDist = 999999999.;
  State* best = 0;
  for ( std::vector<State*>::const_iterator it = m_states.begin() ;
        m_states.end() != it; it++ ) {
    if ( minDist > fabs( z - (*it)->z() ) ) {
      minDist = fabs( z-(*it)->z() );
      best    = *it;
    }
  }
  if ( fabs(z - m_physicsState.z()) < minDist ) return m_physicsState;
  return *best;
};

//=============================================================================
// Retrieve the reference to the state closest to the given plane
//=============================================================================
State & Track::closestState( const HepPlane3D &plane )
{
  double minDist = 999999999.;
  double dist;
  State* best = 0;
  for ( std::vector<State*>::iterator it = m_states.begin() ;
        m_states.end() != it; it++ ) {
    dist = plane.distance( ((*it) -> position()) );
    if ( minDist > dist ) {
      minDist = dist;
      best    = *it;
    }
  }
  if ( fabs( plane.distance(m_physicsState.position())) < minDist ) return m_physicsState;
  return *best;
};

//=============================================================================
// Retrieve the (const) reference to the state closest to the given plane
//=============================================================================
const State & Track::closestState( const HepPlane3D &plane ) const
{
  double minDist = 999999999.;
  double dist;
  State* best = 0;
  for ( std::vector<State*>::const_iterator it = m_states.begin() ;
        m_states.end() != it; it++ ) {
    dist = plane.distance( ((*it) -> position()) );
    if ( minDist > dist ) {
      minDist = dist;
      best    = *it;
    }
  }
  if ( fabs( plane.distance(m_physicsState.position()) ) < minDist )
    return m_physicsState;

  return *best;
};

//=============================================================================
// check the existence of a state at a certain predefined location
//=============================================================================
bool Track::hasStateAt( const State::Location& value ) const
{
  if ( NULL != stateAt(value) ) return true;
  return false;
};

//=============================================================================
// Retrieve the pointer to the state closest to the given plane
//=============================================================================
State* Track::stateAt( const State::Location& value )
{
  for ( std::vector<State*>::iterator it = m_states.begin() ;
        m_states.end() != it; it++ ) {
    if ( (*it) -> checkLocation( value ) ) return (*it);
  }
  if  (m_physicsState.checkLocation(value)) return &m_physicsState;
  return NULL;
};

//=============================================================================
// Retrieve the (const) pointer to the state at a given location
//=============================================================================
const State* Track::stateAt( const State::Location& value ) const
{
  for ( std::vector<State*>::const_iterator it = m_states.begin() ;
        m_states.end() != it; it++ ) {
    if ( (*it) -> checkLocation( value ) ) return (*it);
  }
  if  (m_physicsState.checkLocation(value)) return &m_physicsState;
  return NULL;
};

//=============================================================================
// Clone the track
//=============================================================================
Track* Track::clone() const
{
  Track* tk = new Track();
  *tk = *this;
  return tk;
};

//=============================================================================

void Track::setHistory(const std::string& key )
{
  unsigned int val = Keys::index(Keys::Track::History,key);
  m_flags &= ~historyMask;
  m_flags |= ((((unsigned int)val) << historyBits) & historyMask);
};

const std::string& Track::history() const
{
  unsigned int val = ( m_flags & historyMask) >> historyBits;
  return Keys::key(Keys::Track::History,val);
};

void Track::setHistoryFit( const std::string& key )
{
  unsigned int val = Keys::index(Keys::Track::HistoryFit,key);
  m_flags &= ~historyFitMask;
  m_flags |= ((((unsigned int)val) << historyFitBits) & historyFitMask);
};

const std::string& Track::historyFit() const
{
  unsigned int val = ( m_flags & historyFitMask) >> historyFitBits;
  return Keys::key(Keys::Track::HistoryFit,val);
};

void Track::setType( const std::string& key)
{
  unsigned int val = Keys::index(Keys::Track::Types,key);
  m_flags &= ~typeMask;
  m_flags |= ((((unsigned int)val) << typeBits) & typeMask);
};

const std::string& Track::type() const
{
  unsigned int val = ( m_flags & typeMask) >> typeBits;
  return Keys::key(Keys::Track::Types,val);
}

void Track::setStatus( const std::string& key)
{
  unsigned int val = Keys::index(Keys::Track::Status,key);
  m_flags &= ~statusMask;
  m_flags |= ((((unsigned int)val) << statusBits) & statusMask);
};

const std::string& Track::status() const
{
  unsigned int val = ( m_flags & statusMask) >> statusBits;
  return Keys::key(Keys::Track::Status,val);
}

void Track::setFlag(const std::string& flag, bool ok)
{
  unsigned int value = Keys::bitIndex(Keys::Track::Flags,flag);
  unsigned int val = (((unsigned int) value) << flagBits) & flagMask;
  if (ok) m_flags |= val;
  else m_flags &= ~val;
}

bool Track::checkFlag(const std::string& flag) const
{
  unsigned int value = Keys::bitIndex(Keys::Track::Flags,flag);
  unsigned int val = ((unsigned int)value << flagBits);  
  bool ok = (0 != ( m_flags & flagMask & val ));
  return ok;
}

