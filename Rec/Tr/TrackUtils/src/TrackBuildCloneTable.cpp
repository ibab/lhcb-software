// $Id: TrackBuildCloneTable.cpp,v 1.2 2007-09-14 13:03:41 jonrob Exp $

#include "TrackBuildCloneTable.h"

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( TrackBuildCloneTable );

TrackBuildCloneTable::TrackBuildCloneTable(const std::string& name,
                                           ISvcLocator* pSvcLocator):
  GaudiAlgorithm(name, pSvcLocator)
{

  declareProperty("inputLocation", m_inputLocation = TrackLocation::Default);
  declareProperty("outputLocation", m_outputLocation = TrackLocation::Default+"Clones");
  declareProperty("klCut", m_klCut = 5000);
  declareProperty("zState", m_zState = 0.0);
  declareProperty("maxDz", m_maxDz = 2.0*Gaudi::Units::m);
  declareProperty("ExtrapolatorType", m_extraType = "TrackHerabExtrapolator" );

}

TrackBuildCloneTable::~TrackBuildCloneTable() { }

StatusCode TrackBuildCloneTable::initialize()
{
  const StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) { return Error("Failed to initialize",sc); }

  m_extrapolator = tool<ITrackExtrapolator>(m_extraType);

  info() << "Will build clone table for z=" << m_zState 
         << "mm at '" << m_outputLocation << "'" << endreq;
  info() << "Max Kullbeck Liebler Distance = " << m_klCut << endreq;

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
  for ( Tracks::const_iterator iterT = inCont->begin();
        iterT != inCont->end(); ++iterT )
  {

    // get state closest to reference z pos
    const State & closestState = (*iterT)->closestState(m_zState);

    // only take ones that are close in z
    if ( fabs(closestState.z() - m_zState) > m_maxDz ) continue ;

    // clone state
    State tmpState = closestState;

    // extrapolate to desired z pos
    const StatusCode sc = m_extrapolator->propagate(tmpState, m_zState);
    if (sc.isFailure()) continue;

    // make new object, directly in the vector
    tracks.push_back( CloneTrack(tmpState,*iterT) );

  } // inCont

  // loop over the working objects
  for ( CloneTrack::Vector::const_iterator iterC1 = tracks.begin();
        iterC1 != tracks.end(); ++iterC1 )
  {
    // KL dist is symmetric between tracks, so only compute for each pair once
    CloneTrack::Vector::const_iterator iterC2 = iterC1;
    ++iterC2;
    for ( ; iterC2 != tracks.end(); ++iterC2 )
    {
      const double dist = kullbeckLieblerDist(*iterC1,*iterC2);
      if (dist < m_klCut)
      {
        if ( msgLevel(MSG::DEBUG) )
        {
          debug() << "Possible clones Track1 : key = " << iterC1->track->key()
                  << " history = " << iterC1->track->history() << endreq;
          debug() << "                Track2 : key = " << iterC2->track->key()
                  << " history = " << iterC2->track->history() << endreq;
          debug() << "                       : KL distance = " << dist << endreq;
        }
        // fill twice, for each way around for the track pair
        myLink.link( iterC1->track, iterC2->track, dist );
        myLink.link( iterC2->track, iterC1->track, dist );
      }
    } // iterC2
  } // iterC1

  return StatusCode::SUCCESS;
}

double TrackBuildCloneTable::kullbeckLieblerDist(const CloneTrack& track1,
                                                 const CloneTrack& track2) const
{
  const Gaudi::TrackVector diffVec    
    = track1.state.stateVector() - track2.state.stateVector();
  const Gaudi::TrackSymMatrix diffCov 
    = track1.state.covariance()  - track2.state.covariance();

  const Gaudi::TrackSymMatrix invDiff = track2.invCov - track1.invCov;
  const Gaudi::TrackSymMatrix invSum = track2.invCov + track1.invCov;
  const Gaudi::TrackMatrix diff = diffCov * invDiff;

  // trace
  return Gaudi::Math::trace(diff) + Similarity(invSum,diffVec);
}
