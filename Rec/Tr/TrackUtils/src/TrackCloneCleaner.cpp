#include "TrackCloneCleaner.h"

#include <string>
#include <vector>

#include "Event/Track.h"

// from Event/LinkerEvent
#include "Linker/LinkedFrom.h"


using namespace LHCb;
namespace {

/** @class WorkingTrack TrackCloneCleaner.h
 *
 *  Working track object for TrackCloneCleaner algorithm
 *
 *  @author M.Needham
 *  @date   30/05/2006
 */
struct WorkingTrack final
{
  WorkingTrack() = default;
  WorkingTrack( LHCb::Track* _track, const bool _clone = false )
    : track(_track), clone(_clone) {}
  // Access track Chi^2
  double chi2() const  { return track->chi2PerDoF(); }
  /// Access number of LHCbIDs
  double nLHCbIDs() const { return track->nLHCbIDs(); }
  /// Compare to see if its the same track
  bool sameTrack(const LHCb::Track* aTrack) const { return track == aTrack; }
  /// Return the track type ranking
  int trackTypeRank() const;
  LHCb::Track* track = nullptr; ///< Pointer to the track object
  bool clone = false;         ///< Clone flag
  typedef std::vector<WorkingTrack> Vector;
};

int WorkingTrack::trackTypeRank() const
{
  // CRJ : Should probably make type 'ranking' configurable via options ?
  switch (track->type()) {
  case LHCb::Track::Long       : return 0;
  case LHCb::Track::Downstream : return 1;
  case LHCb::Track::Upstream   : return 2;
  case LHCb::Track::Ttrack     : return 3;
  case LHCb::Track::Velo       : return 4;
  case LHCb::Track::VeloR      : return 5;
  case LHCb::Track::Muon       : return 6;
  default                      : return 999;
  }
}

}
DECLARE_ALGORITHM_FACTORY( TrackCloneCleaner )

TrackCloneCleaner::TrackCloneCleaner(const std::string& name,
                                     ISvcLocator* pSvcLocator):
  GaudiAlgorithm(name, pSvcLocator)
{
  // track locaton
  declareProperty("inputLocation",  m_inputLocation  = TrackLocation::Default);
  declareProperty("linkerLocation", m_linkerLocation = TrackLocation::Default+"Clones");
  declareProperty("CloneCut", m_cloneCut = 5000 );
}

TrackCloneCleaner::~TrackCloneCleaner() = default;


StatusCode TrackCloneCleaner::execute()
{
  // tracks to flag
  Tracks* trackCont = get<Tracks>(m_inputLocation);
  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << "Found " << trackCont->size() << " Tracks at " << m_inputLocation << endmsg;
  }

  // Get the clone libnker info
  LinkedFrom<LHCb::Track,LHCb::Track> linker(evtSvc(), msgSvc(),m_linkerLocation);
  if ( linker.notFound() )
  {
    return Warning("Linker not found !");
  }

  // copy the tracks into a temporary vector
  WorkingTrack::Vector tempTracks; tempTracks.reserve(trackCont->size());

  // loop and make working tracks
  for ( const auto& t : *trackCont ) {
    // only consider tracks with clone info
    if ( linker.first( t ) ) tempTracks.emplace_back( t );
  }

  // sort by type Lowest rank , then highest # of LHCbID, then smallest chi2
  auto order = [](const WorkingTrack& lhs, const WorkingTrack& rhs) {
      return std::make_tuple( lhs.trackTypeRank(), rhs.nLHCbIDs(), lhs.chi2() )
           < std::make_tuple( rhs.trackTypeRank(), lhs.nLHCbIDs(), rhs.chi2() );
  };
  std::sort( tempTracks.begin(), tempTracks.end(), order );

  for ( const auto& track : tempTracks ) {
    if ( msgLevel(MSG::VERBOSE) ) {
      verbose() << "Trying track key=" << track.track->key()
                << " " << track.track->history()
                << " chi2=" << track.chi2()
                << " nMeas=" << track.nLHCbIDs()
                << endmsg;
    }

    // skips if already tagged as a rejected clone
    if ( track.clone ) {
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << " -> Already flagged as a clone. Skipping" << endmsg;
      continue;
    }

    // pick up the clones
    for ( Track* cloneTrack = linker.first( track.track );
          cloneTrack;
          cloneTrack = linker.next() ) {
      // double check track is not flag as clone of itself !!
      if ( UNLIKELY(cloneTrack == track.track) ) {
        Error( "Track flagged as clone of itself !!" ).ignore();
        continue;
      }

      const double dist = linker.weight();
      if ( msgLevel(MSG::VERBOSE) ) {
        verbose() << " -> Clone track key=" << cloneTrack->key() << " " << cloneTrack->history()
                  << " dist=" << dist
                  << endmsg;
      }
      // check clone cut
      if ( dist < m_cloneCut ) {
        auto iter = std::find_if ( tempTracks.begin(), tempTracks.end(),
                                   [&](const WorkingTrack& t)
                                   { return t.sameTrack(cloneTrack); } );
        if ( iter == tempTracks.end() ) continue;
        iter->clone = true;
        if ( iter->track->info(LHCb::Track::CloneDist,1e99) > dist ) {
          if ( msgLevel(MSG::VERBOSE) ) {
            verbose() << "  -> Flagging track " << iter->track
                      << " key=" << iter->track->key() << " "
                      << iter->track->history() << " as a clone" << endmsg;
          }
          iter->track->addInfo( LHCb::Track::CloneDist, dist );
        }
      } // passed cut
    } // clone track
  } // tempTracks

  return StatusCode::SUCCESS;
}

