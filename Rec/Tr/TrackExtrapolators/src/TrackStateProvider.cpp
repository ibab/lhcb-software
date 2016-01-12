// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackStateProvider.h"
#include "TrackInterfaces/ITrackExtrapolator.h"
#include "TrackInterfaces/ITrackInterpolator.h"

// from TrackEvent
#include "Event/Track.h"
#include "Event/State.h"
#include "Event/StateVector.h"
#include "Event/TrackFunctor.h"
#include "Event/StateParameters.h"

// from TrackKernel
#include "TrackKernel/TrackTraj.h"

/** @class TrackStateProvider TrackStateProvider.h
 *  
 *  A TrackStateProvider is a base class implementing methods
 *  from the ITrackStateProvider interface.
 *
 *  @author Wouter Hulsbergen
 *  @date   14/08/2010
 **/

// Disable warning on windows about using 'this' in constructors
// necessary for tool handles
#ifdef _WIN32
#pragma warning ( disable:4355 )
#endif


// forward declarations
namespace {
  class TrackCache ;
}

class TrackStateProvider : public GaudiTool, 
			   virtual public ITrackStateProvider,
			   virtual public IIncidentListener {
public:

  /// Standard constructor
  TrackStateProvider( const std::string& type, 
		     const std::string& name,
		     const IInterface* parent ) ;
  
  /// Destructor
  virtual ~TrackStateProvider( );
  
  /// initialize
  StatusCode initialize() ;

  /// initialize
  StatusCode finalize() ;

  /// Compute the state of the track at position z.  The third
  /// argument is the tolerance: if an existing state is found within
  /// a z-distance 'tolerance', that state is returned.
  /// If there are 'fitnodes' on the track (e.g. in Brunel), this
  /// method will use interpolation. If there are no fitnodes (e.g. in
  /// DaVinci) the method will use extrapolation. In that case the
  /// answer is only correct outside the measurement range.
  virtual StatusCode state( LHCb::State& astate,
			    const LHCb::Track& track,
			    double z,
			    double ztolerance ) const ;

  /// Compute state using cached trajectory
  virtual StatusCode stateFromTrajectory( LHCb::State& state,
					  const LHCb::Track& track, 
					  double z ) const {
    const LHCb::TrackTraj* traj = trajectory( track ) ;
    if( traj ) state = traj->state(z) ;
    return traj ? StatusCode::SUCCESS : StatusCode::FAILURE ;
  }
  
  /// Retrieve the cached trajectory
  virtual const LHCb::TrackTraj* trajectory( const LHCb::Track& track ) const ;
  
  /// Clear the cache
  virtual void clearCache() ;

  /// Clear the cache for a particular track
  virtual void clearCache(const LHCb::Track& track) ;
  
  /// incident service handle
  virtual void handle( const Incident& incident ) ;

protected:
  StatusCode computeState( const TrackCache& tc, double z, LHCb::State& state,
			   const LHCb::TrackTraj::StateContainer::const_iterator& position) const ;
  const LHCb::State* addState( TrackCache& tc, double z,
			       LHCb::State::Location loc = LHCb::State::LocationUnknown,
			       const LHCb::TrackTraj::StateContainer::const_iterator* position=0) const ;
  TrackCache* cache( const LHCb::Track& track ) const ;

private :
  typedef std::map< size_t, ::TrackCache*> TrackCacheMap ;
  mutable TrackCacheMap m_trackcache ;
  ToolHandle< ITrackExtrapolator > m_extrapolator ;
  ToolHandle< ITrackInterpolator > m_interpolator ;
  bool m_applyMaterialCorrections ;
  double m_linearPropagationTolerance ;
  bool m_cacheStatesOnDemand ;
  bool m_debugLevel ;
};



/**********************************************************************************************/



namespace {
  
  inline bool compareStateZ(const LHCb::State* lhs, const LHCb::State* rhs)
  {
    return lhs->z() < rhs->z() ;
  }
  
  // The TrackCache is basically just an 'extendable' TrackTraj
  class TrackCache : public LHCb::TrackTraj {
  private:
    const LHCb::Track* m_track ;
    std::vector<LHCb::State*> m_ownedstates ;
    double m_zFirstMeasurement ;
  public:
    TrackCache( const LHCb::Track& track ) 
      :  LHCb::TrackTraj(track), m_track(&track)
    {
      const LHCb::State* state = track.stateAt( LHCb::State::FirstMeasurement ) ;
      m_zFirstMeasurement = state ? state->z() : -9999 ;
    }

