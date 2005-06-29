// $Id: Track.cpp,v 1.12 2005-06-29 13:11:48 erodrigu Exp $ // Include files

// local
#include "Event/Track.h"
#include "Event/TrackKeys.h"
#include "Event/TrackFunctor.h"
#include <functional>

//-----------------------------------------------------------------------------
// Implementation file for class : Track
//
// 2004-12-14 : Jose Hernando, Eduardo Rodrigues
//-----------------------------------------------------------------------------


//=============================================================================
// Retrieve the position and momentum vectors and the corresponding
// 6D covariance matrix (pos:1->3,mom:4-6) at the physics state
//=============================================================================
void Track::positionAndMomentum( HepPoint3D &pos,
                                 HepVector3D &mom,
                                 HepSymMatrix &cov6D ) const
{
  firstState().positionAndMomentum( pos, mom, cov6D );
};

//=============================================================================
// Retrieve the position and momentum vectors at the physics state
//=============================================================================
void Track::positionAndMomentum( HepPoint3D &pos,
                                       HepVector3D &mom ) const
{
  firstState().positionAndMomentum( pos, mom );
};

//=============================================================================
// Retrieve the 3D-position (+ errors) at the physics state
//=============================================================================
void Track::position( HepPoint3D &pos,
                            HepSymMatrix &errPos ) const
{
  pos    = firstState().position();
  errPos = firstState().errPosition();
};

//=============================================================================
// Retrieve the slopes (dx/dz,dy/dz,1) at the physics state
//=============================================================================
void Track::slopes( HepVector3D &slopes,
                          HepSymMatrix &errSlopes ) const
{
  slopes    = firstState().slopes();
  errSlopes = firstState().errSlopes();
};

//=============================================================================
// Retrieve the momentum at the physics state
//=============================================================================
double Track::p() const
{
  return firstState().p();
};

//=============================================================================
// Retrieve the transverse momentum at the physics state
//=============================================================================
double Track::pt() const
{
  return firstState().pt();
};

//=============================================================================
// Retrieve the momentum vector (+ errors) at the physics state
//=============================================================================
void Track::momentum( HepVector3D &mom,
                            HepSymMatrix &errMom ) const
{
  mom    = firstState().momentum();
  errMom = firstState().errMomentum();
};

//=============================================================================
// Retrieve the 6D covariance matrix (x,y,z,px,py,pz) at the physics state
//=============================================================================
void Track::posMomCovariance( HepSymMatrix &cov6D ) const
{
  cov6D = firstState().posMomCovariance();
};

//=============================================================================
// Retrieve the reference to the state closest to the given z-position
//=============================================================================
State & Track::closestState( double z )
{
  std::vector<State*>::iterator iter = 
    std::max_element(m_states.begin(),m_states.end(),
                     TrackFunctor::closestToZ<State>(z));
  if (iter == m_states.end())
    throw GaudiException( "No state closest to z","Track.cpp",
                          StatusCode::FAILURE );
  return *(*iter);
};

//=============================================================================
// Retrieve the (const) reference to the state closest to the given z-position
//=============================================================================
const State & Track::closestState( double z ) const
{
  std::vector<State*>::const_iterator iter = 
    std::max_element(m_states.begin(),m_states.end(),
                     TrackFunctor::closestToZ<State>(z));
  if (iter == m_states.end())
    throw GaudiException( "No state closest to z","Track.cpp",
                          StatusCode::FAILURE );
  return *(*iter);
};

//=============================================================================
// Retrieve the reference to the state closest to the given plane
//=============================================================================
State & Track::closestState( const HepPlane3D &plane )
{
  std::vector<State*>::iterator iter = 
    std::max_element(m_states.begin(),m_states.end(),
                     TrackFunctor::closestToPlane<State>(plane));
  if (iter == m_states.end())
    throw GaudiException( "No state closest to plane","Track.cpp",
                          StatusCode::FAILURE );
  return *(*iter);
};

//=============================================================================
// Retrieve the (const) reference to the state closest to the given plane
//=============================================================================
const State & Track::closestState( const HepPlane3D &plane ) const
{
  std::vector<State*>::const_iterator iter = 
    std::max_element(m_states.begin(),m_states.end(),
                     TrackFunctor::closestToPlane<State>(plane));
  if (iter == m_states.end())
    throw GaudiException( "No state closest to plane","Track.cpp",
                          StatusCode::FAILURE );
  return *(*iter);
};

