
//---------------------------------------------------------------------------
/** @file RichTrackRayTraceTest.cpp
 *
 *  Implementation file for algorithm class : TrackRayTraceTest
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

// local
#include "RichTrackRayTraceTest.h"

// namespace
using namespace Rich::Rec;

//---------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( TrackRayTraceTest )

// Standard constructor, initializes variables
TrackRayTraceTest::TrackRayTraceTest( const std::string& name,
                                      ISvcLocator* pSvcLocator )
  : Rich::Rec::HistoAlgBase ( name, pSvcLocator ),
    m_rayTrace          ( NULL ),
    m_idTool            ( NULL )
{
  // job opts
}

// Destructor
TrackRayTraceTest::~TrackRayTraceTest() {}

//  Initialize
StatusCode TrackRayTraceTest::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = Rich::Rec::HistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRayTracing",    m_rayTrace, NULL, true );
  acquireTool( "RichSmartIDTool",   m_idTool,   NULL, true );
  acquireTool( "TrackSelector",     m_trSelector, this );

  // initialise variables

  return sc;
}

// Main execution
StatusCode TrackRayTraceTest::execute()
{
  debug() << "Execute" << endmsg;

  // Check event status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Make sure all tracks and segments have been formed
  if ( richTracks()->empty() )
  {
    if ( !trackCreator()->newTracks() ) return StatusCode::FAILURE;
    debug() << "No tracks found : Created " << richTracks()->size()
            << " RichRecTracks " << richSegments()->size()
            << " RichRecSegments" << endmsg;
  }

  // Iterate over segments
  for ( LHCb::RichRecSegments::const_iterator iSeg = richSegments()->begin();
        iSeg != richSegments()->end(); ++iSeg )
  {
    LHCb::RichRecSegment * segment = *iSeg;

    debug() << "Looking at RichRecSegment " << segment->key() << endmsg;

    // apply track selection
    if ( !m_trSelector->trackSelected( segment->richRecTrack() ) ) continue;

    // Test ray tracing
    LHCb::RichTraceMode traceMode;

    traceMode.setDetPlaneBound ( LHCb::RichTraceMode::IgnoreHPDAcceptance );
    testRayTrace( traceMode, segment );

    traceMode.setDetPlaneBound ( LHCb::RichTraceMode::RespectHPDPanel  );
    testRayTrace( traceMode, segment );

    traceMode.setDetPrecision  ( LHCb::RichTraceMode::SimpleHPDs );
    traceMode.setDetPlaneBound ( LHCb::RichTraceMode::RespectHPDTubes );
    testRayTrace( traceMode, segment );

    traceMode.setDetPrecision  ( LHCb::RichTraceMode::FullHPDs        );
    traceMode.setDetPlaneBound ( LHCb::RichTraceMode::RespectHPDTubes );
    testRayTrace( traceMode, segment );

  } // end segment loop

  return StatusCode::SUCCESS;
}

void TrackRayTraceTest::testRayTrace( const LHCb::RichTraceMode traceMode,
                                      LHCb::RichRecSegment * segment ) const
{
  // Histogramming
  PD_LOCAL_POSITIONS_X;
  PD_LOCAL_POSITIONS_Y;

  // which rich detector
  const Rich::DetectorType rich = segment->trackSegment().rich();

  // test ray tracing
  LHCb::RichGeomPhoton photon;
  const LHCb::RichTraceMode::RayTraceResult result = 
    m_rayTrace->traceToDetector( rich,
                                 segment->trackSegment().bestPoint(),
                                 segment->trackSegment().bestMomentum(),
                                 photon,
                                 traceMode, Rich::top,
                                 segment->trackSegment().avPhotonEnergy() );
  if ( traceMode.traceWasOK(result) )
  {
    std::ostringstream mT;
    mT << rich << " " << traceMode;
    const Gaudi::XYZPoint & gloP = photon.detectionPoint();
    const Gaudi::XYZPoint   locP = m_idTool->globalToPDPanel(gloP);
    debug() << mT.str() << " OK : global=" << gloP << " local=" << locP << endmsg;
    plot2D( locP.X(), locP.Y(), mT.str(),
            xMinPDLoc[rich], xMaxPDLoc[rich], yMinPDLoc[rich], yMaxPDLoc[rich], 200, 200 );
  }
  else
  {
    debug() << rich << " " << traceMode << " FAILED" << endmsg;
  }

}
