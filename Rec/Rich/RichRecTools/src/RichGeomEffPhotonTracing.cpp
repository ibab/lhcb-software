// $Id: RichGeomEffPhotonTracing.cpp,v 1.6 2004-07-02 14:30:31 jonrob Exp $

// local
#include "RichGeomEffPhotonTracing.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichGeomEffPhotonTracing
//
// 15/03/2002 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichGeomEffPhotonTracing>          s_factory ;
const        IToolFactory& RichGeomEffPhotonTracingFactory = s_factory ;

// Standard constructor
RichGeomEffPhotonTracing::RichGeomEffPhotonTracing ( const std::string& type,
                                                     const std::string& name,
                                                     const IInterface* parent )
  : RichRecToolBase( type, name, parent ),
    m_rayTrace ( 0 ),
    m_ckAngle  ( 0 ) {

  declareInterface<IRichGeomEff>(this);

  // Define job option parameters
  declareProperty( "NPhotonsGeomEffCalc",    m_nGeomEff        = 100 );
  declareProperty( "NPhotonsGeomEffBailout", m_nGeomEffBailout = 20  );

  // randomn number distribution
  Rndm::Numbers m_uniDist;

}

StatusCode RichGeomEffPhotonTracing::initialize() 
{

  // Sets up various tools and services
  const StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRayTracing",     m_rayTrace );
  acquireTool( "RichCherenkovAngle", m_ckAngle  );

  // randomn number service
  IRndmGenSvc * randSvc = svc<IRndmGenSvc>( "RndmGenSvc", true );
  if ( !m_uniDist.initialize( randSvc, Rndm::Flat(0,1) ) ) {
    return Error( "Unable to initialise randomn numbers" );
  }

  // Set up cached parameters for geometrical efficiency calculation
  m_pdInc = 1.0 / ( static_cast<double>(m_nGeomEff) );
  m_incPhi = M_PI/2.0 + M_2PI/( static_cast<double>(m_nGeomEff) );
  double ckPhi = 0.0;
  m_sinCkPhi.clear();
  m_cosCkPhi.clear();
  for ( int iPhot = 0; iPhot < m_nGeomEff; ++iPhot, ckPhi += m_incPhi ) {
    m_sinCkPhi.push_back( sin(ckPhi) );
    m_cosCkPhi.push_back( cos(ckPhi) );
  }

  // Configure the ray-tracing mode
  m_traceMode.setDetPrecision      ( RichTraceMode::circle );
  m_traceMode.setDetPlaneBound     ( RichTraceMode::loose  );
  m_traceMode.setForcedSide        ( false                 );
  m_traceMode.setOutMirrorBoundary ( false                 );
  m_traceMode.setMirrorSegBoundary ( false                 );

  return StatusCode::SUCCESS;
}

StatusCode RichGeomEffPhotonTracing::finalize() 
{
  // Release things
  m_uniDist.finalize();

  // Execute base class method
  return RichRecToolBase::finalize();
}

