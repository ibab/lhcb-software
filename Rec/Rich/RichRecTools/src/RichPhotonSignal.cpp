// $Id: RichPhotonSignal.cpp,v 1.6 2003-08-26 14:40:19 jonrob Exp $

// local
#include "RichPhotonSignal.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichPhotonSignal
//
// 15/03/2002 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichPhotonSignal>          s_factory ;
const        IToolFactory& RichPhotonSignalFactory = s_factory ;

// Standard constructor
RichPhotonSignal::RichPhotonSignal( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : RichRecToolBase( type, name, parent ) {

  declareInterface<IRichPhotonSignal>(this);

}

StatusCode RichPhotonSignal::initialize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Initialize" << endreq;

  // Sets up various tools and services
  if ( !RichRecToolBase::initialize() ) return StatusCode::FAILURE;

  // Acquire instances of tools
  acquireTool( "RichExpectedTrackSignal", m_signal  );
  acquireTool( "RichCherenkovAngle",      m_ckAngle );
  acquireTool( "RichCherenkovResolution", m_ckRes   );

  // Get Rich Detector elements
  SmartDataPtr<IDetectorElement> Rich1DE( detSvc(), "/dd/Structure/LHCb/Rich1" );
  SmartDataPtr<IDetectorElement> Rich2DE( detSvc(), "/dd/Structure/LHCb/Rich2" );

  // Detector parameters
  // Rich1 radius of curvature in mm
  m_radiusCurv[Rich::Rich1] = Rich1DE->userParameterAsDouble("Rich1Mirror1RadiusQ1");
  // Rich2 radius of curvature in mm
  m_radiusCurv[Rich::Rich2] = Rich2DE->userParameterAsDouble("Rich2SphMirrorRadius");
  // area of pixel in mm^2
  double xSize = Rich1DE->userParameterAsDouble("RichHpdPixelXsize");
  double ySize = Rich1DE->userParameterAsDouble("RichHpdPixelYsize");
  double demagScale = Rich1DE->userParameterAsDouble("HPDDemagScaleFactor");
  m_pixelArea = demagScale*xSize * demagScale*ySize;
  
  // Informational Printout
  msg << MSG::DEBUG
      << " Mirror radii of curvature    = "
      << m_radiusCurv[Rich::Rich1] << " " << m_radiusCurv[Rich::Rich2] << endreq
      << " Pixel area                   = " << m_pixelArea << endreq;

  return StatusCode::SUCCESS;
}

StatusCode RichPhotonSignal::finalize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Finalize" << endreq;

  // release tools
  releaseTool( m_signal  );
  releaseTool( m_ckAngle );
  releaseTool( m_ckRes   );

  // Execute base class method
  return RichRecToolBase::finalize();
}

double RichPhotonSignal::predictedPixelSignal( RichRecPhoton * photon,
                                               const Rich::ParticleIDType id ) {

  if ( !photon->expPixelSignalPhots().dataIsValid(id) ) {

    // Which detector
    Rich::DetectorType det = photon->richRecSegment()->trackSegment().rich();

    // Reconstructed Cherenkov theta angle
    double thetaReco = photon->geomPhoton().CherenkovTheta();

    // Compute the expected pixel contribution
    double pixelSignal = photon->geomPhoton().activeSegmentFraction() *
      ( ( signalProb(photon, id) *
          m_signal->nSignalPhotons(photon->richRecSegment(),id) ) +
        ( scatterProb(photon, id) *
          m_signal->nScatteredPhotons(photon->richRecSegment(),id) ) ) *
      4.0 * m_pixelArea / ( m_radiusCurv[det] * m_radiusCurv[det] *
                            (thetaReco>1e-10 ? thetaReco : 1e-10) );

    photon->setExpPixelSignalPhots( id, pixelSignal );

  }

  return photon->expPixelSignalPhots( id );
}

double RichPhotonSignal::signalProb( RichRecPhoton * photon,
                                     const Rich::ParticleIDType id ) {

  // Expected Cherenkov theta angle
  double thetaExp = m_ckAngle->avgCherenkovTheta( photon->richRecSegment(), id );
  if ( thetaExp < 0.000001 ) return 0.0;

  // Reconstructed Cherenkov theta angle
  double thetaReco = photon->geomPhoton().CherenkovTheta();

  // Expected Cherenkov theta angle resolution
  double thetaExpRes = m_ckRes->ckThetaResolution(photon->richRecSegment(),id);

  // The difference between reco and expected
  double thetaDiff = thetaReco-thetaExp;
  if ( fabs(thetaDiff) > 30.0*thetaExpRes ) return 0;

  // return the probability
  double expArg = 0.5*thetaDiff*thetaDiff/(thetaExpRes*thetaExpRes);
  return ( exp( -(expArg>650 ? 650 : expArg) ) /
           ( sqrt(2.*M_PI)*2.*M_PI*thetaExpRes ) );
}

double RichPhotonSignal::scatterProb( RichRecPhoton * photon,
                                      const Rich::ParticleIDType id ) {

  Rich::RadiatorType rad = photon->richRecSegment()->trackSegment().radiator();

  if ( rad == Rich::Aerogel ) {

    // Expected Cherenkov theta angle
    double thetaExp = m_ckAngle->avgCherenkovTheta( photon->richRecSegment(), id );
    if ( thetaExp < 0.000001 ) return 0.0;

    // Reconstructed Cherenkov theta angle
    double thetaRec = photon->geomPhoton().CherenkovTheta();

    // Compute the scattering
    double fbkg = 0.0;
    if ( thetaRec < thetaExp ) {
      fbkg = ( exp(17.0*thetaRec) - 1.0 ) / ( exp(17.0*thetaExp) - 1.0 );
    } else if ( thetaRec < 0.5*M_PI + thetaExp - 0.04 ) {
      fbkg = cos( thetaRec - thetaExp + 0.04 );
      fbkg = fbkg*fbkg/0.9984;
    } else {
      return 0.0;
    }

    // return the probability
    return 2.0 * fbkg / (M_PI*M_PI);
  }

  return 0.;
}
