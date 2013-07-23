
//--------------------------------------------------------------------------
/** @file RichGlobalPIDLikelihoodTool.cpp
 *
 *  Implementation file for RICH Global PID tool : Rich::Rec::GlobalPID::LikelihoodTool
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2008-03-01
 */
//--------------------------------------------------------------------------

// local
#include "RichGlobalPIDLikelihoodTool.h"

// namespaces
using namespace Rich::Rec::GlobalPID;

namespace 
{ // Parameters
  const double limitA = 0.001;
  const double limitB = 0.01;
  const double limitC = 0.1;
  const double limitD = 1.0;
}

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
                   "Track freeze out value (The point at which it is no longer considered for change)" );

  declareProperty( "TrackForceChangeDLL", m_forceChangeDll = -2,
                   "Track DLL Thresdhold for forced change" );

  declareProperty( "FinalDLLCheck", m_doFinalDllCheck = false,
                   "Flag to turn on final DLL and hypothesis check" );

  declareProperty( "RichDetCheck", m_richCheck = true,
                   "Flag to turn on RICH check in LL minimisation (turn off for max precision)" );

  declareProperty( "MaxTrackChangesPerIt", m_maxTkChanges = 5,
                   "Maximum number of tracks to change in a single event iteration" );

  declareProperty( "MaxIterationRetries", m_maxItRetries = 10, "Maximum retries" );

  //setProperty( "OutputLevel", MSG::DEBUG );
}

//=============================================================================
// Destructor
//=============================================================================
LikelihoodTool::~LikelihoodTool() {}

// Initialize
StatusCode LikelihoodTool::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = Rich::Rec::GlobalPID::ToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // tools
  acquireTool( "GPIDTrackCreator",        m_gtkCreator );
  acquireTool( "RichPhotonSignal",        m_photonSig  );
  acquireTool( "RichExpectedTrackSignal", m_tkSignal   );

  // trick to force pre-loading of various tools. Avoids loading
  // during first processed event and thus biasing any timing numbers
  pixelCreator();
  photonCreator();
  statusCreator();

  // Check if power series expansions are OK
  if ( m_minSig < 0.999*limitA )
  {
    return Warning( "Power series interpolations for log(exp(x)-1) need retuning" );
  }

  // Initialise parameters
  m_logMinSig = logExp(m_minSig);

  // Printout some initialisation info
  info() << "Maximum event iterations                  = " << m_maxEventIterations << endmsg;
  info() << "Minimum signal for LL calculation         = " << m_minSig << endmsg;
  info() << "Track freeze-out DLL value                = " << freezeOutDll() << endmsg;
  info() << "Track forced change DLL value             = " << forceChangeDll() << endmsg;
  info() << "Maximum track changes per event iteration = " << m_maxTkChanges << endmsg;

  return sc;
}

//=============================================================================

