// $Id: RichRecTrackTool.cpp,v 1.6 2003-04-01 14:33:22 jonrob Exp $

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"

// local
#include "RichRecTrackTool.h"

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

}

StatusCode RichRecTrackTool::initialize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Initialize" << endreq;

  StatusCode sc = StatusCode::SUCCESS;

  // Get pointer to EDS
  if ( !serviceLocator()->service( "EventDataSvc", m_evtDataSvc, true ) ) {
    msg << MSG::ERROR << "EventDataSvc not found" << endreq;
    sc = StatusCode::FAILURE;
  }

  // Get pointer to RichRecSegment Tool
  if ( !toolSvc()->retrieveTool( "RichRecSegmentTool", m_richRecSegmentTool) ) {
    msg << MSG::ERROR << "RichRecSegmentTool not found" << endreq;
    sc = StatusCode::FAILURE;
  }

  // Get pointer to RichDetInterface
  if ( !toolSvc()->retrieveTool( "RichDetInterface", m_richDetInterface ) ) {
    msg << MSG::ERROR << "RichDetInterface not found" << endreq;
    sc = StatusCode::FAILURE;
  }

  // Setup incident services
  IIncidentSvc * incSvc;
  if ( !serviceLocator()->service( "IncidentSvc", incSvc, true ) ) {
    msg << MSG::ERROR << "IncidentSvc not found" << endreq;
    sc = StatusCode::FAILURE;
  } else {
    incSvc->addListener( this, "BeginEvent" ); // Informed of a new event
    //incSvc->addListener( this, "EndEvent"   ); // Informed at the end of event
  }

  return sc;
}

StatusCode RichRecTrackTool::finalize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Finalize" << endreq;

  // Release the tools
  if ( m_richRecSegmentTool ) toolSvc()->releaseTool( m_richRecSegmentTool );
  if ( m_richDetInterface )   toolSvc()->releaseTool( m_richDetInterface );

  return StatusCode::SUCCESS;
}

StatusCode RichRecTrackTool::finalize() {

  // Release the tools
  if( m_richRecSegmentTool ) toolSvc()->releaseTool( m_richRecSegmentTool );
  if( m_richDetInterface )   toolSvc()->releaseTool( m_richDetInterface );
  
  return StatusCode::SUCCESS;
}


// Method that handles various Gaudi "software events"
void RichRecTrackTool::handle ( const Incident& incident ) {

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
        m_trackDone[(int)(*iTrack)->key()] = true;
      }

    }

  } // end begin event if

}

