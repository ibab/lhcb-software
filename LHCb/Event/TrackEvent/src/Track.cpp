// $Id: Track.cpp,v 1.34 2006-09-22 12:25:51 mneedham Exp $ // Include files

// local
#include "Event/Track.h"
#include "Event/TrackFunctor.h"
#include <functional>
#include <string>

using namespace Gaudi;
using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : Track
//
// 2004-12-14 : Jose Hernando, Eduardo Rodrigues
//-----------------------------------------------------------------------------


//=============================================================================
// Retrieve the position and momentum vectors and the corresponding
// 6D covariance matrix (pos:1->3,mom:4-6) at the physics state
//=============================================================================
void Track::positionAndMomentum( XYZPoint &pos,
                                 XYZVector &mom,
                                 SymMatrix6x6& cov6D ) const
{
  firstState().positionAndMomentum( pos, mom, cov6D );
};

//=============================================================================
// Retrieve the position and momentum vectors at the physics state
//=============================================================================
void Track::positionAndMomentum( XYZPoint& pos,
                                 XYZVector& mom ) const
{
  firstState().positionAndMomentum( pos, mom );
};

//=============================================================================
// Retrieve the 3D-position (+ errors) at the physics state
//=============================================================================
void Track::position( XYZPoint &pos,
                      SymMatrix3x3 &errPos ) const
{
  pos    = firstState().position();
  errPos = firstState().errPosition();
};

//=============================================================================
// Retrieve the slopes (dx/dz,dy/dz,1) at the physics state
//=============================================================================
void Track::slopes( XYZVector& slopes,
                    SymMatrix3x3& errSlopes ) const
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
void Track::momentum( XYZVector &mom,
                      SymMatrix3x3 &errMom ) const
{
  mom    = firstState().momentum();
  errMom = firstState().errMomentum();
};

//=============================================================================
// Retrieve the 6D covariance matrix (x,y,z,px,py,pz) at the physics state
//=============================================================================
void Track::posMomCovariance( SymMatrix6x6& cov6D ) const
{
  cov6D = firstState().posMomCovariance();
};

//=============================================================================
// Retrieve the reference to the state closest to the given z-position
//=============================================================================
State& Track::closestState( double z )
{
  if ( !m_nodes.empty() ) {
    std::vector<Node*>::iterator iter = 
      std::min_element( m_nodes.begin(),m_nodes.end(),
                        TrackFunctor::distanceAlongZ<Node>(z) );
    if ( iter == m_nodes.end() )
      throw GaudiException( "No state closest to z","Track.cpp",
                            StatusCode::FAILURE );
    return (*iter)->state();    
  } else {
    std::vector<State*>::iterator iter = 
      std::min_element( m_states.begin(),m_states.end(),
                        TrackFunctor::distanceAlongZ<State>(z) );
    if ( iter == m_states.end() )
      throw GaudiException( "No state closest to z","Track.cpp",
                            StatusCode::FAILURE );
    return *(*iter);
  }
};

//=============================================================================
// Retrieve the (const) reference to the state closest to the given z-position
//=============================================================================
const State & Track::closestState( double z ) const
{
  if ( !m_nodes.empty() ) {
    std::vector<Node*>::const_iterator iter = 
      std::min_element( m_nodes.begin(),m_nodes.end(),
                        TrackFunctor::distanceAlongZ<Node>(z) );
    if ( iter == m_nodes.end() )
      throw GaudiException( "No state closest to z","Track.cpp",
                            StatusCode::FAILURE );
    return (*iter)->state();    
  } else {
    std::vector<State*>::const_iterator iter = 
      std::min_element( m_states.begin(),m_states.end(),
                        TrackFunctor::distanceAlongZ<State>(z) );
    if ( iter == m_states.end() )
      throw GaudiException( "No state closest to z","Track.cpp",
                            StatusCode::FAILURE );
    return *(*iter);
  }
};

