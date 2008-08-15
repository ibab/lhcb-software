
//--------------------------------------------------------------------------
/** @file RichGlobalPIDLikelihoodTool.cpp
 *
 *  Implementation file for RICH Global PID tool : Rich::Rec::GlobalPID::LikelihoodTool
 *
 *  CVS Log :-
 *  $Id: RichGlobalPIDLikelihoodTool.cpp,v 1.5 2008-08-15 14:30:22 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2008-03-01
 */
//--------------------------------------------------------------------------

// local
#include "RichGlobalPIDLikelihoodTool.h"

// namespaces
using namespace Rich::Rec::GlobalPID;

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( LikelihoodTool );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
LikelihoodTool::LikelihoodTool( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : Rich::Rec::GlobalPID::ToolBase ( type, name, parent ),
    m_gtkCreator      ( NULL ),
    m_tkSignal        ( NULL ),
    m_photonSig       ( NULL ),
    m_gpidTracksToPID ( NULL ),
    m_inR1            ( true ),
    m_inR2            ( true )
{
  // interfaces
  declareInterface<IRichGlobalPID> ( this );
  declareInterface<IRichPID>       ( this );

  declareProperty( "LikelihoodThreshold", m_epsilon = -1e-3,
                   "Threshold for likelihood maximisation" );

  declareProperty( "MaxEventIterations", m_maxEventIterations = 500,
                   "Maximum number of track iterations" );

  declareProperty( "MinSignalForNoLLCalc", m_minSig = 1e-3,
                   "Minimum signal value for full calculation of log(1-exp(-signal))" );

  declareProperty( "TrackFreezeOutDLL", m_freezeOutDll = 4,
                   "Track freeze out value ( the point at which it is no longer considered for change)" );

  declareProperty( "TrackForceChangeDLL", m_forceChangeDll = -2,
                   "Track DLL Thresdhold for forced change" );

  declareProperty( "FinalDLLCheck", m_doFinalDllCheck = false,
                   "Flag to turn on final DLL and hypothesis check" );

  declareProperty( "RichDetCheck", m_richCheck = true,
                   "Flag to turn on RICH check in LL minimisation (turn off for max precision)" );

  declareProperty( "MaxTrackChangesPerIt", m_maxTkChanges = 5,
                   "Maximum number of tracks to change in a single event iteration" );

  declareProperty( "MaxIterationRetries", m_maxItRetries = 10, "Maximum retries" );
}

//=============================================================================
// Destructor
//=============================================================================
LikelihoodTool::~LikelihoodTool() {}

//  Initialize
StatusCode LikelihoodTool::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = Rich::Rec::GlobalPID::ToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // tools
  acquireTool( "GPIDTrackCreator",       m_gtkCreator );
  acquireTool( "RichPhotonSignal",        m_photonSig );
  acquireTool( "RichExpectedTrackSignal", m_tkSignal  );

  // trick to force pre-loading of various tools. Avoids loading
  // during first processed event and thus biasing any timing numbers
  pixelCreator();
  photonCreator();
  statusCreator();

  // Initialise parameters
  m_logMinSig = std::log( std::exp(m_minSig) - 1.0 );

  // Printout some initialisation info
  info() << "Maximum event iterations                  = " << m_maxEventIterations << endreq;
  info() << "Minimum signal for LL calculation         = " << m_minSig << endreq;
  info() << "Track freeze-out DLL value                = " << freezeOutDll() << endreq;
  info() << "Track forced change DLL value             = " << forceChangeDll() << endreq;
  info() << "Maximum track changes per event iteration = " << m_maxTkChanges << endreq;

  return sc;
}

//=============================================================================

const LHCb::RichPID * LikelihoodTool::pid( const LHCb::Track * track ) const
{
  LHCb::RichRecTrack       * rtrack = trackCreator()->newTrack(track);
  LHCb::RichGlobalPIDTrack * gtrack = ( rtrack ? m_gtkCreator->createTrack(rtrack) : NULL );
  return ( gtrack ? this->pid(gtrack) : NULL );
}

