
//--------------------------------------------------------------------------
/** @file RichRecBackgroundEstiAvHPD.cpp
 *
 *  Implementation file for algorithm class : Rich::Rec::BackgroundEstiAvHPD
 *
 *  CVS Log :-
 *  $Id: RichRecBackgroundEstiAvHPD.cpp,v 1.2 2007-06-22 13:48:51 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   17/04/2002
 */
//--------------------------------------------------------------------------

// local
#include "RichRecBackgroundEstiAvHPD.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//--------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( BackgroundEstiAvHPD );

// Standard constructor, initializes variables
BackgroundEstiAvHPD::BackgroundEstiAvHPD( const std::string& name,
                                          ISvcLocator* pSvcLocator )
  : RichRecAlgBase   ( name, pSvcLocator ),
    m_tkSignal       ( NULL ),
    m_obsPDsignals   ( Rich::NRiches ),
    m_expPDsignals   ( Rich::NRiches ),
    m_expPDbkg       ( Rich::NRiches )
{
  // Maximum number of iterations in background normalisation
  declareProperty( "MaxBackgroundNormIterations", m_maxBkgIterations = 10 );

  // Number of active pixels per PD - To be got from XML eventually
  declareProperty( "PixelsPerPD", m_nPixelsPerPD = 784.763611 );

  // Minimum pixel background
  declareProperty( "MinimumPixelBackground", m_minPixBkg = 0.0 );
}

// Destructor
BackgroundEstiAvHPD::~BackgroundEstiAvHPD() {}

// Initialize
StatusCode BackgroundEstiAvHPD::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecAlgBase::initialize();
  if ( sc.isFailure() ) { return Error( "Failed to initialize base class", sc ); }

  // Acquire instances of tools
  acquireTool( "RichExpectedTrackSignal", m_tkSignal );

  return sc;
}

StatusCode BackgroundEstiAvHPD::execute()
{
  debug() << "Execute" << endreq;

  StatusCode sc = StatusCode::SUCCESS;

  // General init
  sc = richInit();
  if ( sc.isFailure() ) return sc;

  // get expected signals
  sc = fillExpectedSignalMap();
  if ( sc.isFailure() ) return sc;

  // get observed signals
  sc = fillObservedSignalMap();
  if ( sc.isFailure() ) return sc;

  // get the overal RICH backgrounds
  sc = overallRICHBackgrounds();
  if ( sc.isFailure() ) return sc;

  // compute the pixel backgrounds
  sc = pixelBackgrounds();

  return sc;
}

StatusCode BackgroundEstiAvHPD::richInit()
{
  // Event Status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Check segments and pixels
  if ( !richSegments() ) return Error( "Failed to access RichRecSegments" );
  if ( !richPixels()   ) return Error( "Failed to access RichRecPixels"   );
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Found " << richSegments()->size() << " RichRecSegments" << endreq
            << "Found " << richPixels()->size() << " RichRecPixels" << endreq;
  }

  // reset the working maps
  m_obsPDsignals[Rich::Rich1].clear();
  m_obsPDsignals[Rich::Rich2].clear();
  m_expPDsignals[Rich::Rich1].clear();
  m_expPDsignals[Rich::Rich2].clear();
  m_expPDbkg[Rich::Rich1].clear();
  m_expPDbkg[Rich::Rich2].clear();

  return StatusCode::SUCCESS;
}

StatusCode BackgroundEstiAvHPD::fillObservedSignalMap()
{
  // Loop over pixels
  for ( LHCb::RichRecPixels::iterator pixel = richPixels()->begin();
        pixel != richPixels()->end(); ++pixel )
  {
    // count the number of hits in each HPD, in each RICH
    ++(m_obsPDsignals[ (*pixel)->hpdPixelCluster().rich() ])[ (*pixel)->hpdPixelCluster().hpd() ];
  }

  return StatusCode::SUCCESS;
}

StatusCode BackgroundEstiAvHPD::fillExpectedSignalMap()
{
  // loop over segments
  for ( LHCb::RichRecSegments::iterator segment = richSegments()->begin();
        segment != richSegments()->end(); ++segment )
  {

    // Current best hypothesis for this track
    const Rich::ParticleIDType id = (*segment)->richRecTrack()->currentHypothesis();

    // Expected detectable emitted photons for this segment
    const double detPhots = m_tkSignal->nDetectablePhotons(*segment,id);

    // Tally total expected hits for each PD
    LHCb::RichRecSegment::PDGeomEffs & hypoMap = (*segment)->geomEfficiencyPerPD( id );
    for ( LHCb::RichRecSegment::PDGeomEffs::iterator iPD = hypoMap.begin();
          iPD != hypoMap.end();
          ++iPD )
    {
      const Rich::DetectorType rich = (*segment)->trackSegment().rich();
      const double sig = detPhots * iPD->second; // expected signal for this PD
      (m_expPDsignals[rich])[ LHCb::RichSmartID(iPD->first) ] += sig;
    }

  } // end RichRecSegment loop

  return StatusCode::SUCCESS;
}