StatusCode LikelihoodTool::finalize()
{
  return Rich::Rec::GlobalPID::ToolBase::finalize();
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
            << " : " << nChangeFirstIt << " tracks changed hypothesis" << endmsg;
  }
  m_currentBestLL = finalLL;

  // did any tracks change in the first initialisation
  unsigned int eventIteration = 0;
  if ( tracks.size() > 1 && nChangeFirstIt > 0 )
  {

    // Do the event iterations
    eventIteration = doIterations();

    // Final check, to see if we are at the global best set of hypos
    if ( m_doFinalDllCheck )
    {

      unsigned int nChange(1), nRetries(0);
      while ( nChange > 0 && nRetries < m_maxItRetries )
      {
        ++nRetries;

        // First pass at minimising
        nChange = initBestLogLikelihood();
        if ( nChange > 0 )
        {
          // Not quite at best hypos yet ...
          std::ostringstream mess;
          mess << nChange << " track(s) changed hypo after iteration " << nRetries;
          ++counter( mess.str() );

          // debug printout
          if ( msgLevel(MSG::DEBUG) ) { debug() << mess.str() << endmsg; }

          // Rerun iterations again
          if ( msgLevel(MSG::DEBUG) )
          { debug() << "Re-running event iteratons" << endmsg; }

          eventIteration += doIterations();

        }

        // Compute the event likelihood from scratch
        m_currentBestLL = logLikelihood();

        if ( nRetries == m_maxItRetries && nChange > 0 )
        {
          Warning( "Maximum number of iteration re-tries reached",
                   StatusCode::SUCCESS ).ignore();
        }

      } // retry while loop

      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << "Ran " << nRetries << " iteration re-tries" << endmsg;
      }

    } // final check

  } // changes on first try

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Performed " << eventIteration
            << " event minimisation iteration(s). Final LogL = "
            << m_currentBestLL << endmsg;
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
      debug() << "Event Iteration " << eventIteration << endmsg;

    // Iterate finding the best likelihood
    findBestLogLikelihood( minTracks );

    // set track hypotheses to the current best
    if ( !minTracks.empty() )
    {
      for ( MinTrList::iterator iTrack = minTracks.begin();
            iTrack != minTracks.end(); ++iTrack )
      {
        if ( Rich::Unknown == iTrack->second )
        {
          err() << " Track " << (iTrack->first)->key()
                << " has been Id'ed as Unknown !!" << endmsg;
        }
        else
        {
          if ( msgLevel(MSG::DEBUG) )
          {
            debug() << " -> Changing Track " << (iTrack->first)->key()
                    << " hypothesis to from "
                    << (iTrack->first)->richRecTrack()->currentHypothesis()
                    << " to " << iTrack->second << ". LogL = " << m_currentBestLL
                    << endmsg;
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
      { debug() << " -> ALL DONE. Quitting event iterations" << endmsg; }
      tryAgain = false;
    }
    else // have one last try in both riches
    {
      if ( msgLevel(MSG::DEBUG) )
      { debug() << " -> Last try in both RICH detectors" << endmsg; }
      m_inR1 = true;
      m_inR2 = true;
    }

    ++eventIteration;
    if ( eventIteration > m_maxEventIterations )
    {
      Warning( "Taken more than maximum number of iterations -> quitting",
               StatusCode::SUCCESS ).ignore();
      break;
    }

  } // end iteration while loop

  return eventIteration;
}

//=============================================================================

unsigned int LikelihoodTool::initBestLogLikelihood() const
{
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Running initial log likelihood maximisation" << endmsg;

  InitTrackInfo::Vector minTrackData;
  minTrackData.reserve(30); // average number of tracks that change DLL
  m_trackList.clear();

  // Loop over all tracks and find best hypothesis for each track
  for ( LHCb::RichGlobalPIDTrack::Vector::const_iterator track = m_gpidTracksToPID->begin();
        track != m_gpidTracksToPID->end(); ++track )
  {
    LHCb::RichRecTrack * rRTrack = (*track)->richRecTrack();
    if ( msgLevel(MSG::VERBOSE) )
    { verbose() << " -> Track " << rRTrack->key() << endmsg; }

    // skip frozen tracks
    if ( (*track)->frozenHypo() )
    {
      if ( msgLevel(MSG::DEBUG) )
      { debug() << "  -> Skipping globally frozen track " << (*track)->key() << endmsg; }
      continue;
    }

    // Initialise starting values
    double mindeltaLL = boost::numeric::bounds<double>::highest();
    Rich::ParticleIDType minHypo = rRTrack->currentHypothesis();

    // Loop over all particle codes
    for ( Rich::Particles::const_iterator hypo = pidTypes().begin();
          hypo != pidTypes().end(); ++hypo )
    {

      // Skip analysing starting hypothesis
      if ( *hypo == rRTrack->currentHypothesis() ) continue;

      // calculate delta logLikelihood for event with new track hypothesis
      const double deltaLogL = deltaLogLikelihood( rRTrack, *hypo );
      if ( msgLevel(MSG::VERBOSE) )
      { verbose() << "  -> " << *hypo << " dLL = " << deltaLogL << endmsg; }

      // Set the value for deltaLL for this hypothesis
      (*track)->globalPID()->setParticleDeltaLL( *hypo, (float)(deltaLogL) );

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
          (*track)->globalPID()->setParticleDeltaLL( *hypo3, (float)(deltaLogL) );
        }
        break;
      }

    } // end particle for loop

    if ( msgLevel(MSG::VERBOSE) )
    { verbose() << "  -> Min Hypo " << minHypo << endmsg; }

    // Save info on tracks that have a better minimum hypothesis
    if ( minHypo != rRTrack->currentHypothesis() )
    {
      minTrackData.push_back( InitTrackInfo(*track,minHypo,mindeltaLL) );
    }

    // Add this track / deltaLL to the track list
    m_trackList.push_back( TrackPair(mindeltaLL,*track) );

  } // end track for loop

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
              << ". DLL = " << (*iT).minDLL << endmsg ;
    }
    // set best hypothesis
    (*iT).pidTrack->richRecTrack()->setCurrentHypothesis( (*iT).hypo );
    // set delta LL to zero for best hypothesis
    (*iT).pidTrack->globalPID()->setParticleDeltaLL( (*iT).hypo, 0 );
  }

  return minTrackData.size();
}

//=============================================================================

