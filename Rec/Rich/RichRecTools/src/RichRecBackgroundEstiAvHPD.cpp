//--------------------------------------------------------------------------
/** @file RichRecBackgroundEstiAvHPD.cpp
 *
 *  Implementation file for algorithm class : Rich::Rec::BackgroundEstiAvHPD
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   17/04/2002
 */
//--------------------------------------------------------------------------

// local
#include "RichRecBackgroundEstiAvHPD.h"
#include "RichDet/DeRich1.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//--------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( BackgroundEstiAvHPD )

// Standard constructor, initializes variables
BackgroundEstiAvHPD::BackgroundEstiAvHPD( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
    : Rich::Rec::ToolBase ( type, name, parent ),
      m_tkSignal       ( NULL          ),
      m_geomEff        ( NULL          ),
      m_obsPDsignals   ( Rich::NRiches ),
      m_expPDsignals   ( Rich::NRiches ),
      m_expPDbkg       ( Rich::NRiches )
{
  // Define interface for this tool
  declareInterface<IPixelBackgroundEsti>(this);

  declareProperty( "MaxBackgroundNormIterations", m_maxBkgIterations = 10,
                   "Maximum number of iterations in background normalisation" );

    declareProperty( "PixelsPerPD", m_nPixelsPerPD = 784.763611, "Number of active pixels per PD" );

  //declareProperty( "PixelsPerPD", m_nPixelsPerPD = 64,
  //                 "Number of active pixels per PD - To be got from XML eventually" );

  declareProperty( "MinPixelBackground", m_minPixBkg = 0.0,
                   "Minimum pixel background" );

  declareProperty( "IgnoreExpectedSignals", m_ignoreExpSignal = false,
                   "Ignore expectations when calculating backgrounds" );  

  declareProperty( "MinHPDBckForInclusion", m_minHPDbckForInc = 0.0,
                   "Min HPD background level for setting background levels" );
}

// Destructor
BackgroundEstiAvHPD::~BackgroundEstiAvHPD() { }

// Initialize
StatusCode BackgroundEstiAvHPD::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = Rich::Rec::ToolBase::initialize();
  if ( sc.isFailure() ) { return Error( "Failed to initialize base class", sc ); }

  DeRich1* aRich1= getDet<DeRich1> ( DeRichLocations::Rich1 );

  if(aRich1 ->RichPhotoDetConfig() == Rich::HPDConfig ) {

    m_nPixelsPerPD = (aRich1->exists("RichEffectiveActiveNumPixelPerHpd"))? ((aRich1 ->param<int>("RichEffectiveActiveNumPixelPerHpd"))*1.0) :  784.763611;

  }else if (aRich1 ->RichPhotoDetConfig() == Rich::PMTConfig ) {

    m_nPixelsPerPD = (aRich1->exists("RichPmtTotNumPixel"))? ((aRich1 ->param<int>("RichPmtTotNumPixel"))*1.0) : 64.0;
    
  }  
  if ( msgLevel(MSG::INFO) ) info()<<" RichRecbackgrEst :  NumPixel per PD "<<m_nPixelsPerPD<<endmsg;
  

  // Acquire instances of tools
  acquireTool( "RichExpectedTrackSignal", m_tkSignal );
  acquireTool( "RichGeomEff",             m_geomEff  );

  if ( m_ignoreExpSignal )
    info() << "Will ignore expected signals when computing backgrounds" << endmsg;

  info() << "Minimum pixel background = " << m_minPixBkg << endmsg;
  info() << "Min HPD background level for setting background levels = " 
         << m_minHPDbckForInc << endmsg;

  // pre-cache creator tools
  pixelCreator();
  trackCreator();
  statusCreator();

  return sc;
}

void BackgroundEstiAvHPD::computeBackgrounds() const
{
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Computing backgrounds using ALL "
            << richTracks()->size() << " tracks" << endmsg;

  // General init
  richInit();

  // get expected signals
  fillExpectedSignalMap();

  // get observed signals
  fillObservedSignalMap();

  // get the overal RICH backgrounds
  overallRICHBackgrounds();

  // compute the pixel backgrounds
  pixelBackgrounds();
}

void BackgroundEstiAvHPD::computeBackgrounds( const LHCb::RichRecTrack::Vector & tracks ) const
{
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Computing backgrounds using "
            << tracks.size() << " SELECTED tracks" << endmsg;

  // General init
  richInit();

  // get expected signals
  fillExpectedSignalMap(tracks);

  // get observed signals
  fillObservedSignalMap();

  // get the overal RICH backgrounds
  overallRICHBackgrounds();

  // compute the pixel backgrounds
  pixelBackgrounds();
}

