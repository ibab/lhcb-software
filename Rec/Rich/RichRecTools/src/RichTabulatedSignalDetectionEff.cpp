
//-----------------------------------------------------------------------------
/** @file RichTabulatedSignalDetectionEff.cpp
 *
 *  Implementation file for tool : Rich::Rec::TabulatedSignalDetectionEff
 *
 *  CVS Log :-
 *  $Id: RichTabulatedSignalDetectionEff.cpp,v 1.17 2008-04-16 17:42:02 jonrob Exp $
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

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( TabulatedSignalDetectionEff );

// Standard constructor
TabulatedSignalDetectionEff::
TabulatedSignalDetectionEff ( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : RichRecToolBase  ( type, name, parent ),
    m_coneTrace      ( NULL ),
    m_ckAngle        ( NULL ),
    m_riches         ( Rich::NRiches ),
    m_qEffPedLoss    ( 0 ),
    m_traceModeRad   ( Rich::NRadiatorTypes ),
    m_nPoints        ( Rich::NRadiatorTypes, 50 ),
    m_last_segment   ( NULL ),
    m_last_ring      ( NULL ),
    m_last_hypo      ( Rich::Unknown )
{
  // interface
  declareInterface<ISignalDetectionEff>(this);
  // JOs
  declareProperty( "UseDetailedHPDsInRayTracing", m_useDetailedHPDsForRayT = false );
  declareProperty( "RadiatorRingPoints", m_nPoints );
}

StatusCode TabulatedSignalDetectionEff::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = Rich::Rec::ToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // get tools
  acquireTool( "RichRayTraceCKCone", m_coneTrace );
  acquireTool( "RichCherenkovAngle", m_ckAngle   );

  // Rich1 and Rich2
  m_riches[Rich::Rich1] = getDet<DeRich1>( DeRichLocation::Rich1 );
  m_riches[Rich::Rich2] = getDet<DeRich2>( DeRichLocation::Rich2 );

  // HPD panels
  m_hpdPanels[Rich::Rich1][Rich::top]    = getDet<DeRichHPDPanel>(pdPanelName(Rich::Rich1,Rich::top));
  m_hpdPanels[Rich::Rich1][Rich::bottom] = getDet<DeRichHPDPanel>(pdPanelName(Rich::Rich1,Rich::bottom));
  m_hpdPanels[Rich::Rich2][Rich::left]   = getDet<DeRichHPDPanel>(pdPanelName(Rich::Rich2,Rich::left));
  m_hpdPanels[Rich::Rich2][Rich::right]  = getDet<DeRichHPDPanel>(pdPanelName(Rich::Rich2,Rich::right));

  // Quartz window eff
  const double qEff = m_riches[Rich::Rich1]->param<double>( "HPDQuartzWindowEff" );

  // Digitisation pedestal loss
  const double pLos = m_riches[Rich::Rich1]->param<double>( "HPDPedestalDigiEff" );

  // store cached value
  m_qEffPedLoss = qEff * pLos;

  // the ray-tracing mode
  LHCb::RichTraceMode tmpMode ( LHCb::RichTraceMode::RespectHPDTubes,
                                ( m_useDetailedHPDsForRayT ?
                                  LHCb::RichTraceMode::FullHPDs :
                                  LHCb::RichTraceMode::SimpleHPDs ) );
  m_traceModeRad[Rich::Aerogel]  = tmpMode;
  m_traceModeRad[Rich::Aerogel].setAeroRefraction(true);
  m_traceModeRad[Rich::Rich1Gas] = tmpMode;
  m_traceModeRad[Rich::Rich2Gas] = tmpMode;
  debug() << "Aerogel  Track " << m_traceModeRad[Rich::Aerogel]  << endreq;
  debug() << "Rich1Gas Track " << m_traceModeRad[Rich::Rich1Gas] << endreq;
  debug() << "Rich2Gas Track " << m_traceModeRad[Rich::Rich2Gas] << endreq;

  // Informational Printout
  debug() << " HPD quartz window efficiency = " << qEff << endreq
          << " Digitisation pedestal eff.   = " << pLos << endreq;

  return sc;
}

const LHCb::RichRecRing *
TabulatedSignalDetectionEff::ckRing( LHCb::RichRecSegment * segment,
                                     const Rich::ParticleIDType hypo ) const
{
  // Make a CK ring for this segment and hypo. Used to work out which mirrors
  // and HPDs we need to sample the reflectivities and Q.E. values from

  LHCb::RichRecRing * newRing = NULL;

  // Cherenkov theta for this segment/hypothesis combination
  // using emitted photon spectra (to avoid a circular information dependency)
  const double ckTheta = m_ckAngle->avgCherenkovTheta( segment, hypo, true );
  debug() << " -> Making new CK ring : theta = " << ckTheta << endreq;
  if ( ckTheta > 0 )
  {
    // make a ring object
    newRing = new LHCb::RichRecRing( segment, ckTheta, hypo );

    // set ring type info
    newRing->setType ( LHCb::RichRecRing::RayTracedCK );

    // ray tracing
    const Rich::RadiatorType rad = segment->trackSegment().radiator();
    const StatusCode sc = m_coneTrace->rayTrace( newRing, m_nPoints[rad], m_traceModeRad[rad] );
    if ( sc.isFailure() )
    {
      Warning( "Some problem occured during CK cone ray-tracing" );
      delete newRing;
      newRing = NULL;
    }

  }

  // return the ring
  return newRing;
}

double
TabulatedSignalDetectionEff::photonDetEfficiency( LHCb::RichRecSegment * segment,
                                                  const Rich::ParticleIDType hypo,
                                                  const double energy ) const
{
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Computing detection efficiency for " << segment << " " << hypo
            << " photon energy=" << energy << endreq;

  // Get a (local) ring for this segment/hypo
  if ( segment != m_last_segment || m_last_hypo != hypo )
  {
    delete m_last_ring;
    m_last_segment = segment;
    m_last_hypo    = hypo;
    m_last_ring    = ckRing( segment, hypo );
  }
  if ( !m_last_ring ) { debug() << " -> No Ring" << endreq; return 0; }

  typedef Rich::Map<const LHCb::RichSmartID,unsigned int> HPDCount;
  typedef Rich::Map<const DeRichSphMirror *,unsigned int> MirrorCount;

  HPDCount hpdCount;
  MirrorCount primMirrCount, secMirrCount;
  unsigned int totalInHPD(0);
  for ( LHCb::RichRecPointOnRing::Vector::const_iterator iP = m_last_ring->ringPoints().begin();
        iP != m_last_ring->ringPoints().end(); ++iP )
  {
    if ( (*iP).acceptance() == LHCb::RichRecPointOnRing::InHPDTube )
    {
      // Count accepted points
      ++totalInHPD;
      // Count HPDs hit by this ring
      ++hpdCount [ (*iP).smartID() ];
      // Count primary mirrors
      if ( (*iP).primaryMirror()   ) { ++primMirrCount [ (*iP).primaryMirror()   ]; }
      // Count secondary mirrors
      if ( (*iP).secondaryMirror() ) { ++secMirrCount  [ (*iP).secondaryMirror() ]; }
    }
  }
  if ( msgLevel(MSG::DEBUG) )
    debug() << " -> Found " << totalInHPD << " usable ring points out of "
            << m_last_ring->ringPoints().size() << endreq;
  if ( 0 == totalInHPD ) { return 0; }

  // Get weighted average HPD Q.E.
  double hpdQEEff(0);
  if ( !hpdCount.empty() )
  {
    totalInHPD = 0;
    for ( HPDCount::const_iterator iHPD = hpdCount.begin();
          iHPD != hpdCount.end(); ++iHPD )
    {
      // Count HPDs
      totalInHPD += iHPD->second;
      // get pointer to HPD
      const DeRichHPD * hpd = deHPD( iHPD->first );
      // add up Q.E. eff
      hpdQEEff += (double)(iHPD->second) * (*(hpd->hpdQuantumEff()))[energy*Gaudi::Units::eV]/100 ;
    }
    // normalise the result
    hpdQEEff /= (double)(totalInHPD);
  }
  else
  {
    hpdQEEff = 1;
    Warning( "No HPDs found -> Assuming Av. HPD Q.E. of 1", StatusCode::SUCCESS );
  }

  // Weighted primary mirror reflectivity
  double primMirrRefl(0);
  if ( !primMirrCount.empty() )
  {
    totalInHPD = 0;
    for ( MirrorCount::const_iterator iPM = primMirrCount.begin();
          iPM != primMirrCount.end(); ++iPM )
    {
      // count mirrors
      totalInHPD += iPM->second;
      // add up mirror refl.
      primMirrRefl +=
        (double)(iPM->second) * (*(iPM->first->reflectivity()))[energy*Gaudi::Units::eV];
    }
    // normalise the result
    primMirrRefl /= (double)(totalInHPD);
  }
  else
  {
    primMirrRefl = 1;
    Warning( "No primary mirrors found -> Assuming Av. reflectivity of 1", StatusCode::SUCCESS );
  }

  // Weighted secondary mirror reflectivity
  double secMirrRefl(0);
  if ( !secMirrCount.empty() )
  {
    totalInHPD = 0;
    for ( MirrorCount::const_iterator iSM = secMirrCount.begin();
          iSM != secMirrCount.end(); ++iSM )
    {
      // count mirrors
      totalInHPD += iSM->second;
      // add up mirror refl.
      secMirrRefl +=
        (double)(iSM->second) * (*(iSM->first->reflectivity()))[energy*Gaudi::Units::eV];
    }
    // normalise the result
    secMirrRefl /= (double)(totalInHPD);
  }
  else
  {
    secMirrRefl = 1;
    Warning( "No secondary mirrors found -> Assuming Av. reflectivity of 1", StatusCode::SUCCESS );
  }

  if ( msgLevel(MSG::DEBUG) )
    debug() << " -> Av. HPD Q.E. = " << hpdQEEff << " Prim. Mirr Refl. = " << primMirrRefl
            << " Sec. Mirr. Refl. " << secMirrRefl << endreq;

  // return overall efficiency
  return m_qEffPedLoss * hpdQEEff * primMirrRefl * secMirrRefl;
}

//================================================================================
// The HPD panel location
//================================================================================
const std::string &
TabulatedSignalDetectionEff::pdPanelName( const Rich::DetectorType rich,
                                          const Rich::Side         panel ) const
{
  if ( m_riches[rich]->exists("HPDPanelDetElemLocations") )
  {
    return (m_riches[rich]->paramVect<std::string>("HPDPanelDetElemLocations"))[panel];
  }
  else
  {
    // Backwards compat for DC06
    const std::string* dc06Names[Rich::NRiches][Rich::NHPDPanelsPerRICH]
      = { { &DeRichHPDPanelLocation::Rich1Panel0,
            &DeRichHPDPanelLocation::Rich1Panel1 },
          { &DeRichHPDPanelLocation::Rich2Panel0,
            &DeRichHPDPanelLocation::Rich2Panel1 } };
    return *dc06Names[rich][panel];
  }
}
