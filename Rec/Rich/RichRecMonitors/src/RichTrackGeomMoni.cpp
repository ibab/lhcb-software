
//---------------------------------------------------------------------------
/** @file RichTrackGeomMoni.cpp
 *
 *  Implementation file for algorithm class : RichTrackGeomMoni
 *
 *  $Id: RichTrackGeomMoni.cpp,v 1.6 2006-01-23 14:10:48 jonrob Exp $
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
  if ( !m_trSelector.configureTrackTypes() )
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
  if ( richTracks()->empty() ) {
    if ( !trackCreator()->newTracks() ) return StatusCode::FAILURE;
    debug() << "No tracks found : Created " << richTracks()->size()
            << " RichRecTracks " << richSegments()->size()
            << " RichRecSegments" << endreq;
  }

  // Tally number of segments
  unsigned int nSegs[Rich::NRadiatorTypes]   = { 0, 0, 0 };
  unsigned int nMCSegs[Rich::NRadiatorTypes] = { 0, 0, 0 };

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

    const Rich::DetectorType iRich = trackSeg.rich();    // which rich detector
    const Rich::RadiatorType iRad = trackSeg.radiator(); // which radiator
    ++nSegs[iRad]; // count segments per radiator

    // HPD panel impact points (ray traced)
    const Gaudi::XYZPoint & pdPoint    = segment->pdPanelHitPoint();
    const Gaudi::XYZPoint & pdPointLoc = segment->pdPanelHitPointLocal();

    // Pointer to parent MCParticle
    const MCParticle * trackMCPart =
      m_richRecMCTruth->mcParticle( segment->richRecTrack() );

    // radiator entry/exit information
    const Gaudi::XYZVector & trackDir = trackSeg.bestMomentum();

    // entry/exit point histograms
    plot1D( trackSeg.entryPoint().z(), hid(iRad,"zEntry"), "Track entrance z",
            zRadEntGlo[iRad],zRadExitGlo[iRad] );
    plot1D( trackSeg.exitPoint().z(), hid(iRad,"zExit"), "Track exit z",
            zRadEntGlo[iRad],zRadExitGlo[iRad] );
    plot2D( trackSeg.entryPoint().x(),trackSeg.entryPoint().y(),
            hid(iRad,"xyEntry"), "Track entrance yVx",
            -xRadEntGlo[iRad],xRadEntGlo[iRad],-yRadEntGlo[iRad],yRadEntGlo[iRad] );
    plot2D( trackSeg.exitPoint().x(),trackSeg.exitPoint().y(),
            hid(iRad,"xyExit"), "Track exit yVx",
            -xRadExitGlo[iRad],xRadExitGlo[iRad],-yRadExitGlo[iRad],yRadExitGlo[iRad] );
    plot1D( trackSeg.pathLength(), hid(iRad,"pathL"), "Path length",
            zRadLenMin[iRad],zRadLenMax[iRad] );

    // Ray traced hit point on PDPanel
    plot1D( pdPoint.x(), hid(iRich,"xTkPDpan"), "Projected track PDPanel hits x global",
            xMinPDGlo[iRich], xMaxPDGlo[iRich] );
    plot1D( pdPoint.y(), hid(iRich,"yTkPDpan"), "Projected track PDPanel hits y global",
            yMinPDGlo[iRich], yMaxPDGlo[iRich] );
    plot1D( pdPoint.z(), hid(iRich,"zTkPDpan"), "Projected track PDPanel hits z global",
            zMinPDGlo[iRich], zMaxPDGlo[iRich] );

    plot2D( pdPoint.x(), pdPoint.y(),
            hid(iRich,"xyTkPDpan"), "Projected track PDPanel hits yVx global",
            xMinPDGlo[iRich],xMaxPDGlo[iRich],yMinPDGlo[iRich],yMaxPDGlo[iRich] );
    plot2D( pdPoint.z(), pdPoint.x(),
            hid(iRich,"xzTkPDpan"), "Projected track PDPanel hits xVz global",
            zMinPDGlo[iRich],zMaxPDGlo[iRich],xMinPDGlo[iRich],xMaxPDGlo[iRich] );
    plot2D( pdPoint.z(), pdPoint.y(),
            hid(iRich,"yzTkPDpan"), "Projected track PDPanel hits yVz global",
            zMinPDGlo[iRich],zMaxPDGlo[iRich],yMinPDGlo[iRich],yMaxPDGlo[iRich] );

    plot1D( pdPointLoc.x(), hid(iRich,"xTkPDpanLoc"), "Projected track PD hits x local",
            xMinPDLoc[iRich],xMaxPDLoc[iRich] );
    plot1D( pdPointLoc.y(), hid(iRich,"yTkPDpanLoc"), "Projected track PD hits y local",
            yMinPDLoc[iRich],yMaxPDLoc[iRich] );
    plot1D( pdPointLoc.z(), hid(iRich,"zTkPDpanLoc"), "Projected track PD hits z local",
            zMinPDLoc[iRich],zMaxPDLoc[iRich] );

    plot2D( pdPointLoc.x(), pdPointLoc.y(),
            hid(iRich,"xyTkPDpanLoc"), "Projected track PD hits yVx local",
            xMinPDLoc[iRich],xMaxPDLoc[iRich],yMinPDLoc[iRich],yMaxPDLoc[iRich] );

    // HPD panel acceptance
    const double hpdPanAcc = m_geomTool->hpdPanelAcceptance(segment,Rich::Electron);
    plot2D( pdPointLoc.x(), hpdPanAcc,
            hid(iRad,"hpdPanAccX"), "HPD panel acceptance (electron) V x",
            1.1*xMinPDLoc[iRich],1.1*xMaxPDLoc[iRich],0,1.05 );
    plot2D( pdPointLoc.y(), hpdPanAcc,
            hid(iRad,"hpdPanAccY"), "HPD panel acceptance (electron) V y",
            1.1*yMinPDLoc[iRich],1.1*yMaxPDLoc[iRich],0,1.05 );

    // HPD panel acceptance
    const double hpdAcc = m_geomEffic->geomEfficiency(segment,Rich::Electron);
    plot2D( pdPointLoc.x(), hpdAcc,
            hid(iRad,"hpdGeomEffX"), "HPD geom. eff. (electron) V x",
            1.1*xMinPDLoc[iRich],1.1*xMaxPDLoc[iRich],0,1.05 );
    plot2D( pdPointLoc.y(), hpdAcc,
            hid(iRad,"hpdGeomEffY"), "HPD geom. eff. (electron) V y",
            1.1*yMinPDLoc[iRich],1.1*yMaxPDLoc[iRich],0,1.05 );

    // Get associated RichMCSegment
    const MCRichSegment * mcSegment = m_richRecMCTruth->mcRichSegment(segment);
    if ( mcSegment )
    {
      ++nMCSegs[iRad]; // count MC segments per radiator
      // entry/exit coordinates
      plot1D( mcSegment->entryPoint().z(), hid(iRad,"mcEntryZ"), "MC Track entrance z",
              zRadEntGlo[iRad],zRadExitGlo[iRad] );
      plot1D( mcSegment->exitPoint().z(),  hid(iRad,"mcExitZ"),  "MC Track exit z",
              zRadEntGlo[iRad],zRadExitGlo[iRad] );
      plot2D( mcSegment->entryPoint().x(), mcSegment->entryPoint().y(),
              hid(iRad,"mcEntryXY"),  "MC Track entrance yVx",
              -xRadEntGlo[iRad],xRadEntGlo[iRad],-yRadEntGlo[iRad],yRadEntGlo[iRad] );
      plot2D( mcSegment->exitPoint().x(), mcSegment->exitPoint().y(),
              hid(iRad,"mcExitXY"),  "MC Track exit yVx",
              -xRadExitGlo[iRad],xRadExitGlo[iRad],-yRadExitGlo[iRad],yRadExitGlo[iRad] );
      plot1D( mcSegment->pathLength(), hid(iRad,"mcPathL"), "MC Track length",
              zRadLenMin[iRad],zRadLenMax[iRad] );
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
      plot1D( photPoint.x(), hid(iRich,"xTkPD"), "Projected track PD hits x global",
              xMinPDGlo[iRich], xMaxPDGlo[iRich] );
      plot1D( photPoint.y(), hid(iRich,"yTkPD"), "Projected track PD hits y global",
              yMinPDGlo[iRich], yMaxPDGlo[iRich] );
      plot1D( photPoint.z(), hid(iRich,"zTkPD"), "Projected track PD hits z global",
              zMinPDGlo[iRich], zMaxPDGlo[iRich] );
      plot2D( photPoint.x(), photPoint.y(),
              hid(iRich,"xyTkPD"), "Projected track PD hits yVx global",
              xMinPDGlo[iRich],xMaxPDGlo[iRich],yMinPDGlo[iRich],yMaxPDGlo[iRich] );
      plot2D( photPoint.z(), photPoint.x(),
              hid(iRich,"xzTkPD"), "Projected track PD hits xVz global",
              zMinPDGlo[iRich],zMaxPDGlo[iRich],xMinPDGlo[iRich],xMaxPDGlo[iRich] );
      plot2D( photPoint.z(), photPoint.y(),
              hid(iRich,"yzTkPD"), "Projected track PD hits yVz global",
              zMinPDGlo[iRich],zMaxPDGlo[iRich],yMinPDGlo[iRich],yMaxPDGlo[iRich] );

      // Ray traced hit point on PDs, local
      plot1D( pdPointLoc.x(), hid(iRich,"xTkPDLoc"), "Projected track PD hits x local",
              xMinPDLoc[iRich], xMaxPDLoc[iRich] );
      plot1D( pdPointLoc.y(), hid(iRich,"yTkPDLoc"), "Projected track PD hits y local",
              yMinPDLoc[iRich], yMaxPDLoc[iRich] );
      plot1D( pdPointLoc.z(), hid(iRich,"zTkPDLoc"), "Projected track PD hits z local",
              zMinPDLoc[iRich], zMaxPDLoc[iRich] );
      plot2D( pdPointLoc.x(), pdPointLoc.y(),
              hid(iRich,"xyTkPDLoc"), "Projected track PD hits yVx local",
              xMinPDLoc[iRich],xMaxPDLoc[iRich],yMinPDLoc[iRich],yMaxPDLoc[iRich] );

      // Ray traced reflection point on spherical mirror
      const Gaudi::XYZPoint & photSpeMir = photon.sphMirReflectionPoint();
      plot1D( photSpeMir.x(), hid(iRich,"xTkPD"), "Projected track SpheMir hits x global",
              xMinSpheGlo[iRich],xMaxSpheGlo[iRich] );
      plot1D( photSpeMir.y(), hid(iRich,"yTkPD"), "Projected track SpheMir hits y global",
              yMinSpheGlo[iRich],yMaxSpheGlo[iRich] );
      plot1D( photSpeMir.z(), hid(iRich,"zTkPD"), "Projected track SpheMir hits z global",
              zMinSpheGlo[iRich],zMaxSpheGlo[iRich] );
      plot2D( photSpeMir.x(), photSpeMir.y(),
              hid(iRich,"xyTkPD"), "Projected track SpheMir hits yVx global",
              xMinSpheGlo[iRich],xMaxSpheGlo[iRich],yMinSpheGlo[iRich],yMaxSpheGlo[iRich] );
      plot2D( photSpeMir.z(), photSpeMir.x(),
              hid(iRich,"xzTkPD"), "Projected track SpheMir hits xVz global",
              zMinSpheGlo[iRich],zMaxSpheGlo[iRich],xMinSpheGlo[iRich],xMaxSpheGlo[iRich] );
      plot2D( photSpeMir.z(), photSpeMir.y(),
              hid(iRich,"yzTkPD"), "Projected track SpheMir hits yVz global",
              zMinSpheGlo[iRich],zMaxSpheGlo[iRich],yMinSpheGlo[iRich],yMaxSpheGlo[iRich] );

      // Ray traced reflection point on flat mirror
      const Gaudi::XYZPoint & photFlatMir = photon.flatMirReflectionPoint();
      plot1D( photFlatMir.x(), hid(iRich,"xTkPD"), "Projected track FlatMir hits x global",
              xMinFlatGlo[iRich],xMaxFlatGlo[iRich] );
      plot1D( photFlatMir.y(), hid(iRich,"yTkPD"), "Projected track FlatMir hits y global",
              yMinFlatGlo[iRich],yMaxFlatGlo[iRich] );
      plot1D( photFlatMir.z(), hid(iRich,"zTkPD"), "Projected track FlatMir hits z global",
              zMinFlatGlo[iRich],zMaxFlatGlo[iRich] );
      plot2D( photFlatMir.x(), photFlatMir.y(),
              hid(iRich,"xyTkPD"), "Projected track FlatMir hits yVx global",
              xMinFlatGlo[iRich],xMaxFlatGlo[iRich],yMinFlatGlo[iRich],yMaxFlatGlo[iRich] );
      plot2D( photFlatMir.z(), photFlatMir.x(),
              hid(iRich,"xzTkPD"), "Projected track FlatMir hits xVz global",
              zMinFlatGlo[iRich],zMaxFlatGlo[iRich],xMinFlatGlo[iRich],xMaxFlatGlo[iRich] );
      plot2D( photFlatMir.z(), photFlatMir.y(),
              hid(iRich,"yzTkPD"), "Projected track FlatMir hits yVz global",
              zMinFlatGlo[iRich],zMaxFlatGlo[iRich],yMinFlatGlo[iRich],yMaxFlatGlo[iRich] );

      // find out pd positions
      m_xHits[photon.smartID().hpdID()] += photPoint.x();
      m_yHits[photon.smartID().hpdID()] += photPoint.y();
      m_zHits[photon.smartID().hpdID()] += photPoint.z();
      m_hitCount[photon.smartID().hpdID()] += 1;

    }

    // Finally, some debug printout
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "Segment " << segment->key() << " " << iRad << " : "
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
