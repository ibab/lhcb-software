// $Id: RichGlobalPIDAlg.cpp,v 1.7 2003-07-08 11:39:53 cattanem Exp $
// Include files

// local
#include "RichGlobalPIDAlg.h"

//--------------------------------------------------------------------------
// Implementation file for class : RichGlobalPIDAlg
//
// 017/04/2002 : Chris Jones   Christopher.Rob.Jones@cern.ch
//--------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichGlobalPIDAlg>          s_factory ;
const        IAlgFactory& RichGlobalPIDAlgFactory = s_factory ;

// Standard constructor, initializes variables
RichGlobalPIDAlg::RichGlobalPIDAlg( const std::string& name,
                                    ISvcLocator* pSvcLocator )
  : RichGlobalPIDAlgBase ( name, pSvcLocator ) {

  // Threshold for likelihood maximisation
  declareProperty( "LikelihoodThreshold", m_epsilon = 1e-3 );

  // Maximum number of track iterations
  declareProperty( "MaxTrackIterations", m_maxTrackIterations = 100 );

  // Threshold for forced hypothesis change in track minimsation
  declareProperty( "TrackMinDeltaLLThreshold", m_trDeltaThres = -3 );

  // Threshold for freezing out tracks from minimisation iteration
  declareProperty( "TrackFreezeOutThreshold", m_freezeOutDll = 50 );

  // Minimum signal value for full calculation of log(exp(signal)-1)
  declareProperty( "MinSignalForNoLLCalc", m_minSig = 1e-50 );

  // Signal value below which to approximate  log(exp(signal)-1) with log(signal)
  declareProperty( "MinSignalForAproxLLCalc", m_apxSig = 1e-5 );

}

// Destructor
RichGlobalPIDAlg::~RichGlobalPIDAlg() {}

//  Initialize
StatusCode RichGlobalPIDAlg::initialize() {

  MsgStream msg(msgSvc(), name());

  // Sets up various tools and services
  if ( !RichRecAlgBase::initialize() ) return StatusCode::FAILURE;

  // Acquire tools
  acquireTool( "RichPhotonCreator", m_photonCr );
  acquireTool( "RichPhotonSignal", m_photonSig );
  acquireTool( "RichTrackProperties", m_trackProp );

  // Initialise parameters
  m_logMinSig = log(m_minSig);

  msg << MSG::DEBUG << "Initialize" << endreq
      << " Max Track Iterations         = " << m_maxTrackIterations << endreq
      << " Forced change deltaLL        = " << m_trDeltaThres << endreq
      << " Track freeze-out deltaLL     = " << m_freezeOutDll << endreq
      << " Min deltaLL for update       = " << m_epsilon << endreq
      << " DeltaLL signal speed params  = " << m_apxSig << " " << m_minSig << endreq;

  return StatusCode::SUCCESS;
}

// Main execution
StatusCode RichGlobalPIDAlg::execute() {

  MsgStream  msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Execute" << endreq;

  // Update RichRecEvent pointers
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // update pointers to global PID working objects
  if ( !gpidSummary() || !gpidTracks() ) return StatusCode::FAILURE;

  // Reconstruct all photons
  if ( !richPhotons() ) return StatusCode::FAILURE;
  if ( richPhotons()->empty() ) {
    m_photonCr->reconstructPhotons();
    if ( msgLevel(MSG::DEBUG) ) {
      msg << MSG::DEBUG << "Reconstructed " << richPhotons()->size()
          << " photon candidates" << endreq;
    }
  }

  // Compute complete likelihood for event with starting hypotheses
  m_currentBestLL = logLikelihood();
  m_GPIDSummary->addToEventLL( m_currentBestLL );

  // Initial calculation of likelihood
  if ( !initMinLogLikelihood() ) return StatusCode::FAILURE;

  // iterate to minimum solution
  m_trackIteration = 0;
  minTrList minTracks;
  m_lastChance = false;
  bool tryAgain = true;
  while ( tryAgain || 0 == m_trackIteration || !minTracks.empty() ) {
    if ( m_trackIteration > m_maxTrackIterations ) {
      msg << MSG::WARNING << "Taken more than " << m_maxTrackIterations
          << " iterations, quitting." << endreq;
      break;
    }

    // Iterate finding the min likelihood
    findMinLogLikelihood( minTracks );

    // set track hypotheses to the minimum
    if ( !minTracks.empty() ) {
      m_lastChance = false;
      minTrList::iterator iTrack;
      for ( iTrack = minTracks.begin(); iTrack != minTracks.end(); ++iTrack ) {
        if ( Rich::Unknown == iTrack->second ) {
          msg << MSG::ERROR << "Track " << (iTrack->first)->key()
              << " has been Id'ed as Unknown !!" << endreq;
        } else {
          if ( msgLevel(MSG::VERBOSE) ) {
            msg << MSG::VERBOSE << "Changing Track " << (iTrack->first)->key()
                << " hypothesis to from "
                << (iTrack->first)->richRecTrack()->currentHypothesis()
                << " to " << iTrack->second << ". LogL = " << m_currentBestLL
                << endreq;
          }
          (iTrack->first)->richRecTrack()->setCurrentHypothesis( iTrack->second );
        }
      }
    } else if ( !m_lastChance && m_tryAgain && 0 != m_trackIteration ) {
      m_lastChance = true;
    } else {
      tryAgain = false;
    }

    ++m_trackIteration;
  }

  if ( msgLevel(MSG::DEBUG) ) {
    msg << MSG::DEBUG << "Performed " << m_trackIteration
        << " track minimisation iteration(s). Final LogL = "
        << m_currentBestLL << endreq;
  }

  // update event LL vector after processing
  m_GPIDSummary->addToEventLL( m_currentBestLL );

  return StatusCode::SUCCESS;
}

