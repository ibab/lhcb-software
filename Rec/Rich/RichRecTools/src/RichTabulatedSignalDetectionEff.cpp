
/** @file RichTabulatedSignalDetectionEff.cpp
 *
 *  Implementation file for tool : RichTabulatedSignalDetectionEff
 *
 *  CVS Log :-
 *  $Id: RichTabulatedSignalDetectionEff.cpp,v 1.6 2004-07-27 20:15:33 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

// local
#include "RichTabulatedSignalDetectionEff.h"

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichTabulatedSignalDetectionEff>          s_factory ;
const        IToolFactory& RichTabulatedSignalDetectionEffFactory = s_factory ;

// Standard constructor
RichTabulatedSignalDetectionEff::
RichTabulatedSignalDetectionEff ( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : RichRecToolBase  ( type, name, parent ),
    m_flatMirReflLoc ( Rich::NRiches      ),
    m_sphMirReflLoc  ( Rich::NRiches      ),
    m_quartzWinEff   ( 0 ),
    m_pedLoss        ( 0 ),
    m_QE             ( 0 )
{

  declareInterface<IRichSignalDetectionEff>(this);

  // Define job option parameters

  // Spherical mirrors
  m_sphMirReflLoc[Rich::Rich1] =
    "/dd/Geometry/Rich1/Rich1SurfaceTabProperties/Rich1Mirror1SurfaceReflectivityPT";
  m_sphMirReflLoc[Rich::Rich2] =
    "/dd/Geometry/Rich1/Rich2SurfaceTabProperties/Rich1Mirror1SurfaceReflectivityPT";
  declareProperty( "RichSphMirrorLocations", m_sphMirReflLoc );

  // flat mirrors
  m_flatMirReflLoc[Rich::Rich1] =
    "/dd/Geometry/Rich1/Rich1SurfaceTabProperties/Rich1Mirror2SurfaceReflectivityPT";
  m_flatMirReflLoc[Rich::Rich2] =
    "/dd/Geometry/Rich1/Rich2SurfaceTabProperties/Rich1Mirror2SurfaceReflectivityPT";
  declareProperty( "RichFlatMirrorLocations", m_flatMirReflLoc );

  // Quantum efficiency
  declareProperty( "QETableLocation", m_qeTableLoc =
                   "/dd/Materials/RichMaterialTabProperties/HpdQuantumEff" );

}

StatusCode RichTabulatedSignalDetectionEff::initialize() {

  // Sets up various tools and services
  StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire QE Curve from XML
  SmartDataPtr<TabulatedProperty> tabQE( detSvc(), m_qeTableLoc );
  if ( !tabQE ) return Error( "Cannot retrieve QE from " + m_qeTableLoc );

  // Load tabulated reflectivities from XML
  SmartDataPtr<TabulatedProperty> tabF1( detSvc(), m_flatMirReflLoc[Rich::Rich1] );
  if ( !tabF1 ) return Error( "Failed to load "+m_flatMirReflLoc[Rich::Rich1] );
  SmartDataPtr<TabulatedProperty> tabF2( detSvc(), m_flatMirReflLoc[Rich::Rich2] );
  if ( !tabF2 ) return Error( "Failed to load "+m_flatMirReflLoc[Rich::Rich2] );
  SmartDataPtr<TabulatedProperty> tabS1( detSvc(), m_sphMirReflLoc[Rich::Rich1] );
  if ( !tabF1 ) return Error( "Failed to load "+m_sphMirReflLoc[Rich::Rich1] );
  SmartDataPtr<TabulatedProperty> tabS2( detSvc(), m_sphMirReflLoc[Rich::Rich2] );
  if ( !tabF2 ) return Error( "Failed to load "+m_sphMirReflLoc[Rich::Rich2] );

  // Create tabulated property objects from XML information
  m_QE                       = new Rich1DTabProperty( tabQE );
  m_flatMirRefl[Rich::Rich1] = new Rich1DTabProperty( tabF1 );
  m_flatMirRefl[Rich::Rich2] = new Rich1DTabProperty( tabF2 );
  m_sphMirRefl[Rich::Rich1]  = new Rich1DTabProperty( tabS1 );
  m_sphMirRefl[Rich::Rich2]  = new Rich1DTabProperty( tabS2 );

  // Get Rich1 Detector element
  DeRich1 * Rich1DE = getDet<DeRich1>( DeRichLocation::Rich1 );

  // Quartz window eff
  m_quartzWinEff = Rich1DE->userParameterAsDouble( "HPDQuartzWindowEff" );

  // Digitisation pedestal loss
  m_pedLoss = Rich1DE->userParameterAsDouble( "HPDPedestalDigiEff" );

  // Informational Printout
  debug() << " HPD quartz window efficiency = " << m_quartzWinEff << endreq
          << " Digitisation pedestal eff.   = " << m_pedLoss << endreq;

  return StatusCode::SUCCESS;
}

StatusCode RichTabulatedSignalDetectionEff::finalize()
{
  // Tidy up
  for ( MirrorReflectivities::iterator iF = m_flatMirRefl.begin();
        iF != m_flatMirRefl.end(); ++iF ) {
    if ( *iF ) { delete *iF; *iF = 0; }
  }
  for ( MirrorReflectivities::iterator iS = m_sphMirRefl.begin();
        iS != m_sphMirRefl.end(); ++iS ) {
    if ( *iS ) { delete *iS; *iS = 0; }
  }
  if ( m_QE ) { delete m_QE; m_QE = 0; }

  // Execute base class method
  return RichRecToolBase::finalize();
}

double
RichTabulatedSignalDetectionEff::photonDetEfficiency( RichRecSegment * segment,
                                                      const double energy ) const
{
  // which detector
  const Rich::DetectorType det = segment->trackSegment().rich();
  return ( (*m_QE)[energy*eV]/100 * m_quartzWinEff * m_pedLoss *
           (*m_flatMirRefl[det])[energy*eV] * (*m_sphMirRefl[det])[energy*eV] );
}
