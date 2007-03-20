
//--------------------------------------------------------------------------
/** @file RichGlobalPIDLikelihood.cpp
 *
 *  Implementation file for RICH Global PID algorithm class : Rich::Rec::GlobalPID::Likelihood
 *
 *  CVS Log :-
 *  $Id: RichGlobalPIDLikelihood.cpp,v 1.3 2007-03-20 11:06:25 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   17/04/2002
 */
//--------------------------------------------------------------------------

// local
#include "RichGlobalPIDLikelihood.h"

// namespaces
using namespace LHCb;
using namespace Rich::Rec::GlobalPID;

//--------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( Likelihood );

// Standard constructor, initializes variables
Likelihood::Likelihood( const std::string& name,
                        ISvcLocator* pSvcLocator )
  : AlgBase     ( name, pSvcLocator ),
    m_tkSignal  ( NULL ),
    m_photonSig ( NULL ),
    m_inR1      ( true ),
    m_inR2      ( true )
{

  // Threshold for likelihood maximisation
  declareProperty( "LikelihoodThreshold", m_epsilon = -1e-5 );

  // Maximum number of track iterations
  declareProperty( "MaxEventIterations", m_maxEventIterations = 500 );

  // Minimum signal value for full calculation of log(1-exp(-signal))
  declareProperty( "MinSignalForNoLLCalc", m_minSig = 1e-25 );

  // Signal value below which to approximate log(1-exp(-signal)) with log(signal)
  // set to 1e-99 for max precision
  declareProperty( "MinSignalForAproxLLCalc", m_apxSig = 1e-5 );

  // Track freeze out value ( the point at which it is no longer considered for change)
  declareProperty( "TrackFreezeOutDLL", m_freezeOutDll = 999999 );

  // Track DLL Thresdhold for forced change
  declareProperty( "TrackForceChangeDLL", m_forceChangeDll = -999999 );

  // Flag to turn on final DLL and hypothesis check
  declareProperty( "FinalDLLCheck", m_doFinalDllCheck = false );

  // Flag to turn on RICH check in LL minimisation (turn off for max precision)
  declareProperty( "RichDetCheck", m_richCheck = true );

  // Maximum number of tracks to change in a single event iteration
  declareProperty( "MaxTrackChangesPerIt", m_maxTkChanges = 5 );

}

// Destructor
Likelihood::~Likelihood() {}

//  Initialize
StatusCode Likelihood::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = AlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire tools
  acquireTool( "RichPhotonSignal",        m_photonSig );
  acquireTool( "RichExpectedTrackSignal", m_tkSignal  );

  // trick to force pre-loading of various tools. Avoids loading
  // during first processed event and thus biasing any timing numbers
  photonCreator();

  // Initialise parameters
  m_logMinSig = log(m_minSig);

  // Printout some initialisation info
  info() << "Maximum event iterations                  = " << m_maxEventIterations << endreq;
  info() << "Minimum signal for LL calculation         = " << m_minSig << endreq;
  info() << "Minimum signal for approx LL              = " << m_apxSig << endreq;
  info() << "Track freeze-out DLL value                = " << freezeOutDll() << endreq;
  info() << "Track forced change DLL value             = " << forceChangeDll() << endreq;
  info() << "Maximum track changes per event iteration = " << m_maxTkChanges << endreq;

  return sc;
}

