
//-----------------------------------------------------------------------------
/** @file RichTabulatedSignalDetectionEff.cpp
 *
 *  Implementation file for tool : Rich::Rec::TabulatedSignalDetectionEff
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichTabulatedSignalDetectionEff.h"

// Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//-----------------------------------------------------------------------------

// Standard constructor
TabulatedSignalDetectionEff::
TabulatedSignalDetectionEff ( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : ToolBase ( type, name, parent ),
    m_riches ( Rich::NRiches, nullptr )
{
  // interface
  declareInterface<ISignalDetectionEff>(this);
  //setProperty( "OutputLevel", 1 );
}

StatusCode TabulatedSignalDetectionEff::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = ToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Rich1 and Rich2
  m_riches[Rich::Rich1] = getDet<DeRich1>( DeRichLocations::Rich1 );
  m_riches[Rich::Rich2] = getDet<DeRich2>( DeRichLocations::Rich2 );

  // Do we have HPDs or PMTs
  const bool PmtActivate = m_riches[Rich::Rich1]->RichPhotoDetConfig() == Rich::PMTConfig;

  // PD panels
  m_pdPanels[Rich::Rich1][Rich::top]    = m_riches[Rich::Rich1]->pdPanel(Rich::top);
  m_pdPanels[Rich::Rich1][Rich::bottom] = m_riches[Rich::Rich1]->pdPanel(Rich::bottom);
  m_pdPanels[Rich::Rich2][Rich::left]   = m_riches[Rich::Rich2]->pdPanel(Rich::left);
  m_pdPanels[Rich::Rich2][Rich::right]  = m_riches[Rich::Rich2]->pdPanel(Rich::right);

  // Quartz window eff
  const double qEff = m_riches[Rich::Rich1]->param<double>( "HPDQuartzWindowEff" );

  // Digitisation pedestal loss
  const double pLos =
    ( PmtActivate && m_riches[Rich::Rich1]->exists("PMTPedestalDigiEff") ?
      m_riches[Rich::Rich1]->param<double>("PMTPedestalDigiEff") :
      m_riches[Rich::Rich1]->param<double>("HPDPedestalDigiEff") );

  // store cached value
  m_qEffPedLoss = qEff * pLos;

  // Setup incident services
  incSvc()->addListener( this, IncidentType::BeginEvent );

  // Informational Printout
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "PD quartz window eff.      = " << qEff << endmsg;
    debug() << "Digitisation pedestal eff. = " << pLos << endmsg;
  }

  // return
  return sc;
}

// Method that handles various Gaudi "software events"
void TabulatedSignalDetectionEff::handle ( const Incident& /* incident */ )
{
  // We only subscribe to one sort of incident, so no need to check type
  //if ( IncidentType::BeginEvent == incident.type() )
  m_lastRing = nullptr;
}

double
TabulatedSignalDetectionEff::photonDetEfficiency( LHCb::RichRecSegment * segment,
                                                  const Rich::ParticleIDType hypo,
                                                  const double energy ) const
{
  // protect against below threshold case
  if ( Rich::BelowThreshold == hypo ) return 0;

  _ri_debug << "Computing detection efficiency for " << segment 
            << " " << segment->key() 
            << " " << hypo << " photon energy=" << energy 
            << endmsg;

  // Load the CK ring
  const auto * ring = massHypoRingCreator()->massHypoRing( segment, hypo );
  if ( !ring ) { _ri_debug << " -> No Ring" << endmsg; return 0; }

  // If the ring is different to the cached one, update the count maps etc.
  if ( ring != m_lastRing )
  {
    _ri_debug << " -> Update caches" << endmsg;
    m_pdCount.clear();
    m_primMirrCount.clear();
    m_secMirrCount.clear();
    m_lastRing = ring;
    m_totalInPD = 0;
    for ( const auto& P : ring->ringPoints() )
    {
      if ( LHCb::RichRecPointOnRing::InHPDTube == P.acceptance() )
      {
        // Count accepted points
        ++m_totalInPD;
        // Count PDs hit by this ring
        ++m_pdCount [ P.smartID() ];
        // Count primary mirrors
        if ( P.primaryMirror()   ) { ++m_primMirrCount [ P.primaryMirror()   ]; }
        // Count secondary mirrors
        if ( P.secondaryMirror() ) { ++m_secMirrCount  [ P.secondaryMirror() ]; }
      }
    }
  }  

  _ri_debug << " -> Found " << m_totalInPD << " usable ring points out of "
            << ring->ringPoints().size() << endmsg;
  if ( 0 == m_totalInPD ) { return 0; }

  // Get weighted average PD Q.E.
  double pdQEEff(0);
  if ( !m_pdCount.empty() )
  {
    unsigned int totalInPD = 0;
    for ( const auto& PD : m_pdCount )
    {
      // Count HPDs
      totalInPD += PD.second;
      // get pointer to PD
      const DeRichPD * pd = dePD( PD.first );
      // add up Q.E. eff
      pdQEEff += (double)(PD.second) * (*(pd->pdQuantumEff()))[energy*Gaudi::Units::eV]/100 ;
    }
    // normalise the result
    pdQEEff /= (double)(totalInPD);
  }
  else
  {
    pdQEEff = 1;
    Warning( "No PDs found -> Assuming Av. PD Q.E. of 1", StatusCode::SUCCESS );
  }

  // Weighted primary mirror reflectivity
  double primMirrRefl(0);
  if ( !m_primMirrCount.empty() )
  {
    unsigned int totalInPD = 0;
    for ( const auto& PM : m_primMirrCount )
    {
      // count mirrors
      totalInPD += PM.second;
      // add up mirror refl.
      primMirrRefl +=
        (double)(PM.second) * (*(PM.first->reflectivity()))[energy*Gaudi::Units::eV];
    }
    // normalise the result
    primMirrRefl /= (double)(totalInPD);
  }
  else
  {
    primMirrRefl = 1;
    Warning( "No primary mirrors found -> Assuming Av. reflectivity of 1", 
             StatusCode::SUCCESS ).ignore();
  }

  // Weighted secondary mirror reflectivity
  double secMirrRefl(0);
  if ( !m_secMirrCount.empty() )
  {
    unsigned int totalInPD = 0;
    for ( const auto& SM : m_secMirrCount )
    {
      // count mirrors
      totalInPD += SM.second;
      // add up mirror refl.
      secMirrRefl +=
        (double)(SM.second) * (*(SM.first->reflectivity()))[energy*Gaudi::Units::eV];
    }
    // normalise the result
    secMirrRefl /= (double)(totalInPD);
  }
  else
  {
    secMirrRefl = 1;
    Warning( "No secondary mirrors found -> Assuming Av. reflectivity of 1", 
             StatusCode::SUCCESS );
  }

  _ri_debug << " -> Av. PD Q.E. = " << pdQEEff 
            << " Prim. Mirr Refl. = " << primMirrRefl
            << " Sec. Mirr. Refl. " << secMirrRefl << endmsg;

  // return overall efficiency
  return m_qEffPedLoss * pdQEEff * primMirrRefl * secMirrRefl;
}

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( TabulatedSignalDetectionEff )

//-----------------------------------------------------------------------------
