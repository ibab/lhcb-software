
/** @file RichTabulatedSignalDetectionEff.cpp
 *
 *  Implementation file for tool : RichTabulatedSignalDetectionEff
 *
 *  CVS Log :-
 *  $Id: RichTabulatedSignalDetectionEff.cpp,v 1.7 2004-10-27 14:39:41 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.6  2004/07/27 20:15:33  jonrob
 *  Add doxygen file documentation and CVS information
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
    m_quartzWinEff   ( 0 ),
    m_pedLoss        ( 0 ),
    m_QE             ( 0 )
{

  // interface
  declareInterface<IRichSignalDetectionEff>(this);

}

StatusCode RichTabulatedSignalDetectionEff::initialize() 
{
  // Sets up various tools and services
  const StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Rich1 and Rich2 (should be const but "userParameterAsDouble" isn't const ...)
  DeRich * rich1 = getDet<DeRich>( DeRichLocation::Rich1 );
  DeRich * rich2 = getDet<DeRich>( DeRichLocation::Rich2 );

  // QE Curve
  m_QE = new Rich1DTabProperty( rich1->nominalHPDQuantumEff() );

  // mirror reflectivities
  m_flatMirRefl[Rich::Rich1] = new Rich1DTabProperty( rich1->nominalFlatMirrorRefl() );
  m_flatMirRefl[Rich::Rich2] = new Rich1DTabProperty( rich2->nominalFlatMirrorRefl() );
  m_sphMirRefl[Rich::Rich1]  = new Rich1DTabProperty( rich1->nominalSphMirrorRefl()  );
  m_sphMirRefl[Rich::Rich2]  = new Rich1DTabProperty( rich2->nominalSphMirrorRefl()  );

  // Quartz window eff
  m_quartzWinEff = rich1->userParameterAsDouble( "HPDQuartzWindowEff" );

  // Digitisation pedestal loss
  m_pedLoss = rich1->userParameterAsDouble( "HPDPedestalDigiEff" );

  // Informational Printout
  debug() << " HPD quartz window efficiency = " << m_quartzWinEff << endreq
          << " Digitisation pedestal eff.   = " << m_pedLoss << endreq;

  return StatusCode::SUCCESS;
}

StatusCode RichTabulatedSignalDetectionEff::finalize()
{
  // Tidy up
  for ( TabProps::iterator iF = m_flatMirRefl.begin(); iF != m_flatMirRefl.end(); ++iF ) {
    if ( *iF ) { delete *iF; *iF = 0; }
  }
  for ( TabProps::iterator iS = m_sphMirRefl.begin();  iS != m_sphMirRefl.end();  ++iS ) {
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
