// $Id: RichExpectedTrackSignal.cpp,v 1.3 2003-08-26 14:40:18 jonrob Exp $

// local
#include "RichExpectedTrackSignal.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichExpectedTrackSignal
//
// 15/03/2002 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichExpectedTrackSignal>          s_factory ;
const        IToolFactory& RichExpectedTrackSignalFactory = s_factory ;

// Standard constructor
RichExpectedTrackSignal::RichExpectedTrackSignal ( const std::string& type,
                                                   const std::string& name,
                                                   const IInterface* parent )
  : RichRecToolBase( type, name, parent ),
    m_geomEff      ( 0 ),
    m_sellmeir     ( 0 ),
    m_sigDetEff    ( 0 ),
    m_richPartProp ( 0 ),
    m_rayScat      ( 0 ) {

  declareInterface<IRichExpectedTrackSignal>(this);

}

StatusCode RichExpectedTrackSignal::initialize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Initialize" << endreq;

  // Sets up various tools and services
  if ( !RichRecToolBase::initialize() ) return StatusCode::FAILURE;

  // Acquire instances of tools
  acquireTool( "RichGeomEff",            m_geomEff      );
  acquireTool( "RichSellmeirFunc",       m_sellmeir     );
  acquireTool( "RichSignalDetectionEff", m_sigDetEff    );
  acquireTool( "RichParticleProperties", m_richPartProp ); 
  acquireTool( "RichRayleighScatter",    m_rayScat      );

  // Informational Printout
  //msg << MSG::DEBUG

  return StatusCode::SUCCESS;
}

StatusCode RichExpectedTrackSignal::finalize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Finalize" << endreq;

  // release tools
  releaseTool( m_geomEff );
  releaseTool( m_sellmeir );
  releaseTool( m_sigDetEff );
  releaseTool( m_richPartProp );
  releaseTool( m_rayScat );

  // Execute base class method
  return RichRecToolBase::finalize();
}

double RichExpectedTrackSignal::nEmittedPhotons ( RichRecSegment * segment,
                                                  const Rich::ParticleIDType id ) {

  if ( !segment->nEmittedPhotons().dataIsValid(id) ) {

    // loop over energy bins
    double signal = 0;
    RichPhotonSpectra & spectra = segment->emittedPhotonSpectra();
    for ( unsigned int iEnBin = 0; iEnBin < spectra.energyBins(); ++iEnBin ) {

      double phots = 
        m_sellmeir->photonsInEnergyRange( segment,
                                          id,
                                          spectra.binEnergyLowerEdge(iEnBin),
                                          spectra.binEnergyUpperEdge(iEnBin) );
      if ( phots<0 ) phots = 0;
      (spectra.energyDist(id))[iEnBin] = phots;
      signal += phots;
    }

    segment->setNEmittedPhotons( id, signal );
  }

  return segment->nEmittedPhotons( id );
}

double RichExpectedTrackSignal::nDetectablePhotons ( RichRecSegment * segment,
                                                     const Rich::ParticleIDType id ) {

  if ( !segment->nDetectablePhotons().dataIsValid(id) ) {

    // Make sure emitted Photons are calculated
    nEmittedPhotons( segment, id );

    // loop over energy bins
    double signal = 0;
    RichPhotonSpectra & emitSpectra = segment->emittedPhotonSpectra();
    RichPhotonSpectra & detSpectra  = segment->detectablePhotonSpectra();
    for ( unsigned int iEnBin = 0; iEnBin < emitSpectra.energyBins(); ++iEnBin ) {
      double sig = (emitSpectra.energyDist(id))[iEnBin] * 
        m_sigDetEff->photonDetEfficiency( segment, emitSpectra.binEnergy(iEnBin) );
      signal += ( (detSpectra.energyDist(id))[iEnBin] = sig );
    }

    segment->setNDetectablePhotons( id, signal );
  }

  return segment->nDetectablePhotons( id );
}

double
RichExpectedTrackSignal::nSignalPhotons ( RichRecSegment * segment,
                                          const Rich::ParticleIDType id ) {

  if ( !segment->nSignalPhotons().dataIsValid( id ) ) {
    double signal  = 0;
    double scatter = 0;

    // compute detectable emitted photons
    double detectablePhots = nDetectablePhotons( segment, id );
    if ( detectablePhots> 0 ) {

      // which radiator
      Rich::RadiatorType rad = segment->trackSegment().radiator();

      // loop over energy bins
      RichPhotonSpectra & sigSpectra = segment->signalPhotonSpectra();
      RichPhotonSpectra & detSpectra = segment->detectablePhotonSpectra();
      for ( unsigned int iEnBin = 0; iEnBin < detSpectra.energyBins(); ++iEnBin ) {

        double scattProb = 
          ( rad != Rich::Aerogel ? 0 :
            m_rayScat->photonScatteredProb( segment,
                                            detSpectra.binEnergy(iEnBin) ) );

        // observable photons * signal/scatter prob
        (sigSpectra.energyDist(id))[iEnBin] =
          (detSpectra.energyDist(id))[iEnBin] * (1.-scattProb);
        signal  += (sigSpectra.energyDist(id))[iEnBin];
        scatter += (detSpectra.energyDist(id))[iEnBin] * scattProb;

      } // energy bin loop

    }

    segment->setNSignalPhotons( id, signal );
    segment->setNScatteredPhotons( id, scatter );
  }

  return segment->nSignalPhotons( id );
}

