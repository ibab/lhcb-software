// $Id: RichTabulatedHPDSignalDetectionEff.cpp,v 1.1 2003-08-26 14:40:21 jonrob Exp $

// local
#include "RichTabulatedHPDSignalDetectionEff.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichTabulatedHPDSignalDetectionEff
//
// 15/03/2002 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichTabulatedHPDSignalDetectionEff>          s_factory ;
const        IToolFactory& RichTabulatedHPDSignalDetectionEffFactory = s_factory ;

// Standard constructor
RichTabulatedHPDSignalDetectionEff::RichTabulatedHPDSignalDetectionEff ( const std::string& type,
                                                                         const std::string& name,
                                                                         const IInterface* parent )
  : RichRecToolBase  ( type, name, parent ),
    m_flatMirReflLoc ( Rich::NRiches      ),
    m_sphMirReflLoc  ( Rich::NRiches      ) {

  declareInterface<IRichSignalDetectionEff>(this);

  // Define job option parameters

  // Spherical mirrors
  m_sphMirReflLoc[Rich::Rich1] = "/dd/Geometry/Rich1/Rich1SurfaceTabProperties/Rich1Mirror1SurfaceReflectivityPT";
  m_sphMirReflLoc[Rich::Rich2] = "/dd/Geometry/Rich1/Rich2SurfaceTabProperties/Rich1Mirror1SurfaceReflectivityPT";
  declareProperty( "RichSphMirrorLocations", m_sphMirReflLoc );

  // flat mirrors
  m_flatMirReflLoc[Rich::Rich1] = "/dd/Geometry/Rich1/Rich1SurfaceTabProperties/Rich1Mirror2SurfaceReflectivityPT";
  m_flatMirReflLoc[Rich::Rich2] = "/dd/Geometry/Rich1/Rich2SurfaceTabProperties/Rich1Mirror2SurfaceReflectivityPT";
  declareProperty( "RichFlatMirrorLocations", m_flatMirReflLoc );

  // temporary parameters to take into acount degraded performance for robustness tests
  declareProperty( "ScalePhotonEff", m_photonEffScale = 1 );

}

StatusCode RichTabulatedHPDSignalDetectionEff::initialize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Initialize" << endreq;

  // Sets up various tools and services
  if ( !RichRecToolBase::initialize() ) return StatusCode::FAILURE;

  // Acquire instances of tools
  acquireTool("RichDetInterface", m_richDetInt);

  // Get the Quantum Eff
  // For time being assume only one reference curve for all HPDs
  m_referenceQE = m_richDetInt->hpdQuantumEff();
  if ( !m_referenceQE ) {
    msg << MSG::ERROR << "Failed to acquire QE function" << endreq;
    return StatusCode::FAILURE;
  }

  releaseTool( m_richDetInt );

  // Load tabulated reflectivities from XML
  SmartDataPtr<TabulatedProperty> tabF1( detSvc(), m_flatMirReflLoc[Rich::Rich1] );
  if ( !tabF1 ) {
    msg << MSG::ERROR << "Failed to load reflectivity from "
        << m_flatMirReflLoc[Rich::Rich1] << endreq;
    return StatusCode::FAILURE;
  }
  SmartDataPtr<TabulatedProperty> tabF2( detSvc(), m_flatMirReflLoc[Rich::Rich2] );
  if ( !tabF2 ) {
    msg << MSG::ERROR << "Failed to load reflectivity from "
        << m_flatMirReflLoc[Rich::Rich2] << endreq;
    return StatusCode::FAILURE;
  }
  SmartDataPtr<TabulatedProperty> tabS1( detSvc(), m_sphMirReflLoc[Rich::Rich1] );
  if ( !tabF1 ) {
    msg << MSG::ERROR << "Failed to load reflectivity from "
        << m_sphMirReflLoc[Rich::Rich1] << endreq;
    return StatusCode::FAILURE;
  }
  SmartDataPtr<TabulatedProperty> tabS2( detSvc(), m_sphMirReflLoc[Rich::Rich2] );
  if ( !tabF2 ) {
    msg << MSG::ERROR << "Failed to load reflectivity from "
        << m_sphMirReflLoc[Rich::Rich2] << endreq;
    return StatusCode::FAILURE;
  }

  // Create tabulated property objects from XML information
  m_flatMirRefl[Rich::Rich1] = new Rich1DTabProperty( tabF1 );
  m_flatMirRefl[Rich::Rich2] = new Rich1DTabProperty( tabF2 );
  m_sphMirRefl[Rich::Rich1]  = new Rich1DTabProperty( tabS1 );
  m_sphMirRefl[Rich::Rich2]  = new Rich1DTabProperty( tabS2 );

  // Get Rich1 Detector element
  SmartDataPtr<IDetectorElement> Rich1DE( detSvc(), "/dd/Structure/LHCb/Rich1" );

  // Quartz window eff
  m_quartzWinEff = Rich1DE->userParameterAsDouble("HPDQuartzWindowEff");
  // Digitisation pedestal loss
  m_pedLoss = Rich1DE->userParameterAsDouble("HPDPedestalDigiEff");

  // Informational Printout
  msg << MSG::DEBUG
      << " Using XML tabulated implementation" << endreq
      << " Rich1 Sph. Mirror refl.      = " << m_sphMirReflLoc[Rich::Rich1] << endreq
      << " Rich2 Sph. Mirror refl.      = " << m_sphMirReflLoc[Rich::Rich2] << endreq
      << " Rich1 flat Mirror refl.      = " << m_flatMirReflLoc[Rich::Rich1] << endreq
      << " Rich2 flat Mirror refl.      = " << m_flatMirReflLoc[Rich::Rich2] << endreq
      << " HPD quartz window efficiency = " << m_quartzWinEff << endreq
      << " Digitisation pedestal eff.   = " << m_pedLoss << endreq;

  return StatusCode::SUCCESS;
}

StatusCode RichTabulatedHPDSignalDetectionEff::finalize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Finalize" << endreq;

  // Delete tabulated properties
  for ( MirrorReflectivities::iterator iF = m_flatMirRefl.begin();
        iF != m_flatMirRefl.end(); ++iF ) {
    if ( *iF ) { delete *iF; *iF = 0; }
  }
  for ( MirrorReflectivities::iterator iS = m_sphMirRefl.begin();
        iS != m_sphMirRefl.end(); ++iS ) {
    if ( *iS ) { delete *iS; *iS = 0; }
  }

  // Execute base class method
  return RichRecToolBase::finalize();
}

double
RichTabulatedHPDSignalDetectionEff::photonDetEfficiency( RichRecSegment * segment,
                                                         double energy )
{

  // which detector
  Rich::DetectorType det = segment->trackSegment().rich();

  return (*m_referenceQE)[energy*eV]/100 *
    m_quartzWinEff * m_pedLoss * m_photonEffScale *
    (*m_flatMirRefl[det])[energy*eV] * (*m_sphMirRefl[det])[energy*eV];
}
