
//--------------------------------------------------------------------------
/** @file AddMissingMCRichTracksAlg.cpp
 *
 *  Implementation file for algorithm class : AddMissingMCRichTracksAlg
 *
 *  CVS Log :-
 *  $Id: AddMissingMCRichTracksAlg.cpp,v 1.1.1.1 2007-10-26 10:34:19 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//--------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "AddMissingMCRichTracksAlg.h"

// namespace
using namespace Rich::Rec::MC;

//--------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( AddMissingMCRichTracksAlg );

// Standard constructor, initializes variables
AddMissingMCRichTracksAlg::AddMissingMCRichTracksAlg( const std::string& name,
                                                      ISvcLocator* pSvcLocator )
  : RichRecAlgBase ( name, pSvcLocator ),
    m_richRecMCTruth  ( NULL ),
    m_mcTkCreator     ( NULL )
{ 
  declareProperty( "MCRichTracksLocation", 
                   m_mcrTracksLocation = LHCb::MCRichTrackLocation::Default );
}

// Destructor
AddMissingMCRichTracksAlg::~AddMissingMCRichTracksAlg() {}

// Initialize
StatusCode AddMissingMCRichTracksAlg::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // get tools
  acquireTool( "RichRecMCTruthTool", m_richRecMCTruth );
  acquireTool( "MissingMCTrackCreator", m_mcTkCreator, this );

  return sc;
}

StatusCode AddMissingMCRichTracksAlg::execute()
{

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
  const LHCb::MCRichTracks * mcrTracks = get<LHCb::MCRichTracks>( m_mcrTracksLocation );
  debug() << "Found " << mcrTracks->size() << " MCRichTracks at " << m_mcrTracksLocation << endreq;
  for ( LHCb::MCRichTracks::const_iterator mctrack = mcrTracks->begin();
        mctrack != mcrTracks->end(); ++mctrack )
  {
    // do we already have a track for this MCRichTrack
    if ( foundMCTracks.find(*mctrack) != foundMCTracks.end() )
    {
      // No, so make a track for this one
      verbose() << "MCRichTrack " << (*mctrack)->key() 
                << " not already associated to a RichRecTrack -> Creating new track" << endreq;
      m_mcTkCreator->newTrack(*mctrack);
    }
    else
    {
      verbose() << "MCRichTrack " << (*mctrack)->key() 
                << " already associated to a RichRecTrack" << endreq;
    }
    
  }
  
  return StatusCode::SUCCESS;
}

