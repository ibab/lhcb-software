// $Id: RichSegmentProperties.cpp,v 1.3 2003-07-08 06:31:26 cattanem Exp $

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/IParticlePropertySvc.h"

// local
#include "RichSegmentProperties.h"

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

// Rich Kernel
#include "RichKernel/MessageSvcStl.h"


//-----------------------------------------------------------------------------
// Implementation file for class : RichSegmentProperties
//
// 15/03/2002 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichSegmentProperties>          s_factory ;
const        IToolFactory& RichSegmentPropertiesFactory = s_factory ;

// Standard constructor
RichSegmentProperties::RichSegmentProperties ( const std::string& type,
                                               const std::string& name,
                                               const IInterface* parent )
  : RichRecToolBase( type, name, parent ) {

  declareInterface<IRichSegmentProperties>(this);

  // Define job option parameters

  declareProperty( "ExpPhotonEnergyBins", m_EnergyBins = 5 );

  (m_thebin[Rich::Aerogel]).push_back( 0.2 );
  (m_thebin[Rich::Aerogel]).push_back( 0.25 );
  declareProperty( "NAerogelResBins", m_thebin[Rich::Aerogel] );
  (m_theerr[Rich::Aerogel][Rich::Track::Forward]).push_back( 0.00320 );
  (m_theerr[Rich::Aerogel][Rich::Track::Forward]).push_back( 0.00242 );
  (m_theerr[Rich::Aerogel][Rich::Track::Forward]).push_back( 0.00221 );
  declareProperty( "AerogelForwardRes", m_theerr[Rich::Aerogel][Rich::Track::Forward] );
  (m_theerr[Rich::Aerogel][Rich::Track::Match]).push_back( 0.00323 );
  (m_theerr[Rich::Aerogel][Rich::Track::Match]).push_back( 0.00262 );
  (m_theerr[Rich::Aerogel][Rich::Track::Match]).push_back( 0.00221 );
  declareProperty( "AerogelMatchRes", m_theerr[Rich::Aerogel][Rich::Track::Match] );
  (m_theerr[Rich::Aerogel][Rich::Track::UpStream]).push_back( 0.00300 );
  (m_theerr[Rich::Aerogel][Rich::Track::UpStream]).push_back( 0.00272 );
  (m_theerr[Rich::Aerogel][Rich::Track::UpStream]).push_back( 0.00230 );
  declareProperty( "AerogelUpStreamRes", m_theerr[Rich::Aerogel][Rich::Track::UpStream] );
  (m_theerr[Rich::Aerogel][Rich::Track::Seed]).push_back( 999 );
  (m_theerr[Rich::Aerogel][Rich::Track::Seed]).push_back( 999 );
  (m_theerr[Rich::Aerogel][Rich::Track::Seed]).push_back( 999 );
  declareProperty( "AerogelSeedRes", m_theerr[Rich::Aerogel][Rich::Track::Seed] );
  (m_theerr[Rich::Aerogel][Rich::Track::VeloTT]).push_back( 0.01 );
  (m_theerr[Rich::Aerogel][Rich::Track::VeloTT]).push_back( 0.00478 );
  (m_theerr[Rich::Aerogel][Rich::Track::VeloTT]).push_back( 0.00273 );
  declareProperty( "AerogelVTTRes", m_theerr[Rich::Aerogel][Rich::Track::VeloTT] );
  (m_theerr[Rich::Aerogel][Rich::Track::Velo]).push_back( 999 );
  (m_theerr[Rich::Aerogel][Rich::Track::Velo]).push_back( 999 );
  (m_theerr[Rich::Aerogel][Rich::Track::Velo]).push_back( 999 );

  (m_thebin[Rich::C4F10]).push_back( 0.03 );
  (m_thebin[Rich::C4F10]).push_back( 0.05 );
  declareProperty( "NC4F10ResBins", m_thebin[Rich::C4F10] );
  (m_theerr[Rich::C4F10][Rich::Track::Forward]).push_back( 0.00357 );
  (m_theerr[Rich::C4F10][Rich::Track::Forward]).push_back( 0.00226 );
  (m_theerr[Rich::C4F10][Rich::Track::Forward]).push_back( 0.00150 );
  declareProperty( "C4F10ForwardRes", m_theerr[Rich::C4F10][Rich::Track::Forward] );
  (m_theerr[Rich::C4F10][Rich::Track::Match]).push_back( 0.00385 );
  (m_theerr[Rich::C4F10][Rich::Track::Match]).push_back( 0.00231 );
  (m_theerr[Rich::C4F10][Rich::Track::Match]).push_back( 0.00180 );
  declareProperty( "C4F10MatchRes", m_theerr[Rich::C4F10][Rich::Track::Match] );
  (m_theerr[Rich::C4F10][Rich::Track::UpStream]).push_back( 0.00393 );
  (m_theerr[Rich::C4F10][Rich::Track::UpStream]).push_back( 0.00242 );
  (m_theerr[Rich::C4F10][Rich::Track::UpStream]).push_back( 0.00242 );
  declareProperty( "C4F10UpStreamRes", m_theerr[Rich::C4F10][Rich::Track::UpStream] );
  (m_theerr[Rich::C4F10][Rich::Track::Seed]).push_back( 999 );
  (m_theerr[Rich::C4F10][Rich::Track::Seed]).push_back( 999 );
  (m_theerr[Rich::C4F10][Rich::Track::Seed]).push_back( 999 );
  declareProperty( "C4F10SeedRes", m_theerr[Rich::C4F10][Rich::Track::Seed] );
  (m_theerr[Rich::C4F10][Rich::Track::VeloTT]).push_back( 0.00806 );
  (m_theerr[Rich::C4F10][Rich::Track::VeloTT]).push_back( 0.00307 );
  (m_theerr[Rich::C4F10][Rich::Track::VeloTT]).push_back( 0.00211 );
  declareProperty( "C4F10VTTRes", m_theerr[Rich::C4F10][Rich::Track::VeloTT] );
  (m_theerr[Rich::C4F10][Rich::Track::Velo]).push_back( 999 );
  (m_theerr[Rich::C4F10][Rich::Track::Velo]).push_back( 999 );
  (m_theerr[Rich::C4F10][Rich::Track::Velo]).push_back( 999 );

  (m_thebin[Rich::CF4]).push_back( 0.03 );
  (m_thebin[Rich::CF4]).push_back( 0.05 );
  declareProperty( "NCF4ResBins", m_thebin[Rich::CF4] );
  (m_theerr[Rich::CF4][Rich::Track::Forward]).push_back( 0.00119 );
  (m_theerr[Rich::CF4][Rich::Track::Forward]).push_back( 0.00122 );
  (m_theerr[Rich::CF4][Rich::Track::Forward]).push_back( 0.00083 );
  declareProperty( "CF4ForwardRes", m_theerr[Rich::CF4][Rich::Track::Forward] );
  (m_theerr[Rich::CF4][Rich::Track::Match]).push_back( 0.00119 );
  (m_theerr[Rich::CF4][Rich::Track::Match]).push_back( 0.00132 );
  (m_theerr[Rich::CF4][Rich::Track::Match]).push_back( 0.00091 );
  declareProperty( "CF4MatchRes", m_theerr[Rich::CF4][Rich::Track::Match] );
  (m_theerr[Rich::CF4][Rich::Track::UpStream]).push_back( 0.00133 );
  (m_theerr[Rich::CF4][Rich::Track::UpStream]).push_back( 0.00131 );
  (m_theerr[Rich::CF4][Rich::Track::UpStream]).push_back( 0.00109 );
  declareProperty( "CF4UpStreamRes", m_theerr[Rich::CF4][Rich::Track::UpStream] );
  (m_theerr[Rich::CF4][Rich::Track::Seed]).push_back( 0.00138 );
  (m_theerr[Rich::CF4][Rich::Track::Seed]).push_back( 0.00117 );
  (m_theerr[Rich::CF4][Rich::Track::Seed]).push_back( 0.00099 );
  declareProperty( "CF4SeedRes", m_theerr[Rich::CF4][Rich::Track::Seed] );
  (m_theerr[Rich::CF4][Rich::Track::VeloTT]).push_back( 999 );
  (m_theerr[Rich::CF4][Rich::Track::VeloTT]).push_back( 999 );
  (m_theerr[Rich::CF4][Rich::Track::VeloTT]).push_back( 999 );
  declareProperty( "CF4VTTRes", m_theerr[Rich::CF4][Rich::Track::VeloTT] );
  (m_theerr[Rich::CF4][Rich::Track::Velo]).push_back( 999 );
  (m_theerr[Rich::CF4][Rich::Track::Velo]).push_back( 999 );
  (m_theerr[Rich::CF4][Rich::Track::Velo]).push_back( 999 );

  // temporary parameters to take into acount degraded performance for robustness tests
  declareProperty( "ScalePhotonEff", m_photonEffScale = 1 );
  m_emisPntErrScale.push_back( 1 );
  m_emisPntErrScale.push_back( 1 );
  m_emisPntErrScale.push_back( 1 );
  declareProperty( "ScaleEmisPntErr", m_emisPntErrScale );

}