    ~TrackCache() {
      for( std::vector<LHCb::State*>::const_iterator it = m_ownedstates.begin() ;
	   it != m_ownedstates.end() ; ++it) delete *it ;
    }

    /// get the track
    const LHCb::Track& track() const { return *m_track; }

    /// because the pointer to the track is not good enough, for paranoia we compute a sort of uniqueID
    static size_t trackID(const LHCb::Track& track) { 
      // create a hash from 3 lhcbids (3 should definitely be enough:-)
      //std::size_t seed = track.lhcbIDs().front() ;
      //boost::hash_combine( seed, 
      //std::size_t seed = 0 ;
      //using namespace boost ;
      //for ( _LHCbIDs::const_iterator i = ids.begin() ; ids.end() != i ; ++i ) 
      //{ hash_combine ( seed , i->channelID () ) ; }
      return size_t(&track) + track.lhcbIDs().front().lhcbID() + track.lhcbIDs().back().lhcbID() ; }
    
    /// return z position of state at first measurement
    double zFirstMeasurement() const { return m_zFirstMeasurement ; }

    // insert a new state in the container with states
    void insertState( StateContainer::const_iterator pos, LHCb::State* state ) ;

    // return the states (work around keyword protected)
    StateContainer& states() { return refStates() ; }

    // return the states (work around keyword protected)
    const StateContainer& states() const { return refStates() ; }
    
    // return number of owned states
    size_t numOwnedStates() const { return m_ownedstates.size() ;}
  } ;

  /// Add a state
  void TrackCache::insertState( StateContainer::const_iterator pos, LHCb::State* state ) {

    // take ownership of state
    m_ownedstates.push_back(state) ;

    // get the vector with states
    StateContainer& refstates = refStates() ;

    // temporary logic test
    // if( refstates.size()>0) {
    //       if(pos == refstates.end() ) {   
    // 	assert( refstates.back()->z() < state->z() ) ;
    //       } else if ( pos == refstates.begin() ) {
    // 	assert( refstates.front()->z() > state->z() ) ;
    //       } else {	
    // 	StateContainer::iterator prev = pos ; --prev ;
    // 	assert( (*prev)->z() < state->z() && state->z() < (*pos)->z() ) ;
    //       }
    //     }
    
    // insert the state. gcc 4.8 has trouble with the
    // const_iterator. let's hope that the optimization does the
    // obvious with the following strange cast.
    const StateContainer::const_iterator constbegin = refstates.begin() ;
    const StateContainer::iterator nonconstpos = refstates.begin() + std::distance( constbegin, pos ) ;
    refstates.insert(nonconstpos, state) ;

    // update the range of the trajectory
    Trajectory::setRange( refstates.front()->z(), refstates.back()->z() ) ;
    
    // invalidate the cache
    TrackTraj::invalidateCache() ;
  }
}

DECLARE_TOOL_FACTORY( TrackStateProvider )

//=============================================================================
// TrackStateProvider constructor.
//=============================================================================
TrackStateProvider::TrackStateProvider( const std::string& type,
					const std::string& name,
					const IInterface* parent )
  : GaudiTool(type, name, parent),
    m_extrapolator("TrackMasterExtrapolator",this),
    m_interpolator("TrackInterpolator",this),
    m_debugLevel(false)
{ 
  declareInterface<ITrackStateProvider>(this);
  declareProperty("Extrapolator",m_extrapolator) ;
  declareProperty("Interpolator",m_interpolator) ;
  declareProperty("ApplyMaterialCorrections",m_applyMaterialCorrections = true) ;
  declareProperty("LinearPropagationTolerance", m_linearPropagationTolerance = 1.0*Gaudi::Units::mm) ;
  declareProperty("CacheStatesOnDemand",m_cacheStatesOnDemand = true ) ;
}

//=============================================================================
// TrackStateProvider destructor.
//=============================================================================
TrackStateProvider::~TrackStateProvider()
{
  clearCache() ;
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode TrackStateProvider::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  // retrieve extrapolator
  if(sc.isSuccess()) sc = m_extrapolator.retrieve() ;
  if(sc.isSuccess()) sc = m_interpolator.retrieve() ;
  // reset pointer to list of clusters at beginevent
  incSvc()->addListener(this, IncidentType::BeginEvent);
  
  m_debugLevel = msgLevel( MSG::DEBUG ) || msgLevel( MSG::VERBOSE ) ;  
  return sc ;
}

//=============================================================================
// Finalize
//=============================================================================
StatusCode TrackStateProvider::finalize()
{
  clearCache() ;
  m_extrapolator.release().ignore() ;
  m_interpolator.release().ignore() ;
  return GaudiTool::finalize();
}


