
//---------------------------------------------------------------------------
/** @file RichPhotonGeomMonitor.cpp
 *
 *  Implementation file for algorithm class : RichPhotonGeomMonitor
 *
 *  CVS Log :-
 *  $Id: RichPhotonGeomMonitor.cpp,v 1.4 2006-02-16 16:07:48 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

// local
#include "RichPhotonGeomMonitor.h"

// namespace
using namespace LHCb;

//---------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichPhotonGeomMonitor>          s_factory ;
const        IAlgFactory& RichPhotonGeomMonitorFactory = s_factory ;

// Standard constructor, initializes variables
RichPhotonGeomMonitor::RichPhotonGeomMonitor( const std::string& name,
                                              ISvcLocator* pSvcLocator )
  : RichRecHistoAlgBase ( name, pSvcLocator ),
    m_richRecMCTruth    ( 0 ),
    m_ckAngle           ( 0 ),
    m_geomTool          ( 0 )
{
  // track selector
  declareProperty( "TrackSelection", m_trSelector.selectedTrackTypes() );
}

// Destructor
RichPhotonGeomMonitor::~RichPhotonGeomMonitor() {};

//  Initialize
StatusCode RichPhotonGeomMonitor::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecHistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRecMCTruthTool",   m_richRecMCTruth );
  acquireTool( "RichCherenkovAngle",      m_ckAngle     );
  acquireTool( "RichRecGeometry",         m_geomTool    );

  // Configure track selector
  if ( !m_trSelector.configureTrackTypes(msg()) )
    return Error( "Problem configuring track selection" );
  m_trSelector.printTrackSelection( info() );

  return sc;
}

// Main execution
StatusCode RichPhotonGeomMonitor::execute()
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
  //            Radiator          Aerogel  C4F10    CF4
  const double tkHitSepMin[]  = { 0.0,     0.0,     0.0     };
  const double tkHitSepMax[]  = { 500.0,   120.0,   200.0   };

  // Iterate over segments
  for ( RichRecSegments::const_iterator iSeg = richSegments()->begin();
        iSeg != richSegments()->end(); ++iSeg )
  {
    RichRecSegment * segment = *iSeg;

    // apply track selection
    if ( !m_trSelector.trackSelected(segment->richRecTrack()) ) continue;

    // MC type
    const Rich::ParticleIDType mcType = m_richRecMCTruth->mcParticleType( segment );
    if ( mcType == Rich::Unknown ) continue;

    // Radiator info
    const Rich::RadiatorType rad = segment->trackSegment().radiator();

    // Expected Cherenkov theta angle for true particle type
    const double thetaExpTrue = m_ckAngle->avgCherenkovTheta( segment, mcType );

    // Get photons for this segment
    const RichRecSegment::Photons & photons = photonCreator()->reconstructPhotons( segment );
    for ( RichRecSegment::Photons::const_iterator iPhot = photons.begin();
          iPhot != photons.end();
          ++iPhot )
    {
      RichRecPhoton * photon = *iPhot;
      RichRecPixel  * pixel  = photon->richRecPixel();


      // Cherenkov angles
      const double thetaRec = photon->geomPhoton().CherenkovTheta();
      //const double phiRec   = photon->geomPhoton().CherenkovPhi();
      const double sepAngle =
        ( atan2( pixel->localPosition(rad).x() - segment->pdPanelHitPointLocal().x(),
                 pixel->localPosition(rad).y() - segment->pdPanelHitPointLocal().y() ) );

      // sep
      const double sepL = sqrt( m_geomTool->trackPixelHitSep2(segment,pixel) );

      plot2D( thetaRec, sepL, hid(rad,"allSepVCKT"), "Sep V reco theta All",
              minCkTheta[rad],maxCkTheta[rad],tkHitSepMin[rad],tkHitSepMax[rad] );
      profile1D( thetaRec, sepL, hid(rad,"allSepVCKTP"), "Sep V theta All",
                 minCkTheta[rad],maxCkTheta[rad] );
      profile1D( sepAngle, sepL, hid(rad,"sepVphi"), "Sep V Phi All", -M_PI, M_PI );

      const MCParticle * photonParent = m_richRecMCTruth->trueCherenkovPhoton(photon);
      if ( photonParent )
      {
        plot2D( thetaRec, sepL, hid(rad,"allSepVCKTTrue"), "Sep V theta True",
                minCkTheta[rad],maxCkTheta[rad],tkHitSepMin[rad],tkHitSepMax[rad] );
        profile1D( thetaRec, sepL, hid(rad,"allSepVCKTPTrue"), "Sep V reco theta True",
                   minCkTheta[rad],maxCkTheta[rad] );
        profile1D( sepAngle, sepL, hid(rad,"sepVphi"), "Sep V Phi True", -M_PI, M_PI );
        plot2D( thetaExpTrue, sepL, hid(rad,"allSepVCKExpTrue"), "Sep V expected theta True",
                minCkTheta[rad],maxCkTheta[rad],tkHitSepMin[rad],tkHitSepMax[rad] );
        profile1D( thetaExpTrue, sepL, hid(rad,"allSepVCKExpPTrue"), "Sep V expected theta True",
                   minCkTheta[rad],maxCkTheta[rad] );
      }

    }

  }

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode RichPhotonGeomMonitor::finalize()
{
  // Execute base class method
  return RichRecHistoAlgBase::finalize();
}
