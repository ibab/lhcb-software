// $Id: RichRecSegmentTool.cpp,v 1.6 2003-04-09 12:36:50 cattanem Exp $

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"

// local
#include "RichRecSegmentTool.h"

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"


//-----------------------------------------------------------------------------
// Implementation file for class : RichRecSegmentTool
//
// 15/03/2002 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichRecSegmentTool>          s_factory ;
const        IToolFactory& RichRecSegmentToolFactory = s_factory ;

// Standard constructor
RichRecSegmentTool::RichRecSegmentTool ( const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent )
  : AlgTool( type, name, parent ) {

  declareInterface<IRichRecSegmentTool>(this);

  // Define job option parameters
  declareProperty( "RichRecSegmentLocation",
                   m_richRecSegmentLocation = RichRecSegmentLocation::Default );
  declareProperty( "NPhotonsGeomEffCalc", m_nGeomEff = 100 );
  declareProperty( "NPhotonsGeomEffBailout", m_nGeomEffBailout = 20 );
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

  // randomn number distribution
  Rndm::Numbers m_uniDist;

}

StatusCode RichRecSegmentTool::initialize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Initialize" << endreq;

  StatusCode sc = StatusCode::SUCCESS;

  // Get pointer to Event Data service
  if ( !serviceLocator()->service( "EventDataSvc", m_evtDataSvc, true ) ) {
    msg << MSG::ERROR << "EventDataSvc not found" << endreq;
    sc = StatusCode::FAILURE;
  }

  // Retrieve particle property service
  if ( !toolSvc()->retrieveTool( "RichDetInterface", m_richDetInterface ) ) {
    msg << MSG::ERROR << "RichDetInterface not found" << endreq;
    sc = StatusCode::FAILURE;
  }

  // Retrieve particle property service
  if ( !serviceLocator()->service( "ParticlePropertySvc", m_ppSvc ) ) {
    msg << MSG::ERROR << "ParticlePropertySvc not found" << endreq;
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

  // Get pointer to Detector Data service
  if ( !serviceLocator()->service( "DetectorDataSvc", m_detDataSvc, true ) ) {
    msg << MSG::ERROR << "DetectorDataSvc not found" << endreq;
    sc = StatusCode::FAILURE;
  }

  // randomn number service
  IRndmGenSvc * randSvc;
  if ( serviceLocator()->service( "RndmGenSvc", randSvc, true ) ) {
    if ( !m_uniDist.initialize( randSvc, Rndm::Flat(0,1) ) ) {
      msg << MSG::ERROR << "Unable to initialise randomn numbers" << endreq;
      sc = StatusCode::FAILURE;
    }
  } else {
    sc = StatusCode::FAILURE;
  }

  // If all ok, continue
  if ( !sc ) return sc;

  // Set up parameters for geometrical efficiency calculation
  m_hpdInc = 1.0 / ( (double)m_nGeomEff );
  m_incPhi = M_PI/2.0 + M_2PI/( (double)m_nGeomEff );
  //m_incPhi = M_2PI/( (double)m_nGeomEff );
  double ckPhi = 0.0;
  m_sinCkPhi.clear();
  m_cosCkPhi.clear();
  for ( int iPhot = 0; iPhot < m_nGeomEff; ++iPhot, ckPhi += m_incPhi ) {
    m_sinCkPhi.push_back( sin(ckPhi) );
    m_cosCkPhi.push_back( cos(ckPhi) );
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
  m_referenceQE = m_richDetInterface->hpdQuantumEff();
  if ( !m_referenceQE ) {
    msg << MSG::ERROR << "Failed to acquire QE function" << endreq;
    return StatusCode::FAILURE;
  }

  // Retrieve particle masses
  m_particleMass.push_back( m_ppSvc->find("e+" )->mass()/MeV );
  m_particleMass.push_back( m_ppSvc->find("mu+")->mass()/MeV );
  m_particleMass.push_back( m_ppSvc->find("pi+")->mass()/MeV );
  m_particleMass.push_back( m_ppSvc->find("K+" )->mass()/MeV );
  m_particleMass.push_back( m_ppSvc->find("p+" )->mass()/MeV );
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
  for ( int iRad = 0; iRad < Rich::NRadiatorTypes; ++iRad ) {
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
                                         m_pedLoss );

      // Cached refractive index for each energy bin
      m_binRefIndex[iRad].push_back( refractiveIndex( rad, eBin ) );

    } // end energy loop
  } // end radiator loop

  // Setup momentum thresholds
  m_AvRefIndex.push_back( refractiveIndex(Rich::Aerogel) );
  m_AvRefIndex.push_back( refractiveIndex(Rich::C4F10) );
  m_AvRefIndex.push_back( refractiveIndex(Rich::CF4) );
  for ( int iRad = 0; iRad < Rich::NRadiatorTypes; ++iRad ) {
    for ( int iHypo = 0; iHypo < Rich::NParticleTypes; ++iHypo ) {
      m_momThres[iRad][iHypo] = m_particleMass[iHypo]/
        sqrt( m_AvRefIndex[iRad]*m_AvRefIndex[iRad] - 1.0);
    }
  }

  // Informational Printout
  msg << MSG::DEBUG << "Tool Parameters :-" << endreq
      << " Photon Energy Bins         = " << m_EnergyBins << endreq
      << " GeomEff Phots Max/Bailout  = " << m_nGeomEff << "/" << m_nGeomEffBailout << endreq;
  //<< " Particle masses (MeV)      = " << m_particleMass << endreq
  //<< " Average refractive indices = " << m_AvRefIndex << endreq;
  //for ( int iR = 0; iR < Rich::NRadiatorTypes; ++iR ) {
  // msg << " " << (Rich::RadiatorType)iR << " Res. bins = " << m_thebin[iR] << endreq;
  // for ( int iT = 0; iT < Rich::Track::NTrTypes; ++iT ) {
  //   msg << " " << (Rich::RadiatorType)iR << " " << (Rich::Track::Type)iT
  //       << " Ck Res. = " << m_theerr[iR][iT] << endreq;
  // }
  //}

  return sc;
}

