// $Id: RichTrackCreatorFromTrStoredTracks.cpp,v 1.16 2004-06-29 19:53:39 jonesc Exp $

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
RichTrackCreatorFromTrStoredTracks::
RichTrackCreatorFromTrStoredTracks( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : RichRecToolBase        ( type, name, parent ),
    m_tracks               ( 0 ),
    m_trTracks             ( 0 ),
    m_rayTrace             ( 0 ),
    m_smartIDTool          ( 0 ),
    m_massHypoRings        ( 0 ),
    m_segMaker             ( 0 ),
    m_signal               ( 0 ),
    m_trTracksLocation     ( TrStoredTrackLocation::Default ),
    m_richRecTrackLocation ( RichRecTrackLocation::Default  ),
    m_trSegToolNickName    ( "RichDetTrSegMaker"            ),
    m_allDone              ( false ),
    m_buildHypoRings       ( false ),
    m_nTracks              ( Rich::Track::NTrTypes, std::pair<unsigned,unsigned>(0,0) )
{

  // declare interface for this tool
  declareInterface<IRichTrackCreator>(this);

  // job options
  declareProperty( "TrStoredTracksLocation",   m_trTracksLocation        );
  declareProperty( "RichRecTrackLocation",     m_richRecTrackLocation    );
  declareProperty( "TrackMinPtotPerClass",     m_trSelector.minMomenta() );
  declareProperty( "TrackMaxPtotPerClass",     m_trSelector.maxMomenta() );
  declareProperty( "BuildMassHypothesisRings", m_buildHypoRings          );
  declareProperty( "TrackSegmentTool",         m_trSegToolNickName       );
  declareProperty( "TrackSelection", m_trSelector.selectedTrackTypes()   );

}

StatusCode RichTrackCreatorFromTrStoredTracks::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRayTracing",          m_rayTrace    );
  acquireTool( "RichExpectedTrackSignal", m_signal      );
  acquireTool( "RichSmartIDTool",         m_smartIDTool );
  acquireTool( m_trSegToolNickName,       m_segMaker    );
  if ( m_buildHypoRings ) acquireTool( "RichMassHypoRings", m_massHypoRings );

  // Configure track selector
  if ( !m_trSelector.configureTrackTypes() ) return StatusCode::FAILURE;

  // Setup incident services
  IIncidentSvc * incSvc = svc<IIncidentSvc>( "IncidentSvc", true );
  incSvc->addListener( this, IncidentType::BeginEvent );
  if (msgLevel(MSG::DEBUG)) incSvc->addListener( this, IncidentType::EndEvent );

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
  // Update prior to start of event. Used to re-initialise data containers
  if      ( IncidentType::BeginEvent == incident.type() ) { InitNewEvent(); }
  // Debug printout at the end of each event
  else if ( msgLevel(MSG::DEBUG) && IncidentType::EndEvent == incident.type() )
    {
      // Print out of track count
      unsigned nTotTried(0), nTotSel(0);
      {for ( TrackTypeCount::iterator i = m_nTracks.begin();
             i != m_nTracks.end(); ++i ) {
        nTotTried += (*i).first;
        nTotSel   += (*i).second;
      }}
      debug() << "Selected " << nTotSel << "/" << nTotTried << " TrStoredTracks :";
      for ( unsigned iTk = 0; iTk < Rich::Track::NTrTypes; ++iTk ) {
        debug() << " " << (Rich::Track::Type)iTk << "=(" << m_nTracks[iTk].second
                << "/" << m_nTracks[iTk].first << ")";
      }
      debug() << endreq;
    }
}

const StatusCode RichTrackCreatorFromTrStoredTracks::newTracks() const {

  if ( ! m_allDone ) {
    m_allDone = true;

    // Iterate over all reco tracks, and create new RichRecTracks
    richTracks()->reserve( trStoredTracks()->size() );
    for ( TrStoredTracks::const_iterator track = trStoredTracks()->begin();
          track != trStoredTracks()->end();
          ++track) { newTrack( *track ); } // Make new RichRecTrack

  }

  return StatusCode::SUCCESS;
}

const long RichTrackCreatorFromTrStoredTracks::nInputTracks() const
{
  return ( trStoredTracks() ? trStoredTracks()->size() : 0 );
}

const TrStoredTracks *
RichTrackCreatorFromTrStoredTracks::trStoredTracks() const
{
  if ( !m_trTracks ) {
    // Obtain smart data pointer to TrStoredTracks
    m_trTracks = get<TrStoredTracks>( m_trTracksLocation );
    debug() << "located " << m_trTracks->size() << " TrStoredTracks at "
            << m_trTracksLocation << endreq;
  }

  return m_trTracks;
}

// Forms a new RichRecTrack object from a TrStoredTrack
RichRecTrack *
RichTrackCreatorFromTrStoredTracks::newTrack ( const ContainedObject * obj ) const
{

  // Is this a TrStoredTrack ?
  const TrStoredTrack * trTrack = dynamic_cast<const TrStoredTrack*>(obj);
  if ( !trTrack ) return NULL;

  // track type
  const Rich::Track::Type trType = Rich::Track::type(trTrack);

  if ( msgLevel(MSG::VERBOSE) ) {
    verbose() << "Trying TrStoredTrack " << trTrack->key()
              << " type " << trType << ", unique=" << trTrack->unique()
              << ", charge=" << trTrack->charge()
              << endreq;
  }

  // count tried tracks
  if ( msgLevel(MSG::DEBUG) ) ++(m_nTracks[trType].first);

  // Is track a usable type
  if ( !Rich::Track::isUsable(trType) ) return NULL;

  // Track selection
  if ( !m_trSelector.trackSelected(trTrack) ) return NULL;

  // See if this RichRecTrack already exists
  const unsigned long key = static_cast<unsigned long>(trTrack->key());
  if ( m_trackDone[key] ) {
    return richTracks()->object(key);
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
      if ( trackPState ) {
        if ( msgLevel(MSG::VERBOSE) ) {
          verbose() << " Ptot = " << trackPState->p()/GeV << " passed cut" << endreq;
        }

        // Form a new RichRecTrack
        newTrack = new RichRecTrack();

        // Configure TrackID for this TrStoredTrack
        newTrack->trackID().initialiseFor( trTrack );

        bool keepTrack = false;
        for ( std::vector<RichTrackSegment>::iterator iSeg = segments.begin();
              iSeg != segments.end(); ++iSeg ) {

          // make a new RichRecSegment from this RichTrackSegment
          RichRecSegment * newSegment = segmentCreator()->newSegment( *iSeg, newTrack );

          // Get PD panel impact point
          HepPoint3D & hitPoint = newSegment->pdPanelHitPoint();
          const HepVector3D & trackDir = (*iSeg).bestMomentum();
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

            // Set the average photon energy (for default hypothesis)
            newSegment->trackSegment()
              .setAvPhotonEnergy( m_signal->avgSignalPhotEnergy(newSegment,
                                                                newTrack->currentHypothesis()) );

            // make RichRecRings for the mass hypotheses if requested
            if ( m_buildHypoRings ) m_massHypoRings->newMassHypoRings( newSegment );

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

      } // end track state and momentum if

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

      debug() << "Found " << tdsTracks->size() << " pre-existing RichRecTracks in TES at "
              << m_richRecTrackLocation << endreq;

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