StatusCode RichSegmentProperties::initialize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Initialize" << endreq;

  StatusCode sc = StatusCode::SUCCESS;

  // Sets up various tools and services
  if ( !RichRecToolBase::initialize() ) return StatusCode::FAILURE;

  // Acquire instances of tools
  acquireTool("RichDetInterface", m_richDetInt);
  acquireTool("RichGeomEff", m_geomEff);

  // Retrieve particle property service
  IParticlePropertySvc * ppSvc = 0;
  if ( !serviceLocator()->service( "ParticlePropertySvc", ppSvc ) ) {
    msg << MSG::ERROR << "ParticlePropertySvc not found" << endreq;
    sc = StatusCode::FAILURE;
  }

  // Define mirror reflectivities and energy cutoff values.
  // Private implementation that should be removed at some later date and
  // RichDetInterface used instead
  m_detReflectorEff = 0.9*0.9;

  // Rayleigh scattering parameters
  m_rayleighPara[0] = 1.24; // Transform from eV to microns
  m_rayleighPara[1] = 0.0008;

  // Quartz window eff
  m_quartzWinEff = 0.92;

  // Digitisation pedestal loss
  m_pedLoss = 0.899999976;

  // Get the Quantum Eff
  // For time being assume only one reference curve for all HPDs
  m_referenceQE = m_richDetInt->hpdQuantumEff();
  if ( !m_referenceQE ) {
    msg << MSG::ERROR << "Failed to acquire QE function" << endreq;
    return StatusCode::FAILURE;
  }

  // Retrieve particle masses
  m_particleMass.push_back( ppSvc->find("e+" )->mass()/MeV );
  m_particleMass.push_back( ppSvc->find("mu+")->mass()/MeV );
  m_particleMass.push_back( ppSvc->find("pi+")->mass()/MeV );
  m_particleMass.push_back( ppSvc->find("K+" )->mass()/MeV );
  m_particleMass.push_back( ppSvc->find("p+" )->mass()/MeV );
  // cache squares of masses
  m_particleMassSq.push_back( m_particleMass[ Rich::Electron ] *
                              m_particleMass[ Rich::Electron ] );
  m_particleMassSq.push_back( m_particleMass[ Rich::Muon ] *
                              m_particleMass[ Rich::Muon ] );
  m_particleMassSq.push_back( m_particleMass[ Rich::Pion ] *
                              m_particleMass[ Rich::Pion ] );
  m_particleMassSq.push_back( m_particleMass[ Rich::Kaon ] *
                              m_particleMass[ Rich::Kaon ] );
  m_particleMassSq.push_back( m_particleMass[ Rich::Proton ] *
                              m_particleMass[ Rich::Proton ] );

  // Initialise W function for expected signal calculation
  // Hardcoded parameters should be moved to database
  m_selF1[Rich::Aerogel] = 2.653;
  m_selF1[Rich::C4F10]   = 12305.3;
  m_selF1[Rich::CF4]     = 7626.16;
  m_selF2[Rich::Aerogel] = 13.075;
  m_selF2[Rich::C4F10]   = 0.0;
  m_selF2[Rich::CF4]     = 0.0;
  m_selE1[Rich::Aerogel] = 10.666;
  m_selE1[Rich::C4F10]   = 18.938;
  m_selE1[Rich::CF4]     = 20.003;
  m_selE2[Rich::Aerogel] = 18.125;
  m_selE2[Rich::C4F10]   = 50.0;
  m_selE2[Rich::CF4]     = 50.0;
  m_molW[Rich::Aerogel]  = 0;
  m_molW[Rich::C4F10]    = 138.0;
  m_molW[Rich::CF4]      = 88.0;
  m_rho[Rich::Aerogel]   = 0;
  m_rho[Rich::C4F10]     = 0.00964;
  m_rho[Rich::CF4]       = 0.00366;
  m_maxPhotEn[Rich::Aerogel] = 3.5;
  m_maxPhotEn[Rich::C4F10]   = 7.0;
  m_maxPhotEn[Rich::CF4]     = 7.0;
  m_minPhotEn[Rich::Aerogel] = 1.75;
  m_minPhotEn[Rich::C4F10]   = 1.75;
  m_minPhotEn[Rich::CF4]     = 1.75;

  // Use formulae 37-39 in CERN-EP/89-150  (Ypsilantis)
  // This calculation should perhaps live in Det package ?
  {for ( int iRad = 0; iRad < Rich::NRadiatorTypes; ++iRad ) {
    Rich::RadiatorType rad = (Rich::RadiatorType)iRad;

    m_WParaT[iRad].clear();
    m_WParaB[iRad].clear();
    m_eBinValue[iRad].clear();
    m_binEfficiency[iRad].clear();
    m_binRefIndex[iRad].clear();
    m_deltaEn[iRad] = (m_maxPhotEn[iRad]-m_minPhotEn[iRad])/m_EnergyBins;

    // loop over energy bins
    for ( int iEnBin = 0; iEnBin<m_EnergyBins; ++iEnBin ) {
      // Energy bin max, min and central values
      double topEn = m_minPhotEn[iRad] + m_deltaEn[iRad]*(1+iEnBin);
      double botEn = m_minPhotEn[iRad] + m_deltaEn[iRad]*iEnBin;
      double eBin = (topEn+botEn)/2;
      m_eBinValue[iRad].push_back( eBin );

      // compute temporary variables along the way to W function parameters
      double RC3,RC2;
      if ( Rich::Aerogel == iRad ) {
        RC3 = 1.;
        RC2 = 1.;
      } else {
        RC3 = 3.*0.3738*m_rho[iRad]/m_molW[iRad];
        RC2 = 2.*0.3738*m_rho[iRad]/m_molW[iRad];
      }
      double RF = m_selF1[iRad] + m_selF2[iRad];
      double RE02 = ( m_selF1[iRad]*m_selE2[iRad]*m_selE2[iRad] +
                      m_selF2[iRad]*m_selE1[iRad]*m_selE1[iRad] ) / RF;
      double RE = ( m_selE2[iRad]*m_selE2[iRad] +
                    m_selE1[iRad]*m_selE1[iRad] ) / RF;
      double RG = ( m_selE1[iRad]*m_selE1[iRad] *
                    m_selE2[iRad]*m_selE2[iRad] ) / (RF*RE02);
      double RH = RE02/RF;
      double RM = RE + RC2;
      double RS = RG + RC2;
      double RT = sqrt( 0.25*RM*RM - RH*RS );
      double RXSP = sqrt( (RM/2. + RT)/RH );
      double RXSM = sqrt( (RM/2. - RT)/RH );
      double REP = sqrt(RE02) * RXSP;
      double REM = sqrt(RE02) * RXSM;
      double XETEP = (RXSP - 1./RXSP) * log( (REP+topEn)/(REP-topEn) );
      double XETEM = (RXSM - 1./RXSM) * log( (REM+topEn)/(REM-topEn) );
      double XEBEP = (RXSP - 1./RXSP) * log( (REP+botEn)/(REP-botEn) );
      double XEBEM = (RXSM - 1./RXSM) * log( (REM+botEn)/(REM-botEn) );
      // finally store W parameters
      (m_WParaT[iRad]).push_back( (RC3*sqrt(RE02)/(4.*RT))*(XETEP-XETEM) );
      (m_WParaB[iRad]).push_back( (RC3*sqrt(RE02)/(4.*RT))*(XEBEP-XEBEM) );

      // cache QE * quartz window efficiency * mirror reflectivity
      // * loss under pedestal for each energy bin
      (m_binEfficiency[iRad]).push_back( (*m_referenceQE)[eBin*eV]/100 *
                                         m_quartzWinEff * m_detReflectorEff *
                                         m_pedLoss * m_photonEffScale );

      // Cached refractive index for each energy bin
      m_binRefIndex[iRad].push_back( refractiveIndex( rad, eBin ) );

    } // end energy loop
  }} // end radiator loop

  // Setup momentum thresholds
  m_AvRefIndex.push_back( refractiveIndex(Rich::Aerogel) );
  m_AvRefIndex.push_back( refractiveIndex(Rich::C4F10) );
  m_AvRefIndex.push_back( refractiveIndex(Rich::CF4) );
  {for ( int iRad = 0; iRad < Rich::NRadiatorTypes; ++iRad ) {
    for ( int iHypo = 0; iHypo < Rich::NParticleTypes; ++iHypo ) {
      m_momThres[iRad][iHypo] = m_particleMass[iHypo]/
        sqrt( m_AvRefIndex[iRad]*m_AvRefIndex[iRad] - 1.0);
    }
  }}

  // Informational Printout
  msg << MSG::DEBUG
      << " Photon Energy Bins         = " << m_EnergyBins << endreq
      << " Particle masses (MeV)      = " << m_particleMass << endreq
      << " Average refractive indices = " << m_AvRefIndex << endreq;
  for ( int iR = 0; iR < Rich::NRadiatorTypes; ++iR ) {
    msg << " " << (Rich::RadiatorType)iR << " Res. bins = " << m_thebin[iR] << endreq;
    for ( int iT = 0; iT < Rich::Track::NTrTypes; ++iT ) {
      msg << " " << (Rich::RadiatorType)iR << " " << (Rich::Track::Type)iT
          << " Ck Res. = " << m_theerr[iR][iT] << endreq;
    }
  }

  ppSvc->release();
  return sc;
}

