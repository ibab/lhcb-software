
//---------------------------------------------------------------------------
/** @file RichPhotonRecoRayTraceTest.cpp
 *
 *  Implementation file for algorithm class : Rich::Rec::MC::PhotonRecoRayTraceTest
 *
 *  CVS Log :-
 *  $Id: RichPhotonRecoRayTraceTest.cpp,v 1.1 2008-01-03 15:33:33 jonrob Exp $
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
    m_idTool            ( NULL ),
    m_mode              ( LHCb::RichTraceMode::RespectHPDTubes,
                          LHCb::RichTraceMode::SimpleHPDs
                          //LHCb::RichTraceMode::FullHPDs
                          )
{
  // JOs
  declareProperty( "RefractionCorrection", m_refractCorr = true );
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
  acquireTool( "RichRecMCTruthTool",   m_richRecMCTruth    );
  acquireTool( "RichCherenkovAngle",   m_ckAngle           );
  acquireTool( "TrackSelector",        m_trSelector,  this );
  acquireTool( "RichRayTracing",       m_rayTracing        );
  acquireTool( "RichSmartIDTool",      m_idTool,  0, true  );

  m_mode.setAeroRefraction(m_refractCorr);

  info() << "Ray tracing : " << m_mode << endreq;

  return sc;
}

// Main execution
StatusCode PhotonRecoRayTraceTest::execute()
{

  // Check event status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Histogramming
  const RichHistoID hid;
  const double posRange[] = { 30, 10, 10 };

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

    // Radiator info
    const Rich::RadiatorType rad = segment->trackSegment().radiator();

    // Loop over photons for this segment
    const LHCb::RichRecSegment::Photons & photons = photonCreator()->reconstructPhotons( segment );
    for ( LHCb::RichRecSegment::Photons::const_iterator iPhot = photons.begin();
          iPhot != photons.end(); ++iPhot )
    {
      LHCb::RichRecPhoton * photon = *iPhot;

      // Get the photon parameters
      const Gaudi::XYZPoint  & emissPoint  = photon->geomPhoton().emissionPoint();
      const Gaudi::XYZVector & emissDir    = photon->geomPhoton().emissionDir();
      const Gaudi::XYZPoint  & detectPoint = photon->geomPhoton().detectionPoint();

      // Ray trace with these photon parameters
      LHCb::RichGeomPhoton rayTracedPhoton;
      const LHCb::RichTraceMode::RayTraceResult result =
        m_rayTracing->traceToDetector( segment->trackSegment().rich(),
                                       emissPoint,
                                       emissDir,
                                       rayTracedPhoton,
                                       m_mode );

      // compare ray traced photon to original ...

      // acceptance type
      const std::string tag = ( result >= LHCb::RichTraceMode::InHPDTube ? "InHPD" : "OutHPD" );

      // Global coordinates
      const double detPntXDiff = rayTracedPhoton.detectionPoint().x() - detectPoint.x();
      const double detPntYDiff = rayTracedPhoton.detectionPoint().y() - detectPoint.y();
      const double detPntZDiff = rayTracedPhoton.detectionPoint().z() - detectPoint.z();

      plot1D( detPntXDiff, hid(rad,"detPntXDiffGlo"+tag), "Det Point X Sep Global : "+tag, -posRange[rad], posRange[rad], 100 );
      plot1D( detPntYDiff, hid(rad,"detPntYDiffGlo"+tag), "Det Point Y Sep Global : "+tag, -posRange[rad], posRange[rad], 100 );
      plot1D( detPntZDiff, hid(rad,"detPntZDiffGlo"+tag), "Det Point Z Sep Global : "+tag, -posRange[rad], posRange[rad], 100 );

      // Local Coordinates
      const Gaudi::XYZPoint origLocDetPnt = m_idTool->globalToPDPanel(detectPoint);
      const Gaudi::XYZPoint rayTLocDetPnt = m_idTool->globalToPDPanel(rayTracedPhoton.detectionPoint());
      const double detPntXDiffLoc = origLocDetPnt.x() - rayTLocDetPnt.x();
      const double detPntYDiffLoc = origLocDetPnt.y() - rayTLocDetPnt.y();

      plot1D( detPntXDiffLoc, hid(rad,"detPntXDiffLoc"+tag), "Det Point X Sep Local : "+tag, -posRange[rad], posRange[rad], 100 );
      plot1D( detPntYDiffLoc, hid(rad,"detPntYDiffLoc"+tag), "Det Point Y Sep Local : "+tag, -posRange[rad], posRange[rad], 100 );

    } // photons for each segment

  } // segments

  return StatusCode::SUCCESS;
}
