// $Id: RichTrackProperties.cpp,v 1.1 2003-06-30 15:47:06 jonrob Exp $

// local
#include "RichTrackProperties.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichTrackProperties
//
// 15/03/2002 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichTrackProperties>          s_factory ;
const        IToolFactory& RichTrackPropertiesFactory = s_factory ;

// Standard constructor
RichTrackProperties::RichTrackProperties( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
  : RichRecToolBase( type, name, parent ) {
  
  declareInterface<IRichTrackProperties>(this);
  
}

StatusCode RichTrackProperties::initialize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Initialize" << endreq;

  // Sets up various tools and services
  if ( !RichRecToolBase::initialize() ) return StatusCode::FAILURE;

  // Acquire instances of tools
  acquireTool("RichSegmentProperties", m_segProps);

  return StatusCode::SUCCESS;
}

StatusCode RichTrackProperties::finalize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Finalize" << endreq;

  // Execute base class method
  return RichRecToolBase::finalize();
}

double RichTrackProperties::nSignalPhotons ( RichRecTrack * track,
                                             const Rich::ParticleIDType id ) {

  double signal = track->nSignalPhotons( id );
  if ( signal < -0.5 ) {
    signal = 0.0;
    for ( SmartRefVector<RichRecSegment>::iterator segment =
            track->richRecSegments().begin();
          segment != track->richRecSegments().end();
          ++segment ) {
      signal += m_segProps->nSignalPhotons( *segment, id );
    }
    track->setNSignalPhotons( id, signal );
  }

  return signal;
}

double RichTrackProperties::nObservableSignalPhotons ( RichRecTrack * track,
                                                       const Rich::ParticleIDType id ) {

  double signal = track->nObservableSignalPhotons( id );
  if ( signal < -0.5 ) {
    signal = 0.0;
    for ( SmartRefVector<RichRecSegment>::iterator segment =
            track->richRecSegments().begin();
          segment != track->richRecSegments().end();
          ++segment ) {
      signal += m_segProps->nObservableSignalPhotons( *segment, id );
    }
    track->setNObservableSignalPhotons( id, signal );
  }

  return signal;
}

double RichTrackProperties::nScatteredPhotons ( RichRecTrack * track,
                                                const Rich::ParticleIDType id ) {

  double signal = track->nScatteredPhotons( id );
  if ( signal < -0.5 ) {
    signal = 0.0;
    for ( SmartRefVector<RichRecSegment>::iterator segment =
            track->richRecSegments().begin();
          segment != track->richRecSegments().end();
          ++segment ) {
      signal += m_segProps->nScatteredPhotons( *segment, id );
    }
    track->setNScatteredPhotons( id, signal );
  }

  return signal;
}

double RichTrackProperties::nObservableScatteredPhotons ( RichRecTrack * track,
                                                          const Rich::ParticleIDType id ) {

  double signal = track->nObservableScatteredPhotons( id );
  if ( signal < -0.5 ) {
    signal = 0.0;
    for ( SmartRefVector<RichRecSegment>::iterator segment =
            track->richRecSegments().begin();
          segment != track->richRecSegments().end();
          ++segment ) {
      signal += m_segProps->nObservableScatteredPhotons( *segment, id );
    }
    track->setNObservableScatteredPhotons( id, signal );
  }

  return signal;
}

double RichTrackProperties::nTotalObservablePhotons ( RichRecTrack * track,
                                                      const Rich::ParticleIDType id ) {
  return nObservableSignalPhotons( track, id ) + nObservableScatteredPhotons( track, id );
}

double RichTrackProperties::nEmittedPhotons ( RichRecTrack * track,
                                              const Rich::ParticleIDType id ) {

  double signal = track->nEmittedPhotons( id );
  if ( signal < -0.5 ) {
    signal = 0.0;
    for ( SmartRefVector<RichRecSegment>::iterator segment =
            track->richRecSegments().begin();
          segment != track->richRecSegments().end();
          ++segment ) {
      signal += m_segProps->nEmittedPhotons( *segment, id );
    }
    track->setNEmittedPhotons( id, signal );
  }

  return signal;
}

double RichTrackProperties::nDetectablePhotons ( RichRecTrack * track,
                                                 const Rich::ParticleIDType id ) {

  double signal = track->nDetectablePhotons( id );
  if ( signal < -0.5 ) {
    signal = 0.0;
    for ( SmartRefVector<RichRecSegment>::iterator segment =
            track->richRecSegments().begin();
          segment != track->richRecSegments().end();
          ++segment ) {
      signal += m_segProps->nDetectablePhotons( *segment, id );
    }
    track->setNDetectablePhotons( id, signal );
  }

  return signal;
}

bool RichTrackProperties::activeInRadiator( RichRecTrack * track,
                                            const Rich::RadiatorType rad,
                                            const Rich::ParticleIDType id ) {

  for ( SmartRefVector<RichRecSegment>::iterator segment =
          track->richRecSegments().begin();
        segment != track->richRecSegments().end();
        ++segment ) {
    if ( rad == (*segment)->trackSegment().radiator() ) {
      if ( m_segProps->nEmittedPhotons(*segment,id) > 0 ) return true;
    }
  }

  return false;
}

bool RichTrackProperties::hasRichInfo( RichRecTrack * track ) {

  for ( SmartRefVector<RichRecSegment>::iterator segment =
          track->richRecSegments().begin();
        segment != track->richRecSegments().end();
        ++segment ) {
    if ( m_segProps->hasRichInfo(*segment) ) return true;
  }

  return false;
}

bool RichTrackProperties::aboveThreshold( RichRecTrack * track,
                                          const Rich::ParticleIDType type ) {
  // loop over segments
  for ( SmartRefVector<RichRecSegment>::iterator segment =
          track->richRecSegments().begin();
        segment != track->richRecSegments().end();
        ++segment ) {
    if ( m_segProps->aboveThreshold( *segment, type ) ) return true;
  }

  return false;
}

bool RichTrackProperties::aboveThreshold( RichRecTrack * track,
                                          const Rich::ParticleIDType type,
                                          const Rich::RadiatorType radiator ) {

  // loop over segments
  for ( SmartRefVector<RichRecSegment>::iterator segment =
          track->richRecSegments().begin();
        segment != track->richRecSegments().end();
        ++segment ) {
    if ( radiator == (*segment)->trackSegment().radiator() ) {
      if ( m_segProps->aboveThreshold( *segment, type ) ) return true;
    }
  }

  return false;
}

// Set the threshold information in a RichPID object for given track
void RichTrackProperties::setThresholdInfo( RichRecTrack * track, RichPID * pid ) {

  if ( aboveThreshold(track,Rich::Electron) ) {
    if ( nTotalObservablePhotons(track,Rich::Electron)>0 ) pid->setElectronHypoAboveThres(1);
  } else { return; }
  if ( aboveThreshold(track,Rich::Muon) ) {
    if ( nTotalObservablePhotons(track,Rich::Muon)>0 ) pid->setMuonHypoAboveThres(1);
  } else { return; }
  if ( aboveThreshold(track,Rich::Pion) ) {
    if ( nTotalObservablePhotons(track,Rich::Pion)>0 ) pid->setPionHypoAboveThres(1);
  } else { return; }
  if ( aboveThreshold(track,Rich::Kaon) ) {
    if ( nTotalObservablePhotons(track,Rich::Kaon)>0 ) pid->setKaonHypoAboveThres(1);
  } else { return; }
  if ( aboveThreshold(track,Rich::Proton) ) {
    if ( nTotalObservablePhotons(track,Rich::Proton)>0 ) pid->setProtonHypoAboveThres(1);
  } else { return; }

}