StatusCode RichSegmentProperties::finalize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Finalize" << endreq;

  // release tools
  releaseTool( m_richDetInt );
  releaseTool( m_geomEff );

  // Execute base class method
  return RichRecToolBase::finalize();
}

double
RichSegmentProperties::nSignalPhotons ( RichRecSegment * segment,
                                        const Rich::ParticleIDType id ) {

  double signal = segment->nSignalPhotons( id );
  if ( signal < -0.5 ) {
    signal = 0;
    double scatter = 0;

    // compute detectable emitted photons
    double detectablePhots = nDetectablePhotons( segment, id );
    if ( detectablePhots> 0 ) {

      Rich::RadiatorType rad = segment->trackSegment().radiator();

      // loop over energy bins
      std::vector<float>& photsPerBin = segment->nEmitPhotsPerEnBin( id );
      for ( int iEnBin = 0; iEnBin<m_EnergyBins; ++iEnBin ) {

        double unscattProb = 1;
        if ( rad == Rich::Aerogel ) {
          unscattProb = photonUnscatteredProb( (m_eBinValue[rad])[iEnBin],
                                               segment->trackSegment().pathLength() );
        }

        // observable photons * signal/scatter prob
        signal  += photsPerBin[iEnBin] * unscattProb;
        scatter += photsPerBin[iEnBin] * (1-unscattProb);
        photsPerBin[iEnBin] *= unscattProb;

      } // energy bin loop

    }

    segment->setNSignalPhotons( id, signal );
    segment->setNScatteredPhotons( id, scatter );
  }

  return signal;
}

