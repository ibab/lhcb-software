// $Id: RichTrackCreatorFromCheatedTrStoredTracks.cpp,v 1.3 2004-02-02 14:24:41 jonesc Exp $

// local
#include "RichTrackCreatorFromCheatedTrStoredTracks.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichTrackCreatorFromCheatedTrStoredTracks
//
// 15/09/2003 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichTrackCreatorFromCheatedTrStoredTracks>          s_factory ;
const        IToolFactory& RichTrackCreatorFromCheatedTrStoredTracksFactory = s_factory ;

// Standard constructor
RichTrackCreatorFromCheatedTrStoredTracks::RichTrackCreatorFromCheatedTrStoredTracks( const std::string& type,
                                                                                      const std::string& name,
                                                                                      const IInterface* parent )
  : RichRecToolBase( type, name, parent ),
    m_trTracks    ( 0 ),
    m_tracks      ( 0 ),
    m_rayTrace    ( 0 ),
    m_segMaker    ( 0 ),
    m_smartIDTool ( 0 ),
    m_signal      ( 0 )
{

  declareInterface<IRichTrackCreator>(this);

  declareProperty( "TrStoredTracksLocation",
                   m_trTracksLocation = TrStoredTrackLocation::Default );
  declareProperty( "RichRecTrackLocation",
                   m_richRecTrackLocation = RichRecTrackLocation::Default );
  declareProperty( "TrackAsctName", m_trAsctName = "TrackToMCP" );
  declareProperty( "TrackAsctType",
                   m_trAsctType = "AssociatorWeighted<TrStoredTrack,MCParticle,double>");
  declareProperty( "SkipNonUniqueTracks", m_skipNonUnique = true );

}

StatusCode RichTrackCreatorFromCheatedTrStoredTracks::initialize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Initialize" << endreq;

  // Sets up various tools and services
  if ( !RichRecToolBase::initialize() ) return StatusCode::FAILURE;

  // Acquire instances of tools
  acquireTool( "RichRayTracing",          m_rayTrace    );
  acquireTool( "RichDetTrSegMaker",       m_segMaker    );
  acquireTool( "RichExpectedTrackSignal", m_signal      );
  acquireTool( "RichSmartIDTool",         m_smartIDTool );

  // Get pointer to Track MC truth
  if ( !toolSvc()->retrieveTool(m_trAsctType,m_trAsctName,m_trackToMCP) ) {
    msg << MSG::ERROR << m_trAsctName+" not found" << endreq;
    return StatusCode::FAILURE;
  }

  // Setup incident services
  IIncidentSvc * incSvc;
  if ( !serviceLocator()->service( "IncidentSvc", incSvc, true ) ) {
    msg << MSG::ERROR << "IncidentSvc not found" << endreq;
    return StatusCode::FAILURE;
  } else {
    incSvc->addListener( this, "BeginEvent" ); // Informed of a new event
    incSvc->release();
  }

  // Informational printout
  msg << MSG::DEBUG
      << " Using MCParticles associated to TrStoredTracks" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode RichTrackCreatorFromCheatedTrStoredTracks::finalize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Finalize" << endreq;

  // release services and tools
  releaseTool( m_rayTrace    );
  releaseTool( m_segMaker    );
  releaseTool( m_signal      );
  releaseTool( m_trackToMCP  );
  releaseTool( m_smartIDTool );

  // Execute base class method
  return RichRecToolBase::finalize();
}

// Method that handles various Gaudi "software events"
void RichTrackCreatorFromCheatedTrStoredTracks::handle ( const Incident& incident ) {

  if ( "BeginEvent" == incident.type() ) {

    // Initialise navigation data
    m_trackDone.clear();
    m_allDone = false;
    m_trTracks = 0;

    SmartDataPtr<RichRecTracks> tdsTracks( eventSvc(),
                                           m_richRecTrackLocation );
    if ( !tdsTracks ) {

      // Reinitialise the Photon Container
      m_tracks = new RichRecTracks();

      // Register new RichRecPhoton container to Gaudi data store
      if ( !eventSvc()->registerObject(m_richRecTrackLocation, m_tracks) ) {
        MsgStream msg( msgSvc(), name() );
        msg << MSG::ERROR << "Failed to register RichRecTracks at "
            << m_richRecTrackLocation << endreq;
      }

    } else {

      // Set smartref to TES track container
      m_tracks = tdsTracks;

      // Remake local track reference map
      for ( RichRecTracks::const_iterator iTrack = tdsTracks->begin();
            iTrack != tdsTracks->end();
            ++iTrack ) {
        m_trackDone[(*iTrack)->key()] = true;
      }

    }

  } // end begin event if

}

