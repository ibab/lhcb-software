// $: Track.cpp,v 1.39 2007/05/15 06:57:34 wouter Exp $ // Include files

#include <functional>
#include <string>
#include <map>

// from gsl
#include "gsl/gsl_cdf.h"

// local
#include "Event/Track.h"
#include "Event/TrackFunctor.h"
#include "Event/Node.h"

using namespace Gaudi;
using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : Track
//
// 2004-12-14 : Jose Hernando, Eduardo Rodrigues
//-----------------------------------------------------------------------------

//=============================================================================
// Retrieve the probability of chi^2
//=============================================================================
double Track::probChi2() const
{
  double val(0) ; 
  if (nDoF()>0) { 
    const double limit = 1e-15;
    double chi2max = gsl_cdf_chisq_Qinv (limit, nDoF()); 
    val = chi2() < chi2max ? gsl_cdf_chisq_Q(chi2(),nDoF()) : 0; 
  } 
  return val ;
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
State & Track::closestState( const Gaudi::Plane3D& plane )
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
const State & Track::closestState( const Gaudi::Plane3D& plane ) const
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
// Add a list of states to the list associated to the Track. This takes ownership.
//=============================================================================
void Track::addToStates( StateContainer& states )
{
  // Make sure that the incoming states are properly sorted. The 'if' is ugly, but more efficient than using 'orderByZ'.
  bool backward = checkFlag(Track::Backward) ;
  if(backward) std::sort(states.begin(),states.end(),TrackFunctor::decreasingByZ<State>());
  else         std::sort(states.begin(),states.end(),TrackFunctor::increasingByZ<State>());
  // Now append and use std::inplace_merge. Make sure there is enough capacity such that iterators stay valid.
  m_states.reserve( states.size() + m_states.size()) ;
  StateContainer::iterator middle = m_states.end() ;
  m_states.insert(middle, states.begin(), states.end()) ;
  if(backward) std::inplace_merge(m_states.begin(),middle,m_states.end(),TrackFunctor::decreasingByZ<State>());
  else         std::inplace_merge(m_states.begin(),middle,m_states.end(),TrackFunctor::increasingByZ<State>());
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
// Add a list of measurement to the list associated to the Track. This takes ownership.
//=============================================================================
void Track::addToMeasurements( MeasurementContainer& measurements )
{
  // Make sure that the incoming measurements are properly sorted. The 'if' is ugly, but more efficient than using 'orderByZ'.
  bool backward = checkFlag(Track::Backward) ;
  if(backward) std::sort(measurements.begin(),measurements.end(),TrackFunctor::decreasingByZ<Measurement>());
  else         std::sort(measurements.begin(),measurements.end(),TrackFunctor::increasingByZ<Measurement>());
  // Now append and use std::inplace_merge. Make sure there is enough capacity such that iterators stay valid.
  m_measurements.reserve( measurements.size() + m_measurements.size()) ;
  MeasurementContainer::iterator middle = m_measurements.end() ;
  m_measurements.insert(middle, measurements.begin(), measurements.end()) ;
  if(backward) std::inplace_merge(m_measurements.begin(),middle,m_measurements.end(),TrackFunctor::decreasingByZ<Measurement>());
  else         std::inplace_merge(m_measurements.begin(),middle,m_measurements.end(),TrackFunctor::increasingByZ<Measurement>());
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
// Remove all measurements from the track
//=============================================================================
void Track::clearMeasurements() 
{
  // remove all nodes first
  clearNodes() ;
  // now remove the measurements
  std::for_each(m_measurements.begin(), m_measurements.end(),TrackFunctor::deleteObject()) ;
  m_measurements.clear() ;
  setPatRecStatus( Track::PatRecIDs );
}

//=============================================================================
// Remove all nodes from the track
//=============================================================================
void Track::clearNodes() 
{
  std::for_each(m_nodes.begin(), m_nodes.end(),TrackFunctor::deleteObject()) ;
  m_nodes.clear() ;
}

//=============================================================================
// reset the track
//=============================================================================
void Track::reset()
{
  m_chi2PerDoF = 0;
  m_nDoF       = 0;
  m_flags      = 0;
  m_lhcbIDs.clear();
  std::for_each(m_states.begin(), m_states.end(),TrackFunctor::deleteObject()) ;
  std::for_each(m_measurements.begin(), m_measurements.end(),TrackFunctor::deleteObject()) ;
  std::for_each(m_nodes.begin(), m_nodes.end(),TrackFunctor::deleteObject()) ;
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
  
  // copy the states
  m_states.reserve( track.states().size() ) ;
  for( std::vector<State*>::const_iterator istate = track.states().begin() ;
       istate != track.states().end(); ++istate)
    m_states.push_back( (*istate)->clone() ) ;
  
  // copy the measurements. make a map from old to new measurements.
  typedef std::map<const Measurement*, Measurement*> MeasurementMap ;
  MeasurementMap measurementmap ;
  m_measurements.reserve(track.measurements().size()) ;
  for( std::vector<Measurement*>::const_iterator imeas = track.measurements().begin() ;
       imeas != track.measurements().end(); ++imeas) {
    m_measurements.push_back( (*imeas)->clone() ) ;
    measurementmap[*imeas] = m_measurements.back() ;
  }

  // copy the nodes. be sure to remap the measurement.
  m_nodes.reserve(track.nodes().size()) ;
  for (std::vector<Node*>::const_iterator inode = track.nodes().begin();
       inode != track.nodes().end(); ++inode) {
    m_nodes.push_back((*inode)->clone()) ;
    if( (*inode)->hasMeasurement() ) {
      MeasurementMap::const_iterator it = measurementmap.find( &(*inode)->measurement() );
      if(it != measurementmap.end()) 
        m_nodes.back()->setMeasurement(*(it->second)) ;
      else 
        throw GaudiException( "Track::copy: found a node pointing to a measurement not on track!",
                              "Track.cpp",StatusCode::FAILURE );
    }
  }
  
  // copy the ancestors
  const SmartRefVector<Track>& ancestors = track.ancestors();
  for (SmartRefVector<Track>::const_iterator it4 = ancestors.begin();
       it4 != ancestors.end();  ++it4) addToAncestors(*(*it4));
}

//=============================================================================
// Clear the state vector
//=============================================================================
void Track::clearStates() { 
  std::for_each(m_states.begin(), m_states.end(),TrackFunctor::deleteObject()) ;
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
/** Add new information, associated with the specified key.
 *  This method cannot be used to modify information for an already existing key
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
 *  @return 'true' if information is inserted,
 *         'false' if information was not inserted, due to already existing key
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
// Count the number of outliers
//=============================================================================

unsigned int LHCb::Track::nMeasurementsRemoved() const 
{
  size_t noutlier(0) ;
  for( NodeContainer::const_iterator inode = nodes().begin() ;
       inode != nodes().end(); ++inode)
    if( (*inode)->type() == LHCb::Node::Outlier ) ++noutlier ;
  return noutlier ;
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

//=============================================================================
// Fill stream
//=============================================================================
std::ostream& LHCb::Track::fillStream(std::ostream& os) const
{
  os << "*** Track ***" << std::endl
     << " key     : " << key() << std::endl
     << " type    : " << type() << std::endl
     << " history : " << history() << std::endl
     << " fitstatus: " << fitStatus() << std::endl
     << " # ids    : " << nLHCbIDs() << std::endl
     << " # meas   : " << nMeasurements() << std::endl
     << " chi2PerDoF : " << (float)m_chi2PerDoF << std::endl
     << " nDoF       : " << m_nDoF << std::endl;

  os << " extraInfo : [";
  for ( ExtraInfo::const_iterator i = extraInfo().begin();
        i != extraInfo().end(); ++i )
  {
    const LHCb::Track::AdditionalInfo info =
      static_cast<LHCb::Track::AdditionalInfo>(i->first);
    os << " " << info << "=" << i->second << " ";
  }
  os << "]" << std::endl;

  if ( !m_states.empty() ) {
    os << " p  : " << (float) firstState().p() <<std::endl
       << " pt : " << (float) firstState().pt() <<std::endl
       << " " << nStates() << " states at z =";
    for ( std::vector<LHCb::State*>::const_iterator iS = states().begin();
          iS != states().end(); ++iS )
    {
      if (*iS) os << " " << (*iS)->z();
    }
    os << "  :-" << std::endl;
    for ( std::vector<LHCb::State*>::const_iterator iS = states().begin();
          iS != states().end(); ++iS )
    {
      os << " " << **iS;
    }
    os << std::endl;
  }
  else {
    os << " # states : " << nStates() << std::endl;
  }

  return os;
}
