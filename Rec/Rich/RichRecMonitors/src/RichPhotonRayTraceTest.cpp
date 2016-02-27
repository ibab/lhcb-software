
//---------------------------------------------------------------------------
/** @file RichPhotonRayTraceTest.cpp
 *
 *  Implementation file for algorithm class : RichPhotonRayTraceTest
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

// local
#include "RichPhotonRayTraceTest.h"

// From Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// namespace
using namespace Rich::Rec::MC;

//---------------------------------------------------------------------------

// Standard constructor, initializes variables
PhotonRayTraceTest::PhotonRayTraceTest( const std::string& name,
                                        ISvcLocator* pSvcLocator )
  : Rich::Rec::TupleAlgBase ( name, pSvcLocator ) { }

// Destructor
PhotonRayTraceTest::~PhotonRayTraceTest() {}

//  Initialize
StatusCode PhotonRayTraceTest::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = Rich::Rec::TupleAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRecMCTruthTool", m_richRecMCTruth );
  acquireTool( "TrackSelector",   m_trSelector, this );
  acquireTool( "RichRayTracing",  m_rayTrace,    nullptr, true );
  acquireTool( "RichSmartIDTool", m_smartIDTool, nullptr, true );

  return sc;
}

// Main execution
StatusCode PhotonRayTraceTest::execute()
{

  // Check event status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Iterate over segments
  for ( auto * segment : *richSegments() )
  {

    // apply track selection
    if ( !m_trSelector->trackSelected(segment->richRecTrack()) ) continue;

    // loop over photons
    const auto & photons = photonCreator()->reconstructPhotons( segment );
    for ( const auto * photon : photons )
    {

      // ray trace with the simple HPD mode
      rayTrace( photon,
                LHCb::RichTraceMode( LHCb::RichTraceMode::RespectHPDTubes,
                                     LHCb::RichTraceMode::SimpleHPDs ),
                "SimpleHPDs" );

      // ray trace with the full HPD mode
      rayTrace( photon,
                LHCb::RichTraceMode( LHCb::RichTraceMode::RespectHPDTubes,
                                     LHCb::RichTraceMode::FullHPDs ),
                "FullHPDs" );

    } // loop over segment photons

  } // loop over segments

  return StatusCode::SUCCESS;
}

void PhotonRayTraceTest::rayTrace( const LHCb::RichRecPhoton * photon,
                                   const LHCb::RichTraceMode mode,
                                   const std::string & name )
{
  // Histogramming
  const Rich::HistoID hid;
  //PD_GLOBAL_POSITIONS_X;
  //PD_GLOBAL_POSITIONS_Y;
  PD_LOCAL_POSITIONS_X;
  PD_LOCAL_POSITIONS_Y;
  SPHERICAL_MIRROR_GLOBAL_POSITIONS_X;
  SPHERICAL_MIRROR_GLOBAL_POSITIONS_Y;
  FLAT_MIRROR_GLOBAL_POSITIONS_X;
  FLAT_MIRROR_GLOBAL_POSITIONS_Y;

  // Reco photon
  const auto & recoPhot = photon->geomPhoton();
  // track segment
  const auto & tkSeg = photon->richRecSegment()->trackSegment();

  // RICH and Radiator info
  const auto rich = tkSeg.rich();
  //const auto rad  = tkSeg.radiator();

  // Ray trace a photon based on the reconstructed photon parameters
  // Note this only works if emissionDir is well defined, which is only
  // the case for the Quartic reconstruction method. This is is therefore
  // designed to compare the ray tracing to this method.
  LHCb::RichGeomPhoton rtPhot;
  const auto result = m_rayTrace->traceToDetector( rich,
                                                   recoPhot.emissionPoint(),
                                                   recoPhot.emissionDir(),
                                                   rtPhot, tkSeg,
                                                   mode, Rich::top );

  richHisto1D( hid(rich,name+"/RayTraceStatus"), name+" Ray Trace StatusCode",
               -0.5, 1.5, 2 ) -> fill( mode.traceWasOK(result) );

  // Only look at photons where the same mirror segments are used.
  if ( rtPhot.primaryMirror()   != recoPhot.primaryMirror()   ||
       rtPhot.secondaryMirror() != recoPhot.secondaryMirror() ) { return; }

  if ( mode.traceWasOK(result) )
  {

    // primary mirror reflection point
    const auto & recoPhotPrimMirr = recoPhot.sphMirReflectionPoint();
    const auto &   rtPhotPrimMirr =   rtPhot.sphMirReflectionPoint();
    const auto xDiffPrimMirr = recoPhotPrimMirr.x() - rtPhotPrimMirr.x();
    const auto yDiffPrimMirr = recoPhotPrimMirr.y() - rtPhotPrimMirr.y();
    const auto zDiffPrimMirr = recoPhotPrimMirr.z() - rtPhotPrimMirr.z();

    richHisto1D( hid(rich,name+"/GloPrimMirrXDiff"),
                 name+" Global Prim Mirr | Reco X - RayTrace X",
                 -0.2, 0.2, nBins1D() ) -> fill( xDiffPrimMirr );
    richHisto1D( hid(rich,name+"/GloPrimMirrYDiff"),
                 name+" Global Prim Mirr | Reco Y - RayTrace Y",
                 -0.2, 0.2, nBins1D() ) -> fill( yDiffPrimMirr );
    richHisto1D( hid(rich,name+"/GloPrimMirrZDiff"),
                 name+" Global Prim Mirr | Reco Z - RayTrace Z",
                 -0.2, 0.2, nBins1D() ) -> fill( zDiffPrimMirr );

    richProfile2D( hid(rich,name+"/GloPrimMirrXDiffVXY"),
                   name+" Global Prim Mirr | Reco X - RayTrace X V (x,y)",
                   xMinSpheGlo[rich], xMaxSpheGlo[rich], nBins1D(),
                   yMinSpheGlo[rich], yMaxSpheGlo[rich], nBins1D() )
      -> fill( recoPhotPrimMirr.x(), recoPhotPrimMirr.y(), xDiffPrimMirr );
    richProfile2D( hid(rich,name+"/GloPrimMirrYDiffVXY"),
                   name+" Global Prim Mirr | Reco Y - RayTrace Y V (x,y)",
                   xMinSpheGlo[rich], xMaxSpheGlo[rich], nBins1D(),
                   yMinSpheGlo[rich], yMaxSpheGlo[rich], nBins1D() )
      -> fill( recoPhotPrimMirr.x(), recoPhotPrimMirr.y(), yDiffPrimMirr );
    richProfile2D( hid(rich,name+"/GloPrimMirrZDiffVXY"),
                   name+" Global Prim Mirr | Reco Z - RayTrace Z V (x,y)",
                   xMinSpheGlo[rich], xMaxSpheGlo[rich], nBins1D(),
                   yMinSpheGlo[rich], yMaxSpheGlo[rich], nBins1D() )
      -> fill( recoPhotPrimMirr.x(), recoPhotPrimMirr.y(), zDiffPrimMirr );

    // secondary mirror reflection point
    const auto & recoPhotSecMirr  = recoPhot.flatMirReflectionPoint();
    const auto &   rtPhotSecMirr  =   rtPhot.flatMirReflectionPoint();
    const auto xDiffSecMirr = recoPhotSecMirr.x() - rtPhotSecMirr.x();
    const auto yDiffSecMirr = recoPhotSecMirr.y() - rtPhotSecMirr.y();
    const auto zDiffSecMirr = recoPhotSecMirr.z() - rtPhotSecMirr.z();

    richHisto1D( hid(rich,name+"/GloSecMirrXDiff"),
                 name+" Global Sec Mirr | Reco X - RayTrace X",
                 -2, 2, nBins1D() ) -> fill( xDiffSecMirr );
    richHisto1D( hid(rich,name+"/GloSecMirrYDiff"),
                 name+" Global Sec Mirr | Reco Y - RayTrace Y",
                 -2, 2, nBins1D() ) -> fill( yDiffSecMirr );
    richHisto1D( hid(rich,name+"/GloSecMirrZDiff"),
                 name+" Global Sec Mirr | Reco Z - RayTrace Z",
                 -2, 2, nBins1D() ) -> fill( zDiffSecMirr );

    richProfile2D( hid(rich,name+"/GloSecMirrXDiffVXY"),
                   name+" Global Sec Mirr | Reco X - RayTrace X V (x,y)",
                   xMinFlatGlo[rich], xMaxFlatGlo[rich], nBins1D(),
                   yMinFlatGlo[rich], yMaxFlatGlo[rich], nBins1D() )
      -> fill( recoPhotSecMirr.x(), recoPhotSecMirr.y(), xDiffSecMirr );
    richProfile2D( hid(rich,name+"/GloSecMirrYDiffVXY"),
                   name+" Global Sec Mirr | Reco Y - RayTrace Y V (x,y)",
                   xMinFlatGlo[rich], xMaxFlatGlo[rich], nBins1D(),
                   yMinFlatGlo[rich], yMaxFlatGlo[rich], nBins1D() )
      -> fill( recoPhotSecMirr.x(), recoPhotSecMirr.y(), yDiffSecMirr );
    richProfile2D( hid(rich,name+"/GloSecMirrZDiffVXY"),
                   name+" Global Sec Mirr | Reco Z - RayTrace Z V (x,y)",
                   xMinFlatGlo[rich], xMaxFlatGlo[rich], nBins1D(),
                   yMinFlatGlo[rich], yMaxFlatGlo[rich], nBins1D() )
      -> fill( recoPhotSecMirr.x(), recoPhotSecMirr.y(), zDiffSecMirr );

    // hit points in global coordinates
    const auto & recoPhotHitGlo = recoPhot.detectionPoint();
    const auto &   rtPhotHitGlo =   rtPhot.detectionPoint();
    const auto xDiffHitGlo = recoPhotHitGlo.x() - rtPhotHitGlo.x();
    const auto yDiffHitGlo = recoPhotHitGlo.y() - rtPhotHitGlo.y();
    const auto zDiffHitGlo = recoPhotHitGlo.z() - rtPhotHitGlo.z();

    richHisto1D( hid(rich,name+"/GloHitXDiff"),
                 name+" Global PD hit | Reco X - RayTrace X",
                 -10, 10, nBins1D() ) -> fill( xDiffHitGlo );
    richHisto1D( hid(rich,name+"/GloHitYDiff"),
                 name+" Global PD hit | Reco Y - RayTrace Y",
                 -10, 10, nBins1D() ) -> fill( yDiffHitGlo );
    richHisto1D( hid(rich,name+"/GloHitZDiff"),
                 name+" Global PD hit | Reco Z - RayTrace Z",
                 -10, 10, nBins1D() ) -> fill( zDiffHitGlo );

    // hit points in local coordinates
    const auto recoPhotHitLoc = m_smartIDTool->globalToPDPanel(recoPhotHitGlo);
    const auto   rtPhotHitLoc = m_smartIDTool->globalToPDPanel(rtPhotHitGlo);
    const auto xDiffHitLoc = recoPhotHitLoc.x() - rtPhotHitLoc.x();
    const auto yDiffHitLoc = recoPhotHitLoc.y() - rtPhotHitLoc.y();
    const auto zDiffHitLoc = recoPhotHitLoc.z() - rtPhotHitLoc.z();

    richHisto1D( hid(rich,name+"/LocHitXDiff"),
                 name+" Local PD hit | Reco X - RayTrace X",
                 -3, 3, nBins1D() ) -> fill( xDiffHitLoc );
    richHisto1D( hid(rich,name+"/LocHitYDiff"),
                 name+" Local PD hit | Reco Y - RayTrace Y",
                 -3, 3, nBins1D() ) -> fill( yDiffHitLoc );
    richHisto1D( hid(rich,name+"/LocHitZDiff"),
                 name+" Local PD hit | Reco Z - RayTrace Z",
                 -10, 10, nBins1D() ) -> fill( zDiffHitLoc );

    richProfile2D( hid(rich,name+"/LocHitXDiffVXY"),
                   name+" Local PD hit | Reco X - RayTrace X V (x,y)",
                   xMinPDLoc[rich], xMaxPDLoc[rich], nBins1D(),
                   yMinPDLoc[rich], yMaxPDLoc[rich], nBins1D() )
      -> fill( recoPhotHitLoc.x(), recoPhotHitLoc.y(), xDiffHitLoc );
    richProfile2D( hid(rich,name+"/LocHitYDiffVXY"),
                   name+" Local PD hit | Reco Y - RayTrace Y V (x,y)",
                   xMinPDLoc[rich], xMaxPDLoc[rich], nBins1D(),
                   yMinPDLoc[rich], yMaxPDLoc[rich], nBins1D() )
      -> fill( recoPhotHitLoc.x(), recoPhotHitLoc.y(), yDiffHitLoc );
    richProfile2D( hid(rich,name+"/LocHitZDiffVXY"),
                   name+" Local PD hit | Reco Z - RayTrace Z V (x,y)",
                   xMinPDLoc[rich], xMaxPDLoc[rich], nBins1D(),
                   yMinPDLoc[rich], yMaxPDLoc[rich], nBins1D() )
      -> fill( recoPhotHitLoc.x(), recoPhotHitLoc.y(), zDiffHitLoc );

    // Associated MCRichOpticalPhoton
    const auto * mcPhoton = m_richRecMCTruth->trueOpticalPhoton(photon);
    if ( mcPhoton )
    {

      // MC coordinates
      const auto & mcPrimMirrGlo = mcPhoton->sphericalMirrorReflectPoint();
      const auto & mcSecMirrGlo  = mcPhoton->flatMirrorReflectPoint();
      const auto & mcPhotHitGlo  = mcPhoton->pdIncidencePoint();
      const auto   mcPhotHitLoc  = m_smartIDTool->globalToPDPanel(mcPhotHitGlo);

      const auto mcRecoGloPrimMirrXDiff = recoPhotPrimMirr.x() - mcPrimMirrGlo.x();
      const auto mcRecoGloPrimMirrYDiff = recoPhotPrimMirr.y() - mcPrimMirrGlo.y();
      const auto mcRecoGloPrimMirrZDiff = recoPhotPrimMirr.z() - mcPrimMirrGlo.z();

      richHisto1D( hid(rich,name+"/mcRecoGloPrimMirrXDiff"),
                   name+" Global Prim Mirr | Reco X - MC X",
                   -4, 4, nBins1D() ) -> fill( mcRecoGloPrimMirrXDiff );
      richHisto1D( hid(rich,name+"/mcRecoGloPrimMirrYDiff"),
                   name+" Global Prim Mirr | Reco Y - MC Y",
                   -4, 4, nBins1D() ) -> fill( mcRecoGloPrimMirrYDiff );
      richHisto1D( hid(rich,name+"/mcRecoGloPrimMirrZDiff"),
                   name+" Global Prim Mirr | Reco Z - MC Z",
                   -4, 4, nBins1D() ) -> fill( mcRecoGloPrimMirrZDiff );

      const auto mcRayTGloPrimMirrXDiff = rtPhotPrimMirr.x() - mcPrimMirrGlo.x();
      const auto mcRayTGloPrimMirrYDiff = rtPhotPrimMirr.y() - mcPrimMirrGlo.y();
      const auto mcRayTGloPrimMirrZDiff = rtPhotPrimMirr.z() - mcPrimMirrGlo.z();

      richHisto1D( hid(rich,name+"/mcRayTGloPrimMirrXDiff"),
                   name+" Global Prim Mirr | RayT X - MC X",
                   -4, 4, nBins1D() ) -> fill( mcRayTGloPrimMirrXDiff );
      richHisto1D( hid(rich,name+"/mcRayTGloPrimMirrYDiff"),
                   name+" Global Prim Mirr | RayT Y - MC Y",
                   -4, 4, nBins1D() ) -> fill( mcRayTGloPrimMirrYDiff );
      richHisto1D( hid(rich,name+"/mcRayTGloPrimMirrZDiff"),
                   name+" Global Prim Mirr | RayT Z - MC Z",
                   -4, 4, nBins1D() ) -> fill( mcRayTGloPrimMirrZDiff );

      const auto mcRecoGloSecMirrXDiff = recoPhotSecMirr.x() - mcSecMirrGlo.x();
      const auto mcRecoGloSecMirrYDiff = recoPhotSecMirr.y() - mcSecMirrGlo.y();
      const auto mcRecoGloSecMirrZDiff = recoPhotSecMirr.z() - mcSecMirrGlo.z();

      richHisto1D( hid(rich,name+"/mcRecoGloSecMirrXDiff"),
                   name+" Global Sec Mirr | Reco X - MC X",
                   -4, 4, nBins1D() ) -> fill( mcRecoGloSecMirrXDiff );
      richHisto1D( hid(rich,name+"/mcRecoGloSecMirrYDiff"),
                   name+" Global Sec Mirr | Reco Y - MC Y",
                   -4, 4, nBins1D() ) -> fill( mcRecoGloSecMirrYDiff );
      richHisto1D( hid(rich,name+"/mcRecoGloSecMirrZDiff"),
                   name+" Global Sec Mirr | Reco Z - MC Z",
                   -4, 4, nBins1D() ) -> fill( mcRecoGloSecMirrZDiff );

      const auto mcRayTGloSecMirrXDiff = rtPhotSecMirr.x() - mcSecMirrGlo.x();
      const auto mcRayTGloSecMirrYDiff = rtPhotSecMirr.y() - mcSecMirrGlo.y();
      const auto mcRayTGloSecMirrZDiff = rtPhotSecMirr.z() - mcSecMirrGlo.z();

      richHisto1D( hid(rich,name+"/mcRayTGloSecMirrXDiff"),
                   name+" Global Sec Mirr | RayT X - MC X",
                   -4, 4, nBins1D() ) -> fill( mcRayTGloSecMirrXDiff );
      richHisto1D( hid(rich,name+"/mcRayTGloSecMirrYDiff"),
                   name+" Global Sec Mirr | RayT Y - MC Y",
                   -4, 4, nBins1D() ) -> fill( mcRayTGloSecMirrYDiff );
      richHisto1D( hid(rich,name+"/mcRayTGloSecMirrZDiff"),
                   name+" Global Sec Mirr | RayT Z - MC Z",
                   -4, 4, nBins1D() ) -> fill( mcRayTGloSecMirrZDiff );

      const auto mcRecoLocPhotHitXDiff = recoPhotHitLoc.x() - mcPhotHitLoc.x();
      const auto mcRecoLocPhotHitYDiff = recoPhotHitLoc.y() - mcPhotHitLoc.y();
      const auto mcRecoLocPhotHitZDiff = recoPhotHitLoc.z() - mcPhotHitLoc.z();

      richHisto1D( hid(rich,name+"/mcRecoLocPhotHitXDiff"),
                   name+" Local PD hit | Reco X - MC X",
                   -5, 5, nBins1D() ) -> fill( mcRecoLocPhotHitXDiff );
      richHisto1D( hid(rich,name+"/mcRecoLocPhotHitYDiff"),
                   name+" Local PD hit | Reco Y - MC Y",
                   -5, 5, nBins1D() ) -> fill( mcRecoLocPhotHitYDiff );
      richHisto1D( hid(rich,name+"/mcRecoLocPhotHitZDiff"),
                   name+" Local PD hit | Reco Z - MC Z",
                   -10, 10, nBins1D() ) -> fill( mcRecoLocPhotHitZDiff );

      const auto mcRayTLocPhotHitXDiff = rtPhotHitLoc.x() - mcPhotHitLoc.x();
      const auto mcRayTLocPhotHitYDiff = rtPhotHitLoc.y() - mcPhotHitLoc.y();
      const auto mcRayTLocPhotHitZDiff = rtPhotHitLoc.z() - mcPhotHitLoc.z();

      richHisto1D( hid(rich,name+"/mcRayTLocPhotHitXDiff"),
                   name+" Local PD hit | RayT X - MC X",
                   -5, 5, nBins1D() ) -> fill( mcRayTLocPhotHitXDiff );
      richHisto1D( hid(rich,name+"/mcRayTLocPhotHitYDiff"),
                   name+" Local PD hit | RayT Y - MC Y",
                   -5, 5, nBins1D() ) -> fill( mcRayTLocPhotHitYDiff );
      richHisto1D( hid(rich,name+"/mcRayTLocPhotHitZDiff"),
                   name+" Local PD hit | RayT Z - MC Z",
                   -10, 10, nBins1D() ) -> fill( mcRayTLocPhotHitZDiff );


    } // MC photon OK

  } // ray tracing OK

}

//---------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( PhotonRayTraceTest )

//---------------------------------------------------------------------------