double
RichSegmentProperties::nScatteredPhotons ( RichRecSegment * segment,
                                           const Rich::ParticleIDType id ) {

  double signal = segment->nScatteredPhotons( id );
  if ( signal < -0.5 ) {
    // Scattered componented is calculated with the signal
    nSignalPhotons( segment, id );
  }

  return segment->nScatteredPhotons( id );
}

double RichSegmentProperties::photonUnscatteredProb( const double energy,
                                                     const double path ) {
  if ( energy <= 0 || path <= 0 ) return 0;
  double lambda = m_rayleighPara[0]/energy;
  double scatLeng = lambda*lambda*lambda*lambda/m_rayleighPara[1];
  return (scatLeng/path)*(1-exp(-1*path/scatLeng));
}

double RichSegmentProperties::nDetectablePhotons ( RichRecSegment * segment,
                                                   const Rich::ParticleIDType id ) {

  double signal = segment->nDetectablePhotons( id );
  if ( signal < -0.5 ) {

    // Make sure emitted Photons is calculated
    nEmittedPhotons( segment, id );

    int iRad = (int)segment->trackSegment().radiator();

    // loop over energy bins
    signal = 0;
    std::vector<float>& photsPerBin = segment->nEmitPhotsPerEnBin( id );
    for ( int iEnBin = 0; iEnBin<m_EnergyBins; ++iEnBin ) {
      // scale by QE and quartz window efficiency
      photsPerBin[iEnBin] *= (m_binEfficiency[iRad])[iEnBin];
      signal += photsPerBin[iEnBin];
    }

    segment->setNDetectablePhotons( id, signal );
  }

  return signal;
}

