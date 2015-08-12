
//--------------------------------------------------------------------------
/** @file AddMissingMCRichTracksAlg.cpp
 *
 *  Implementation file for algorithm class : AddMissingMCRichTracksAlg
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//--------------------------------------------------------------------------

// local
#include "AddMissingMCRichTracksAlg.h"

// namespace
using namespace Rich::Rec::MC;

//--------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( AddMissingMCRichTracksAlg )

// Standard constructor, initializes variables
AddMissingMCRichTracksAlg::AddMissingMCRichTracksAlg( const std::string& name,
                                                      ISvcLocator* pSvcLocator )
  : Rich::Rec::AlgBase ( name, pSvcLocator ),
    m_richRecMCTruth   ( NULL ),
    m_mcTkCreator      ( NULL )
{
  declareProperty( "MCRichTracksLocation",
                   m_mcrTracksLocation = LHCb::MCRichTrackLocation::Default );
  declareProperty( "AddFraction", m_mcAddFrac = 1.0 );
}

// Destructor
AddMissingMCRichTracksAlg::~AddMissingMCRichTracksAlg() {}

// Initialize
StatusCode AddMissingMCRichTracksAlg::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = Rich::Rec::AlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // get tools
  acquireTool( "RichRecMCTruthTool", m_richRecMCTruth );
  acquireTool( "MissingMCTrackCreator", m_mcTkCreator, this );

  // random number generator
  IRndmGenSvc * randSvc = svc<IRndmGenSvc>( "RndmGenSvc", true );
  if ( !m_rndm.initialize( randSvc, Rndm::Flat(0.,1.) ) )
  {
    return Error( "Unable to create Random generator" );
  }

  info() << "Will add " << m_mcAddFrac*100
         << " % of missing MC tracks" << endmsg;

  return sc;
}

StatusCode AddMissingMCRichTracksAlg::execute()
{
  // find MCRichTracks
  const LHCb::MCRichTracks * mcrTracks = get<LHCb::MCRichTracks>( m_mcrTracksLocation );
  if ( !mcrTracks )
  {
    return Warning( "Failed to find MCRichTracks at '" + m_mcrTracksLocation 
                    + "' -> Cannot add any missing tracks" );
  }
  debug() << "Found " << mcrTracks->size() << " MCRichTracks at " << m_mcrTracksLocation << endmsg;

  // List of found MCRichTracks
  typedef std::set<const LHCb::MCRichTrack*> FoundMCTracks;
  FoundMCTracks foundMCTracks;

  // First, loop over all tracks currently in use
  // and find out those which have an MCRichTrack
  for ( LHCb::RichRecTracks::iterator track = richTracks()->begin();
        track != richTracks()->end(); ++track )
  {
    // Does this track have an MC track ?
    const LHCb::MCRichTrack * mcTk = m_richRecMCTruth->mcRichTrack(*track);
    if ( mcTk ) foundMCTracks.insert(mcTk);
  }

  // Now, loop over all MCRichTracks
  for ( LHCb::MCRichTracks::const_iterator mctrack = mcrTracks->begin();
        mctrack != mcrTracks->end(); ++mctrack )
  {
    // do we already have a track for this MCRichTrack
    if ( foundMCTracks.find(*mctrack) == foundMCTracks.end() )
    {
      verbose() << "MCRichTrack " << (*mctrack)->key()
                << " not already associated to a RichRecTrack";
      // No, so make a track for this one
      // toss a coin to only make a given fraction of tracks
      if ( m_rndm() < m_mcAddFrac )
      {
        verbose() << " -> Creating new track" << endmsg;
        m_mcTkCreator->newTrack(*mctrack);
      }
      else
      {
        verbose() << " -> NOT creating new track" << endmsg;
      }
    }
    else
    {
      verbose() << "MCRichTrack " << (*mctrack)->key()
                << " already associated to a RichRecTrack" << endmsg;
    }

  }

  return StatusCode::SUCCESS;
}

StatusCode AddMissingMCRichTracksAlg::finalize()
{
  // finalize random number generator
  m_rndm.finalize();
  // Execute base class method
  return Rich::Rec::AlgBase::finalize();
}
