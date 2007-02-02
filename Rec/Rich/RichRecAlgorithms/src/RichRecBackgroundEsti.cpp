
//--------------------------------------------------------------------------
/** @file RichRecBackgroundEsti.cpp
 *
 *  Implementation file for algorithm class : Rich::Rec::BackgroundEsti
 *
 *  CVS Log :-
 *  $Id: RichRecBackgroundEsti.cpp,v 1.6 2007-02-02 10:05:50 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   17/04/2002
 */
//--------------------------------------------------------------------------

// local
#include "RichRecBackgroundEsti.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//--------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( BackgroundEsti );

// Standard constructor, initializes variables
BackgroundEsti::BackgroundEsti( const std::string& name,
                                ISvcLocator* pSvcLocator )
  : RichRecAlgBase ( name, pSvcLocator ),
    m_tkSignal     ( NULL )
{

  // Maximum number of iterations in background normalisation
  declareProperty( "MaxBackgroundNormIterations", m_maxBkgIterations = 10 );

  // Number of active pixels per PD - To be got from XML eventually
  declareProperty( "PixelsPerPD", m_nPixelsPerPD = 784.763611 );

}

// Destructor
BackgroundEsti::~BackgroundEsti() {}

// Initialize
StatusCode BackgroundEsti::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecAlgBase::initialize();
  if ( sc.isFailure() ) { return Error( "Failed to initialize base class", sc ); }

  // Acquire instances of tools
  acquireTool( "RichExpectedTrackSignal", m_tkSignal );

  return sc;
}

StatusCode BackgroundEsti::execute()
{
  if (msgLevel(MSG::DEBUG)) debug() << "Execute" << endreq;

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

  // Get detector backgrounds
  std::vector<LHCb::RichRecStatus::FloatType> bckEstimate = richStatus()->detOverallBkg();

  // Initialise detector backgrounds
  bckEstimate[Rich::Rich1] = 0;
  bckEstimate[Rich::Rich2] = 0;
  typedef Rich::Map<LHCb::RichSmartID,double> PDsignals;
  PDsignals obsPDsignals[Rich::NRiches];
  PDsignals expPDsignals[Rich::NRiches];
  PDsignals expPDbkg    [Rich::NRiches];
  std::vector<double> sigRich(Rich::NRiches,0);
  std::vector<double> expRich(Rich::NRiches,0);

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
      (expPDsignals[rich])[ LHCb::RichSmartID(iPD->first) ] += sig;
      expRich[rich] += sig;
    }

  } // end RichRecSegment loop

    // Loop over pixels
  {for ( LHCb::RichRecPixels::iterator pixel = richPixels()->begin();
         pixel != richPixels()->end(); ++pixel )
  {
    // count the number of hits in each HPD, in each RICH
    ++(obsPDsignals[ (*pixel)->smartID().rich() ])[ (*pixel)->smartID().hpdID() ];
    // count the total number of hits in each RICH
    ++sigRich[(*pixel)->smartID().rich()];
  }} // end pixel loop

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Expected signals    RICH1/2 : " << expRich << endreq
            << "Observed signals    RICH1/2 : " << sigRich << endreq;
  }

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
      for ( PDsignals::const_iterator iPD = obsPDsignals[*iRich].begin();
            iPD !=  obsPDsignals[*iRich].end();
            ++iPD )
      {
        const LHCb::RichSmartID pd = iPD->first;
        const double obs           = iPD->second;
        const double exp           = (expPDsignals[*iRich])[pd];
        double & bkg               = (expPDbkg[*iRich])[pd];

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
    for ( PDsignals::iterator iPD = expPDbkg[*iRich].begin();
          iPD !=  expPDbkg[*iRich].end(); ++iPD )
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

    // Loop over pixels again to set background term
  {for ( LHCb::RichRecPixels::iterator pixel = richPixels()->begin();
         pixel != richPixels()->end();
         ++pixel )
  {
    const LHCb::RichSmartID   pd = (*pixel)->smartID().hpdID();
    const Rich::DetectorType det = (*pixel)->smartID().rich();

    const double rbckexp = (obsPDsignals[det])[pd] - (expPDsignals[det])[pd];
    (*pixel)->setCurrentBackground( rbckexp>0 ? rbckexp/m_nPixelsPerPD : 0 );

    // Debug printout
    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << "Pixel " << (*pixel)->smartID()
                << " Obs "  << (obsPDsignals[det])[pd]
                << " Exp "  << (expPDsignals[det])[pd]
                << " bkg "  << (*pixel)->currentBackground()
                << endreq;
    }
  }}

  return StatusCode::SUCCESS;
}