//=============================================================================
// Incident handle
//=============================================================================

void TrackStateProvider::handle ( const Incident& incident )
{
  if ( IncidentType::BeginEvent == incident.type() ) 
    clearCache() ;
}

//=============================================================================
// Clear cache (and update some counters)
//=============================================================================

void TrackStateProvider::clearCache()
{
  if(m_debugLevel) debug() << "Clearing cache. Size is " << m_trackcache.size() << "." << endmsg ;
  counter("Number of tracks seen") += m_trackcache.size()  ;
  size_t numOwnedStates(0) ;
  for(TrackCacheMap::iterator it = m_trackcache.begin() ; 
      it != m_trackcache.end(); ++it) {
    numOwnedStates += it->second->numOwnedStates() ;
    delete it->second ;
  }
  if( !m_trackcache.empty() )
    counter("Number of states added") += numOwnedStates ;
  m_trackcache.clear() ;
}


//=============================================================================
// Clear cache for a given track
//=============================================================================

void TrackStateProvider::clearCache(const LHCb::Track& track)
{
  if(m_debugLevel) debug() << "Clearing cache for track: " << &track << endmsg ;
  size_t key = TrackCache::trackID(track) ;
  TrackCacheMap::iterator it = m_trackcache.find( key ) ;
  if( it != m_trackcache.end() ) {
    delete it->second ;
    m_trackcache.erase( it ) ;
  }
}


//=============================================================================
// get a state at a particular z position, within given tolerance
//=============================================================================

StatusCode TrackStateProvider::state( LHCb::State& state,
				      const LHCb::Track& track, 
				      double z,
				      double ztolerance ) const
{
  StatusCode sc = StatusCode::SUCCESS ;
  TrackCache* tc = cache( track ) ;

  // locate the closest state with lower_bound, but cache the
  // insertion point, because we need that multiple times
  state.setZ( z ) ;
  const LHCb::TrackTraj::StateContainer::const_iterator position = 
    std::lower_bound( tc->states().begin(),tc->states().end(),&state, compareStateZ ) ;
  const LHCb::State* closeststate(0) ;
  if( position == tc->states().end() ) {
    closeststate = tc->states().back() ;
  } else if( position == tc->states().begin() ) {
    closeststate = tc->states().front() ;
  } else {
    auto prev = position ; --prev ;
    //assert( z - (*prev)->z()>=0) ;
    //assert( (*it)->z() - z>=0) ;
    closeststate = z - (*prev)->z() < (*position)->z() - z ? *prev : *position ;
  }

  double absdz = std::abs( z - closeststate->z() ) ;
  if( absdz > std::max(ztolerance,TrackParameters::propagationTolerance) ) {
    if( absdz > m_linearPropagationTolerance ) {
      if( m_cacheStatesOnDemand ) {
	const LHCb::State* newstate = addState( *tc, z, LHCb::State::LocationUnknown, &position ) ;
	counter("AddedNumberOnDemandStates") +=1 ;
	if( newstate == 0 ) {
	  sc = StatusCode::FAILURE ; 
	} else {
	  state = *newstate ;
	}
      } else {
	sc = computeState( *tc, z, state, position ) ;
      }
    } else {
      // if we are really close, we'll just use linear extrapolation and do not cache the state.
      state = *closeststate ;
      state.linearTransportTo( z ) ;
    }
  } else {
    state = *closeststate ;
  }
  return sc ;
}

//=============================================================================
// add a state to the cache of a given track
//=============================================================================

const LHCb::State* TrackStateProvider::addState( TrackCache& tc, double z, LHCb::State::Location loc,
						 const LHCb::TrackTraj::StateContainer::const_iterator* position ) const
{
  LHCb::State* state = new LHCb::State( loc ) ;
  state->setZ( z ) ;
  LHCb::TrackTraj::StateContainer::const_iterator it =
    position ? *position : std::lower_bound( tc.states().begin(),tc.states().end(),state, compareStateZ ) ;
  StatusCode sc = computeState(tc,z,*state,it) ;
  if( sc.isFailure() ) {
    delete state ;
    state = 0 ;
  } else {
    state->setLocation( loc ) ;
    tc.insertState( it, state ) ;
    if( m_debugLevel ) {
      debug() << "Adding state to track cache: "
	      << tc.trackID(tc.track()) << " " << z << " " << loc << endreq ;
    }
  }
  return state ;
}