double RichSegmentProperties::nEmittedPhotons ( RichRecSegment * segment,
                                                const Rich::ParticleIDType id ) {

  double signal = segment->nEmittedPhotons( id );
  if ( signal < -0.5 ) {

    // Some parameters of the segment
    double momentum = segment->trackSegment().bestMomentumMag();
    double Esq = momentum * momentum + m_particleMassSq[id];
    double betaSq = ( Esq>0 ? momentum*momentum/Esq : 0 );
    double gammaSq = Esq/m_particleMassSq[id];
    double length = segment->trackSegment().pathLength();
    int iRad = (int)segment->trackSegment().radiator();

    std::vector<float>& photsPerBin = segment->nEmitPhotsPerEnBin( id );
    photsPerBin.reserve( m_EnergyBins );

    // loop over energy bins
    signal = 0;
    for ( int iEnBin = 0; iEnBin<m_EnergyBins; ++iEnBin ) {
      double topEn = m_minPhotEn[iRad] + m_deltaEn[iRad]*(1+iEnBin);
      double botEn = m_minPhotEn[iRad] + m_deltaEn[iRad]*iEnBin;
      double phots = ( 37.0 * length / betaSq ) * ( (m_WParaT[iRad])[iEnBin] -
                                                    (m_WParaB[iRad])[iEnBin] -
                                                    (topEn-botEn)/gammaSq );
      if ( phots<0 ) phots = 0;
      photsPerBin.push_back( phots );
      signal += phots;
    }

    segment->setNEmittedPhotons( id, signal );
  }

  return signal;
}