//=============================================================================
// Retrieve the reference to the state closest to the given plane
//=============================================================================
State & Track::closestState( const Plane3D& plane )
{
  if ( !m_nodes.empty() ) {
    std::vector<Node*>::iterator iter = 
      std::min_element( m_nodes.begin(),m_nodes.end(),
                        TrackFunctor::distanceToPlane<Node>(plane) );
    if ( iter == m_nodes.end() )
      throw GaudiException( "No state closest to plane","Track.cpp",
                            StatusCode::FAILURE );
    return (*iter)->state();
  } else {
    std::vector<State*>::iterator iter = 
      std::min_element( m_states.begin(),m_states.end(),
                        TrackFunctor::distanceToPlane<State>(plane) );
    if ( iter == m_states.end() )
      throw GaudiException( "No state closest to plane","Track.cpp",
                            StatusCode::FAILURE );
    return *(*iter);
  }
};

//=============================================================================
// Retrieve the (const) reference to the state closest to the given plane
//=============================================================================
const State & Track::closestState( const Plane3D& plane ) const
{
  if ( !m_nodes.empty() ) {
    std::vector<Node*>::const_iterator iter = 
      std::min_element( m_nodes.begin(),m_nodes.end(),
                        TrackFunctor::distanceToPlane<Node>(plane) );
    if ( iter == m_nodes.end() )
      throw GaudiException( "No state closest to plane","Track.cpp",
                            StatusCode::FAILURE );
    return (*iter)->state();
  } else {
    std::vector<State*>::const_iterator iter = 
      std::min_element( m_states.begin(),m_states.end(),
                        TrackFunctor::distanceToPlane<State>(plane) );
    if ( iter == m_states.end() )
      throw GaudiException( "No state closest to plane","Track.cpp",
                            StatusCode::FAILURE );
    return *(*iter);
  }
};

//=============================================================================
// check the existence of a state at a certain predefined location
//=============================================================================
bool Track::hasStateAt( const LHCb::State::Location& location ) const
{
  std::vector<State*>::const_iterator iter =
    std::find_if( m_states.begin(),m_states.end(),
                  TrackFunctor::HasKey<State,const LHCb::State::Location&>
                  (&State::checkLocation,location) );
  return ( iter != m_states.end() );
};

//=============================================================================
// Retrieve the pointer to the state closest to the given plane
//=============================================================================
State& Track::stateAt( const LHCb::State::Location& location )
{
  std::vector<State*>::iterator iter = 
    std::find_if( m_states.begin(),m_states.end(),
                  TrackFunctor::HasKey<State,const LHCb::State::Location&>
                  (&State::checkLocation,location) );
  if ( iter == m_states.end() ) {

    std::ostringstream mess;
    mess << "There is no state at requested location " << location 
         << " track type " << type();      

    throw GaudiException( mess.str(),
                          "Track.cpp",
                          StatusCode::FAILURE );
  }
  return *(*iter);
};

//=============================================================================
// Retrieve the (const) pointer to the state at a given location
//=============================================================================
const State& Track::stateAt( const LHCb::State::Location& location ) const
{
  std::vector<State*>::const_iterator iter = 
    std::find_if( m_states.begin(),m_states.end(),
                  TrackFunctor::HasKey<State,const LHCb::State::Location&>
                  (&State::checkLocation,location) );
  if ( iter == m_states.end() ){

    std::ostringstream mess;
    mess << "There is no state at requested location " << location 
         << " track type " << type();      

    throw GaudiException( mess.str(),
                          "Track.cpp",
                          StatusCode::FAILURE );
  }
  return *(*iter);
};

//=============================================================================
// Add a State to the list of States associated to the Track
//=============================================================================
void Track::addToStates( const State& state )
{
  State* local = state.clone();
  int order = checkFlag(Track::Backward) ? -1 : 1;
  std::vector<State*>::iterator ipos = 
    std::upper_bound(m_states.begin(),
                     m_states.end(),
                     local,  
                     TrackFunctor::orderByZ<State>(order));
  m_states.insert(ipos,local);    
};

//=============================================================================
// Add a Measurement to the list associated to the Track
//=============================================================================
void Track::addToMeasurements( const Measurement& meas )
{
  // Only add a Measurement if the corresponding LHCbID belongs to the Track!
  if ( !isOnTrack( meas.lhcbID() ) ) return;
  Measurement* local = meas.clone();
  int order = checkFlag(Track::Backward) ? -1 : 1;
  std::vector<Measurement*>::iterator i = 
    std::upper_bound(m_measurements.begin(),
                     m_measurements.end(),
                     local,  
                     TrackFunctor::orderByZ<Measurement>(order));
  m_measurements.insert(i,local);
};

