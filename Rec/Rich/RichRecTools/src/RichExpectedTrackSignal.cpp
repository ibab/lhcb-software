// $Id: RichExpectedTrackSignal.cpp,v 1.8 2004-07-12 14:32:06 jonrob Exp $

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
    m_rayScat      ( 0 ) 
{
  declareInterface<IRichExpectedTrackSignal>(this);
}

StatusCode RichExpectedTrackSignal::initialize()
{

  // Sets up various tools and services
  StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichGeomEff",            m_geomEff      );
  acquireTool( "RichSellmeirFunc",       m_sellmeir     );
  acquireTool( "RichSignalDetectionEff", m_sigDetEff    );
  acquireTool( "RichParticleProperties", m_richPartProp );
  acquireTool( "RichRayleighScatter",    m_rayScat      );
  acquireTool( "RichGasQuartzWindow",    m_gasQuartzWin );

  return StatusCode::SUCCESS;
}

StatusCode RichExpectedTrackSignal::finalize()
{
  // Execute base class method
  return RichRecToolBase::finalize();
}

double RichExpectedTrackSignal::nEmittedPhotons ( RichRecSegment * segment,
                                                  const Rich::ParticleIDType id ) const {

  if ( !segment->nEmittedPhotons().dataIsValid(id) ) {

    // loop over energy bins
    double signal = 0;
    RichPhotonSpectra<RichRecSegment::FloatType> & spectra = segment->emittedPhotonSpectra();
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
                                                     const Rich::ParticleIDType id ) const {

  if ( !segment->nDetectablePhotons().dataIsValid(id) ) {

    // Make sure emitted Photons are calculated
    nEmittedPhotons( segment, id );

    // loop over energy bins
    double signal = 0;
    RichPhotonSpectra<RichRecSegment::FloatType> & emitSpectra = segment->emittedPhotonSpectra();
    RichPhotonSpectra<RichRecSegment::FloatType> & detSpectra  = segment->detectablePhotonSpectra();
    for ( unsigned int iEnBin = 0; iEnBin < emitSpectra.energyBins(); ++iEnBin ) {
      const double sig = (emitSpectra.energyDist(id))[iEnBin] *
        m_sigDetEff->photonDetEfficiency( segment, emitSpectra.binEnergy(iEnBin) );
      const double gasQuartzWinTrans =
        m_gasQuartzWin->photonTransProb(segment,emitSpectra.binEnergy(iEnBin));
      signal += ( (detSpectra.energyDist(id))[iEnBin] = sig*gasQuartzWinTrans );
    }

    segment->setNDetectablePhotons( id, signal );
  }

  return segment->nDetectablePhotons( id );
}

double
RichExpectedTrackSignal::nSignalPhotons ( RichRecSegment * segment,
                                          const Rich::ParticleIDType id ) const {

  if ( !segment->nSignalPhotons().dataIsValid( id ) ) {
    double signal  = 0;
    double scatter = 0;

    // compute detectable emitted photons
    double detectablePhots = nDetectablePhotons( segment, id );
    if ( detectablePhots > 0 ) {

      // which radiator
      const Rich::RadiatorType rad = segment->trackSegment().radiator();

      // loop over energy bins
      RichPhotonSpectra<RichRecSegment::FloatType> & sigSpectra = segment->signalPhotonSpectra();
      const RichPhotonSpectra<RichRecSegment::FloatType> & detSpectra = segment->detectablePhotonSpectra();
      for ( unsigned int iEnBin = 0; iEnBin < detSpectra.energyBins(); ++iEnBin ) {

        const double scattProb =
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
RichExpectedTrackSignal::avgSignalPhotEnergy( RichRecSegment * segment,
                                              const Rich::ParticleIDType id ) const
{

  // NB : If used often this method should cache information in segment

  // make sure signal photons are calculated
  double nSig = nSignalPhotons ( segment, id );

  double avgEnergy = 0;
  if ( nSig> 0 ) {

    // loop over energy bins
    const RichPhotonSpectra<RichRecSegment::FloatType> & spectra = segment->signalPhotonSpectra();
    double totalEnergy = 0;
    for ( unsigned int iEnBin = 0; iEnBin < spectra.energyBins(); ++iEnBin ) {
      avgEnergy   += (spectra.energyDist(id))[iEnBin] * spectra.binEnergy(iEnBin);
      totalEnergy += (spectra.energyDist(id))[iEnBin];
    } // energy bin loop
    avgEnergy = ( totalEnergy>0 ? avgEnergy/totalEnergy : 0 );

  }

  return avgEnergy;
}

double
RichExpectedTrackSignal::avgEmitPhotEnergy( RichRecSegment * segment,
                                            const Rich::ParticleIDType id ) const
{
  // NB : If used often this method should cache information in segment

  // make sure signal photons are calculated
  const double nSig = nEmittedPhotons ( segment, id );

  double avgEnergy = 0;
  if ( nSig> 0 ) {

    // loop over energy bins
    RichPhotonSpectra<RichRecSegment::FloatType> & spectra = segment->emittedPhotonSpectra();
    double totalEnergy = 0;
    for ( unsigned int iEnBin = 0; iEnBin < spectra.energyBins(); ++iEnBin ) {
      avgEnergy   += (spectra.energyDist(id))[iEnBin] * spectra.binEnergy(iEnBin);
      totalEnergy += (spectra.energyDist(id))[iEnBin];
    } // energy bin loop
    avgEnergy = ( totalEnergy>0 ? avgEnergy/totalEnergy : 0 );

  }

  return avgEnergy;
}

double
RichExpectedTrackSignal::nScatteredPhotons ( RichRecSegment * segment,
                                             const Rich::ParticleIDType id ) const
{
  if ( !segment->nScatteredPhotons().dataIsValid(id) ) {
    // Scattered componented is calculated with the signal
    nSignalPhotons( segment, id );
  }

  return segment->nScatteredPhotons( id );
}

bool RichExpectedTrackSignal::hasRichInfo( RichRecSegment * segment ) const
{
  for ( int iHypo = 0; iHypo < Rich::NParticleTypes; ++iHypo ) {
    if ( m_geomEff->geomEfficiency(segment,(Rich::ParticleIDType)iHypo) > 0 ) return true;
  }

  return false;
}

double
RichExpectedTrackSignal::nTotalObservablePhotons ( RichRecSegment * segment,
                                                   const Rich::ParticleIDType id ) const
{
  return ( m_geomEff->geomEfficiency(segment,id) * nSignalPhotons(segment,id) ) +
    ( m_geomEff->geomEfficiencyScat(segment,id) * nScatteredPhotons(segment,id) );
}

double
RichExpectedTrackSignal::nObservableSignalPhotons ( RichRecSegment * segment,
                                                    const Rich::ParticleIDType id ) const
{
  return m_geomEff->geomEfficiency(segment,id) * nSignalPhotons(segment,id);
}

double
RichExpectedTrackSignal::nObservableScatteredPhotons ( RichRecSegment * segment,
                                                       const Rich::ParticleIDType id ) const
{
  return m_geomEff->geomEfficiencyScat(segment,id) * nScatteredPhotons(segment,id);
}

bool
RichExpectedTrackSignal::aboveThreshold( RichRecSegment * segment,
                                         const Rich::ParticleIDType type ) const
{
  // Geometrical track segment
  const RichTrackSegment & tkSeg = segment->trackSegment();

  // max possible momentum for this segment
  //const double minP = tkSeg.bestMomentum().mag();
  const double minP = tkSeg.bestMomentum().mag() + tkSeg.middleErrors().errP();

  // is this momentum above the treshod momentum
  return ( minP > m_richPartProp->thresholdMomentum(type,segment->trackSegment().radiator()) );
}

double
RichExpectedTrackSignal::nSignalPhotons ( RichRecTrack * track,
                                          const Rich::ParticleIDType id ) const
{

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

double
RichExpectedTrackSignal::nObservableSignalPhotons ( RichRecTrack * track,
                                                    const Rich::ParticleIDType id ) const
{
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

double
RichExpectedTrackSignal::nScatteredPhotons ( RichRecTrack * track,
                                             const Rich::ParticleIDType id ) const
{
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

double
RichExpectedTrackSignal::nObservableScatteredPhotons ( RichRecTrack * track,
                                                       const Rich::ParticleIDType id ) const
{
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

double
RichExpectedTrackSignal::nTotalObservablePhotons ( RichRecTrack * track,
                                                   const Rich::ParticleIDType id ) const
{
  return nObservableSignalPhotons( track, id ) + nObservableScatteredPhotons( track, id );
}

double
RichExpectedTrackSignal::nEmittedPhotons ( RichRecTrack * track,
                                           const Rich::ParticleIDType id ) const
{
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

double
RichExpectedTrackSignal::nDetectablePhotons ( RichRecTrack * track,
                                              const Rich::ParticleIDType id ) const
{
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

bool
RichExpectedTrackSignal::activeInRadiator( RichRecTrack * track,
                                           const Rich::RadiatorType rad,
                                           const Rich::ParticleIDType id ) const
{
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

bool RichExpectedTrackSignal::hasRichInfo( RichRecTrack * track ) const
{
  for ( RichRecTrack::Segments::iterator segment =
          track->richRecSegments().begin();
        segment != track->richRecSegments().end();
        ++segment ) {
    if ( hasRichInfo(*segment) ) return true;
  }

  return false;
}

bool
RichExpectedTrackSignal::aboveThreshold( RichRecTrack * track,
                                         const Rich::ParticleIDType type ) const
{
  // loop over segments
  for ( RichRecTrack::Segments::iterator segment =
          track->richRecSegments().begin();
        segment != track->richRecSegments().end();
        ++segment ) {
    if ( aboveThreshold(*segment,type) ) return true;
  }

  return false;
}

bool
RichExpectedTrackSignal::aboveThreshold( RichRecTrack * track,
                                         const Rich::ParticleIDType type,
                                         const Rich::RadiatorType radiator ) const
{
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
void RichExpectedTrackSignal::setThresholdInfo( RichRecTrack * track,
                                                RichPID * pid ) const
{
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

// Set the threshold information in a RichPID object for given segment
void RichExpectedTrackSignal::setThresholdInfo( RichRecSegment * segment,
                                                RichPID * pid ) const
{
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
