
//-------------------------------------------------------------------------------------
/** @file RichTrackCreatorFromTrStoredTracks.cpp
 *
 *  Implementation file for tool : RichTrackCreatorFromTrStoredTracks
 *
 *  CVS Log :-
 *  $Id: RichTrackCreatorFromTrStoredTracks.cpp,v 1.26 2005-04-08 13:08:15 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-------------------------------------------------------------------------------------

// local
#include "RichTrackCreatorFromTrStoredTracks.h"

//-------------------------------------------------------------------------------------

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
    m_bookKeep             ( false ),
    m_Nevts                ( 0     ),
    m_hasBeenCalled        ( false )
{

  // declare interface for this tool
  declareInterface<IRichTrackCreator>(this);

  // job options
  declareProperty( "DoBookKeeping", m_bookKeep );
  // data locations
  declareProperty( "TrStoredTracksLocation",   m_trTracksLocation        );
  declareProperty( "RichRecTrackLocation",     m_richRecTrackLocation    );
  // tool options
  declareProperty( "BuildMassHypothesisRings", m_buildHypoRings          );
  declareProperty( "TrackSegmentTool",         m_trSegToolNickName       );

  // track selection
  declareProperty( "TrackMomentumCuts", m_trSelector.setMomentumCuts()   );
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
  info() << "Selecting '" << m_trSelector.selectedTracksAsString() << "' tracks" << endreq;

  // Configure the ray-tracing mode
  m_traceMode.setDetPrecision      ( RichTraceMode::circle );
  m_traceMode.setDetPlaneBound     ( RichTraceMode::loose  );
  m_traceMode.setForcedSide        ( false                 );
  m_traceMode.setOutMirrorBoundary ( false                 );
  m_traceMode.setMirrorSegBoundary ( false                 );

  // Setup incident services
  incSvc()->addListener( this, IncidentType::BeginEvent );
  incSvc()->addListener( this, IncidentType::EndEvent );

  return sc;
}

StatusCode RichTrackCreatorFromTrStoredTracks::finalize()
{

  // Statistical tools
  RichPoissonEffFunctor eff("%6.2f +-%5.2f");
  RichStatDivFunctor occ("%8.2f +-%5.2f");

  // Print out final track stats
  info() << "===============================================================================" << endreq
         << "                    Track Selection Summary : " << m_Nevts << " events" << endreq
         << "-------------------------------------------------------------------------------" << endreq;
  for ( TrackTypeCount::iterator i = m_nTracksAll.begin();
        i != m_nTracksAll.end(); ++i )
  {
    std::string name =
      ( (*i).first.second ? "Unique " : "NonUnique " ) + Rich::text( (*i).first.first );
    name.resize(17,' ');
    info() << "  " << name << " :" << occ((*i).second.selectedTracks,m_Nevts)
           << " tracks/event : RICH eff " << eff((*i).second.selectedTracks,(*i).second.triedTracks)
           << " % " << endreq;
    if ( (*i).second.aeroSegs>0 )
      info() << "                    :"
             << occ((*i).second.aeroSegs,m_Nevts)  << " Aerogel  segments/event" << endreq;
    if ( (*i).second.c4f10Segs>0 )
      info() << "                    :"
             << occ((*i).second.c4f10Segs,m_Nevts) << " C4F10    segments/event" << endreq;
    if ( (*i).second.cf4Segs>0 )
      info() << "                    :"
             << occ((*i).second.cf4Segs,m_Nevts)   << " CF4      segments/event" << endreq;
  }
  info() << "===============================================================================" << endreq;

  // Execute base class method
  return RichRecToolBase::finalize();
}

// Method that handles various Gaudi "software events"
void RichTrackCreatorFromTrStoredTracks::handle ( const Incident & incident )
{
  // Update prior to start of event. Used to re-initialise data containers
  if      ( IncidentType::BeginEvent == incident.type() )
  {
    InitEvent();
  }
  // End of event
  else if ( IncidentType::EndEvent == incident.type() )
  {
    FinishEvent();
    if ( msgLevel(MSG::DEBUG) )
    {
      // Print out of track count for this event
      unsigned int nTotTried(0), nTotSel(0);
      for ( TrackTypeCount::iterator i = m_nTracksEv.begin(); i != m_nTracksEv.end(); ++i )
      {
        nTotTried += (*i).second.triedTracks;
        nTotSel   += (*i).second.selectedTracks;
      }
      debug() << "Selected " << nTotSel << "/" << nTotTried << " TrStoredTracks :";
      for ( TrackTypeCount::iterator i = m_nTracksEv.begin(); i != m_nTracksEv.end(); ++i )
      {
        const std::string name =
          ( (*i).first.second ? "Unique:" : "NonUnique:" ) + Rich::text( (*i).first.first );
        debug() << " " << name << "=(" << (*i).second.selectedTracks << "/" << (*i).second.triedTracks << ")";
      }
      debug() << endreq;
    }
  }
}

const StatusCode RichTrackCreatorFromTrStoredTracks::newTracks() const
{

  if ( ! m_allDone ) {
    m_allDone = true;

    // Iterate over all reco tracks, and create new RichRecTracks
    richTracks()->reserve( nInputTracks() );
    for ( TrStoredTracks::const_iterator track = trStoredTracks()->begin();
          track != trStoredTracks()->end(); ++track) { newTrack( *track ); }

  }

  return StatusCode::SUCCESS;
}

const long RichTrackCreatorFromTrStoredTracks::nInputTracks() const
{
  const TrStoredTracks * tracks = trStoredTracks();
  return ( tracks ? tracks->size() : 0 );
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

  // flag the tool as having been used this event
  m_hasBeenCalled = true;

  // Is this a TrStoredTrack ?
  const TrStoredTrack * trTrack = dynamic_cast<const TrStoredTrack*>(obj);
  if ( !trTrack ) {
    Warning( "Input data object is not of type TrStoredTrack" );
    return NULL;
  }

  // track type
  const Rich::Track::Type trType = Rich::Track::type(trTrack);

  if ( msgLevel(MSG::VERBOSE) ) {
    verbose() << "Trying TrStoredTrack " << trTrack->key()
              << " type " << trType << ", unique=" << trTrack->unique()
              << ", charge=" << trTrack->charge()
              << endreq;
  }

  // Is track a usable type
  if ( !Rich::Track::isUsable(trType) ) return NULL;

  // Track selection
  if ( !m_trSelector.trackSelected(trTrack) ) return NULL;

  // Get reference to track stats object
  const std::pair<Rich::Track::Type,bool> tkFlag(trType,trTrack->unique());
  TrackCount & tkCount = m_nTracksAll[tkFlag];

  // See if this RichRecTrack already exists
  if ( m_bookKeep && m_trackDone[trTrack->key()] )
  {
    return richTracks()->object(trTrack->key());
  }
  else {

    // count tried tracks
    ++tkCount.triedTracks;
    if (msgLevel(MSG::DEBUG)) ++(m_nTracksEv[tkFlag]).triedTracks;

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
                                                      m_traceMode ) ) {

            // Get PD panel hit point in local coordinates
            // need to for fixed-value geom-eff tool
            // Need to make this "on demand"
            newSegment->pdPanelHitPointLocal() = m_smartIDTool->globalToPDPanel(hitPoint);

            if ( m_signal->hasRichInfo(newSegment) ) {

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

              // Set the average photon energy (for default hypothesis)
              newSegment->trackSegment()
                .setAvPhotonEnergy( m_signal->avgSignalPhotEnergy(newSegment,
                                                                  newTrack->currentHypothesis()) );

              // make RichRecRings for the mass hypotheses if requested
              if ( m_buildHypoRings ) m_massHypoRings->newMassHypoRings( newSegment );

              // Count radiator segments ( order by abundance )
              if      ( Rich::C4F10   == (*iSeg).radiator() ) { ++tkCount.c4f10Segs; }
              else if ( Rich::CF4     == (*iSeg).radiator() ) { ++tkCount.cf4Segs;   }
              else if ( Rich::Aerogel == (*iSeg).radiator() ) { ++tkCount.aeroSegs;  }

            } else {
              if ( msgLevel(MSG::VERBOSE) )
                verbose() << " TrackSegment in " << (*iSeg).radiator() << " rejected" << endreq;
              delete newSegment;
              newSegment = NULL;
            }

          } else {
            if ( msgLevel(MSG::VERBOSE) )
              verbose() << " TrackSegment in " << (*iSeg).radiator() << " rejected" << endreq;
            delete newSegment;
            newSegment = NULL;
          }

        } // end loop over RichTrackSegments

        if ( keepTrack ) {

          // give to container
          richTracks()->insert( newTrack, trTrack->key() );

          // Set vertex momentum
          newTrack->setVertexMomentum( trackPState->p() );

          // track charge
          newTrack->setCharge( trTrack->charge() );

          // Set parent information
          newTrack->setParentTrack( trTrack );

          // Count selected tracks
          ++tkCount.selectedTracks;
          if (msgLevel(MSG::DEBUG)) ++(m_nTracksEv[tkFlag]).selectedTracks;

        } else {
          delete newTrack;
          newTrack = NULL;
        }

      } // end track state and momentum if

    } // end segments if

    // Add to reference map
    if ( m_bookKeep ) m_trackDone[trTrack->key()] = true;

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

      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << "Found " << tdsTracks->size() << " pre-existing RichRecTracks in TES at "
                << m_richRecTrackLocation << endreq;
      }

      // Set smartref to TES track container
      m_tracks = tdsTracks;

      if ( m_bookKeep ) {
        // Remake local track reference map
        for ( RichRecTracks::const_iterator iTrack = tdsTracks->begin();
              iTrack != tdsTracks->end();
              ++iTrack ) {
          m_trackDone[(*iTrack)->key()] = true;
        }
      }

    }

  }

  return m_tracks;
}