StatusCode TrackStateProvider::computeState( const TrackCache& tc, double z, LHCb::State& state,
					     const LHCb::TrackTraj::StateContainer::const_iterator& position ) const
{
  // in brunel, we simply use the interpolator. in davinci, we use the
  // extrapolator, and we take some control over material corrections.
  const LHCb::Track& track = tc.track() ;
  StatusCode sc ;
  if( track.fitResult() && !track.fitResult()->nodes().empty() && track.fitStatus()==LHCb::Track::Fitted) {
    sc = m_interpolator->interpolate( track, z, state ) ;
  } else {
    // locate the states surrounding this z position
    const LHCb::TrackTraj::StateContainer& refstates = tc.states() ;
    state.setZ( z ) ;
    auto it = position ;
    bool applyMaterialCorrections = false ;
    if( it == refstates.end() ) {
      state = *refstates.back() ;
    } else if( it == refstates.begin() || z < tc.zFirstMeasurement() ) {
      state = **it ;
      // if we extrapolate from ClosestToBeam, we don't apply mat corrections.
      applyMaterialCorrections = state.location() != LHCb::State::ClosestToBeam && m_applyMaterialCorrections ;
    } else {
      // take the closest state.
      auto prev = it ;
      --prev ;
      state = std::abs( (**it).z() - z ) < std::abs( (**prev).z() - z ) ? **it : **prev ;
    }
    if( m_debugLevel) 
      debug() << "Extrapolating to z = " << z << " from state at z= " << state.z() << endreq ;

    if( applyMaterialCorrections ) {
      sc = m_extrapolator->propagate( state, z ) ;
    } else {
      LHCb::StateVector statevec( state.stateVector(), state.z() ) ;
      Gaudi::TrackMatrix transmat ;
      sc = m_extrapolator->propagate( statevec, z, &transmat ) ;
      state.setState( statevec ) ;
      state.setCovariance( ROOT::Math::Similarity<double,Gaudi::TrackMatrix::kRows,Gaudi::TrackMatrix::kCols>
			   ( transmat, state.covariance() ) );
    }
  }
  return sc ;
}

//=============================================================================
// retrieve the cache for a given track
//=============================================================================

TrackCache* TrackStateProvider::cache( const LHCb::Track& track ) const
{
  TrackCache* tc(0) ;
  size_t key = TrackCache::trackID(track) ;
  TrackCacheMap::iterator it = m_trackcache.find( key ) ;
  if( it == m_trackcache.end() ) {
    if(m_debugLevel)
      debug() << "Creating track cache for track: " << &track << " " << key << " " << track.states().size() << endmsg ;
    // create a new entry in the cache
    tc = new TrackCache( track ) ;
    m_trackcache[key] = tc ;
    // make sure downstream tracks have a few ref states before the first measurement.
    if( track.type()==LHCb::Track::Downstream ) {
      if( track.stateAt( LHCb::State::EndRich1 ) == 0 ) 
	addState( *tc, StateParameters::ZEndRich1,LHCb::State::EndRich1) ;
      if( track.stateAt( LHCb::State::BegRich1 ) == 0 ) 
	addState( *tc, StateParameters::ZBegRich1,LHCb::State::BegRich1) ;
      if( track.stateAt( LHCb::State::EndVelo ) == 0 ) 
	addState( *tc, StateParameters::ZEndVelo,LHCb::State::EndVelo) ;
    }
    
    // make sure all tracks (incl. Downstream) get assigned a state at
    // the beamline. this is useful for the trajectory approximation.
    if( (track.hasVelo() || track.hasTT() ) &&
	track.firstState().location() != LHCb::State::ClosestToBeam &&
	track.stateAt( LHCb::State::ClosestToBeam ) == 0 ) {
      // compute poca of first state with z-axis 
      const Gaudi::TrackVector& vec = track.firstState().stateVector();
      // check on division by zero (track parallel to beam line!)
      double t2 = vec[2]*vec[2] + vec[3]*vec[3] ;
      if ( t2 > 1e-12 ) {
	double dz = - ( vec[0]*vec[2] + vec[1]*vec[3] ) / t2 ;
	// don't add the state if it is too close
	if( dz < -10*Gaudi::Units::cm ) {
	  double z = track.firstState().z() + dz ;
	  if(z > -100*Gaudi::Units::cm ) { // beginning of velo
	    addState( *tc, z, LHCb::State::ClosestToBeam ) ;
	  }
	} 
      }
    }
  } else {
    tc = it->second ;
  }
  
  return tc ;
}

//=============================================================================
// retrieve a trajectory for a given track (from the cache)
//=============================================================================
const LHCb::TrackTraj* TrackStateProvider::trajectory( const LHCb::Track& track ) const
{
  const TrackCache* tc = cache( track ) ;
  return tc ? static_cast<const LHCb::TrackTraj*>(tc) : 0 ;
}
