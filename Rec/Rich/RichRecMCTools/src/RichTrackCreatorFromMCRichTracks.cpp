
//-------------------------------------------------------------------------------------
/** @file RichTrackCreatorFromMCRichTracks.cpp
 *
 *  Implementation file for tool : RichTrackCreatorFromMCRichTracks
 *
 *  CVS Log :-
 *  $Id: RichTrackCreatorFromMCRichTracks.cpp,v 1.1 2006-02-09 17:59:00 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-------------------------------------------------------------------------------------

// local
#include "RichTrackCreatorFromMCRichTracks.h"

// namespaces
using namespace LHCb;

//-------------------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichTrackCreatorFromMCRichTracks>          s_factory ;
const        IToolFactory& RichTrackCreatorFromMCRichTracksFactory = s_factory ;

// Standard constructor
RichTrackCreatorFromMCRichTracks::
RichTrackCreatorFromMCRichTracks( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : RichTrackCreatorBase   ( type, name, parent ),
    m_mcrTracks            ( 0 ),
    m_rayTrace             ( 0 ),
    m_smartIDTool          ( 0 ),
    m_massHypoRings        ( 0 ),
    m_segMaker             ( 0 ),
    m_signal               ( 0 ),
    m_mcrTracksLocation    ( MCRichTrackLocation::Default     ),
    m_trSegToolNickName    ( "RichTrSegMakerFromMCRichTracks" ),
    m_allDone              ( false ),
    m_buildHypoRings       ( false )
{
  // declare interface for this tool
  declareInterface<IRichTrackCreator>(this);

  // job options
  declareProperty( "TracksLocation",           m_mcrTracksLocation   );
  declareProperty( "BuildMassHypothesisRings", m_buildHypoRings     );
  declareProperty( "TrackSegmentTool",         m_trSegToolNickName  );
}

StatusCode RichTrackCreatorFromMCRichTracks::initialize()
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

StatusCode RichTrackCreatorFromMCRichTracks::finalize()
{
  // Execute base class method
  return RichTrackCreatorBase::finalize();
}

const StatusCode RichTrackCreatorFromMCRichTracks::newTracks() const
{

  if ( !m_allDone )
  {
    m_allDone = true;

    // Iterate over all reco tracks, and create new RichRecTracks
    richTracks()->reserve( nInputTracks() );
    const MCRichTracks * tracks = trTracks();
    for ( MCRichTracks::const_iterator track = tracks->begin();
          track != tracks->end(); ++track )
    {
      newTrack( *track );
    }

  }

  return StatusCode::SUCCESS;
}

const long RichTrackCreatorFromMCRichTracks::nInputTracks() const
{
  const MCRichTracks * tracks = trTracks();
  return ( tracks ? tracks->size() : 0 );
}

const MCRichTracks *
RichTrackCreatorFromMCRichTracks::trTracks() const
{
  if ( !m_mcrTracks )
  {
    // Obtain smart data pointer to Tracks
    m_mcrTracks = get<MCRichTracks>( m_mcrTracksLocation );
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "located " << m_mcrTracks->size() << " MCRichTracks at "
              << m_mcrTracksLocation << endreq;
    }
  }

  return m_mcrTracks;
}

// Forms a new RichRecTrack object from a Track
RichRecTrack *
RichTrackCreatorFromMCRichTracks::newTrack ( const ContainedObject * obj ) const
{

  // Is this a Track ?
  const MCRichTrack * mcrTrack = dynamic_cast<const MCRichTrack*>(obj);
  if ( !mcrTrack )
  {
    Warning( "Input data object is not of type 'MCRichTrack'" );
    return NULL;
  }

  // Pointer to underlying MCParticle
  const MCParticle * mcPart = mcrTrack->mcParticle();
  if ( !mcPart )
  {
    Warning( "MCRichTrack has null MCParticle reference" );
    return NULL;
  }

  // flag the tool as having been used this event
  m_hasBeenCalled = true;

  // track type
  Rich::Track::Type trType = Rich::Track::Unknown;
  try { trType = Rich::Track::type(mcrTrack); }
  // Catch exceptions ( track type unknown )
  catch ( const GaudiException & expt )
  {
    Error( expt.message() );
    return 0;
  }

  // unique ( by definition for MC ... )
  const bool trUnique = true;

  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << "Trying MCRichTrack " << mcrTrack->key() << endreq;
  }

  // Is track a usable type
  if ( !Rich::Track::isUsable(trType) ) return NULL;

  // Track selection
  //if ( !trackSelector().trackSelected(mcrTrack) ) return NULL;

  // Get reference to track stats object
  TrackCount & tkCount = trackStats().trackStats(trType,trUnique);

  // See if this RichRecTrack already exists
  if ( bookKeep() && m_trackDone[mcrTrack->key()] )
  {
    return richTracks()->object(mcrTrack->key());
  }
  else
  {

    // count tried tracks
    ++tkCount.triedTracks;

    // New track object pointer
    RichRecTrack * newTrack = NULL;

    // Form the RichRecSegments for this track
    std::vector<RichTrackSegment*> segments;
    const int Nsegs = m_segMaker->constructSegments( mcrTrack, segments );
    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << " Found " << Nsegs << " radiator segments" << endreq;
    }
    if ( 0 < Nsegs )
    {
      if ( msgLevel(MSG::VERBOSE) )
      {
        verbose() << " Ptot = " << mcPart->p()/GeV << " passed cut" << endreq;
      }

      // Form a new RichRecTrack
      newTrack = new RichRecTrack();

      // Configure TrackID for this Track
      newTrack->trackID().initialiseFor( mcrTrack );

      bool keepTrack = false;
      for ( std::vector<RichTrackSegment*>::iterator iSeg = segments.begin();
            iSeg != segments.end(); ++iSeg )
      {
        if ( !(*iSeg) ) continue;

        // make a new RichRecSegment from this RichTrackSegment
        // takes ownership of RichTrackSegment* : (*iSeg) is responsible for deletion
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
        richTracks()->insert( newTrack, mcrTrack->key() );

        // Set vertex momentum
        newTrack->setVertexMomentum( mcPart->p() );

        // track charge
        newTrack->setCharge( mcPart->particleID().threeCharge()/3 );

        // Set parent information
        newTrack->setParentTrack( mcrTrack );

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
    if ( bookKeep() ) m_trackDone[mcrTrack->key()] = true;

    return newTrack;
  }

}

void RichTrackCreatorFromMCRichTracks::InitNewEvent()
{
  RichTrackCreatorBase::InitNewEvent();
  m_allDone  = false;
  m_mcrTracks = 0;
}
