
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

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
LikelihoodTool::LikelihoodTool( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : Rich::Rec::GlobalPID::ToolBase ( type, name, parent )
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
  //if ( m_minSig < 0.999*limitA )
  //{
  //  return Warning( "Power series interpolations for log(exp(x)-1) need retuning" );
  //}

  // Initialise parameters
  m_logMinSig = logExp(m_minSig);

  // Initialise the interpolator
  m_logExpLookUp.init( m_minSig, 1.0, 10000 );

  // Printout some initialisation info
  _ri_debug << "Maximum event iterations                  = " << m_maxEventIterations << endmsg;
  _ri_debug << "Minimum signal for LL calculation         = " << m_minSig << endmsg;
  _ri_debug << "Track freeze-out DLL value                = " << freezeOutDll() << endmsg;
  _ri_debug << "Track forced change DLL value             = " << forceChangeDll() << endmsg;
  _ri_debug << "Maximum track changes per event iteration = " << m_maxTkChanges << endmsg;

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
  LHCb::RichGlobalPIDTrack * gtrack = ( rtrack ? m_gtkCreator->createTrack(rtrack) : nullptr );
  return ( gtrack ? this->pid(gtrack) : nullptr );
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
  for ( const auto * T : tracks )
  {
    auto * rtrack = trackCreator()->newTrack(T);
    auto * gtrack = ( rtrack ? m_gtkCreator->createTrack(rtrack) : nullptr );
    gtracks.push_back( gtrack );
    richpids.push_back( gtrack ? gtrack->globalPID() : nullptr );
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

  // Pre-fill all the predicted pixel signal values for each hypo, for speed
  m_photonSig->prefillPredictedPixelSignal();

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
          _ri_debug << mess.str() << endmsg; 

          // Rerun iterations again
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

      _ri_debug << "Ran " << nRetries << " iteration re-tries" << endmsg;

    } // final check

  } // changes on first try

  _ri_debug << "Performed " << eventIteration
            << " event minimisation iteration(s). Final LogL = "
            << m_currentBestLL << endmsg;
  
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
    _ri_debug << "Event Iteration " << eventIteration << endmsg;

    // Iterate finding the best likelihood
    findBestLogLikelihood( minTracks );

    // set track hypotheses to the current best
    if ( !minTracks.empty() )
    {
      for ( auto & tk : minTracks )
      {
        if ( Rich::Unknown == tk.second )
        {
          err() << " Track " << (tk.first)->key()
                << " has been Id'ed as Unknown !!" << endmsg;
        }
        else
        {
          _ri_debug << " -> Changing Track " << (tk.first)->key()
                    << " hypothesis to from "
                    << (tk.first)->richRecTrack()->currentHypothesis()
                    << " to " << tk.second << ". LogL = " << m_currentBestLL
                    << endmsg;
          // Update hypothesis to best
          (tk.first)->richRecTrack()->setCurrentHypothesis( tk.second );
          // set deltaLL to zero
          (tk.first)->globalPID()->setParticleDeltaLL( tk.second, 0 );
        }
      }
    }
    // only quit if last iteration was with both riches
    else if ( m_inR1 && m_inR2 )
    {
      _ri_debug << " -> ALL DONE. Quitting event iterations" << endmsg; 
      tryAgain = false;
    }
    else // have one last try in both riches
    {
      _ri_debug << " -> Last try in both RICH detectors" << endmsg;
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
  _ri_debug << "Running initial log likelihood maximisation" << endmsg;

  InitTrackInfo::Vector minTrackData;
  minTrackData.reserve(30); // average number of tracks that change DLL
  m_trackList.clear();

  // Loop over all tracks and find best hypothesis for each track
  for ( auto * track : *m_gpidTracksToPID )
  {
    auto * rRTrack = track->richRecTrack();
    _ri_verbo << " -> Track " << rRTrack->key() << endmsg; 

    // skip frozen tracks
    if ( track->frozenHypo() )
    {
      _ri_debug << "  -> Skipping globally frozen track " << track->key() << endmsg;
      continue;
    }

    // Initialise starting values
    double mindeltaLL = boost::numeric::bounds<double>::highest();
    Rich::ParticleIDType minHypo = rRTrack->currentHypothesis();

    // Loop over all particle codes
    
    for ( auto hypo = pidTypes().begin(); hypo != pidTypes().end(); ++hypo )
    {

      // Skip analysing starting hypothesis
      if ( *hypo == rRTrack->currentHypothesis() ) continue;

      // calculate delta logLikelihood for event with new track hypothesis
      const double deltaLogL = deltaLogLikelihood( rRTrack, *hypo );
      _ri_verbo << "  -> " << *hypo << " dLL = " << deltaLogL << endmsg;

      // Set the value for deltaLL for this hypothesis
      track->globalPID()->setParticleDeltaLL( *hypo, (float)(deltaLogL) );

      // Set new minimum if lower logLikelihood is achieved
      if ( deltaLogL < mindeltaLL )
      {
        if ( 0 != deltaLogL ) mindeltaLL = deltaLogL;
        if ( deltaLogL < m_epsilon ) minHypo = *hypo;
      }

      // In case the threshold is reached, skip other hypotheses
      bool threshold = true;
      for ( auto hypo2 = hypo; hypo2 != pidTypes().end(); ++hypo2 )
      {
        if ( m_tkSignal->nDetectablePhotons( rRTrack, *hypo2 ) > 0 )
        {
          threshold = false; break;
        }
      }
      if ( threshold )
      {
        for ( auto hypo3 = hypo; hypo3 != pidTypes().end(); ++hypo3 )
        {
          track->globalPID()->setParticleDeltaLL( *hypo3, (float)(deltaLogL) );
        }
        break;
      }

    } // end particle for loop

    _ri_verbo << "  -> Min Hypo " << minHypo << endmsg;

    // Save info on tracks that have a better minimum hypothesis
    if ( minHypo != rRTrack->currentHypothesis() )
    {
      minTrackData.emplace_back( InitTrackInfo(track,minHypo,mindeltaLL) );
    }

    // Add this track / deltaLL to the track list
    m_trackList.emplace_back( TrackPair(mindeltaLL,track) );

  } // end track for loop

  // Finally, set all track hypotheses to their minimum
  for ( const auto& T : minTrackData )
  {
    _ri_debug << " -> Track "
              << boost::format("%4i") % T.pidTrack->key()
              << " prefers hypothesis " << T.hypo << " to "
              << T.pidTrack->richRecTrack()->currentHypothesis()
              << ". DLL = " << T.minDLL << endmsg ;
    // set best hypothesis
    T.pidTrack->richRecTrack()->setCurrentHypothesis( T.hypo );
    // set delta LL to zero for best hypothesis
    T.pidTrack->globalPID()->setParticleDeltaLL( T.hypo, 0 );
  }

  return minTrackData.size();
}

