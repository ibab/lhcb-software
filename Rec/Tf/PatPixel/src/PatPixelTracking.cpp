// Gaudi
#include "GaudiKernel/AlgFactory.h"
// LHCb
#include "Event/Track.h"
#include "Event/StateParameters.h"
// Local
#include "PatPixelTracking.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatPixelTracking
//
// 2011-12-16 : Olivier Callot
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY(PatPixelTracking)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatPixelTracking::PatPixelTracking(const std::string& name,
                                   ISvcLocator* pSvcLocator) :
#ifdef DEBUG_HISTO
    GaudiTupleAlg(name, pSvcLocator),
#else
    GaudiAlgorithm(name, pSvcLocator),
#endif
    m_debugTool(NULL) { 

  declareProperty("OutputTracksName", m_outputLocation = LHCb::TrackLocation::Velo);
  declareProperty("MaxXSlope", m_maxXSlope = 0.400);                     // 0.400
  declareProperty("MaxYSlope", m_maxYSlope = 0.400);                     // 0.300
  declareProperty("ExtraTol", m_extraTol = 1.000 * Gaudi::Units::mm);  // 0.150 // (initial) extrapolation tolerance when adding hits
  declareProperty("MaxMissed", m_maxMissed = 4);  // stops the extrapolation after that many sensors
  declareProperty("ClearHits", m_clearHits = false);  
  declareProperty("MaxScatter", m_maxScatter = 0.004);

  // Parameters for debugging
  declareProperty("DebugToolName", m_debugToolName = "");
  declareProperty("WantedKey", m_wantedKey = -100);
  declareProperty("TimingMeasurement", m_doTiming = false);

  // Parameters for Kalman fit
  declareProperty("ClosestToBeamStateKalmanFit", m_stateClosestToBeamKalmanFit = true);
  declareProperty("EndVeloStateKalmanFit", m_stateEndVeloKalmanFit = false);
  declareProperty("AddFirstLastMeasurementStatesKalmanFit", m_addStateFirstLastMeasurementKalmanFit = false);

  // obsolete parameters - only kept in for backwards compatibility
  declareProperty("MaxZForRBeamCut",       m_maxZForRBeamCut   = 200.0 * Gaudi::Units::mm );  // 
  declareProperty("MaxR2Beam",             m_maxR2Beam         =   1.0 * Gaudi::Units::mm2 ); // 1.0 // limit on squared track distance to the beam axis
  declareProperty("MaxChi2PerHit",         m_maxChi2PerHit     = 25.0  );  // 16.0 // limit when removing worst hits
  declareProperty("MaxChi2Short",          m_maxChi2Short      = 20.0  );  //  6.0 // short (3-hit) tracks are removed with this limit
  declareProperty("MaxChi2ToAdd",          m_maxChi2ToAdd      = 200.0 );  // 100.0 // (initial) limit on track chi2 when adding new hits
  declareProperty("MaxChi2SameSensor",     m_maxChi2SameSensor = 16.0  );  // 16.0 // limit when adding hits from sensor that already has hits ?

}

//=============================================================================
// Destructor
//=============================================================================
PatPixelTracking::~PatPixelTracking() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode PatPixelTracking::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;
  m_isDebug = msgLevel(MSG::DEBUG);
  // Setup the hit manager.
  m_hitManager = tool<PatPixelHitManager>("PatPixelHitManager");
  // Setup the debug tool.
  if ("" != m_debugToolName) m_debugTool = tool<IPatDebugTool>(m_debugToolName);
  // Setup the timing measurement.
  if (m_doTiming) {
    m_timerTool = tool<ISequencerTimerTool>("SequencerTimerTool/Timer", this);
    m_timeTotal = m_timerTool->addTimer("Total");
    m_timerTool->increaseIndent();
    m_timePrepare = m_timerTool->addTimer("Prepare");
    m_timePairs = m_timerTool->addTimer("Find by pairs");
    m_timeFinal = m_timerTool->addTimer("Store tracks");
    m_timerTool->decreaseIndent();
  }