// Main execution
StatusCode Likelihood::execute()
{

  // ------------------------- General  RICH reco stuff --------------------------------

  // Update RichRecEvent pointers
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // update pointers to global PID working objects
  if ( !gpidSummary() || !gpidTracks() ) return StatusCode::FAILURE;

  // how many tracks ?
  if ( m_GPIDtracks->empty() ) return StatusCode::SUCCESS;

  // Reconstruct all photons
  if ( !richPhotons() ) return StatusCode::FAILURE;
  if ( richPhotons()->empty() )
  {
    if ( !photonCreator()->reconstructPhotons() ) return StatusCode::FAILURE;
    if ( msgLevel(MSG::DEBUG) )
      debug() << "Reconstructed " << richPhotons()->size() << " photon candidates" << endreq;
    if ( richPhotons()->empty() )
      return Warning("No reconstructed photons -> Abort",StatusCode::SUCCESS);
  }

  // ------------------------- General  RICH reco stuff --------------------------------

  // Now start Global PID

  // Compute complete likelihood for event with starting hypotheses
  m_currentBestLL = logLikelihood();
  // (CRJ : minus sign as we work with -loglikelihood values)
  m_GPIDSummary->addToEventLL( -m_currentBestLL );

  // Initial calculation of likelihood
  const unsigned int nChangeFirstIt = initBestLogLikelihood();
  const double finalLL              = logLikelihood();
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "LogLL before/after initial minimisation = "
            << m_currentBestLL << "/" << finalLL
            << " : " << nChangeFirstIt << " tracks changed hypothesis" << endreq;
  }
  m_currentBestLL = finalLL;

  // did any tracks change in the first initialisation
  unsigned int eventIteration = 0;
  if ( nChangeFirstIt>0 )
  {

    // Do the event iterations
    eventIteration = doIterations();

    // Final check, to see if we are at the global best set of hypos
    if ( m_doFinalDllCheck )
    {
      const unsigned int nChange = initBestLogLikelihood();
      if ( nChange != 0 )
      {
        // Not quite at best hypos yet ...
        std::ostringstream mess;
        mess << nChange << " track(s) changed hypothesis after final iteration -> Re-running";
        Warning( mess.str(), StatusCode::SUCCESS, 3 );
        // Rerun iterations
        if ( msgLevel(MSG::DEBUG) )
        { debug() << "Re-running event iteratons" << endreq; }
        eventIteration += doIterations();
      }
      m_currentBestLL = logLikelihood();
    }

  }

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Performed " << eventIteration
            << " event minimisation iteration(s). Final LogL = "
            << m_currentBestLL << endreq;
  }

  // update event LL vector after processing
  // (CRJ : minus sign as we work with -loglikelihood values)
  m_GPIDSummary->addToEventLL( -m_currentBestLL );

  return StatusCode::SUCCESS;
}

unsigned int Likelihood::doIterations()
{
  // iterate to minimum solution
  unsigned int eventIteration = 0;
  MinTrList minTracks;
  bool tryAgain = true;
  while ( tryAgain || !minTracks.empty() )
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << "Event Iteration " << eventIteration << endreq;

    // Iterate finding the best likelihood
    findBestLogLikelihood( minTracks );

    // set track hypotheses to the current best
    if ( !minTracks.empty() )
    {
      MinTrList::iterator iTrack;
      for ( iTrack = minTracks.begin(); iTrack != minTracks.end(); ++iTrack )
      {
        if ( Rich::Unknown == iTrack->second )
        {
          err() << " Track " << (iTrack->first)->key()
                << " has been Id'ed as Unknown !!" << endreq;
        }
        else
        {
          if ( msgLevel(MSG::DEBUG) )
          {
            debug() << " -> Changing Track " << (iTrack->first)->key()
                    << " hypothesis to from "
                    << (iTrack->first)->richRecTrack()->currentHypothesis()
                    << " to " << iTrack->second << ". LogL = " << m_currentBestLL
                    << endreq;
          }
          // Update hypothesis to best
          (iTrack->first)->richRecTrack()->setCurrentHypothesis( iTrack->second );
          // set deltaLL to zero
          (iTrack->first)->globalPID()->setParticleDeltaLL( iTrack->second, 0 );
        }
      }
    }
    // only quit if last iteration was with both riches
    else if ( m_inR1 && m_inR2 )
    {
      if ( msgLevel(MSG::DEBUG) )
      { debug() << " -> ALL DONE. Quitting event iterations" << endreq; }
      tryAgain = false;
    }
    else // have one last try in both riches
    {
      if ( msgLevel(MSG::DEBUG) )
      { debug() << " -> Last try in both RICH detectors" << endreq; }
      m_inR1 = true;
      m_inR2 = true;
    }

    ++eventIteration;
    if ( eventIteration > m_maxEventIterations )
    {
      Warning( "Taken more than max number of iterations -> quitting",
               StatusCode::SUCCESS );
      break;
    }

  } // end iteration while loop

  return eventIteration;
}