double
RichExpectedTrackSignal::nScatteredPhotons ( RichRecSegment * segment,
                                             const Rich::ParticleIDType id ) {

  if ( !segment->nScatteredPhotons().dataIsValid(id) ) {
    // Scattered componented is calculated with the signal
    nSignalPhotons( segment, id );
  }

  return segment->nScatteredPhotons( id );
}

bool RichExpectedTrackSignal::hasRichInfo( RichRecSegment * segment ) {

  for ( int iHypo = 0; iHypo < Rich::NParticleTypes; ++iHypo ) {
    if ( m_geomEff->geomEfficiency(segment,(Rich::ParticleIDType)iHypo) > 0 ) return true;
  }

  return false;
}

double RichExpectedTrackSignal::nTotalObservablePhotons ( RichRecSegment * segment,
                                                          const Rich::ParticleIDType id ) {
  return ( m_geomEff->geomEfficiency(segment,id) * nSignalPhotons(segment,id) ) +
    ( m_geomEff->geomEfficiencyScat(segment,id) * nScatteredPhotons(segment,id) );
}

double RichExpectedTrackSignal::nObservableSignalPhotons ( RichRecSegment * segment,
                                                           const Rich::ParticleIDType id ) {
  return m_geomEff->geomEfficiency(segment,id) * nSignalPhotons(segment,id);
}

double RichExpectedTrackSignal::nObservableScatteredPhotons ( RichRecSegment * segment,
                                                              const Rich::ParticleIDType id ) {
  return m_geomEff->geomEfficiencyScat(segment,id) * nScatteredPhotons(segment,id);
}

bool RichExpectedTrackSignal::aboveThreshold( RichRecSegment * segment,
                                              const Rich::ParticleIDType type ) {
  return ( segment->trackSegment().bestMomentumMag() >
           m_richPartProp->thresholdMomentum(type, segment->trackSegment().radiator()) );
}

// Set the threshold information in a RichPID object for given segment
void RichExpectedTrackSignal::setThresholdInfo( RichRecSegment * segment,
                                                RichPID * pid ) {

  if ( aboveThreshold(segment,Rich::Electron) ) {
    if ( nTotalObservablePhotons(segment,Rich::Electron)>0 ) pid->setElectronHypoAboveThres(1);
  } else { return; }
  if ( aboveThreshold(segment,Rich::Muon) ) {
    if ( nTotalObservablePhotons(segment,Rich::Muon)>0 ) pid->setMuonHypoAboveThres(1);
  } else { return; }
  if ( aboveThreshold(segment,Rich::Pion) ) {
    if ( nTotalObservablePhotons(segment,Rich::Pion)>0 ) pid->setPionHypoAboveThres(1);
  } else { return; }
  if ( aboveThreshold(segment,Rich::Kaon) ) {
    if ( nTotalObservablePhotons(segment,Rich::Kaon)>0 ) pid->setKaonHypoAboveThres(1);
  } else { return; }
  if ( aboveThreshold(segment,Rich::Proton) ) {
    if ( nTotalObservablePhotons(segment,Rich::Proton)>0 ) pid->setProtonHypoAboveThres(1);
  } else { return; }

}

double RichExpectedTrackSignal::nSignalPhotons ( RichRecTrack * track,
                                                 const Rich::ParticleIDType id ) {

  if ( !track->nSignalPhotons().dataIsValid(id) ) {
    double signal = 0;
    for ( RichRecTrack::Segments::iterator segment =
            track->richRecSegments().begin();
          segment != track->richRecSegments().end();
          ++segment ) {
      signal += nSignalPhotons( *segment, id );
    }
    track->setNSignalPhotons( id, signal );
  }

  return track->nSignalPhotons( id );
}

double RichExpectedTrackSignal::nObservableSignalPhotons ( RichRecTrack * track,
                                                           const Rich::ParticleIDType id ) {

  if ( !track->nObservableSignalPhotons().dataIsValid(id) ) {
    double signal = 0;
    for ( RichRecTrack::Segments::iterator segment =
            track->richRecSegments().begin();
          segment != track->richRecSegments().end();
          ++segment ) {
      signal += nObservableSignalPhotons( *segment, id );
    }
    track->setNObservableSignalPhotons( id, signal );
  }

  return track->nObservableSignalPhotons( id );
}