StatusCode RichGlobalPIDAlg::initMinLogLikelihood() {

  RichGlobalPIDTrackVector minTrack;
  Rich::ParticleIDTypeVector minTrackHypo;
  std::vector<double> minDLL;
  m_trackList.clear();

  // Loop over all tracks and find best hypothesis for each track
  {for ( RichGlobalPIDTracks::iterator track = m_GPIDtracks->begin();
         track != m_GPIDtracks->end();
         ++track ) {
    RichRecTrack * rRTrack = (*track)->richRecTrack();

    // Initialise starting values
    double mindeltaLL = 99999.;
    Rich::ParticleIDType minHypo = rRTrack->currentHypothesis();

    // Loop over all particle codes
    for ( int iHypo = 0; iHypo < Rich::NParticleTypes; ++iHypo ) {
      Rich::ParticleIDType hypo = (Rich::ParticleIDType)iHypo;

      // Skip analysing starting hypothesis
      if ( hypo == rRTrack->currentHypothesis() ) continue;

      // calculate delta logLikelihood for event with new track hypothesis
      double deltaLogL = deltaLogLikelihood( rRTrack, hypo );

      // Set the value for deltaLL for this hypothesis
      (*track)->globalPID()->setParticleDeltaLL( hypo, deltaLogL );

      // Set new minimum if lower logLikelihood is achieved
      if ( deltaLogL < mindeltaLL )  {
        if ( 0 != deltaLogL ) mindeltaLL = deltaLogL;
        if ( deltaLogL+m_epsilon < 0.0 ) minHypo = hypo;
      }

      // In case the threshold is reached, skip other hypotheses
      bool threshold = true;
      for ( int ihypo = (int)hypo; ihypo < Rich::NParticleTypes; ++ihypo ) {
        if ( m_trackProp->nDetectablePhotons( rRTrack,
                                              (Rich::ParticleIDType)ihypo ) > 0 ) {
          threshold = false;
          break;
        }
      }
      if ( threshold ) break;

    } // end particle for loop

    // Save info on tracks that have a better minimum hypothesis
    if ( minHypo != rRTrack->currentHypothesis() ) {
      minTrack.push_back( *track );
      minTrackHypo.push_back( minHypo );
      minDLL.push_back( mindeltaLL );
    }

    // Add this track / deltaLL to the track list
    m_trackList.push_back( TrackPair(mindeltaLL, *track) );

  }} // end track for loop

  // Finally, set all track hypotheses to their minimum
  Rich::ParticleIDTypeVector::const_iterator hypo = minTrackHypo.begin();
  std::vector<double>::const_iterator dll = minDLL.begin();
  for ( RichGlobalPIDTrackVector::iterator track = minTrack.begin();
        track != minTrack.end();
        ++track ) {
    if ( msgLevel(MSG::VERBOSE) ) {
      MsgStream msg( msgSvc(), name() );
      msg << MSG::VERBOSE << "initMinLogLikelihood : Track "
          << format("%4i",(*track)->key())
          << " prefers hypothesis " <<  *hypo << " to "
          << (*track)->richRecTrack()->currentHypothesis()
          << ". DLL = " << *dll << endreq ;
    }
    (*track)->richRecTrack()->setCurrentHypothesis( *hypo );
    ++hypo;
    ++dll;
  }

  double finalLL = logLikelihood();
  if ( msgLevel(MSG::DEBUG) ) {
    MsgStream msg( msgSvc(), name() );
    msg << MSG::DEBUG << "LogLL before/after initial minimisation = "
        << m_currentBestLL << "/" << finalLL << endreq;
  }
  m_currentBestLL = finalLL;

  return StatusCode::SUCCESS;
}

