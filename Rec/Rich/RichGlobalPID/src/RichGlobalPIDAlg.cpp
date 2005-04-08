
//--------------------------------------------------------------------------
/** @file RichGlobalPIDAlg.cpp
 *
 *  Implementation file for RICH Global PID algorithm class : RichGlobalPIDAlg
 *
 *  CVS Log :-
 *  $Id: RichGlobalPIDAlg.cpp,v 1.22 2005-04-08 13:16:45 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   17/04/2002
 */
//--------------------------------------------------------------------------

// local
#include "RichGlobalPIDAlg.h"

//--------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichGlobalPIDAlg>          s_factory ;
const        IAlgFactory& RichGlobalPIDAlgFactory = s_factory ;

// Standard constructor, initializes variables
RichGlobalPIDAlg::RichGlobalPIDAlg( const std::string& name,
                                    ISvcLocator* pSvcLocator )
  : RichGlobalPIDAlgBase ( name, pSvcLocator ),
    m_tkSignal  ( 0 ),
    m_photonSig ( 0 )
{

  // Threshold for likelihood maximisation
  declareProperty( "LikelihoodThreshold", m_epsilon = -1e-3 );

  // Maximum number of track iterations
  declareProperty( "MaxTrackIterations", m_maxTrackIterations = 100 );

  // Minimum signal value for full calculation of log(exp(signal)-1)
  declareProperty( "MinSignalForNoLLCalc", m_minSig = 1e-50 );

  // Signal value below which to approximate log(exp(signal)-1) with log(signal)
  declareProperty( "MinSignalForAproxLLCalc", m_apxSig = 1e-5 );

  m_fP.push_back(0);
  m_fP.push_back(10);
  m_fP.push_back(0);
  m_fP.push_back(300);
  m_fP.push_back(5);
  m_fP.push_back(5);
  m_fP.push_back(5);
  m_fP.push_back(1);
  declareProperty( "FreezeOutPara", m_fP );

  m_cP.push_back(0);
  m_cP.push_back(10);
  m_cP.push_back(0);
  m_cP.push_back(300);
  m_cP.push_back(-1);
  m_cP.push_back(-1);
  m_cP.push_back(-1);
  m_cP.push_back(-0.5);
  declareProperty( "TrackMinDeltaLLPara", m_cP );

}

// Destructor
RichGlobalPIDAlg::~RichGlobalPIDAlg() {}

//  Initialize
StatusCode RichGlobalPIDAlg::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire tools
  acquireTool( "RichPhotonSignal",        m_photonSig );
  acquireTool( "RichExpectedTrackSignal", m_tkSignal  );

  // trick to force pre-loading of various tools. Avoids loading
  // during first processed event and thus biased any timing numbers
  photonCreator(); // pre-load the photon creator tool

  // Initialise parameters
  m_logMinSig = log(m_minSig);

  return sc;
}

//  Finalize
StatusCode RichGlobalPIDAlg::finalize()
{
  // Execute base class method
  return RichRecAlgBase::finalize();
}