unsigned int Likelihood::initBestLogLikelihood()
{
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Running initial log likelihood maximisation" << endreq;

  InitTrackInfo::Vector minTrackData;
  minTrackData.reserve(30); // average number of tracks that change DLL
  m_trackList.clear();

  // Loop over all tracks and find best hypothesis for each track
  {for ( RichGlobalPIDTracks::iterator track = m_GPIDtracks->begin();
         track != m_GPIDtracks->end();
         ++track )
  {
    RichRecTrack * rRTrack = (*track)->richRecTrack();

    // Initialise starting values
    double mindeltaLL = 99999.;
    Rich::ParticleIDType minHypo = rRTrack->currentHypothesis();

    // Loop over all particle codes
    for ( int iHypo = 0; iHypo < Rich::NParticleTypes; ++iHypo )
    {
      Rich::ParticleIDType hypo = static_cast<Rich::ParticleIDType>(iHypo);

      // Skip analysing starting hypothesis
      if ( hypo == rRTrack->currentHypothesis() ) continue;

      // calculate delta logLikelihood for event with new track hypothesis
      const double deltaLogL = deltaLogLikelihood( rRTrack, hypo );

      // Set the value for deltaLL for this hypothesis
      (*track)->globalPID()->setParticleDeltaLL( hypo, deltaLogL );

      // Set new minimum if lower logLikelihood is achieved
      if ( deltaLogL < mindeltaLL )
      {
        if ( 0 != deltaLogL ) mindeltaLL = deltaLogL;
        if ( deltaLogL < m_epsilon ) minHypo = hypo;
      }

      // In case the threshold is reached, skip other hypotheses
      bool threshold = true;
      for ( int ihypo = iHypo; ihypo < Rich::NParticleTypes; ++ihypo )
      {
        if ( m_tkSignal->nDetectablePhotons( rRTrack,
                                             static_cast<Rich::ParticleIDType>(ihypo) ) > 0 )
        {
          threshold = false; break;
        }
      }
      if ( threshold )
      {
        for ( int ihypo = iHypo; ihypo < Rich::NParticleTypes; ++ihypo )
        {
          (*track)->globalPID()->setParticleDeltaLL( static_cast<Rich::ParticleIDType>(ihypo),
                                                     deltaLogL );
        }
        break;
      }

    } // end particle for loop

      // Save info on tracks that have a better minimum hypothesis
    if ( minHypo != rRTrack->currentHypothesis() )
    {
      minTrackData.push_back( InitTrackInfo(*track,minHypo,mindeltaLL) );
    }

    // Add this track / deltaLL to the track list
    m_trackList.push_back( TrackPair(mindeltaLL, *track) );

  }} // end track for loop

  // Finally, set all track hypotheses to their minimum
  for ( InitTrackInfo::Vector::const_iterator iT = minTrackData.begin();
        iT != minTrackData.end(); ++iT )
  {
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << " -> Track "
              << boost::format("%4i") % (*iT).pidTrack->key()
              << " prefers hypothesis " << (*iT).hypo << " to "
              << (*iT).pidTrack->richRecTrack()->currentHypothesis()
              << ". DLL = " << (*iT).minDLL << endreq ;
    }
    // set best hypothesis
    (*iT).pidTrack->richRecTrack()->setCurrentHypothesis( (*iT).hypo );
    // set delta LL to zero for best hypothesis
    (*iT).pidTrack->globalPID()->setParticleDeltaLL( (*iT).hypo, 0 );
  }

  return minTrackData.size();
}

