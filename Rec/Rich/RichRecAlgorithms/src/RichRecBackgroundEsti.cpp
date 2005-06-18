
//--------------------------------------------------------------------------
/** @file RichRecBackgroundEsti.cpp
 *
 *  Implementation file for algorithm class : RichRecBackgroundEsti
 *
 *  CVS Log :-
 *  $Id: RichRecBackgroundEsti.cpp,v 1.1.1.1 2005-06-18 08:42:43 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   17/04/2002
 */
//--------------------------------------------------------------------------

// local
#include "RichRecBackgroundEsti.h"

//--------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichRecBackgroundEsti>          s_factory ;
const        IAlgFactory& RichRecBackgroundEstiFactory = s_factory ;

// Standard constructor, initializes variables
RichRecBackgroundEsti::RichRecBackgroundEsti( const std::string& name,
                                              ISvcLocator* pSvcLocator )
  : RichRecAlgBase ( name, pSvcLocator ),
    m_tkSignal     ( 0 )
{

  // Maximum number of iterations in background normalisation
  declareProperty( "MaxBackgroundNormIterations", m_maxBkgIterations = 10 );

  // Number of active pixels per PD - To be got from XML eventually
  declareProperty( "PixelsPerPD", m_nPixelsPerPD = 784.763611 );

}

// Destructor
RichRecBackgroundEsti::~RichRecBackgroundEsti() {}

// Initialize
StatusCode RichRecBackgroundEsti::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichExpectedTrackSignal", m_tkSignal );

  return sc;
}


StatusCode RichRecBackgroundEsti::execute()
{
  debug() << "Execute" << endreq;

  // Event Status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Get reference to detector backgrounds
  std::vector<RichRecStatus::FloatType> & bckEstimate = richStatus()->detOverallBkg();

  // Initialise detector backgrounds
  bckEstimate[Rich::Rich1] = 0;
  bckEstimate[Rich::Rich2] = 0;
  PDsignals obsPDsignals[Rich::NRiches];
  PDsignals expPDsignals[Rich::NRiches];
  PDsignals expPDbkg    [Rich::NRiches];
  std::vector<double> sigRich(Rich::NRiches);
  sigRich[Rich::Rich1] = 0; sigRich[Rich::Rich2] = 0;
  std::vector<double> expRich(Rich::NRiches);
  expRich[Rich::Rich1] = 0; expRich[Rich::Rich2] = 0;

  // loop over segments
  if ( !richSegments() ) return StatusCode::FAILURE;
  debug() << "Found " << richSegments()->size() << " RichRecSegments" << endreq;
  for ( RichRecSegments::iterator segment = richSegments()->begin();
        segment != richSegments()->end(); ++segment ) 
  {

    // Current best hypothesis for this track
    const Rich::ParticleIDType id = (*segment)->richRecTrack()->currentHypothesis();

    // Expected detectable emitted photons for this segment
    const double detPhots = m_tkSignal->nDetectablePhotons(*segment,id);

    // Tally total expected hits for each PD
    RichRecSegment::PDGeomEffs & hypoMap = (*segment)->geomEfficiencyPerPD( id );
    for ( RichRecSegment::PDGeomEffs::iterator iPD = hypoMap.begin();
          iPD != hypoMap.end();
          ++iPD ) 
    {
      const Rich::DetectorType rich = (*segment)->trackSegment().rich();
      const double sig = detPhots * iPD->second; // expected signal for this PD
      (expPDsignals[rich])[iPD->first] += sig;
      expRich[rich] += sig;
    }

  } // end RichRecSegment loop

    // Loop over pixels
  if ( !richPixels() ) return StatusCode::FAILURE;
  debug() << "Found " << richPixels()->size() << " RichRecPixels" << endreq;
  {for ( RichRecPixels::iterator pixel = richPixels()->begin();
         pixel != richPixels()->end(); ++pixel ) 
  {
    ++(obsPDsignals[ (*pixel)->smartID().rich() ])[ (*pixel)->smartID().pdID() ];
    ++sigRich[(*pixel)->smartID().rich()];
  }} // end pixel loop

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Expected signals    RICH1/2 : " << expRich << endreq
            << "Observed signals    RICH1/2 : " << sigRich << endreq;
  }

  // Obtain background term PD by PD
  for ( int iRich = Rich::Rich1; iRich <= Rich::Rich2; ++iRich ) 
  {
  
    int iter = 1;
    bool cont = true;
    double rnorm = 0.0;
    while ( cont ) 
    {

      int nBelow = 0;
      int nAbove = 0;
      double tBelow = 0.0;
      for ( PDsignals::iterator iPD = obsPDsignals[iRich].begin();
            iPD !=  obsPDsignals[iRich].end();
            ++iPD ) 
      {
        const RichSmartID::KeyType pd = iPD->first;
        double & obs = iPD->second;
        double & exp = (expPDsignals[iRich])[pd];
        double & bkg = (expPDbkg[iRich])[pd];

        if ( 1 == iter ) 
        {
          bkg = obs - exp;
        } 
        else 
        {
          if ( bkg > 0.0 ) 
          {
            bkg -= rnorm;
          } else {
            bkg = 0.0;
          }
        }

        if ( bkg < 0.0 ) 
        {
          ++nBelow;
          tBelow += fabs( bkg );
        } 
        else if ( bkg > 0.0 ) 
        {
          ++nAbove;
        }

      } // end loop over signal PDs

      if ( nBelow > 0 && nAbove > 0 ) 
      {
        rnorm = tBelow/( static_cast<double>(nAbove) );
      } 
      else 
      {
        cont = false;
      }
      if ( iter > m_maxBkgIterations ) cont = false;

      ++iter;
    } // end while loop

      // Finally, fill background estimates
    for ( PDsignals::iterator iPD = expPDbkg[iRich].begin();
          iPD !=  expPDbkg[iRich].end(); ++iPD ) 
    {
      if ( iPD->second < 0 ) iPD->second = 0;
      bckEstimate[ static_cast<Rich::DetectorType>(iRich) ] += iPD->second;
    }

  } // end rich loop

    // Loop over pixels again to set background term
  {for ( RichRecPixels::iterator pixel = richPixels()->begin();
         pixel != richPixels()->end();
         ++pixel ) 
  {

    const RichSmartID::KeyType pd = (*pixel)->smartID().pdID();
    const Rich::DetectorType det = (Rich::DetectorType)(*pixel)->smartID().rich();

    double rbckexp = (obsPDsignals[det])[pd] - (expPDsignals[det])[pd];
    rbckexp = ( rbckexp>0 ? rbckexp/m_nPixelsPerPD : 0 );
    (*pixel)->setCurrentBackground( rbckexp );

    // Debug printout
    if ( msgLevel(MSG::VERBOSE) ) 
    {
      verbose() << "Pixel " << static_cast<unsigned long>((*pixel)->key())
                << " Obs " << (obsPDsignals[det])[pd]
                << " Exp " << (expPDsignals[det])[pd]
                << " bkg " << (*pixel)->currentBackground()
                << endreq;
    }

  }}

  if ( msgLevel(MSG::DEBUG) )
    debug() << "Overall backgrounds RICH1/2 : " << bckEstimate << endreq;

  return StatusCode::SUCCESS;
}

StatusCode RichRecBackgroundEsti::finalize()
{
  // Execute base class method
  return RichRecAlgBase::finalize();
}
