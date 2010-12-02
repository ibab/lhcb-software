// $: Track.cpp,v 1.39 2007/05/15 06:57:34 wouter Exp $ 
// Include files

#include <functional>
#include <string>
#include <map>

// from gsl
#include "gsl/gsl_cdf.h"

// local
#include "Event/Track.h"
#include "Event/TrackFunctor.h"
#include "Event/TrackFitResult.h"
#include "Event/Node.h"

using namespace Gaudi;
using namespace LHCb;

// ============================================================================
// Boost 
// ============================================================================
#include "boost/static_assert.hpp"
// ============================================================================

//-----------------------------------------------------------------------------
// Implementation file for class : Track
//
// 2004-12-14 : Jose Hernando, Eduardo Rodrigues
//-----------------------------------------------------------------------------

//=============================================================================
// Retrieve a pointer to the fit result
//=============================================================================
const TrackFitResult* Track::fitResult() const
{
  //if( m_fitResult == 0 ) const_cast<Track*>(this)->m_fitResult = new TrackFitResult() ;
  return m_fitResult ;
}

//=============================================================================
// Retrieve a pointer to the fit result
//=============================================================================
TrackFitResult* Track::fitResult()  
{
  //if( m_fitResult == 0 ) m_fitResult = new TrackFitResult() ;
  return m_fitResult ;
}

//=============================================================================
// Set the fit result. This takes ownership.
//=============================================================================
void Track::setFitResult(LHCb::TrackFitResult* absfit) 
{
  if( m_fitResult != absfit ) {
    if( m_fitResult ) delete m_fitResult ;
    m_fitResult = absfit ;
  }
}


//=============================================================================
// Get a range of nodes in this track 
//=============================================================================
Track::ConstNodeRange Track::nodes() const 
{
  if ( !m_fitResult ) { return Track::ConstNodeRange() ; }
  //
  const LHCb::TrackFitResult* _result = m_fitResult ;
  // cast the const container to a container of const pointers
  const LHCb::TrackFitResult::NodeContainer& nodes_ = _result->nodes() ;
  //
  typedef LHCb::TrackFitResult::NodeContainer::const_iterator Iterator1 ;
  typedef Track::ConstNodeRange::const_iterator               Iterator2 ;
  //
  const Iterator1 begin = nodes_ . begin () ;
  const Iterator1 end   = nodes_ . end   () ;
  //
  // Helper union to avoid reinterpret_cast 
  union _IteratorCast
  {
    //
    //
    const Iterator1* input  ;
    const Iterator2* output ;
    //
    BOOST_STATIC_ASSERT( sizeof(Iterator1) == sizeof(Iterator2) ) ;
  } ;
  //
  volatile _IteratorCast _begin ;
  volatile _IteratorCast _end   ;
  //
  _begin . input = &begin ;
  _end   . input = &end   ;
  //
  return Track::ConstNodeRange ( *_begin.output , *_end.output ) ;  
}