double RichExpectedTrackSignal::nScatteredPhotons ( RichRecTrack * track,
                                                    const Rich::ParticleIDType id ) {

  if ( !track->nScatteredPhotons().dataIsValid(id) ) {
    double signal = 0;
    for ( RichRecTrack::Segments::iterator segment =
            track->richRecSegments().begin();
          segment != track->richRecSegments().end();
          ++segment ) {
      signal += nScatteredPhotons( *segment, id );
    }
    track->setNScatteredPhotons( id, signal );
  }

  return track->nScatteredPhotons( id );
}

double RichExpectedTrackSignal::nObservableScatteredPhotons ( RichRecTrack * track,
                                                              const Rich::ParticleIDType id ) {

  if ( !track->nObservableScatteredPhotons().dataIsValid(id) ) {
    double signal = 0;
    for ( RichRecTrack::Segments::iterator segment =
            track->richRecSegments().begin();
          segment != track->richRecSegments().end();
          ++segment ) {
      signal += nObservableScatteredPhotons( *segment, id );
    }
    track->setNObservableScatteredPhotons( id, signal );
  }

  return track->nObservableScatteredPhotons( id );
}

double RichExpectedTrackSignal::nTotalObservablePhotons ( RichRecTrack * track,
                                                          const Rich::ParticleIDType id ) {
  return nObservableSignalPhotons( track, id ) + nObservableScatteredPhotons( track, id );
}

double RichExpectedTrackSignal::nEmittedPhotons ( RichRecTrack * track,
                                                  const Rich::ParticleIDType id ) {

  if ( !track->nEmittedPhotons().dataIsValid(id) ) {
    double signal = 0.0;
    for ( RichRecTrack::Segments::iterator segment =
            track->richRecSegments().begin();
          segment != track->richRecSegments().end();
          ++segment ) {
      signal += nEmittedPhotons( *segment, id );
    }
    track->setNEmittedPhotons( id, signal );
  }

  return track->nEmittedPhotons( id );
}

double RichExpectedTrackSignal::nDetectablePhotons ( RichRecTrack * track,
                                                     const Rich::ParticleIDType id ) {

  if ( !track->nDetectablePhotons().dataIsValid(id) ) {
    double signal = 0;
    for ( RichRecTrack::Segments::iterator segment =
            track->richRecSegments().begin();
          segment != track->richRecSegments().end();
          ++segment ) {
      signal += nDetectablePhotons( *segment, id );
    }
    track->setNDetectablePhotons( id, signal );
  }

  return track->nDetectablePhotons( id );
}

bool RichExpectedTrackSignal::activeInRadiator( RichRecTrack * track,
                                                const Rich::RadiatorType rad,
                                                const Rich::ParticleIDType id ) {

  for ( RichRecTrack::Segments::iterator segment =
          track->richRecSegments().begin();
        segment != track->richRecSegments().end();
        ++segment ) {
    if ( rad == (*segment)->trackSegment().radiator() ) {
      if ( nEmittedPhotons(*segment,id) > 0 ) return true;
    }
  }

  return false;
}

bool RichExpectedTrackSignal::hasRichInfo( RichRecTrack * track ) {

  for ( RichRecTrack::Segments::iterator segment =
          track->richRecSegments().begin();
        segment != track->richRecSegments().end();
        ++segment ) {
    if ( hasRichInfo(*segment) ) return true;
  }

  return false;
}

bool RichExpectedTrackSignal::aboveThreshold( RichRecTrack * track,
                                              const Rich::ParticleIDType type ) {
  // loop over segments
  for ( RichRecTrack::Segments::iterator segment =
          track->richRecSegments().begin();
        segment != track->richRecSegments().end();
        ++segment ) {
    if ( aboveThreshold( *segment, type ) ) return true;
  }

  return false;
}

bool RichExpectedTrackSignal::aboveThreshold( RichRecTrack * track,
                                              const Rich::ParticleIDType type,
                                              const Rich::RadiatorType radiator ) {

  // loop over segments
  for ( RichRecTrack::Segments::iterator segment =
          track->richRecSegments().begin();
        segment != track->richRecSegments().end();
        ++segment ) {
    if ( radiator == (*segment)->trackSegment().radiator() ) {
      if ( aboveThreshold( *segment, type ) ) return true;
    }
  }

  return false;
}

// Set the threshold information in a RichPID object for given track
void RichExpectedTrackSignal::setThresholdInfo( RichRecTrack * track, RichPID * pid ) {

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
