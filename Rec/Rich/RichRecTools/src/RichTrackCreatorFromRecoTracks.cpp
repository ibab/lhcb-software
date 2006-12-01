
//-------------------------------------------------------------------------------------
/** @file RichTrackCreatorFromRecoTracks.cpp
 *
 *  Implementation file for tool : RichTrackCreatorFromRecoTracks
 *
 *  CVS Log :-
 *  $Id: RichTrackCreatorFromRecoTracks.cpp,v 1.15 2006-12-01 17:05:09 cattanem Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-------------------------------------------------------------------------------------

// local
#include "RichTrackCreatorFromRecoTracks.h"

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// namespaces
using namespace LHCb;

//-------------------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( RichTrackCreatorFromRecoTracks );

// Standard constructor
RichTrackCreatorFromRecoTracks::
RichTrackCreatorFromRecoTracks( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : RichTrackCreatorBase   ( type, name, parent ),
    m_trTracks             ( 0 ),
    m_massHypoRings        ( 0 ),
    m_segMaker             ( 0 ),
    m_signal               ( 0 ),
    m_trTracksLocation     ( TrackLocation::Default         ),
    m_trSegToolNickName    ( "RichTrSegMakerFromRecoTracks" ),
    m_allDone              ( false ),
    m_buildHypoRings       ( false )
{

  // declare interface for this tool
  declareInterface<IRichTrackCreator>(this);

  // job options
  declareProperty( "TracksLocation",           m_trTracksLocation   );
  declareProperty( "BuildMassHypothesisRings", m_buildHypoRings     );
  declareProperty( "TrackSegmentTool",         m_trSegToolNickName  );

}

StatusCode RichTrackCreatorFromRecoTracks::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichTrackCreatorBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichExpectedTrackSignal", m_signal      );
  acquireTool( m_trSegToolNickName,       m_segMaker,    this );
  if ( m_buildHypoRings ) 
  { 
    acquireTool( "RichMassHypoRings", m_massHypoRings );
    info() << "Will create Mass hypothesis rings for each track" << endreq;
  }

  return sc;
}

StatusCode RichTrackCreatorFromRecoTracks::finalize()
{
  // Execute base class method
  return RichTrackCreatorBase::finalize();
}

const StatusCode RichTrackCreatorFromRecoTracks::newTracks() const
{

  if ( !m_allDone )
  {
    m_allDone = true;

    // Iterate over all reco tracks, and create new RichRecTracks
    richTracks()->reserve( nInputTracks() );
    const Tracks * tracks = trTracks();
    for ( Tracks::const_iterator track = tracks->begin();
          track != tracks->end(); ++track )
    {
      newTrack( *track );
    }

  }

  return StatusCode::SUCCESS;
}

const long RichTrackCreatorFromRecoTracks::nInputTracks() const
{
  const Tracks * tracks = trTracks();
  return ( tracks ? tracks->size() : 0 );
}

const Tracks *
RichTrackCreatorFromRecoTracks::trTracks() const
{
  if ( !m_trTracks )
  {
    // Obtain smart data pointer to Tracks
    m_trTracks = get<Tracks>( m_trTracksLocation );
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "located " << m_trTracks->size() << " Tracks at "
              << m_trTracksLocation << endreq;
    }
  }

  return m_trTracks;
}

// Forms a new RichRecTrack object from a Track
RichRecTrack *
RichTrackCreatorFromRecoTracks::newTrack ( const ContainedObject * obj ) const
{

  // Is this a Track ?
  const Track * trTrack = dynamic_cast<const Track*>(obj);
  if ( !trTrack )
  {
    Warning( "Input data object is not of type 'Track'" );
    return NULL;
  }

  // flag the tool as having been used this event
  m_hasBeenCalled = true;

  // track type
  Rich::Track::Type trType = Rich::Track::Unknown;
  try { trType = Rich::Track::type(trTrack); }
  // Catch exceptions ( track type unknown )
  catch ( const GaudiException & expt )
  {
    Error( expt.message() );
    return NULL;
  }

  // unique ?
  const bool trUnique = !trTrack->checkFlag(LHCb::Track::Clone);

  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << "Trying Track " << trTrack->key()
              << " type " << trType << ", unique=" << trUnique
              << ", charge=" << trTrack->charge()
              << endreq;
  }

  // Is track a usable type
  if ( !Rich::Track::isUsable(trType) ) return NULL;

  // Get reference to track stats object
  TrackCount & tkCount = trackStats().trackStats(trType,trUnique);

  // See if this RichRecTrack already exists
  if ( bookKeep() && m_trackDone[trTrack->key()] )
  {
    return richTracks()->object(trTrack->key());
  }
  else
  {

    // count tried tracks
    ++tkCount.triedTracks;

    // Track selection
    if ( !trackSelector().trackSelected(trTrack) ) return NULL;

    // New track object pointer
    RichRecTrack * newTrack = NULL;

    // Form the RichRecSegments for this track
    std::vector<RichTrackSegment*> segments;
    const int Nsegs = m_segMaker->constructSegments( trTrack, segments );
    
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << " Found " << Nsegs << " radiator segment(s)" << endreq;

    if ( 0 < Nsegs )
    {

      // Form a new RichRecTrack
      newTrack = new RichRecTrack();

      // Configure TrackID for this Track
      newTrack->trackID().initialiseFor( trTrack );

      bool keepTrack = false;
      for ( std::vector<RichTrackSegment*>::iterator iSeg = segments.begin();
            iSeg != segments.end(); ++iSeg )
      {
        if ( !(*iSeg) ) continue;

        if ( msgLevel(MSG::VERBOSE) )
          verbose() << "  -> Testing " << (*iSeg)->radiator() << " segment" << endreq;

        // make a new RichRecSegment from this RichTrackSegment
        // takes ownership of RichTrackSegment* *iSeg - responsible for deletion
        RichRecSegment * newSegment = segmentCreator()->newSegment( *iSeg, newTrack );

        // Get PD panel impact point
        if ( rayTraceHPDPanelPoints(**iSeg,newSegment).isSuccess() )
        {
          // test if this segment has valid information
          if ( m_signal->hasRichInfo(newSegment) )
          {

            if ( msgLevel(MSG::VERBOSE) )
              verbose() << "   -> TrackSegment in " << (*iSeg)->radiator() << " selected" << endreq;

            // keep track
            keepTrack = true;

            // Save this segment
            segmentCreator()->saveSegment( newSegment );

            // Add to the SmartRefVector of RichSegments for this RichRecTrack
            newTrack->addToRichRecSegments( newSegment );

            // set radiator info
            setDetInfo( newTrack, (*iSeg)->radiator() );

            // Set the average photon energy (for default hypothesis)
            newSegment->trackSegment()
              .setAvPhotonEnergy( m_signal->avgSignalPhotEnergy(newSegment,
                                                                newTrack->currentHypothesis()) );

            // make RichRecRings for the mass hypotheses if requested
            if ( m_buildHypoRings ) m_massHypoRings->newMassHypoRings( newSegment );

            // Count radiator segments
            tkCount.countRadiator( (*iSeg)->radiator() );

          }
          else
          {
            if ( msgLevel(MSG::VERBOSE) )
            {
              verbose() << "   -> TrackSegment has no RICH info -> rejected" << endreq;
            }
            delete newSegment;
            newSegment = NULL;
          }

        }
        else
        {
          if ( msgLevel(MSG::VERBOSE) )
          {
            verbose() << "   -> TrackSegment does not trace to an HPD panel -> rejected" << endreq;
          }
          delete newSegment;
          newSegment = NULL;
        }

      } // end loop over RichTrackSegments

      if ( keepTrack )
      {

        // give to container
        richTracks()->insert( newTrack, trTrack->key() );

        // Set vertex momentum
        newTrack->setVertexMomentum ( trTrack->p()  );
        newTrack->setVertexPt       ( trTrack->pt() );

        // chi2
        newTrack->setChi2PerDoF( trTrack->chi2PerDoF() );
        newTrack->setNDoF      ( trTrack->nDoF()       );

        // track charge
        newTrack->setCharge( trTrack->charge() );

        // Set parent information
        newTrack->setParentTrack( trTrack );

        // Count selected tracks
        ++tkCount.selectedTracks;

      }
      else
      {
        delete newTrack;
        newTrack = NULL;
      }

    } // end segments if

    // Add to reference map
    if ( bookKeep() ) m_trackDone[trTrack->key()] = true;

    return newTrack;
  }

}

void RichTrackCreatorFromRecoTracks::InitNewEvent()
{
  RichTrackCreatorBase::InitNewEvent();
  m_allDone  = false;
  m_trTracks = 0;
}
