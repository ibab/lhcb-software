// $Id: FitTrack.cpp,v 1.1.1.1 2005-04-07 20:24:28 erodrigu Exp $
// Include files
// -------------

// local
#include "Event/FitTrack.h"
#include "Event/MeasurementFunctor.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FitTrack
//
// 2005-04-05 : Eduardo Rodrigues
//-----------------------------------------------------------------------------

//=============================================================================
// Sort the measurements by increasing z
//=============================================================================
void FitTrack::sortMeasurements() {
  std::sort( m_measurements.begin(),
             m_measurements.end(),
             MeasurementFunctor::increasingByZ() );
};

//=============================================================================
// Retrieve the reference to the state closest to the given z-position
//=============================================================================
State & FitTrack::closestState( double z )
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
const State & FitTrack::closestState( double z ) const
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
State & FitTrack::closestState( const HepPlane3D &plane )
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
const State & FitTrack::closestState( const HepPlane3D &plane ) const
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
bool FitTrack::hasStateAt( const State::Location& value ) const
{
  if ( NULL != stateAt(value) ) return true;
  return false;
};

//=============================================================================
// Retrieve the pointer to the state closest to the given plane
//=============================================================================
State* FitTrack::stateAt( const State::Location& value )
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
const State* FitTrack::stateAt( const State::Location& value ) const
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
FitTrack* FitTrack::clone() const
{

};

//=============================================================================
// Check whether the track was produced by a given algorithm
//=============================================================================
bool FitTrack::producedByAlgo( const HistoryFlag& value ) const
{
  unsigned int val = (unsigned int)value;
  return 0 != ( m_flags & historyMask & val );
};

//=============================================================================
// Update the name of the algorithm that produced the track
//=============================================================================
void FitTrack::setProducedByAlgo( const HistoryFlag& value )
{
  unsigned int val = (unsigned int)value;
  m_flags &= ~historyMask;
  m_flags |= ((((unsigned int)val) << historyBits) & historyMask);
};

//=============================================================================