// Main execution
StatusCode RichGlobalPIDAlg::execute()
{
  debug() << "Execute" << endreq;

  // Update RichRecEvent pointers
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // update pointers to global PID working objects
  if ( !gpidSummary() || !gpidTracks() ) return StatusCode::FAILURE;

  // how many tracks ?
  if ( m_GPIDtracks->empty() ) return StatusCode::SUCCESS;

  // Reconstruct all photons
  if ( !richPhotons() ) return StatusCode::FAILURE;
  if ( richPhotons()->empty() ) {
    photonCreator()->reconstructPhotons();
    debug() << "Reconstructed " << richPhotons()->size() << " photon candidates" << endreq;
    if ( richPhotons()->empty() ) 
      return Warning("No reconstructed photons -> Abort",StatusCode::SUCCESS);
  }

  // Compute complete likelihood for event with starting hypotheses
  m_currentBestLL = logLikelihood();
  m_GPIDSummary->addToEventLL( m_currentBestLL );

  // Initial calculation of likelihood
  if ( !initMinLogLikelihood() ) return StatusCode::FAILURE;

  const double finalLL = logLikelihood();
  debug() << "LogLL before/after initial minimisation = "
          << m_currentBestLL << "/" << finalLL << endreq;
  m_currentBestLL = finalLL;

  // iterate to minimum solution
  m_trackIteration = 0;
  minTrList minTracks;
  m_lastChance = false;
  bool tryAgain = true;
  while ( tryAgain || 0 == m_trackIteration || !minTracks.empty() ) {
    if ( m_trackIteration > m_maxTrackIterations ) {
      Warning("Taken more than max number of iterations -> quitting");
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
          err() << "Track " << (iTrack->first)->key()
                << " has been Id'ed as Unknown !!" << endreq;
        } else {
          if ( msgLevel(MSG::VERBOSE) ) {
            verbose() << "Changing Track " << (iTrack->first)->key()
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
    } else if ( !m_lastChance && m_tryAgain && 0 != m_trackIteration ) {
      m_lastChance = true;
    } else {
      tryAgain = false;
    }

    ++m_trackIteration;
  }

  debug() << "Performed " << m_trackIteration
          << " track minimisation iteration(s). Final LogL = "
          << m_currentBestLL << endreq;

  // update event LL vector after processing
  m_GPIDSummary->addToEventLL( m_currentBestLL );

  return StatusCode::SUCCESS;
}

StatusCode RichGlobalPIDAlg::initMinLogLikelihood()
{
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
      Rich::ParticleIDType hypo = static_cast<Rich::ParticleIDType>(iHypo);

      // Skip analysing starting hypothesis
      if ( hypo == rRTrack->currentHypothesis() ) continue;

      // calculate delta logLikelihood for event with new track hypothesis
      const double deltaLogL = deltaLogLikelihood( rRTrack, hypo );

      // Set the value for deltaLL for this hypothesis
      (*track)->globalPID()->setParticleDeltaLL( hypo, deltaLogL );

      // Set new minimum if lower logLikelihood is achieved
      if ( deltaLogL < mindeltaLL )  {
        if ( 0 != deltaLogL ) mindeltaLL = deltaLogL;
        if ( deltaLogL < m_epsilon ) minHypo = hypo;
      }

      // In case the threshold is reached, skip other hypotheses
      bool threshold = true;
      for ( int ihypo = iHypo; ihypo < Rich::NParticleTypes; ++ihypo ) {
        if ( m_tkSignal->nDetectablePhotons( rRTrack,
                                             static_cast<Rich::ParticleIDType>(ihypo) ) > 0 ) {
          threshold = false; break;
        }
      }
      if ( threshold ) {
        for ( int ihypo = iHypo; ihypo < Rich::NParticleTypes; ++ihypo ) {
          (*track)->globalPID()->setParticleDeltaLL( static_cast<Rich::ParticleIDType>(ihypo),
                                                     deltaLogL );
        }
        break;
      }

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
      verbose() << "initMinLogLikelihood : Track "
                << format("%4i",(*track)->key())
                << " prefers hypothesis " <<  *hypo << " to "
                << (*track)->richRecTrack()->currentHypothesis()
                << ". DLL = " << *dll << endreq ;
    }
    // set best hypothesis
    (*track)->richRecTrack()->setCurrentHypothesis( *hypo );
    // set delta LL to zero for best hypothesis
    (*track)->globalPID()->setParticleDeltaLL( *hypo, 0 );
    // iterative counters
    ++hypo; ++dll;
  }

  return StatusCode::SUCCESS;
}

