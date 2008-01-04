
//---------------------------------------------------------------------------
/** @file RichPhotonRecoRayTraceTest.cpp
 *
 *  Implementation file for algorithm class : Rich::Rec::MC::PhotonRecoRayTraceTest
 *
 *  CVS Log :-
 *  $Id: RichPhotonRecoRayTraceTest.cpp,v 1.2 2008-01-04 16:15:14 jonrob Exp $
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
DECLARE_ALGORITHM_FACTORY( PhotonRecoRayTraceTest );

// Standard constructor, initializes variables
PhotonRecoRayTraceTest::PhotonRecoRayTraceTest( const std::string& name,
                                                ISvcLocator* pSvcLocator )
  : Rich::Rec::HistoAlgBase ( name, pSvcLocator ),
    m_richRecMCTruth    ( NULL ),
    m_ckAngle           ( NULL ),
    m_trSelector        ( NULL ),
    m_rayTracing        ( NULL ),
    m_idTool            ( NULL )
{
  // JOs
  declareProperty( "RefractionCorrection", m_refractCorr = true  );
  declareProperty( "FullHPDsInRayTracing", m_fullHPDs    = false );
  // number of bins
  declareProperty( "NumberBins1D",  m_nBins1D = 200 );
  declareProperty( "NumberBins2D",  m_nBins2D = 100 );
}

// Destructor
PhotonRecoRayTraceTest::~PhotonRecoRayTraceTest() {}

//  Initialize
StatusCode PhotonRecoRayTraceTest::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = Rich::Rec::HistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRecMCTruthTool",   m_richRecMCTruth   );
  acquireTool( "RichCherenkovAngle",   m_ckAngle          );
  acquireTool( "TrackSelector",        m_trSelector, this );
  acquireTool( "RichRayTracing",       m_rayTracing       );
  acquireTool( "RichSmartIDTool",      m_idTool,  0, true );

  // ray tracing options
  m_mode = LHCb::RichTraceMode( LHCb::RichTraceMode::RespectHPDTubes,
                                ( m_fullHPDs ?
                                  LHCb::RichTraceMode::FullHPDs   :
                                  LHCb::RichTraceMode::SimpleHPDs ) );
  m_mode.setAeroRefraction(m_refractCorr);
  info() << "Ray tracing : " << m_mode << endreq;

  return sc;
}

// Main execution
StatusCode PhotonRecoRayTraceTest::execute()
{
  // Check event status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Iterate over segments
  for ( LHCb::RichRecSegments::const_iterator iSeg = richSegments()->begin();
        iSeg != richSegments()->end(); ++iSeg )
  {
    LHCb::RichRecSegment * segment = *iSeg;

    // apply track selection
    if ( !m_trSelector->trackSelected(segment->richRecTrack()) ) continue;

    // MC type
    const Rich::ParticleIDType mcType = m_richRecMCTruth->mcParticleType( segment );
    if ( mcType == Rich::Unknown ) continue;

    // Loop over photons for this segment
    const LHCb::RichRecSegment::Photons & photons = photonCreator()->reconstructPhotons( segment );
    for ( LHCb::RichRecSegment::Photons::const_iterator iPhot = photons.begin();
          iPhot != photons.end(); ++iPhot )
    {
      // Make plots against reco photon
      makePlots( "Reco/",
                 (*iPhot)->geomPhoton().emissionPoint(),
                 (*iPhot)->geomPhoton().emissionDir(),
                 (*iPhot)->geomPhoton().detectionPoint(),
                 (*iPhot)->geomPhoton().sphMirReflectionPoint(),
                 (*iPhot)->geomPhoton().flatMirReflectionPoint(),
                 segment->trackSegment().rich(),
                 segment->trackSegment().radiator(),
                 (*iPhot)->geomPhoton().CherenkovPhi(),
                 segment->trackSegment().avPhotonEnergy() );

      // Locate MC photon
      const LHCb::MCRichOpticalPhoton * mcPhot = m_richRecMCTruth->trueOpticalPhoton(*iPhot);
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
  const RichHistoID hid;
  //                       Aero  C4F10  CF4
  const double pRLoc[]  = { 5,    4,     4   };
  const double pRGlo[]  = { 10,   10,    10  };
  const double primMR[] = { 0.6,  0.2,   0.2 };
  const double secMR[]  = { 0.6,  0.2,   0.6 };

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
  if ( result < LHCb::RichTraceMode::InHPDTube ) return;
  const std::string tag = ( result >= LHCb::RichTraceMode::InHPDTube ? "InHPD" : "OutHPD" );

  // HPD Hit positions Global coordinates
  const double detPntXDiff = rayTracedPhoton.detectionPoint().x() - detectPoint.x();
  const double detPntYDiff = rayTracedPhoton.detectionPoint().y() - detectPoint.y();
  const double detPntZDiff = rayTracedPhoton.detectionPoint().z() - detectPoint.z();
  plot1D( detPntXDiff, hid(rad,type+"detPntXDiffGlo"+tag), "Det Point X Sep Global : "+tag, -pRGlo[rad], pRGlo[rad], m_nBins1D );
  plot1D( detPntYDiff, hid(rad,type+"detPntYDiffGlo"+tag), "Det Point Y Sep Global : "+tag, -pRGlo[rad], pRGlo[rad], m_nBins1D );
  plot1D( detPntZDiff, hid(rad,type+"detPntZDiffGlo"+tag), "Det Point Z Sep Global : "+tag, -pRGlo[rad], pRGlo[rad], m_nBins1D );

  // HPD Hit positions Local Coordinates
  const Gaudi::XYZPoint origLocDetPnt = m_idTool->globalToPDPanel(detectPoint);
  const Gaudi::XYZPoint rayTLocDetPnt = m_idTool->globalToPDPanel(rayTracedPhoton.detectionPoint());
  const double detPntXDiffLoc = rayTLocDetPnt.x() - origLocDetPnt.x();
  const double detPntYDiffLoc = rayTLocDetPnt.y() - origLocDetPnt.y();
  plot1D( detPntXDiffLoc, hid(rad,type+"detPntXDiffLoc"+tag), "Det Point X Sep Local : "+tag, -pRLoc[rad], pRLoc[rad], m_nBins1D );
  plot1D( detPntYDiffLoc, hid(rad,type+"detPntYDiffLoc"+tag), "Det Point Y Sep Local : "+tag, -pRLoc[rad], pRLoc[rad], m_nBins1D );
  plot2D( ckPhi, detPntXDiffLoc,
          hid(rad,type+"detPntXDiffLocVPhi"+tag), "Det Point X Sep Local V CK phi : "+tag,
          0, 2*M_PI, -pRLoc[rad], pRLoc[rad], m_nBins2D, m_nBins2D );
  plot2D( ckPhi, detPntYDiffLoc,
          hid(rad,type+"detPntYDiffLocVPhi"+tag), "Det Point Y Sep Local V CK phi : "+tag,
          0, 2*M_PI, -pRLoc[rad], pRLoc[rad], m_nBins2D, m_nBins2D );

  // Primary mirror reflection points
  const double primMirrXDiff = rayTracedPhoton.sphMirReflectionPoint().x() - primMirrPnt.x();
  const double primMirrYDiff = rayTracedPhoton.sphMirReflectionPoint().y() - primMirrPnt.y();
  const double primMirrZDiff = rayTracedPhoton.sphMirReflectionPoint().z() - primMirrPnt.z();
  plot1D( primMirrXDiff, hid(rad,type+"primMirrXDiff"+tag), "Primary Mirror X Sep : "+tag, -primMR[rad], primMR[rad], m_nBins1D );
  plot1D( primMirrYDiff, hid(rad,type+"primMirrYDiff"+tag), "Primary Mirror Y Sep : "+tag, -primMR[rad], primMR[rad], m_nBins1D );
  plot1D( primMirrZDiff, hid(rad,type+"primMirrZDiff"+tag), "Primary Mirror Z Sep : "+tag, -primMR[rad], primMR[rad], m_nBins1D );
  plot2D( ckPhi, primMirrXDiff, hid(rad,type+"primMirrXDiffVPhi"+tag), "Primary Mirror X Sep V CK Phi : "+tag,
          0, 2*M_PI, -primMR[rad], primMR[rad], m_nBins2D, m_nBins2D );
  plot2D( ckPhi, primMirrYDiff, hid(rad,type+"primMirrYDiffVPhi"+tag), "Primary Mirror Y Sep V CK Phi : "+tag,
          0, 2*M_PI, -primMR[rad], primMR[rad], m_nBins2D, m_nBins2D );
  plot2D( ckPhi, primMirrZDiff, hid(rad,type+"primMirrZDiffVPji"+tag), "Primary Mirror Z Sep V CK Phi : "+tag,
          0, 2*M_PI, -primMR[rad], primMR[rad], m_nBins2D, m_nBins2D );

  // Primary mirror reflection points
  const double secMirrXDiff = rayTracedPhoton.flatMirReflectionPoint().x() - secMirrPnt.x();
  const double secMirrYDiff = rayTracedPhoton.flatMirReflectionPoint().y() - secMirrPnt.y();
  const double secMirrZDiff = rayTracedPhoton.flatMirReflectionPoint().z() - secMirrPnt.z();
  plot1D( secMirrXDiff, hid(rad,type+"secMirrXDiff"+tag), "Secondary Mirror X Sep : "+tag, -secMR[rad], secMR[rad], m_nBins1D );
  plot1D( secMirrYDiff, hid(rad,type+"secMirrYDiff"+tag), "Secondary Mirror Y Sep : "+tag, -secMR[rad], secMR[rad], m_nBins1D );
  plot1D( secMirrZDiff, hid(rad,type+"secMirrZDiff"+tag), "Secondary Mirror Z Sep : "+tag, -secMR[rad], secMR[rad], m_nBins1D );
  plot2D( ckPhi, secMirrXDiff, hid(rad,type+"secMirrXDiffVPhi"+tag), "Secondary Mirror X Sep V CK Phi : "+tag,
          0, 2*M_PI, -secMR[rad], secMR[rad], m_nBins2D, m_nBins2D );
  plot2D( ckPhi, secMirrYDiff, hid(rad,type+"secMirrYDiffVPhi"+tag), "Secondary Mirror Y Sep V CK Phi : "+tag,
          0, 2*M_PI, -secMR[rad], secMR[rad], m_nBins2D, m_nBins2D );
  plot2D( ckPhi, secMirrZDiff, hid(rad,type+"secMirrZDiffVPji"+tag), "Secondary Mirror Z Sep V CK Phi : "+tag,
          0, 2*M_PI, -secMR[rad], secMR[rad], m_nBins2D, m_nBins2D );

}