void RichGlobalPIDAlg::findMinLogLikelihood( minTrList & minTracks ) {

  // which RICHes to look at ?
  if ( !minTracks.empty() ) {
    m_inR1 = false; m_inR2 = false;
    for ( minTrList::iterator iT = minTracks.begin();
          iT != minTracks.end(); ++iT ) {
      RichRecTrack * rTk = ((*iT).first)->richRecTrack();
      if ( rTk->inRICH1() ) m_inR1 = true;
      if ( rTk->inRICH2() ) m_inR2 = true;
    }
  } else {
    if ( m_lastChance ) {
      m_inR1 = !m_inR1; m_inR2 = !m_inR2;
    } else {
      m_inR1 = true; m_inR2 = true;
    }
  }
  m_tryAgain = !( m_inR1 == m_inR2 );

  // Initialise
  minTracks.clear();
  RichGlobalPIDTrack* minTrack = NULL;
  Rich::ParticleIDType minHypo = Rich::Unknown;
  double minDLLs = 0;
  double minEventDll = 99999.;

  // sort Track list according to delta LL
  std::sort( m_trackList.begin(), m_trackList.end() );

  // Loop on all tracks
  double * pCh = 0;
  for ( TrackList::iterator iP = m_trackList.begin();
        iP != m_trackList.end(); ++iP ) {
    RichGlobalPIDTrack * gTrack = (*iP).second;
    RichRecTrack * rRTrack = gTrack->richRecTrack();

    // skip tracks that do not share a RICH with the last changed tracks
    if ( !( (m_inR1 && rRTrack->inRICH1()) ||
            (m_inR2 && rRTrack->inRICH2()) ) ) continue;

    // skip frozen tracks
    //if ( m_trackIteration != 0 && (*iP).first > m_freezeOutDll ) break;
    if ( (*iP).first > m_freezeOutDll ) break;

    // Set best hypothesis deltaLL to zero
    gTrack->globalPID()->setParticleDeltaLL( rRTrack->currentHypothesis(),
                                             0.0 );

    bool addto = false;
    bool minFound = false;
    double minTrackDll(999999);
    for ( int iHypo = 0; iHypo < Rich::NParticleTypes; ++iHypo ) {
      Rich::ParticleIDType hypothesis = (Rich::ParticleIDType)iHypo;

      // Skip analysing starting hpyothesis
      if ( hypothesis == rRTrack->currentHypothesis() ) continue;

      // calculate delta logLikelihood for event with new track hypothesis
      double deltaLogL = deltaLogLikelihood( rRTrack, hypothesis );

      // Set the value for deltaLL for this hypothesis
      gTrack->globalPID()->setParticleDeltaLL( hypothesis, deltaLogL );

      // Set new minimums if lower logLikelihood
      if ( deltaLogL+m_epsilon < 0.0 && deltaLogL < minEventDll ) {
        if ( msgLevel(MSG::VERBOSE) ) {
          MsgStream msg( msgSvc(), name() );
          msg << MSG::VERBOSE << "findMinLogLikelihood : Track "
              << format("%4i",gTrack->key()) << " prefers hypothesis "
              << hypothesis << " to " << rRTrack->currentHypothesis()
              << ". DLL = " << deltaLogL << endreq ;
        }
        minFound = true;
        minTrack = gTrack;
        minHypo = hypothesis;
        minEventDll = deltaLogL;
        (*iP).first = deltaLogL;
        pCh = &((*iP).first);
        if ( deltaLogL < m_trDeltaThres ) addto = true;
      }
      if ( deltaLogL<minTrackDll ) minTrackDll = deltaLogL;

      // If threshold is reached, set the deltaLL for all other hypotheses
      if ( (int)hypothesis < Rich::NParticleTypes) {
        bool threshold = true;
        for ( int hypo = (int)hypothesis; hypo < Rich::NParticleTypes; ++hypo ) {
          if ( m_trackProp->nDetectablePhotons( gTrack->richRecTrack(),
                                                (Rich::ParticleIDType)hypo ) > 0 ) {
            threshold = false;
            break;
          }
        }
        if ( threshold ) {
          for ( int hypo = (int)hypothesis; hypo < Rich::NParticleTypes; ++hypo ) {
            gTrack->globalPID()->setParticleDeltaLL( (Rich::ParticleIDType)hypo, deltaLogL );
          }
          break;
        }
      }

    } // end hypothesis loop
    if ( !minFound ) (*iP).first = minTrackDll;

    // if found tracks with good enough deltaLL, add to force-change map
    if ( addto ) {
      minTracks[minTrack] = minHypo;
      minDLLs += minEventDll;
      (*iP).first = 0;
      break;
    }

  } // end track loop

  // update final information
  if ( minTrack ) {
    if ( minTracks.empty() ) {
      minTracks[minTrack] = minHypo;
      minDLLs = minEventDll;
      if (pCh) *pCh = 0;
    }
    m_currentBestLL += minDLLs;
  }

}