const StatusCode
RichTrackCreatorFromCheatedTrStoredTracks::newTracks() const {

  if ( ! m_allDone ) {
    m_allDone = true;

    // Load tracks
    if ( !m_trTracks ) { if ( !loadTrStoredTracks() ) return StatusCode::FAILURE; }

    // Iterate over all reco tracks, and create new RichRecTracks
    for ( TrStoredTracks::const_iterator track = m_trTracks->begin();
          track != m_trTracks->end();
          ++track) { newTrack( *track ); } // Make new RichRecTrack

  }

  return StatusCode::SUCCESS;
}

const long RichTrackCreatorFromCheatedTrStoredTracks::nInputTracks() const
{
  if ( !m_trTracks ) { loadTrStoredTracks(); }
  return ( m_trTracks ? m_trTracks->size() : 0 );
}

const bool RichTrackCreatorFromCheatedTrStoredTracks::loadTrStoredTracks() const
{

  // Obtain smart data pointer to TrStoredTracks
  SmartDataPtr<TrStoredTracks> tracks( eventSvc(), m_trTracksLocation );
  if ( !tracks ) {
    MsgStream msg( msgSvc(), name() );
    msg << MSG::ERROR << "Failed to locate TrStoredTracks at "
        << m_trTracksLocation << endreq;
    return false;
  }
  m_trTracks = tracks;
  if ( msgLevel(MSG::DEBUG) ) {
    MsgStream msg( msgSvc(), name() );
    msg << MSG::DEBUG << "located " << tracks->size() << " TrStoredTracks at "
        << m_trTracksLocation << endreq;
  }

  return true;
}

RichRecTrack *
RichTrackCreatorFromCheatedTrStoredTracks::newTrack ( const ContainedObject * obj ) const
{

  const TrStoredTrack * trTrack = dynamic_cast<const TrStoredTrack*>(obj);
  if ( !trTrack ) return NULL;
  if ( m_skipNonUnique && !trTrack->unique() ) return NULL;
  const MCParticle * mcP = m_trackToMCP->associatedFrom( trTrack );
  if ( !mcP ) return NULL;

  unsigned long key = trTrack->key();

  // See if this RichRecTrack already exists
  if ( m_trackDone[key] ) {
    return static_cast<RichRecTrack*>(m_tracks->object(key));
  } else {

    RichRecTrack * newTrack = NULL;

    // Form the RichRecSegments for this track
    // Here use MC information but pretend parent is a TrStoredTrack
    std::vector<RichTrackSegment> segments;
    int Nsegs = m_segMaker->constructSegments( *mcP, segments );
    if ( 0 < Nsegs ) {

      // Form a new RichRecTrack
      newTrack = new RichRecTrack();

      // Configure TrackID for TrStoredTrack
      newTrack->trackID().initTrStoredTrack( trTrack );

      bool keepTrack = false;

      for ( std::vector<RichTrackSegment>::iterator iSeg = segments.begin();
            iSeg != segments.end(); ++iSeg ) {

        // make a new RichRecSegment from this RichTrackSegment
        RichRecSegment * newSegment = segmentCreator()->newSegment( *iSeg, newTrack );

        // Get PD panel impact point
        HepPoint3D & hitPoint = newSegment->pdPanelHitPoint();
        HepVector3D trackDir = (*iSeg).bestMomentum();
        if ( m_rayTrace->traceToDetectorWithoutEff( (*iSeg).rich(),
                                                    (*iSeg).bestPoint(),
                                                    trackDir,
                                                    hitPoint,
                                                    DeRichPDPanel::loose )
             && m_signal->hasRichInfo(newSegment) ) {

          // keep track
          keepTrack = true;

          // Save this segment
          segmentCreator()->saveSegment( newSegment );

          // Add to the SmartRefVector of RichSegments for this RichRecTrack
          newTrack->addToRichRecSegments( newSegment );

          // set radiator info
          const Rich::RadiatorType rad = (*iSeg).radiator();
          if      ( Rich::Aerogel  == rad ) { newTrack->setInRICH1(1); newTrack->setInAerogel(1); }
          else if ( Rich::Rich1Gas == rad ) { newTrack->setInRICH1(1); newTrack->setInGas1(1);    }
          else if ( Rich::Rich2Gas == rad ) { newTrack->setInRICH2(1); newTrack->setInGas2(1);    }

          // Get PD panel hit point in local coordinates
          newSegment->pdPanelHitPointLocal() = m_smartIDTool->globalToPDPanel(hitPoint);

        } else {
          delete newSegment;
          newSegment = NULL;
        }

      }

      if ( keepTrack ) {

        // give to container
        m_tracks->insert( newTrack, key );

        // Set vertex momentum
        newTrack->setVertexMomentum( mcP->momentum().vect().mag() );

        // Set parent information
        newTrack->setParentTrack( trTrack );

      } else {
        delete newTrack;
        newTrack = NULL;
      }

    } // end segments if

    // Add to reference map
    m_trackDone[key] = true;

    return newTrack;
  }

}

RichRecTracks * RichTrackCreatorFromCheatedTrStoredTracks::richTracks() const {
  return m_tracks;
}

