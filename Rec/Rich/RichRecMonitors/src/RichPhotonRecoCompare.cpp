
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
  : Rich::Rec::TupleAlgBase ( name, pSvcLocator ) { }

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
  const Rich::HistoID hid;

  // min and max P for histos
  //const double maxP = m_trSelector->maxPCut() * Gaudi::Units::GeV;
  //const double minP = m_trSelector->minPCut() * Gaudi::Units::GeV;
  const double maxP = 100 * Gaudi::Units::GeV;
  const double minP = 0   * Gaudi::Units::GeV;

  // Iterate over segments
  for ( auto * segment : *richSegments() )
  {
    // apply track selection
    if ( !m_trSelector->trackSelected(segment->richRecTrack()) ) continue;

    // segment momentum
    const auto pTot = std::sqrt(segment->trackSegment().bestMomentum().Mag2());

    // Radiator info
    const auto rad = segment->trackSegment().radiator();

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
        const LHCb::RichGeomPhoton & stdPhot = photon->geomPhoton();

        // Create on the fly a comparison photon using the baseline tool
        // starting off as a clone of the above
        LHCb::RichGeomPhoton basePhot;
        const auto sc = m_reco->reconstructPhoton( photon->richRecSegment(),
                                                   photon->richRecPixel(),
                                                   basePhot );
        if ( sc.isSuccess() )
        {
          // compare the photons ...

          // CK theta values
          const auto stdCKTheta  = stdPhot.CherenkovTheta();
          const auto baseCKTheta = basePhot.CherenkovTheta();
          const auto mcCKTheta   = mcPhot->cherenkovTheta();
          // differences
          const auto stdMCDiff  = stdCKTheta  - mcCKTheta;
          const auto baseMCDiff = baseCKTheta - mcCKTheta;
          const auto stdBaseDiffDiff = fabs(stdMCDiff) - fabs(baseMCDiff);

          // Emission point
          //const auto& stdEmissPtn  = stdPhot.emissionPoint();
          //const auto& baseEmissPtn = stdPhot.emissionPoint();
          //const auto& mcEmissPtn   = mcPhot->emissionPoint();

          // plots

          // all hypos
          richProfile1D( hid(rad,"stdBaseMCDiffDiffVP"),
                         "|StdRec-MC| - |BaseRec-MC| CK Theta V P",
                         minP, maxP, nBins1D() )
            -> fill( pTot, stdBaseDiffDiff );
          richProfile1D( hid(rad,"stdBaseMCDiffDiffVCKTheta"),
                         "|StdRec-MC| - |BaseRec-MC| CK Theta V CK Theta True",
                         minCkTheta[rad], maxCkTheta[rad], nBins1D() )
            -> fill( mcCKTheta, stdBaseDiffDiff );
          richHisto1D( hid(rad,"stdBaseMCDiffDiff"),
                       "|StdRec-MC| - |BaseRec-MC| CK Theta V P",
                       -ckRange[rad],ckRange[rad], nBins1D() )
            -> fill( stdBaseDiffDiff );
          richHisto2D( hid(rad,"stdMCDiffVbaseMCDIff"),
                       "|StdRec-MC| CK Theta Versue |BaseRec-MC| CK Theta",
                       minCkTheta[rad], maxCkTheta[rad], nBins1D(),
                       minCkTheta[rad], maxCkTheta[rad], nBins1D() )
            -> fill( baseCKTheta, stdCKTheta );   
          
          // true PID type
          richProfile1D( hid(rad,mcType,"stdBaseMCDiffDiffVP"),
                         "|StdRec-MC| - |BaseRec-MC| CK Theta V P",
                         minP, maxP, nBins1D() )
            -> fill( pTot, stdBaseDiffDiff );
          richProfile1D( hid(rad,mcType,"stdBaseMCDiffDiffVCKTheta"),
                         "|StdRec-MC| - |BaseRec-MC| CK Theta V CK Theta True",
                         minCkTheta[rad], maxCkTheta[rad], nBins1D() )
            -> fill( mcCKTheta, stdBaseDiffDiff );
          richHisto1D( hid(rad,mcType,"stdBaseMCDiffDiff"),
                       "|StdRec-MC| - |BaseRec-MC| CK Theta V P",
                       -ckRange[rad],ckRange[rad], nBins1D() )
            -> fill( stdBaseDiffDiff );
          richHisto2D( hid(rad,mcType,"stdMCDiffVbaseMCDIff"),
                       "|StdRec-MC| CK Theta Versue |BaseRec-MC| CK Theta",
                       minCkTheta[rad], maxCkTheta[rad], nBins1D(),
                       minCkTheta[rad], maxCkTheta[rad], nBins1D() )
            -> fill( baseCKTheta, stdCKTheta );

        }
        
      }
      
    }

  }

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( PhotonRecoCompare )

//---------------------------------------------------------------------------
