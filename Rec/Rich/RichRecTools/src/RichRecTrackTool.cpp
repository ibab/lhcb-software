// $Id: RichRecTrackTool.cpp,v 1.3 2002-12-02 09:42:21 jonrob Exp $

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/GaudiException.h"

// local
#include "RichRecTools/RichRecTrackTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichRecTrackTool
//
// 15/03/2002 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichRecTrackTool>          s_factory ;
const        IToolFactory& RichRecTrackToolFactory = s_factory ;

// Standard constructor
RichRecTrackTool::RichRecTrackTool( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : AlgTool( type, name, parent ) {

  declareInterface<IRichRecTrackTool>(this);

  // Define job option parameters
  declareProperty( "TrStoredTracksLocation",
                   m_trTracksLocation = TrStoredTrackLocation::Default );
  declareProperty( "RichRecTrackLocation",
                   m_richRecTrackLocation = RichRecTrackLocation::Default );
  declareProperty( "ChronoTiming", m_timing = false );

}

StatusCode RichRecTrackTool::initialize() {

  MsgStream log( msgSvc(), name() );
  log << MSG::DEBUG << "Initialize" << endreq;

  StatusCode sc = StatusCode::SUCCESS;

  // Get pointer to EDS
  if ( !serviceLocator()->service( "EventDataSvc", m_evtDataSvc, true ) ) {
    log << MSG::ERROR << "EventDataSvc not found" << endreq;
    sc = StatusCode::FAILURE;
  }

  // Get pointer to RichRecSegment Tool
  if ( !toolSvc()->retrieveTool( "RichRecSegmentTool", m_richRecSegmentTool) ) {
    log << MSG::ERROR << "RichRecSegmentTool not found" << endreq;
    sc = StatusCode::FAILURE;
  }

  // Get pointer to RichDetInterface
  if ( !toolSvc()->retrieveTool( "RichDetInterface", m_richDetInterface ) ) {
    log << MSG::ERROR << "RichDetInterface not found" << endreq;
    sc = StatusCode::FAILURE;
  }

  // Setup incident services
  IIncidentSvc * incSvc;
  if ( !serviceLocator()->service( "IncidentSvc", incSvc, true ) ) {
    log << MSG::ERROR << "IncidentSvc not found" << endreq;
    sc = StatusCode::FAILURE;
  } else {
    incSvc->addListener( this, "BeginEvent" ); // Informed of a new event
    incSvc->addListener( this, "EndEvent"   ); // Informed at the end of event
  }

  // ChronoStat timing
  if ( m_timing ) {
    if ( !serviceLocator()->service( "ChronoStatSvc", m_chrono, true ) ) {
      log << MSG::ERROR << "ChronoStatSvc not found" << endreq;
      m_timing = false;
      sc = StatusCode::FAILURE;
    }
  }

  return sc;
}

// Method that handles various Gaudi "software events"
void RichRecTrackTool::handle ( const Incident& incident ) {

  MsgStream log( msgSvc(), name() );
  log << MSG::VERBOSE << "Informed of incident '" << incident.type()
      << "' generated by '" << incident.source() << "'" << endreq;

  if ( "BeginEvent" == incident.type() ) {

    // Initialise navigation data
    m_trackDone.clear();
    m_allDone = false;

    SmartDataPtr<RichRecTracks> tdsTracks( m_evtDataSvc,
                                           m_richRecTrackLocation );
    if ( !tdsTracks ) {

      // Reinitialise the Photon Container
      m_tracks = new RichRecTracks();

      // Register new RichRecPhoton container to Gaudi data store
      if ( !m_evtDataSvc->registerObject(m_richRecTrackLocation, m_tracks) ) {
        log << MSG::ERROR << "Failed to register RichRecTracks at "
            << m_richRecTrackLocation << endreq;
      }

    } else {

      log << MSG::DEBUG
          << "Located " << tdsTracks->size() << " RichRecTracks at "
          << m_richRecTrackLocation << endreq;

      // Set smartref to TES track container
      m_tracks = tdsTracks;

      // Remake local track reference map
      for ( RichRecTracks::const_iterator iTrack = tdsTracks->begin();
            iTrack != tdsTracks->end();
            ++iTrack ) {
        m_trackDone[(int)(*iTrack)->key()] = true;
      }

    }

  } else if ( "EndEvent" == incident.type() ) {

    log << MSG::DEBUG << "Finalised with " << m_tracks->size()
        << " RichRecTracks at " << m_richRecTrackLocation << endreq;

  }

}

StatusCode RichRecTrackTool::newTracks() {

  if ( ! m_allDone ) {

    MsgStream log( msgSvc(), name() );

    // Obtain smart data pointer to TrStoredTracks
    SmartDataPtr<TrStoredTracks> tracks( m_evtDataSvc, m_trTracksLocation );
    if ( !tracks ) {
      log << MSG::ERROR << "Failed to locate tracks at "
          << m_trTracksLocation << endreq;
      return StatusCode::FAILURE;
    } else {
      log << MSG::DEBUG << "Successfully located " << tracks->size()
          << " tracks at " << m_trTracksLocation << endreq;
    }

    // Iterate over all reco tracks, and create new RichRecTracks
    for ( TrStoredTracks::const_iterator track = tracks->begin();
          track != tracks->end();
          ++track) {

      // Make new RichRecTrack
      newTrack( *track );

    }

    m_allDone = true;
  }

  return StatusCode::SUCCESS;
}

// Forms a new RichRecTrack object from a TrStoredTrack
RichRecTrack *
RichRecTrackTool::newTrack ( TrStoredTrack * trTrack ) {

  int key = (int)trTrack->key();

  // See if this RichRecTrack already exists
  if ( m_trackDone[key] ) {
    return (RichRecTrack*)(m_tracks->object(key));
  } else {

    // if (m_timing) m_chrono->chronoStart("RichRecTrackTool:newTrack");

    MsgStream log( msgSvc(), name() );

    RichRecTrack * newTrack = NULL;

    // Form the RichRecSegments for this track
    std::vector<RichTrackSegment> segments;
    int Nsegs = m_richDetInterface->constructSegments( *trTrack, segments );
    if ( Nsegs <= 0 ) {
      log << MSG::VERBOSE << "TrTrack " << key
          << " has no path segments in any radiator" << endreq;
    } else {

      // Find TrStoredTrack current state and momentum
      SmartRef<TrState> trackState = trTrack->closestState(-999999.);
      TrStateP * trackPState = dynamic_cast<TrStateP*>( (TrState*)trackState );
      if ( trackPState ) {

        // Form a new RichRecTrack
        newTrack = new RichRecTrack();

        bool keepTrack = false;

        for ( std::vector<RichTrackSegment>::const_iterator iSeg =
                segments.begin();
              iSeg != segments.end();
              ++iSeg ) {

          // make a new segment
          RichRecSegment * newSegment = new RichRecSegment();
          // Set TrackSegment
          newSegment->setTrackSegment( *iSeg );

          // Is this segment useful ?
          bool keepSegment = false;
          for ( Rich::ParticleIDType hypo = Rich::ParticleIDTypeFirst;
                hypo <= Rich::ParticleIDTypeLast;
                ++hypo ) {
            if ( hypo == Rich::Electron ||
                 m_richRecSegmentTool->avgCherenkovTheta( newSegment,
                                                          hypo ) > 0.0 ) {
              if ( m_richRecSegmentTool->geomEfficiency ( newSegment,
                                                          hypo ) > 0.0 ) {
                keepSegment = true;
                break;
              }
            }
          }

          if ( keepSegment ) {

            // keep track
            keepTrack = true;

            // Save this segment
            m_richRecSegmentTool->saveSegment( newSegment );

            // Set associated RichRecTrack
            newSegment->setRichRecTrack( newTrack );

            // Add to the SmartRefVector of RichSegments for this RichRecTrack
            newTrack->addToRichRecSegments( newSegment );

            // Get HPD panel impact point
            HepPoint3D & hitPoint = newSegment->hpdPanelHitPoint();
            HepVector3D trackDir = (*iSeg).exitPoint() - (*iSeg).entryPoint();
            if ( !m_richDetInterface->traceToDetectorWithoutEff( (*iSeg).rich(),
                                                                 (*iSeg).middlePoint(),
                                                                 trackDir,
                                                                 hitPoint ) ) {

              log << MSG::WARNING << "Segment " << newSegment->key()
                  << " has no HPD panel impact point !!" << endreq;

            }

            log << MSG::VERBOSE << "New RichRichSegment " << newSegment->key()
                << " in " << newSegment->trackSegment().radiator() << endreq;

          } else {
            log << MSG::VERBOSE << "Segment in "
                << newSegment->trackSegment().radiator()
                << " has no Rich information. rejecting" << endreq;
            delete newSegment;
            newSegment = NULL;
          }

        }

        if ( keepTrack ) {

          // give to container
          m_tracks->insert( newTrack, key );

          log << MSG::VERBOSE << "New RichRichTrack with " << Nsegs
              << " radiator segments for TrTrack " << trTrack->key() << endreq;

          // Set vertex momentum
          newTrack->setVertexMomentum( trackPState->p() );

          // Set parent information
          newTrack->setParentTrack( trTrack );
          newTrack->setParentType( Rich::RecTrack::TrStoredTrack );

        } else {
          delete newTrack;
          newTrack = NULL;
          log << MSG::VERBOSE
              << "Track has no Rich information, rejecting." << endreq;
        }

      }
    }

    // Add to reference map
    m_trackDone[key] = true;

    // if (m_timing) m_chrono->chronoStop("RichRecTrackTool:newTrack");

    return newTrack;
  }

}

double RichRecTrackTool::signalPhotons ( RichRecTrack * track,
                                         const Rich::ParticleIDType& id ) {

  double signal = track->signalPhotons( id );
  if ( signal < -0.5 ) {
    signal = 0.0;
    for ( SmartRefVector<RichRecSegment>::iterator segment =
            track->richRecSegments().begin();
          segment != track->richRecSegments().end();
          ++segment ) {
      signal += m_richRecSegmentTool->signalPhotons( *segment, id );
    }
    track->setSignalPhotons( id, signal );
  }

  return signal;
}

double RichRecTrackTool::scatteredPhotons ( RichRecTrack * track,
                                            const Rich::ParticleIDType& id ) {

  double signal = track->scatteredPhotons( id );
  if ( signal < -0.5 ) {
    signal = 0.0;
    for ( SmartRefVector<RichRecSegment>::iterator segment =
            track->richRecSegments().begin();
          segment != track->richRecSegments().end();
          ++segment ) {
      signal += m_richRecSegmentTool->scatteredPhotons( *segment, id );
    }
    track->setScatteredPhotons( id, signal );
  }

  return signal;
}

double RichRecTrackTool::allPhotons ( RichRecTrack * track,
                                      const Rich::ParticleIDType& id ) {
  return signalPhotons( track, id ) + scatteredPhotons( track, id );
}

double RichRecTrackTool::emittedPhotons ( RichRecTrack * track,
                                          const Rich::ParticleIDType& id ) {

  double signal = track->emittedPhotons( id );
  if ( signal < -0.5 ) {
    signal = 0.0;
    for ( SmartRefVector<RichRecSegment>::iterator segment =
            track->richRecSegments().begin();
          segment != track->richRecSegments().end();
          ++segment ) {
      signal += m_richRecSegmentTool->emittedPhotons( *segment, id );
    }
    track->setEmittedPhotons( id, signal );
  }

  return signal;
}

bool RichRecTrackTool::activeInRadiator( RichRecTrack * track,
                                         const Rich::RadiatorType rad,
                                         const Rich::ParticleIDType id ) {

  for ( SmartRefVector<RichRecSegment>::iterator segment =
          track->richRecSegments().begin();
        segment != track->richRecSegments().end();
        ++segment ) {
    if ( rad == (*segment)->trackSegment().radiator() ) {
      if ( m_richRecSegmentTool->emittedPhotons(*segment,id) > 0.0 ) return true;
    }
  }

  return false;
}

bool RichRecTrackTool::hasRichInfo( RichRecTrack * track ) {

  for ( SmartRefVector<RichRecSegment>::iterator segment =
          track->richRecSegments().begin();
        segment != track->richRecSegments().end();
        ++segment ) {
    if ( m_richRecSegmentTool->hasRichInfo(*segment) ) return true;
  }

  return false;
}

RichRecTracks * RichRecTrackTool::richTracks() {
  return m_tracks;
}

MCParticle * RichRecTrackTool::parentMCP ( const RichRecTrack * track ) {
  return dynamic_cast<MCParticle*>( (ContainedObject*)track->parentTrack() );
}

TrStoredTrack * RichRecTrackTool::parentTrTrack ( const RichRecTrack * track ) {
  return dynamic_cast<TrStoredTrack*>( (ContainedObject*)track->parentTrack() );
}

bool RichRecTrackTool::aboveThreshold( RichRecTrack * track,
                                       Rich::ParticleIDType & type ) {
  // loop over segments
  for ( SmartRefVector<RichRecSegment>::iterator segment =
          track->richRecSegments().begin();
        segment != track->richRecSegments().end();
        ++segment ) {
    if ( m_richRecSegmentTool->aboveThreshold( *segment, type ) ) return true;
  }

  return false;
}

bool RichRecTrackTool::aboveThreshold( RichRecTrack * track,
                                       Rich::ParticleIDType & type,
                                       Rich::RadiatorType & radiator ) {

  // loop over segments
  for ( SmartRefVector<RichRecSegment>::iterator segment =
          track->richRecSegments().begin();
        segment != track->richRecSegments().end();
        ++segment ) {
    if ( radiator == (*segment)->trackSegment().radiator() )
      if ( m_richRecSegmentTool->aboveThreshold( *segment, type ) ) return true;
  }

  return false;
}