void LikelihoodTool::printTrackList( const MSG::Level level ) const
{
  if ( msgLevel(level) )
  {
    msgStream(level) << m_trackList.size() << " Tracks in DLL list" << endmsg;
    for ( TrackList::const_iterator iP = m_trackList.begin();
          iP != m_trackList.end(); ++iP )
    {
      const LHCb::RichGlobalPIDTrack * gTrack = (*iP).second;
      msgStream(level) << " -> Track " << gTrack->key()
                       << " DLL = " << (*iP).first
                       << " " << gTrack->richRecTrack()->currentHypothesis()
                       << endmsg;
    }
  }
}

//=============================================================================

void LikelihoodTool::findBestLogLikelihood( MinTrList & minTracks ) const
{
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Finding best overall log likelihood" << endmsg;

  // update RICH flags
  if (m_richCheck) updateRichFlags(minTracks);

  // Initialise
  minTracks.clear();
  LHCb::RichGlobalPIDTrack * overallMinTrack = NULL;
  Rich::ParticleIDType overallMinHypo  = Rich::Unknown;
  double minDLLs                       = 0;
  double minEventDll                   = boost::numeric::bounds<double>::highest();

  // sort Track list according to delta LL
  sortTrackList();
  if ( msgLevel(MSG::DEBUG) ) { printTrackList(MSG::DEBUG); }

  // Loop on all tracks
  for ( TrackList::iterator iP = m_trackList.begin();
        iP != m_trackList.end(); ++iP )
  {
    LHCb::RichGlobalPIDTrack * gTrack = (*iP).second;
    LHCb::RichRecTrack * rRTrack = gTrack->richRecTrack();

    if ( msgLevel(MSG::DEBUG) )
      debug() << " -> Track " << gTrack->key() << " DLL = " << (*iP).first << endmsg;

    // skip globally frozen tracks
    if ( gTrack->frozenHypo() )
    {
      if ( msgLevel(MSG::DEBUG) )
      { debug() << "  -> Skipping globally frozen track" << gTrack->key() << endmsg; }
      continue;
    }

    // skip tracks frozen for this iteration
    if ( (*iP).first > freezeOutDll() )
    {
      if ( msgLevel(MSG::DEBUG) )
      { debug() << "  -> Freeze-out value = " << freezeOutDll()
                << " -> Aborting remaining tracks" << endmsg; }
      break;
    }

    // skip tracks that do not share a RICH with the last changed tracks
    // for these tracks it is not possible that the hypos could change
    if ( m_richCheck && !( (m_inR1 && rRTrack->inRICH1()) ||
                           (m_inR2 && rRTrack->inRICH2()) ) )
    {
      if ( msgLevel(MSG::DEBUG) )
      { debug() << "  -> Skipping track in unaltered RICH" << endmsg; }
      continue;
    }

    bool addto(false), minFound(false);
    double minTrackDll = boost::numeric::bounds<double>::highest();
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
        verbose() << "  -> Trying " << *hypo << " : DLL = " << deltaLogL << endmsg;
      }

      // Set the value for deltaLL for this hypothesis
      gTrack->globalPID()->setParticleDeltaLL( *hypo, (float)(deltaLogL) );

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
                    << ". DLL = " << deltaLogL << endmsg ;
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
              debug() << "     -> Adding to force change list" << endmsg;
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
          gTrack->globalPID()->setParticleDeltaLL( *hypo3, (float)(deltaLogL) );
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
                << " tracks to change. Aborting track loop" << endmsg;
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
  { debug() << "Found " << minTracks.size() << " track(s) to change" << endmsg; }

}

//=============================================================================

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

//=============================================================================

double
LikelihoodTool::deltaLogLikelihood( LHCb::RichRecTrack * track,
                                    const Rich::ParticleIDType newHypo ) const
{

  // Change due to track expectation
  const double tkDeltaLL =
    ( m_tkSignal->nTotalObservablePhotons( track, newHypo ) -
      m_tkSignal->nTotalObservablePhotons( track, track->currentHypothesis() ) );

  // Photon part
  double pixDeltaLL(0);
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
          m_photonSig->predictedPixelSignal( *iPhoton,
                                             (*iPhoton)->richRecTrack()->currentHypothesis() );
        oldSig += tmpOldSig;
        newSig += ( (*iPhoton)->richRecTrack() != track ? tmpOldSig :
                    m_photonSig->predictedPixelSignal( *iPhoton, newHypo ) );
      } // end photon loop

      // increment change to likelihood for this pixel
      pixDeltaLL -= ( sigFunc(newSig) - sigFunc(oldSig) );

    } // end photons not empty

  } // end pixel loop

  //   if ( msgLevel(MSG::VERBOSE) )
  //   {
  //     verbose() << "deltaLogLikelihood : Track " << track->key()
  //               << " " << track->currentHypothesis()
  //               << " " << newHypo
  //               << " tkDLL=" << tkDeltaLL
  //               << " pixDLL=" << pixDeltaLL
  //               << endmsg;
  //   }

  return ( pixDeltaLL + tkDeltaLL );
}

