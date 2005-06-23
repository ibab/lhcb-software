
//---------------------------------------------------------------------------
/** @file RichTrackGeomMoni.cpp
 *
 *  Implementation file for algorithm class : RichTrackGeomMoni
 *
 *  $Id: RichTrackGeomMoni.cpp,v 1.2 2005-06-23 15:14:56 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

// local
#include "RichTrackGeomMoni.h"

//---------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichTrackGeomMoni>          s_factory ;
const        IAlgFactory& RichTrackGeomMoniFactory = s_factory ;

// Standard constructor, initializes variables
RichTrackGeomMoni::RichTrackGeomMoni( const std::string& name,
                                      ISvcLocator* pSvcLocator )
  : RichRecMoniAlgBase ( name, pSvcLocator ),
    m_rayTrace         ( 0 ),
    m_richRecMCTruth   ( 0 ),
    m_geomTool         ( 0 ),
    m_geomEffic        ( 0 )
{

  // histogram locations
  declareProperty( "MCHistoPath", m_mcHistPth = "RICH/RecTrGeom/MC/" );
  declareProperty( "HistoPath",   m_histPth   = "RICH/RecTrGeom/" );

  // track selector
  declareProperty( "TrackSelection", m_trSelector.selectedTrackTypes() );

}

// Destructor
RichTrackGeomMoni::~RichTrackGeomMoni() {};

//  Initialize
StatusCode RichTrackGeomMoni::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecMoniAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRayTracing",       m_rayTrace       );
  acquireTool( "RichRecMCTruthTool",   m_richRecMCTruth );
  m_geomTool = geometryTool();
  acquireTool( "RichGeomEff",          m_geomEffic      );
  acquireTool( "RichMCTrackInfoTool",  m_mcTkInfo       );

  // Configure track selector
  if ( !m_trSelector.configureTrackTypes() ) return StatusCode::FAILURE;

  // Book histograms
  if ( !bookHistograms() || !bookMCHistograms() ) return StatusCode::FAILURE;

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

const StatusCode RichTrackGeomMoni::bookHistograms()
{

  std::string title;
  int id;
  const int nBins = 100;

  // Defines for various parameters
  RICH_HISTO_OFFSET;
  RAD_HISTO_OFFSET;
  RICH_NAMES;
  RADIATOR_NAMES;
  PD_GLOBAL_POSITIONS;
  PD_LOCAL_POSITIONS;
  RADIATOR_GLOBAL_POSITIONS;
  SPHERICAL_MIRROR_GLOBAL_POSITIONS;
  FLAT_MIRROR_GLOBAL_POSITIONS;

  for ( int iRich = 0; iRich<2; ++iRich )
  {

    title = rich[iRich]+" projected track PD hits x global";
    id = richOffset*(iRich+1) + 1;
    m_projPDHitsXGlo[iRich] =
      histoSvc()->book(m_histPth,id,title,nBins,xMinPDGlo[iRich],xMaxPDGlo[iRich]);

    title = rich[iRich]+" projected track PD hits y global";
    id = richOffset*(iRich+1) + 2;
    m_projPDHitsYGlo[iRich] =
      histoSvc()->book(m_histPth,id,title,nBins,yMinPDGlo[iRich],yMaxPDGlo[iRich]);

    title = rich[iRich]+" projected track PD hits z global";
    id = richOffset*(iRich+1) + 3;
    m_projPDHitsZGlo[iRich] =
      histoSvc()->book(m_histPth,id,title,nBins,zMinPDGlo[iRich],zMaxPDGlo[iRich]);

    title = rich[iRich]+" projected track PD hits yVx global";
    id = richOffset*(iRich+1) + 4;
    m_projPDHitsXYGlo[iRich] =
      histoSvc()->book(m_histPth,id,title,
                       nBins,xMinPDGlo[iRich],xMaxPDGlo[iRich],
                       nBins,yMinPDGlo[iRich],yMaxPDGlo[iRich]);

    title = rich[iRich]+" projected track PD hits xVz global";
    id = richOffset*(iRich+1) + 5;
    m_projPDHitsXZGlo[iRich] =
      histoSvc()->book(m_histPth,id,title,
                       nBins,zMinPDGlo[iRich],zMaxPDGlo[iRich],
                       nBins,xMinPDGlo[iRich],xMaxPDGlo[iRich]);

    title = rich[iRich]+" projected track PD hits yVz global";
    id = richOffset*(iRich+1) + 6;
    m_projPDHitsYZGlo[iRich] =
      histoSvc()->book(m_histPth,id,title,
                       nBins,zMinPDGlo[iRich],zMaxPDGlo[iRich],
                       nBins,yMinPDGlo[iRich],yMaxPDGlo[iRich]);

    // Spherical mirror plots

    title = rich[iRich]+" projected track SpheMir hits x global";
    id = richOffset*(iRich+1) + 11;
    m_projSpheHitsXGlo[iRich] =
      histoSvc()->book(m_histPth,id,title,
                       nBins,xMinSpheGlo[iRich],xMaxSpheGlo[iRich]);

    title = rich[iRich]+" projected track SpheMir hits y global";
    id = richOffset*(iRich+1) + 12;
    m_projSpheHitsYGlo[iRich] =
      histoSvc()->book(m_histPth,id,title,nBins,yMinSpheGlo[iRich],yMaxSpheGlo[iRich]);

    title = rich[iRich]+" projected track SpheMir hits z global";
    id = richOffset*(iRich+1) + 13;
    m_projSpheHitsZGlo[iRich] =
      histoSvc()->book(m_histPth,id,title,nBins,zMinSpheGlo[iRich],zMaxSpheGlo[iRich]);

    title = rich[iRich]+" projected track SpheMir hits yVx global";
    id = richOffset*(iRich+1) + 14;
    m_projSpheHitsXYGlo[iRich] =
      histoSvc()->book(m_histPth,id,title,
                       nBins,xMinSpheGlo[iRich],xMaxSpheGlo[iRich],
                       nBins,yMinSpheGlo[iRich],yMaxSpheGlo[iRich]);

    title = rich[iRich]+" projected track SpheMir hits xVz global";
    id = richOffset*(iRich+1) + 15;
    m_projSpheHitsXZGlo[iRich] =
      histoSvc()->book(m_histPth,id,title,
                       nBins,zMinSpheGlo[iRich],zMaxSpheGlo[iRich],
                       nBins,xMinSpheGlo[iRich],xMaxSpheGlo[iRich]);

    title = rich[iRich]+" projected track SpheMir hits yVz global";
    id = richOffset*(iRich+1) + 16;
    m_projSpheHitsYZGlo[iRich] =
      histoSvc()->book(m_histPth,id,title,
                       nBins,zMinSpheGlo[iRich],zMaxSpheGlo[iRich],
                       nBins,yMinSpheGlo[iRich],yMaxSpheGlo[iRich]);

    // Flat mirror plots

    title = rich[iRich]+" projected track FlatMir hits x global";
    id = richOffset*(iRich+1) + 21;
    m_projFlatHitsXGlo[iRich] =
      histoSvc()->book(m_histPth,id,title,nBins,xMinFlatGlo[iRich],xMaxFlatGlo[iRich]);

    title = rich[iRich]+" projected track FlatMir hits y global";
    id = richOffset*(iRich+1) + 22;
    m_projFlatHitsYGlo[iRich] =
      histoSvc()->book(m_histPth,id,title,nBins,yMinFlatGlo[iRich],yMaxFlatGlo[iRich]);

    title = rich[iRich]+" projected track FlatMir hits z global";
    id = richOffset*(iRich+1) + 23;
    m_projFlatHitsZGlo[iRich] =
      histoSvc()->book(m_histPth,id,title,nBins,zMinFlatGlo[iRich],zMaxFlatGlo[iRich]);

    title = rich[iRich]+" projected track FlatMir hits yVx global";
    id = richOffset*(iRich+1) + 24;
    m_projFlatHitsXYGlo[iRich] =
      histoSvc()->book(m_histPth,id,title,
                       nBins,xMinFlatGlo[iRich],xMaxFlatGlo[iRich],
                       nBins,yMinFlatGlo[iRich],yMaxFlatGlo[iRich]);

    title = rich[iRich]+" projected track FlatMir hits xVz global";
    id = richOffset*(iRich+1) + 25;
    m_projFlatHitsXZGlo[iRich] =
      histoSvc()->book(m_histPth,id,title,
                       nBins,zMinFlatGlo[iRich],zMaxFlatGlo[iRich],
                       nBins,xMinFlatGlo[iRich],xMaxFlatGlo[iRich]);

    title = rich[iRich]+" projected track FlatMir hits yVz global";
    id = richOffset*(iRich+1) + 26;
    m_projFlatHitsYZGlo[iRich] =
      histoSvc()->book(m_histPth,id,title,
                       nBins,zMinFlatGlo[iRich],zMaxFlatGlo[iRich],
                       nBins,yMinFlatGlo[iRich],yMaxFlatGlo[iRich]);

    // PD Panel hits

    title = rich[iRich]+" projected track PDPanel hits x global";
    id = richOffset*(iRich+1) + 31;
    m_projPDPanelHitsXGlo[iRich] =
      histoSvc()->book(m_histPth,id,title,nBins,xMinPDGlo[iRich],xMaxPDGlo[iRich]);

    title = rich[iRich]+" projected track PDPanel hits y global";
    id = richOffset*(iRich+1) + 32;
    m_projPDPanelHitsYGlo[iRich] =
      histoSvc()->book(m_histPth,id,title,nBins,yMinPDGlo[iRich],yMaxPDGlo[iRich]);

    title = rich[iRich]+" projected track PDPanel hits z global";
    id = richOffset*(iRich+1) + 33;
    m_projPDPanelHitsZGlo[iRich] =
      histoSvc()->book(m_histPth,id,title,nBins,zMinPDGlo[iRich],zMaxPDGlo[iRich]);

    title = rich[iRich]+" projected track PDPanel hits yVx global";
    id = richOffset*(iRich+1) + 34;
    m_projPDPanelHitsXYGlo[iRich] =
      histoSvc()->book(m_histPth,id,title,
                       nBins,xMinPDGlo[iRich],xMaxPDGlo[iRich],
                       nBins,yMinPDGlo[iRich],yMaxPDGlo[iRich]);

    title = rich[iRich]+" projected track PDPanel hits xVz global";
    id = richOffset*(iRich+1) + 35;
    m_projPDPanelHitsXZGlo[iRich] =
      histoSvc()->book(m_histPth,id,title,
                       nBins,zMinPDGlo[iRich],zMaxPDGlo[iRich],
                       nBins,xMinPDGlo[iRich],xMaxPDGlo[iRich]);

    title = rich[iRich]+" projected track PDPanel hits yVz global";
    id = richOffset*(iRich+1) + 36;
    m_projPDPanelHitsYZGlo[iRich] =
      histoSvc()->book(m_histPth,id,title,
                       nBins,zMinPDGlo[iRich],zMaxPDGlo[iRich],
                       nBins,yMinPDGlo[iRich],yMaxPDGlo[iRich]);

    title = rich[iRich]+" projected track PD hits x local";
    id = richOffset*(iRich+1) + 51;
    m_projPDHitsXLoc[iRich] =
      histoSvc()->book(m_histPth,id,title,nBins,xMinPDLoc[iRich],xMaxPDLoc[iRich]);

    title = rich[iRich]+" projected track PD hits y local";
    id = richOffset*(iRich+1) + 52;
    m_projPDHitsYLoc[iRich] =
      histoSvc()->book(m_histPth,id,title,nBins,yMinPDLoc[iRich],yMaxPDLoc[iRich]);

    title = rich[iRich]+" projected track PD hits z local";
    id = richOffset*(iRich+1) + 53;
    m_projPDHitsZLoc[iRich] =
      histoSvc()->book(m_histPth,id,title,nBins,zMinPDLoc[iRich],zMaxPDLoc[iRich]);

    title = rich[iRich]+" projected track PD hits yVx local";
    id = richOffset*(iRich+1) + 54;
    m_projPDHitsXYLoc[iRich] =
      histoSvc()->book(m_histPth,id,title,
                       nBins,xMinPDLoc[iRich],xMaxPDLoc[iRich],
                       nBins,yMinPDLoc[iRich],yMaxPDLoc[iRich]);

    title = rich[iRich]+" projected track PDPanel hits x local";
    id = richOffset*(iRich+1) + 61;
    m_projPDPanelHitsXLoc[iRich] =
      histoSvc()->book(m_histPth,id,title,nBins,xMinPDLoc[iRich],xMaxPDLoc[iRich]);

    title = rich[iRich]+" projected track PDPanel hits y local";
    id = richOffset*(iRich+1) + 62;
    m_projPDPanelHitsYLoc[iRich] =
      histoSvc()->book(m_histPth,id,title,nBins,yMinPDLoc[iRich],yMaxPDLoc[iRich]);

    title = rich[iRich]+" projected track PDPanel hits z local";
    id = richOffset*(iRich+1) + 63;
    m_projPDPanelHitsZLoc[iRich] =
      histoSvc()->book(m_histPth,id,title,nBins,zMinPDLoc[iRich],zMaxPDLoc[iRich]);

    title = rich[iRich]+" projected track PDPanel hits yVx local";
    id = richOffset*(iRich+1) + 64;
    m_projPDPanelHitsXYLoc[iRich] =
      histoSvc()->book(m_histPth,id,title,
                       nBins,xMinPDLoc[iRich],xMaxPDLoc[iRich],
                       nBins,yMinPDLoc[iRich],yMaxPDLoc[iRich]);

  } // end Rich loop

  // radiator loop
  for ( int iRad = 0; iRad < Rich::NRadiatorTypes; ++iRad ) 
  {

    // which RICH ?
    const int iRich = ( iRad == Rich::CF4 ? Rich::Rich2 : Rich::Rich1 ); 

    title = "Track entrance z : "+radiator[iRad];
    id = radOffset*(iRad+1) + 1;
    m_trEntrZ[iRad] =
      histoSvc()->book(m_histPth,id,title,nBins,zRadEntGlo[iRad],zRadExitGlo[iRad]);

    title = "Track exit z : "+radiator[iRad];
    id = radOffset*(iRad+1) + 2;
    m_trExitZ[iRad] =
      histoSvc()->book(m_histPth,id,title,nBins,zRadEntGlo[iRad],zRadExitGlo[iRad]);

    title = "Track entrance yVx : "+radiator[iRad];
    id = radOffset*(iRad+1) + 3;
    m_trEntrXY[iRad] =
      histoSvc()->book(m_histPth,id,title,
                       nBins,-xRadEntGlo[iRad],xRadEntGlo[iRad],
                       nBins,-yRadEntGlo[iRad],yRadEntGlo[iRad]);

    title = "Track exit yVx : "+radiator[iRad];
    id = radOffset*(iRad+1) + 4;
    m_trExitXY[iRad] =
      histoSvc()->book(m_histPth,id,title,
                       nBins,-xRadExitGlo[iRad],xRadExitGlo[iRad],
                       nBins,-yRadExitGlo[iRad],yRadExitGlo[iRad]);

    title = "Track length : "+radiator[iRad];
    id = radOffset*(iRad+1) + 5;
    m_trLength[iRad] =
      histoSvc()->book(m_histPth,id,title,
                       nBins,zRadLenMin[iRad],zRadLenMax[iRad]);

    title = radiator[iRad]+" HPD panel acceptance (electron) V x";
    id = radOffset*(iRad+1) + 6;
    m_hpdPanAccVx[iRad] =
      histoSvc()->book(m_histPth,id,title,
                       nBins,1.1*xMinPDLoc[iRich],1.1*xMaxPDLoc[iRich],
                       nBins,0,1.05);

    title = radiator[iRad]+" HPD panel acceptance (electron) V y";
    id = radOffset*(iRad+1) + 7;
    m_hpdPanAccVy[iRad] =
      histoSvc()->book(m_histPth,id,title,
                       nBins,1.1*yMinPDLoc[iRich],1.1*yMaxPDLoc[iRich],
                       nBins,0,1.05);

    title = radiator[iRad]+" HPD acceptance (electron) V x";
    id = radOffset*(iRad+1) + 8;
    m_geomEffVx[iRad] =
      histoSvc()->book(m_histPth,id,title,
                       nBins,1.1*xMinPDLoc[iRich],1.1*xMaxPDLoc[iRich],
                       nBins,0,1.05 );

    title = radiator[iRad]+" HPD acceptance (electron) V y";
    id = radOffset*(iRad+1) + 9;
    m_geomEffVy[iRad] =
      histoSvc()->book(m_histPth,id,title,
                       nBins,1.1*yMinPDLoc[iRich],1.1*yMaxPDLoc[iRich],
                       nBins,0,1.05 );

  } // end radiator loop

  return StatusCode::SUCCESS;
}

const StatusCode RichTrackGeomMoni::bookMCHistograms()
{

  std::string title;
  int id;
  const int nBins = 100;

  // Defines for various parameters
  RAD_HISTO_OFFSET;
  RADIATOR_NAMES;
  RADIATOR_GLOBAL_POSITIONS;

  for ( int iRad = 0; iRad < Rich::NRadiatorTypes; ++iRad ) {

    title = "MC Track entrance z : "+radiator[iRad];
    id = radOffset*(iRad+1) + 1;
    m_mctrEntrZ[iRad] =
      histoSvc()->book(m_mcHistPth,id,title,nBins,zRadEntGlo[iRad],zRadExitGlo[iRad]);

    title = "MC Track exit z : "+radiator[iRad];
    id = radOffset*(iRad+1) + 2;
    m_mctrExitZ[iRad] =
      histoSvc()->book(m_mcHistPth,id,title,nBins,zRadEntGlo[iRad],zRadExitGlo[iRad]);

    title = "MC Track entrance yVx : "+radiator[iRad];
    id = radOffset*(iRad+1) + 3;
    m_mctrEntrXY[iRad] =
      histoSvc()->book(m_mcHistPth,id,title,
                       nBins,-xRadEntGlo[iRad],xRadEntGlo[iRad],
                       nBins,-yRadEntGlo[iRad],yRadEntGlo[iRad]);

    title = "MC Track exit yVx : "+radiator[iRad];
    id = radOffset*(iRad+1) + 4;
    m_mctrExitXY[iRad] =
      histoSvc()->book(m_mcHistPth,id,title,
                       nBins,-xRadExitGlo[iRad],xRadExitGlo[iRad],
                       nBins,-yRadExitGlo[iRad],yRadExitGlo[iRad]);

    title = "MC Track length : "+radiator[iRad];
    id = radOffset*(iRad+1) + 5;
    m_mctrLength[iRad] =
      histoSvc()->book(m_mcHistPth,id,title,
                       nBins,zRadLenMin[iRad],zRadLenMax[iRad]);

  } // radiator loop

  return StatusCode::SUCCESS;
}

// Main execution
StatusCode RichTrackGeomMoni::execute() {

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

  // Iterate over segments
  for ( RichRecSegments::const_iterator iSeg = richSegments()->begin();
        iSeg != richSegments()->end(); ++iSeg ) {
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
    const HepPoint3D & pdPoint    = segment->pdPanelHitPoint();
    const HepPoint3D & pdPointLoc = segment->pdPanelHitPointLocal();

    // Pointer to parent MCParticle
    const MCParticle * trackMCPart =
      m_richRecMCTruth->mcParticle( segment->richRecTrack() );

    // radiator entry/exit information
    const HepVector3D & trackDir = trackSeg.bestMomentum();
    m_trEntrZ[iRad]->fill( trackSeg.entryPoint().z() );

    m_trExitZ[iRad]->fill( trackSeg.exitPoint().z() );
    m_trEntrXY[iRad]->fill( trackSeg.entryPoint().x(),
                            trackSeg.entryPoint().y() );
    m_trExitXY[iRad]->fill( trackSeg.exitPoint().x(),
                            trackSeg.exitPoint().y() );
    m_trLength[iRad]->fill( trackSeg.pathLength() );

    // Ray traced hit point on PDPanel
    m_projPDPanelHitsXGlo[iRich]->fill( pdPoint.x() );
    m_projPDPanelHitsYGlo[iRich]->fill( pdPoint.y() );
    m_projPDPanelHitsZGlo[iRich]->fill( pdPoint.z() );
    m_projPDPanelHitsXYGlo[iRich]->fill( pdPoint.x(), pdPoint.y() );
    m_projPDPanelHitsXZGlo[iRich]->fill( pdPoint.z(), pdPoint.x() );
    m_projPDPanelHitsYZGlo[iRich]->fill( pdPoint.z(), pdPoint.y() );
    m_projPDPanelHitsXLoc[iRich]->fill( pdPointLoc.x() );
    m_projPDPanelHitsYLoc[iRich]->fill( pdPointLoc.y() );
    m_projPDPanelHitsZLoc[iRich]->fill( pdPointLoc.z() );
    m_projPDPanelHitsXYLoc[iRich]->fill( pdPointLoc.x(), pdPointLoc.y() );

    // HPD panel acceptance
    const double hpdPanAcc = m_geomTool->hpdPanelAcceptance(segment,Rich::Electron);
    m_hpdPanAccVx[iRad]->fill( pdPointLoc.x(), hpdPanAcc );
    m_hpdPanAccVy[iRad]->fill( pdPointLoc.y(), hpdPanAcc );

    // HPD panel acceptance
    const double hpdAcc = m_geomEffic->geomEfficiency(segment,Rich::Electron);
    m_geomEffVx[iRad]->fill( pdPointLoc.x(), hpdAcc );
    m_geomEffVy[iRad]->fill( pdPointLoc.y(), hpdAcc );

    // Get associated RichMCSegment
    const MCRichSegment * mcSegment = m_richRecMCTruth->mcRichSegment(segment);
    if ( mcSegment ) {
      ++nMCSegs[iRad]; // count MC segments per radiator

      // entry/exit coordinates
      m_mctrEntrZ[iRad]->fill( mcSegment->entryPoint().z() );
      m_mctrExitZ[iRad]->fill( mcSegment->exitPoint().z() );
      m_mctrEntrXY[iRad]->fill( mcSegment->entryPoint().x(),
                                mcSegment->entryPoint().y() );
      m_mctrExitXY[iRad]->fill( mcSegment->exitPoint().x(),
                                mcSegment->exitPoint().y() );
      m_mctrLength[iRad]->fill( mcSegment->pathLength() );

    }

    // Project track direction to active detector plane and histogram hits
    RichGeomPhoton photon;
    if ( m_rayTrace->traceToDetector( trackSeg.rich(),
                                      trackSeg.bestPoint(),
                                      trackDir,
                                      photon,
                                      m_traceMode ) != 0 ) {

      // Ray traced hit point on PDs, global
      const HepPoint3D & photPoint = photon.detectionPoint();
      m_projPDHitsXGlo[iRich]->fill( photPoint.x() );
      m_projPDHitsYGlo[iRich]->fill( photPoint.y() );
      m_projPDHitsZGlo[iRich]->fill( photPoint.z() );
      m_projPDHitsXYGlo[iRich]->fill( photPoint.x(), photPoint.y() );
      m_projPDHitsXZGlo[iRich]->fill( photPoint.z(), photPoint.x() );
      m_projPDHitsYZGlo[iRich]->fill( photPoint.z(), photPoint.y() );

      // Ray traced hit point on PDs, local
      m_projPDHitsXLoc[iRich]->fill( pdPointLoc.x() );
      m_projPDHitsYLoc[iRich]->fill( pdPointLoc.y() );
      m_projPDHitsZLoc[iRich]->fill( pdPointLoc.z() );
      m_projPDHitsXYLoc[iRich]->fill( pdPointLoc.x(), pdPointLoc.y() );

      // Ray traced reflection point on spherical mirror
      const HepPoint3D & photSpeMir = photon.sphMirReflectionPoint();
      m_projSpheHitsXGlo[iRich]->fill( photSpeMir.x() );
      m_projSpheHitsYGlo[iRich]->fill( photSpeMir.y() );
      m_projSpheHitsZGlo[iRich]->fill( photSpeMir.z() );
      m_projSpheHitsXYGlo[iRich]->fill( photSpeMir.x(), photSpeMir.y() );
      m_projSpheHitsXZGlo[iRich]->fill( photSpeMir.z(), photSpeMir.x() );
      m_projSpheHitsYZGlo[iRich]->fill( photSpeMir.z(), photSpeMir.y() );

      // Ray traced reflection point on flat mirror
      const HepPoint3D & photFlatMir = photon.flatMirReflectionPoint();
      m_projFlatHitsXGlo[iRich]->fill( photFlatMir.x() );
      m_projFlatHitsYGlo[iRich]->fill( photFlatMir.y() );
      m_projFlatHitsZGlo[iRich]->fill( photFlatMir.z() );
      m_projFlatHitsXYGlo[iRich]->fill( photFlatMir.x(), photFlatMir.y() );
      m_projFlatHitsXZGlo[iRich]->fill( photFlatMir.z(), photFlatMir.x() );
      m_projFlatHitsYZGlo[iRich]->fill( photFlatMir.z(), photFlatMir.y() );

      // find out pd positions
      m_xHits[photon.smartID().pdID()] += photPoint.x();
      m_yHits[photon.smartID().pdID()] += photPoint.y();
      m_zHits[photon.smartID().pdID()] += photPoint.z();
      m_hitCount[photon.smartID().pdID()] += 1;

    }

    // Finally, some debug printout
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "Segment " << segment->key() << " " << iRad << " : "
              << rad << " : TrTrackType " << trType << " : pTot "
              << segment->trackSegment().bestMomentum().mag()
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

const StatusCode RichTrackGeomMoni::bookFinalHistograms() {

  // book pd number histograms
  RICH_HISTO_OFFSET;
  RICH_NAMES;
  PD_GLOBAL_POSITIONS;
  const std::string panel[] = { "panel0", "panel1" };
  int nBins = 25;
  int id;
  std::string title;

  for ( int iRich = 0; iRich<2; ++iRich ) {
    for ( int iPan = 0; iPan<2; ++iPan ) {

      title = rich[iRich]+" "+panel[iPan]+" PDCol yVx";
      id = richOffset*(iRich+1) + 101 +iPan*3;
      m_pdColNumXY[iRich][iPan] =
        histoSvc()->book(m_histPth,id,title,
                         nBins,xMinPDGlo[iRich],xMaxPDGlo[iRich],
                         nBins,yMinPDGlo[iRich],yMaxPDGlo[iRich]);

      title = rich[iRich]+" "+panel[iPan]+" PDCol xVz";
      id = richOffset*(iRich+1) + 102 +iPan*3;
      m_pdColNumXZ[iRich][iPan] =
        histoSvc()->book(m_histPth,id,title,
                         nBins,zMinPDGlo[iRich],zMaxPDGlo[iRich],
                         nBins,xMinPDGlo[iRich],xMaxPDGlo[iRich]);

      title = rich[iRich]+" "+panel[iPan]+" PDCol yVz";
      id = richOffset*(iRich+1) + 103 +iPan*3;
      m_pdColNumYZ[iRich][iPan] =
        histoSvc()->book(m_histPth,id,title,
                         nBins,zMinPDGlo[iRich],zMaxPDGlo[iRich],
                         nBins,yMinPDGlo[iRich],yMaxPDGlo[iRich]);

      title = rich[iRich]+" "+panel[iPan]+" PDRow yVx";
      id = richOffset*(iRich+1) + 201 +iPan*3;
      m_pdRowNumXY[iRich][iPan] =
        histoSvc()->book(m_histPth,id,title,
                         nBins,xMinPDGlo[iRich],xMaxPDGlo[iRich],
                         nBins,yMinPDGlo[iRich],yMaxPDGlo[iRich]);

      title = rich[iRich]+" "+panel[iPan]+" PDRow xVz";
      id = richOffset*(iRich+1) + 202 +iPan*3;
      m_pdRowNumXZ[iRich][iPan] =
        histoSvc()->book(m_histPth,id,title,
                         nBins,zMinPDGlo[iRich],zMaxPDGlo[iRich],
                         nBins,xMinPDGlo[iRich],xMaxPDGlo[iRich]);

      title = rich[iRich]+" "+panel[iPan]+" PDRow yVz";
      id = richOffset*(iRich+1) + 203 +iPan*3;
      m_pdRowNumYZ[iRich][iPan] =
        histoSvc()->book(m_histPth,id,title,
                         nBins,zMinPDGlo[iRich],zMaxPDGlo[iRich],
                         nBins,yMinPDGlo[iRich],yMaxPDGlo[iRich]);

    }
  } // rich loop

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode RichTrackGeomMoni::finalize()
{
  // Book final histograms
  if ( !bookFinalHistograms() ) return StatusCode::FAILURE;

  debug() << "Track PD Positions :-" << endreq;
  for ( RichMap<RichSmartID, double>::const_iterator ix = m_xHits.begin();
        ix != m_xHits.end(); ++ix )
  {
    const int hits = m_hitCount[ix->first];
    const double avX = ( hits>0 ? ix->second/hits : 0 );
    const double avY = ( hits>0 ? m_yHits[ix->first]/hits : 0 );
    const double avZ = ( hits>0 ? m_zHits[ix->first]/hits : 0 );
    debug() << "PD " << ix->first << " Pos. " << HepPoint3D(avX,avY,avZ)
            << " Hits " << hits << endreq;
    const Rich::DetectorType iRich = ( avZ>5000 ? Rich::Rich2 : Rich::Rich1 );
    m_pdColNumXY[iRich][ix->first.panel()]->fill( avX, avY, ix->first.pdCol() );
    m_pdColNumXZ[iRich][ix->first.panel()]->fill( avZ, avX, ix->first.pdCol() );
    m_pdColNumYZ[iRich][ix->first.panel()]->fill( avZ, avY, ix->first.pdCol() );
    m_pdRowNumXY[iRich][ix->first.panel()]->fill( avX, avY, ix->first.pdRow() );
    m_pdRowNumXZ[iRich][ix->first.panel()]->fill( avZ, avX, ix->first.pdRow() );
    m_pdRowNumYZ[iRich][ix->first.panel()]->fill( avZ, avY, ix->first.pdRow() );
  }

  // Execute base class method
  return RichRecMoniAlgBase::finalize();
}