//=============================================================================
// Remove an LHCbID from the list of LHCbIDs associated to the Track
//=============================================================================
void Track::removeFromLhcbIDs( const LHCbID& value )
{
  std::vector<LHCbID>::iterator iter =
    std::remove( m_lhcbIDs.begin(), m_lhcbIDs.end(), value );
  m_lhcbIDs.erase( iter, m_lhcbIDs.end() );
  // Also remove the corresponding Measurement if present!
  if ( isMeasurementOnTrack( value ) ) {
    Measurement& meas = measurement( value );
    removeFromMeasurements( &meas );
  }
};

//=============================================================================
// Remove a State from the list of States associated to the Track
//=============================================================================
void Track::removeFromStates( State* state )
{
  TrackFunctor::deleteFromList<State>(m_states,state);
};

//=============================================================================
// Remove a Measurement from the list of Measurements associated to the Track
//=============================================================================
void Track::removeFromMeasurements( Measurement* meas )
{
  TrackFunctor::deleteFromList<Measurement>( m_measurements, meas );
  // set the appropriate flag is case the last measurement was removed ;-)
  if ( m_measurements.empty() ) setPatRecStatus( Track::PatRecIDs );
};

//=============================================================================
// Remove a Node from the list of Nodes associated to the Track
//=============================================================================
void Track::removeFromNodes( Node* node )
{
  // Also delete from the Track the Measurement corresponding
  // to the deleted Node, if present!
  if ( node -> hasMeasurement() ) {
    Measurement& meas = node -> measurement();
    TrackFunctor::deleteFromList<Node>(m_nodes,node);
    removeFromMeasurements( &meas );
  }
  else {
    TrackFunctor::deleteFromList<Node>(m_nodes,node);
  }
};

//=============================================================================
// Check whether the given LHCbID is on the Track
//=============================================================================
bool Track::isOnTrack( const LHCbID& value ) const
{
  return ( std::find( m_lhcbIDs.begin(), m_lhcbIDs.end(), value )
           != m_lhcbIDs.end() );
};

//=============================================================================
// Check whether the given Measurement is on the Track
//=============================================================================
bool Track::isOnTrack( const Measurement& value ) const
{
  for ( std::vector<Measurement*>::const_iterator it = m_measurements.begin();
        it != m_measurements.end(); ++it ) {
    if ( (*it) == (Measurement*) &value ) return true;
  }
  return false;
};

//=============================================================================
// Check whether the Measurement on the Track corresponding to the input LHCbID
// is present
//=============================================================================
bool Track::isMeasurementOnTrack( const LHCbID& value ) const
{
  for ( std::vector<Measurement*>::const_iterator it = m_measurements.begin();
        it != m_measurements.end(); ++it ) {
    if ( (*it) -> lhcbID() == value ) return true;
  }
  return false;
};

//=============================================================================
// Return the Measurement on the Track corresponding to the input LHCbID
//=============================================================================
const Measurement& Track::measurement( const LHCbID& value ) const
{
  for ( std::vector<Measurement*>::const_iterator it = m_measurements.begin();
        it != m_measurements.end(); ++it ) {
    if ( (*it) -> lhcbID() == value ) return *(*it);
  }
  throw GaudiException( "Measurement for LHCbID not present on Track",
                        "Track.cpp",
                        StatusCode::FAILURE );
};

//=============================================================================
// Return the Measurement on the Track corresponding to the input LHCbID
//=============================================================================
Measurement& Track::measurement( const LHCbID& value )
{
  for ( std::vector<Measurement*>::const_iterator it = m_measurements.begin();
        it != m_measurements.end(); ++it ) {
    if ( (*it) -> lhcbID() == value ) return *(*it);
  }
  throw GaudiException( "Measurement for LHCbID not present on Track",
                        "Track.cpp",
                        StatusCode::FAILURE );
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
       it != m_states.end(); ++it) delete *it;
  for (std::vector<Measurement*>::iterator it2 = m_measurements.begin();
       it2 != m_measurements.end(); ++it2) delete *it2;
  for (std::vector<Node*>::iterator it3 = m_nodes.begin();
       it3 != m_nodes.end(); ++it3) delete *it3;
  m_states.clear();
  m_measurements.clear();
  m_nodes.clear();
  m_ancestors.clear();
  m_extraInfo.clear();
};

