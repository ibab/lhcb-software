
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
  : HistoAlgBase        ( name, pSvcLocator ),
    m_fitter            ( NULL ),
    m_trSelector        ( NULL ),
    m_ckAngle           ( NULL ),
    m_ckRes             ( NULL ),
    m_richRecMCTruth    ( NULL )
{
  std::vector<double> t1 = boost::assign::list_of(0.1)(0.03)(0.01);
  std::vector<double> t2 = boost::assign::list_of(0.3)(0.08)(0.05);
  std::vector<double> t3 = boost::assign::list_of(0.01)(0.004)(0.002);
  
  declareProperty( "ChThetaRecHistoLimitMin", m_ckThetaMin = t1 );
  declareProperty( "ChThetaRecHistoLimitMax", m_ckThetaMax = t2 );
  declareProperty( "CKResHistoRange",         m_ckResRange = t3 );
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
  debug() << "Execute" << endmsg;

  // Check event status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Make sure all tracks and segments have been formed
  if ( richTracks()->empty() )
  {
    if ( !trackCreator()->newTracks() ) return StatusCode::FAILURE;
    debug() << "No tracks found : Created " << richTracks()->size()
            << " RichRecTracks " << richSegments()->size()
            << " RichRecSegments" << endmsg;
  }

  // Rich Histo ID
  const Rich::HistoID hid;

  // Iterate over segments
  for ( LHCb::RichRecSegments::iterator iSeg = richSegments()->begin();
        iSeg != richSegments()->end(); ++iSeg )
  {
    LHCb::RichRecSegment * segment = *iSeg;

    // track selection
    if ( !m_trSelector->trackSelected(segment->richRecTrack()) ) continue;

    // Radiator info
    const Rich::RadiatorType rad = segment->trackSegment().radiator();

    // skip aerogel for the moment ...
    if ( rad == Rich::Aerogel ) continue;

    // mass hypo
    const Rich::ParticleIDType pid = segment->richRecTrack()->currentHypothesis();

    // do the fit
    IStereoFitter::Configuration config(pid);
    const IStereoFitter::Result fitR = m_fitter->Fit( segment, config );

    if ( msgLevel(MSG::DEBUG) )
      debug() << "RichRecSegment " << segment->key()
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
      for ( LHCb::RichRecSegment::Photons::const_iterator iPhot
              = segment->richRecPhotons().begin();
            iPhot != segment->richRecPhotons().end(); ++iPhot )
      {
        if ( fabs( (*iPhot)->geomPhoton().CherenkovTheta() - thetaExp ) <
             nSigma * expCKres )
        {
          ++nPhots;
          segAvgCKtheta += (*iPhot)->geomPhoton().CherenkovTheta();
          // true CK photon ?
          const LHCb::MCRichOpticalPhoton * mcPhot = m_richRecMCTruth->trueOpticalPhoton(*iPhot);
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

