// $Id: RichTrackCreatorFromTrStoredTracks.cpp,v 1.7 2003-11-25 14:06:41 jonrob Exp $

// local
#include "RichTrackCreatorFromTrStoredTracks.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichTrackCreatorFromTrStoredTracks
//
// 15/03/2002 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichTrackCreatorFromTrStoredTracks>          s_factory ;
const        IToolFactory& RichTrackCreatorFromTrStoredTracksFactory = s_factory ;

// Standard constructor
RichTrackCreatorFromTrStoredTracks::RichTrackCreatorFromTrStoredTracks( const std::string& type,
                                                                        const std::string& name,
                                                                        const IInterface* parent )
  : RichRecToolBase( type, name, parent ),
    m_tkPcut( Rich::Track::NTrTypes, 0 ) {

  declareInterface<IRichTrackCreator>(this);

  declareProperty( "TrStoredTracksLocation",
                   m_trTracksLocation = TrStoredTrackLocation::Default );
  declareProperty( "RichRecTrackLocation",
                   m_richRecTrackLocation = RichRecTrackLocation::Default );
  declareProperty( "SkipNonUniqueTracks", m_skipNonUnique = true );

  m_tkPcut[Rich::Track::Forward]  = 0;
  m_tkPcut[Rich::Track::Match]    = 0;
  m_tkPcut[Rich::Track::UpStream] = 0;
  m_tkPcut[Rich::Track::Seed]     = 0;
  m_tkPcut[Rich::Track::VeloTT]   = 0;
  declareProperty( "TrackMinPtotPerClass", m_tkPcut );

}

StatusCode RichTrackCreatorFromTrStoredTracks::initialize() {

  MsgStream msg( msgSvc(), name() );

  // Sets up various tools and services
  if ( !RichRecToolBase::initialize() ) return StatusCode::FAILURE;

  // Acquire instances of tools
  acquireTool( "RichSegmentCreator",      m_segCr      );
  acquireTool( "RichDetInterface",        m_richDetInt );
  acquireTool( "RichExpectedTrackSignal", m_signal     );

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
  msg << MSG::DEBUG << "Initialize :-" 
      << " Using TrStoredTracks" << endreq
      << " Ptot max cut per track type  = " << m_tkPcut << endreq;

  return StatusCode::SUCCESS;
}

StatusCode RichTrackCreatorFromTrStoredTracks::finalize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Finalize" << endreq;

  // release services and tools
  releaseTool( m_segCr );
  releaseTool( m_richDetInt );
  releaseTool( m_signal );

  // Execute base class method
  return RichRecToolBase::finalize();
}

