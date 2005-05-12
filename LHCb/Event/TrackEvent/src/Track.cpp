// $Id: Track.cpp,v 1.8 2005-05-12 14:00:59 erodrigu Exp $ // Include files

// local
#include "Event/Track.h"
#include "Event/TrackKeys.h"
#include "Event/TrackFunctor.h"

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
  physicsState().positionAndMomentum( pos, mom, cov6D );

  return StatusCode::SUCCESS;
};

//=============================================================================
// Retrieve the position and momentum vectors at the physics state
//=============================================================================
StatusCode Track::positionAndMomentum( HepPoint3D &pos,
                                       HepVector3D &mom ) const
{
  physicsState().positionAndMomentum( pos, mom );

  return StatusCode::SUCCESS;
};

//=============================================================================
// Retrieve the 3D-position (+ errors) at the physics state
//=============================================================================
StatusCode Track::position( HepPoint3D &pos,
                            HepSymMatrix &errPos ) const
{
  pos    = physicsState().position();
  errPos = physicsState().errPosition();

  return StatusCode::SUCCESS;
};

//=============================================================================
// Retrieve the slopes (dx/dz,dy/dz,1) at the physics state
//=============================================================================
StatusCode Track::slopes( HepVector3D &slopes,
                          HepSymMatrix &errSlopes ) const
{
  slopes    = physicsState().slopes();
  errSlopes = physicsState().errSlopes();

  return StatusCode::SUCCESS;
};

//=============================================================================
// Retrieve the momentum at the physics state
//=============================================================================
double Track::p() const
{
  return physicsState().p();
};

//=============================================================================
// Retrieve the transverse momentum at the physics state
//=============================================================================
double Track::pt() const
{
  return physicsState().pt();
};

//=============================================================================
// Retrieve the momentum vector (+ errors) at the physics state
//=============================================================================
StatusCode Track::momentum( HepVector3D &mom,
                            HepSymMatrix &errMom ) const
{
  mom    = physicsState().momentum();
  errMom = physicsState().errMomentum();

  return StatusCode::SUCCESS;
};

//=============================================================================
// Retrieve the 6D covariance matrix (x,y,z,px,py,pz) at the physics state
//=============================================================================
StatusCode Track::posMomCovariance( HepSymMatrix &cov6D ) const
{
  cov6D = physicsState().posMomCovariance();

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
  return *best;
};

//=============================================================================
// check the existence of a state at a certain predefined location
//=============================================================================
bool Track::hasStateAt( unsigned int location ) const
{
  bool ok = false;
  for (unsigned int i = 0 ; i < m_states.size(); i++)
    if (m_states[i]-> checkLocation(location) ) return true;
  return ok;
};

//=============================================================================
// Retrieve the pointer to the state closest to the given plane
//=============================================================================
State& Track::stateAt( unsigned int location )
{
  if (!hasStateAt(location))
    throw GaudiException( "There is no state at requested location",
                          "Track.cpp",
                          StatusCode::FAILURE );
  unsigned int index = 0;
  for (unsigned int i = 0 ; i < m_states.size(); i++)
    if (m_states[i]-> checkLocation(location) ) index = i;

  return *m_states[index];
};

//=============================================================================
// Retrieve the (const) pointer to the state at a given location
//=============================================================================
const State& Track::stateAt( unsigned int location ) const
{
  if (!hasStateAt(location))
    throw GaudiException( "There is no state at requested location",
                          "Track.cpp",
                          StatusCode::FAILURE );
  unsigned int index = 0;
  for (unsigned int i = 0 ; i < m_states.size(); i++)
    if (m_states[i]-> checkLocation(location) ) index = i;
  
  return *m_states[index];
};

//=============================================================================
// reset the track
//=============================================================================
void Track::reset() 
{

  m_chi2PerDoF = 0;
  m_nDoF       = 0;
  m_flags      = 0;
  m_lhcbIDs.clear();
  for (std::vector<State*>::iterator it = m_states.begin();
       it != m_states.end(); it++) delete *it;
  for (std::vector<Measurement*>::iterator it2 = m_measurements.begin();
       it2 != m_measurements.end(); it2++) delete *it2;
  for (std::vector<Node*>::iterator it3 = m_nodes.begin();
       it3 != m_nodes.end(); it3++) delete *it3;
  m_states.clear();
  m_measurements.clear();
  m_nodes.clear();
  m_ancestors.clear();
};

//=============================================================================
// Clone the track
//=============================================================================
Track* Track::clone() const
{
  Track* tr = new Track();
  tr->setChi2PerDoF( chi2PerDoF() );
  tr->setNDoF( nDoF() );
  tr->setFlags( flags() );
  tr->setLhcbIDs( lhcbIDs() );
  for (std::vector<State*>::const_iterator it = m_states.begin();
       it != m_states.end(); it++) tr->addToStates( *(*it), false );
  for (std::vector<Measurement*>::const_iterator it2 = m_measurements.begin();
       it2 != m_measurements.end(); it2++) 
    tr->addToMeasurements( *(*it2), false );
  for (std::vector<Node*>::const_iterator it3 = m_nodes.begin();
       it3 != m_nodes.end(); it3++) tr->addToNodes( (*it3)->clone() );
  for (std::vector<Track*>::const_iterator it4 = m_ancestors.begin();
       it4 != m_ancestors.end();  it4++) tr->addToAncestors( *(*it4) );
  return tr;
};

//=============================================================================
// Clone the track
//=============================================================================

void Track::addToStates(const State& state, bool inOrder) 
{
  State* local = state.clone();
  m_states.push_back(local);
  if (!inOrder) return;
  int order = checkFlag(TrackKeys::Backward) ? -1 : 1;
  std::vector<State*>::iterator i = 
    std::upper_bound(m_states.begin(),
                     m_states.end(),
                     local,  
                     TrackFunctor::orderByZ<State>(order));
  m_states.insert(i,local);    
}
void Track::addToMeasurements(const Measurement& meas, bool inOrder) 
{
  Measurement* local = meas.clone();
  m_measurements.push_back(local);
  if (!inOrder) return;
  int order = checkFlag(TrackKeys::Backward) ? -1 : 1;
  std::vector<Measurement*>::iterator i = 
    std::upper_bound(m_measurements.begin(),
                     m_measurements.end(),
                     local,  
                     TrackFunctor::orderByZ<Measurement>(order));
  m_measurements.insert(i,local);
}
//=============================================================================

