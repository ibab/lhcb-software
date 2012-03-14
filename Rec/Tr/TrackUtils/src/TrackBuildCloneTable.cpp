#include "TrackBuildCloneTable.h"

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( TrackBuildCloneTable )

TrackBuildCloneTable::TrackBuildCloneTable(const std::string& name,
                                           ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name, pSvcLocator )
  , m_zStates      ( 1, 0.0 )
  , m_extrapolator(0)
{
  declareProperty("inputLocation", m_inputLocation = TrackLocation::Default);
  declareProperty("outputLocation", m_outputLocation = TrackLocation::Default+"Clones");
  declareProperty("klCut", m_klCut = 5000);
  declareProperty("zStates", m_zStates );
  declareProperty("maxDz", m_maxDz = 2.0*Gaudi::Units::m);
  declareProperty("ExtrapolatorType", m_extraType = "TrackHerabExtrapolator" );
}

TrackBuildCloneTable::~TrackBuildCloneTable() { }

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

  // working objects ---> invert the covariance matrix only once...
  CloneTrack::Vector tracks;
  tracks.reserve(inCont->size());
  for ( Tracks::const_iterator iT = inCont->begin();
        iT != inCont->end(); ++iT )
  {
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug() << "Track " << (*iT)->key() << " " << (*iT)->history() << endmsg;

    // make working track object, directly in the vector
    tracks.push_back( CloneTrack(*iT) );
    CloneTrack & cloneTrack = tracks.back();

    // Loop over all z positions
    for ( std::vector<double>::const_iterator iZ = m_zStates.begin();
          iZ != m_zStates.end(); ++iZ )
    {
      // get state closest to reference z pos
      const State & cState = (*iT)->closestState(*iZ);

      // only take ones that are close in z
      if ( fabs( cState.z() - *iZ ) > m_maxDz ) continue ;
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
        debug() << " -> Found state closest to z=" << *iZ << " at z=" << cState.z() << endmsg;

      // clone state
      State tmpState ( cState );

      // extrapolate to desired z pos
      const StatusCode sc = m_extrapolator->propagate(tmpState,*iZ);
      if (sc.isFailure())
      { Warning("Problem extrapolating state",StatusCode::SUCCESS).ignore(); continue; }

      // Add Clone state to clone track
      cloneTrack.states.insert( CloneTrack::CloneStates::value_type(*iZ,CloneState(tmpState)) );

    } // z states

    // Did we find any states ? If not remove clone track
    if ( cloneTrack.states.empty() ) tracks.pop_back();

  } // inCont

  // loop over the working objects
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << "Looping over all Track pairs to find clones :-" << endmsg;
  for ( CloneTrack::Vector::const_iterator iterC1 = tracks.begin();
        iterC1 != tracks.end(); ++iterC1 )
  {
    // KL dist is symmetric between tracks, so only compute for each pair once
    CloneTrack::Vector::const_iterator iterC2 = iterC1;
    ++iterC2;
    for ( ; iterC2 != tracks.end(); ++iterC2 )
    {
      // overall distance for this track pair
      double overall_dist(0);
      bool haveComparison(false);

      // loop over possible z positions
      for ( std::vector<double>::const_iterator iZ = m_zStates.begin();
            iZ != m_zStates.end(); ++iZ )
      {
        // Do both clone tracks have states at this z ?
        const CloneState * c1 = (*iterC1).findState(*iZ);
        if ( c1 )
        {
          const CloneState * c2 = (*iterC2).findState(*iZ);
          if ( c2 )
          {
            haveComparison = true;
            const double dist = kullbeckLieblerDist(*c1,*c2);
            overall_dist += dist;
            if ( msgLevel(MSG::VERBOSE) || 
                 (msgLevel(MSG::DEBUG) && dist < m_klCut ) )
              debug() << " -> Tracks " << (*iterC1).track->key() << " and "
                      << (*iterC2).track->key() << " both have a state at z=" << *iZ
                      << " KL-dist=" << dist
                      << endmsg;
          }
        }
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

double TrackBuildCloneTable::kullbeckLieblerDist( const CloneState& c1,
                                                  const CloneState& c2) const
{
  const Gaudi::TrackVector diffVec
    = c1.state.stateVector() - c2.state.stateVector();
  const Gaudi::TrackSymMatrix diffCov
    = c1.state.covariance()  - c2.state.covariance();

  const Gaudi::TrackSymMatrix invDiff = c2.invCov - c1.invCov;
  const Gaudi::TrackSymMatrix invSum = c2.invCov + c1.invCov;
  const Gaudi::TrackMatrix diff = diffCov * invDiff;

  // trace
  return Gaudi::Math::trace(diff) + Similarity(invSum,diffVec);
}

const TrackBuildCloneTable::CloneState *
TrackBuildCloneTable::CloneTrack::findState( const double z ) const
{
  const CloneState * cState = NULL;
  for ( CloneStates::const_iterator iC = states.begin();
        iC != states.end(); ++iC )
  {
    if ( fabs( z - iC->first ) < 1e-3 ) { cState = &(iC->second); break; }
  }
  return cState;
}