void LikelihoodTool::pids( const LHCb::Track::ConstVector & tracks,
                           LHCb::RichPID::ConstVector & richpids ) const
{
  // purge output PID container
  richpids.clear();
  richpids.reserve(tracks.size());

  // make a local container of Global PID tracks
  LHCb::RichGlobalPIDTrack::Vector gtracks;
  gtracks.reserve(tracks.size());
  for ( LHCb::Track::ConstVector::const_iterator iT = tracks.begin();
        iT != tracks.end(); ++iT )
  {
    LHCb::RichRecTrack * rtrack = trackCreator()->newTrack(*iT);
    LHCb::RichGlobalPIDTrack * gtrack = ( rtrack ? m_gtkCreator->createTrack(rtrack) : NULL );
    gtracks.push_back( gtrack );
    richpids.push_back( gtrack ? gtrack->globalPID() : NULL );
  }

  // PID these tracks
  this->pids(gtracks);
}

//=============================================================================

LHCb::RichGlobalPID * LikelihoodTool::pid( LHCb::RichGlobalPIDTrack * track ) const
{
  LHCb::RichGlobalPIDTrack::Vector gtracks(1,track);
  this->pids(gtracks);
  return gtracks.front()->globalPID();
}

void LikelihoodTool::pids( const LHCb::RichGlobalPIDTrack::Vector & tracks ) const
{
  // Update currently considered track list
  m_gpidTracksToPID = &tracks;

  // Compute complete likelihood for event with starting hypotheses
  m_currentBestLL = logLikelihood();

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
  if ( tracks.size() > 1 && nChangeFirstIt>0 )
  {

    // Do the event iterations
    eventIteration = doIterations();

    // Final check, to see if we are at the global best set of hypos
    if ( m_doFinalDllCheck )
    {

      unsigned int nChange(1), nRetries(0);
      while ( nChange>0 && nRetries<m_maxItRetries )
      {
        ++nRetries;

        nChange = initBestLogLikelihood();
        if ( nChange > 0 )
        {
          // Not quite at best hypos yet ...
          std::ostringstream mess;
          mess << nChange << " track(s) changed hypothesis after LL minimisation "
               << nRetries << " -> Re-running";
          Warning( mess.str(), StatusCode::SUCCESS, 3 ).ignore();
          // Rerun iterations
          if ( msgLevel(MSG::DEBUG) )
          { debug() << "Re-running event iteratons" << endreq; }
          eventIteration += doIterations();
        }
        m_currentBestLL = logLikelihood();

        if ( nRetries == m_maxItRetries && nChange > 0 )
        {
          Warning( "Maximum number of iteration re-tries reached", 
                   StatusCode::SUCCESS ).ignore();
        }

      } // retry while loop
      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << "Ran " << nRetries << " iteration re-tries" << endreq;
      }

    } // final check

  } // changes on first try

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Performed " << eventIteration
            << " event minimisation iteration(s). Final LogL = "
            << m_currentBestLL << endreq;
  }

}

//=============================================================================

unsigned int LikelihoodTool::doIterations() const
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
               StatusCode::SUCCESS ).ignore();
      break;
    }

  } // end iteration while loop

  return eventIteration;
}