// Method that handles various Gaudi "software events"
void RichTrackCreatorFromTrStoredTracks::handle ( const Incident& incident ) {

  if ( "BeginEvent" == incident.type() ) {

    // Initialise navigation data
    m_trackDone.clear();
    m_allDone = false;
    m_trTracks = 0;

    SmartDataPtr<RichRecTracks> tdsTracks( eventSvc(), m_richRecTrackLocation );
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

StatusCode RichTrackCreatorFromTrStoredTracks::newTracks() {

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

long RichTrackCreatorFromTrStoredTracks::nInputTracks()
{
  if ( !m_trTracks ) { loadTrStoredTracks(); }
  return ( m_trTracks ? m_trTracks->size() : 0 );
}

bool RichTrackCreatorFromTrStoredTracks::loadTrStoredTracks()
{

  // Obtain smart data pointer to TrStoredTracks
  SmartDataPtr<TrStoredTracks> tracks( eventSvc(), m_trTracksLocation );
  if ( !tracks ) {
    MsgStream msg( msgSvc(), name() );
    msg << MSG::ERROR << "Failed to locate TrStoredTracks at "
        << m_trTracksLocation << endreq;
    return StatusCode::FAILURE;
  }
  m_trTracks = tracks;
  if ( msgLevel(MSG::DEBUG) ) {
    MsgStream msg( msgSvc(), name() );
    msg << MSG::DEBUG << "located " << tracks->size() << " TrStoredTracks at "
        << m_trTracksLocation << endreq; 
  }

  return StatusCode::SUCCESS;
}

// Forms a new RichRecTrack object from a TrStoredTrack
RichRecTrack *
RichTrackCreatorFromTrStoredTracks::newTrack ( const ContainedObject * obj ) {

  const TrStoredTrack * trTrack = dynamic_cast<const TrStoredTrack*>(obj);
  if ( !trTrack ) return NULL;
  if ( m_skipNonUnique && !trTrack->unique() ) return NULL;

  unsigned long key = (int)trTrack->key();

  // See if this RichRecTrack already exists
  if ( m_trackDone[key] ) {
    return static_cast<RichRecTrack*>(m_tracks->object(key));
  } else {

    RichRecTrack * newTrack = NULL;

    // Form the RichRecSegments for this track
    std::vector<RichTrackSegment> segments;
    int Nsegs = m_richDetInt->constructSegments( *trTrack, segments );
    if ( 0 < Nsegs ) {

      // Find TrStoredTrack current state and momentum
      SmartRef<TrState> trackState = trTrack->closestState(-999999.);
      TrStateP * trackPState = dynamic_cast<TrStateP*>( static_cast<TrState*>(trackState) );
      if ( trackPState && 
           trackPState->p()/GeV > m_tkPcut[Rich::Track::type(trTrack)] ) {

        // Form a new RichRecTrack
        newTrack = new RichRecTrack();

        // Configure TrackID for TrStoredTrack
        newTrack->trackID().initTrStoredTrack( trTrack );

        bool keepTrack = false;

        for ( std::vector<RichTrackSegment>::iterator iSeg = segments.begin();
              iSeg != segments.end(); ++iSeg ) {

          // make a new RichRecSegment from this RichTrackSegment
          RichRecSegment * newSegment = m_segCr->newSegment( *iSeg, newTrack );

          // Get PD panel impact point
          HepPoint3D & hitPoint = newSegment->pdPanelHitPoint();
          HepVector3D trackDir = (*iSeg).bestMomentum();
          if ( m_richDetInt->traceToDetectorWithoutEff( (*iSeg).rich(),
                                                        (*iSeg).bestPoint(),
                                                        trackDir,
                                                        hitPoint,
                                                        DeRichPDPanel::loose )
               && m_signal->hasRichInfo(newSegment) ) {

            // keep track
            keepTrack = true;

            // Save this segment
            m_segCr->saveSegment( newSegment );

            // Add to the SmartRefVector of RichSegments for this RichRecTrack
            newTrack->addToRichRecSegments( newSegment );

            // set radiator info
            Rich::RadiatorType rad = (*iSeg).radiator();
            if ( Rich::Aerogel  == rad ) { newTrack->setInRICH1(1); newTrack->setInAerogel(1); }
            if ( Rich::Rich1Gas == rad ) { newTrack->setInRICH1(1); newTrack->setInGas1(1); }
            if ( Rich::Rich2Gas == rad ) { newTrack->setInRICH2(1); newTrack->setInGas2(1); }

          } else {
            delete newSegment;
            newSegment = NULL;
          }

        }

        if ( keepTrack ) {

          // give to container
          m_tracks->insert( newTrack, key );

          // Set vertex momentum
          newTrack->setVertexMomentum( trackPState->p() );

          // Set parent information
          newTrack->setParentTrack( trTrack );

        } else {
          delete newTrack;
          newTrack = NULL;
        }

      } // end track state if
    } // end segments if

    // Add to reference map
    m_trackDone[key] = true;

    return newTrack;
  }

}

RichRecTracks *& RichTrackCreatorFromTrStoredTracks::richTracks()
{
  return m_tracks;
}
