
//-----------------------------------------------------------------------------
/** @file RichTabulatedSignalDetectionEff.cpp
 *
 *  Implementation file for tool : RichTabulatedSignalDetectionEff
 *
 *  CVS Log :-
 *  $Id: RichTabulatedSignalDetectionEff.cpp,v 1.11 2006-08-31 13:38:25 cattanem Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichTabulatedSignalDetectionEff.h"

// Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

// namespaces
using namespace LHCb;

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( RichTabulatedSignalDetectionEff );

// Standard constructor
RichTabulatedSignalDetectionEff::
RichTabulatedSignalDetectionEff ( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : RichRecToolBase  ( type, name, parent ),
    m_riches         ( Rich::NRiches ),
    m_quartzWinEff   ( 0 ),
    m_pedLoss        ( 0 )
{
  // interface
  declareInterface<IRichSignalDetectionEff>(this);
}

StatusCode RichTabulatedSignalDetectionEff::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Rich1 and Rich2
  m_riches[Rich::Rich1] = getDet<DeRich1>( DeRichLocation::Rich1 );
  m_riches[Rich::Rich2] = getDet<DeRich2>( DeRichLocation::Rich2 );

  // Quartz window eff
  m_quartzWinEff = m_riches[Rich::Rich1]->param<double>( "HPDQuartzWindowEff" );

  // Digitisation pedestal loss
  m_pedLoss =      m_riches[Rich::Rich1]->param<double>( "HPDPedestalDigiEff" );

  // Informational Printout
  debug() << " HPD quartz window efficiency = " << m_quartzWinEff << endreq
          << " Digitisation pedestal eff.   = " << m_pedLoss << endreq;

  return sc;
}

StatusCode RichTabulatedSignalDetectionEff::finalize()
{
  // Execute base class method
  return RichRecToolBase::finalize();
}

double
RichTabulatedSignalDetectionEff::photonDetEfficiency( RichRecSegment * segment,
                                                      const double energy ) const
{
  // which detector
  const Rich::DetectorType det = segment->trackSegment().rich();
  
  // Note - Only using nominal HPD from Rich1 until Rich2 also has this defined
  return ( (*(m_riches[Rich::Rich1]->nominalHPDQuantumEff()))[energy*Gaudi::Units::eV]/100
            * m_quartzWinEff * m_pedLoss *
           (*(m_riches[det]->nominalSecMirrorRefl()))[energy*Gaudi::Units::eV] *
           (*(m_riches[det]->nominalSphMirrorRefl()))[energy*Gaudi::Units::eV]
           );
}
