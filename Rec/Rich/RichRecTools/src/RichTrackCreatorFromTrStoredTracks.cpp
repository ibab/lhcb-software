
/** @file RichTrackCreatorFromTrStoredTracks.cpp
 *
 *  Implementation file for tool : RichTrackCreatorFromTrStoredTracks
 *
 *  CVS Log :-
 *  $Id: RichTrackCreatorFromTrStoredTracks.cpp,v 1.21 2004-11-11 16:51:31 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.20  2004/10/13 09:52:41  jonrob
 *  Speed improvements + various minor changes
 *
 *  Revision 1.19  2004/07/27 20:15:33  jonrob
 *  Add doxygen file documentation and CVS information
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

// local
#include "RichTrackCreatorFromTrStoredTracks.h"

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
    m_buildHypoRings       ( false )
{

  // declare interface for this tool
  declareInterface<IRichTrackCreator>(this);

  // job options

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

  // Configure the ray-tracing mode
  m_traceMode.setDetPrecision      ( RichTraceMode::circle );
  m_traceMode.setDetPlaneBound     ( RichTraceMode::loose  );
  m_traceMode.setForcedSide        ( false                 );
  m_traceMode.setOutMirrorBoundary ( false                 );
  m_traceMode.setMirrorSegBoundary ( false                 );

  // Setup incident services
  incSvc()->addListener( this, IncidentType::BeginEvent );
  if (msgLevel(MSG::DEBUG)) incSvc()->addListener( this, IncidentType::EndEvent );

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
    // Print out of unique track count
    {
      unsigned int nTotTried(0), nTotSel(0);
      for ( TrackTypeCount::iterator i = m_nTracksUnique.begin();
            i != m_nTracksUnique.end(); ++i ) {
        nTotTried += (*i).second.first;
        nTotSel   += (*i).second.second;
      }
      debug() << "Selected " << nTotSel << "/" << nTotTried << " Unique TrStoredTracks :";
      for (  TrackTypeCount::iterator iTk = m_nTracksUnique.begin();
             iTk != m_nTracksUnique.end(); ++iTk ) {
        debug() << " " << (*iTk).first << "=(" << (*iTk).second.second
                << "/" << (*iTk).second.first << ")";
      }
      debug() << endreq;
    }
    // Print out of non-unique track count
    {
      unsigned int nTotTried(0), nTotSel(0);
      for ( TrackTypeCount::iterator i = m_nTracksNonUnique.begin();
            i != m_nTracksNonUnique.end(); ++i ) {
        nTotTried += (*i).second.first;
        nTotSel   += (*i).second.second;
      }
      debug() << "Selected " << nTotSel << "/" << nTotTried << " Non-unique TrStoredTracks :";
      for (  TrackTypeCount::iterator iTk = m_nTracksNonUnique.begin();
             iTk != m_nTracksNonUnique.end(); ++iTk ) {
        debug() << " " << (*iTk).first << "=(" << (*iTk).second.second
                << "/" << (*iTk).second.first << ")";
      }
      debug() << endreq;
    }
  }
}

const StatusCode RichTrackCreatorFromTrStoredTracks::newTracks() const {

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

  // count tried tracks
  if ( msgLevel(MSG::DEBUG) ) {
    if ( trTrack->unique() ) {
      ++(m_nTracksUnique[trType].first);
    } else {
      ++(m_nTracksNonUnique[trType].first);
    }
  }

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
                                                      m_traceMode )
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

        } // end loop over RichTrackSegments

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
          if ( msgLevel(MSG::DEBUG) ) {
            if ( trTrack->unique() ) {
              ++(m_nTracksUnique[trType].second);
            } else {
              ++(m_nTracksNonUnique[trType].second);
            }
          }

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