#ifdef DEBUG_HISTO
  setHistoTopDir("VP/");
#endif
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PatPixelTracking::execute() {

  // Build hits from clusters and sort them by global x within each module. 
  if (m_doTiming) {
    m_timerTool->start(m_timeTotal);
    m_timerTool->start(m_timePrepare);
  }
  if (m_clearHits) m_hitManager->clearHits();
  m_hitManager->buildHits();
  m_hitManager->sortByX();

  if (m_isDebug) {
    for (unsigned int i = m_hitManager->firstModule(); m_hitManager->lastModule() >= i; ++i) {
      PatPixelHits::const_iterator ith;
      for (ith = m_hitManager->hits(i).begin(); m_hitManager->hits(i).end() != ith; ++ith) {
        printHit(*ith);
      }
    }
  }
  if (0 <= m_wantedKey) {
    info() << "--- Looking for track " << m_wantedKey << endmsg;
    for (unsigned int i = m_hitManager->firstModule(); m_hitManager->lastModule() >= i; ++i) {
      PatPixelHits::const_iterator ith;
      for (ith = m_hitManager->hits(i).begin(); m_hitManager->hits(i).end() != ith; ++ith) {
        if (matchKey(*ith)) printHit(*ith);
      }
    }
  }

  if (m_doTiming) m_timerTool->stop(m_timePrepare);

  // Search for tracks by finding a pair of hits, then extrapolating.
  if (m_doTiming) m_timerTool->start(m_timePairs);
  searchByPair();                                                    
  if (m_doTiming) m_timerTool->stop(m_timePairs);

  // Convert temporary tracks to LHCb tracks.
  if (m_doTiming) m_timerTool->start(m_timeFinal);
  makeLHCbTracks();
  if (m_doTiming) {
    m_timerTool->stop(m_timeFinal);
    m_timerTool->stop(m_timeTotal);
  }

#ifdef DEBUG_HISTO
  for (unsigned int i = m_hitManager->firstModule(); i < m_hitManager->lastModule(); ++i) {
    PatPixelHits::iterator ith;
    for (ith = m_hitManager->hits(i).begin(); ith != m_hitManager->hits(i).end(); ++ith) {
      PatPixelHit* iC = *ith;
      const double x = iC->x();
      const double y = iC->y();
      const double z = iC->z();
      const double r = sqrt(x * x + y * y);
      if (!iC->isUsed()) {
        plot3D(x, y, z, "UnusedHits3D", "Distribution of UnusedHits",-50.0,50.0,-50.0,50.0,-500.0,800.0,100,100,200);
        plot2D(r, z, "UnusedHits_rz", "Distribution of Unused Hits", 0.0, 60.0,-500.0,800.0,100,100);
        plot2D(x, y, "UnusedHits2D", "Distribution of Unused Hits",-50.0,50.0,-50.0,50.0,100,100);
      }
      plot3D(x, y, z, "Hits_3D", "3D Distribution of Hits",-50.0,50.0,-50.0,50.0,-500.0,800.0,100,100,200);
      plot2D(r, z, "Hits_RZ", "RZ Distribution of Hits", 0.0, 60.0,-500.0,800.0,100,100);
      plot2D(x, y, "Hits_2D", "2D Distribution of Hits",-50.0,50.0,-50.0,50.0,100,100);
    }
  }

  const unsigned int nbHits = m_hitManager->nbHits();
  plot(nbHits, "HitsPerEvent", "Number of hits per event", 0.0, 8000.0, 80);
  plot(m_tracks.size(), "TracksPerEvent", "Number of tracks per event", 0.0,  800.0, 80);
  if (nbHits > 0) {
    const unsigned int nbHitsUsed = m_hitManager->nbHitsUsed();
    plot((100.0*nbHitsUsed)/nbHits, "PercentUsedHitsPerEvent", "Percent of hits assigned to tracks", 0.0, 100.0, 100);
  }
  for (PatPixelTracks::const_iterator itT = m_tracks.begin(); itT != m_tracks.end(); ++itT) {
    if ((*itT).size() <= 3) continue;
    // Calculate radius at first and last hit (assume that hits are sorted by sensor)
    const double x1 = (*itT).hits.front()->x();
    const double y1 = (*itT).hits.front()->y();
    const double r1 = sqrt(x1 * x1 + y1 * y1);
    const double x2 = (*itT).hits.back()->x();
    const double y2 = (*itT).hits.back()->y();
    const double r2 = sqrt(x2 * x2 + y2 * y2);
    const double minR = r1 > r2 ? r2 : r1;
    const double maxR = r1 > r2 ? r1 : r2;
    plot(minR, "MinHitRadiusPerTrack", "Smallest hit radius [mm] per track (of 4 or more hits)", 0.0, 50.0, 100);
    plot(maxR, "MaxHitRadiusPerTrack", "Largest hit radius [mm] per track (of 4 or more hits)",  0.0, 50.0, 100);
  }
#endif

  return StatusCode::SUCCESS;

}

