
//---------------------------------------------------------------------------
/** @file RichStereoPhotonFitTest.cpp
 *
 *  Implementation file for algorithm class : StereoPhotonFitTest
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

// local
#include "RichStereoPhotonFitTest.h"

// namespace
using namespace Rich::Rec::MC;

//---------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( StereoPhotonFitTest )

// Standard constructor, initializes variables
StereoPhotonFitTest::StereoPhotonFitTest( const std::string& name,
                                          ISvcLocator* pSvcLocator )
  : HistoAlgBase( name, pSvcLocator )
{
  declareProperty( "ChThetaRecHistoLimitMin", m_ckThetaMin = { 0.1,  0.03,  0.01  } );
  declareProperty( "ChThetaRecHistoLimitMax", m_ckThetaMax = { 0.3,  0.08,  0.05  } );
  declareProperty( "CKResHistoRange",         m_ckResRange = { 0.01, 0.004, 0.002 } );
}

// Destructor
StereoPhotonFitTest::~StereoPhotonFitTest() { }

//  Initialize
StatusCode StereoPhotonFitTest::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = HistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichStereoFitter",   m_fitter, this );
  acquireTool( "TrackSelector",      m_trSelector, this );
  acquireTool( "RichCherenkovAngle", m_ckAngle );
  acquireTool( "RichCherenkovResolution", m_ckRes );
  acquireTool( "RichRecMCTruthTool", m_richRecMCTruth );

  return sc;
}

// Main execution
StatusCode StereoPhotonFitTest::execute()
{
  _ri_debug << "Execute" << endmsg;

  // Check event status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Make sure all tracks and segments have been formed
  if ( richTracks()->empty() )
  {
    if ( !trackCreator()->newTracks() ) return StatusCode::FAILURE;
    _ri_debug << "No tracks found : Created " << richTracks()->size()
              << " RichRecTracks " << richSegments()->size()
              << " RichRecSegments" << endmsg;
  }

  // Rich Histo ID
  const Rich::HistoID hid;

  // Iterate over segments
  for ( auto * segment : *richSegments() )
  {

    // Radiator info
    const auto rad = segment->trackSegment().radiator();

    // skip aerogel for the moment ...
    if ( rad == Rich::Aerogel ) continue;

    // track selection
    if ( !m_trSelector->trackSelected(segment->richRecTrack()) ) continue;

    // mass hypo
    const auto pid = segment->richRecTrack()->currentHypothesis();

    // do the fit
    IStereoFitter::Configuration config(pid);
    const IStereoFitter::Result fitR = m_fitter->Fit( segment, config );

    _ri_debug << "RichRecSegment " << segment->key()
              << " " << rad
              << " " << pid
              << " : FitResult= " << fitR
              << endmsg;

    // Is Fit 'OK' ?? Skip failed fits
    const bool fitOK = fitR.status == IStereoFitter::Result::Succeeded;
    plot1D( (int)fitOK, hid(rad,"fitStatus"), "Fit Status (1=OK, 0=FAIL)",
            -0.5, 1.5, 2 );
    if ( !fitOK ) continue;

    // Expected Cherenkov theta angle
    const double thetaExp = m_ckAngle->avgCherenkovTheta( segment, pid );
    // expected CK theta resolution
    const double expCKres = m_ckRes->ckThetaResolution(segment,pid);

    // Compute the simple 'average' CK theta value
    double segAvgCKtheta(0), segAvgCKthetaMC(0);
    unsigned int nPhots(0), nPhotsMC(0);
    if ( !segment->richRecPhotons().empty() )
    {
      const double nSigma(3.0);
      for ( const auto * phot : segment->richRecPhotons() )
      {
        if ( fabs( phot->geomPhoton().CherenkovTheta() - thetaExp ) <
             nSigma * expCKres )
        {
          ++nPhots;
          segAvgCKtheta += phot->geomPhoton().CherenkovTheta();
          // true CK photon ?
          const auto * mcPhot = m_richRecMCTruth->trueOpticalPhoton(phot);
          if ( mcPhot )
          {
            ++nPhotsMC;
            segAvgCKthetaMC += mcPhot->cherenkovTheta();
          }
        }
      }
      if ( nPhots>0   ) segAvgCKtheta   /= (double)(nPhots  );
      if ( nPhotsMC>0 ) segAvgCKthetaMC /= (double)(nPhotsMC);
    }

    // some plots
    plot1D( fitR.chi2, hid(rad,"fitChi2"), "Fit Chi^2", 0, 100, nBins1D() );
    plot1D( fitR.probChi2, hid(rad,"fitProbChi2"), "Fit Chi^2 Probability", 0, 1, nBins1D() );
    plot1D( fitR.ndof, hid(rad,"fitNDOF"), "Fit NDOF", 0, 100, nBins1D() );

    plot1D( fitR.thcFit, hid(rad,"fittedCKtheta"), "Fitted CK theta : All photons",
            m_ckThetaMin[rad], m_ckThetaMax[rad], nBins1D() );
    plot1D( fitR.thcFit-thetaExp, hid(rad,"fittedCKthetaRes"),
            "Fitted CK theta - expected CK theta : All photons",
            -m_ckResRange[rad], m_ckResRange[rad], nBins1D() );
    plot1D( fitR.thcFit-segAvgCKthetaMC, hid(rad,"fittedCKthetaResMC"),
            "Fitted CK theta - Average MC theta : All photons",
            -m_ckResRange[rad], m_ckResRange[rad], nBins1D() );

    plot1D( segAvgCKtheta, hid(rad,"avgCKtheta"), "Simple Avg. CK theta : All photons",
            m_ckThetaMin[rad], m_ckThetaMax[rad], nBins1D() );
    plot1D( segAvgCKtheta-thetaExp, hid(rad,"avgCKthetaRes"),
            "Simple Avg. CK theta - expected CK theta : All photons",
            -m_ckResRange[rad], m_ckResRange[rad], nBins1D() );
    plot1D( segAvgCKtheta-segAvgCKthetaMC, hid(rad,"avgCKthetaResMC"),
            "Simple Avg. CK theta - Average MC theta : All photons",
            -m_ckResRange[rad], m_ckResRange[rad], nBins1D() );


  } // end segment loop

  return StatusCode::SUCCESS;
}

