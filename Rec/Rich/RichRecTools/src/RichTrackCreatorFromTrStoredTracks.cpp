// $Id: RichTrackCreatorFromTrStoredTracks.cpp,v 1.11 2004-04-20 13:34:15 jonesc Exp $

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
  : RichRecToolBase ( type, name, parent ),
    m_tracks        ( 0 ),
    m_trTracks      ( 0 ),
    m_rayTrace      ( 0 ),
    m_smartIDTool   ( 0 ),
    m_segMaker      ( 0 ),
    m_signal        ( 0 ),
    m_skipNonUnique ( true ),
    m_allDone       ( false ),
    m_tkPcut        ( Rich::Track::NTrTypes, 0 ),
    m_nTracks       ( Rich::Track::NTrTypes, std::pair<unsigned,unsigned>(0,0) )
{

  // declare interface for this tool
  declareInterface<IRichTrackCreator>(this);

  // job options
  declareProperty( "TrStoredTracksLocation",
                   m_trTracksLocation = TrStoredTrackLocation::Default );
  declareProperty( "RichRecTrackLocation",
                   m_richRecTrackLocation = RichRecTrackLocation::Default );
  declareProperty( "SkipNonUniqueTracks", m_skipNonUnique = true );
  declareProperty( "TrackMinPtotPerClass", m_tkPcut );

}

StatusCode RichTrackCreatorFromTrStoredTracks::initialize() 
{
  // Sets up various tools and services
  StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRayTracing",          m_rayTrace    );
  acquireTool( "RichExpectedTrackSignal", m_signal      );
  acquireTool( "RichSmartIDTool",         m_smartIDTool );
  acquireTool( "RichDetTrSegMaker",       m_segMaker    );

  // Setup incident services
  IIncidentSvc * incSvc = svc<IIncidentSvc>( "IncidentSvc", true );
  incSvc->addListener( this, IncidentType::BeginEvent );
  incSvc->addListener( this, IncidentType::EndEvent   );

  // Make sure we are ready for a new event
  InitNewEvent();

  return StatusCode::SUCCESS;
}

StatusCode RichTrackCreatorFromTrStoredTracks::finalize() 
{
  // Execute base class method
  return RichRecToolBase::finalize();
}

// Method that handles various Gaudi "software events"
void RichTrackCreatorFromTrStoredTracks::handle ( const Incident & incident )
{
  if      ( IncidentType::BeginEvent == incident.type() ) { InitNewEvent(); }
  else if ( msgLevel(MSG::DEBUG) && IncidentType::EndEvent == incident.type() ) 
    {
      debug() << "Selected " << richTracks()->size() << " TrStoredTracks :";
      for ( int iTk = 0; iTk < Rich::Track::NTrTypes; ++iTk ) {
        debug() << " " << (Rich::Track::Type)iTk << "=(" << m_nTracks[iTk].second
                << "/" << m_nTracks[iTk].first << ")";
      }
      debug() << endreq;
    }
}

const StatusCode RichTrackCreatorFromTrStoredTracks::newTracks() const {

  if ( ! m_allDone ) {
    m_allDone = true;

    // Load tracks
    if ( !m_trTracks ) { if ( !loadTrStoredTracks() ) return StatusCode::FAILURE; }

    // Iterate over all reco tracks, and create new RichRecTracks
    richTracks()->reserve( m_trTracks->size() );
    for ( TrStoredTracks::const_iterator track = m_trTracks->begin();
          track != m_trTracks->end();
          ++track) { newTrack( *track ); } // Make new RichRecTrack

  }

  return StatusCode::SUCCESS;
}

const long RichTrackCreatorFromTrStoredTracks::nInputTracks() const
{
  if ( !m_trTracks ) { loadTrStoredTracks(); }
  return ( m_trTracks ? m_trTracks->size() : 0 );
}

const bool RichTrackCreatorFromTrStoredTracks::loadTrStoredTracks() const
{

  // Obtain smart data pointer to TrStoredTracks
  m_trTracks = get<TrStoredTracks>( m_trTracksLocation );
  debug() << "located " << m_trTracks->size() << " TrStoredTracks at "
          << m_trTracksLocation << endreq;

  return StatusCode::SUCCESS;
}