double RichSegmentProperties::avgCherenkovTheta( RichRecSegment * segment,
                                                 const Rich::ParticleIDType id ) {

  double angle = segment->averageCKTheta( id );
  if ( angle < -0.5 ) {
    angle = 0;

    Rich::RadiatorType rad = segment->trackSegment().radiator();

    // total unscattered signal
    double unscat = nSignalPhotons( segment, id );
    if ( unscat > 0 ) {
      std::vector<float>& photsPerBin = segment->nEmitPhotsPerEnBin( id );
      for ( int iEnBin = 0; iEnBin<m_EnergyBins; ++iEnBin ) {
        double temp = (m_binRefIndex[rad])[iEnBin] * beta(segment, id);
        angle += photsPerBin[iEnBin] * ( temp>1 ? acos(1/temp) : 0 );
      }
      angle = angle/unscat;
    }

    segment->setAverageCKTheta( id, angle );
  }

  return angle;
}

double RichSegmentProperties::ckThetaResolution( RichRecSegment * segment,
                                                 const Rich::ParticleIDType id ) {

  // Expected Cherenkov theta angle
  double thetaExp = this->avgCherenkovTheta( segment, id );
  if ( thetaExp < 0.000001 ) return 0;

  Rich::RadiatorType rad = segment->trackSegment().radiator();
  Rich::Track::Type type = segment->richRecTrack()->trackType();
  double res = 0;
  if ( thetaExp > 0. &&  thetaExp < (m_thebin[rad])[0] ) {
    res = (m_theerr[rad][type])[0];
  } else if ( thetaExp > (m_thebin[rad])[0] &&
              thetaExp < (m_thebin[rad])[1] ) {
    res = (m_theerr[rad][type])[1];
  } else if ( thetaExp > (m_thebin[rad])[1] ) {
    res = (m_theerr[rad][type])[2];
  }

  // Scale for robustness tests and return
  return ( res *= m_emisPntErrScale[rad] );
}