//=============================================================================
// Get the measurements on the track. Node that it does not return a
// reference. This is because I want to remove this container from
// fitresult.
//=============================================================================
Track::MeasurementContainer Track::measurements() const
{
  MeasurementContainer meas ;
  if( m_fitResult ) 
    meas.insert(meas.end(),m_fitResult->measurements().begin(),m_fitResult->measurements().end()) ; 
  return meas ;
}

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
State & Track::closestState( double z )
{
  if ( m_fitResult && !m_fitResult->nodes().empty() ) {
    std::vector<Node*>::iterator iter =
      std::min_element( m_fitResult->nodes().begin(),m_fitResult->nodes().end(),
                        TrackFunctor::distanceAlongZ<Node>(z) );
    if ( iter == m_fitResult->nodes().end() )
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
// Retrieve the (const) reference to the state closest to the given z-position
//=============================================================================
const State & Track::closestState( double z ) const
{
  if ( m_fitResult && !m_fitResult->nodes().empty() ) {
    std::vector<Node*>::const_iterator iter =
      std::min_element( m_fitResult->nodes().begin(),m_fitResult->nodes().end(),
                        TrackFunctor::distanceAlongZ<Node>(z) );
    if ( iter == m_fitResult->nodes().end() )
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
// Retrieve the (const) reference to the state closest to the given plane
//=============================================================================
const State & Track::closestState( const Gaudi::Plane3D& plane ) const
{
  if ( m_fitResult && !m_fitResult->nodes().empty() ) {
     std::vector<Node*>::const_iterator iter =
      std::min_element( m_fitResult->nodes().begin(),m_fitResult->nodes().end(),
                        TrackFunctor::distanceToPlane<Node>(plane) );
    if ( iter == m_fitResult->nodes().end() )
      throw GaudiException( "No state closest to z","Track.cpp",
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
  return stateAt(location)!=0 ;
};

//=============================================================================
// Retrieve the pointer to the state closest to the given plane
//=============================================================================
State* Track::stateAt( const LHCb::State::Location& location )
{
  StateContainer::iterator iter =
    std::find_if( m_states.begin(),m_states.end(),
                  TrackFunctor::HasKey<State,const LHCb::State::Location&>
                  (&State::checkLocation,location) );
  return iter == m_states.end() ? 0 : *iter ;
}

//=============================================================================
// Retrieve the (const) pointer to the state at a given location
//=============================================================================
const State* Track::stateAt( const LHCb::State::Location& location ) const
{
  StateContainer::const_iterator iter =
    std::find_if( m_states.begin(),m_states.end(),
                 TrackFunctor::HasKey<State,const LHCb::State::Location&>
                  (&State::checkLocation,location) );
  return iter == m_states.end() ? 0 : *iter ;
}

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
// Remove an LHCbID from the list of LHCbIDs associated to the Track
//=============================================================================
void Track::removeFromLhcbIDs( const LHCbID& value )
{ 
  LHCbIDContainer::iterator pos =
    std::lower_bound( m_lhcbIDs.begin(), m_lhcbIDs.end(), value ) ;
  m_lhcbIDs.erase( pos ) ;
};

//=============================================================================
// Remove a State from the list of States associated to the Track
//=============================================================================
void Track::removeFromStates( State* state )
{
  TrackFunctor::deleteFromList<State>(m_states,state);
};

//=============================================================================
// Add LHCbIDs to track
//=============================================================================
bool Track::addToLhcbIDs( const LHCb::LHCbID& value ) 
{
  LHCbIDContainer::iterator pos =
    std::lower_bound( m_lhcbIDs.begin(),m_lhcbIDs.end(),value ) ;
  bool rc = (pos == m_lhcbIDs.end()) || !( (*pos) == value ) ;
  if( rc ) m_lhcbIDs.insert( pos, value ) ;
  return rc ;
}

//=============================================================================
// Add LHCbIDs to track
//=============================================================================
bool Track::addSortedToLhcbIDs( const LHCbIDContainer& ids ) 
{
  LHCbIDContainer result( ids.size() + m_lhcbIDs.size() ) ;
  LHCbIDContainer::iterator pos =
    std::set_union( ids.begin(), ids.end(),
		    m_lhcbIDs.begin(), m_lhcbIDs.end(),
		    result.begin()) ;
  bool rc = pos == result.end() ;
  result.erase( pos, result.end() ) ;
  m_lhcbIDs.swap( result ) ;
  return rc ;
}  


//=============================================================================
// Compute the number of LHCbIDs that two tracks have in common
//=============================================================================
size_t Track::nCommonLhcbIDs(const Track& rhs) const
{
  // adapted from std::set_intersection
  size_t rc(0) ;
  LHCbIDContainer::const_iterator first1 = m_lhcbIDs.begin() ;
  LHCbIDContainer::const_iterator last1  = m_lhcbIDs.end() ;
  LHCbIDContainer::const_iterator first2 = rhs.m_lhcbIDs.begin() ;
  LHCbIDContainer::const_iterator last2  = rhs.m_lhcbIDs.end() ;
  while (first1 != last1 && first2 != last2)
    if (*first1 < *first2)
      ++first1;
    else if (*first2 < *first1)
      ++first2;
    else {
      ++first1;
      ++first2;
      ++rc ;
    }
  return rc ;
}

//=============================================================================
// Check whether the given LHCbID is on the Track
//=============================================================================
bool Track::isOnTrack( const LHCb::LHCbID& value ) const
{
  LHCbIDContainer::const_iterator pos =
    std::lower_bound( m_lhcbIDs.begin(), m_lhcbIDs.end(), value ) ;
  return pos != m_lhcbIDs.end() && *pos == value ;
} ;

//=============================================================================
// Return the Measurement on the Track corresponding to the input LHCbID
//=============================================================================
const Measurement* Track::measurement( const LHCbID& value ) const
{
  return m_fitResult ? m_fitResult->measurement(value) : 0 ;
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
  m_states.clear();
  m_ancestors.clear();
  m_extraInfo.clear();
  if( m_fitResult ) {
    delete m_fitResult ;
    m_fitResult = 0 ;
  }
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
  
  // copy the track fit info
  if( track.m_fitResult ) m_fitResult = track.m_fitResult->clone() ;

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
  return m_fitResult ? m_fitResult->nOutliers() : 0 ;
}

//=============================================================================
// Count the number of outliers
//=============================================================================

unsigned int LHCb::Track::nMeasurements() const 
{
  return m_fitResult ? m_fitResult->nActiveMeasurements() : 0 ;
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
