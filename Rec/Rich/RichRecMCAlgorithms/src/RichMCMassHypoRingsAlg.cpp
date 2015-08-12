
//--------------------------------------------------------------------------
/** @file RichMCMassHypoRingsAlg.cpp
 *
 *  Implementation file for algorithm class : RichMCMassHypoRingsAlg
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//--------------------------------------------------------------------------

// local
#include "RichMCMassHypoRingsAlg.h"

// namespace
using namespace Rich::Rec::MC;

//--------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( MCMassHypoRingsAlg )

// Standard constructor, initializes variables
MCMassHypoRingsAlg::MCMassHypoRingsAlg( const std::string& name,
                                        ISvcLocator* pSvcLocator )
  : Rich::Rec::AlgBase ( name, pSvcLocator ),
    m_truth            ( 0 ),
    m_mcTkInfo         ( 0 ),
    m_rayTrace         ( 0 ),
    m_maxCKtheta       ( Rich::NRadiatorTypes, 999 ),
    m_minCKtheta       ( Rich::NRadiatorTypes, 0   ),
    m_linker           ( NULL )
{

  // Event locations to process
  m_evtLocs.clear();
  m_evtLocs.push_back( ""               );
  m_evtLocs.push_back( "Prev/"          );
  m_evtLocs.push_back( "PrevPrev/"      );
  m_evtLocs.push_back( "Next/"          );
  m_evtLocs.push_back( "NextNext/"      );
  m_evtLocs.push_back( "LHCBackground/" );
  declareProperty( "EventLocations", m_evtLocs );

  m_minCKtheta[Rich::Aerogel]  = 0.09 ;   // aerogel
  m_minCKtheta[Rich::Rich1Gas] = 0 ;      // rich1Gas
  m_minCKtheta[Rich::Rich2Gas] = 0 ;      // rich2Gas
  declareProperty( "MinCherenkovTheta", m_minCKtheta );

  m_maxCKtheta[Rich::Aerogel]  = 0.300; // aerogel
  m_maxCKtheta[Rich::Rich1Gas] = 0.080; // rich1Gas
  m_maxCKtheta[Rich::Rich2Gas] = 0.050; // rich2Gas
  declareProperty( "MaxCherenkovTheta", m_maxCKtheta );

  m_usedRads[Rich::Aerogel]  = true;
  m_usedRads[Rich::Rich1Gas] = true;
  m_usedRads[Rich::Rich2Gas] = true;

  declareProperty( "UseDetailedHPDsInRayTracing", m_useDetailedHPDsForRayT = false );

}

// Destructor
MCMassHypoRingsAlg::~MCMassHypoRingsAlg() {}

// Initialize
StatusCode MCMassHypoRingsAlg::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = Rich::Rec::AlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // acquire tools
  acquireTool( "RichMCTruthTool",     m_truth, NULL, true );
  acquireTool( "RichMCTrackInfoTool", m_mcTkInfo );
  acquireTool( "RichRayTraceCKCone",  m_rayTrace );

  /// Ray-tracing configuration object
  m_traceMode = LHCb::RichTraceMode( LHCb::RichTraceMode::RespectHPDTubes,
                                     ( m_useDetailedHPDsForRayT ? 
                                       LHCb::RichTraceMode::FullHPDs : 
                                       LHCb::RichTraceMode::SimpleHPDs ) );
  m_traceMode.setAeroRefraction ( true );
  info() << "MCTrack " << m_traceMode << endmsg;
  
  return sc;
}

StatusCode MCMassHypoRingsAlg::execute()
{
  debug() << "Execute" << endmsg;

  // loop over data locations
  StatusCode sc = StatusCode::SUCCESS;
  for ( EventList::const_iterator iEvt = m_evtLocs.begin();
        iEvt != m_evtLocs.end(); ++iEvt )
  {
    sc = buildRings( *iEvt );
    if ( sc.isFailure() ) { break; }
  }

  // force a new linker for next event
  resetLinker();

  return sc;
}

StatusCode
MCMassHypoRingsAlg::buildRings( const std::string & evtLoc ) const
{
  // Try to locate MCRichSegments
  SmartDataPtr<LHCb::MCRichSegments> mcSegs( eventSvc(),
                                             evtLoc+LHCb::MCRichSegmentLocation::Default );
  if ( !mcSegs ) { return StatusCode::SUCCESS; }
  debug() << "Successfully located " << mcSegs->size()
          << " MCRichSegments at " << evtLoc << LHCb::MCRichSegmentLocation::Default << endmsg;

  // Cache pointer to Ring tool for this event location
  const IMassHypothesisRingCreator * ringCr = ringCreator(evtLoc);

  // iterate over segments
  for ( LHCb::MCRichSegments::const_iterator iSeg = mcSegs->begin();
        iSeg != mcSegs->end(); ++iSeg )
  {
    const LHCb::MCRichSegment * segment = *iSeg;
    if ( !segment ) continue;
    verbose() << "Trying MCRichSegment " << segment->key() << " : radiator = "
              << segment->radiator() << endmsg;
    if ( !m_usedRads[segment->radiator()] )
    {
      verbose() << " -> Radiator type is rejected" << endmsg;
      continue;
    }

    // MC PID
    const Rich::ParticleIDType mcpid =
      m_truth->mcParticleType( segment->mcParticle() );
    if ( Rich::Unknown == mcpid )
    {
      verbose() << " -> MC type is Unknown -> abort" << endmsg;
      continue;
    }
    verbose() << " -> MC type is " << mcpid << endmsg;

    // Cherenkov angle for this segment
    const double theta = ckTheta(segment);
    if ( theta < m_minCKtheta[segment->radiator()] ||
         theta > m_maxCKtheta[segment->radiator()] )
    {
      verbose() << " -> " << theta << " fails CK theta cut -> abort" << endmsg;
      continue;
    }
    verbose() << " -> CK theta = " << theta << endmsg;

    int nPoints = static_cast<int>( 1500 * theta );
    if ( nPoints<30 ) nPoints = 30;

    // Emission point and direction
    const Gaudi::XYZPoint  bestPtn = segment->bestPoint(0.5);
    const Gaudi::XYZVector bestDir = segment->bestMomentum(0.5);

    // ray-trace the ring points in tight mode to find out if any part of
    // the ring is in the general acceptance of the HPD panels
    std::vector<Gaudi::XYZPoint> points;
    LHCb::RichTraceMode mode;
    mode.setDetPrecision      ( LHCb::RichTraceMode::SimpleHPDs      );
    mode.setDetPlaneBound     ( LHCb::RichTraceMode::RespectHPDPanel );
    mode.setForcedSide        ( false                 );
    mode.setOutMirrorBoundary ( false                 );
    mode.setMirrorSegBoundary ( false                 );
    m_rayTrace->rayTrace( segment->rich(), bestPtn, bestDir,
                          theta, points, nPoints, mode );
    // if no points in acceptance, delete this ring and continue
    if ( points.empty() )
    {
      verbose() << " -> CK ring found to not intersect any HPD -> abort" << endmsg;
      continue;
    }

    // Get a new Ring object
    LHCb::RichRecRing * ring = ringCr->newMassHypoRing();

    // set ring type info
    ring->setType ( LHCb::RichRecRing::RayTracedCK );

    // set angle
    ring->setRadius( static_cast<LHCb::RichRecRing::FloatType>(theta) );

    // set the PID for this segment
    ring->setMassHypo( mcpid );

    // detector information
    ring->setRich( segment->rich() );

    // Set the PD panel points
    Gaudi::XYZPoint tmpPoint;
    if ( m_mcTkInfo->panelIntersectGlobal( *iSeg, tmpPoint ) )
    {
      ring->setCentrePointGlobal(tmpPoint);
    }
    if ( m_mcTkInfo->panelIntersectLocal( *iSeg, tmpPoint ) )
    {
      ring->setCentrePointLocal(tmpPoint);
    }

    // ray-trace again using loose mode to get the full ring.
    m_rayTrace->rayTrace( segment->rich(), bestPtn, bestDir,
                          theta, ring, nPoints, m_traceMode );

    // save the new ring
    ringCr->saveMassHypoRing(ring);

    // create link between MCRichSegment and the ring
    linker()->link( segment, ring );

    verbose() << " -> All OK -> new ring created : CKtheta=" << ring->radius()
              << endmsg;

  }

  debug() << "Created " << ringCr->massHypoRings()->size()
          << " MC mass hypothesis rings for " << evtLoc
          << LHCb::MCRichSegmentLocation::Default << endmsg;

  return StatusCode::SUCCESS;
}

double MCMassHypoRingsAlg::ckTheta( const LHCb::MCRichSegment * segment ) const
{
  const SmartRefVector<LHCb::MCRichOpticalPhoton> & photons = segment->mcRichOpticalPhotons();
  if ( photons.empty() )
  {
    verbose() << "  -> Segment has no associated MCRichOpticalPhotons" << endmsg;
    return 0;
  }
  double angle = 0;
  int nPhots(0);
  for ( SmartRefVector<LHCb::MCRichOpticalPhoton>::const_iterator iPhot = photons.begin();
        iPhot != photons.end(); ++iPhot )
  {
    if ( *iPhot && (*iPhot)->mcRichHit()->isSignal() )
    {
      ++nPhots;
      angle += (*iPhot)->cherenkovTheta();
    }
  }

  return ( nPhots>0 ? angle / static_cast<double>(nPhots) : 0 );
}

//===============================================================================
// Create linker object
//===============================================================================
MCMassHypoRingsAlg::MCRichSegmentToMCCKRing * MCMassHypoRingsAlg::linker() const
{
  if ( !m_linker )
  {
    // New linker object
    m_linker =
      new MCRichSegmentToMCCKRing( evtSvc(), msgSvc(),
                                   LHCb::MCRichSegmentLocation::Default+"2MCCKRings" );
    // set the ordering
    m_linker->setDecreasingWeight();
  }
  return m_linker;
}
