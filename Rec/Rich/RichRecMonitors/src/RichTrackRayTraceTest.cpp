
//---------------------------------------------------------------------------
/** @file RichTrackRayTraceTest.cpp
 *
 *  Implementation file for algorithm class : RichTrackRayTraceTest
 *
 *  $Id: RichTrackRayTraceTest.cpp,v 1.2 2006-08-13 17:13:15 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

// local
#include "RichTrackRayTraceTest.h"

// namespace
using namespace LHCb;

//---------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichTrackRayTraceTest>          s_factory ;
const        IAlgFactory& RichTrackRayTraceTestFactory = s_factory ;

// Standard constructor, initializes variables
RichTrackRayTraceTest::RichTrackRayTraceTest( const std::string& name,
                                              ISvcLocator* pSvcLocator )
  : RichRecHistoAlgBase ( name, pSvcLocator ),
    m_rayTrace          ( NULL ),
    m_idTool            ( NULL )
{
  // job opts
}

// Destructor
RichTrackRayTraceTest::~RichTrackRayTraceTest() {};

//  Initialize
StatusCode RichTrackRayTraceTest::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecHistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRayTracing",    m_rayTrace       );
  acquireTool( "RichSmartIDTool",   m_idTool,   0, true );
  acquireTool( "TrackSelector",     m_trSelector, this );
 
  // initialise variables

  return sc;
}

// Main execution
StatusCode RichTrackRayTraceTest::execute()
{
  debug() << "Execute" << endreq;

  // Check event status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Make sure all tracks and segments have been formed
  if ( richTracks()->empty() )
  {
    if ( !trackCreator()->newTracks() ) return StatusCode::FAILURE;
    debug() << "No tracks found : Created " << richTracks()->size()
            << " RichRecTracks " << richSegments()->size()
            << " RichRecSegments" << endreq;
  }

  // Iterate over segments
  for ( RichRecSegments::const_iterator iSeg = richSegments()->begin();
        iSeg != richSegments()->end(); ++iSeg )
  {
    RichRecSegment * segment = *iSeg;

    debug() << "Looking at RichRecSegment " << segment->key() << endreq;

    // apply track selection
    if ( !m_trSelector->trackSelected( segment->richRecTrack() ) ) continue;

    // Test ray tracing
    RichTraceMode traceMode;

    traceMode.setDetPlaneBound ( RichTraceMode::IgnoreHPDAcceptance );
    testRayTrace( traceMode, segment );

    traceMode.setDetPlaneBound ( RichTraceMode::RespectHPDPanel  );
    testRayTrace( traceMode, segment );

    traceMode.setDetPrecision  ( RichTraceMode::SimpleHPDs );
    traceMode.setDetPlaneBound ( RichTraceMode::RespectHPDTubes );
    testRayTrace( traceMode, segment );

    traceMode.setDetPrecision  ( RichTraceMode::FullHPDs        );
    traceMode.setDetPlaneBound ( RichTraceMode::RespectHPDTubes );
    testRayTrace( traceMode, segment );


  } // end segment loop

  return StatusCode::SUCCESS;
}

void RichTrackRayTraceTest::testRayTrace( const RichTraceMode traceMode,
                                          RichRecSegment * segment ) const
{
  // Histogramming
  PD_LOCAL_POSITIONS_X;
  PD_LOCAL_POSITIONS_Y;

  // which rich detector
  const Rich::DetectorType rich = segment->trackSegment().rich();

  // test ray tracing
  RichGeomPhoton photon;
  if ( m_rayTrace->traceToDetector( rich,
                                    segment->trackSegment().bestPoint(),
                                    segment->trackSegment().bestMomentum(),
                                    photon,
                                    traceMode ) )
  {
    std::ostringstream mT;
    mT << rich << " " << traceMode;
    const Gaudi::XYZPoint & gloP = photon.detectionPoint();
    const Gaudi::XYZPoint   locP = m_idTool->globalToPDPanel(gloP);
    debug() << mT.str() << " OK : global=" << gloP << " local=" << locP << endreq;
    plot2D( locP.X(), locP.Y(), mT.str(),
            xMinPDLoc[rich], xMaxPDLoc[rich], yMinPDLoc[rich], yMaxPDLoc[rich], 200, 200 );
  }
  else
  {
    debug() << rich << " " << traceMode << " FAILED" << endreq;
  }

}

//  Finalize
StatusCode RichTrackRayTraceTest::finalize()
{
  // Execute base class method
  return RichRecHistoAlgBase::finalize();
}