void Likelihood::findBestLogLikelihood( MinTrList & minTracks )
{
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Finding best overall log likelihood" << endreq;

  // update RICH flags
  if (m_richCheck) updateRichFlags(minTracks);

  // Initialise
  minTracks.clear();
  RichGlobalPIDTrack * overallMinTrack = NULL;
  Rich::ParticleIDType overallMinHypo  = Rich::Unknown;
  double minDLLs                       = 0;
  double minEventDll                   = 999999;

  // sort Track list according to delta LL
  std::sort( m_trackList.begin(), m_trackList.end() );

  // Loop on all tracks
  for ( TrackList::iterator iP = m_trackList.begin();
        iP != m_trackList.end(); ++iP )
  {
    RichGlobalPIDTrack * gTrack = (*iP).second;
    RichRecTrack * rRTrack = gTrack->richRecTrack();

    if ( msgLevel(MSG::DEBUG) )
      debug() << " -> Track " << gTrack->key() << " DLL = " << (*iP).first << endreq;

    // skip frozen tracks
    if ( (*iP).first > freezeOutDll() )
    {
      if ( msgLevel(MSG::DEBUG) )
      { debug() << "  -> Freeze-out value = " << freezeOutDll()
                << " -> Aborting remaining tracks" << endreq; }
      break;
    }

    // skip tracks that do not share a RICH with the last changed tracks
    // for these tracks it is not possible that the hypos could change
    if ( m_richCheck && !( (m_inR1 && rRTrack->inRICH1()) ||
                           (m_inR2 && rRTrack->inRICH2()) ) )
    {
      if ( msgLevel(MSG::DEBUG) )
      { debug() << "  -> Skipping track in unaltered RICH" << endreq; }
      continue;
    }

    bool addto(false), minFound(false);
    double minTrackDll = 99999999;
    Rich::ParticleIDType minHypo = Rich::Unknown;
    for ( int iHypo = 0; iHypo < Rich::NParticleTypes; ++iHypo )
    {
      const Rich::ParticleIDType hypothesis = static_cast<Rich::ParticleIDType>(iHypo);

      // Skip analysing starting hpyothesis
      if ( hypothesis == rRTrack->currentHypothesis() ) continue;

      // calculate delta logLikelihood for event with new track hypothesis
      const double deltaLogL = deltaLogLikelihood( rRTrack, hypothesis );
      if ( msgLevel(MSG::VERBOSE) )
      {
        verbose() << "  -> Trying " << hypothesis << " : DLL = " << deltaLogL << endreq;
      }

      // Set the value for deltaLL for this hypothesis
      gTrack->globalPID()->setParticleDeltaLL( hypothesis, deltaLogL );

      // is DLL change significant ?
      if ( deltaLogL < m_epsilon )
      {

        // Is this the best hypo for this track
        if ( deltaLogL < minTrackDll )
        {
          if ( msgLevel(MSG::DEBUG) )
          {
            debug() << "    -> Track "
                    << boost::format("%4i") % gTrack->key() << " prefers hypothesis "
                    << hypothesis << " to " << rRTrack->currentHypothesis()
                    << ". DLL = " << deltaLogL << endreq ;
          }

          // set that a new best is found and update best dll and type
          minFound    = true;
          (*iP).first = deltaLogL;
          minHypo     = hypothesis;
          minTrackDll = deltaLogL;

          // Is dll change enough to add to force change list
          if ( !addto && (deltaLogL < forceChangeDll()) )
          {
            if ( msgLevel(MSG::DEBUG) )
            {
              debug() << "     -> Adding to force change list" << endreq;
            }
            addto = true;
          }

          // Overall best track change for event ?
          if ( deltaLogL < minEventDll )
          {
            overallMinTrack = gTrack;
            overallMinHypo  = hypothesis;
            minEventDll     = deltaLogL;
          }

        } // best hypo for this track

      }
      else
      {
        // store best DLL for this track to enable it to be set at end of hypo loop
        // do here so set even if deltaLogL > m_epsilon
        if ( deltaLogL != 0 && deltaLogL<minTrackDll ) { minTrackDll = deltaLogL; }
      }

      // If threshold is reached, set the deltaLL for all other hypotheses
      bool threshold = true;
      for ( int hypo = iHypo; hypo < Rich::NParticleTypes; ++hypo )
      {
        if ( m_tkSignal->nDetectablePhotons( gTrack->richRecTrack(),
                                             static_cast<Rich::ParticleIDType>(hypo) ) > 0 )
        {
          threshold = false; break;
        }
      }
      if ( threshold )
      {
        for ( int hypo = iHypo; hypo < Rich::NParticleTypes; ++hypo )
        {
          gTrack->globalPID()->setParticleDeltaLL( static_cast<Rich::ParticleIDType>(hypo),
                                                   deltaLogL );
        }
        break;
      }

    } // end hypothesis loop

    // if a good new hypo is not found, just update track dll in container to best
    if ( !minFound ) (*iP).first = minTrackDll;

    // if found tracks with good enough deltaLL, add to force-change map
    if ( addto )
    {
      minTracks[gTrack] = minHypo;
      minDLLs          += minTrackDll;
      (*iP).first       = 0;
      break;
    }

    // do we have enough tracks to change to break out the loop early ?
    if ( minTracks.size() >= m_maxTkChanges )
    {
      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << "    -> Found " << minTracks.size()
                << " tracks to change. Aborting track loop" << endreq;
      }
      break;
    }

  } // end track loop

  // update final information
  if ( overallMinTrack )
  {
    if ( minTracks.empty() )
    {
      minTracks[overallMinTrack] = overallMinHypo;
      minDLLs = minEventDll;
    }
    m_currentBestLL += minDLLs;
  }

  if ( msgLevel(MSG::DEBUG) )
  { debug() << "Found " << minTracks.size() << " track(s) to change" << endreq; }

}

