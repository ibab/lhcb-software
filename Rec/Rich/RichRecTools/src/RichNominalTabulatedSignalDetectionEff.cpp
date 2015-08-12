//-----------------------------------------------------------------------------
/** @file RichNominalTabulatedSignalDetectionEff.cpp
 *
 *  Implementation file for tool : Rich::Rec::NominalTabulatedSignalDetectionEff
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichNominalTabulatedSignalDetectionEff.h"

// Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( NominalTabulatedSignalDetectionEff )

// Standard constructor
NominalTabulatedSignalDetectionEff::
NominalTabulatedSignalDetectionEff ( const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent )
: ToolBase       ( type, name, parent ),
  m_riches       ( Rich::NRiches ),
  m_qEffPedLoss  ( 0 )
{
  // interface
  declareInterface<ISignalDetectionEff>(this);
}

StatusCode NominalTabulatedSignalDetectionEff::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = ToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Rich1 and Rich2
  m_riches[Rich::Rich1] = getDet<DeRich1>( DeRichLocations::Rich1 );
  m_riches[Rich::Rich2] = getDet<DeRich2>( DeRichLocations::Rich2 );

  // Do we have HPDs or PMTs
  const bool PmtActivate = m_riches[Rich::Rich1]->RichPhotoDetConfig() == Rich::PMTConfig;

  // Quartz window eff
  const double qEff = m_riches[Rich::Rich1]->param<double>( "HPDQuartzWindowEff" );

  // Digitisation pedestal loss
  const double pLos = 
    ( PmtActivate && m_riches[Rich::Rich1]->exists("PMTPedestalDigiEff") ? 
      m_riches[Rich::Rich1]->param<double>("PMTPedestalDigiEff") :  
      m_riches[Rich::Rich1]->param<double>("HPDPedestalDigiEff") );
  m_qEffPedLoss = qEff * pLos;

  // Informational Printout
  if ( msgLevel(MSG::DEBUG) )
    debug() << " HPD quartz window efficiency = " << qEff 
            << " Digitisation pedestal eff.   = " << pLos << endmsg;
  
  return sc;
}

double
NominalTabulatedSignalDetectionEff::photonDetEfficiency( LHCb::RichRecSegment * segment,
                                                         const Rich::ParticleIDType /* hypo */,
                                                         const double energy ) const
{
  // which detector
  const Rich::DetectorType det = segment->trackSegment().rich();

  // Note - Only using nominal HPD from Rich1 until Rich2 also has this defined
  return ( (*(m_riches[Rich::Rich1]->nominalPDQuantumEff()))[energy*Gaudi::Units::eV]/100
           * m_qEffPedLoss *
           (*(m_riches[det]->nominalSecMirrorRefl()))[energy*Gaudi::Units::eV] *
           (*(m_riches[det]->nominalSphMirrorRefl()))[energy*Gaudi::Units::eV]
           );
}
