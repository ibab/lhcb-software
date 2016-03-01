
//---------------------------------------------------------------------------
/** @file RichPhotonRecoCompare.cpp
 *
 *  Implementation file for algorithm class : RichPhotonRecoCompare
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   14/01/2016
 */
//---------------------------------------------------------------------------

// local
#include "RichPhotonRecoCompare.h"

// From Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// namespace
using namespace Rich::Rec::MC;

//---------------------------------------------------------------------------

// Standard constructor, initializes variables
PhotonRecoCompare::PhotonRecoCompare( const std::string& name,
                                      ISvcLocator* pSvcLocator )
  : Rich::Rec::TupleAlgBase ( name , pSvcLocator )
{
  declareProperty( "DetailedPlots",  m_detailedPlots  = true  );
  declareProperty( "BadPhotonPlots", m_badPhotonPlots = false );
}

// Destructor
PhotonRecoCompare::~PhotonRecoCompare() { }

//  Initialize
StatusCode PhotonRecoCompare::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = Rich::Rec::TupleAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  acquireTool( "TrackSelector",      m_trSelector, this );
  acquireTool( "BaselinePhotonReco", m_reco,       this );
  acquireTool( "RichRecMCTruthTool", m_richRecMCTruth   );

  return sc;
}

// Main execution
StatusCode PhotonRecoCompare::execute()
{
  // Check event status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Histogramming
  MAX_CKTHETA_RAD;
  MIN_CKTHETA_RAD;
  CKTHETADIFF_RANGE;
  PD_GLOBAL_POSITIONS;
  PD_LOCAL_POSITIONS;
  RADIATOR_GLOBAL_POSITIONS_X_EXIT;
  RADIATOR_GLOBAL_POSITIONS_Y_EXIT;
  RADIATOR_GLOBAL_POSITIONS_Z;
  const Rich::HistoID hid;

  // min and max P for histos
  //const double maxP = m_trSelector->maxPCut() * Gaudi::Units::GeV;
  //const double minP = m_trSelector->minPCut() * Gaudi::Units::GeV;
  constexpr double maxP = 100 * Gaudi::Units::GeV;
  constexpr double minP = 0   * Gaudi::Units::GeV;

  // Max deviation in reconstructed CK theta difference (Aero,R1gas,R2gas)
  const std::vector<double> maxDev = { 0.0, 5e-4, 5e-4 };

  // Iterate over segments
  for ( auto * segment : *richSegments() )
  {
    // apply track selection
    if ( !m_trSelector->trackSelected(segment->richRecTrack()) ) continue;

    // segment momentum
    const auto pTot = std::sqrt(segment->trackSegment().bestMomentum().Mag2());

    // Radiator info
    const auto rad  = segment->trackSegment().radiator();
    const auto rich = segment->trackSegment().rich();

    // MC type
    const auto mcType = m_richRecMCTruth->mcParticleType( segment );

    // Get photons for this segment
    const auto & photons = photonCreator()->reconstructPhotons( segment );

    // loop over photons
    for ( auto * photon : photons )
    {

      // Access the MC info for this photon
      const auto * mcPhot = m_richRecMCTruth->trueOpticalPhoton(photon);

      // If we have truth info, continue
      if ( mcPhot )
      {

        // Get the standard photon info from the candidate
        const auto & stdPhot = photon->geomPhoton();

        // Create on the fly a comparison photon using the baseline tool
        LHCb::RichGeomPhoton basePhot;
        const auto sc = m_reco->reconstructPhoton( photon->richRecSegment(),
                                                   photon->richRecPixel(),
                                                   basePhot );
        if ( sc.isSuccess() )
        {
          // compare the photons ...

          // range for 'DiffDiff' plots. 25% the normal CK theta range ...
          const auto ckDiffDiff = 0.25 * ckRange[rad];

          // CK theta values
          const auto stdCKTheta  = stdPhot.CherenkovTheta();
          const auto baseCKTheta = basePhot.CherenkovTheta();
          const auto mcCKTheta   = mcPhot->cherenkovTheta();
          // differences
          const auto stdMCDiff   = stdCKTheta  - mcCKTheta;
          const auto baseMCDiff  = baseCKTheta - mcCKTheta;
          auto   stdBaseDiffDiff = fabs(stdMCDiff) - fabs(baseMCDiff);
          // sanitise outliers ...
          if      (  ckDiffDiff < stdBaseDiffDiff ) { stdBaseDiffDiff =  0.99*ckDiffDiff; }
          else if ( -ckDiffDiff > stdBaseDiffDiff ) { stdBaseDiffDiff = -0.99*ckDiffDiff; }

          // Emission point
          //const auto& stdEmissPtn  = stdPhot.emissionPoint();
          const auto& baseEmissPtn = stdPhot.emissionPoint();
          const auto& mcEmissPtn   = mcPhot->emissionPoint();

          // plots

          // all hypos
          richHisto1D( hid(rad,"stdRecMCDiff"),
                       "StdRec-MC CK theta",
                       -ckRange[rad], ckRange[rad], nBins1D() ) -> fill( stdMCDiff );
          richHisto1D( hid(rad,"baseRecMCDiff"),
                       "BaseRec-MC CK theta",
                       -ckRange[rad], ckRange[rad], nBins1D() ) -> fill( baseMCDiff );
          richProfile1D( hid(rad,"stdBaseMCDiffDiffVP"),
                         "|StdRec-MC| - |BaseRec-MC| CK Theta V P",
                         minP, maxP, nBins1D() )
            -> fill( pTot, stdBaseDiffDiff );
          richProfile1D( hid(rad,"stdBaseMCDiffDiffVCKTheta"),
                         "|StdRec-MC| - |BaseRec-MC| CK Theta V CK Theta True",
                         minCkTheta[rad], maxCkTheta[rad], nBins1D() )
            -> fill( mcCKTheta, stdBaseDiffDiff );
          richHisto1D( hid(rad,"stdBaseMCDiffDiff"),
                       "|StdRec-MC| - |BaseRec-MC| CK Theta",
                       -ckDiffDiff, ckDiffDiff, nBins1D() )
            -> fill( stdBaseDiffDiff );
          richHisto2D( hid(rad,"stdMCDiffVbaseMCDIff"),
                       "|StdRec-MC| CK Theta Versus |BaseRec-MC| CK Theta",
                       minCkTheta[rad], maxCkTheta[rad], nBins1D(),
                       minCkTheta[rad], maxCkTheta[rad], nBins1D() )
            -> fill( baseCKTheta, stdCKTheta );

          // true PID type
          richHisto1D( hid(rad,mcType,"stdRecMCDiff"),
                       "StdRec-MC CK theta",
                       -ckRange[rad], ckRange[rad], nBins1D() ) -> fill( stdMCDiff );
          richHisto1D( hid(rad,mcType,"baseRecMCDiff"),
                       "BaseRec-MC CK theta",
                       -ckRange[rad], ckRange[rad], nBins1D() ) -> fill( baseMCDiff );
          richProfile1D( hid(rad,mcType,"stdBaseMCDiffDiffVP"),
                         "|StdRec-MC| - |BaseRec-MC| CK Theta V P",
                         minP, maxP, nBins1D() )
            -> fill( pTot, stdBaseDiffDiff );
          richProfile1D( hid(rad,mcType,"stdBaseMCDiffDiffVCKTheta"),
                         "|StdRec-MC| - |BaseRec-MC| CK Theta V CK Theta True",
                         minCkTheta[rad], maxCkTheta[rad], nBins1D() )
            -> fill( mcCKTheta, stdBaseDiffDiff );
          richHisto1D( hid(rad,mcType,"stdBaseMCDiffDiff"),
                       "|StdRec-MC| - |BaseRec-MC| CK Theta",
                       -ckDiffDiff, ckDiffDiff, nBins1D() )
            -> fill( stdBaseDiffDiff );
          richHisto2D( hid(rad,mcType,"stdMCDiffVbaseMCDIff"),
                       "|StdRec-MC| CK Theta Versus |BaseRec-MC| CK Theta",
                       minCkTheta[rad], maxCkTheta[rad], nBins1D(),
                       minCkTheta[rad], maxCkTheta[rad], nBins1D() )
            -> fill( baseCKTheta, stdCKTheta );

          // Detailed plots
          if ( UNLIKELY(m_detailedPlots) )
          {

            // P binned plots
            if ( pTot >= minP && pTot <= maxP )
            {

              // Get bin min and max
              const auto incr   = ( maxP - minP ) / nBins1D();
              const auto bin    = std::floor( ( pTot - minP ) / incr );
              const auto binmin = ( (  bin * incr ) + minP ) / Gaudi::Units::GeV;
              const auto binmax = binmin + ( incr / Gaudi::Units::GeV );

              // Form ID and title
              std::ostringstream id, title;
              id    << "Ptot/Bin" << bin << "-" << binmin << "to" << binmax << "GeV";
              title << " | Ptot " << binmin << " to " << binmax << " GeV/c";

              // Fill plots

              richHisto1D( hid(rad,id.str()+"/stdRecMCDiff"),
                           "StdRec-MC CK theta" + title.str(),
                           -ckRange[rad], ckRange[rad], nBins1D() ) -> fill( stdMCDiff );
              richHisto1D( hid(rad,mcType,id.str()+"/stdRecMCDiff"),
                           "StdRec-MC CK theta" + title.str(),
                           -ckRange[rad], ckRange[rad], nBins1D() ) -> fill( stdMCDiff );
              richHisto1D( hid(rad,id.str()+"/baseRecMCDiff"),
                           "BaseRec-MC CK theta" + title.str(),
                           -ckRange[rad], ckRange[rad], nBins1D() ) -> fill( baseMCDiff );
              richHisto1D( hid(rad,mcType,id.str()+"/baseRecMCDiff"),
                           "BaseRec-MC CK theta" + title.str(),
                           -ckRange[rad], ckRange[rad], nBins1D() ) -> fill( baseMCDiff );
              richHisto1D( hid(rad,id.str()+"/stdBaseMCDiffDiff"),
                           "|StdRec-MC| - |BaseRec-MC| CK Theta" + title.str(),
                           -ckDiffDiff, ckDiffDiff, nBins1D() )
                -> fill( stdBaseDiffDiff );
              richHisto1D( hid(rad,mcType,id.str()+"/stdBaseMCDiffDiff"),
                           "|StdRec-MC| - |BaseRec-MC| CK Theta" + title.str(),
                           -ckDiffDiff, ckDiffDiff, nBins1D() )
                -> fill( stdBaseDiffDiff );

            }

            // CK theta binned plots
            if ( mcCKTheta >= minCkTheta[rad] && mcCKTheta <= maxCkTheta[rad] )
            {

              // Get bin min and max
              const auto incr   = ( maxCkTheta[rad] - minCkTheta[rad] ) / nBins1D();
              const auto bin    = std::floor( ( mcCKTheta - minCkTheta[rad] ) / incr );
              const auto binmin = 1000 * ( (  bin * incr ) + minCkTheta[rad] );
              const auto binmax = binmin + ( 1000 * incr );

              // Form ID and title
              std::ostringstream id, title;
              id    << "CKtheta/Bin" << bin << "-" << binmin << "to" << binmax << "mrad";
              title << " | CKTheta " << binmin << " to " << binmax << " mrad";

              // Fill plots
              richHisto1D( hid(rad,id.str()+"/stdRecMCDiff"),
                           "StdRec-MC CK theta" + title.str(),
                           -ckRange[rad], ckRange[rad], nBins1D() ) -> fill( stdMCDiff );
              richHisto1D( hid(rad,mcType,id.str()+"/stdRecMCDiff"),
                           "StdRec-MC CK theta" + title.str(),
                           -ckRange[rad], ckRange[rad], nBins1D() ) -> fill( stdMCDiff );
              richHisto1D( hid(rad,id.str()+"/baseRecMCDiff"),
                           "BaseRec-MC CK theta" + title.str(),
                           -ckRange[rad], ckRange[rad], nBins1D() ) -> fill( baseMCDiff );
              richHisto1D( hid(rad,mcType,id.str()+"/baseRecMCDiff"),
                           "BaseRec-MC CK theta" + title.str(),
                           -ckRange[rad], ckRange[rad], nBins1D() ) -> fill( baseMCDiff );
              richHisto1D( hid(rad,id.str()+"/stdBaseMCDiffDiff"),
                           "|StdRec-MC| - |BaseRec-MC| CK Theta" + title.str(),
                           -ckDiffDiff, ckDiffDiff, nBins1D() )
                -> fill( stdBaseDiffDiff );
              richHisto1D( hid(rad,mcType,id.str()+"/stdBaseMCDiffDiff"),
                           "|StdRec-MC| - |BaseRec-MC| CK Theta" + title.str(),
                           -ckDiffDiff, ckDiffDiff, nBins1D() )
                -> fill( stdBaseDiffDiff );

            }

          } // detailed plots

          // Look in detail at photons which differ a lot
          if ( UNLIKELY( m_badPhotonPlots && stdBaseDiffDiff > maxDev[rad] ) )
          {

            // Track momentum
            richHisto1D( hid(rad,"BadPhotons/Ptot"), "Bad Photons | Ptot",
                         minP, maxP, nBins1D() ) -> fill( pTot );
            richHisto1D( hid(rad,mcType,"BadPhotons/Ptot"), "Bad Photons | Ptot",
                         minP, maxP, nBins1D() ) -> fill( pTot );

            // CK theta
            richHisto1D( hid(rad,"BadPhotons/CKtheta"), "Bad Photons | Cherenkov Theta",
                         minCkTheta[rad], maxCkTheta[rad], nBins1D() ) -> fill( mcCKTheta );
            richHisto1D( hid(rad,mcType,"BadPhotons/CKtheta"), "Bad Photons | Cherenkov Theta",
                         minCkTheta[rad], maxCkTheta[rad], nBins1D() ) -> fill( mcCKTheta );

            // Pixel for this photon
            const auto * pixel = photon->richRecPixel();

            // global position
            const auto & gPos = pixel->globalPosition();
            // local position on HPD panels
            const auto & lPos = pixel->localPosition();

            // Position plots - global
            richHisto1D( hid(rad,"BadPhotons/globalX"), "Bad Photons | Global Pixel x",
                         xMinPDGlo[rich], xMaxPDGlo[rich], nBins1D() ) -> fill( gPos.x() );
            richHisto1D( hid(rad,mcType,"BadPhotons/globalX"), "Bad Photons | Global Pixel x",
                         xMinPDGlo[rich], xMaxPDGlo[rich], nBins1D() ) -> fill( gPos.x() );
            richHisto1D( hid(rad,"BadPhotons/globalY"), "Bad Photons | Global Pixel y",
                         yMinPDGlo[rich], yMaxPDGlo[rich], nBins1D() ) -> fill( gPos.y() );
            richHisto1D( hid(rad,mcType,"BadPhotons/globalY"), "Bad Photons | Global Pixel y",
                         yMinPDGlo[rich], yMaxPDGlo[rich], nBins1D() ) -> fill( gPos.y() );
            richHisto1D( hid(rad,"BadPhotons/globalZ"), "Bad Photons | Global Pixel z",
                         zMinPDGlo[rich], zMaxPDGlo[rich], nBins1D() ) -> fill( gPos.z() );
            richHisto1D( hid(rad,mcType,"BadPhotons/globalZ"), "Bad Photons | Global Pixel z",
                         zMinPDGlo[rich], zMaxPDGlo[rich], nBins1D() ) -> fill( gPos.z() );
            richHisto2D( hid(rad,"BadPhotons/globalXY"), "Bad Photons | Global Pixel x v y",
                         xMinPDGlo[rich], xMaxPDGlo[rich], nBins2D(),
                         yMinPDGlo[rich], yMaxPDGlo[rich], nBins2D() ) -> fill( gPos.x(), gPos.y() );
            richHisto2D( hid(rad,mcType,"BadPhotons/globalXY"), "Bad Photons | Global Pixel x v y",
                         xMinPDGlo[rich], xMaxPDGlo[rich], nBins2D(),
                         yMinPDGlo[rich], yMaxPDGlo[rich], nBins2D() ) -> fill( gPos.x(), gPos.y() );

            // Position plots - local
            richHisto1D( hid(rad,"BadPhotons/localX"), "Bad Photons | Local Pixel x",
                         xMinPDLoc[rich], xMaxPDLoc[rich], nBins1D() ) -> fill( lPos.x() );
            richHisto1D( hid(rad,mcType,"BadPhotons/localX"), "Bad Photons | Local Pixel x",
                         xMinPDLoc[rich], xMaxPDLoc[rich], nBins1D() ) -> fill( lPos.x() );
            richHisto1D( hid(rad,"BadPhotons/localY"), "Bad Photons | Local Pixel y",
                         yMinPDLoc[rich], yMaxPDLoc[rich], nBins1D() ) -> fill( lPos.y() );
            richHisto1D( hid(rad,mcType,"BadPhotons/localY"), "Bad Photons | Local Pixel y",
                         yMinPDLoc[rich], yMaxPDLoc[rich], nBins1D() ) -> fill( lPos.y() );
            richHisto1D( hid(rad,"BadPhotons/localZ"), "Bad Photons | Local Pixel z",
                         zMinPDLoc[rich], zMaxPDLoc[rich], nBins1D() ) -> fill( lPos.z() );
            richHisto1D( hid(rad,mcType,"BadPhotons/localZ"), "Bad Photons | Local Pixel z",
                         zMinPDLoc[rich], zMaxPDLoc[rich], nBins1D() ) -> fill( lPos.z() );
            richHisto2D( hid(rad,"BadPhotons/localXY"), "Bad Photons | Local Pixel x v y",
                         xMinPDLoc[rich], xMaxPDLoc[rich], nBins2D(),
                         yMinPDLoc[rich], yMaxPDLoc[rich], nBins2D() ) -> fill( lPos.x(), lPos.y() );
            richHisto2D( hid(rad,mcType,"BadPhotons/localXY"), "Bad Photons | Local Pixel x v y",
                         xMinPDLoc[rich], xMaxPDLoc[rich], nBins2D(),
                         yMinPDLoc[rich], yMaxPDLoc[rich], nBins2D() ) -> fill( lPos.x(), lPos.y() );

            // base reco emission point plots
            richHisto1D( hid(rad,"BadPhotons/baseEmissX"), "Bad Photons | Base Emission point x",
                         -xRadExitGlo[rad], xRadExitGlo[rad], nBins1D() ) -> fill( baseEmissPtn.x() );
            richHisto1D( hid(rad,mcType,"BadPhotons/baseEmissX"), "Bad Photons | Base Emission point x",
                         -xRadExitGlo[rad], xRadExitGlo[rad], nBins1D() ) -> fill( baseEmissPtn.x() );
            richHisto1D( hid(rad,"BadPhotons/baseEmissY"), "Bad Photons | Base Emission point y",
                         -yRadExitGlo[rad], yRadExitGlo[rad], nBins1D() ) -> fill( baseEmissPtn.y() );
            richHisto1D( hid(rad,mcType,"BadPhotons/baseEmissY"), "Bad Photons | Base Emission point y",
                         -yRadExitGlo[rad], yRadExitGlo[rad], nBins1D() ) -> fill( baseEmissPtn.y() );
            richHisto1D( hid(rad,"BadPhotons/baseEmissZ"), "Bad Photons | Base Emission point z",
                         zRadEntGlo[rad], zRadExitGlo[rad], nBins1D() ) -> fill( baseEmissPtn.z() );
            richHisto1D( hid(rad,mcType,"BadPhotons/baseEmissZ"), "Bad Photons | Base Emission point z",
                         zRadEntGlo[rad], zRadExitGlo[rad], nBins1D() ) -> fill( baseEmissPtn.z() );

            richHisto2D( hid(rad,"BadPhotons/BaseEmissXY"), "Bad Photons | Base emission point x v y",
                         -xRadExitGlo[rad], xRadExitGlo[rad], nBins2D(),
                         -yRadExitGlo[rad], yRadExitGlo[rad], nBins2D() ) -> fill( baseEmissPtn.x(),
                                                                                   baseEmissPtn.y() );

            // mc reco emission point plots
            richHisto1D( hid(rad,"BadPhotons/mcEmissX"), "Bad Photons | MC Emission point x",
                         -xRadExitGlo[rad], xRadExitGlo[rad], nBins1D() ) -> fill( mcEmissPtn.x() );
            richHisto1D( hid(rad,mcType,"BadPhotons/mcEmissX"), "Bad Photons | MC Emission point x",
                         -xRadExitGlo[rad], xRadExitGlo[rad], nBins1D() ) -> fill( mcEmissPtn.x() );
            richHisto1D( hid(rad,"BadPhotons/mcEmissY"), "Bad Photons | MC Emission point y",
                         -yRadExitGlo[rad], yRadExitGlo[rad], nBins1D() ) -> fill( mcEmissPtn.y() );
            richHisto1D( hid(rad,mcType,"BadPhotons/mcEmissY"), "Bad Photons | MC Emission point y",
                         -yRadExitGlo[rad], yRadExitGlo[rad], nBins1D() ) -> fill( mcEmissPtn.y() );
            richHisto1D( hid(rad,"BadPhotons/mcEmissZ"), "Bad Photons | MC Emission point z",
                         zRadEntGlo[rad], zRadExitGlo[rad], nBins1D() ) -> fill( mcEmissPtn.z() );
            richHisto1D( hid(rad,mcType,"BadPhotons/mcEmissZ"), "Bad Photons | MC Emission point z",
                         zRadEntGlo[rad], zRadExitGlo[rad], nBins1D() ) -> fill( mcEmissPtn.z() );

            richHisto2D( hid(rad,"BadPhotons/mcEmissXY"), "Bad Photons | MC emission point x v y",
                         -xRadExitGlo[rad], xRadExitGlo[rad], nBins2D(),
                         -yRadExitGlo[rad], yRadExitGlo[rad], nBins2D() ) -> fill( mcEmissPtn.x(),
                                                                                   mcEmissPtn.y() );

          }


        }

      }

    }

  }

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( PhotonRecoCompare )

//---------------------------------------------------------------------------