unsigned int LikelihoodTool::initBestLogLikelihood() const
{
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Running initial log likelihood maximisation" << endreq;

  InitTrackInfo::Vector minTrackData;
  minTrackData.reserve(30); // average number of tracks that change DLL
  m_trackList.clear();

  // Loop over all tracks and find best hypothesis for each track
  {for ( LHCb::RichGlobalPIDTrack::Vector::const_iterator track = m_gpidTracksToPID->begin();
         track != m_gpidTracksToPID->end(); ++track )
  {
    LHCb::RichRecTrack * rRTrack = (*track)->richRecTrack();

    // skip frozen tracks
    if ( (*track)->frozenHypo() )
    {
      if ( msgLevel(MSG::DEBUG) )
      { debug() << "  -> Skipping globally frozen track " << (*track)->key() << endreq; }
      continue;
    }
    // Initialise starting values
    double mindeltaLL = 99999.;
    Rich::ParticleIDType minHypo = rRTrack->currentHypothesis();

    // Loop over all particle codes
    for ( Rich::Particles::const_iterator hypo = pidTypes().begin();
          hypo != pidTypes().end(); ++hypo )
    {

      // Skip analysing starting hypothesis
      if ( *hypo == rRTrack->currentHypothesis() ) continue;

      // calculate delta logLikelihood for event with new track hypothesis
      const double deltaLogL = deltaLogLikelihood( rRTrack, *hypo );

      // Set the value for deltaLL for this hypothesis
      (*track)->globalPID()->setParticleDeltaLL( *hypo, deltaLogL );

      // Set new minimum if lower logLikelihood is achieved
      if ( deltaLogL < mindeltaLL )
      {
        if ( 0 != deltaLogL ) mindeltaLL = deltaLogL;
        if ( deltaLogL < m_epsilon ) minHypo = *hypo;
      }

      // In case the threshold is reached, skip other hypotheses
      bool threshold = true;
      for ( Rich::Particles::const_iterator hypo2 = hypo; hypo2 != pidTypes().end(); ++hypo2 )
      {
        if ( m_tkSignal->nDetectablePhotons( rRTrack, *hypo2 ) > 0 )
        {
          threshold = false; break;
        }
      }
      if ( threshold )
      {
        for ( Rich::Particles::const_iterator hypo3 = hypo;
              hypo3 != pidTypes().end(); ++hypo3 )
        {
          (*track)->globalPID()->setParticleDeltaLL( *hypo3, deltaLogL );
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

void LikelihoodTool::findBestLogLikelihood( MinTrList & minTracks ) const
{
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Finding best overall log likelihood" << endreq;

  // update RICH flags
  if (m_richCheck) updateRichFlags(minTracks);

  // Initialise
  minTracks.clear();
  LHCb::RichGlobalPIDTrack * overallMinTrack = NULL;
  Rich::ParticleIDType overallMinHypo  = Rich::Unknown;
  double minDLLs                       = 0;
  double minEventDll                   = 999999;

  // sort Track list according to delta LL
  std::sort( m_trackList.begin(), m_trackList.end() );

  // Loop on all tracks
  for ( TrackList::iterator iP = m_trackList.begin();
        iP != m_trackList.end(); ++iP )
  {
    LHCb::RichGlobalPIDTrack * gTrack = (*iP).second;
    LHCb::RichRecTrack * rRTrack = gTrack->richRecTrack();

    if ( msgLevel(MSG::DEBUG) )
      debug() << " -> Track " << gTrack->key() << " DLL = " << (*iP).first << endreq;

    // skip globally frozen tracks
    if ( gTrack->frozenHypo() )
    {
      if ( msgLevel(MSG::DEBUG) )
      { debug() << "  -> Skipping globally frozen track" << gTrack->key() << endreq; }
      continue;
    }

    // skip tracks frozen for this iteration
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
    for ( Rich::Particles::const_iterator hypo = pidTypes().begin();
          hypo != pidTypes().end(); ++hypo )
    {

      // Skip analysing starting hpyothesis
      if ( *hypo == rRTrack->currentHypothesis() ) continue;

      // calculate delta logLikelihood for event with new track hypothesis
      const double deltaLogL = deltaLogLikelihood( rRTrack, *hypo );
      if ( msgLevel(MSG::VERBOSE) )
      {
        verbose() << "  -> Trying " << *hypo << " : DLL = " << deltaLogL << endreq;
      }

      // Set the value for deltaLL for this hypothesis
      gTrack->globalPID()->setParticleDeltaLL( *hypo, deltaLogL );

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
                    << *hypo << " to " << rRTrack->currentHypothesis()
                    << ". DLL = " << deltaLogL << endreq ;
          }

          // set that a new best is found and update best dll and type
          minFound    = true;
          (*iP).first = deltaLogL;
          minHypo     = *hypo;
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
            overallMinHypo  = *hypo;
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
      for ( Rich::Particles::const_iterator hypo2 = hypo; hypo2 != pidTypes().end(); ++hypo2 )
      {
        if ( m_tkSignal->nDetectablePhotons( gTrack->richRecTrack(), *hypo2 ) > 0 )
        {
          threshold = false; break;
        }
      }
      if ( threshold )
      {
        for ( Rich::Particles::const_iterator hypo3 = hypo;
              hypo3 != pidTypes().end(); ++hypo3 )
        {
          gTrack->globalPID()->setParticleDeltaLL( *hypo3, deltaLogL );
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

void LikelihoodTool::updateRichFlags( const MinTrList & minTracks ) const
{
  if ( !minTracks.empty() )
  {
    m_inR1 = false; m_inR2 = false;
    for ( MinTrList::const_iterator iT = minTracks.begin();
          iT != minTracks.end(); ++iT )
    {
      LHCb::RichRecTrack * rTk = ((*iT).first)->richRecTrack();
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
LikelihoodTool::deltaLogLikelihood( LHCb::RichRecTrack * track,
                                    const Rich::ParticleIDType newHypo ) const
{

  // Change due to track expectation
  double deltaLL = ( m_tkSignal->nTotalObservablePhotons( track, newHypo ) -
                     m_tkSignal->nTotalObservablePhotons( track, track->currentHypothesis() ) );

  // Photon part
  LHCb::RichRecTrack::Pixels & pixels = track->richRecPixels();
  const LHCb::RichRecTrack::Pixels::iterator ePixel = pixels.end();
  for ( LHCb::RichRecTrack::Pixels::iterator iPixel = pixels.begin();
        iPixel != ePixel; ++iPixel )
  {

    // photons for this pixel
    LHCb::RichRecPixel::Photons & photons = (*iPixel)->richRecPhotons();
    if ( !photons.empty() )
    {

      double oldSig = (*iPixel)->currentBackground();
      double newSig = oldSig;

      // Loop over photons for this pixel
      const LHCb::RichRecPixel::Photons::iterator iPhotonEnd    = photons.end();
      for ( LHCb::RichRecPixel::Photons::const_iterator iPhoton = photons.begin();
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

      // increment change to likelihood for this pixel
      deltaLL -= sigFunc(newSig) - sigFunc(oldSig);

    } // end photons not empty

  } // end pixel loop

  return deltaLL;
}

double LikelihoodTool::logLikelihood() const
{
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Computing event log likelihood using " << m_gpidTracksToPID->size()
            << " tracks and " <<  richPixels()->size() << " pixels" << endreq;
  }

  // Loop over tracks to form total expected hits part of LL
  double trackLL = 0.0;
  for ( LHCb::RichGlobalPIDTrack::Vector::const_iterator track = m_gpidTracksToPID->begin();
        track != m_gpidTracksToPID->end(); ++track )
  {
    // Sum expected photons from each track with current assumed hypotheses
    LHCb::RichRecTrack * rRTrack = (*track)->richRecTrack();
    if ( !rRTrack->inUse() ) continue;
    trackLL += m_tkSignal->nTotalObservablePhotons( rRTrack,
                                                    rRTrack->currentHypothesis() );
  } // end track loop

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << " -> Track contribution    = " << trackLL << endreq;
  }

  // Pixel loop
  double pixelLL = 0.0;
  for ( LHCb::RichRecPixels::const_iterator iPixel = richPixels()->begin();
        iPixel != richPixels()->end(); ++iPixel )
  {
    const LHCb::RichRecPixel * pixel = *iPixel;

    // loop over all the photons for this pixel
    double photonSig = 0.0;
    bool foundSelectedTrack = false;
    const LHCb::RichRecPixel::Photons & photons = pixel->richRecPhotons();
    const LHCb::RichRecPixel::Photons::const_iterator iPhotonEnd = photons.end();
    for ( LHCb::RichRecPixel::Photons::const_iterator iPhoton    = photons.begin();
          iPhoton != iPhotonEnd; ++iPhoton )
    {
      const LHCb::RichRecTrack * rRTrack = (*iPhoton)->richRecTrack();
      if ( rRTrack->inUse() )
      {
        if ( msgLevel(MSG::VERBOSE) )
        {
          verbose() << "  -> Using photon : track=" << rRTrack->key()
                    << " pixel=" << pixel->key()
                    << " bkg=" << pixel->currentBackground() 
                    << " sig=" << m_photonSig->predictedPixelSignal( *iPhoton,
                                                                     rRTrack->currentHypothesis() )
                    << endreq;
        }
        foundSelectedTrack = true;
        photonSig += m_photonSig->predictedPixelSignal( *iPhoton,
                                                        rRTrack->currentHypothesis() );
      }
    } // loop over photons
    if ( foundSelectedTrack )
    {
      pixelLL -= sigFunc( photonSig + pixel->currentBackground() );
    }

  } // loop over all pixels

  // Detector contribution
  const double detectorLL = ( richStatus()->detOverallBkg( Rich::Rich1 ) +
                              richStatus()->detOverallBkg( Rich::Rich2 ) );

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << " -> Pixel contribution    = " << pixelLL << endreq;
    debug() << " -> Detector contribution = " << detectorLL << endreq;
  }

  // return overall LL
  return ( trackLL + pixelLL );
}