void Likelihood::updateRichFlags( const MinTrList & minTracks )
{
  if ( !minTracks.empty() )
  {
    m_inR1 = false; m_inR2 = false;
    for ( MinTrList::const_iterator iT = minTracks.begin();
          iT != minTracks.end(); ++iT )
    {
      RichRecTrack * rTk = ((*iT).first)->richRecTrack();
      if ( rTk->inRICH1() ) m_inR1 = true;
      if ( rTk->inRICH2() ) m_inR2 = true;
    }
  }
  else
  {
    // default to both being true
    m_inR1 = true;
    m_inR2 = true;
  }
}

double
Likelihood::deltaLogLikelihood( RichRecTrack * track,
                                const Rich::ParticleIDType newHypo )
{

  // Change due to track expectation
  double deltaLL = ( m_tkSignal->nTotalObservablePhotons( track, newHypo ) -
                     m_tkSignal->nTotalObservablePhotons( track, track->currentHypothesis() ) );
  //  ( track->nObservableSignalPhotons(track->currentHypothesis()) +
  //    track->nObservableScatteredPhotons(track->currentHypothesis()) );

  // Photon part
  RichRecTrack::Pixels & pixels = track->richRecPixels();
  const RichRecTrack::Pixels::iterator ePixel = pixels.end();
  for ( RichRecTrack::Pixels::iterator iPixel = pixels.begin();
        iPixel != ePixel; ++iPixel )
  {

    // photons for this pixel
    RichRecPixel::Photons & photons = (*iPixel)->richRecPhotons();
    if ( !photons.empty() )
    {

      double oldSig = (*iPixel)->currentBackground();
      double newSig = oldSig;

      // Loop over photons for this pixel
      const RichRecPixel::Photons::iterator iPhotonEnd    = photons.end();
      for ( RichRecPixel::Photons::const_iterator iPhoton = photons.begin();
            iPhoton != iPhotonEnd; ++iPhoton )
      {
        // Skip tracks not in use
        if ( !(*iPhoton)->richRecTrack()->inUse() ) continue;

        // update signal numbers
        const double tmpOldSig =
          (*iPhoton)->expPixelSignalPhots( (*iPhoton)->richRecTrack()->currentHypothesis() );
        oldSig += tmpOldSig;
        newSig += ( (*iPhoton)->richRecTrack() != track ? tmpOldSig :
                    m_photonSig->predictedPixelSignal( *iPhoton, newHypo ) );
      } // end photon loop

      // increment change to likelihood for this photon
      deltaLL -= sigFunc(newSig) - sigFunc(oldSig);

    } // end photons not empty

  } // end pixel loop

  return deltaLL;
}

double Likelihood::logLikelihood()
{
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Computing event log likelihood" << endreq;

  // Loop over tracks to form total expected hits part of LL
  double trackLL = 0.0;
  for ( RichGlobalPIDTracks::iterator track = m_GPIDtracks->begin();
        track != m_GPIDtracks->end();
        ++track )
  {
    // Sum expected photons from each track with current assumed hypotheses
    RichRecTrack * rRTrack = (*track)->richRecTrack();
    trackLL += m_tkSignal->nTotalObservablePhotons( rRTrack,
                                                    rRTrack->currentHypothesis() );
  } // end track loop

  // Add background terms for expectation
  trackLL += ( richStatus()->detOverallBkg( Rich::Rich1 ) +
               richStatus()->detOverallBkg( Rich::Rich2 ) );

  // Pixel loop
  double pixelLL = 0.0;
  for ( RichRecPixels::iterator iPixel = richPixels()->begin();
        iPixel != richPixels()->end();
        ++iPixel )
  {
    RichRecPixel * pixel = *iPixel;

    // loop over all the photons for this pixel
    double photonSig = 0.0;
    RichRecPixel::Photons & photons = pixel->richRecPhotons();
    const RichRecPixel::Photons::iterator iPhotonEnd = photons.end();
    for ( RichRecPixel::Photons::iterator iPhoton = photons.begin();
          iPhoton != iPhotonEnd; ++iPhoton )
    {
      RichRecTrack * rRTrack = (*iPhoton)->richRecTrack();
      if ( !rRTrack->inUse() ) continue;
      photonSig += m_photonSig->predictedPixelSignal( *iPhoton,
                                                      rRTrack->currentHypothesis() );
    }

    pixelLL -= sigFunc( photonSig + pixel->currentBackground() );

  }

  return (trackLL+pixelLL);
}