StatusCode BackgroundEstiAvHPD::overallRICHBackgrounds()
{

  // Get detector backgrounds
  std::vector<LHCb::RichRecStatus::FloatType> bckEstimate = richStatus()->detOverallBkg();

  // Initialise detector backgrounds
  bckEstimate[Rich::Rich1] = 0;
  bckEstimate[Rich::Rich2] = 0;

  // Obtain background term PD by PD
  for ( Rich::Detectors::const_iterator iRich = detectors().begin();
        iRich != detectors().end(); ++iRich )
  {
    verbose() << "Computing HPD backgrounds in " << *iRich << endreq;

    int iter = 1;
    bool cont = true;
    double rnorm = 0.0;
    while ( cont )
    {
      verbose() << " -> Iteration " << iter << endreq;

      int nBelow(0), nAbove(0);
      double tBelow = 0.0;
      for ( PDsignals::const_iterator iPD = m_obsPDsignals[*iRich].begin();
            iPD !=  m_obsPDsignals[*iRich].end();
            ++iPD )
      {
        const LHCb::RichSmartID pd = iPD->first;
        const double obs           = iPD->second;
        const double exp           = (m_expPDsignals[*iRich])[pd];
        double & bkg               = (m_expPDbkg[*iRich])[pd];

        if ( 1 == iter )
        {
          // First iteration, just set background for this HPD to the difference
          // between the observed and and expected number of hits in the HPD
          verbose() << "  -> HPD " << pd << " obs. = " << obs << " exp. = " << exp << endreq;
          bkg = obs - exp;
        }
        else
        {
          // For additional interations apply the normalisation factor
          bkg = ( bkg > 0 ? bkg-rnorm : 0 );
        }

        if ( bkg < 0.0 )
        {
          // Count the number of HPDs below expectation for this iteration
          ++nBelow;
          // save the total amount below expectation
          tBelow += fabs( bkg );
        }
        else if ( bkg > 0.0 )
        {
          // count the number of HPDs above expectation
          ++nAbove;
        }

      } // end loop over signal PDs

      verbose() << " -> Above = " << nAbove << " Below = " << nBelow << endreq;

      if ( nBelow > 0 && nAbove > 0 )
      {
        // we have some HPDs above and below expectation
        // calculate the amount of signal below per above HPD
        rnorm = tBelow / ( static_cast<double>(nAbove) );
        verbose() << "  -> Correction factor per HPD above = " << rnorm << endreq;
      }
      else
      {
        verbose() << "  -> Aborting iterations" << endreq;
        cont = false;
      }
      if ( iter > m_maxBkgIterations ) cont = false;

      ++iter;
    } // end while loop

      // Finally, fill background estimates
    for ( PDsignals::iterator iPD = m_expPDbkg[*iRich].begin();
          iPD != m_expPDbkg[*iRich].end(); ++iPD )
    {
      if ( iPD->second < 0 ) iPD->second = 0;
      bckEstimate[ *iRich ] += iPD->second;
    }

  } // end rich loop

  // Update detector backgrounds
  richStatus()->setDetOverallBkg(bckEstimate);

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Overall backgrounds RICH1/2 : " << bckEstimate << endreq;
  }

  return StatusCode::SUCCESS;
}

StatusCode BackgroundEstiAvHPD::pixelBackgrounds()
{
  // Loop over pixels again to set background term
  for ( LHCb::RichRecPixels::iterator pixel = richPixels()->begin();
        pixel != richPixels()->end();
        ++pixel )
  {
    const LHCb::RichSmartID   pd = (*pixel)->hpdPixelCluster().hpd();
    const Rich::DetectorType det = (*pixel)->hpdPixelCluster().rich();

    // background for this HPD
    const double rbckexp = (m_obsPDsignals[det])[pd] - (m_expPDsignals[det])[pd];

    // Save this value in the pixel
    double bkg = ( rbckexp>0 ? rbckexp/m_nPixelsPerPD : 0 );
    if ( bkg < m_minPixBkg ) bkg = m_minPixBkg;
    (*pixel)->setCurrentBackground( bkg );

    // Debug printout
    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << "Pixel " << (*pixel)->hpdPixelCluster()
                << " rbckexp " << rbckexp
                << " Obs "  << (m_obsPDsignals[det])[pd]
                << " Exp "  << (m_expPDsignals[det])[pd]
                << " bkg "  << (*pixel)->currentBackground()
                << endreq;
    }

  } // loop over pixels

  return StatusCode::SUCCESS;
}