StatusCode RichRecTrackTool::newTracks() {

  if ( ! m_allDone ) {

    // Obtain smart data pointer to TrStoredTracks
    SmartDataPtr<TrStoredTracks> tracks( m_evtDataSvc, m_trTracksLocation );
    if ( !tracks ) {
      MsgStream msg( msgSvc(), name() );
      msg << MSG::ERROR << "Failed to locate tracks at "
          << m_trTracksLocation << endreq;
      return StatusCode::FAILURE;
    }

    // Iterate over all reco tracks, and create new RichRecTracks
    for ( TrStoredTracks::const_iterator track = tracks->begin();
          track != tracks->end();
          ++track) { newTrack( *track ); } // Make new RichRecTrack

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

    RichRecTrack * newTrack = NULL;

    // Form the RichRecSegments for this track
    std::vector<RichTrackSegment> segments;
    int Nsegs = m_richDetInterface->constructSegments( *trTrack, segments );
    if ( 0 < Nsegs ) {

      // Find TrStoredTrack current state and momentum
      SmartRef<TrState> trackState = trTrack->closestState(-999999.);
      TrStateP * trackPState = dynamic_cast<TrStateP*>( (TrState*)trackState );
      if ( trackPState ) {

        // Form a new RichRecTrack
        newTrack = new RichRecTrack();

        // Track type
        if ( trTrack->forward() ) {
          newTrack->setTrackType( Rich::Track::Forward );
        } else if ( trTrack->match() ) {
          newTrack->setTrackType( Rich::Track::Match );
        } else if ( trTrack->upstream() ) {
          newTrack->setTrackType( Rich::Track::UpStream );
        } else if ( trTrack->seed() ) {
          newTrack->setTrackType( Rich::Track::Seed );
        } else if ( trTrack->veloTT() ) {
          newTrack->setTrackType( Rich::Track::VeloTT );
        }

        bool keepTrack = false;

        for ( std::vector<RichTrackSegment>::iterator iSeg = segments.begin();
              iSeg != segments.end(); ++iSeg ) {

          // make a new segment
          RichRecSegment * newSegment = new RichRecSegment();
          newSegment->setTrackSegment( *iSeg );

          // Is this segment useful ?
          if ( m_richRecSegmentTool->hasRichInfo(newSegment) ) {

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
            HepVector3D trackDir = (*iSeg).bestMomentum();
            if ( !m_richDetInterface->traceToDetectorWithoutEff( (*iSeg).rich(),
                                                                 (*iSeg).bestPoint(),
                                                                 trackDir,
                                                                 hitPoint ) ) {
              MsgStream msg( msgSvc(), name() );
              msg << MSG::WARNING << "Segment " << newSegment->key()
                  << " has no HPD panel impact point !!" << endreq;
            }

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
          newTrack->setParentType( Rich::RecTrack::TrStoredTrack );

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

double RichRecTrackTool::nSignalPhotons ( RichRecTrack * track,
                                          const Rich::ParticleIDType& id ) {

  double signal = track->nSignalPhotons( id );
  if ( signal < -0.5 ) {
    signal = 0.0;
    for ( SmartRefVector<RichRecSegment>::iterator segment =
            track->richRecSegments().begin();
          segment != track->richRecSegments().end();
          ++segment ) {
      signal += m_richRecSegmentTool->nSignalPhotons( *segment, id );
    }
    track->setNSignalPhotons( id, signal );
  }

  return signal;
}

double RichRecTrackTool::nObservableSignalPhotons ( RichRecTrack * track,
                                                    const Rich::ParticleIDType& id ) {

  double signal = track->nObservableSignalPhotons( id );
  if ( signal < -0.5 ) {
    signal = 0.0;
    for ( SmartRefVector<RichRecSegment>::iterator segment =
            track->richRecSegments().begin();
          segment != track->richRecSegments().end();
          ++segment ) {
      signal += m_richRecSegmentTool->nObservableSignalPhotons( *segment, id );
    }
    track->setNObservableSignalPhotons( id, signal );
  }

  return signal;
}

double RichRecTrackTool::nScatteredPhotons ( RichRecTrack * track,
                                             const Rich::ParticleIDType& id ) {

  double signal = track->nScatteredPhotons( id );
  if ( signal < -0.5 ) {
    signal = 0.0;
    for ( SmartRefVector<RichRecSegment>::iterator segment =
            track->richRecSegments().begin();
          segment != track->richRecSegments().end();
          ++segment ) {
      signal += m_richRecSegmentTool->nScatteredPhotons( *segment, id );
    }
    track->setNScatteredPhotons( id, signal );
  }

  return signal;
}

double RichRecTrackTool::nObservableScatteredPhotons ( RichRecTrack * track,
                                                       const Rich::ParticleIDType& id ) {

  double signal = track->nObservableScatteredPhotons( id );
  if ( signal < -0.5 ) {
    signal = 0.0;
    for ( SmartRefVector<RichRecSegment>::iterator segment =
            track->richRecSegments().begin();
          segment != track->richRecSegments().end();
          ++segment ) {
      signal += m_richRecSegmentTool->nObservableScatteredPhotons( *segment, id );
    }
    track->setNObservableScatteredPhotons( id, signal );
  }

  return signal;
}

double RichRecTrackTool::nTotalObservablePhotons ( RichRecTrack * track,
                                                   const Rich::ParticleIDType& id ) {
  return nObservableSignalPhotons( track, id ) + nObservableScatteredPhotons( track, id );
}

double RichRecTrackTool::nEmittedPhotons ( RichRecTrack * track,
                                           const Rich::ParticleIDType& id ) {

  double signal = track->nEmittedPhotons( id );
  if ( signal < -0.5 ) {
    signal = 0.0;
    for ( SmartRefVector<RichRecSegment>::iterator segment =
            track->richRecSegments().begin();
          segment != track->richRecSegments().end();
          ++segment ) {
      signal += m_richRecSegmentTool->nEmittedPhotons( *segment, id );
    }
    track->setNEmittedPhotons( id, signal );
  }

  return signal;
}

double RichRecTrackTool::nDetectablePhotons ( RichRecTrack * track,
                                              const Rich::ParticleIDType& id ) {

  double signal = track->nDetectablePhotons( id );
  if ( signal < -0.5 ) {
    signal = 0.0;
    for ( SmartRefVector<RichRecSegment>::iterator segment =
            track->richRecSegments().begin();
          segment != track->richRecSegments().end();
          ++segment ) {
      signal += m_richRecSegmentTool->nDetectablePhotons( *segment, id );
    }
    track->setNDetectablePhotons( id, signal );
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
      if ( m_richRecSegmentTool->nEmittedPhotons(*segment,id) > 0 ) return true;
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
                                       const Rich::ParticleIDType & type ) {
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
                                       const Rich::ParticleIDType & type,
                                       const Rich::RadiatorType & radiator ) {

  // loop over segments
  for ( SmartRefVector<RichRecSegment>::iterator segment =
          track->richRecSegments().begin();
        segment != track->richRecSegments().end();
        ++segment ) {
    if ( radiator == (*segment)->trackSegment().radiator() ) {
      if ( m_richRecSegmentTool->aboveThreshold( *segment, type ) ) return true;
    }
  }

  return false;
}
