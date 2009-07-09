
//-----------------------------------------------------------------------------
/** @file RichExpectedTrackSignal.cpp
 *
 *  Implementation file for tool : Rich::Rec::ExpectedTrackSignal
 *
 *  CVS Log :-
 *  $Id: RichExpectedTrackSignal.cpp,v 1.5 2009-07-09 11:21:24 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichExpectedTrackSignal.h"

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( ExpectedTrackSignal );

// Standard constructor
ExpectedTrackSignal::ExpectedTrackSignal ( const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent )
  : RichRecToolBase( type, name, parent ),
    m_geomEff      ( 0 ),
    m_sellmeir     ( 0 ),
    m_sigDetEff    ( 0 ),
    m_richPartProp ( 0 ),
    m_rayScat      ( 0 ),
    m_minPhotonsPerRad ( Rich::NRadiatorTypes, 0 )
{
  // interface
  declareInterface<IExpectedTrackSignal>(this);
  // JOS
  declareProperty( "MinNumPhotonsPerRad", 
                   m_minPhotonsPerRad,
                   "Minimum number of photons in each radiator for a radiator segment to be considered as having RICH information" );
}

StatusCode ExpectedTrackSignal::initialize()
{

  // Sets up various tools and services
  const StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichGeomEff" ,           m_geomEff      );
  acquireTool( "RichSellmeirFunc",       m_sellmeir     );
  acquireTool( "RichSignalDetectionEff", m_sigDetEff    );
  acquireTool( "RichParticleProperties", m_richPartProp );
  acquireTool( "RichRayleighScatter",    m_rayScat      );
  acquireTool( "RichGasQuartzWindow",    m_gasQuartzWin );

  m_pidTypes = m_richPartProp->particleTypes();
  info() << "Particle types considered = " << m_pidTypes << endreq;

  info() << "Minimum number of expected photons (Aero/R1Gas/R2Gas) : " << m_minPhotonsPerRad << endmsg;

  return sc;
}

double ExpectedTrackSignal::nEmittedPhotons ( LHCb::RichRecSegment * segment,
                                              const Rich::ParticleIDType id ) const
{

  if ( !segment->nEmittedPhotons().dataIsValid(id) )
  {

    // loop over energy bins
    double signal = 0;
    Rich::PhotonSpectra<LHCb::RichRecSegment::FloatType> & spectra = segment->emittedPhotonSpectra();
    for ( unsigned int iEnBin = 0; iEnBin < spectra.energyBins(); ++iEnBin )
    {

      double phots =
        m_sellmeir->photonsInEnergyRange( segment,
                                          id,
                                          spectra.binEnergyLowerEdge(iEnBin),
                                          spectra.binEnergyUpperEdge(iEnBin) );
      if ( phots<0 ) phots = 0;
      (spectra.energyDist(id))[iEnBin] = static_cast<LHCb::RichRecSegment::FloatType>(phots);
      signal += phots;

    }

    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "RichRecSegment " << segment->key() << " " << id
              << " nEmittedPhotons = " << signal << endreq;
    }

    segment->setNEmittedPhotons( id, static_cast<LHCb::RichRecSegment::FloatType>(signal) );
  }

  return segment->nEmittedPhotons( id );
}

double ExpectedTrackSignal::nDetectablePhotons (  LHCb::RichRecSegment * segment,
                                                  const Rich::ParticleIDType id ) const
{

  if ( !segment->nDetectablePhotons().dataIsValid(id) )
  {

    // Make sure emitted Photons are calculated
    nEmittedPhotons( segment, id );

    // loop over energy bins
    double signal = 0;
    const Rich::PhotonSpectra<LHCb::RichRecSegment::FloatType> & emitSpectra
      = segment->emittedPhotonSpectra();
    Rich::PhotonSpectra<LHCb::RichRecSegment::FloatType> & detSpectra
      = segment->detectablePhotonSpectra();
    for ( unsigned int iEnBin = 0; iEnBin < emitSpectra.energyBins(); ++iEnBin )
    {
      const double sig = (emitSpectra.energyDist(id))[iEnBin] *
        m_sigDetEff->photonDetEfficiency( segment, id, emitSpectra.binEnergy(iEnBin) );
      const double gasQuartzWinTrans =
        m_gasQuartzWin->photonTransProb(segment,emitSpectra.binEnergy(iEnBin));
      const double T = sig*gasQuartzWinTrans;
      (detSpectra.energyDist(id))[iEnBin] = static_cast<LHCb::RichRecSegment::FloatType>(T);
      signal += T;
    }

    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "RichRecSegment " << segment->key() << " " << id
              << " nDetectablePhotons = " << signal << endreq;
    }

    segment->setNDetectablePhotons( id, static_cast<LHCb::RichRecSegment::FloatType>(signal) );
  }

  return segment->nDetectablePhotons( id );
}

double
ExpectedTrackSignal::nSignalPhotons (  LHCb::RichRecSegment * segment,
                                       const Rich::ParticleIDType id ) const {

  if ( !segment->nSignalPhotons().dataIsValid( id ) )
  {
    double signal(0), scatter(0);

    // compute detectable emitted photons
    double detectablePhots = nDetectablePhotons( segment, id );
    if ( detectablePhots > 0 )
    {

      // which radiator
      const Rich::RadiatorType rad = segment->trackSegment().radiator();

      // loop over energy bins
      Rich::PhotonSpectra<LHCb::RichRecSegment::FloatType> & sigSpectra
        = segment->signalPhotonSpectra();
      const Rich::PhotonSpectra<LHCb::RichRecSegment::FloatType> & detSpectra
        = segment->detectablePhotonSpectra();
      for ( unsigned int iEnBin = 0; iEnBin < detSpectra.energyBins(); ++iEnBin )
      {

        const double scattProb =
          ( rad != Rich::Aerogel ? 0 :
            m_rayScat->photonScatteredProb( segment,
                                            detSpectra.binEnergy(iEnBin) ) );

        // observable photons * signal/scatter prob
        (sigSpectra.energyDist(id))[iEnBin] =
          static_cast<LHCb::RichRecSegment::FloatType>( (detSpectra.energyDist(id))[iEnBin] * (1.-scattProb) );
        signal  += (sigSpectra.energyDist(id))[iEnBin];
        scatter += (detSpectra.energyDist(id))[iEnBin] * scattProb;

      } // energy bin loop

    }

    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "RichRecSegment " << segment->key() << " " << id
              << " nSignalPhotons = " << signal
              << " nScatteredPhotons = " << scatter << endreq;
    }

    segment->setNSignalPhotons( id, static_cast<LHCb::RichRecSegment::FloatType>(signal) );
    segment->setNScatteredPhotons( id, static_cast<LHCb::RichRecSegment::FloatType>(scatter) );
  }

  return segment->nSignalPhotons( id );
}

double
ExpectedTrackSignal::avgSignalPhotEnergy( LHCb::RichRecSegment * segment,
                                          const Rich::ParticleIDType id ) const
{
  // First set a 'rough' guess using the emitted energy
  //segment->trackSegment().setAvPhotonEnergy( avgEmitPhotEnergy(segment) );

  // make sure signal photons are calculated
  const double nSig = nSignalPhotons ( segment, id );

  double avgEnergy = 0;
  if ( nSig> 0 )
  {
    // loop over energy bins
    const Rich::PhotonSpectra<LHCb::RichRecSegment::FloatType> & spectra = segment->signalPhotonSpectra();
    double totalEnergy = 0;
    for ( unsigned int iEnBin = 0; iEnBin < spectra.energyBins(); ++iEnBin ) 
    {
      avgEnergy   += (spectra.energyDist(id))[iEnBin] * spectra.binEnergy(iEnBin);
      totalEnergy += (spectra.energyDist(id))[iEnBin];
    } // energy bin loop
    
    // normalise result
    avgEnergy = ( totalEnergy>0 ? avgEnergy/totalEnergy : 0 );
  }

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "RichRecSegment " << segment->key() << " " << id
            << " avgSignalPhotEnergy = " << avgEnergy << endreq;
  }

  return avgEnergy;
}

double
ExpectedTrackSignal::avgEmitPhotEnergy( LHCb::RichRecSegment * segment,
                                        const Rich::ParticleIDType id ) const
{

  // make sure emitted photons are calculated
  const double nSig = nEmittedPhotons ( segment, id );

  double avgEnergy = 0;
  if ( nSig> 0 )
  {
    // loop over energy bins
    const Rich::PhotonSpectra<LHCb::RichRecSegment::FloatType> & spectra = segment->emittedPhotonSpectra();
    double totalEnergy = 0;
    for ( unsigned int iEnBin = 0; iEnBin < spectra.energyBins(); ++iEnBin ) 
    {
      avgEnergy   += (spectra.energyDist(id))[iEnBin] * spectra.binEnergy(iEnBin);
      totalEnergy += (spectra.energyDist(id))[iEnBin];
    } // energy bin loop

    // normalise result
    avgEnergy = ( totalEnergy>0 ? avgEnergy/totalEnergy : 0 );
  }

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "RichRecSegment " << segment->key() << " " << id
            << " avgEmitPhotEnergy = " << avgEnergy << endreq;
  }

  return avgEnergy;
}

double
ExpectedTrackSignal::avgSignalPhotEnergy( LHCb::RichRecSegment * segment ) const
{
  return ( nSignalPhotons(segment,Rich::Pion) > 0 ?
           avgSignalPhotEnergy ( segment, Rich::Pion         ) :
           avgSignalPhotEnergy ( segment, m_pidTypes.front() ) );
}

double
ExpectedTrackSignal::avgEmitPhotEnergy( LHCb::RichRecSegment * segment ) const
{
  return ( nEmittedPhotons(segment,Rich::Pion) > 0 ?
           avgEmitPhotEnergy ( segment, Rich::Pion         ) :
           avgEmitPhotEnergy ( segment, m_pidTypes.front() ) );
}

double
ExpectedTrackSignal::nScatteredPhotons ( LHCb::RichRecSegment * segment,
                                         const Rich::ParticleIDType id ) const
{
  if ( !segment->nScatteredPhotons().dataIsValid(id) ) 
  {
    // Scattered componented is calculated with the signal
    nSignalPhotons( segment, id );
  }

  return segment->nScatteredPhotons( id );
}

double
ExpectedTrackSignal::nTotalObservablePhotons ( LHCb::RichRecSegment * segment,
                                               const Rich::ParticleIDType id ) const
{
  return ( m_geomEff->geomEfficiency(segment,id) * nSignalPhotons(segment,id) ) +
    ( m_geomEff->geomEfficiencyScat(segment,id) * nScatteredPhotons(segment,id) );
}

double
ExpectedTrackSignal::nObservableSignalPhotons ( LHCb::RichRecSegment * segment,
                                                const Rich::ParticleIDType id ) const
{
  return m_geomEff->geomEfficiency(segment,id) * nSignalPhotons(segment,id);
}

double
ExpectedTrackSignal::nObservableScatteredPhotons ( LHCb::RichRecSegment * segment,
                                                   const Rich::ParticleIDType id ) const
{
  return m_geomEff->geomEfficiencyScat(segment,id) * nScatteredPhotons(segment,id);
}

double
ExpectedTrackSignal::nSignalPhotons ( LHCb::RichRecTrack * track,
                                      const Rich::ParticleIDType id ) const
{

  if ( !track->nSignalPhotons().dataIsValid(id) ) {
    double signal = 0;
    for ( LHCb::RichRecTrack::Segments::iterator segment =
            track->richRecSegments().begin();
          segment != track->richRecSegments().end();
          ++segment ) {
      signal += nSignalPhotons( *segment, id );
    }
    track->setNSignalPhotons( id, static_cast<LHCb::RichRecTrack::FloatType>(signal) );
  }

  return track->nSignalPhotons( id );
}

double
ExpectedTrackSignal::nObservableSignalPhotons ( LHCb::RichRecTrack * track,
                                                const Rich::ParticleIDType id ) const
{
  if ( !track->nObservableSignalPhotons().dataIsValid(id) ) {
    double signal = 0;
    for ( LHCb::RichRecTrack::Segments::iterator segment =
            track->richRecSegments().begin();
          segment != track->richRecSegments().end();
          ++segment ) {
      signal += nObservableSignalPhotons( *segment, id );
    }
    track->setNObservableSignalPhotons( id, static_cast<LHCb::RichRecTrack::FloatType>(signal) );
  }

  return track->nObservableSignalPhotons( id );
}

double
ExpectedTrackSignal::nScatteredPhotons ( LHCb::RichRecTrack * track,
                                         const Rich::ParticleIDType id ) const
{
  if ( !track->nScatteredPhotons().dataIsValid(id) ) {
    double signal = 0;
    for ( LHCb::RichRecTrack::Segments::iterator segment =
            track->richRecSegments().begin();
          segment != track->richRecSegments().end();
          ++segment ) {
      signal += nScatteredPhotons( *segment, id );
    }
    track->setNScatteredPhotons( id, static_cast<LHCb::RichRecTrack::FloatType>(signal) );
  }

  return track->nScatteredPhotons( id );
}

double
ExpectedTrackSignal::nObservableScatteredPhotons ( LHCb::RichRecTrack * track,
                                                   const Rich::ParticleIDType id ) const
{
  if ( !track->nObservableScatteredPhotons().dataIsValid(id) ) {
    double signal = 0;
    for ( LHCb::RichRecTrack::Segments::iterator segment =
            track->richRecSegments().begin();
          segment != track->richRecSegments().end();
          ++segment ) {
      signal += nObservableScatteredPhotons( *segment, id );
    }
    track->setNObservableScatteredPhotons( id, static_cast<LHCb::RichRecTrack::FloatType>(signal) );
  }

  return track->nObservableScatteredPhotons( id );
}

double
ExpectedTrackSignal::nTotalObservablePhotons ( LHCb::RichRecTrack * track,
                                               const Rich::ParticleIDType id ) const
{
  return nObservableSignalPhotons( track, id ) + nObservableScatteredPhotons( track, id );
}

double
ExpectedTrackSignal::nEmittedPhotons ( LHCb::RichRecTrack * track,
                                       const Rich::ParticleIDType id ) const
{
  if ( !track->nEmittedPhotons().dataIsValid(id) ) {
    double signal = 0.0;
    for ( LHCb::RichRecTrack::Segments::iterator segment =
            track->richRecSegments().begin();
          segment != track->richRecSegments().end();
          ++segment ) {
      signal += nEmittedPhotons( *segment, id );
    }
    track->setNEmittedPhotons( id, static_cast<LHCb::RichRecTrack::FloatType>(signal) );
  }

  return track->nEmittedPhotons( id );
}

double
ExpectedTrackSignal::nDetectablePhotons ( LHCb::RichRecTrack * track,
                                          const Rich::ParticleIDType id ) const
{
  if ( !track->nDetectablePhotons().dataIsValid(id) ) {
    double signal = 0;
    for ( LHCb::RichRecTrack::Segments::iterator segment =
            track->richRecSegments().begin();
          segment != track->richRecSegments().end();
          ++segment ) {
      signal += nDetectablePhotons( *segment, id );
    }
    track->setNDetectablePhotons( id, static_cast<LHCb::RichRecTrack::FloatType>(signal) );
  }

  return track->nDetectablePhotons( id );
}

bool
ExpectedTrackSignal::activeInRadiator( LHCb::RichRecTrack * track,
                                       const Rich::RadiatorType rad,
                                       const Rich::ParticleIDType id ) const
{
  for ( LHCb::RichRecTrack::Segments::iterator segment =
          track->richRecSegments().begin();
        segment != track->richRecSegments().end();
        ++segment )
  {
    if ( rad == (*segment)->trackSegment().radiator() )
    {
      if ( nEmittedPhotons(*segment,id) > 0 ) return true;
    }
  }

  return false;
}

bool
ExpectedTrackSignal::hasRichInfo( LHCb::RichRecSegment * segment ) const
{
  // default to no info
  bool hasInfo = false;
  // above lowest mass hypothesis threshold ?
  if ( aboveThreshold( segment, m_pidTypes.front() ) )
  {
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "RichRecSegment is above " << m_pidTypes.front()
              << " threshold -> hasRichInfo" << endreq;
    }

    // see if any mass hypothesis is detectable
    for ( Rich::Particles::const_iterator hypo = m_pidTypes.begin();
          hypo != m_pidTypes.end(); ++hypo )
    {
      if ( m_geomEff->geomEfficiency(segment,*hypo) > 0 )
      {
        hasInfo = true; break;
      }
    }
    // Check segment has minimum number of required photons expected
    if ( hasInfo )
    {
      hasInfo = false;
      for ( Rich::Particles::const_iterator hypo = m_pidTypes.begin();
            hypo != m_pidTypes.end(); ++hypo )
      {
        if ( m_minPhotonsPerRad[segment->trackSegment().radiator()] < 
             nObservableSignalPhotons(segment,*hypo) )
        { 
          hasInfo = true; break;
        }
      }
    }
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "RichRecSegment has richInfo = " << hasInfo << endmsg;
    }

  }
  else if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "RichRecSegment is below " << m_pidTypes.front()
            << " threshold -> noRichInfo" << endreq;
  }

  return hasInfo;
}

bool
ExpectedTrackSignal::hasRichInfo( LHCb::RichRecTrack * track ) const
{
  for ( LHCb::RichRecTrack::Segments::iterator segment =
          track->richRecSegments().begin();
        segment != track->richRecSegments().end();
        ++segment )
  {
    if ( hasRichInfo(*segment) ) return true;
  }
  return false;
}

bool
ExpectedTrackSignal::aboveThreshold( LHCb::RichRecSegment * segment,
                                     const Rich::ParticleIDType type ) const
{
  // Geometrical track segment
  const LHCb::RichTrackSegment & tkSeg = segment->trackSegment();

  // momentum for this track segment
  const double P = sqrt(tkSeg.bestMomentum().mag2());
  // Adjust momentum to account for a 1 sigma fluctuation,
  // so segment is really above threshold but measured below
  const double Perr = tkSeg.middleErrors().errP();

  // is this momentum above the cherenkov threshold momentum
  const double pthres = m_richPartProp->thresholdMomentum(type,tkSeg.radiator());
  const bool above = ( P > pthres );
  //const bool above = ( P+Perr > pthres );
  //const bool above = ( P-Perr > pthres );

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Threshold check : " << tkSeg.radiator() << " " << type
            << " : P=" << P << " Perr=" << Perr << " Pthres=" << pthres
            << " : above=" << above << endreq;
  }

  // return status
  return above;
}

bool
ExpectedTrackSignal::aboveThreshold( LHCb::RichRecTrack * track,
                                     const Rich::ParticleIDType type ) const
{
  // loop over segments
  for ( LHCb::RichRecTrack::Segments::iterator segment = track->richRecSegments().begin();
        segment != track->richRecSegments().end();
        ++segment )
  {
    if ( aboveThreshold(*segment,type) ) return true;
  }
  return false;
}

bool
ExpectedTrackSignal::aboveThreshold( LHCb::RichRecTrack * track,
                                     const Rich::ParticleIDType type,
                                     const Rich::RadiatorType radiator ) const
{
  // loop over segments
  for ( LHCb::RichRecTrack::Segments::iterator segment = track->richRecSegments().begin();
        segment != track->richRecSegments().end();
        ++segment )
  {
    if ( radiator == (*segment)->trackSegment().radiator() )
    {
      if ( aboveThreshold( *segment, type ) ) return true;
    }
  }
  return false;
}

// Set the threshold information in a RichPID object for given track
void
ExpectedTrackSignal::setThresholdInfo( LHCb::RichRecTrack * track,
                                       LHCb::RichPID * pid ) const
{

  pid->setElectronHypoAboveThres(false);
  pid->setMuonHypoAboveThres(false);
  pid->setPionHypoAboveThres(false);
  pid->setKaonHypoAboveThres(false);
  pid->setProtonHypoAboveThres(false);
  for ( Rich::Particles::const_iterator hypo = m_pidTypes.begin();
        hypo != m_pidTypes.end(); ++hypo )
  {
    pid->setAboveThreshold(*hypo,aboveThreshold(track,*hypo));
  }

}

// Set the threshold information in a RichPID object for given segment
void
ExpectedTrackSignal::setThresholdInfo( LHCb::RichRecSegment * segment,
                                       LHCb::RichPID * pid ) const
{

  pid->setElectronHypoAboveThres(false);
  pid->setMuonHypoAboveThres(false);
  pid->setPionHypoAboveThres(false);
  pid->setKaonHypoAboveThres(false);
  pid->setProtonHypoAboveThres(false);
  for ( Rich::Particles::const_iterator hypo = m_pidTypes.begin();
        hypo != m_pidTypes.end(); ++hypo )
  {
    pid->setAboveThreshold(*hypo,aboveThreshold(segment,*hypo));
  }

}