double
RichGeomEffPhotonTracing::geomEfficiency ( RichRecSegment * segment,
                                           const Rich::ParticleIDType id ) const {

  if ( !segment->geomEfficiency().dataIsValid(id) ) {
    double eff = 0;

    // Cherenkov theta for this segment/hypothesis combination
    const double ckTheta = m_ckAngle->avgCherenkovTheta( segment, id );
    if ( ckTheta > 0 ) {

      const RichTrackSegment & trackSeg = segment->trackSegment();

      // Define rotation matrix
      const HepVector3D z = trackSeg.bestMomentum().unit();
      HepVector3D y = ( fabs( z * HepVector3D(1.,0.,0.) ) < 1. ?
                        z.cross( HepVector3D(0.,1.,0.) ) :
                        z.cross( HepVector3D(1.,0.,0.) ) );
      y.setMag(1);
      const HepVector3D x = y.cross(z);
      HepRotation rotation = HepRotation();
      rotation.rotateAxes(x,y,z);

      int nDetect = 0;
      const double sinCkTheta = sin(ckTheta);
      const double cosCkTheta = cos(ckTheta);
      AngleVector::const_iterator iCos = m_cosCkPhi.begin();
      AngleVector::const_iterator iSin = m_sinCkPhi.begin();
      for ( int iPhot = 0; iPhot < m_nGeomEff; ++iPhot, ++iCos, ++iSin ) {

        // Photon emission point is random between segment start and end points
        //const HepPoint3D emissionPt = trackSeg.bestPoint( m_uniDist() );
        // Photon emission point is half-way between segment start and end points
        const HepPoint3D & emissionPt = trackSeg.bestPoint();

        // Photon direction around loop
        const HepVector3D photDir = rotation * HepVector3D( sinCkTheta*(*iCos),
                                                            sinCkTheta*(*iSin),
                                                            cosCkTheta );

        // Ray trace through detector, using fast circle modelling of HPDs
        RichGeomPhoton photon;
        if ( 0 != m_rayTrace->traceToDetector( trackSeg.rich(),
                                               emissionPt,
                                               photDir,
                                               photon,
                                               m_traceMode ) ) {

          ++nDetect;
          segment->addToGeomEfficiencyPerPD( id,
                                             photon.smartID().pdID(),
                                             m_pdInc );
          if ( photon.detectionPoint().x() > 0 ) {
            segment->setPhotonsInXPlus(true);
          } else {
            segment->setPhotonsInXMinus(true);
          }
          if ( photon.detectionPoint().y() > 0 ) {
            segment->setPhotonsInYPlus(true);
          } else {
            segment->setPhotonsInYMinus(true);
          }

        }

        // Bail out if tried m_geomEffBailout times and all have failed
        if ( 0 == nDetect && iPhot >= m_nGeomEffBailout ) break;

      } // fake photon loop

      eff = static_cast<double>(nDetect)/static_cast<double>(m_nGeomEff);

    } // CK theta IF

    segment->setGeomEfficiency( id, eff );
  }

  return segment->geomEfficiency( id );
}

double
RichGeomEffPhotonTracing::geomEfficiencyScat ( RichRecSegment * segment,
                                               const Rich::ParticleIDType id ) const {

  if ( !segment->geomEfficiencyScat().dataIsValid(id) ) {
    double eff = 0;

    // only for aerogel
    const double ckTheta = m_ckAngle->avgCherenkovTheta( segment, id );
    if ( ckTheta > 0 && segment->trackSegment().radiator() == Rich::Aerogel ) {

      const RichTrackSegment & trackSeg = segment->trackSegment();

      // Photon emission point is end of aerogel
      const HepPoint3D emissionPt = trackSeg.exitPoint();

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
      AngleVector::const_iterator iCos = m_cosCkPhi.begin();
      AngleVector::const_iterator iSin = m_sinCkPhi.begin();
      for ( int iPhot = 0; iPhot < m_nGeomEff; ++iPhot , ++iCos, ++iSin ) {

        // generate randomn cos(theta)**2 distribution for thetaCk
        double ckTheta;
        do {
          ckTheta = m_uniDist()*M_PI_2;
        } while ( m_uniDist() > gsl_pow_2(cos(ckTheta)) );

        // Photon direction around loop
        const HepVector3D photDir = rotation*HepVector3D( sin(ckTheta)*(*iCos),
                                                          sin(ckTheta)*(*iSin),
                                                          cos(ckTheta) );

        // Ray trace through detector
        if ( 0 != m_rayTrace->traceToDetector( trackSeg.rich(),
                                               emissionPt,
                                               photDir,
                                               photon,
                                               m_traceMode ) ) { ++nDetect; }

        // Bail out if tried m_geomEffBailout times and all have failed
        if ( 0 == nDetect && iPhot >= m_nGeomEffBailout ) break;

      } // fake photon loop

      eff = static_cast<double>(nDetect)/static_cast<double>(m_nGeomEff);

    } // radiator

    // Assign this efficiency to all hypotheses
    segment->setGeomEfficiencyScat( Rich::Electron, eff );
    segment->setGeomEfficiencyScat( Rich::Muon, eff );
    segment->setGeomEfficiencyScat( Rich::Pion, eff );
    segment->setGeomEfficiencyScat( Rich::Kaon, eff );
    segment->setGeomEfficiencyScat( Rich::Proton, eff );

  }

  return segment->geomEfficiencyScat( id );
}