//=============================================================================

void LikelihoodTool::printTrackList( const MSG::Level level ) const
{
  if ( msgLevel(level) )
  {
    msgStream(level) << m_trackList.size() << " Tracks in DLL list" << endmsg;
    for ( const auto & P : m_trackList )
    {
      const auto * gTrack = P.second;
      msgStream(level) << " -> Track " << gTrack->key()
                       << " DLL = " << P.first
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
  LHCb::RichGlobalPIDTrack * overallMinTrack = nullptr;
  Rich::ParticleIDType overallMinHypo  = Rich::Unknown;
  double minDLLs                       = 0;
  double minEventDll                   = boost::numeric::bounds<double>::highest();

  // sort Track list according to delta LL
  sortTrackList();
  if ( msgLevel(MSG::DEBUG) ) { printTrackList(MSG::DEBUG); }

  // Loop on all tracks
  for ( auto & P : m_trackList )
  {
    auto * gTrack  = P.second;
    auto * rRTrack = gTrack->richRecTrack();

    _ri_debug << " -> Track " << gTrack->key() << " DLL = " << P.first << endmsg;

    // skip globally frozen tracks
    if ( gTrack->frozenHypo() )
    {
      _ri_debug << "  -> Skipping globally frozen track" << gTrack->key() << endmsg; 
      continue;
    }

    // skip tracks frozen for this iteration
    if ( P.first > freezeOutDll() )
    {
      _ri_debug << "  -> Freeze-out value = " << freezeOutDll()
                << " -> Aborting remaining tracks" << endmsg;
      break;
    }

    // skip tracks that do not share a RICH with the last changed tracks
    // for these tracks it is not possible that the hypos could change
    if ( m_richCheck && !( (m_inR1 && rRTrack->inRICH1()) ||
                           (m_inR2 && rRTrack->inRICH2()) ) )
    {
      _ri_debug << "  -> Skipping track in unaltered RICH" << endmsg;
      continue;
    }

    bool addto(false), minFound(false);
    double minTrackDll = boost::numeric::bounds<double>::highest();
    Rich::ParticleIDType minHypo = Rich::Unknown;
    for ( auto hypo = pidTypes().begin(); hypo != pidTypes().end(); ++hypo )
    {

      // Skip analysing starting hpyothesis
      if ( *hypo == rRTrack->currentHypothesis() ) continue;

      // calculate delta logLikelihood for event with new track hypothesis
      const double deltaLogL = deltaLogLikelihood( rRTrack, *hypo );
      _ri_verbo << "  -> Trying " << *hypo << " : DLL = " << deltaLogL << endmsg;

      // Set the value for deltaLL for this hypothesis
      gTrack->globalPID()->setParticleDeltaLL( *hypo, (float)(deltaLogL) );

      // is DLL change significant ?
      if ( deltaLogL < m_epsilon )
      {

        // Is this the best hypo for this track
        if ( deltaLogL < minTrackDll )
        {
          _ri_debug << "    -> Track "
                    << boost::format("%4i") % gTrack->key() << " prefers hypothesis "
                    << *hypo << " to " << rRTrack->currentHypothesis()
                    << ". DLL = " << deltaLogL << endmsg ;

          // set that a new best is found and update best dll and type
          minFound    = true;
          P.first     = deltaLogL;
          minHypo     = *hypo;
          minTrackDll = deltaLogL;

          // Is dll change enough to add to force change list
          if ( !addto && (deltaLogL < forceChangeDll()) )
          {
            _ri_debug << "     -> Adding to force change list" << endmsg;
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
      for ( auto hypo2 = hypo; hypo2 != pidTypes().end(); ++hypo2 )
      {
        if ( m_tkSignal->nDetectablePhotons( gTrack->richRecTrack(), *hypo2 ) > 0 )
        {
          threshold = false; break;
        }
      }
      if ( threshold )
      {
        for ( auto hypo3 = hypo; hypo3 != pidTypes().end(); ++hypo3 )
        {
          gTrack->globalPID()->setParticleDeltaLL( *hypo3, (float)(deltaLogL) );
        }
        break;
      }

    } // end hypothesis loop

    // if a good new hypo is not found, just update track dll in container to best
    if ( !minFound ) P.first = minTrackDll;

    // if found tracks with good enough deltaLL, add to force-change map
    if ( addto )
    {
      minTracks[gTrack] = minHypo;
      minDLLs          += minTrackDll;
      P.first           = 0;
      break;
    }

    // do we have enough tracks to change to break out the loop early ?
    if ( minTracks.size() >= m_maxTkChanges )
    {
      _ri_debug << "    -> Found " << minTracks.size()
                << " tracks to change. Aborting track loop" << endmsg;
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

  _ri_debug << "Found " << minTracks.size() << " track(s) to change" << endmsg;

}

//=============================================================================

void LikelihoodTool::updateRichFlags( const MinTrList & minTracks ) const
{
  if ( !minTracks.empty() )
  {
    m_inR1 = false; m_inR2 = false;
    for ( const auto& T : minTracks )
    {
      auto * rTk = (T.first)->richRecTrack();
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
  double deltaLL =
    ( m_tkSignal->nTotalObservablePhotons( track, newHypo ) -
      m_tkSignal->nTotalObservablePhotons( track, track->currentHypothesis() ) );

  // Photon part
  for ( LHCb::RichRecPixel * pix : track->richRecPixels() )
  {

    // photons for this pixel
    LHCb::RichRecPixel::Photons & photons = pix->richRecPhotons();
    if ( !photons.empty() )
    {

      double oldSig = pix->currentBackground();
      double newSig = oldSig;

      // Loop over photons for this pixel
      for ( const LHCb::RichRecPhoton * phot : photons )
      {
        // Skip tracks not in use
        if ( !phot->richRecTrack()->inUse() ) continue;

        // update signal numbers
        const double tmpOldSig = phot->expPixelSignalPhots(phot->richRecTrack()->currentHypothesis()); 
        oldSig += tmpOldSig;
        newSig += ( phot->richRecTrack() != track ? tmpOldSig : phot->expPixelSignalPhots(newHypo) );

      } // end photon loop

      // increment change to likelihood for this pixel
      deltaLL -= ( sigFunc(newSig) - sigFunc(oldSig) );

      // new test way
      //deltaLL -= sigFuncDiff(newSig,oldSig);

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

  return deltaLL;
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
  for ( auto * track : *m_gpidTracksToPID )
  {
    // Sum expected photons from each track with current assumed hypotheses
    auto * rRTrack = track->richRecTrack();
    if ( !rRTrack->inUse() ) continue;
    const double obsPhots = m_tkSignal->nTotalObservablePhotons( rRTrack,
                                                                 rRTrack->currentHypothesis() );
    trackLL += obsPhots;
    _ri_verbo << " -> Track " << rRTrack->key() << " obsPhots=" << obsPhots << endmsg;
  } // end track loop

  // Pixel loop
  double pixelLL = 0.0;
  for ( const LHCb::RichRecPixel * pixel : *richPixels() )
  {

    // loop over all the photons for this pixel
    double photonSig = 0.0;
    bool foundSelectedTrack = false;
    for ( auto * phot : pixel->richRecPhotons() )
    {
      const LHCb::RichRecTrack * rRTrack = phot->richRecTrack();
      if ( rRTrack->inUse() )
      {
        _ri_verbo << "  -> Using photon : track=" << rRTrack->key()
                  << " pixel=" << pixel->key()
                  << " bkg=" << pixel->currentBackground()
                  << " sig=" << m_photonSig->predictedPixelSignal( phot,
                                                                   rRTrack->currentHypothesis() )
                  << endmsg;
        foundSelectedTrack = true;
        //photonSig += m_photonSig->predictedPixelSignal( *iPhoton, rRTrack->currentHypothesis() );
        photonSig += phot->expPixelSignalPhots(rRTrack->currentHypothesis());
      }
    } // end loop over photons

    if ( foundSelectedTrack )
    {
      const double pixSig = sigFunc( photonSig + pixel->currentBackground() );
      pixelLL -= pixSig;
      _ri_verbo << " -> Pixel " << pixel->key() << " " << pixSig << endmsg;
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

// float LikelihoodTool::logExpVectorClass( const float x ) const
// {

//   // Parameters
//   const auto limitA ( 0.001f );
//   const auto limitB ( 0.01f  );
//   const auto limitC ( 0.1f   );
//   const auto limitD ( 1.0f   );
  
//   // Initialise return value
//   auto res ( 0.0f );

//   // pick the interpolation to use
//   if ( x <= limitD )
//   {
//     // A collection of rational power series covering the important range
//     // note by construction this function should never be called for x < limitA
    
//     // vector with powers of x
//     const Vec4f x1( x,    x,    x,    x );
//     const Vec4f x2( 1.0f, x,    x,    x );
//     const Vec4f x3( 1.0f, 1.0f, x,    x );
//     const Vec4f x4( 1.0f, 1.0f, 1.0f, x );
//     const auto xxxx = x1 * x2 * x3 * x4;
    
//     if      ( x > limitC ) // 0.1 -> 1
//     {
//       const Vec4f topP( -101.76227310588246f, -127.31825068394369f,
//                         317.31602891172594f,  135.0567120741147f );
//       const Vec4f botP(  40.31685825976414f,   202.81583582446692f,
//                          160.94697665742055f, -2.088756969431516f );
//       res = ( ( -5.143220028201872f + horizontal_add( topP * xxxx ) ) /
//               ( 1.0f                + horizontal_add( botP * xxxx ) ) );
//     }
//     else if ( x > limitB ) // 0.01 -> 0.1
//     {
//       const Vec4f topP( -1957.455251107371f,  -63820.69082039389f,
//                         -227666.32263762745f,  431986.40491931344f );
//       const Vec4f botP(  403.96307514244734f,  20798.07111653145f,
//                          188119.3222406968f,   180043.28571064543f );
//       res = ( ( -7.4441271863249625f + horizontal_add( topP * xxxx ) ) /
//               ( 1.0f                 + horizontal_add( botP * xxxx ) ) );
//     }
//     else if ( x > limitA ) // 0.001 -> 0.01
//     {
//       const Vec4f topP( -29524.941133033215f,  -1.1896019336956682e7f,
//                         -7.860596631970513e8f, -2.795797091759987e9f );
//       const Vec4f botP(  4115.755490541176f,    2.19207068008156e6f,
//                          2.1358160394371653e8f, 2.6911720709828815e9f );
//       res = ( ( -9.760898531545386f + horizontal_add( topP * xxxx ) ) /
//               ( 1.0f                + horizontal_add( botP * xxxx ) ) );
//     }
//     else
//     {
//       // should never get here. But just in case ...
//       //res = std::log( std::exp(x) - 1.0 );
//       res = vdt::fast_logf( vdt::fast_expf(x) - 1.0f );
//     }
//   }
//   else
//   {
//     // Very very rarely called in this regime, so just use the full fat version
//     //res = std::log( std::exp(x) - 1.0 );
//     res = vdt::fast_logf( vdt::fast_expf(x) - 1.0f );
//   }
  
//   return res;
// }

//=============================================================================

// float LikelihoodTool::sigFuncDiff( const float x, const float y ) const
// {
//   // Parameters
//   const auto limitA ( 0.001f );
//   const auto limitB ( 0.01f  );
//   const auto limitC ( 0.1f   );
//   const auto limitD ( 1.0f   );

//   const Vec8f x1( x,    x,    x,    x, y,    y,    y,    y ); 
//   const Vec8f x2( 1.0f, x,    x,    x, 1.0f, y,    y,    y );
//   const Vec8f x3( 1.0f, 1.0f, x,    x, 1.0f, 1.0f, y,    y );
//   const Vec8f x4( 1.0f, 1.0f, 1.0f, x, 1.0f, 1.0f, 1.0f, y );
//   const auto xxyy = x1 * x2 * x3 * x4;

//   // Compute now the values for x and y

//   // A collection of rational power series covering the important range
//   // note by construction this function should never be called for x < limitA

//   // Initialise return value
//   auto res ( 0.0f );

//   // compute the value for x
//   if ( x < m_minSig )
//   {
//     res = m_logMinSig;
//   }
//   else if ( x <= limitD )
//   {
//     if      ( x > limitC ) // 0.1 -> 1
//     {
//       const Vec4f topP( -101.76227310588246f, -127.31825068394369f,
//                         317.31602891172594f,  135.0567120741147f );
//       const Vec4f botP(  40.31685825976414f,   202.81583582446692f,
//                          160.94697665742055f,   -2.088756969431516f );
//       res = ( ( -5.143220028201872f + horizontal_add( topP * xxyy.get_low() ) ) /
//               ( 1.0f                + horizontal_add( botP * xxyy.get_low() ) ) );
//     }
//     else if ( x > limitB ) // 0.01 -> 0.1
//     {
//       const Vec4f topP( -1957.455251107371f,   -63820.69082039389f,
//                         -227666.32263762745f,  431986.40491931344f );
//       const Vec4f botP( 403.96307514244734f,   20798.07111653145f,
//                         188119.3222406968f,    180043.28571064543f );
//       res = ( ( -7.4441271863249625f + horizontal_add( topP * xxyy.get_low() ) ) /
//               ( 1.0f                 + horizontal_add( botP * xxyy.get_low() ) ) );
//     }
//     else if ( x > limitA ) // 0.001 -> 0.01
//     {
//       const Vec4f topP( -29524.941133033215f,  -1.1896019336956682e7f,
//                         -7.860596631970513e8f, -2.795797091759987e9f );
//       const Vec4f botP( 4115.755490541176f,    2.19207068008156e6f,
//                         2.1358160394371653e8f, 2.6911720709828815e9f );
//       res = ( ( -9.760898531545386f + horizontal_add( topP * xxyy.get_low() ) ) /
//               ( 1.0f                + horizontal_add( botP * xxyy.get_low() ) ) );
//     }
//     else
//     {
//       // should never get here. But just in case ...
//       res = vdt::fast_logf( vdt::fast_expf(x) - 1.0f );
//     }
//   }
//   else
//   {
//     // Very very rarely called in this regime, so just use the full fat version
//     res = vdt::fast_logf( vdt::fast_expf(x) - 1.0f );
//   }

//   // now subtract y
//   if ( y < m_minSig )
//   {
//     res -= m_logMinSig;
//   }
//   else if ( y <= limitD )
//   {
//     if      ( y > limitC ) // 0.1 -> 1
//     {
//       const Vec4f topP( -101.76227310588246f, -127.31825068394369f,
//                         317.31602891172594f,  135.0567120741147f );
//       const Vec4f botP(  40.31685825976414f,   202.81583582446692f,
//                          160.94697665742055f,   -2.088756969431516f );
//       res -= ( ( -5.143220028201872f + horizontal_add( topP * xxyy.get_high() ) ) /
//                ( 1.0f                + horizontal_add( botP * xxyy.get_high() ) ) );
//     }
//     else if ( y > limitB ) // 0.01 -> 0.1
//     {
//       const Vec4f topP( -1957.455251107371f,   -63820.69082039389f,
//                         -227666.32263762745f,  431986.40491931344f );
//       const Vec4f botP( 403.96307514244734f,   20798.07111653145f,
//                         188119.3222406968f,    180043.28571064543f );
//       res -= ( ( -7.4441271863249625f + horizontal_add( topP * xxyy.get_high() ) ) /
//                ( 1.0f                 + horizontal_add( botP * xxyy.get_high() ) ) );
//     }
//     else if ( y > limitA ) // 0.001 -> 0.01
//     {
//       const Vec4f topP( -29524.941133033215f,  -1.1896019336956682e7f,
//                         -7.860596631970513e8f, -2.795797091759987e9f );
//       const Vec4f botP( 4115.755490541176f,    2.19207068008156e6f,
//                         2.1358160394371653e8f, 2.6911720709828815e9f );
//       res -= ( ( -9.760898531545386f + horizontal_add( topP * xxyy.get_high() ) ) /
//                ( 1.0f                + horizontal_add( botP * xxyy.get_high() ) ) );
//     }
//     else
//     {
//       // should never get here. But just in case ...
//       res -= vdt::fast_logf( vdt::fast_expf(y) - 1.0f );
//     }
//   }
//   else
//   {
//     // Very very rarely called in this regime, so just use the full fat version
//     res -= vdt::fast_logf( vdt::fast_expf(y) - 1.0f );
//   }

//   return res;
// }

//=============================================================================

// float LikelihoodTool::logExpEigen( const float x ) const
// {
//   typedef ::Eigen::Array4f V4f;

//   // Parameters
//   const auto limitA ( 0.001f );
//   const auto limitB ( 0.01f  );
//   const auto limitC ( 0.1f   );
//   const auto limitD ( 1.0f   );
  
//   // Initialise return value
//   auto res ( 0.0f );

//   // pick the interpolation to use
//   if ( x <= limitD )
//   {
//     // A collection of rational power series covering the important range
//     // note by construction this function should never be called for x < limitA
    
//     // vector with powers of x
//     const V4f x1( x,    x,    x,    x );
//     const V4f x2( 1.0f, x,    x,    x );
//     const V4f x3( 1.0f, 1.0f, x,    x );
//     const V4f x4( 1.0f, 1.0f, 1.0f, x );
//     const auto xxxx = x1 * x2 * x3 * x4;
    
//     if      ( x > limitC ) // 0.1 -> 1
//     {
//       const V4f topP( -101.76227310588246f, -127.31825068394369f,
//                        317.31602891172594f,  135.0567120741147f );
//       const V4f botP(  40.31685825976414f,   202.81583582446692f,
//                        160.94697665742055f, -2.088756969431516f );
//       res = ( ( -5.143220028201872f + ( topP * xxxx ).sum() ) /
//               ( 1.0f                + ( botP * xxxx ).sum() ) );
//     }
//     else if ( x > limitB ) // 0.01 -> 0.1
//     {
//       const V4f topP( -1957.455251107371f,   63820.69082039389f,
//                       -227666.32263762745f,  431986.40491931344f );
//       const V4f botP(  403.96307514244734f,  20798.07111653145f,
//                        188119.3222406968f,   180043.28571064543f );
//       res = ( ( -7.4441271863249625f + ( topP * xxxx ).sum() ) /
//               ( 1.0f                 + ( botP * xxxx ).sum() ) );
//     }
//     else if ( x > limitA ) // 0.001 -> 0.01
//     {
//       const V4f topP( -29524.941133033215f,  -1.1896019336956682e7f,
//                       -7.860596631970513e8f, -2.795797091759987e9f );
//       const V4f botP(  4115.755490541176f,    2.19207068008156e6f,
//                        2.1358160394371653e8f, 2.6911720709828815e9f );
//       res = ( ( -9.760898531545386f + ( topP * xxxx ).sum() ) /
//               ( 1.0f                + ( botP * xxxx ).sum() ) );
//     }
//     else
//     {
//       // should never get here. But just in case ...
//       //res = std::log( std::exp(x) - 1.0 );
//       res = vdt::fast_logf( vdt::fast_expf(x) - 1.0f );
//     }
//   }
//   else
//   {
//     // Very very rarely called in this regime, so just use the full fat version
//     //res = std::log( std::exp(x) - 1.0 );
//     res = vdt::fast_logf( vdt::fast_expf(x) - 1.0f );
//   }
  
//   return res;
// }

//=============================================================================

// float LikelihoodTool::logExpOriginal( const float x ) const
// {
  
//   // Parameters
//   const float limitA ( 0.001 );
//   const float limitB ( 0.01  );
//   const float limitC ( 0.1   );
//   const float limitD ( 1.0   );
  
//   // Initialise 
//   float res ( 0.0 );
  
//   // pick the interpolation to use
//   if ( x <= limitD )
//   {
//     // A collection of rational power series covering the important range
//     // note by construction this function should never be called for x < limitA

//     const float xx    = x*x;
//     const float xxx   = xx*x;
//     const float xxxx  = xx*xx;
//     const float xxxxx = xx*xxx;
    
//     if      ( x > limitC )
//     {
//       res = (-5.751779337152293 - 261.58791552313113*x -
//              1610.1902353909695*xx - 291.61172549536417*xxx +
//              3733.957211885683*xxxx + 1224.2104172168554*xxxxx)/
//         (1.0 + 79.52981108433892*x + 953.4570349071099*xx +
//                2638.609797400796*xxx + 1506.9612115322623*xxxx -
//          27.33558114045007*xxxxx);
//     }
//     else if ( x > limitB )
//     {
//       res = (-7.845788509794026 - 3428.7804135353526*x -
//              228752.20145929293*xx - 3.082032088759535e6*xxx -
//              3.836270197409883e6*xxxx + 1.2251900378118051e7*xxxxx)/
//         (1.0 + 638.7306815040638*x + 60430.91709817034*xx +
//          1.315432074531156e6*xxx + 6.373056770682967e6*xxxx +
//          3.3914176474223877e6*xxxxx);
//     }
//     else if ( x > limitA )
//     {
//       res = (-10.160864268729455 - 49897.23275778952*x -
//              3.855669108991894e7*xx - 6.777802095268419e9*xxx -
//              2.421987003565588e11*xxxx - 3.5610129242332263e11*xxxxx)/
//         (1.0 + 6487.897657865318*x + 6.294785881144457e6*xx +
//          1.4333658673633337e9*xxx + 7.670700007081306e10*xxxx +
//          6.06654149712832e11*xxxxx);
//     }
//     else
//     {
//       // should never get here. But just in case ...
//       //res = std::log( std::exp(x) - 1.0 );
//       res = vdt::fast_logf( vdt::fast_expf(x) - 1.0 );
//     }
//   }
//   else
//   {
//     // Very very rarely called in this regime, so just use the full fat version
//     //res = std::log( std::exp(x) - 1.0 );
//     res = vdt::fast_logf( vdt::fast_expf(x) - 1.0 );
//   }
  
//   // return
//   return res;
// }

//=============================================================================

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( LikelihoodTool )

//=============================================================================
