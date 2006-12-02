
//---------------------------------------------------------------------------
/** @file RichGhostTrackMoni.cpp
 *
 *  Implementation file for algorithm class : RichGhostTrackMoni
 *
 *  $Id: RichGhostTrackMoni.cpp,v 1.2 2006-12-02 15:26:24 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

// local
#include "RichGhostTrackMoni.h"

//---------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_NAMESPACE_ALGORITHM_FACTORY( Rich, RichGhostTrackMoni );

// Standard constructor, initializes variables
Rich::RichGhostTrackMoni::RichGhostTrackMoni( const std::string& name,
                                              ISvcLocator* pSvcLocator )
  : RichRecTupleAlgBase ( name, pSvcLocator ),
    m_trSelector        ( NULL ),
    m_richRecMCTruth    ( NULL )
{
  // job opts
}

// Destructor
Rich::RichGhostTrackMoni::~RichGhostTrackMoni() {}

// Initialize
StatusCode Rich::RichGhostTrackMoni::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecTupleAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // tools
  acquireTool( "TrackSelector", m_trSelector, this );
  acquireTool( "RichRecMCTruthTool", m_richRecMCTruth );

  return sc;
}

// Main execution
StatusCode Rich::RichGhostTrackMoni::execute()
{

  // Do we have track MC truth
  if ( m_richRecMCTruth->trackToMCPAvailable() )
  {

    // loop over tracks
    for ( LHCb::RichRecTracks::const_iterator iT = richTracks()->begin();
          iT != richTracks()->end(); ++iT )
    {
      // is this track a ghost ?
      const LHCb::MCParticle * mcP = m_richRecMCTruth->mcParticle(*iT);
      const bool isGhost = (mcP == NULL);

      // points to segments
      LHCb::RichRecSegment * aeroSeg  = (*iT)->segmentInRad(Rich::Aerogel);
      LHCb::RichRecSegment * c4f10Seg = (*iT)->segmentInRad(Rich::Rich1Gas);
      LHCb::RichRecSegment * cf4Seg   = (*iT)->segmentInRad(Rich::Rich2Gas);

      // Count photons
      const unsigned int nAero  = ( aeroSeg  ? aeroSeg  ->richRecPhotons().size() : 0 );
      const unsigned int nC4F10 = ( c4f10Seg ? c4f10Seg ->richRecPhotons().size() : 0 );
      const unsigned int nCF4   = ( cf4Seg   ? cf4Seg   ->richRecPhotons().size() : 0 );

      // fill a tuple with track info
      Tuple tuple = nTuple( "ghosts", "TrackGeomTuple" );

      tuple->column( "IsGhost", (int)isGhost );

      tuple->write();

    }

  } // have MC truth
  else
  {
    // Print one message only
    Warning( "No Track MC truth available -> Cannot find ghost tracks.", 1 );
  }

  return StatusCode::SUCCESS;
}

// Finalize
StatusCode Rich::RichGhostTrackMoni::finalize()
{
  // Execute base class method
  return RichRecTupleAlgBase::finalize();
}