// Forms a new RichRecTrack object from a TrStoredTrack
RichRecTrack *
RichTrackCreatorFromTrStoredTracks::newTrack ( const ContainedObject * obj ) const
{

  // Is this a TrStoredTrack ?
  const TrStoredTrack * trTrack = dynamic_cast<const TrStoredTrack*>(obj);
  if ( !trTrack ) return NULL;
  if ( msgLevel(MSG::VERBOSE) ) {
    verbose() << "Trying TrStoredTrack " << trTrack->key()
              << " type " << Rich::Track::type(trTrack) << ", unique=" << trTrack->unique()
              << endreq;
  }

  // track type
  const Rich::Track::Type trType = Rich::Track::type(trTrack);
  if ( Rich::Track::Unknown == trType ) {
    Warning( "TrStoredTrack of unknown algorithm type");
    return NULL;
  }

  // count tried tracks
  if ( msgLevel(MSG::DEBUG) ) ++(m_nTracks[trType].first);

  // Is track a usable type
  if ( !Rich::Track::isUsable(trType) ) return NULL;

  // skip non-unique tracks if requested
  if ( m_skipNonUnique && !trTrack->unique() ) return NULL;

  // See if this RichRecTrack already exists
  const unsigned long key = static_cast<unsigned long>(trTrack->key());
  if ( m_trackDone[key] ) {
    return static_cast<RichRecTrack*>( richTracks()->object(key) );
  } else {

    RichRecTrack * newTrack = NULL;

    // Form the RichRecSegments for this track
    std::vector<RichTrackSegment> segments;
    const int Nsegs = m_segMaker->constructSegments( trTrack, segments );
    verbose() << " Found " << Nsegs << " radiator segments" << endreq;
    if ( 0 < Nsegs ) {

      // Find TrStoredTrack current state and momentum
      SmartRef<TrState> trackState = trTrack->closestState(-999999.);
      TrStateP * trackPState = dynamic_cast<TrStateP*>( static_cast<TrState*>(trackState) );
      if ( trackPState &&
           trackPState->p()/GeV > m_tkPcut[trType] ) {
        if ( msgLevel(MSG::VERBOSE) ) {
          verbose() << " Ptot = " << trackPState->p()/GeV << " passed cut" << endreq;
        }

        // Form a new RichRecTrack
        newTrack = new RichRecTrack();

        // Configure TrackID for TrStoredTrack
        newTrack->trackID().initialiseFor( trTrack );

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
                                                      DeRichHPDPanel::loose )
               && m_signal->hasRichInfo(newSegment) ) {
            if ( msgLevel(MSG::VERBOSE) ) 
              verbose() << " TrackSegment in " << (*iSeg).radiator() << " selected" << endreq;

            // keep track
            keepTrack = true;

            // Save this segment
            segmentCreator()->saveSegment( newSegment );

            // Add to the SmartRefVector of RichSegments for this RichRecTrack
            newTrack->addToRichRecSegments( newSegment );

            // set radiator info
            const Rich::RadiatorType rad = (*iSeg).radiator();
            if        ( Rich::Aerogel  == rad ) {
              newTrack->setInRICH1(true); newTrack->setInAerogel(true);
            } else if ( Rich::Rich1Gas == rad ) {
              newTrack->setInRICH1(true); newTrack->setInGas1(true);
            } else if ( Rich::Rich2Gas == rad ) {
              newTrack->setInRICH2(true); newTrack->setInGas2(true);
            }

            // Get PD panel hit point in local coordinates
            newSegment->pdPanelHitPointLocal() = m_smartIDTool->globalToPDPanel(hitPoint);

            // Set the average photon energy (for Pion hypothesis)
            newSegment->trackSegment().setAvPhotonEnergy( m_signal->avgSignalPhotEnergy(newSegment,Rich::Pion) );

          } else {
            if ( msgLevel(MSG::VERBOSE) ) 
              verbose() << " TrackSegment in " << (*iSeg).radiator() << " rejected" << endreq;
            delete newSegment;
            newSegment = NULL;
          }

        }

        if ( keepTrack ) {

          // give to container
          richTracks()->insert( newTrack, key );

          // Set vertex momentum
          newTrack->setVertexMomentum( trackPState->p() );

          // track charge
          newTrack->setCharge( trTrack->charge() );

          // Set parent information
          newTrack->setParentTrack( trTrack );

          // Count selected tracks 
          if ( msgLevel(MSG::DEBUG) ) ++(m_nTracks[trType].second);

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

RichRecTracks * RichTrackCreatorFromTrStoredTracks::richTracks() const
{
  if ( !m_tracks ) {
    SmartDataPtr<RichRecTracks> tdsTracks( evtSvc(), m_richRecTrackLocation );
    if ( !tdsTracks ) {

      // Reinitialise the track Container
      m_tracks = new RichRecTracks();

      // Register new RichRecTrack container to Gaudi data store
      put( m_tracks, m_richRecTrackLocation );

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
  }
  return m_tracks;
}