void BackgroundEstiAvHPD::computeBackgrounds( const LHCb::RichRecTrack * track ) const
{
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Computing backgrounds using SINGLE track : key=" << track->key()
            << endmsg;

  // General init
  richInit();

  // get expected signals
  fillExpectedSignalMap(track);

  // get observed signals
  fillObservedSignalMap();

  // get the overal RICH backgrounds
  overallRICHBackgrounds();

  // compute the pixel backgrounds
  pixelBackgrounds();
}

void BackgroundEstiAvHPD::richInit() const
{
  // reset the working maps
  m_obsPDsignals[Rich::Rich1].clear();
  m_obsPDsignals[Rich::Rich2].clear();
  m_expPDsignals[Rich::Rich1].clear();
  m_expPDsignals[Rich::Rich2].clear();
  m_expPDbkg[Rich::Rich1].clear();
  m_expPDbkg[Rich::Rich2].clear();
}

void BackgroundEstiAvHPD::fillObservedSignalMap() const
{
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Filling observed signals maps" << endmsg;
  // Loop over pixels
  for ( LHCb::RichRecPixels::const_iterator pixel = richPixels()->begin();
        pixel != richPixels()->end(); ++pixel )
  {
    // count the number of hits in each HPD, in each RICH
    ++(m_obsPDsignals[ (*pixel)->hpdPixelCluster().rich() ])[ (*pixel)->hpdPixelCluster().hpd() ];
  }
}

void BackgroundEstiAvHPD::fillExpectedSignalMap() const
{
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Filling expected signals map for ALL tracks" << endmsg;
  // loop over segments
  for ( LHCb::RichRecTracks::const_iterator track = richTracks()->begin();
        track != richTracks()->end(); ++track )
  {
    fillExpectedSignalMap(*track);
  }
}

void BackgroundEstiAvHPD::fillExpectedSignalMap( const LHCb::RichRecTrack::Vector & tracks ) const
{
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Filling expected signals map for selected tracks" << endmsg;
  // loop over segments
  for ( LHCb::RichRecTrack::Vector::const_iterator track = tracks.begin();
        track != tracks.end(); ++track )
  {
    fillExpectedSignalMap(*track);
  }
}

void BackgroundEstiAvHPD::fillExpectedSignalMap( const LHCb::RichRecTrack * track ) const
{
  if ( track && !m_ignoreExpSignal )
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << " -> Track " << track->key() << " " << track->inUse()
              << " " << track->currentHypothesis()
              << endmsg;

    // skip tracks not in use
    if ( track->inUse() )
    {

      // Current best hypothesis for this track
      const Rich::ParticleIDType id = track->currentHypothesis();

      // skip tracks below threshold, as they for sure contribute nothing to the signal
      if ( Rich::BelowThreshold != id )
      {

        for ( LHCb::RichRecTrack::Segments::const_iterator segment = track->richRecSegments().begin();
              segment != track->richRecSegments().end(); ++segment )
        {
          // Expected detectable emitted photons for this segment
          const double detPhots = m_tkSignal->nDetectablePhotons(*segment,id);

          // which RICH
          const Rich::DetectorType rich = (*segment)->trackSegment().rich();

          if ( msgLevel(MSG::DEBUG) )
            debug() << "  -> Segment " << (*segment)->key() << " " << rich
                    << " " << (*segment)->trackSegment().radiator()
                    << " DetPhots=" << detPhots << endmsg;

          // Tally total expected hits for each PD
          m_geomEff->geomEfficiency(*segment,id); // needed to ensure map below is filled
          const LHCb::RichRecSegment::PDGeomEffs & hypoMap = (*segment)->geomEfficiencyPerPD( id );
          for ( LHCb::RichRecSegment::PDGeomEffs::const_iterator iPD = hypoMap.begin();
                iPD != hypoMap.end(); ++iPD )
          {
            const double sig = detPhots * iPD->second; // expected signal for this PD
            (m_expPDsignals[rich])[ LHCb::RichSmartID(iPD->first) ] += sig;
            if ( msgLevel(MSG::DEBUG) )
              debug() << "   -> " << LHCb::RichSmartID(iPD->first) << " DetPhots=" << sig << endmsg;
          }

        } // loop over segments

      } // Hypo OK

    } // track in use

  } // track OK

}