//=============================================================================
// Extend downstream, on both sides of the detector as soon as one hit is missed
//=============================================================================
void PatPixelTracking::trackDownstream(const PatPixelHit* h1, const PatPixelHit* h2) {

  // Initially scan every second module (stay on the same side).
  int extraStep = 2;
  // Start two sensors behind the last one.
  int extraSensIdx = m_track.lastModule() - extraStep; 
  // Count sensors without hits found.
  unsigned int nbMissed = 0;
  while (extraSensIdx >= 0) {
    PatPixelSensor* extra = m_hitManager->sensor(extraSensIdx); 
    // Attempt to add new hits from this sensor with given tolerances
    PatPixelHit* h3 = bestHit(extra, m_extraTol, m_maxScatter, h1, h2); 
    if (h3) {
      m_track.addHit(h3);
      // Reset missed hit counter.
      nbMissed = 0;
      h1 = h2;
      h2 = h3;
    } else {
      // No hits found.
      if (extraStep == 2) {
        // Look on the other side.
        extra = m_hitManager->sensor(extraSensIdx + 1);
        h3 = bestHit(extra, m_extraTol, m_maxScatter, h1, h2);
        if (!h3) {
          nbMissed += extraStep;
        } else {
          m_track.addHit(h3);
          h1 = h2;
          h2 = h3;
        }
        // Switch to scanning every module (left and right). 
        extraStep = 1; 
      } else {
        ++nbMissed;
      }
    }
    if (m_maxMissed < nbMissed) break;
    extraSensIdx -= extraStep;
  }

}

