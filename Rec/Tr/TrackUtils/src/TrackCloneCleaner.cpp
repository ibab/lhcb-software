#include "TrackCloneCleaner.h"

using namespace LHCb;
using namespace boost::lambda;

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

TrackCloneCleaner::~TrackCloneCleaner() { }

StatusCode TrackCloneCleaner::initialize()
{
  const StatusCode sc = GaudiAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;

  return sc;
}

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
  WorkingTrack::Vector tempTracks;

  // loop and make working tracks
  for ( Tracks::const_iterator iterT = trackCont->begin();
        iterT != trackCont->end(); ++iterT )
  {
    // only consider tracks with clone info
    const LHCb::Track* cloneTrack = linker.first( *iterT );
    if ( cloneTrack )
    {
      tempTracks.push_back( WorkingTrack(*iterT) );
    }
  }

  // sort by chi2, best (smallest) first
  std::stable_sort( tempTracks.begin(), tempTracks.end(),
                    bind(&WorkingTrack::chi2,_1) < bind(&WorkingTrack::chi2,_2) );
  // sort by decreasing number of LHCb
  std::stable_sort( tempTracks.begin(), tempTracks.end(),
                    bind(&WorkingTrack::nLHCbIDs,_1) > bind(&WorkingTrack::nLHCbIDs,_2) );
  // sort by type (lowest rank first)
  std::stable_sort( tempTracks.begin(), tempTracks.end(),
                    bind(&WorkingTrack::trackTypeRank,_1) < bind(&WorkingTrack::trackTypeRank,_2) );

  for ( WorkingTrack::Vector::iterator iterW = tempTracks.begin();
        iterW != tempTracks.end(); ++iterW )
  {
    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << "Trying track key=" << iterW->track->key()
                << " " << iterW->track->history()
                << " chi2=" << iterW->chi2()
                << " nMeas=" << iterW->nLHCbIDs()
                << endmsg;
    }

    // skips if already tagged as a rejected clone
    if ( iterW->clone )
    {
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << " -> Already flagged as a clone. Skipping" << endmsg;
      continue;
    }

    // pick up the clones
    Track* cloneTrack = linker.first( iterW->track );
    while ( cloneTrack != NULL )
    {
      // double check track is not flag as clone of itself !!
      if ( cloneTrack != iterW->track )
      {
        const double dist = linker.weight();
        if ( msgLevel(MSG::VERBOSE) )
        {
          verbose() << " -> Clone track key=" << cloneTrack->key() << " " << cloneTrack->history()
                    << " dist=" << dist
                    << endmsg;
        }
        // check clone cut
        if ( dist < m_cloneCut )
        {
          WorkingTrack::Vector::iterator iter = std::find_if ( tempTracks.begin(),
                                                               tempTracks.end(),
                                                               bind(&WorkingTrack::sameTrack,_1,cloneTrack) );
          if ( iter != tempTracks.end() )
          {
            iter->clone = true;
            if ( iter->track->info(LHCb::Track::CloneDist,1e99) > dist )
            {
              if ( msgLevel(MSG::VERBOSE) )
              {
                verbose() << "  -> Flagging track " << iter->track
                          << " key=" << iter->track->key() << " "
                          << iter->track->history() << " as a clone" << endmsg;
              }
              iter->track->addInfo( LHCb::Track::CloneDist, dist );
            }
          }

        } // passed cut
      }
      else
      {
        Error( "Track flagged as clone of itself !!" ).ignore();
      }
      cloneTrack = linker.next();
    } // clone track

  } // iterW

  return StatusCode::SUCCESS;
}

int TrackCloneCleaner::WorkingTrack::trackTypeRank() const
{
  // CRJ : Should probably make type 'ranking' configurable via options ?
  if ( track->type() == LHCb::Track::Long       ) return 0;
  if ( track->type() == LHCb::Track::Downstream ) return 1;
  if ( track->type() == LHCb::Track::Upstream   ) return 2;
  if ( track->type() == LHCb::Track::Ttrack     ) return 3;
  if ( track->type() == LHCb::Track::Velo       ) return 4;
  if ( track->type() == LHCb::Track::VeloR      ) return 5;
  if ( track->type() == LHCb::Track::Muon       ) return 6;
  return 999;
}
