
//---------------------------------------------------------------------------
/** @file RichPhotonRecoEffMonitor.cpp
 *
 *  Implementation file for algorithm class : PhotonRecoEffMonitor
 *
 *  CVS Log :-
 *  $Id: RichPhotonRecoEffMonitor.cpp,v 1.13 2007-08-13 12:38:49 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

// local
#include "RichPhotonRecoEffMonitor.h"

// From Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// namespace
using namespace Rich::Rec::MC;

//---------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( PhotonRecoEffMonitor );

// Standard constructor, initializes variables
PhotonRecoEffMonitor::PhotonRecoEffMonitor( const std::string& name,
                                            ISvcLocator* pSvcLocator )
  : RichRecHistoAlgBase ( name, pSvcLocator ),
    m_richRecMCTruth    ( NULL ),
    m_ckAngle           ( NULL ),
    m_geomTool          ( NULL ),
    m_forcedPhotCreator ( NULL ),
    m_trSelector        ( NULL )
{
  // job opts
}

// Destructor
PhotonRecoEffMonitor::~PhotonRecoEffMonitor() {};

//  Initialize
StatusCode PhotonRecoEffMonitor::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecHistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRecMCTruthTool",      m_richRecMCTruth );
  acquireTool( "RichCherenkovAngle",      m_ckAngle     );
  acquireTool( "RichRecGeometry",         m_geomTool    );
  acquireTool( "ForcedRichPhotonCreator", m_forcedPhotCreator );
  acquireTool( "TrackSelector",           m_trSelector, this );

  return sc;
}

// Main execution
StatusCode PhotonRecoEffMonitor::execute()
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
  PD_LOCAL_POSITIONS_X;
  PD_LOCAL_POSITIONS_Y;
  //            Radiator          Aerogel  Rich1Gas    Rich2Gas
  const double ckRange[]      = { 0.015,   0.01,    0.005   };
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
    if ( mcType == Rich::Unknown ) continue;

    // which rich
    const Rich::DetectorType rich = segment->trackSegment().rich();
    // Radiator info
    const Rich::RadiatorType rad = segment->trackSegment().radiator();

    // total number of reconstructed photons
    const unsigned int nRecoPhots = segment->richRecPhotons().size();
    // number of reconstructed and non-reconstructed true photons
    unsigned int nRecoPhotsTrue(0), notRecoPhotsTrue(0);

    // loop over pixels in same RICH as segment
    IPixelCreator::PixelRange range = pixelCreator()->range(rich);
    for ( IPixelCreator::PixelRange::const_iterator iPix = range.begin();
          iPix != range.end(); ++iPix )
    {
      LHCb::RichRecPixel * pixel = *iPix;
      const Gaudi::XYZPoint & locPos = pixel->radCorrLocalPositions().position(rad);

      // Is this a true Cherenkov Photon
      const LHCb::MCParticle * trueCKPhotonMCP = m_richRecMCTruth->trueCherenkovPhoton(segment,pixel);
      if ( trueCKPhotonMCP )
      {
        // Find the reco-photon for this combination
        LHCb::RichRecPhoton * recPhot = photonCreator()->checkForExistingPhoton(segment,pixel);
        if ( recPhot )
        {
          ++nRecoPhotsTrue;
        }
        else
        {
          ++notRecoPhotsTrue;
          // This is a true CK photon that was not reconstructed. Why ?
          ++counter("Found non-reconstructed true "+Rich::text(rad)+" cherenkov photon");

          // Force create a photon
          recPhot = m_forcedPhotCreator->reconstructPhoton(segment,pixel);
          if ( !recPhot )
          {
            ++counter( "Failed force creation of "+Rich::text(rad)+" cherenkov photon" );

            // compute hit seperation : global
            const double sep = sqrt( m_geomTool->trackPixelHitSep2(segment, pixel) );
            // expect CK theta
            const double thetaExp = m_ckAngle->avgCherenkovTheta(segment,mcType);
            const double sepAngle =
              ( atan2( locPos.x() - segment->pdPanelHitPointLocal().x(),
                       locPos.y() - segment->pdPanelHitPointLocal().y() ) );

            plot1D( sep, hid(rad,"CannotReco/nonRecoSep"), "Non-reco. pixel/segment sep",
                    tkHitSepMin[rad],tkHitSepMax[rad] );

            plot2D( locPos.x(), locPos.y(),
                    hid(rad,"CannotReco/nonRecoXvY"), "Non-reco. pixel XvY",
                    xMinPDLoc[rich],xMaxPDLoc[rich],yMinPDLoc[rich],yMaxPDLoc[rich] );

            profile1D( sepAngle, sep, hid(rad,"CannotReco/nonRecPhiVsep"), "Non reco. sep V CK phi", -M_PI,M_PI );

            // get MC photon
            const LHCb::MCRichOpticalPhoton * mcPhot = m_richRecMCTruth->trueOpticalPhoton(segment,pixel);
            if ( mcPhot )
            {
              plot1D( mcPhot->cherenkovTheta(), hid(rad,"CannotReco/nonRecoCKthetaMC"),
                      "Non-reco. CK theta", minCkTheta[rad],maxCkTheta[rad] );
              plot2D( mcPhot->cherenkovTheta(), sep, hid(rad,"CannotReco/nonRecoSepVCKtMC"),
                      "Non-reco. Sep V MC CK theta true",
                      minCkTheta[rad],maxCkTheta[rad],tkHitSepMin[rad],tkHitSepMax[rad] );
              plot1D( mcPhot->cherenkovTheta() - thetaExp, hid(rad,"CannotReco/nonRecoCKresMC"),
                      "Non Reco. MC CK res", -ckRange[rad], ckRange[rad]);
            } // mc photon

          }
          else
          {

            // compute hit seperation : global
            const double sep = sqrt( m_geomTool->trackPixelHitSep2(segment, pixel) );
            // expect CK theta
            const double thetaExp = m_ckAngle->avgCherenkovTheta(segment,mcType);
            const double sepAngle =
              ( atan2( locPos.x() - segment->pdPanelHitPointLocal().x(),
                       locPos.y() - segment->pdPanelHitPointLocal().y() ) );

            plot1D( sep, hid(rad,"CanReco/nonRecoSep"), "Non-reco. pixel/segment sep",
                    tkHitSepMin[rad],tkHitSepMax[rad] );

            plot2D( locPos.x(), locPos.y(),
                    hid(rad,"CanReco/nonRecoXvY"), "Non-reco. pixel XvY",
                    xMinPDLoc[rich],xMaxPDLoc[rich],yMinPDLoc[rich],yMaxPDLoc[rich] );

            profile1D( sepAngle, sep, hid(rad,"CanReco/nonRecPhiVsep"), "Non reco. sep V CK phi", -M_PI,M_PI );

            // get MC photon
            const LHCb::MCRichOpticalPhoton * mcPhot = m_richRecMCTruth->trueOpticalPhoton(segment,pixel);
            if ( mcPhot )
            {
              plot1D( mcPhot->cherenkovTheta(), hid(rad,"CanReco/nonRecoCKthetaMC"),
                      "Non-reco. CK theta", minCkTheta[rad],maxCkTheta[rad] );
              plot2D( mcPhot->cherenkovTheta(), sep, hid(rad,"CanReco/nonRecoSepVCKtMC"),
                      "Non-reco. Sep V MC CK theta true",
                      minCkTheta[rad],maxCkTheta[rad],tkHitSepMin[rad],tkHitSepMax[rad] );
              plot1D( mcPhot->cherenkovTheta() - thetaExp, hid(rad,"CanReco/nonRecoCKresMC"),
                      "Non Reco. MC CK res", -ckRange[rad], ckRange[rad]);
            } // mc photon

              // Plot res for these photons
            plot1D( recPhot->geomPhoton().CherenkovTheta(), hid(rad,"CanReco/nonRecoCKtheta"),
                    "Non reco. CK theta", minCkTheta[rad],maxCkTheta[rad] );
            plot2D( recPhot->geomPhoton().CherenkovTheta(), sep, hid(rad,"CanReco/nonRecoSepVCKt"),
                    "Non-reco. Sep V Reco CK theta true",
                    minCkTheta[rad],maxCkTheta[rad],tkHitSepMin[rad],tkHitSepMax[rad] );
            plot1D( recPhot->geomPhoton().CherenkovTheta() - thetaExp, hid(rad,"CanReco/nonRecoCKresMC"),
                    "Non Reco. CK res", -ckRange[rad], ckRange[rad]);

          }

        } // photon not reconstructed

      } // true photon

    } // loop over pixels for same rich as segment

    const unsigned int nTotTrue = nRecoPhotsTrue+notRecoPhotsTrue;
    const double recoEff = 100 * ( nTotTrue>0 ? (double)nRecoPhotsTrue/(double)nTotTrue : 0 );
    plot1D( recoEff, hid(rad,"photRecoEff"), "Photon Reconstruction Eff.", 0, 100 );
    profile1D( segment->richRecTrack()->vertexMomentum(), recoEff,
               hid(rad,"photRecoEffVptot"), "Photon Reconstruction Eff. V Ptot", 0, 100*Gaudi::Units::GeV );

    const double recoPurity = 100 * ( nRecoPhots>0 ? (double)nRecoPhotsTrue/(double)nRecoPhots : 0 );
    plot1D( recoPurity, hid(rad,"photRecoPur"), "Photon Reconstruction Purity", 0, 100 );
    profile1D( segment->richRecTrack()->vertexMomentum(), recoPurity,
               hid(rad,"photRecoPurVptot"), "Photon Reconstruction Purity V Ptot", 0, 100*Gaudi::Units::GeV );

  } // loop over all segments

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode PhotonRecoEffMonitor::finalize()
{
  // Execute base class method
  return RichRecHistoAlgBase::finalize();
}
