
//---------------------------------------------------------------------------
/** @file RichStereoPhotonFitTest.cpp
 *
 *  Implementation file for algorithm class : StereoPhotonFitTest
 *
 *  $Id: RichStereoPhotonFitTest.cpp,v 1.1 2008-05-01 19:42:24 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

// local
#include "RichStereoPhotonFitTest.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// namespace
using namespace Rich::Rec::MC;

//---------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( StereoPhotonFitTest );

// Standard constructor, initializes variables
StereoPhotonFitTest::StereoPhotonFitTest( const std::string& name,
                                          ISvcLocator* pSvcLocator )
  : RichRecHistoAlgBase ( name, pSvcLocator ),
    m_fitter            ( NULL ),
    m_trSelector        ( NULL ),
    m_ckAngle           ( NULL )
{
  declareProperty( "NumberBins", m_nBins = 100 );
  declareProperty( "ChThetaRecHistoLimitMin",
                   m_ckThetaMin = boost::assign::list_of(0.1)(0.03)(0.01) );
  declareProperty( "ChThetaRecHistoLimitMax",
                   m_ckThetaMax = boost::assign::list_of(0.3)(0.08)(0.05) );
  declareProperty( "CKResHistoRange",
                   m_ckResRange = boost::assign::list_of(0.01)(0.005)(0.0025) );
}

// Destructor
StereoPhotonFitTest::~StereoPhotonFitTest() { }

//  Initialize
StatusCode StereoPhotonFitTest::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecHistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichStereoFitter",   m_fitter, this );
  acquireTool( "TrackSelector",      m_trSelector, this );
  acquireTool( "RichCherenkovAngle", m_ckAngle );

  return sc;
}

// Main execution
StatusCode StereoPhotonFitTest::execute()
{
  debug() << "Execute" << endreq;

  // Check event status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Make sure all tracks and segments have been formed
  if ( richTracks()->empty() )
  {
    if ( !trackCreator()->newTracks() ) return StatusCode::FAILURE;
    debug() << "No tracks found : Created " << richTracks()->size()
            << " RichRecTracks " << richSegments()->size()
            << " RichRecSegments" << endreq;
  }

  // Rich Histo ID
  const RichHistoID hid;

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
              << endreq;

    // Is Fit 'OK' ?? Skip failed fits
    if ( fitR.status != IStereoFitter::Result::Succeeded ) continue;

    // Compute the simple 'average' CK theta value
    double segAvgCKtheta(0);
    if ( !segment->richRecPhotons().empty() )
    {
      for ( LHCb::RichRecSegment::Photons::const_iterator iPhot
              = segment->richRecPhotons().begin();
            iPhot != segment->richRecPhotons().end(); ++iPhot )
      {
        segAvgCKtheta += (*iPhot)->geomPhoton().CherenkovTheta();
      }
      segAvgCKtheta /= (double)(segment->richRecPhotons().size());
    }

    // Expected Cherenkov theta angle
    const double thetaExp = m_ckAngle->avgCherenkovTheta( segment, pid );

    // some plots
    plot1D( fitR.chi2, hid(rad,"fitChi2"), "Fit Chi^2", 0, 100, m_nBins );
    plot1D( fitR.probChi2, hid(rad,"fitProbChi2"), "Fit Chi^2 Probability", 0, 1, m_nBins );
    plot1D( fitR.ndof, hid(rad,"fitNDOF"), "Fit NDOF", 0, 100, m_nBins );

    plot1D( fitR.thcFit, hid(rad,"fittedCKtheta"), "Fitted CK theta : All photons",
            m_ckThetaMin[rad], m_ckThetaMax[rad], m_nBins );
    plot1D( fitR.thcFit-thetaExp, hid(rad,"fittedCKthetaRes"),
            "Fitted CK theta - expected CK theta : All photons",
            -m_ckResRange[rad], m_ckResRange[rad], m_nBins );

    plot1D( segAvgCKtheta, hid(rad,"avgCKtheta"), "Simple Avg. CK theta : All photons",
            m_ckThetaMin[rad], m_ckThetaMax[rad], m_nBins );
    plot1D( segAvgCKtheta-thetaExp, hid(rad,"avgCKthetaRes"),
            "Simple Avg. CK theta - expected CK theta : All photons",
            -m_ckResRange[rad], m_ckResRange[rad], m_nBins );

  } // end segment loop

  return StatusCode::SUCCESS;
}