void BackgroundEstiAvHPD::overallRICHBackgrounds() const
{
  // Initialise detector backgrounds
  std::vector<double> bckEstimate(Rich::NRiches,0);

  // Obtain background term PD by PD
  for ( Rich::Detectors::const_iterator iRich = detectors().begin();
        iRich != detectors().end(); ++iRich )
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << "Computing HPD backgrounds in " << *iRich << endmsg;

    int iter = 1;
    bool cont = true;
    double rnorm = 0.0;
    while ( cont )
    {
      if ( msgLevel(MSG::DEBUG) ) debug() << " -> Iteration " << iter << endmsg;

      int nBelow(0), nAbove(0);
      double tBelow = 0.0;
      for ( PDsignals::const_iterator iPD = m_obsPDsignals[*iRich].begin();
            iPD != m_obsPDsignals[*iRich].end(); ++iPD )
      {
        const LHCb::RichSmartID& pd = iPD->first;
        double & bkg                = (m_expPDbkg[*iRich])[pd];

        if ( 1 == iter )
        {
          const double obs = iPD->second;
          const double exp = (m_expPDsignals[*iRich])[pd];
          // First iteration, just set background for this HPD to the difference
          // between the observed and and expected number of hits in the HPD
          if ( msgLevel(MSG::DEBUG) )
            debug() << "  -> HPD " << pd << " obs. = " << obs << " exp. = " << exp << endmsg;
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

      if ( msgLevel(MSG::DEBUG) )
        debug() << "  -> Above = " << nAbove << " Below = " << nBelow << endmsg;

      if ( nBelow > 0 && nAbove > 0 )
      {
        // we have some HPDs above and below expectation
        // calculate the amount of signal below per above HPD
        rnorm = tBelow / ( static_cast<double>(nAbove) );
        if ( msgLevel(MSG::DEBUG) )
          debug() << "   -> Correction factor per HPD above = " << rnorm << endmsg;
      }
      else
      {
        if ( msgLevel(MSG::DEBUG) ) debug() << "  -> Aborting iterations" << endmsg;
        cont = false;
      }

      // Final protection against infinite loops
      if ( ++iter > m_maxBkgIterations ) cont = false;

    } // end while loop

      // Finally, fill background estimates
    for ( PDsignals::iterator iPD = m_expPDbkg[*iRich].begin();
          iPD != m_expPDbkg[*iRich].end(); ++iPD )
    {
      if ( iPD->second < m_minHPDbckForInc ) iPD->second = 0;
      bckEstimate[*iRich] += iPD->second;
    }

  } // end rich loop

  // Update detector backgrounds
  // CRJ : Ugly conversion from double to float .... To be improved
  std::vector<LHCb::RichRecStatus::FloatType> bckEstiUpdate(Rich::NRiches,0);
  bckEstiUpdate[Rich::Rich1] = (LHCb::RichRecStatus::FloatType)(bckEstimate[Rich::Rich1]);
  bckEstiUpdate[Rich::Rich2] = (LHCb::RichRecStatus::FloatType)(bckEstimate[Rich::Rich2]);
  LHCb::RichRecStatus * status = const_cast<LHCb::RichRecStatus*>(richStatus());
  status->setDetOverallBkg(bckEstiUpdate);

  if ( msgLevel(MSG::DEBUG) )
  {
    for ( Rich::Detectors::const_iterator iRich = detectors().begin();
          iRich != detectors().end(); ++iRich )
    {
      debug() << "Overall backgrounds " << *iRich << " " << bckEstimate[*iRich] << endmsg;
      // loop over HPDs with signal and print the backgrounds
      for ( PDsignals::iterator iPD = m_expPDbkg[*iRich].begin();
            iPD != m_expPDbkg[*iRich].end(); ++iPD )
      {
        debug() << "  " << iPD->first << " bkg = " << iPD->second << endmsg;
      }
    }
  }

}

void BackgroundEstiAvHPD::pixelBackgrounds() const
{
  // Loop over pixels again to set background term
  for ( LHCb::RichRecPixels::const_iterator pixel = richPixels()->begin();
        pixel != richPixels()->end(); ++pixel )
  {
    const LHCb::RichSmartID&   pd = (*pixel)->hpdPixelCluster().hpd();
    const Rich::DetectorType& det = (*pixel)->hpdPixelCluster().rich();

    // background for this HPD
    const double rbckexp = (m_expPDbkg[det])[pd];

    // Save this value in the pixel
    double bkg = ( rbckexp>0 ? rbckexp/m_nPixelsPerPD : 0 );
    if ( bkg < m_minPixBkg ) bkg = m_minPixBkg;
    (*pixel)->setCurrentBackground( (LHCb::RichRecRing::FloatType)(bkg) );

    // printout
    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << "Pixel " << (*pixel)->hpdPixelCluster()
                << " Obs "  << (m_obsPDsignals[det])[pd]
                << " Exp "  << (m_expPDsignals[det])[pd]
                << " bkg "  << (*pixel)->currentBackground()
                << endmsg;
    }

  } // loop over pixels

}