StatusCode RichRecSegmentTool::finalize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Finalize" << endreq;

  // Release tools
  if ( m_richDetInterface ) toolSvc()->releaseTool( m_richDetInterface );

  // Release the random generator
  m_uniDist.finalize();

  return StatusCode::SUCCESS;
}

// Method that handles various Gaudi "software events"
void RichRecSegmentTool::handle ( const Incident& incident ) {

  if ( "BeginEvent" == incident.type() ) {

    SmartDataPtr<RichRecSegments> tdsSegments( m_evtDataSvc,
                                               m_richRecSegmentLocation );
    if ( !tdsSegments ) {

      // Reinitialise the Photon Container
      m_segments = new RichRecSegments();

      // Register new RichRecPhoton container to Gaudi data store
      if (!m_evtDataSvc->registerObject(m_richRecSegmentLocation, m_segments)) {
        MsgStream msg( msgSvc(), name() );
        msg << MSG::ERROR << "Failed to register RichRecSegments at "
            << m_richRecSegmentLocation << endreq;
      }

    } else {

      // Set smartref to TES segment container
      m_segments = tdsSegments;

    }

  } // begin event if

}

// Forms a new RichRecSegment object
void RichRecSegmentTool::saveSegment ( RichRecSegment * segment ) {
  m_segments->insert( segment );
}