void RichGlobalPIDAlg::findMinLogLikelihood( minTrList & minTracks )
{

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
  RichGlobalPIDTrack * minTrack = NULL;
  Rich::ParticleIDType minHypo = Rich::Unknown;
  double minDLLs = 0;
  double minEventDll = 999999;
  updateFreezeOutValue();
  updateDllThres();

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
    if ( (*iP).first > freezeOutValue() ) break;

    bool addto = false;
    bool minFound = false;
    double minTrackDll = 999999;
    for ( int iHypo = 0; iHypo < Rich::NParticleTypes; ++iHypo ) {
      Rich::ParticleIDType hypothesis = static_cast<Rich::ParticleIDType>(iHypo);

      // Skip analysing starting hpyothesis
      if ( hypothesis == rRTrack->currentHypothesis() ) continue;

      // calculate delta logLikelihood for event with new track hypothesis
      const double deltaLogL = deltaLogLikelihood( rRTrack, hypothesis );

      // Set the value for deltaLL for this hypothesis
      gTrack->globalPID()->setParticleDeltaLL( hypothesis, deltaLogL );

      // Set new minimums if lower logLikelihood
      if ( deltaLogL < m_epsilon && deltaLogL < minEventDll ) {
        if ( msgLevel(MSG::VERBOSE) ) {
          verbose() << "findMinLogLikelihood : Track "
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
        if ( deltaLogL < dllThres() ) addto = true;
      }
      if ( deltaLogL != 0 && deltaLogL<minTrackDll ) minTrackDll = deltaLogL;

      // If threshold is reached, set the deltaLL for all other hypotheses
      bool threshold = true;
      for ( int hypo = iHypo; hypo < Rich::NParticleTypes; ++hypo ) {
        if ( m_tkSignal->nDetectablePhotons( gTrack->richRecTrack(),
                                             static_cast<Rich::ParticleIDType>(hypo) ) > 0 ) {
          threshold = false; break;
        }
      }
      if ( threshold ) {
        for ( int hypo = iHypo; hypo < Rich::NParticleTypes; ++hypo ) {
          gTrack->globalPID()->setParticleDeltaLL( static_cast<Rich::ParticleIDType>(hypo),
                                                   deltaLogL );
        }
        break;
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

double
RichGlobalPIDAlg::deltaLogLikelihood( RichRecTrack * track,
                                      const Rich::ParticleIDType newHypo )
{

  // Change due to track expectation
  double deltaLL = m_tkSignal->nTotalObservablePhotons( track, newHypo ) -
    ( track->nObservableSignalPhotons(track->currentHypothesis()) +
      track->nObservableScatteredPhotons(track->currentHypothesis()) );

  // Photon part
  RichRecTrack::Pixels & pixels = track->richRecPixels();
  for ( RichRecTrack::Pixels::iterator iPixel = pixels.begin();
        iPixel != pixels.end(); ++iPixel ) {

    // photons for this pixel
    RichRecPixel::Photons & photons = (*iPixel)->richRecPhotons();
    if ( !photons.empty() ) {

      double oldSig = (*iPixel)->currentBackground();
      double newSig = oldSig;

      // cache end point since container size will not change in proceeding loop
      const RichRecPixel::Photons::iterator iPhotonEnd = photons.end();
      for ( RichRecPixel::Photons::const_iterator iPhoton = photons.begin();
            iPhoton != iPhotonEnd; ++iPhoton ) {
        RichRecPhoton * photon = *iPhoton;

        if ( !photon->richRecTrack()->inUse() ) continue;

        const double tmpOldSig =
          photon->expPixelSignalPhots( photon->richRecTrack()->currentHypothesis() );
        oldSig += tmpOldSig;
        newSig += ( photon->richRecTrack() != track ? tmpOldSig :
                    m_photonSig->predictedPixelSignal( photon, newHypo ) );

      } // end photon loop

      deltaLL -= sigFunc(newSig) - sigFunc(oldSig);

    } // end photons not empty

  } // end pixel loop

  return deltaLL;
}

double RichGlobalPIDAlg::logLikelihood()
{

  // Loop over tracks to form total expected hits part of LL
  double trackLL = 0.0;
  for ( RichGlobalPIDTracks::iterator track = m_GPIDtracks->begin();
        track != m_GPIDtracks->end();
        ++track ) {
    // Sum expected photons from each track with current assumed hypotheses
    RichRecTrack * rRTrack = (*track)->richRecTrack();
    trackLL +=
      m_tkSignal->nTotalObservablePhotons( rRTrack,
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
    RichRecPixel::Photons & photons = pixel->richRecPhotons();
    for ( RichRecPixel::Photons::iterator iPhoton = photons.begin();
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

  return (trackLL+pixelLL);
}

// Please don't look at these following methods ....

void RichGlobalPIDAlg::updateDllThres()
{
  double m(0), c(0), A(0), B(0);
  if ( m_GPIDtracks->size() < m_cP[2] ) {
    A = m_cP[4];
  } else if ( m_GPIDtracks->size() > m_cP[3] ) {
    A = m_cP[5];
  } else {
    m = (m_cP[4]-m_cP[6])/(m_cP[2]-m_cP[3]);
    c = (m_cP[2]*m_cP[6]-m_cP[4]*m_cP[3])/(m_cP[2]-m_cP[3]);
    A = m*m_GPIDtracks->size() + c;
  }
  if ( m_GPIDtracks->size() < m_cP[2] ) {
    B = m_cP[6];
  } else if ( m_GPIDtracks->size() > m_cP[3] ) {
    B = m_cP[7];
  } else {
    m = (m_cP[5]-m_cP[7])/(m_cP[2]-m_cP[3]);
    c = (m_cP[2]*m_cP[7]-m_cP[5]*m_cP[3])/(m_cP[2]-m_cP[3]);
    B = m*m_GPIDtracks->size() + c;
  }
  if ( m_trackIteration < m_cP[0] ) {
    m_currentDllTreshold = A;
  } else if ( m_trackIteration > m_cP[1] ) {
    m_currentDllTreshold = B;
  } else {
    m = (A-B)/(m_cP[0]-m_cP[1]);
    c = (m_cP[0]*B-A*m_cP[1])/(m_cP[0]-m_cP[1]);
    m_currentDllTreshold = m*m_trackIteration + c;
  }
  verbose() << "New update DLL threshold " << m_currentDllTreshold  << endreq;
}

void RichGlobalPIDAlg::updateFreezeOutValue()
{
  double m(0), c(0), A(0), B(0);
  if ( m_GPIDtracks->size() < m_fP[2] ) {
    A = m_fP[4];
  } else if ( m_GPIDtracks->size() > m_fP[3] ) {
    A = m_fP[5];
  } else {
    m = (m_fP[4]-m_fP[6])/(m_fP[2]-m_fP[3]);
    c = (m_fP[2]*m_fP[6]-m_fP[4]*m_fP[3])/(m_fP[2]-m_fP[3]);
    A = m*m_GPIDtracks->size() + c;
  }
  if ( m_GPIDtracks->size() < m_fP[2] ) {
    B = m_fP[6];
  } else if ( m_GPIDtracks->size() > m_fP[3] ) {
    B = m_fP[7];
  } else {
    m = (m_fP[5]-m_fP[7])/(m_fP[2]-m_fP[3]);
    c = (m_fP[2]*m_fP[7]-m_fP[5]*m_fP[3])/(m_fP[2]-m_fP[3]);
    B = m*m_GPIDtracks->size() + c;
  }
  if ( m_trackIteration < m_fP[0] ) {
    m_currentFreezeOutValue = A;
  } else if ( m_trackIteration > m_fP[1] ) {
    m_currentFreezeOutValue = B;
  } else {
    m = (A-B)/(m_fP[0]-m_fP[1]);
    c = (m_fP[0]*B-A*m_fP[1])/(m_fP[0]-m_fP[1]);
    m_currentFreezeOutValue = m*m_trackIteration + c;
  }
  verbose() << "New freeze out value " << m_currentFreezeOutValue << endreq;
}