/*
void PatPixelTracking::trackUpstream() {
 
  int extraStep = 1;
  int extraSensIdx = m_track.lastModule() + extraStep;
  unsigned int nbMissed = 0;
  while (extraSensIdx <= (int)(m_hitManager->lastModule())) {
    PatPixelSensor* extra = m_hitManager->sensor(extraSensIdx);
    bool added = addHits( extra, m_extraTol, m_maxScatter );
    if (added) { 
      nbMissed = 0;
    } else { 
      nbMissed += extraStep;
    }
    if (m_maxMissed < nbMissed) break;
    extraSensIdx += extraStep;
  }

}
*/
//=========================================================================
//  Search starting with a pair of consecutive sensors.
//=========================================================================
void PatPixelTracking::searchByPair() {

  // Get the range of modules to start search on,
  // starting with the one at largest Z. 
  const int lastModule  = m_hitManager->lastModule();
  const int firstModule = m_hitManager->firstModule() + 2;
  for (int sens0 = lastModule; firstModule <= sens0; sens0 -= 1) { 
    // Pick-up the "paired" sensor one station backwards
    const int sens1 = sens0 - 2;
    PatPixelSensor* sensor0 = m_hitManager->sensor(sens0);
    PatPixelSensor* sensor1 = m_hitManager->sensor(sens1);
    double z0 = sensor0->z();
    double z1 = sensor1->z();
    double dz = z0 - z1;
    // Does it make sense to skip pairs very far apart ?
    // if( fabs(dz) > 60.0) continue;                                      
#ifdef DEBUG_HISTO
    plot(dz, "SeedPairDeltaZ", "Separation in Z [mm] between the seed pair sensors", -200.0, +200.0, 400);
#endif
    // Calculate the search window from the slope limits. 
    const double dxMax = m_maxXSlope * fabs(dz);
    const double dyMax = m_maxYSlope * fabs(dz);
    // Loop over hits in the first module (larger Z) in the pair.
    PatPixelHits::const_iterator ith0;
    for (ith0 = sensor0->hits().begin(); sensor0->hits().end() != ith0; ++ith0) {
      // Skip hits already assigned to tracks.
      if ((*ith0)->isUsed()) continue;                                            
      const double x0 = (*ith0)->x();
      const double y0 = (*ith0)->y();
      // Calculate x-pos. limits on the other module.
      const double xMin = x0 - dxMax;
      const double xMax = x0 + dxMax;
      if (m_debugTool && matchKey(*ith0)) {
        info() << format("s1%3d xMin%9.3f xMax%9.3f ", sens1, xMin, xMax);
        printHit(*ith0, "St0");
      }
      // Loop over hits in the second sensor (smaller Z) in the pair.
      PatPixelHits::const_iterator ith1; // , first1 = sensor1->hits().begin();
      for (ith1 = sensor1->hits().begin(); sensor1->hits().end() != ith1; ++ith1) { 
        const double x1 = (*ith1)->x();
        // Skip hits below the X-pos. limit.
        if (x1 < xMin) continue; // { first1 = itH1+1; continue; }
        // Stop search when above the X-pos. limit.
        if (x1 > xMax) break;
        // Skip hits already assigned to tracks.
        if ((*ith1)->isUsed()) continue;
        // Check y compatibility...
        const double y1 = (*ith1)->y();
        // Skip hits out of Y-pos. limit.
        if (fabs(y1 - y0) > dyMax) continue;

        m_debug = m_isDebug;
        if (m_debugTool) {
          if (matchKey(*ith0) && matchKey(*ith1)) m_debug = true;
          if (m_debug) {
            info() << format("s1%3d dxRel %7.3f dyRel %7.3f    ", 
                             sens1, (x1-xMin)/(xMax-xMin), fabs((*ith1)->y()-y0)/dyMax);
            printHit(*ith1);
          }
        }
        // Make a seed track out of these two hits.
        m_track.set(*ith0, *ith1);
        // Extend the seed track towards smaller Z.
        // TODO: downstream, on both sides of the detector as soon as one hit is missed
        trackDownstream(*ith0, *ith1);
        // trackUpstream();

        if (m_track.hits().size() < 3) continue;

        // Final checks
        if (m_track.hits().size() == 3) {
          // If only 3 hits, all should be unused and chi2 good.
          if (m_track.nbUnused() != 3) {
            if (m_debug) {
              info() << "  -- reject, only " << m_track.nbUnused() << " unused hits." << endmsg;
              printTrack(m_track); 
            }
            continue;
          }
        } else {
          // TODO: make fraction unused a parameter
          if (m_track.nbUnused() < 0.5 * m_track.hits().size()) {
            if (m_debug) {
              info() << "  -- reject, only " << m_track.nbUnused() << "/" 
                     << m_track.hits().size() << " hits are unused." << endmsg;
              printTrack(m_track); 
            }
            continue;
          }
        }

        m_tracks.push_back(m_track);
        if (m_debug) {
          info() << "=== Store track Nb " << m_tracks.size() << endmsg;
          printTrack(m_track);
        }
        if (m_track.hits().size() > 3) {
          m_track.tagUsedHits();
          break;
        }
      }
    }
  }

}