double
RichRecSegmentTool::nSignalPhotons ( RichRecSegment * segment,
                                     const Rich::ParticleIDType& id ) {

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
RichRecSegmentTool::nScatteredPhotons ( RichRecSegment * segment,
                                        const Rich::ParticleIDType& id ) {

  double signal = segment->nScatteredPhotons( id );
  if ( signal < -0.5 ) {
    // Scattered componented is calculated with the signal
    nSignalPhotons( segment, id );
  }

  return segment->nScatteredPhotons( id );
}

double RichRecSegmentTool::photonUnscatteredProb( const double& energy,
                                                  const double& path ) {
  if ( energy <= 0 || path <= 0 ) return 0;
  double lambda = m_rayleighPara[0]/energy;
  double scatLeng = lambda*lambda*lambda*lambda/m_rayleighPara[1];
  return (scatLeng/path)*(1-exp(-1*path/scatLeng));
}

double RichRecSegmentTool::nDetectablePhotons ( RichRecSegment * segment,
                                                const Rich::ParticleIDType& id ) {

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

double RichRecSegmentTool::nEmittedPhotons ( RichRecSegment * segment,
                                             const Rich::ParticleIDType& id ) {

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

double RichRecSegmentTool::geomEfficiency ( RichRecSegment * segment,
                                            const Rich::ParticleIDType& id ) {

  double eff = segment->geomEfficiency( id );
  if ( eff < -0.5 ) {

    // Cherenkov theta for this segment/hypothesis combination
    double ckTheta = this->avgCherenkovTheta( segment, id );
    if ( ckTheta <= 0 ) {
      eff = 0;
    } else {

      RichTrackSegment & trackSeg = segment->trackSegment();

      // Define rotation matrix
      HepVector3D z = trackSeg.bestMomentum().unit();
      HepVector3D y = ( fabs( z * HepVector3D(1.,0.,0.) ) < 1. ?
                        z.cross( HepVector3D(0.,1.,0.) ) :
                        z.cross( HepVector3D(1.,0.,0.) ) );
      y.setMag(1);
      HepVector3D x = y.cross(z);
      HepRotation rotation = HepRotation();
      rotation.rotateAxes(x,y,z);

      int nDetect = 0;
      double sinCkTheta = sin(ckTheta);
      double cosCkTheta = cos(ckTheta);
      for ( int iPhot = 0; iPhot < m_nGeomEff; ++iPhot ) {

        // Photon emission point is random between segment start and end points
        HepPoint3D emissionPt = trackSeg.bestPoint( m_uniDist() );

        // Photon direction around loop
        HepVector3D photDir = rotation*HepVector3D( sinCkTheta*m_cosCkPhi[iPhot],
                                                    sinCkTheta*m_sinCkPhi[iPhot],
                                                    cosCkTheta );

        // Ray trace through detector
        RichGeomPhoton photon;
        if ( 0 != m_richDetInterface->traceToDetector( trackSeg.rich(),
                                                       emissionPt,
                                                       photDir,
                                                       photon ) ) {
          ++nDetect;
          segment->addToGeomEfficiencyPerHPD( id,
                                              (int)(photon.smartID().hpdID()),
                                              m_hpdInc );
          if ( photon.detectionPoint().x() > 0 ) {
            segment->setPhotonsInXPlus(1);
          } else {
            segment->setPhotonsInXMinus(1);
          }
          if ( photon.detectionPoint().y() > 0 ) {
            segment->setPhotonsInYPlus(1);
          } else {
            segment->setPhotonsInYMinus(1);
          }

        }

        // Bail out if tried m_geomEffBailout times and all have failed
        if ( 0 == nDetect && iPhot >= m_nGeomEffBailout ) break;

      } // fake photon loop

      eff = (double)nDetect/(double)m_nGeomEff;

    } // CK theta IF

    segment->setGeomEfficiency( id, eff );
  }

  return eff;
}

double RichRecSegmentTool::geomEfficiencyScat ( RichRecSegment * segment,
                                                const Rich::ParticleIDType& id ) {

  double eff = segment->geomEfficiencyScat( id );
  if ( eff < -0.5 ) {

    // only for aerogel
    if ( segment->trackSegment().radiator() != Rich::Aerogel ) {
      eff = 0;
    } else {

      RichTrackSegment & trackSeg = segment->trackSegment();

      // Photon emission point is end of aerogel
      HepPoint3D emissionPt = trackSeg.exitPoint();

      // Define rotation matrix
      HepVector3D z = trackSeg.bestMomentum().unit();
      HepVector3D y = ( fabs( z * HepVector3D(1.,0.,0.) ) < 1. ?
                        z.cross( HepVector3D(0.,1.,0.) ) :
                        z.cross( HepVector3D(1.,0.,0.) ) );
      y.setMag(1);
      HepVector3D x = y.cross(z);
      HepRotation rotation = HepRotation();
      rotation.rotateAxes(x,y,z);

      int nDetect = 0;
      RichGeomPhoton photon;
      for ( int iPhot = 0; iPhot < m_nGeomEff; ++iPhot ) {

        // generate randomn cos(theta)**2 distribution for thetaCk
        double ckTheta;
        do {
          ckTheta = m_uniDist()*M_PI;
        } while ( m_uniDist() > pow(cos(ckTheta),2) );

        // Photon direction around loop
        HepVector3D photDir = rotation*HepVector3D( sin(ckTheta)*m_cosCkPhi[iPhot],
                                                    sin(ckTheta)*m_sinCkPhi[iPhot],
                                                    cos(ckTheta) );

        // Ray trace through detector
        if ( 0 != m_richDetInterface->traceToDetector( trackSeg.rich(),
                                                       emissionPt,
                                                       photDir,
                                                       photon ) ) { ++nDetect; }

        // Bail out if tried m_geomEffBailout times and all have failed
        if ( 0 == nDetect && iPhot >= m_nGeomEffBailout ) break;

      } // fake photon loop

      eff = (double)nDetect/(double)m_nGeomEff;

    } // CK theta IF

    // For time being use same efficiency for all hypotheses
    segment->setGeomEfficiencyScat( Rich::Electron, eff );
    segment->setGeomEfficiencyScat( Rich::Muon, eff );
    segment->setGeomEfficiencyScat( Rich::Pion, eff );
    segment->setGeomEfficiencyScat( Rich::Kaon, eff );
    segment->setGeomEfficiencyScat( Rich::Proton, eff );

  }

  return eff;
}

double RichRecSegmentTool::avgCherenkovTheta( RichRecSegment * segment,
                                              const Rich::ParticleIDType& id ) {

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

double RichRecSegmentTool::ckThetaResolution( RichRecSegment * segment,
                                              const Rich::ParticleIDType& id ) {

  // Expected Cherenkov theta angle
  double thetaExp = this->avgCherenkovTheta( segment, id );
  if ( thetaExp < 0.000001 ) return 0;

  Rich::RadiatorType rad = segment->trackSegment().radiator();
  Rich::Track::Type type = segment->richRecTrack()->trackType();
  if ( thetaExp > 0. &&  thetaExp < (m_thebin[rad])[0] ) {
    return (m_theerr[rad][type])[0];
  } else if ( thetaExp > (m_thebin[rad])[0] &&
              thetaExp < (m_thebin[rad])[1] ) {
    return (m_theerr[rad][type])[1];
  } else if ( thetaExp > (m_thebin[rad])[1] ) {
    return (m_theerr[rad][type])[2];
  }

  return 0;
}

double RichRecSegmentTool::beta( RichRecSegment * segment,
                                 const Rich::ParticleIDType& id ) {

  double momentum = segment->trackSegment().bestMomentumMag();
  double Esquare = momentum*momentum + m_particleMassSq[id];

  return (Esquare > 0.0 ? momentum/sqrt(Esquare) : 0.0);
}

bool RichRecSegmentTool::hasRichInfo( RichRecSegment * segment ) {

  for ( int iHypo = 0; iHypo < Rich::NParticleTypes; ++iHypo ) {
    if ( geomEfficiency(segment,(Rich::ParticleIDType)iHypo) > 0 ) return true;
  }

  return false;
}

RichRecSegments * RichRecSegmentTool::richSegments() {
  return m_segments;
}

double RichRecSegmentTool::nTotalObservablePhotons ( RichRecSegment * segment,
                                                     const Rich::ParticleIDType& id ) {
  return ( geomEfficiency(segment,id) * nSignalPhotons(segment,id) ) +
    ( geomEfficiencyScat(segment,id) * nScatteredPhotons(segment,id) );
}

double RichRecSegmentTool::nObservableSignalPhotons ( RichRecSegment * segment,
                                                      const Rich::ParticleIDType& id ) {
  return geomEfficiency(segment,id) * nSignalPhotons(segment,id);
}

double RichRecSegmentTool::nObservableScatteredPhotons ( RichRecSegment * segment,
                                                         const Rich::ParticleIDType & id ) {
  return geomEfficiencyScat(segment,id) * nScatteredPhotons(segment,id);
}

bool RichRecSegmentTool::aboveThreshold( RichRecSegment * segment,
                                         const Rich::ParticleIDType& type ) {
  return segment->trackSegment().bestMomentumMag() >
    m_momThres[(int)segment->trackSegment().radiator()][(int)type];
}

double RichRecSegmentTool::refractiveIndex( const Rich::RadiatorType& radiator,
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

double RichRecSegmentTool::refractiveIndex( RichRecSegment * segment ) {
  return refractiveIndex( segment->trackSegment().radiator() );
}

double RichRecSegmentTool::refractiveIndex( const Rich::RadiatorType& radiator ) {

  double meanEnergy = m_referenceQE->meanX( m_referenceQE->minX(),
                                            m_referenceQE->maxX() ) / eV;
  return this->refractiveIndex( radiator, meanEnergy );
}
