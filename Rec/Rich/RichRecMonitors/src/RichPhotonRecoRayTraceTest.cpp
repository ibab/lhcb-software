
//---------------------------------------------------------------------------
/** @file RichPhotonRecoRayTraceTest.cpp
 *
 *  Implementation file for algorithm class : Rich::Rec::MC::PhotonRecoRayTraceTest
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   03/01/2008
 */
//---------------------------------------------------------------------------

// local
#include "RichPhotonRecoRayTraceTest.h"

//---------------------------------------------------------------------------

// namespace
using namespace Rich::Rec::MC;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PhotonRecoRayTraceTest )

// Standard constructor, initializes variables
PhotonRecoRayTraceTest::PhotonRecoRayTraceTest( const std::string& name,
                                                ISvcLocator* pSvcLocator )
  : HistoAlgBase( name, pSvcLocator )
{
  // JOs
  declareProperty( "RefractionCorrection", m_refractCorr = true  );
  declareProperty( "FullHPDsInRayTracing", m_fullHPDs    = false );
  declareProperty( "RejectAmbiguousPhotons", m_rejectAmbigPhots = false );
  // number of bins
  setProperty( "NBins1DHistos", 200 );
  setProperty( "NBins2DHistos", 100 );
}

// Destructor
PhotonRecoRayTraceTest::~PhotonRecoRayTraceTest() {}

//  Initialize
StatusCode PhotonRecoRayTraceTest::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = HistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRecMCTruthTool",   m_richRecMCTruth   );
  acquireTool( "RichCherenkovAngle",   m_ckAngle          );
  acquireTool( "TrackSelector",        m_trSelector, this );
  acquireTool( "RichRayTracing",       m_rayTracing, nullptr, true );
  acquireTool( "RichSmartIDTool",      m_idTool,     nullptr, true );

  // ray tracing options
  m_mode = LHCb::RichTraceMode( LHCb::RichTraceMode::RespectHPDTubes,
                                ( m_fullHPDs ?
                                  LHCb::RichTraceMode::FullHPDs   :
                                  LHCb::RichTraceMode::SimpleHPDs ) );
  m_mode.setAeroRefraction(m_refractCorr);
  info() << "Ray tracing : " << m_mode << endmsg;

  return sc;
}

// Main execution
StatusCode PhotonRecoRayTraceTest::execute()
{
  // Check event status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Iterate over segments
  for ( auto * segment : *richSegments() )
  {

    // apply track selection
    if ( !m_trSelector->trackSelected(segment->richRecTrack()) ) continue;

    // MC type
    const Rich::ParticleIDType mcType = m_richRecMCTruth->mcParticleType( segment );
    if ( mcType == Rich::Unknown ) continue;

    // Loop over photons for this segment
    const auto & photons = photonCreator()->reconstructPhotons( segment );
    for ( auto * photon : photons )
    {
      // photon selection
      if ( m_rejectAmbigPhots && !photon->geomPhoton().unambiguousPhoton() ) continue;

      // Make plots against reco photon
      makePlots( "Reco/",
                 photon->geomPhoton().emissionPoint(),
                 photon->geomPhoton().emissionDir(),
                 photon->geomPhoton().detectionPoint(),
                 photon->geomPhoton().sphMirReflectionPoint(),
                 photon->geomPhoton().flatMirReflectionPoint(),
                 segment->trackSegment().rich(),
                 segment->trackSegment().radiator(),
                 photon->geomPhoton().CherenkovPhi(),
                 segment->trackSegment().avPhotonEnergy() );

      // Locate MC photon
      const LHCb::MCRichOpticalPhoton * mcPhot = m_richRecMCTruth->trueOpticalPhoton(photon);
      if ( mcPhot )
      {
        // Make plots against MC photon
        const Gaudi::XYZPoint & vEnd = ( mcPhot->mcRichHit()->radiator() == Rich::Aerogel ?
                                         mcPhot->aerogelExitPoint() : 
                                         mcPhot->sphericalMirrorReflectPoint() );
        makePlots( "MC/",
                   mcPhot->emissionPoint(),
                   vEnd - mcPhot->emissionPoint(),
                   mcPhot->pdIncidencePoint(),
                   mcPhot->sphericalMirrorReflectPoint(),
                   mcPhot->flatMirrorReflectPoint(),
                   mcPhot->mcRichHit()->rich(),
                   mcPhot->mcRichHit()->radiator(),
                   mcPhot->cherenkovPhi(),
                   mcPhot->energyAtProduction() );
      }

    } // photons for each segment

  } // segments

  return StatusCode::SUCCESS;
}

