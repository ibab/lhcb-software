
//---------------------------------------------------------------------------
/** @file RichPhotonRecoEffMonitor.cpp
 *
 *  Implementation file for algorithm class : RichPhotonRecoEffMonitor
 *
 *  CVS Log :-
 *  $Id: RichPhotonRecoEffMonitor.cpp,v 1.1 2005-11-07 09:37:20 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

// local
#include "RichPhotonRecoEffMonitor.h"

//---------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichPhotonRecoEffMonitor>          s_factory ;
const        IAlgFactory& RichPhotonRecoEffMonitorFactory = s_factory ;

// Standard constructor, initializes variables
RichPhotonRecoEffMonitor::RichPhotonRecoEffMonitor( const std::string& name,
                                                    ISvcLocator* pSvcLocator )
  : RichRecHistoAlgBase ( name, pSvcLocator ),
    m_richRecMCTruth    ( 0 ),
    m_ckAngle           ( 0 ),
    m_geomTool          ( 0 ),
    m_forcedPhotCreator ( 0 )
{
  // track selector
  declareProperty( "TrackSelection", m_trSelector.selectedTrackTypes() );
}

// Destructor
RichPhotonRecoEffMonitor::~RichPhotonRecoEffMonitor() {};

//  Initialize
StatusCode RichPhotonRecoEffMonitor::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecHistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRecMCTruthTool",      m_richRecMCTruth );
  acquireTool( "RichCherenkovAngle",      m_ckAngle     );
  acquireTool( "RichRecGeometry",         m_geomTool    );
  acquireTool( "ForcedRichPhotonCreator", m_forcedPhotCreator );

  // Configure track selector
  if ( !m_trSelector.configureTrackTypes() )
    return Error( "Problem configuring track selection" );
  m_trSelector.printTrackSelection( info() );

  return sc;
}

// Main execution
StatusCode RichPhotonRecoEffMonitor::execute()
{

  // Check event status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Histogramming
  const RichHistoID hid;
  MAX_CKTHETA_RAD;
  MIN_CKTHETA_RAD;
  PD_LOCAL_POSITIONS_X;
  PD_LOCAL_POSITIONS_Y;
  //            Radiator          Aerogel  C4F10    CF4
  const double ckRange[]      = { 0.015,   0.01,    0.005   };
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

    // which rich
    const Rich::DetectorType rich = segment->trackSegment().rich();
    // Radiator info
    const Rich::RadiatorType rad = segment->trackSegment().radiator();

    // loop over pixels in same RICH as segment
    for ( RichRecPixels::const_iterator iPix = pixelCreator()->begin(rich);
          iPix != pixelCreator()->end(rich); ++iPix )
    {
      RichRecPixel * pixel = *iPix;

      // Is this a true Cherenkov Photon
      const MCParticle * trueCKPhotonMCP = m_richRecMCTruth->trueCherenkovPhoton(segment,pixel);
      if ( trueCKPhotonMCP )
      {
        // Find the reco-photon for this combination
        RichRecPhoton * recPhot = photonCreator()->checkForExistingPhoton(segment,pixel);
        if ( !recPhot )
        {
          // This is a true CK photon that was not reconstructed. Why ?
          //++counter("Found non-reconstructed true "+Rich::text(rad)+" cherenkov photon");

          // compute hit seperation : global
          const double sep = sqrt( m_geomTool->trackPixelHitSep2(segment, pixel) );
          // expect CK theta
          const double thetaExp = m_ckAngle->avgCherenkovTheta(segment,mcType);
          const double sepAngle = 
            ( atan2( pixel->localPosition(rad).x() - segment->pdPanelHitPointLocal().x(),
                     pixel->localPosition(rad).y() - segment->pdPanelHitPointLocal().y() ) );

          plot1D( sep, hid(rad,"nonRecoSep"), "Non-reco. pixel/segment sep",
                  tkHitSepMin[rad],tkHitSepMax[rad] );

          plot2D( pixel->localPosition(rad).x(), pixel->localPosition(rad).y(),
                  hid(rad,"nonRecoXvY"), "Non-reco. pixel XvY",
                  xMinPDLoc[rich],xMaxPDLoc[rich],yMinPDLoc[rich],yMaxPDLoc[rich] );

          profile1D( sepAngle, sep, hid(rad,"nonRecPhiVsep"), "Non reco. sep V CK phi", -M_PI,M_PI );

          // get MC photon
          const MCRichOpticalPhoton * mcPhot = m_richRecMCTruth->trueOpticalPhoton(segment,pixel);
          if ( mcPhot )
          {
            plot1D( mcPhot->cherenkovTheta(), hid(rad,"nonRecoCKthetaMC"),
                    "Non-reco. CK theta", minCkTheta[rad],maxCkTheta[rad] );
            plot2D( mcPhot->cherenkovTheta(), sep, hid(rad,"nonRecoSepVCKtMC"),
                    "Non-reco. Sep V MC CK theta true", 
                    minCkTheta[rad],maxCkTheta[rad],tkHitSepMin[rad],tkHitSepMax[rad] );
            plot1D( mcPhot->cherenkovTheta() - thetaExp, hid(rad,"nonRecoCKresMC"),
                    "Non Reco. MC CK res", -ckRange[rad], ckRange[rad]);
          } // mc photon

          // Force create a photon
          recPhot = m_forcedPhotCreator->reconstructPhoton(segment,pixel);
          if ( !recPhot )
          {
            //++counter( "Failed to force creation of "+Rich::text(rad)+" cherenkov photon" );
          }
          else
          {
            plot1D( recPhot->geomPhoton().CherenkovTheta(), hid(rad,"nonRecoCKtheta"),
                    "Non reco. CK theta", minCkTheta[rad],maxCkTheta[rad] );
            plot2D( recPhot->geomPhoton().CherenkovTheta(), sep, hid(rad,"nonRecoSepVCKt"),
                    "Non-reco. Sep V Reco CK theta true", 
                    minCkTheta[rad],maxCkTheta[rad],tkHitSepMin[rad],tkHitSepMax[rad] );
            plot1D( recPhot->geomPhoton().CherenkovTheta() - thetaExp, hid(rad,"nonRecoCKresMC"),
                    "Non Reco. CK res", -ckRange[rad], ckRange[rad]);
          }
          
        } // photon not reconstructed

      } // true photon

    } // loop over pixels for same rad as segment

  } // loop over all segments

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode RichPhotonRecoEffMonitor::finalize()
{
  // Execute base class method
  return RichRecHistoAlgBase::finalize();
}