double RichGlobalPIDAlg::deltaLogLikelihood( RichRecTrack * track,
                                             const Rich::ParticleIDType newHypo ) {

  // Change due to track expectation
  double deltaLL =
    m_trackProp->nTotalObservablePhotons( track, newHypo ) -
    ( track->nObservableSignalPhotons(track->currentHypothesis()) +
      track->nObservableScatteredPhotons(track->currentHypothesis()) );

  // Photon part
  SmartRefVector<RichRecPixel> & pixels = track->richRecPixels();
  for ( SmartRefVector<RichRecPixel>::iterator iPixel = pixels.begin();
        iPixel != pixels.end();
        ++iPixel ) {
    RichRecPixel * pixel = *iPixel;

    double oldSig( pixel->currentBackground() );
    double newSig( oldSig );
    SmartRefVector<RichRecPhoton> & photons = pixel->richRecPhotons();
    for ( SmartRefVector<RichRecPhoton>::iterator iPhoton = photons.begin();
          iPhoton != photons.end();
          ++iPhoton ) {
      RichRecPhoton * photon = *iPhoton;

      RichRecTrack * pixTrack = photon->richRecTrack();
      if ( !pixTrack->inUse() ) continue;

      double tmpOldSig = photon->expPixelSignalPhots( pixTrack->currentHypothesis() );
      oldSig += tmpOldSig;
      newSig += ( pixTrack != track ? tmpOldSig :
                  m_photonSig->predictedPixelSignal( photon, newHypo ) );

    } // end photon loop

    deltaLL -= sigFunc(newSig) - sigFunc(oldSig);

  } // end pixel loop

  return deltaLL;
}

double RichGlobalPIDAlg::logLikelihood() {

  // Loop over tracks to form total expected hits part of LL
  double trackLL = 0.0;
  for ( RichGlobalPIDTracks::iterator track = m_GPIDtracks->begin();
        track != m_GPIDtracks->end();
        ++track ) {

    // Sum expected photons from each track with current assumed hypotheses
    RichRecTrack * rRTrack = (*track)->richRecTrack();
    trackLL +=
      m_trackProp->nTotalObservablePhotons( rRTrack,
                                            rRTrack->currentHypothesis() );

  } // end track loop

  // Add background terms for expectation
  trackLL += ( richStatus()->detOverallBkg( Rich::Rich1 ) +
               richStatus()->detOverallBkg( Rich::Rich2 ) );

  // Pixel loop
  double pixelLL = 0.0;
  for ( RichRecPixels::iterator iPixel = richPixels()->begin();
        iPixel != richPixels()->end();
        ++iPixel ) {
    RichRecPixel * pixel = *iPixel;

    double photonSig = 0.0;
    SmartRefVector<RichRecPhoton> & photons = pixel->richRecPhotons();
    for ( SmartRefVector<RichRecPhoton>::iterator iPhoton = photons.begin();
          iPhoton != photons.end();
          ++iPhoton ) {
      RichRecPhoton * photon = *iPhoton;

      RichRecTrack * rRTrack = photon->richRecTrack();
      if ( !rRTrack->inUse() ) continue;
      photonSig += m_photonSig->predictedPixelSignal( photon,
                                                      rRTrack->currentHypothesis() );
    }

    pixelLL -= sigFunc( photonSig + pixel->currentBackground() );

  }

  return trackLL+pixelLL;
}

//  Finalize
StatusCode RichGlobalPIDAlg::finalize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "Finalize" << endreq;

  // release tools
  releaseTool( m_photonCr );
  releaseTool( m_photonSig );
  releaseTool( m_trackProp );

  // Execute base class method
  return RichRecAlgBase::finalize();
}
