#include "TrackBuildCloneTable.h"

#include "GaudiKernel/PhysicalConstants.h"
#include "LHCbMath/MatrixUtils.h"
#include "Event/TrackUnitsConverters.h"
#include "Event/State.h"
#include "Event/Track.h"
#include "LHCbMath/Similarity.h"

// from Event/LinkerEvent
#include "Linker/LinkerWithKey.h"

using namespace LHCb;
namespace {

inline void invertCovMatrix(Gaudi::TrackSymMatrix& m)
{
  TrackUnitsConverters::convertToG3( m );
  m.Invert();
  TrackUnitsConverters::convertToG4( m );
}

/** @class CloneState TrackBuildCloneTable.h
 *
 *  Working state object for clone finder
 *
 *  @author M.Needham
 *  @date   30/05/2006
 */
struct CloneState final
{
  /// Contructor from a state and parent track object
  CloneState( const LHCb::State & _state )
    : state  ( _state),
      invCov ( _state.covariance() )
  { invertCovMatrix(invCov); }
  LHCb::State state;  ///< The state to use for the comparisions
  Gaudi::TrackSymMatrix invCov; ///< Cached inverted matrix
};

/** @class CloneTrack TrackBuildCloneTable.h
 *
 *  Working track object for clone finder
 *
 *  @author M.Needham
 *  @date   30/05/2006
 */
struct CloneTrack final
{
  CloneTrack( const LHCb::Track* _track = nullptr ) : track(_track) { }
  const CloneState * findState( double z ) const;
  std::vector<std::pair<double,CloneState>> states;
  const LHCb::Track* track; ///< Pointer to the parent track
  /// Container of working tracks
  typedef std::vector<CloneTrack> Vector;
};

const CloneState *CloneTrack::findState( double z ) const
{
  auto i = std::find_if( states.begin(), states.end(),
                         [&](const std::pair<double, CloneState>& cs) {
            return fabs( z - cs.first ) < 1e-3;
  });
  return i!=states.end() ? &(i->second) : nullptr;
}


/// Computes the KL distance between the two tracks
double kullbeckLieblerDist( const CloneState& c1, const CloneState& c2)
{
  Gaudi::TrackSymMatrix diffCov = c1.state.covariance() - c2.state.covariance();
  Gaudi::TrackSymMatrix invDiff = c2.invCov             - c1.invCov;
  // trace
  return Gaudi::Math::trace(diffCov*invDiff) 
       + LHCb::Math::Similarity(c1.state.stateVector() - c2.state.stateVector(),
                                c2.invCov              + c1.invCov);
}
}

DECLARE_ALGORITHM_FACTORY( TrackBuildCloneTable )

TrackBuildCloneTable::TrackBuildCloneTable(const std::string& name,
                                           ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name, pSvcLocator )
{
  declareProperty("inputLocation", m_inputLocation = TrackLocation::Default);
  declareProperty("outputLocation", m_outputLocation = TrackLocation::Default+"Clones");
  declareProperty("klCut", m_klCut = 5000);
  declareProperty("zStates", m_zStates );
  declareProperty("maxDz", m_maxDz = 2.0*Gaudi::Units::m);
  declareProperty("ExtrapolatorType", m_extraType = "TrackHerabExtrapolator" );
}

TrackBuildCloneTable::~TrackBuildCloneTable() = default;

StatusCode TrackBuildCloneTable::initialize()
{
  const StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) { return Error("Failed to initialize",sc); }

  m_extrapolator = tool<ITrackExtrapolator>(m_extraType);

  info() << "Will build clone table for z=" << m_zStates
         << "mm at '" << m_outputLocation << "'" << endmsg;
  info() << "Max Kullbeck Liebler Distance = " << m_klCut << endmsg;

  return sc;
}

StatusCode TrackBuildCloneTable::execute()
{

  // Tracks
  const Tracks* inCont = get<Tracks>(m_inputLocation);

  // new linker to store results
  LinkerWithKey<Track,Track> myLink( eventSvc(), msgSvc(), m_outputLocation);

  // need more than one track to do something usefull...
  if (inCont->size()<2) return StatusCode::SUCCESS;

  // working objects ---> invert the covariance matrix only once...
  CloneTrack::Vector tracks; tracks.reserve(inCont->size());
  for ( const auto& track : *inCont )
  {
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug() << "Track " << track->key() << " " << track->history() << endmsg;

    // make working track object, directly in the vector
    tracks.emplace_back( track );
    CloneTrack& cloneTrack = tracks.back();
    cloneTrack.states.reserve( m_zStates.size() );

    // Loop over all z positions
    for ( auto z : m_zStates ) {
      // get state closest to reference z pos
      const State & cState = track->closestState(z);

      // only take ones that are close in z
      if ( fabs( cState.z() - z ) > m_maxDz ) continue ;
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
        debug() << " -> Found state closest to z=" << z << " at z=" << cState.z() << endmsg;

      // clone state
      State tmpState ( cState );

      // extrapolate to desired z pos
      const StatusCode sc = m_extrapolator->propagate(tmpState,z);
      if (sc.isFailure())
      { Warning("Problem extrapolating state",StatusCode::SUCCESS).ignore(); continue; }

      // Add Clone state to clone track
      cloneTrack.states.emplace_back( z,tmpState );

    } // z states

    // Did we find any states ? If not remove clone track
    if ( cloneTrack.states.empty() ) tracks.pop_back();

  } // inCont

  // loop over the working objects
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
    debug() << "Looping over all Track pairs to find clones :-" << endmsg;
  for ( auto iterC1 = tracks.cbegin(); iterC1 != tracks.cend(); ++iterC1 )
  {
    // KL dist is symmetric between tracks, so only compute for each pair once
    for (auto iterC2 = std::next(iterC1); iterC2 != tracks.cend(); ++iterC2 )
    {
      // overall distance for this track pair
      double overall_dist(0);
      bool haveComparison(false);

      // loop over possible z positions
      for ( const auto& z : m_zStates ) {
        // Do both clone tracks have states at this z ?
        const CloneState * c1 = (*iterC1).findState(z);
        if ( !c1 ) continue;
        const CloneState * c2 = (*iterC2).findState(z);
        if ( !c2 ) continue;

        haveComparison = true;
        const double dist = kullbeckLieblerDist(*c1,*c2);
        overall_dist += dist;
        if ( msgLevel(MSG::VERBOSE) ||
             (msgLevel(MSG::DEBUG) && dist < m_klCut ) )
          debug() << " -> Tracks " << (*iterC1).track->key() << " and "
                  << (*iterC2).track->key() << " both have a state at z=" << z
                  << " KL-dist=" << dist
                  << endmsg;
      } // z states

      if ( haveComparison && (overall_dist < m_klCut) )
      {
        if ( msgLevel(MSG::DEBUG) )
        {
          debug() << "  -> Possible clones Track1 : key = " << iterC1->track->key()
                  << " history = " << iterC1->track->history() << endmsg;
          debug() << "                     Track2 : key = " << iterC2->track->key()
                  << " history = " << iterC2->track->history() << endmsg;
          debug() << "                            : KL distance = " << overall_dist << endmsg;
        }
        // fill twice, for each way around for the track pair
        myLink.link( iterC1->track, iterC2->track, overall_dist );
        myLink.link( iterC2->track, iterC1->track, overall_dist );
      }

    } // iterC2
  } // iterC1

  return StatusCode::SUCCESS;
}