//=============================================================================
// Clone the track keeping the key
//=============================================================================
Track* Track::cloneWithKey( ) const
{
  int theKey = this -> key();
  Track* tr = new Track( theKey );
  tr -> copy( *this );
  return tr;
};

//=============================================================================
// Clone the track
//=============================================================================
Track* Track::clone() const
{
  Track* tr = new Track();
  tr -> copy( *this );
  return tr;
};

//=============================================================================
// Copy the info from the argument track into this track
//=============================================================================
void Track::copy( const Track& track )
{
  reset();
  setChi2PerDoF( track.chi2PerDoF() );
  setNDoF( track.nDoF() );
  setFlags( track.flags() );
  setLhcbIDs( track.lhcbIDs() );
  setExtraInfo( track.extraInfo() );
  const std::vector<State*>& states = track.states();
  for (std::vector<State*>::const_iterator it = states.begin();
       it != states.end(); ++it) addToStates( *(*it));
  const std::vector<Measurement*>& measurements = track.measurements();
  for (std::vector<Measurement*>::const_iterator it2 = measurements.begin();
       it2 != measurements.end(); ++it2) addToMeasurements( *(*it2) );
  const std::vector<Node*>& nodes = track.nodes();
  for (std::vector<Node*>::const_iterator it3 = nodes.begin();
       it3 != nodes.end(); ++it3) addToNodes( (*it3)->clone() );
  const SmartRefVector<Track>& ancestors = track.ancestors();
  for (SmartRefVector<Track>::const_iterator it4 = ancestors.begin();
       it4 != ancestors.end();  ++it4) addToAncestors(*(*it4));
}

//=============================================================================
// Clear the state vector
//=============================================================================
void Track::clearStates() {
  for ( std::vector<State*>::iterator it = m_states.begin();
        it != m_states.end(); ++it ) delete *it;
  m_states.clear();
};

//=============================================================================
/** Check the presence of the information associated with 
 *  a given key
 *  
 *  @code
 * 
 *  const Track* p = ... ;
 *
 *  Track::Key key = ... ; 
 *  bool hasKey = p->hasInfo( key ) ;
 *
 *  @endcode 
 *  @param    key key to be checked 
 *  @return  'true' if there is informaiton with the 'key', 
 *           'false' otherwise
 */
//=============================================================================
bool LHCb::Track::hasInfo ( const int key ) const
{ return m_extraInfo.end() != m_extraInfo.find( key ) ; }

//=============================================================================
/** add/replace new information , associated with the key
 *  
 *  @code
 * 
 *  Track* p = ... ;
 *
 *  Track::Key  key   = ... ; 
 *  Track::Info info  = ... ;
 * 
 *  bool inserted = p->addInfo( key , info ) ;
 *
 *  @endcode 
 * 
 *  @param key key for the information
 *  @param info information to be associated with the key
 *  @param 'true' if informaiton is inserted, 
 *         'false' if the previous information has been replaced 
 */
//=============================================================================
bool  LHCb::Track::addInfo ( const int key, const double info )
{ return m_extraInfo.insert( key , info ).second ;}

//=============================================================================
/** extract the information associated with the given key 
 *  If there is no such infomration the default value will 
 *  be returned 
 * 
 *  @code
 * 
 *  const Track* p = ... ;
 *
 *  Track::Key  key   = ... ; 
 *
 *  // extract the information
 *  Track::Info info = p->info( key, -999 ) ;
 * 
 *  @endcode 
 *
 *  @param key key for the information
 *  @param def the default value to be returned 
 *         in the case of missing info
 *  @return information associated with the key if there 
 *          is such information, the default value otherwise 
 */
//=============================================================================
double LHCb::Track::info( const int key, const double def ) const 
{
  ExtraInfo::iterator i = m_extraInfo.find( key ) ;
  return m_extraInfo.end() == i ? def : i->second ;
}

//=============================================================================
/** erase the information associated with the given key
 *
 *  @code
 * 
 *  Track* p = ... ;
 *
 *  Track::Key  key   = ... ; 
 * 
 *  int erased = p->eraseInfo( key ) ;
 *
 *  @endcode 
 * 
 *  @param key key for the information
 *  @return return number of erased elements 
 */
//=============================================================================
LHCb::Track::ExtraInfo::size_type 
LHCb::Track::eraseInfo( const int key )
{ 
  return m_extraInfo.erase( key ) ; 
}