double RichSegmentProperties::beta( RichRecSegment * segment,
                                    const Rich::ParticleIDType id ) {

  double momentum = segment->trackSegment().bestMomentumMag();
  double Esquare = momentum*momentum + m_particleMassSq[id];

  return (Esquare > 0.0 ? momentum/sqrt(Esquare) : 0.0);
}

bool RichSegmentProperties::hasRichInfo( RichRecSegment * segment ) {

  for ( int iHypo = 0; iHypo < Rich::NParticleTypes; ++iHypo ) {
    if ( m_geomEff->geomEfficiency(segment,(Rich::ParticleIDType)iHypo) > 0 ) return true;
  }

  return false;
}

double RichSegmentProperties::nTotalObservablePhotons ( RichRecSegment * segment,
                                                        const Rich::ParticleIDType id ) {
  return ( m_geomEff->geomEfficiency(segment,id) * nSignalPhotons(segment,id) ) +
    ( m_geomEff->geomEfficiencyScat(segment,id) * nScatteredPhotons(segment,id) );
}

double RichSegmentProperties::nObservableSignalPhotons ( RichRecSegment * segment,
                                                         const Rich::ParticleIDType id ) {
  return m_geomEff->geomEfficiency(segment,id) * nSignalPhotons(segment,id);
}

double RichSegmentProperties::nObservableScatteredPhotons ( RichRecSegment * segment,
                                                            const Rich::ParticleIDType id ) {
  return m_geomEff->geomEfficiencyScat(segment,id) * nScatteredPhotons(segment,id);
}

bool RichSegmentProperties::aboveThreshold( RichRecSegment * segment,
                                            const Rich::ParticleIDType type ) {
  return segment->trackSegment().bestMomentumMag() >
    m_momThres[(int)segment->trackSegment().radiator()][(int)type];
}

double RichSegmentProperties::refractiveIndex( const Rich::RadiatorType radiator,
                                               double energy ) {

  double index = 0;
  double fe =
    m_selF1[radiator]/(m_selE1[radiator]*m_selE1[radiator] - energy*energy) +
    m_selF2[radiator]/(m_selE2[radiator]*m_selE2[radiator] - energy*energy);
  if ( Rich::Aerogel == radiator ) {
    index = sqrt( fe + 1 );
  } else {
    double cfe = 0.3738*(m_rho[radiator]/m_molW[radiator])*fe;
    index = sqrt( (1+2*cfe)/(1-cfe) );
  }

  return index;
}

double RichSegmentProperties::refractiveIndex( RichRecSegment * segment ) {
  return refractiveIndex( segment->trackSegment().radiator() );
}

double RichSegmentProperties::refractiveIndex( const Rich::RadiatorType radiator ) {

  double meanEnergy = m_referenceQE->meanX( m_referenceQE->minX(),
                                            m_referenceQE->maxX() ) / eV;
  return this->refractiveIndex( radiator, meanEnergy );
}

// Set the threshold information in a RichPID object for given segment
void RichSegmentProperties::setThresholdInfo( RichRecSegment * segment,
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
