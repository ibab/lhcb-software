
//---------------------------------------------------------------------------
/** @file RichTrackGeomMoni.cpp
 *
 *  Implementation file for algorithm class : RichTrackGeomMoni
 *
 *  $Id: RichTrackGeomMoni.cpp,v 1.8 2006-03-02 15:26:30 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

// local
#include "RichTrackGeomMoni.h"

// namespace
using namespace LHCb;

//---------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichTrackGeomMoni>          s_factory ;
const        IAlgFactory& RichTrackGeomMoniFactory = s_factory ;

// Standard constructor, initializes variables
RichTrackGeomMoni::RichTrackGeomMoni( const std::string& name,
                                      ISvcLocator* pSvcLocator )
  : RichRecHistoAlgBase ( name, pSvcLocator ),
    m_rayTrace          ( 0 ),
    m_richRecMCTruth    ( 0 ),
    m_geomTool          ( 0 ),
    m_geomEffic         ( 0 )
{
  // track selector
  declareProperty( "TrackSelection", m_trSelector.selectedTrackTypes() );
}

// Destructor
RichTrackGeomMoni::~RichTrackGeomMoni() {};

//  Initialize
StatusCode RichTrackGeomMoni::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecHistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRayTracing",       m_rayTrace       );
  acquireTool( "RichRecMCTruthTool",   m_richRecMCTruth );
  acquireTool( "RichRecGeometry",      m_geomTool       );
  acquireTool( "RichGeomEff",          m_geomEffic      );
  acquireTool( "RichMCTrackInfoTool",  m_mcTkInfo       );

  // Configure track selector
  if ( !m_trSelector.configureTrackTypes(msg()) )
    return Error( "Problem configuring track selection" );
  m_trSelector.printTrackSelection( info() );

  // Configure the ray-tracing mode
  m_traceMode.setDetPrecision      ( RichTraceMode::circle );
  m_traceMode.setDetPlaneBound     ( RichTraceMode::loose  );
  m_traceMode.setForcedSide        ( false                 );
  m_traceMode.setOutMirrorBoundary ( false                 );
  m_traceMode.setMirrorSegBoundary ( false                 );

  // initialise variables
  m_xHits.clear();
  m_yHits.clear();
  m_zHits.clear();
  m_hitCount.clear();

  return sc;
}

// Main execution
StatusCode RichTrackGeomMoni::execute()
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

  // Histogramming
  const RichHistoID hid;
  PD_GLOBAL_POSITIONS;
  PD_LOCAL_POSITIONS;
  RADIATOR_GLOBAL_POSITIONS;
  SPHERICAL_MIRROR_GLOBAL_POSITIONS;
  FLAT_MIRROR_GLOBAL_POSITIONS;

  // Iterate over segments
  for ( RichRecSegments::const_iterator iSeg = richSegments()->begin();
        iSeg != richSegments()->end(); ++iSeg )
  {
    RichRecSegment * segment = *iSeg;

    debug() << "Looking at RichRecSegment " << segment->key() << endreq;

    // apply track selection
    if ( !m_trSelector.trackSelected( segment->richRecTrack() ) ) continue;

    // Track type
    const Rich::Track::Type trType = segment->richRecTrack()->trackID().trackType();

    // track segment
    const RichTrackSegment & trackSeg = segment->trackSegment();

    const Rich::DetectorType rich = trackSeg.rich();    // which rich detector
    const Rich::RadiatorType rad  = trackSeg.radiator(); // which radiator

    // HPD panel impact points (ray traced)
    const Gaudi::XYZPoint & pdPoint    = segment->pdPanelHitPoint();
    const Gaudi::XYZPoint & pdPointLoc = segment->pdPanelHitPointLocal();

    // Pointer to parent MCParticle
    const MCParticle * trackMCPart =
      m_richRecMCTruth->mcParticle( segment->richRecTrack() );

    // radiator entry/exit information
    const Gaudi::XYZVector & trackDir = trackSeg.bestMomentum();

    // entry/exit point histograms
    plot1D( trackSeg.entryPoint().z(), hid(rad,"entryZ"), "Track entrance z",
            zRadEntGlo[rad],zRadExitGlo[rad] );
    plot1D( trackSeg.exitPoint().z(), hid(rad,"exitZ"), "Track exit z",
            zRadEntGlo[rad],zRadExitGlo[rad] );
    plot2D( trackSeg.entryPoint().x(),trackSeg.entryPoint().y(),
            hid(rad,"entryXY"), "Track entrance yVx",
            -xRadEntGlo[rad],xRadEntGlo[rad],-yRadEntGlo[rad],yRadEntGlo[rad] );
    plot2D( trackSeg.exitPoint().x(),trackSeg.exitPoint().y(),
            hid(rad,"exitXY"), "Track exit yVx",
            -xRadExitGlo[rad],xRadExitGlo[rad],-yRadExitGlo[rad],yRadExitGlo[rad] );
    plot1D( trackSeg.pathLength(), hid(rad,"pathL"), "Path length",
            zRadLenMin[rad],zRadLenMax[rad] );
    plot3D( trackSeg.entryPoint().z(), trackSeg.entryPoint().x(), trackSeg.entryPoint().y(),
            hid(rad,"entryXYZ"), "Track entrance xVyVz",
            zRadEntGlo[rad],zRadExitGlo[rad],
            -xRadEntGlo[rad],xRadEntGlo[rad],-yRadEntGlo[rad],yRadEntGlo[rad] );
    plot3D( trackSeg.exitPoint().z(), trackSeg.exitPoint().x(), trackSeg.exitPoint().y(),
            hid(rad,"exitXYZ"), "Track exit xVyVz",
            zRadEntGlo[rad],zRadExitGlo[rad],
            -xRadExitGlo[rad],xRadExitGlo[rad],-yRadExitGlo[rad],yRadExitGlo[rad] );

    // Ray traced hit point on PDPanel
    plot1D( pdPoint.x(), hid(rich,"xTkPDpan"), "Projected track PDPanel hits x global",
            xMinPDGlo[rich], xMaxPDGlo[rich] );
    plot1D( pdPoint.y(), hid(rich,"yTkPDpan"), "Projected track PDPanel hits y global",
            yMinPDGlo[rich], yMaxPDGlo[rich] );
    plot1D( pdPoint.z(), hid(rich,"zTkPDpan"), "Projected track PDPanel hits z global",
            zMinPDGlo[rich], zMaxPDGlo[rich] );

    plot2D( pdPoint.x(), pdPoint.y(),
            hid(rich,"xyTkPDpan"), "Projected track PDPanel hits yVx global",
            xMinPDGlo[rich],xMaxPDGlo[rich],yMinPDGlo[rich],yMaxPDGlo[rich] );
    plot2D( pdPoint.z(), pdPoint.x(),
            hid(rich,"xzTkPDpan"), "Projected track PDPanel hits xVz global",
            zMinPDGlo[rich],zMaxPDGlo[rich],xMinPDGlo[rich],xMaxPDGlo[rich] );
    plot2D( pdPoint.z(), pdPoint.y(),
            hid(rich,"yzTkPDpan"), "Projected track PDPanel hits yVz global",
            zMinPDGlo[rich],zMaxPDGlo[rich],yMinPDGlo[rich],yMaxPDGlo[rich] );

    plot1D( pdPointLoc.x(), hid(rich,"xTkPDpanLoc"), "Projected track PD hits x local",
            xMinPDLoc[rich],xMaxPDLoc[rich] );
    plot1D( pdPointLoc.y(), hid(rich,"yTkPDpanLoc"), "Projected track PD hits y local",
            yMinPDLoc[rich],yMaxPDLoc[rich] );
    plot1D( pdPointLoc.z(), hid(rich,"zTkPDpanLoc"), "Projected track PD hits z local",
            zMinPDLoc[rich],zMaxPDLoc[rich] );

    plot2D( pdPointLoc.x(), pdPointLoc.y(),
            hid(rich,"xyTkPDpanLoc"), "Projected track PD hits yVx local",
            xMinPDLoc[rich],xMaxPDLoc[rich],yMinPDLoc[rich],yMaxPDLoc[rich] );

    // HPD panel acceptance
    const double hpdPanAcc = m_geomTool->hpdPanelAcceptance(segment,Rich::Electron);
    plot2D( pdPointLoc.x(), hpdPanAcc,
            hid(rad,"hpdPanAccX"), "HPD panel acceptance (electron) V x",
            1.1*xMinPDLoc[rich],1.1*xMaxPDLoc[rich],0,1.05 );
    plot2D( pdPointLoc.y(), hpdPanAcc,
            hid(rad,"hpdPanAccY"), "HPD panel acceptance (electron) V y",
            1.1*yMinPDLoc[rich],1.1*yMaxPDLoc[rich],0,1.05 );

    // HPD panel acceptance
    const double hpdAcc = m_geomEffic->geomEfficiency(segment,Rich::Electron);
    plot2D( pdPointLoc.x(), hpdAcc,
            hid(rad,"hpdGeomEffX"), "HPD geom. eff. (electron) V x",
            1.1*xMinPDLoc[rich],1.1*xMaxPDLoc[rich],0,1.05 );
    plot2D( pdPointLoc.y(), hpdAcc,
            hid(rad,"hpdGeomEffY"), "HPD geom. eff. (electron) V y",
            1.1*yMinPDLoc[rich],1.1*yMaxPDLoc[rich],0,1.05 );

    // Get associated RichMCSegment
    const MCRichSegment * mcSegment = m_richRecMCTruth->mcRichSegment(segment);
    if ( mcSegment )
    {
      // entry/exit coordinates
      plot1D( mcSegment->entryPoint().z(), hid(rad,"mcEntryZ"), "MC Track entrance z",
              zRadEntGlo[rad],zRadExitGlo[rad] );
      plot1D( mcSegment->exitPoint().z(),  hid(rad,"mcExitZ"),  "MC Track exit z",
              zRadEntGlo[rad],zRadExitGlo[rad] );
      plot2D( mcSegment->entryPoint().x(), mcSegment->entryPoint().y(),
              hid(rad,"mcEntryXY"),  "MC Track entrance yVx",
              -xRadEntGlo[rad],xRadEntGlo[rad],-yRadEntGlo[rad],yRadEntGlo[rad] );
      plot2D( mcSegment->exitPoint().x(), mcSegment->exitPoint().y(),
              hid(rad,"mcExitXY"),  "MC Track exit yVx",
              -xRadExitGlo[rad],xRadExitGlo[rad],-yRadExitGlo[rad],yRadExitGlo[rad] );
      plot1D( mcSegment->pathLength(), hid(rad,"mcPathL"), "MC Track length",
              zRadLenMin[rad],zRadLenMax[rad] );
      plot3D( mcSegment->entryPoint().z(), mcSegment->entryPoint().x(), mcSegment->entryPoint().y(),
              hid(rad,"mcEntryXYZ"), "MC Track entrance xVyVz",
              zRadEntGlo[rad],zRadExitGlo[rad],
              -xRadEntGlo[rad],xRadEntGlo[rad],-yRadEntGlo[rad],yRadEntGlo[rad] );
      plot3D( mcSegment->exitPoint().z(), mcSegment->exitPoint().x(), mcSegment->exitPoint().y(),
              hid(rad,"mcExitXYZ"), "MC Track exit xVyVz",
              zRadEntGlo[rad],zRadExitGlo[rad],
              -xRadExitGlo[rad],xRadExitGlo[rad],-yRadExitGlo[rad],yRadExitGlo[rad] );
    }

    // Project track direction to active detector plane and histogram hits
    RichGeomPhoton photon;
    if ( m_rayTrace->traceToDetector( trackSeg.rich(),
                                      trackSeg.bestPoint(),
                                      trackDir,
                                      photon,
                                      m_traceMode ) != 0 )
    {

      // Ray traced hit point on PDs, global
      const Gaudi::XYZPoint & photPoint = photon.detectionPoint();
      plot1D( photPoint.x(), hid(rich,"xTkPD"), "Projected track PD hits x global",
              xMinPDGlo[rich], xMaxPDGlo[rich] );
      plot1D( photPoint.y(), hid(rich,"yTkPD"), "Projected track PD hits y global",
              yMinPDGlo[rich], yMaxPDGlo[rich] );
      plot1D( photPoint.z(), hid(rich,"zTkPD"), "Projected track PD hits z global",
              zMinPDGlo[rich], zMaxPDGlo[rich] );
      plot2D( photPoint.x(), photPoint.y(),
              hid(rich,"xyTkPD"), "Projected track PD hits yVx global",
              xMinPDGlo[rich],xMaxPDGlo[rich],yMinPDGlo[rich],yMaxPDGlo[rich] );
      plot2D( photPoint.z(), photPoint.x(),
              hid(rich,"xzTkPD"), "Projected track PD hits xVz global",
              zMinPDGlo[rich],zMaxPDGlo[rich],xMinPDGlo[rich],xMaxPDGlo[rich] );
      plot2D( photPoint.z(), photPoint.y(),
              hid(rich,"yzTkPD"), "Projected track PD hits yVz global",
              zMinPDGlo[rich],zMaxPDGlo[rich],yMinPDGlo[rich],yMaxPDGlo[rich] );

      // Ray traced hit point on PDs, local
      plot1D( pdPointLoc.x(), hid(rich,"xTkPDLoc"), "Projected track PD hits x local",
              xMinPDLoc[rich], xMaxPDLoc[rich] );
      plot1D( pdPointLoc.y(), hid(rich,"yTkPDLoc"), "Projected track PD hits y local",
              yMinPDLoc[rich], yMaxPDLoc[rich] );
      plot1D( pdPointLoc.z(), hid(rich,"zTkPDLoc"), "Projected track PD hits z local",
              zMinPDLoc[rich], zMaxPDLoc[rich] );
      plot2D( pdPointLoc.x(), pdPointLoc.y(),
              hid(rich,"xyTkPDLoc"), "Projected track PD hits yVx local",
              xMinPDLoc[rich],xMaxPDLoc[rich],yMinPDLoc[rich],yMaxPDLoc[rich] );

      // Ray traced reflection point on spherical mirror
      const Gaudi::XYZPoint & photSpeMir = photon.sphMirReflectionPoint();
      plot1D( photSpeMir.x(), hid(rich,"xTkPD"), "Projected track SpheMir hits x global",
              xMinSpheGlo[rich],xMaxSpheGlo[rich] );
      plot1D( photSpeMir.y(), hid(rich,"yTkPD"), "Projected track SpheMir hits y global",
              yMinSpheGlo[rich],yMaxSpheGlo[rich] );
      plot1D( photSpeMir.z(), hid(rich,"zTkPD"), "Projected track SpheMir hits z global",
              zMinSpheGlo[rich],zMaxSpheGlo[rich] );
      plot2D( photSpeMir.x(), photSpeMir.y(),
              hid(rich,"xyTkPD"), "Projected track SpheMir hits yVx global",
              xMinSpheGlo[rich],xMaxSpheGlo[rich],yMinSpheGlo[rich],yMaxSpheGlo[rich] );
      plot2D( photSpeMir.z(), photSpeMir.x(),
              hid(rich,"xzTkPD"), "Projected track SpheMir hits xVz global",
              zMinSpheGlo[rich],zMaxSpheGlo[rich],xMinSpheGlo[rich],xMaxSpheGlo[rich] );
      plot2D( photSpeMir.z(), photSpeMir.y(),
              hid(rich,"yzTkPD"), "Projected track SpheMir hits yVz global",
              zMinSpheGlo[rich],zMaxSpheGlo[rich],yMinSpheGlo[rich],yMaxSpheGlo[rich] );

      // Ray traced reflection point on flat mirror
      const Gaudi::XYZPoint & photFlatMir = photon.flatMirReflectionPoint();
      plot1D( photFlatMir.x(), hid(rich,"xTkPD"), "Projected track FlatMir hits x global",
              xMinFlatGlo[rich],xMaxFlatGlo[rich] );
      plot1D( photFlatMir.y(), hid(rich,"yTkPD"), "Projected track FlatMir hits y global",
              yMinFlatGlo[rich],yMaxFlatGlo[rich] );
      plot1D( photFlatMir.z(), hid(rich,"zTkPD"), "Projected track FlatMir hits z global",
              zMinFlatGlo[rich],zMaxFlatGlo[rich] );
      plot2D( photFlatMir.x(), photFlatMir.y(),
              hid(rich,"xyTkPD"), "Projected track FlatMir hits yVx global",
              xMinFlatGlo[rich],xMaxFlatGlo[rich],yMinFlatGlo[rich],yMaxFlatGlo[rich] );
      plot2D( photFlatMir.z(), photFlatMir.x(),
              hid(rich,"xzTkPD"), "Projected track FlatMir hits xVz global",
              zMinFlatGlo[rich],zMaxFlatGlo[rich],xMinFlatGlo[rich],xMaxFlatGlo[rich] );
      plot2D( photFlatMir.z(), photFlatMir.y(),
              hid(rich,"yzTkPD"), "Projected track FlatMir hits yVz global",
              zMinFlatGlo[rich],zMaxFlatGlo[rich],yMinFlatGlo[rich],yMaxFlatGlo[rich] );

      // find out pd positions
      m_xHits[photon.smartID().hpdID()] += photPoint.x();
      m_yHits[photon.smartID().hpdID()] += photPoint.y();
      m_zHits[photon.smartID().hpdID()] += photPoint.z();
      m_hitCount[photon.smartID().hpdID()] += 1;

    }

    // Finally, some debug printout
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "Segment " << segment->key() << " " << rad << " : "
              << rad << " : TrTrackType " << trType << " : pTot "
              << sqrt(segment->trackSegment().bestMomentum().Mag2())
              << " pthLen " << segment->trackSegment().pathLength()
              << endreq;
      debug() << " Entry : Point " << segment->trackSegment().entryPoint()
              << " Momentum " << segment->trackSegment().entryMomentum() << endreq
              << " Exit  : Point " << segment->trackSegment().exitPoint()
              << " Momentum " << segment->trackSegment().exitMomentum() << endreq;
      if (trackMCPart)
      {
        debug() << " Found associated MCParticle " << trackMCPart->key() << " :";
        if (mcSegment)
        {
          debug() << " Located associated MCRichSegment " << mcSegment->key() << endreq
                  << " Entry : Point " << mcSegment->entryPoint()
                  << " Momentum " << mcSegment->entryMomentum() << endreq
                  << " Exit  : Point " << mcSegment->exitPoint()
                  << " Momentum " << mcSegment->exitMomentum() << endreq;
        } else {
          debug() << " No asscociated MCRichSegment" << endreq;
        }
      } else {
        debug() << " No asscociated MCParticle" << endreq;
      }
    }

  } // end segment loop

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode RichTrackGeomMoni::finalize()
{
  // Execute base class method
  return RichRecHistoAlgBase::finalize();
}
