
//--------------------------------------------------------------------------
/** @file RichMCMassHypoRingsAlg.cpp
 *
 *  Implementation file for algorithm class : RichMCMassHypoRingsAlg
 *
 *  CVS Log :-
 *  $Id: RichMCMassHypoRingsAlg.cpp,v 1.3 2005-11-07 09:37:19 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//--------------------------------------------------------------------------

// local
#include "RichMCMassHypoRingsAlg.h"

//--------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichMCMassHypoRingsAlg>          s_factory ;
const        IAlgFactory& RichMCMassHypoRingsAlgFactory = s_factory ;

// Standard constructor, initializes variables
RichMCMassHypoRingsAlg::RichMCMassHypoRingsAlg( const std::string& name,
                                                ISvcLocator* pSvcLocator )
  : RichRecAlgBase ( name, pSvcLocator ),
    m_truth        ( 0 ),
    m_mcTkInfo     ( 0 ),
    m_rayTrace     ( 0 ),
    m_usedRads     ( Rich::NRadiatorTypes, true )
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

  m_minCKtheta.push_back( 0.09 );   // aerogel
  m_minCKtheta.push_back( 0 );      // c4f10
  m_minCKtheta.push_back( 0 );      // cf4
  declareProperty( "MinCherenkovTheta", m_minCKtheta );

  m_maxCKtheta.push_back( 0.300 ); // aerogel
  m_maxCKtheta.push_back( 0.080 ); // c4f10
  m_maxCKtheta.push_back( 0.050 ); // cf4
  declareProperty( "MaxCherenkovTheta", m_maxCKtheta );

  // which radiators to use
  declareProperty( "UseRadiators", m_usedRads );

}

// Destructor
RichMCMassHypoRingsAlg::~RichMCMassHypoRingsAlg() {}

// Initialize
StatusCode RichMCMassHypoRingsAlg::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // acquire tools
  acquireTool( "RichMCTruthTool",     m_truth    );
  acquireTool( "RichMCTrackInfoTool", m_mcTkInfo );
  acquireTool( "RichRayTraceCKCone",  m_rayTrace );

  return sc;
}


StatusCode RichMCMassHypoRingsAlg::execute()
{
  debug() << "Execute" << endreq;

  // loop over data locations
  for ( EventList::const_iterator iEvt = m_evtLocs.begin();
        iEvt != m_evtLocs.end(); ++iEvt ) 
  {
    const StatusCode sc = buildRings( *iEvt );
    if ( sc.isFailure() ) { return sc; }
  }

  return StatusCode::SUCCESS;
}

StatusCode
RichMCMassHypoRingsAlg::buildRings( const std::string & evtLoc ) const
{
  // Try to locate MCRichSegments
  SmartDataPtr<MCRichSegments> mcSegs( eventSvc(),
                                       evtLoc+MCRichSegmentLocation::Default );
  if ( !mcSegs ) { return StatusCode::SUCCESS; }
  debug() << "Successfully located " << mcSegs->size()
          << " MCRichSegments at " << evtLoc << MCRichSegmentLocation::Default << endreq;

  // Cache pointer to Ring tool for this event location
  const IRichMassHypothesisRingCreator * ringCr = ringCreator(evtLoc);

  // Ray tracing mode
  RichTraceMode mode;
  
  // iterate over segments
  for ( MCRichSegments::const_iterator iSeg = mcSegs->begin();
        iSeg != mcSegs->end(); ++iSeg ) 
  {
    const MCRichSegment * segment = *iSeg;
    if ( !segment ) continue;

    // Check if this radiator is in use
    if ( !m_usedRads[segment->radiator()] ) continue;

    // MC PID
    const Rich::ParticleIDType mcpid =
      m_truth->mcParticleType( segment->mcParticle() );
    if ( Rich::Unknown == mcpid ) continue;

    // Cherenkov angle for this segment
    const double theta = ckTheta(segment);
    if ( theta < m_minCKtheta[segment->radiator()] ||
         theta > m_maxCKtheta[segment->radiator()] ) continue;

    // Emission point and direction
    const HepPoint3D  bestPtn = segment->bestPoint(0.5);
    const HepVector3D bestDir = segment->bestMomentum(0.5);

    // ray-trace the ring points in tight mode to find out if any part of
    // the ring is in the general acceptance of the HPD panels
    std::vector<HepPoint3D> points;
    mode.setDetPlaneBound( RichTraceMode::tight ); // dis-regard HPD panel boundaries
    m_rayTrace->rayTrace( segment->rich(), bestPtn, bestDir,
                          theta, points, mode );
    // if no points in acceptance, delete this ring and continue
    if ( points.empty() ) { continue; }

    // Get a new Ring object
    RichRecRing * ring = ringCr->newMassHypoRing();

    // set angle
    ring->setRadius( theta );

    // set the PID for this segment
    ring->setMassHypo( mcpid );

    // detector information
    ring->setRich( segment->rich() );

    // Set the PD panel points
    m_mcTkInfo->panelIntersectGlobal ( *iSeg, ring->centrePointGlobal() );
    m_mcTkInfo->panelIntersectLocal  ( *iSeg, ring->centrePointLocal()  );

    // ray-trace again using loose mode to get the full ring.
    mode.setDetPlaneBound( RichTraceMode::loose ); // dis-regard HPD panel boundaries
    m_rayTrace->rayTrace( segment->rich(), bestPtn, bestDir,
                          theta, ring->ringPoints(), mode );

    // save the new ring
    ringCr->saveMassHypoRing(ring);

  }

  debug() << "Created " << ringCr->massHypoRings()->size()
          << " MC mass hypothesis rings for " << evtLoc << endreq;

  return StatusCode::SUCCESS;
}

double RichMCMassHypoRingsAlg::ckTheta( const MCRichSegment * segment ) const
{
  const SmartRefVector<MCRichOpticalPhoton>& photons = segment->mcRichOpticalPhotons();
  if ( photons.empty() ) return 0;
  double angle = 0;
  for ( SmartRefVector<MCRichOpticalPhoton>::const_iterator iPhot = photons.begin();
        iPhot != photons.end(); ++iPhot ) {
    if ( *iPhot && !m_truth->isBackground( (*iPhot)->mcRichHit() ) ) {
      angle += (*iPhot)->cherenkovTheta();
    }
  }

  return ( angle / static_cast<double>(photons.size()) );
}

StatusCode RichMCMassHypoRingsAlg::finalize()
{
  // Execute base class method
  return RichRecAlgBase::finalize();
}