//=========================================================================
//  Convert the local tracks to LHCb tracks
//=========================================================================
void PatPixelTracking::makeLHCbTracks() {

#ifdef DEBUG_HISTO
  unsigned int nFwd = 0; 
  unsigned int nBwd = 0;
#endif

  LHCb::Tracks* outputTracks = new LHCb::Tracks();
  put(outputTracks, m_outputLocation);
  unsigned int key = 0;
  PatPixelTracks::iterator itt;
  for (itt = m_tracks.begin(); m_tracks.end() != itt; ++itt) {
    // Skip 3-hit tracks with double-used hits.
    if ((*itt).hits().size() == 3 && (*itt).nbUnused() != 3 ) continue; 
    // Create a new LHCb track.
    LHCb::Track* newTrack = new LHCb::Track(key++);
    newTrack->setType(LHCb::Track::Velo);
    newTrack->setHistory(LHCb::Track::PatFastVelo);
    newTrack->setPatRecStatus(LHCb::Track::PatRecIDs);
    if (m_debug) {
      info() << "=== Store track Nb " << outputTracks->size() << endmsg;
      printTrack(*itt);
    }

    // Loop over the hits, add their LHCbIDs to the LHCb track and 
    // find the highest Z.
    double zMax = -1.e9;
    PatPixelHits::iterator ith;
    for (ith = (*itt).hits().begin(); (*itt).hits().end() != ith; ++ith) {
      newTrack->addToLhcbIDs((*ith)->id());
      if ((*ith)->z() > zMax) zMax = (*ith)->z(); 
    }
    // Decide if this is a forward or backward track.
    // Calculate Z where the track passes closest to the beam.
    const double zBeam = (*itt).zBeam();
    // Define backward as z closest to beam downstream of hits.
    const bool backward = zBeam > zMax;
    newTrack->setFlag(LHCb::Track::Backward, backward);

    // Get the state at zBeam from the straight line fit.
    LHCb::State state;
    state.setLocation(LHCb::State::ClosestToBeam);
    state.setState((*itt).state(zBeam));
    state.setCovariance((*itt).covariance(zBeam));

    // Parameters for kalmanfit scattering. calibrated on MC, shamelessly hardcoded:
    const double tx = state.tx(); const double ty = state.ty();
    const double scat2 = 1e-8 + 7e-6*(tx*tx+ty*ty) ;

    // The logic is a bit messy in the following, so I hope we got all cases right
    if (m_stateClosestToBeamKalmanFit || m_addStateFirstLastMeasurementKalmanFit) {
      // Run a K-filter with scattering to improve IP resolution
      LHCb::State upstreamstate;
      (*itt).fitKalman(upstreamstate, backward ? 1 : -1 , scat2);
      // Add this state as state at first measurement if requested
      if (m_addStateFirstLastMeasurementKalmanFit) {
	upstreamstate.setLocation(LHCb::State::FirstMeasurement);
	newTrack->addToStates(upstreamstate);
      }
      // Transport the state to the closestToBeam position
      if (m_stateClosestToBeamKalmanFit) {
	upstreamstate.setLocation(LHCb::State::ClosestToBeam);
	upstreamstate.linearTransportTo(zBeam);
	newTrack->addToStates(upstreamstate);
      }
    }
    if (!m_stateClosestToBeamKalmanFit) {
      newTrack->addToStates(state);
    }
    
    // Set state at last measurement, if requested
    if ((!backward && m_stateEndVeloKalmanFit) || m_addStateFirstLastMeasurementKalmanFit) {
      LHCb::State downstreamstate;
      (*itt).fitKalman(downstreamstate, backward ? -1 : +1 , scat2);
      if(m_addStateFirstLastMeasurementKalmanFit) {
	downstreamstate.setLocation(LHCb::State::LastMeasurement);
	newTrack->addToStates(downstreamstate);
      }
      if (m_stateEndVeloKalmanFit) {
	state = downstreamstate;
      }
    } 
    
    // Add state at end of velo
    if (!backward) {
      state.setLocation(LHCb::State::EndVelo) ;
      state.linearTransportTo(StateParameters::ZEndVelo);
      newTrack->addToStates(state);
    }

    // Set the chi2/dof
    newTrack->setNDoF(2 * ((*itt).hits().size() - 2)); 
    newTrack->setChi2PerDoF((*itt).chi2());
    // Add the LHCb track to the list.
    outputTracks->insert(newTrack);

#ifdef DEBUG_HISTO
    const unsigned int nHitsPerTrack = (*itt).hits().size();
    if (backward) {
      plot(nHitsPerTrack, "Bwd_HitsPerTrack", "Number of hits per backward track",
           0.5, 40.5, 40);
      plot(newTrack->chi2PerDoF(), "Bwd_Chi2PerTrack", "Chi2/DoF of backward tracks",
           0.0, 10.0, 50);
      plot(newTrack->pseudoRapidity(), "Bwd_EtaOfTracks", "pseudoRapidity of backward tracks",
           1.0, 6.0, 50);
      plot(newTrack->phi()*(180.0/M_PI), "Bwd_PhiOfTracks", "Phi-angle of backward tracks",
           -180.0, 180.0, 60);
      plot2D(newTrack->pseudoRapidity(), nHitsPerTrack, "Bwd_HitsPerTrackVsEta", "hits/track vs pseudoRapidity of backward tracks",
             1.0, 6.0, 0.5, 15.5, 50, 15);
      plot2D(newTrack->pseudoRapidity(), newTrack->chi2PerDoF(), "Bwd_Chi2VsEta", "Chi2/DoF vs pseudoRapidity of backward tracks",
             1.0, 6.0, 0.0, 10.0, 50, 20);
      plot2D(nHitsPerTrack, newTrack->chi2PerDoF(), "Bwd_Chi2VsHitsPerTrack", "Chi2/DoF vs hits/backward track",
             0.5, 15.5, 0.0, 10.0, 15, 20);
      nBwd++;
    } else {
      plot(nHitsPerTrack, "Fwd_HitsPerTrack", "Number of hits per forward track",
           0.5, 40.5, 40);
      plot(newTrack->chi2PerDoF(), "Fwd_Chi2PerTrack", "Chi2/DoF of forward tracks",
           0.0, 10.0, 50);
      plot(newTrack->pseudoRapidity(), "Fwd_EtaOfTracks", "pseudoRapidity of forward tracks",
           1.0, 6.0, 50);
      plot(newTrack->phi()*(180.0/M_PI), "Fwd_PhiOfTracks", "Phi-angle of forward tracks",
           -180.0, 180.0, 60);
      plot2D(newTrack->pseudoRapidity(), nHitsPerTrack, "Fwd_HitsPerTrackVsEta", "hits/track vs pseudoRapidity of forward tracks",
             1.0, 6.0, 0.5, 15.5, 50, 15);
      plot2D(newTrack->pseudoRapidity(), newTrack->chi2PerDoF(), "Fwd_Chi2VsEta", "Chi2/DoF vs pseudoRapidity of forward tracks",
             1.0, 6.0, 0.0, 10.0, 50, 20);
      plot2D(nHitsPerTrack, newTrack->chi2PerDoF(), "Fwd_Chi2VsHitsPerTrack", "Chi2/DoF vs hits/forward track",
             0.5, 15.5, 0.0, 10.0, 15, 20);
      nFwd++;
    }
#endif
  }

#ifdef DEBUG_HISTO
  plot(nFwd, "Fwd_TracksPerEvent", "Number of forward tracks per event", 0.0, 400.0, 40);
  plot(nBwd, "Bwd_TracksPerEvent", "Number of backward tracks per event", 0.0, 400.0, 40);
#endif

  m_tracks.clear();

}

