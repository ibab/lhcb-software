
//-------------------------------------------------------------------------------------
/** @file RichTrackCreatorFromRecoTracks.cpp
 *
 *  Implementation file for tool : RichTrackCreatorFromRecoTracks
 *
 *  CVS Log :-
 *  $Id: RichTrackCreatorFromRecoTracks.cpp,v 1.7 2006-02-16 16:15:36 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-------------------------------------------------------------------------------------

// local
#include "RichTrackCreatorFromRecoTracks.h"

// namespaces
using namespace LHCb;

//-------------------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichTrackCreatorFromRecoTracks>          s_factory ;
const        IToolFactory& RichTrackCreatorFromRecoTracksFactory = s_factory ;

// Standard constructor
RichTrackCreatorFromRecoTracks::
RichTrackCreatorFromRecoTracks( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : RichTrackCreatorBase   ( type, name, parent ),
    m_trTracks             ( 0 ),
    m_rayTrace             ( 0 ),
    m_smartIDTool          ( 0 ),
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
  acquireTool( "RichRayTracing",          m_rayTrace    );
  acquireTool( "RichExpectedTrackSignal", m_signal      );
  acquireTool( "RichSmartIDTool",         m_smartIDTool, 0, true );
  acquireTool( m_trSegToolNickName,       m_segMaker    );
  if ( m_buildHypoRings ) acquireTool( "RichMassHypoRings", m_massHypoRings );

  // Configure the ray-tracing mode
  m_traceMode.setDetPrecision      ( RichTraceMode::circle );
  m_traceMode.setDetPlaneBound     ( RichTraceMode::loose  );
  m_traceMode.setForcedSide        ( false                 );
  m_traceMode.setOutMirrorBoundary ( false                 );
  m_traceMode.setMirrorSegBoundary ( false                 );

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
    return 0;
  }

  // unique ?
  const bool trUnique = trTrack->checkFlag(LHCb::Track::Unique);

  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << "Trying Track " << trTrack->key()
              << " type " << trType << ", unique=" << trUnique
              << ", charge=" << trTrack->charge()
              << endreq;
  }

  // Is track a usable type
  if ( !Rich::Track::isUsable(trType) ) return NULL;

  // Track selection
  if ( !trackSelector().trackSelected(trTrack) ) return NULL;

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

    // New track object pointer
    RichRecTrack * newTrack = NULL;

    // Form the RichRecSegments for this track
    std::vector<RichTrackSegment*> segments;
    const int Nsegs = m_segMaker->constructSegments( trTrack, segments );
    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << " Found " << Nsegs << " radiator segments" << endreq;
    }
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

        // make a new RichRecSegment from this RichTrackSegment
        // takes ownership of RichTrackSegment* *iSeg - responsible for deletion
        RichRecSegment * newSegment = segmentCreator()->newSegment( *iSeg, newTrack );

        // Get PD panel impact point
        Gaudi::XYZPoint hitPoint;
        const Gaudi::XYZVector & trackDir = (*iSeg)->bestMomentum();
        if ( m_rayTrace->traceToDetectorWithoutEff( (*iSeg)->rich(),
                                                    (*iSeg)->bestPoint(),
                                                    trackDir,
                                                    hitPoint,
                                                    m_traceMode ) )
        {
          // set global hit point
          newSegment->setPdPanelHitPoint( hitPoint );

          // Get PD panel hit point in local coordinates
          // need to do before test below, since potentially needed by geom eff tool
          // need to make this data "on-demand" to avoid this sort of thing
          newSegment->setPdPanelHitPointLocal( m_smartIDTool->globalToPDPanel(hitPoint) );

          // test if this segment has valid information
          if ( m_signal->hasRichInfo(newSegment) )
          {

            if ( msgLevel(MSG::VERBOSE) )
              verbose() << " TrackSegment in " << (*iSeg)->radiator() << " selected" << endreq;

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
              verbose() << " TrackSegment in " << (*iSeg)->radiator() << " rejected" << endreq;
            }
            delete newSegment;
            newSegment = NULL;
          }

        }
        else
        {
          if ( msgLevel(MSG::VERBOSE) )
          {
            verbose() << " TrackSegment in " << (*iSeg)->radiator() << " rejected" << endreq;
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
        newTrack->setVertexMomentum( trTrack->p() );

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
