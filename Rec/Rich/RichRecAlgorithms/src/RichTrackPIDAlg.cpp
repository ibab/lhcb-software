
//--------------------------------------------------------------------------
/** @file RichRecInit.cpp
 *
 *  Implementation file for algorithm class : RichRecInit
 *
 *  CVS Log :-
 *  $Id: RichTrackPIDAlg.cpp,v 1.3 2009-07-30 11:13:48 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   17/04/2002
 */
//--------------------------------------------------------------------------

// local
#include "RichTrackPIDAlg.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//--------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TrackPIDAlg );

// Standard constructor, initializes variables
TrackPIDAlg::TrackPIDAlg( const std::string& name,
                          ISvcLocator* pSvcLocator )
  : Rich::Rec::AlgBase ( name, pSvcLocator ),
    m_pidTool          ( NULL ),
    m_trSelector       ( NULL )
{
  // Context specific track locations
  declareProperty( "PIDToolName", m_pidToolName = "RichPIDTool" );
  declareProperty( "TracksLocation", m_trTracksLocation = "" );
  declareProperty( "NTracksAtOnce",  m_nAtOnce = 1 );
}

// Destructor
TrackPIDAlg::~TrackPIDAlg() {}

StatusCode TrackPIDAlg::initialize()
{
  const StatusCode sc = RichRecAlgBase::initialize();
  if ( sc.isFailure() ) return sc;

  if ( m_trTracksLocation.empty() )
  { return Error( "Input track location is not set" ); }

  // load tools
  acquireTool( m_pidToolName,   m_pidTool          );
  acquireTool( "TrackSelector", m_trSelector, this );

  info() << "Will PID " << m_nAtOnce << " track(s) at a time" << endmsg;

  return sc;
}

StatusCode TrackPIDAlg::execute()
{
  // Event Status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // working list of tracks to PID in one go
  LHCb::Track::ConstVector tracksToPID;
  LHCb::RichPID::ConstVector richpids;

  // load and select tracks
  const LHCb::Tracks * tracks = get<LHCb::Tracks>( m_trTracksLocation );
  for ( LHCb::Tracks::const_iterator iTrack = tracks->begin();
        iTrack != tracks->end(); ++iTrack )
  {
    // select tracks
    if ( !m_trSelector->trackSelected(*iTrack) ) continue;

    // add to list of tracks to PID
    tracksToPID.push_back(*iTrack);

    // if correct number, PID them
    if ( tracksToPID.size() == m_nAtOnce ) 
    {
      m_pidTool->pids(tracksToPID,richpids);
      tracksToPID.clear();
    }
  }

  // any left over to do ?
  if ( !tracksToPID.empty() ) m_pidTool->pids(tracksToPID,richpids);

  return StatusCode::SUCCESS;
}