//=========================================================================
//  Add hits from the specified sensor to the track
//=========================================================================
PatPixelHit* PatPixelTracking::bestHit(PatPixelSensor* sensor, double xTol, double maxScatter,
                                       const PatPixelHit* h1, const PatPixelHit* h2) {

  if (sensor->hits().empty()) return NULL;
  const double x1 = h1->x();
  const double y1 = h1->y();
  const double z1 = h1->z();
  const double x2 = h2->x();
  const double y2 = h2->y();
  const double z2 = h2->z();
  const double tx = (x2 - x1) / (z2 - z1);
  const double ty = (y2 - y1) / (z2 - z1); 
  // Extrapolate to the z-position of the module.
  const double xGuess = x1 + tx * (sensor->z() - z1) - xTol;

  // If the first hit is already below this limit we can stop here.
  if (sensor->hits().back()->x() < xGuess) return NULL;
  // Do a binary search through the hits.
  PatPixelHits::const_iterator itStart = sensor->hits().begin();   
  unsigned int step = sensor->hits().size();
  while (2 < step) { // quick skip of hits that are above the X-limit
    step /= 2;
    if ((*(itStart + step))->x() < xGuess) itStart += step;
  }

  // Find the hit that matches best.
  unsigned int nFound = 0;
  double bestScatter = maxScatter;
  PatPixelHit* bestHit = 0;
  PatPixelHits::const_iterator ith;
  for (ith = itStart; sensor->hits().end() != ith; ++ith) {
    const double dz = (*ith)->z() - z1; 
    const double xPred = x1 + tx * dz;
    const double yPred = y1 + ty * dz;
#ifdef DEBUG_HISTO
    plot(((*ith)->x() - xPred) / xTol, "HitExtraErrPerTol", "Hit X extrapolation error / tolerance", -4.0, +4.0, 400);
#endif
    // If x-position is above prediction + tolerance, keep looking.
    if ((*ith)->x() + xTol < xPred) continue;
    // If x-position is below prediction - tolerance, stop the search.
    if ((*ith)->x() - xTol > xPred) break;
    const double dx = xPred - (*ith)->x();
    const double dy = yPred - (*ith)->y();
    const double scatter = sqrt(dx * dx + dy * dy) / fabs((*ith)->z() - z2);
    if (scatter < bestScatter) { 
      bestHit = *ith; 
      bestScatter = scatter;
    }
    if (scatter < maxScatter) ++nFound;
#ifdef DEBUG_HISTO
    plot(scatter, "HitScatter", "hit scatter [rad]", 0.0, 0.5, 500);
#endif
  }
#ifdef DEBUG_HISTO
  plot(nFound, "HitExtraCount", "Number of hits within the extrapolation window with chi2 within limits", 0.0, 10.0, 10);
#endif
  if (bestHit) {
#ifdef DEBUG_HISTO
    plot(bestScatter, "HitBestScatter", "best hit scatter [rad]",
         0.0, 0.1, 100);
#endif
    if (m_debug) printHitOnTrack(bestHit, false);
  }
  return bestHit;

}

//=========================================================================
// Debug the content of a hit
//=========================================================================
void PatPixelTracking::printHit(const PatPixelHit* hit, std::string title) {
  info() << title;
  info() << format(" module%3d x%8.3f y%8.3f z%8.2f used%2d",
                   hit->module(), hit->x(), hit->y(), hit->z(), hit->isUsed());
  if (m_debugTool) {
    LHCb::LHCbID id = hit->id();
    info() << " MC: ";
    m_debugTool->printKey(info(), id);
    if (matchKey(hit)) info() << " ***";
  }
  info() << endmsg;
}

//=========================================================================
// Print a track, with distance and chi2
//=========================================================================
void PatPixelTracking::printTrack(PatPixelTrack& track) {
  PatPixelHits::const_iterator ith;
  for (ith = track.hits().begin(); track.hits().end() != ith; ++ith) {
    printHit(*ith);
  }
}

//=========================================================================
// Print a hit on a track, with its distance.
//=========================================================================
void PatPixelTracking::printHitOnTrack(PatPixelHit* hit, bool ifMatch) {
  bool isMatching = matchKey(hit);
  isMatching = (isMatching && ifMatch) || (!isMatching && !ifMatch);
  if (isMatching) printHit(hit, "   ");
}
