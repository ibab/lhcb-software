// $Id: RichRecSegmentTool.cpp,v 1.2 2002-11-14 13:54:25 jonrob Exp $

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"

// local
#include "RichRecTools/RichRecSegmentTool.h"

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
RichRecSegmentTool::RichRecSegmentTool( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
  : AlgTool( type, name, parent ) {

  declareInterface<IRichRecSegmentTool>(this);

  // Define job option parameters
  declareProperty( "RichRecSegmentLocation",
                   m_richRecSegmentLocation = RichRecSegmentLocation::Default );
  declareProperty( "NPhotonsGeomEffCalc", m_nGeomEff = 100 );
  declareProperty( "NPhotonsGeomEffBailout", m_ngeomEffBailout = 10 );
  declareProperty( "ChronoTiming", m_timing = false );

}

// Standard destructor
RichRecSegmentTool::~RichRecSegmentTool() {}

StatusCode RichRecSegmentTool::initialize() {

  MsgStream log( msgSvc(), name() );
  log << MSG::DEBUG << "Initialize" << endreq;

  StatusCode sc = StatusCode::SUCCESS;

  // Get pointer to Event Data service
  if ( !serviceLocator()->service( "EventDataSvc", m_evtDataSvc, true ) ) {
    log << MSG::ERROR << "EventDataSvc not found" << endreq;
    sc = StatusCode::FAILURE;
  }

  // Get Pointer to PixelFinder (temporary for SICB data)
  if ( !toolSvc()->retrieveTool( "PixelFinder", m_pixelFinder ) ) {
    log << MSG::ERROR << "PixelFinder not found" << endreq;
    sc = StatusCode::FAILURE;
  }

  // Retrieve particle property service
  if ( !toolSvc()->retrieveTool( "RichDetInterface", m_richDetInterface ) ) {
    log << MSG::ERROR << "RichDetInterface not found" << endreq;
    sc = StatusCode::FAILURE;
  }

  // Retrieve particle property service
  if ( !serviceLocator()->service( "ParticlePropertySvc", m_ppSvc ) ) {
    log << MSG::ERROR << "ParticlePropertySvc not found" << endreq;
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

  // Get pointer to Detector Data service
  if ( !serviceLocator()->service( "DetectorDataSvc", m_detDataSvc, true ) ) {
    log << MSG::ERROR << "DetectorDataSvc not found" << endreq;
    sc = StatusCode::FAILURE;
  }

  // ChronoStat timing
  if ( m_timing ) {
    if ( !serviceLocator()->service( "ChronoStatSvc", m_chrono, true ) ) {
      log << MSG::ERROR << "ChronoStatSvc not found" << endreq;
      m_timing = false;
      sc = StatusCode::FAILURE;
    }
  }

  // If all ok, continue
  if ( !sc ) return sc;

  // Set up some parameters
  m_hpdInc = 1.0 / ( (double)m_nGeomEff );
  m_incPhi = M_PI/2.0 + M_2PI/( (double)m_nGeomEff );

  // Retrieve particle masses
  m_particleMass[ (int)Rich::Electron ] = m_ppSvc->find("e+" )->mass()/MeV;
  m_particleMass[ (int)Rich::Muon ]     = m_ppSvc->find("mu+")->mass()/MeV;
  m_particleMass[ (int)Rich::Pion ]     = m_ppSvc->find("pi+")->mass()/MeV;
  m_particleMass[ (int)Rich::Kaon ]     = m_ppSvc->find("K+" )->mass()/MeV;
  m_particleMass[ (int)Rich::Proton ]   = m_ppSvc->find("p+" )->mass()/MeV;
  log << MSG::DEBUG << "Particle masses (MeV) :"
      << " el=" << m_particleMass[ Rich::Electron ]
      << " mu=" << m_particleMass[ Rich::Muon ]
      << " pi=" << m_particleMass[ Rich::Pion ]
      << " K="  << m_particleMass[ Rich::Kaon ]
      << " P="  << m_particleMass[ Rich::Proton ] << endreq;

  // Setup momentum thresholds
  m_AvRefIndex.push_back( m_richDetInterface->refractiveIndex(Rich::Aerogel) );
  m_AvRefIndex.push_back( m_richDetInterface->refractiveIndex(Rich::C4F10) );
  m_AvRefIndex.push_back( m_richDetInterface->refractiveIndex(Rich::CF4) );
  log << MSG::DEBUG << "Average refractive indices :"
      << " aerogel=" << m_AvRefIndex[(int)Rich::Aerogel]
      << " C4F10="   << m_AvRefIndex[(int)Rich::C4F10]
      << " CF4="     << m_AvRefIndex[(int)Rich::CF4] << endreq;
  for ( int iRad = 0; iRad < Rich::NRadiatorTypes; iRad++ ) {
    log << MSG::DEBUG << (Rich::RadiatorType)iRad 
        << " thresholds (MeV/c) :";
    for ( int iHypo = 0; iHypo < Rich::NParticleTypes; iHypo++ ) {
      m_momThres[iRad][iHypo] = m_particleMass[iHypo]/
        sqrt( m_AvRefIndex[iRad]*m_AvRefIndex[iRad] - 1.0);
      log << " " << (Rich::ParticleIDType)iHypo << "=" << m_momThres[iRad][iHypo];
    }
    log << endreq;
  }

  // Define mirror reflectivities and energy cutoff values.
  // Private implementation that should be removed at some later date and
  // RichDetInterface used instead
  m_detReflectorEff[ Rich::Rich1 ] = 0.9;
  m_detReflectorEff[ Rich::Rich2 ] = 0.9*0.9;

  // Issue health warnings to remind me of various things
  log << MSG::WARNING;
  log << "Using private mirror reflectivities" << endreq;
  log << "No treatment of scattering as yet" << endreq;
  //log << "Harding geom eff to 0.73 until HPD problems are sorted" << endreq;

  return sc;
}

// Method that handles various Gaudi "software events"
void RichRecSegmentTool::handle ( const Incident& incident ) {

  MsgStream log( msgSvc(), name() );
  log << MSG::VERBOSE << "Informed of incident '" << incident.type()
      << "' generated by '" << incident.source() << "'" << endreq;

  if ( "BeginEvent" == incident.type() ) {

    SmartDataPtr<RichRecSegments> tdsSegments( m_evtDataSvc,
                                               m_richRecSegmentLocation );
    if ( !tdsSegments ) {

      // Reinitialise the Photon Container
      m_segments = new RichRecSegments();

      // Register new RichRecPhoton container to Gaudi data store
      if (!m_evtDataSvc->registerObject(m_richRecSegmentLocation, m_segments)) {
        log << MSG::ERROR << "Failed to register RichRecSegments at "
            << m_richRecSegmentLocation << endreq;
      }

    } else {

      log << MSG::DEBUG
          << "Located " << tdsSegments->size() << " RichRecSegments at "
          << m_richRecSegmentLocation << endreq;

      // Set smartref to TES segment container
      m_segments = tdsSegments;

    }

  } else if ( "EndEvent" == incident.type() ) {

    log << MSG::DEBUG << "Finalised with " << m_segments->size()
        << " RichRecSegments at " << m_richRecSegmentLocation << endreq;

  }

}

// Forms a new RichRecSegment object
void RichRecSegmentTool::saveSegment( RichRecSegment * segment ) {
  
  // Give to container
  m_segments->insert( segment );
}

double
RichRecSegmentTool::signalPhotons ( RichRecSegment * segment,
                                    const Rich::ParticleIDType& id ) {

  double signal = segment->signalPhotons( id );
  if ( signal < -0.5 ) {

    // obtain number of emitted photons
    double emittedPhots = emittedPhotons( segment, id );

    // Obtain geometrical efficiency
    double geomEff =
      ( emittedPhots < 0.001 ? 0.0 : geomEfficiency( segment, id ) );

    signal = emittedPhots*geomEff;
    segment->setSignalPhotons( id, signal );
  }

  return signal;
}

double
RichRecSegmentTool::scatteredPhotons ( RichRecSegment * segment,
                                       const Rich::ParticleIDType& id ) {
  segment->setScatteredPhotons( id, 0.0 );
  return 0.0;
}

double RichRecSegmentTool::emittedPhotons ( RichRecSegment * segment,
                                            const Rich::ParticleIDType& id ) {

  double signal = segment->emittedPhotons( id );
  if ( signal < -0.5 ) {

    // if (m_timing) m_chrono->chronoStart("RichRecSegmentTool:emittedPhotons");

    // Some parameters of the segment
    double beta = this->beta( segment, id );
    double length = segment->trackSegment().pathLength();
    Rich::RadiatorType radiator = segment->trackSegment().radiator();

    // Make working copy of RichPhotonSpectrum
    RichPhotonSpectrum spectrum;
    m_richDetInterface->photonSpectrum( radiator, spectrum );

    // Scale by mirror reflectivity
    // Private implementation that should be moved elsewhere
    spectrum *= m_detReflectorEff[ segment->trackSegment().rich() ];

    // Loop over bins to apply number of photons scaling
    int nBins = spectrum.energyBins();
    double delta = spectrum.deltaEnergy();
    double energy = spectrum.lowerEnergy() - 0.5*delta;
    for ( int i = 0; i<nBins; i++ ) {
      energy += delta;

      // Refractive index
      double index = m_richDetInterface->refractiveIndex( radiator, energy );
      if ( beta*index <= 1.0 ) { spectrum[i] = 0.0; continue; }

      // Calculate number of photons for this energy bin
      // This has magic numbers which need to be reviewed
      // Note distances are in mm here
      double a = beta*beta*index*index;
      double numPhots = 0.0;
      if ( a > 0.0 ) numPhots = 37.0 * length * delta * (1.0 - 1.0/a);

      // Scale the spectrum
      spectrum[i] *= numPhots;

    }

    // Get total number of emitted photons from spectrum
    signal = spectrum.integral();
    segment->setEmittedPhotons( id, signal );

    // if (m_timing) m_chrono->chronoStop("RichRecSegmentTool:emittedPhotons");
  }

  return signal;
}

double
RichRecSegmentTool::geomEfficiency ( RichRecSegment * segment,
                                     const Rich::ParticleIDType& id ) {

  if ( Rich::Unknown == id || Rich::BelowThreshold == id ) {
    MsgStream log( msgSvc(), name() );
    log << MSG::WARNING << "Geomemtrical efficiency requested for segment "
        << segment->key() << " particle ID type " << id << " !!" << endreq;
    return 0.0;
  }

  double eff = segment->geomEfficiency( id );
  if ( eff < -0.5 ) {

    // if (m_timing) m_chrono->chronoStart("RichRecSegmentTool:geomEfficiency");

    // temporarily set geom eff to 0.73 until pixels are in correct places
    //segment->setGeomEfficiency( id, 0.73 );
    //return 0.73;

    RichTrackSegment & trackSeg = segment->trackSegment();

    // Define rotation matrix
    HepVector3D z = trackSeg.exitPoint() - trackSeg.entryPoint();
    z.setMag(1.);
    HepVector3D y = ( abs( z * HepVector3D(1.,0.,0.) ) < 1. ?
                      z.cross( HepVector3D(0.,1.,0.) ) :
                      z.cross( HepVector3D(1.,0.,0.) ) );
    y.setMag(1.);
    HepVector3D x = y.cross(z);
    HepRotation rotation = HepRotation();
    rotation.rotateAxes(x,y,z);

    // Cherenkov theta for this segment/hypothesis combination
    double ckTheta = this->avgCherenkovTheta( segment, id );

    double ckPhi = 0.0;
    int nDetect = 0;
    double sinCkTheta = sin(ckTheta);
    double cosCkTheta = cos(ckTheta);
    RichGeomPhoton photon;
    for ( int iPhot = 0; iPhot < m_nGeomEff; iPhot++, ckPhi += m_incPhi ) {

      HepVector3D photDir = rotation * HepVector3D( sinCkTheta*cos(ckPhi),
                                                    sinCkTheta*sin(ckPhi),
                                                    cosCkTheta );

      // Ray trace through detector
      if ( m_richDetInterface->traceToDetector( trackSeg.rich(),
                                                trackSeg.middlePoint(),
                                                photDir,
                                                photon ) != 0 ) {
        nDetect++;
        segment->addToGeomEfficiencyPerHPD( id,
                                            (int)(photon.smartID().hpdID()),
                                            m_hpdInc );

        // Need to find a better way of seting these variables
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
      if ( 0 == nDetect && iPhot >= m_ngeomEffBailout ) break;

    }

    eff = (double)nDetect/(double)m_nGeomEff;
    segment->setGeomEfficiency( id, eff );

    // if (m_timing) m_chrono->chronoStop("RichRecSegmentTool:geomEfficiency");
  }

  return eff;
}

double RichRecSegmentTool::avgCherenkovTheta( RichRecSegment * segment,
                                              const Rich::ParticleIDType& id ) {

  double angle = segment->averageCKTheta( id );
  if ( angle < -0.5 ) {

    // if (m_timing) m_chrono->chronoStart("RichRecSegmentTool:avgCherenkovTheta");

    double refIndex =
      m_richDetInterface->refractiveIndex( segment->trackSegment().radiator() );
    double beta = this->beta( segment, id );
    angle = ( refIndex*beta > 1. ? acos( 1./(beta*refIndex) ) : 0.0 );

    segment->setAverageCKTheta( id, angle );

    // if (m_timing) m_chrono->chronoStop("RichRecSegmentTool:avgCherenkovTheta");
  }

  return angle;
}

double RichRecSegmentTool::beta( RichRecSegment * segment,
                                 const Rich::ParticleIDType& id ) {

  double momentum = segment->trackSegment().entryMomentum().mag();
  double Esquare = momentum * momentum + 
    m_particleMass[(int)id] * m_particleMass[(int)id];

  return (Esquare > 0.0 ? momentum/sqrt(Esquare) : 0.0);
}

bool RichRecSegmentTool::hasRichInfo( RichRecSegment * segment ) {
  return ( geomEfficiency( segment, Rich::Electron ) > 0.0 );
}

RichRecSegments * RichRecSegmentTool::richSegments() {
  return m_segments;
}

double RichRecSegmentTool::allPhotons ( RichRecSegment * segment,
                                        const Rich::ParticleIDType& id ) {
  return signalPhotons( segment, id ) + scatteredPhotons( segment, id );
}

bool RichRecSegmentTool::aboveThreshold( RichRecSegment * segment,
                                         Rich::ParticleIDType& type ) {
  return segment->trackSegment().entryMomentum().mag() > 
    m_momThres[(int)segment->trackSegment().radiator()][(int)type];
}