void PhotonRecoRayTraceTest::makePlots( const std::string & type,
                                        const Gaudi::XYZPoint  & emissPoint,
                                        const Gaudi::XYZVector & emissDir,
                                        const Gaudi::XYZPoint  & detectPoint,
                                        const Gaudi::XYZPoint  & primMirrPnt,
                                        const Gaudi::XYZPoint  & secMirrPnt,
                                        const Rich::DetectorType rich,
                                        const Rich::RadiatorType rad,
                                        const double ckPhi,
                                        const double photonEnergy )
{
  // Histogramming
  const Rich::HistoID hid;
  //                       Aero  C4F10  CF4
  const double pRLoc[]  = { 0.6,  0.5,   1     };
  const double pRGlo[]  = { 1,    1,     3     };
  const double primMR[] = { 0.5,  0.001, 0.001 };
  const double secMR[]  = { 0.2,  0.03,  0.3   };

  // Ray trace with these photon parameters
  LHCb::RichGeomPhoton rayTracedPhoton;
  const LHCb::RichTraceMode::RayTraceResult result
    = m_rayTracing->traceToDetector( rich,
                                     emissPoint,
                                     emissDir,
                                     rayTracedPhoton,
                                     m_mode, Rich::top,
                                     photonEnergy );

  // compare ray traced photon to original ...

  // acceptance type
  //if ( result < LHCb::RichTraceMode::InHPDTube ) return;
  const std::string tag = ( result >= LHCb::RichTraceMode::InHPDTube ? "InHPD" : "OutHPD" );

  // Detector region
  std::string region, regionT;
  if      ( detectPoint.y() < 0 && detectPoint.x() < 0 ) { region = "R1"; regionT = "y<0 x<0"; }
  else if ( detectPoint.y() > 0 && detectPoint.x() < 0 ) { region = "R2"; regionT = "y>0 x<0"; }
  else if ( detectPoint.y() < 0 && detectPoint.x() > 0 ) { region = "R3"; regionT = "y<0 x>0"; }
  else if ( detectPoint.y() > 0 && detectPoint.x() > 0 ) { region = "R4"; regionT = "y>0 x>0"; }

  // HPD Hit positions Global coordinates
  const double detPntXDiff = rayTracedPhoton.detectionPoint().x() - detectPoint.x();
  const double detPntYDiff = rayTracedPhoton.detectionPoint().y() - detectPoint.y();
  const double detPntZDiff = rayTracedPhoton.detectionPoint().z() - detectPoint.z();
  plot1D( detPntXDiff, hid(rad,type+"detPnt/Global/XDiff"+tag), "Det Point X Sep Global : "+tag, -pRGlo[rad], pRGlo[rad], nBins1D() );
  plot1D( detPntYDiff, hid(rad,type+"detPnt/Global/YDiff"+tag), "Det Point Y Sep Global : "+tag, -pRGlo[rad], pRGlo[rad], nBins1D() );
  plot1D( detPntZDiff, hid(rad,type+"detPnt/Global/ZDiff"+tag), "Det Point Z Sep Global : "+tag, -pRGlo[rad], pRGlo[rad], nBins1D() );
  plot2D( ckPhi, detPntXDiff, 
          hid(rad,type+"detPnt/Global/XDiffVPhi"+tag), "Det Point X Sep Global V CK Phi: "+tag, 
          0, 2*M_PI, -pRGlo[rad], pRGlo[rad], nBins2D(), nBins2D() );
  plot2D( ckPhi, detPntYDiff, 
          hid(rad,type+"detPnt/Global/YDiffVPhi"+tag), "Det Point Y Sep Global V CK Phi: "+tag, 
          0, 2*M_PI, -pRGlo[rad], pRGlo[rad], nBins2D(), nBins2D() );
  plot2D( ckPhi, detPntZDiff, 
          hid(rad,type+"detPnt/Global/ZDiffVPhi"+tag), "Det Point Z Sep Global V CK Phi: "+tag, 
          0, 2*M_PI, -pRGlo[rad], pRGlo[rad], nBins2D(), nBins2D() );
  plot2D( ckPhi, detPntXDiff, 
          hid(rad,type+"detPnt/Global/XDiffVPhi"+tag+region), "Det Point X Sep Global V CK Phi: "+tag+" : "+regionT, 
          0, 2*M_PI, -pRGlo[rad], pRGlo[rad], nBins2D(), nBins2D() );
  plot2D( ckPhi, detPntYDiff, 
          hid(rad,type+"detPnt/Global/YDiffVPhi"+tag+region), "Det Point Y Sep Global V CK Phi: "+tag+" : "+regionT, 
          0, 2*M_PI, -pRGlo[rad], pRGlo[rad], nBins2D(), nBins2D() );
  plot2D( ckPhi, detPntZDiff, 
          hid(rad,type+"detPnt/Global/ZDiffVPhi"+tag+region), "Det Point Z Sep Global V CK Phi: "+tag+" : "+regionT, 
          0, 2*M_PI, -pRGlo[rad], pRGlo[rad], nBins2D(), nBins2D() );

  // HPD Hit positions Local Coordinates
  const Gaudi::XYZPoint origLocDetPnt = m_idTool->globalToPDPanel(detectPoint);
  const Gaudi::XYZPoint rayTLocDetPnt = m_idTool->globalToPDPanel(rayTracedPhoton.detectionPoint());
  const double detPntXDiffLoc = rayTLocDetPnt.x() - origLocDetPnt.x();
  const double detPntYDiffLoc = rayTLocDetPnt.y() - origLocDetPnt.y();
  plot1D( detPntXDiffLoc, hid(rad,type+"detPnt/Local/XDiff"+tag), "Det Point X Sep Local : "+tag, -pRLoc[rad], pRLoc[rad], nBins1D() );
  plot1D( detPntYDiffLoc, hid(rad,type+"detPnt/Local/YDiff"+tag), "Det Point Y Sep Local : "+tag, -pRLoc[rad], pRLoc[rad], nBins1D() );
  plot2D( ckPhi, detPntXDiffLoc,
          hid(rad,type+"detPnt/Local/XDiffVPhi"+tag), "Det Point X Sep Local V CK phi : "+tag,
          0, 2*M_PI, -pRLoc[rad], pRLoc[rad], nBins2D(), nBins2D() );
  plot2D( ckPhi, detPntYDiffLoc,
          hid(rad,type+"detPnt/Local/YDiffVPhi"+tag), "Det Point Y Sep Local V CK phi : "+tag,
          0, 2*M_PI, -pRLoc[rad], pRLoc[rad], nBins2D(), nBins2D() );
  plot2D( ckPhi, detPntXDiffLoc,
          hid(rad,type+"detPnt/Local/XDiffVPhi"+tag+region), "Det Point X Sep Local V CK phi : "+tag+" : "+regionT,
          0, 2*M_PI, -pRLoc[rad], pRLoc[rad], nBins2D(), nBins2D() );
  plot2D( ckPhi, detPntYDiffLoc,
          hid(rad,type+"detPnt/Local/YDiffVPhi"+tag+region), "Det Point Y Sep Local V CK phi : "+tag+" : "+regionT,
          0, 2*M_PI, -pRLoc[rad], pRLoc[rad], nBins2D(), nBins2D() );

  // Primary mirror reflection points
  const double primMirrXDiff = rayTracedPhoton.sphMirReflectionPoint().x() - primMirrPnt.x();
  const double primMirrYDiff = rayTracedPhoton.sphMirReflectionPoint().y() - primMirrPnt.y();
  const double primMirrZDiff = rayTracedPhoton.sphMirReflectionPoint().z() - primMirrPnt.z();
  plot1D( primMirrXDiff, hid(rad,type+"primMirr/XDiff"+tag), "Primary Mirror X Sep : "+tag, -primMR[rad], primMR[rad], nBins1D() );
  plot1D( primMirrYDiff, hid(rad,type+"primMirr/YDiff"+tag), "Primary Mirror Y Sep : "+tag, -primMR[rad], primMR[rad], nBins1D() );
  plot1D( primMirrZDiff, hid(rad,type+"primMirr/ZDiff"+tag), "Primary Mirror Z Sep : "+tag, -primMR[rad], primMR[rad], nBins1D() );
  plot2D( ckPhi, primMirrXDiff, hid(rad,type+"primMirr/XDiffVPhi"+tag), "Primary Mirror X Sep V CK Phi : "+tag,
          0, 2*M_PI, -primMR[rad], primMR[rad], nBins2D(), nBins2D() );
  plot2D( ckPhi, primMirrYDiff, hid(rad,type+"primMirr/YDiffVPhi"+tag), "Primary Mirror Y Sep V CK Phi : "+tag,
          0, 2*M_PI, -primMR[rad], primMR[rad], nBins2D(), nBins2D() );
  plot2D( ckPhi, primMirrZDiff, hid(rad,type+"primMirr/ZDiffVPji"+tag), "Primary Mirror Z Sep V CK Phi : "+tag,
          0, 2*M_PI, -primMR[rad], primMR[rad], nBins2D(), nBins2D() );
  plot2D( ckPhi, primMirrXDiff, hid(rad,type+"primMirr/XDiffVPhi"+tag+region), "Primary Mirror X Sep V CK Phi : "+tag+" : "+regionT,
          0, 2*M_PI, -primMR[rad], primMR[rad], nBins2D(), nBins2D() );
  plot2D( ckPhi, primMirrYDiff, hid(rad,type+"primMirr/YDiffVPhi"+tag+region), "Primary Mirror Y Sep V CK Phi : "+tag+" : "+regionT,
          0, 2*M_PI, -primMR[rad], primMR[rad], nBins2D(), nBins2D() );
  plot2D( ckPhi, primMirrZDiff, hid(rad,type+"primMirr/ZDiffVPji"+tag+region), "Primary Mirror Z Sep V CK Phi : "+tag+" : "+regionT,
          0, 2*M_PI, -primMR[rad], primMR[rad], nBins2D(), nBins2D() );

  // Primary mirror reflection points
  const double secMirrXDiff = rayTracedPhoton.flatMirReflectionPoint().x() - secMirrPnt.x();
  const double secMirrYDiff = rayTracedPhoton.flatMirReflectionPoint().y() - secMirrPnt.y();
  const double secMirrZDiff = rayTracedPhoton.flatMirReflectionPoint().z() - secMirrPnt.z();
  plot1D( secMirrXDiff, hid(rad,type+"secMirr/XDiff"+tag), "Secondary Mirror X Sep : "+tag, -secMR[rad], secMR[rad], nBins1D() );
  plot1D( secMirrYDiff, hid(rad,type+"secMirr/YDiff"+tag), "Secondary Mirror Y Sep : "+tag, -secMR[rad], secMR[rad], nBins1D() );
  plot1D( secMirrZDiff, hid(rad,type+"secMirr/ZDiff"+tag), "Secondary Mirror Z Sep : "+tag, -secMR[rad], secMR[rad], nBins1D() );
  plot2D( ckPhi, secMirrXDiff, hid(rad,type+"secMirr/XDiffVPhi"+tag), "Secondary Mirror X Sep V CK Phi : "+tag,
          0, 2*M_PI, -secMR[rad], secMR[rad], nBins2D(), nBins2D() );
  plot2D( ckPhi, secMirrYDiff, hid(rad,type+"secMirr/YDiffVPhi"+tag), "Secondary Mirror Y Sep V CK Phi : "+tag,
          0, 2*M_PI, -secMR[rad], secMR[rad], nBins2D(), nBins2D() );
  plot2D( ckPhi, secMirrZDiff, hid(rad,type+"secMirr/ZDiffVPji"+tag), "Secondary Mirror Z Sep V CK Phi : "+tag,
          0, 2*M_PI, -secMR[rad], secMR[rad], nBins2D(), nBins2D() );
  plot2D( ckPhi, secMirrXDiff, hid(rad,type+"secMirr/XDiffVPhi"+tag+region), "Secondary Mirror X Sep V CK Phi : "+tag+" : "+regionT,
          0, 2*M_PI, -secMR[rad], secMR[rad], nBins2D(), nBins2D() );
  plot2D( ckPhi, secMirrYDiff, hid(rad,type+"secMirr/YDiffVPhi"+tag+region), "Secondary Mirror Y Sep V CK Phi : "+tag+" : "+regionT,
          0, 2*M_PI, -secMR[rad], secMR[rad], nBins2D(), nBins2D() );
  plot2D( ckPhi, secMirrZDiff, hid(rad,type+"secMirr/ZDiffVPji"+tag+region), "Secondary Mirror Z Sep V CK Phi : "+tag+" : "+regionT,
          0, 2*M_PI, -secMR[rad], secMR[rad], nBins2D(), nBins2D() );

}