//=============================================================================

double LikelihoodTool::logLikelihood() const
{
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Computing event log likelihood using " << m_gpidTracksToPID->size()
            << " tracks and " <<  richPixels()->size() << " pixels" << endmsg;
  }

  // Loop over tracks to form total expected hits part of LL
  double trackLL = 0.0;
  for ( LHCb::RichGlobalPIDTrack::Vector::const_iterator track = m_gpidTracksToPID->begin();
        track != m_gpidTracksToPID->end(); ++track )
  {
    // Sum expected photons from each track with current assumed hypotheses
    LHCb::RichRecTrack * rRTrack = (*track)->richRecTrack();
    if ( !rRTrack->inUse() ) continue;
    const double obsPhots = m_tkSignal->nTotalObservablePhotons( rRTrack,
                                                                 rRTrack->currentHypothesis() );
    trackLL += obsPhots;
    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << " -> Track " << rRTrack->key() << " obsPhots=" << obsPhots << endmsg;
    }
  } // end track loop

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
                    << endmsg;
        }
        foundSelectedTrack = true;
        photonSig += m_photonSig->predictedPixelSignal( *iPhoton,
                                                        rRTrack->currentHypothesis() );
      }
    } // end loop over photons

    if ( foundSelectedTrack )
    {
      const double pixSig = sigFunc( photonSig + pixel->currentBackground() );
      pixelLL -= pixSig;
      if ( msgLevel(MSG::VERBOSE) )
      {
        verbose() << " -> Pixel " << pixel->key() << " " << pixSig << endmsg;
      }
    }

  } // loop over all pixels

  // Detector contribution
  const double detectorLL = ( richStatus()->detOverallBkg( Rich::Rich1 ) +
                              richStatus()->detOverallBkg( Rich::Rich2 ) );

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << " -> Track contribution    = " << trackLL << endmsg;
    debug() << " -> Pixel contribution    = " << pixelLL << endmsg;
    debug() << " -> Detector contribution = " << detectorLL << endmsg;
  }

  // return overall LL
  return ( trackLL + pixelLL );
}

//=============================================================================

double LikelihoodTool::logExp( const double x ) const
{
  double res(0);
  if ( x <= limitD )
  {
    // A collection of rational power series covering the important range
    // note by construction this function should never be called for x < limitA

    const double xx    = x*x;
    const double xxx   = xx*x;
    const double xxxx  = xx*xx;
    const double xxxxx = xx*xxx;

    if      ( x > limitC )
    {
      res = (-5.751779337152293 - 261.58791552313113*x -
             1610.1902353909695*xx - 291.61172549536417*xxx +
             3733.957211885683*xxxx + 1224.2104172168554*xxxxx)/
        (1.0 + 79.52981108433892*x + 953.4570349071099*xx +
         2638.609797400796*xxx + 1506.9612115322623*xxxx -
         27.33558114045007*xxxxx);
    }
    else if ( x > limitB )
    {
      res = (-7.845788509794026 - 3428.7804135353526*x -
             228752.20145929293*xx - 3.082032088759535e6*xxx -
             3.836270197409883e6*xxxx + 1.2251900378118051e7*xxxxx)/
        (1.0 + 638.7306815040638*x + 60430.91709817034*xx +
         1.315432074531156e6*xxx + 6.373056770682967e6*xxxx +
         3.3914176474223877e6*xxxxx);
    }
    else if ( x > limitA )
    {
      res = (-10.160864268729455 - 49897.23275778952*x -
             3.855669108991894e7*xx - 6.777802095268419e9*xxx -
             2.421987003565588e11*xxxx - 3.5610129242332263e11*xxxxx)/
        (1.0 + 6487.897657865318*x + 6.294785881144457e6*xx +
         1.4333658673633337e9*xxx + 7.670700007081306e10*xxxx +
         6.06654149712832e11*xxxxx);
    }
    else
    {
      // should never get here. But just in case ...
      res = std::log( std::exp(x) - 1.0 );
    }
  }
  else
  {
    // Very very rarely called in this regime, so just use the full fat version
    res = std::log( std::exp(x) - 1.0 );
  }
  return res;
}

//=============================================================================

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( LikelihoodTool )

//=============================================================================