//=============================================================================
// check the existence of a state at a certain predefined location
//=============================================================================
bool Track::hasStateAt( unsigned int location ) const
{
  std::vector<State*>::const_iterator iter =
    std::find_if(m_states.begin(),m_states.end(),
                 std::bind2nd(std::mem_fun(&State::checkLocation),location));
  // the last line should be equivalent to:
  //             TrackFunctor::HasKey<State>(&State::checkLocation,location));
  return (iter != m_states.end());
};

//=============================================================================
// Retrieve the pointer to the state closest to the given plane
//=============================================================================
State& Track::stateAt( unsigned int location )
{
  std::vector<State*>::iterator iter = 
    std::find_if(m_states.begin(),m_states.end(),
                 TrackFunctor::HasKey<State>(&State::checkLocation,location));
  if (iter == m_states.end())
    throw GaudiException( "There is no state at requested location",
                          "Track.cpp",
                          StatusCode::FAILURE );
  return *(*iter);
};

//=============================================================================
// Retrieve the (const) pointer to the state at a given location
//=============================================================================
const State& Track::stateAt( unsigned int location ) const
{
  std::vector<State*>::const_iterator iter = 
    std::find_if(m_states.begin(),m_states.end(),
                 TrackFunctor::HasKey<State>(&State::checkLocation,location));
  if (iter == m_states.end())
    throw GaudiException( "There is no state at requested location",
                          "Track.cpp",
                          StatusCode::FAILURE );
  return *(*iter);
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
// Clone the track keeping the key
//=============================================================================
Track* Track::cloneWithKey( ) const
{
  int theKey = this -> key();
  Track* tr = new Track( theKey );
  tr->setChi2PerDoF( chi2PerDoF() );
  tr->setNDoF( nDoF() );
  tr->setFlags( flags() );
  tr->setLhcbIDs( lhcbIDs() );
  for (std::vector<State*>::const_iterator it = m_states.begin();
       it != m_states.end(); it++) tr->addToStates( *(*it));
  for (std::vector<Measurement*>::const_iterator it2 = m_measurements.begin();
       it2 != m_measurements.end(); it2++) 
    tr->addToMeasurements( *(*it2) );
  for (std::vector<Node*>::const_iterator it3 = m_nodes.begin();
       it3 != m_nodes.end(); it3++) tr->addToNodes( (*it3)->clone() );
  for (SmartRefVector<Track>::const_iterator it4 = m_ancestors.begin();
       it4 != m_ancestors.end();  it4++) tr->addToAncestors(*(*it4));
  
  return tr;
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
       it != m_states.end(); it++) tr->addToStates( *(*it));
  for (std::vector<Measurement*>::const_iterator it2 = m_measurements.begin();
       it2 != m_measurements.end(); it2++) 
    tr->addToMeasurements( *(*it2) );
  for (std::vector<Node*>::const_iterator it3 = m_nodes.begin();
       it3 != m_nodes.end(); it3++) tr->addToNodes( (*it3)->clone() );
  for (SmartRefVector<Track>::const_iterator it4 = m_ancestors.begin();
       it4 != m_ancestors.end();  it4++) tr->addToAncestors(*(*it4));
  
  return tr;
};

//=============================================================================
// 
//=============================================================================
void Track::addToStates(const State& state) 
{
  State* local = state.clone();
  int order = checkFlag(TrackKeys::Backward) ? -1 : 1;
  std::vector<State*>::iterator i = 
    std::upper_bound(m_states.begin(),
                     m_states.end(),
                     local,  
                     TrackFunctor::orderByZ<State>(order));
  m_states.insert(i,local);    
}

//=============================================================================
void Track::addToMeasurements(const Measurement& meas) 
{
  const LHCbID& id = meas.lhcbID();
  if (std::find(m_lhcbIDs.begin(),m_lhcbIDs.end(),id) == m_lhcbIDs.end())
    addToLhcbIDs(id);
  Measurement* local = meas.clone();
  int order = checkFlag(TrackKeys::Backward) ? -1 : 1;
  std::vector<Measurement*>::iterator i = 
    std::upper_bound(m_measurements.begin(),
                     m_measurements.end(),
                     local,  
                     TrackFunctor::orderByZ<Measurement>(order));
  m_measurements.insert(i,local);
}
//=============================================================================

void Track::removeFromMeasurements(Measurement* meas) 
{
  const LHCbID& id = meas->lhcbID();
  removeFromLhcbIDs(id);
  TrackFunctor::deleteFromList<Measurement>(m_measurements,meas);
}

void Track::removeFromNodes(Node* node) 
{
  TrackFunctor::deleteFromList<Node>(m_nodes,node);
}

void Track::removeFromStates(State* state) 
{
  TrackFunctor::deleteFromList<State>(m_states,state);
}
