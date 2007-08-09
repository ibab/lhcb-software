
//---------------------------------------------------------------------------
/** @file RichPhotonGeomMonitor.cpp
 *
 *  Implementation file for algorithm class : RichPhotonGeomMonitor
 *
 *  CVS Log :-
 *  $Id: RichPhotonGeomMonitor.cpp,v 1.13 2007-08-09 16:17:58 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "RichPhotonGeomMonitor.h"

// namespace
using namespace Rich::Rec::MC;

//---------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( PhotonGeomMonitor );

// Standard constructor, initializes variables
PhotonGeomMonitor::PhotonGeomMonitor( const std::string& name,
                                      ISvcLocator* pSvcLocator )
  : RichRecHistoAlgBase ( name, pSvcLocator ),
    m_richRecMCTruth    ( NULL ),
    m_ckAngle           ( NULL ),
    m_geomTool          ( NULL ),
    m_trSelector        ( NULL )
{
  // job opts
}

// Destructor
PhotonGeomMonitor::~PhotonGeomMonitor() {};

//  Initialize
StatusCode PhotonGeomMonitor::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecHistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRecMCTruthTool",   m_richRecMCTruth );
  acquireTool( "RichCherenkovAngle",      m_ckAngle     );
  acquireTool( "RichRecGeometry",         m_geomTool    );
  acquireTool( "TrackSelector",      m_trSelector, this );

  return sc;
}

// Main execution
StatusCode PhotonGeomMonitor::execute()
{

  // Check event status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Make sure all tracks and segments have been formed
  if ( trackCreator()->newTracks().isFailure() )
    return Error( "Problem creating RichRecTracks" );

  // make sure RichrecPixels are ready
  if ( pixelCreator()->newPixels().isFailure() )
    return Error( "Problem creating RichRecPixels" );

  // make sure photons are available
  if ( photonCreator()->reconstructPhotons().isFailure() )
    return Error( "Problem creating RichRecPhotons" );

  // Histogramming
  const RichHistoID hid;
  MAX_CKTHETA_RAD;
  MIN_CKTHETA_RAD;
  //            Radiator          Aerogel  Rich1Gas    Rich2Gas
  const double tkHitSepMin[]  = { 0.0,     0.0,     0.0     };
  const double tkHitSepMax[]  = { 500.0,   120.0,   200.0   };

  // Iterate over segments
  for ( LHCb::RichRecSegments::const_iterator iSeg = richSegments()->begin();
        iSeg != richSegments()->end(); ++iSeg )
  {
    LHCb::RichRecSegment * segment = *iSeg;

    // apply track selection
    if ( !m_trSelector->trackSelected(segment->richRecTrack()) ) continue;

    // MC type
    const Rich::ParticleIDType mcType = m_richRecMCTruth->mcParticleType( segment );

    // Radiator info
    const Rich::RadiatorType rad = segment->trackSegment().radiator();

    // Expected Cherenkov theta angle for true particle type
    // if MC type is not known, assume pion (maybe type should be a job option ??)
    const double thetaExpTrue = ( mcType == Rich::Unknown ?
                                  m_ckAngle->avgCherenkovTheta( segment, Rich::Pion ) :
                                  m_ckAngle->avgCherenkovTheta( segment, mcType ) );


    // Get photons for this segment
    const LHCb::RichRecSegment::Photons & photons = photonCreator()->reconstructPhotons( segment );
    for ( LHCb::RichRecSegment::Photons::const_iterator iPhot = photons.begin();
          iPhot != photons.end();
          ++iPhot )
    {
      LHCb::RichRecPhoton * photon = *iPhot;
      LHCb::RichRecPixel  * pixel  = photon->richRecPixel();

      // Cherenkov angles
      const double thetaRec = photon->geomPhoton().CherenkovTheta();
      //const double phiRec   = photon->geomPhoton().CherenkovPhi();
      const Gaudi::XYZPoint& locPos = pixel->radCorrLocalPositions().position(rad);
      const double sepAngle =
        ( atan2( locPos.x() - segment->pdPanelHitPointLocal().x(),
                 locPos.y() - segment->pdPanelHitPointLocal().y() ) );

      // sep
      const double sepL = sqrt( m_geomTool->trackPixelHitSep2(segment,pixel) );

      plot1D( sepL, hid(rad,"allSep"), "Local Sep. All", tkHitSepMin[rad],tkHitSepMax[rad] );

      plot2D( thetaRec, sepL, hid(rad,"allSepVCKT"), "Local Sep. V reco theta All",
              minCkTheta[rad],maxCkTheta[rad],tkHitSepMin[rad],tkHitSepMax[rad] );
      profile1D( thetaRec, sepL, hid(rad,"allSepVCKTP"), "Local Sep. V reco theta All",
                 minCkTheta[rad],maxCkTheta[rad] );

      profile1D( sepAngle, sepL, hid(rad,"allSepVphi"), "Local Sep. V Phi All", -M_PI, M_PI );

      const LHCb::MCParticle * photonParent = m_richRecMCTruth->trueCherenkovPhoton(photon);
      if ( photonParent )
      {
        plot1D( sepL, hid(rad,"trueSep"), "Local Sep. True", tkHitSepMin[rad],tkHitSepMax[rad] );

        plot2D( thetaRec, sepL, hid(rad,"trueSepVCKT"), "Local Sep. V reco theta True",
                minCkTheta[rad],maxCkTheta[rad],tkHitSepMin[rad],tkHitSepMax[rad] );
        profile1D( thetaRec, sepL, hid(rad,"trueSepVCKTP"), "Local Sep. V reco theta True",
                   minCkTheta[rad],maxCkTheta[rad] );

        profile1D( sepAngle, sepL, hid(rad,"trueSepVphi"), "Local Sep. V Phi True", -M_PI, M_PI );

        plot2D( thetaExpTrue, sepL, hid(rad,"trueSepVCKExp"), "Local Sep. V expected theta True",
                minCkTheta[rad],maxCkTheta[rad],tkHitSepMin[rad],tkHitSepMax[rad] );
        profile1D( thetaExpTrue, sepL, hid(rad,"trueSepVCKExpP"), "Local Sep. V expected theta True",
                   minCkTheta[rad],maxCkTheta[rad] );
      }

    }

  }

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode PhotonGeomMonitor::finalize()
{
  // Execute base class method
  return RichRecHistoAlgBase::finalize();
}
