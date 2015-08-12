// LHCb
#include "Event/Track.h"
#include "Event/StateParameters.h"
#include "Kernel/VPConstants.h"
// Local
#include "PrPixelTracking.h"

DECLARE_ALGORITHM_FACTORY(PrPixelTracking)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrPixelTracking::PrPixelTracking(const std::string &name,
                                 ISvcLocator *pSvcLocator)
    :
#ifdef DEBUG_HISTO
      GaudiTupleAlg(name, pSvcLocator),
#else
      GaudiAlgorithm(name, pSvcLocator),
#endif
      m_debugTool(NULL) {

  declareProperty("OutputTracksName",
                  m_outputLocation = LHCb::TrackLocation::Velo);
  declareProperty("MaxXSlope", m_maxXSlope = 0.400);
  declareProperty("MaxYSlope", m_maxYSlope = 0.400);
  // Tolerance window when adding hits
  declareProperty("ExtraTol", m_extraTol = 0.6 * Gaudi::Units::mm);
  // Number of modules without a hit after which to stop extrapolation
  declareProperty("MaxMissed", m_maxMissed = 3);

  // Acceptance criteria for adding new hits
  declareProperty("MaxScatter", m_maxScatter = 0.004);

  // Acceptance criteria for track candidates
  // Max. chi2 for 3-hit tracks
  declareProperty("MaxChi2Short", m_maxChi2Short = 20.0);
  // Min. fraction of unused hits
  declareProperty("FractionUnused", m_fractionUnused = 0.5);

  // Flag to clear hits (for rerunning in same event)
  declareProperty("ClearHits", m_clearHits = false);

  // Parameters for debugging
  declareProperty("DebugToolName", m_debugToolName = "");
  declareProperty("WantedKey", m_wantedKey = -100);
  declareProperty("TimingMeasurement", m_doTiming = false);

  // Parameters for Kalman fit
  declareProperty("ClosestToBeamStateKalmanFit",
                  m_stateClosestToBeamKalmanFit = true);
  declareProperty("EndVeloStateKalmanFit", m_stateEndVeloKalmanFit = false);
  declareProperty("AddFirstLastMeasurementStatesKalmanFit",
                  m_addStateFirstLastMeasurementKalmanFit = false);

  // Parameters for 3D hit building
  declareProperty("RunOnRawBanks", m_runOnRawBanks = true);
  declareProperty("MaxClusterSize", m_maxClusterSize = VP::NPixelsPerSensor);
  declareProperty("Trigger", m_trigger = false);

  m_tracks.reserve(10000);
}

//=============================================================================
// Destructor
//=============================================================================
PrPixelTracking::~PrPixelTracking() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode PrPixelTracking::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;
  // Setup the hit manager.
  m_hitManager = tool<PrPixelHitManager>("PrPixelHitManager");
  m_hitManager->setMaxClusterSize(m_maxClusterSize);
  m_hitManager->setTrigger(m_trigger);

  // Setup the debug tool.
  if ("" != m_debugToolName) m_debugTool = tool<IPrDebugTool>(m_debugToolName);
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

  // use the square of the scatter to avoid calls to sqrt()
  m_maxScatter *= m_maxScatter;

#ifdef DEBUG_HISTO
  setHistoTopDir("VP/");
#endif
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PrPixelTracking::execute() {

  // Build hits from clusters and sort them by global x within each module.
  if (m_doTiming) {
    m_timerTool->start(m_timeTotal);
    m_timerTool->start(m_timePrepare);
  }
  if (m_clearHits) m_hitManager->clearHits();
  if (m_runOnRawBanks) {
    if (!m_hitManager->buildHitsFromRawBank()) {
      return Error("Cannot retrieve/decode raw bank.", StatusCode::SUCCESS);
    }
  } else {
    if (!m_hitManager->buildHitsFromClusters()) {
      return Error("Cannot retrieve clusters.", StatusCode::SUCCESS);
    }
  }
  m_hitManager->sortByX();

  if (0 <= m_wantedKey) {
    if (0 <= m_wantedKey) {
      info() << "--- Looking for track " << m_wantedKey << endmsg;
    }
    const unsigned int firstModule = m_hitManager->firstModule();
    const unsigned int lastModule = m_hitManager->lastModule();
    for (unsigned int i = firstModule; i <= lastModule; ++i) {
      const PrPixelHits& hits = m_hitManager->hits(i);
      for (auto it = hits.cbegin(), end = hits.cend(); it != end; ++it) { 
        if (matchKey(*it)) printHit(*it);
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

  return StatusCode::SUCCESS;
}

//=============================================================================
// Extend track towards smaller z,
// on both sides of the detector as soon as one hit is missed.
//=============================================================================
void PrPixelTracking::extendTrack(const PrPixelHit *h1, const PrPixelHit *h2) {

  // Initially scan every second module (stay on the same side).
  int step = 2;
  // Start two modules behind the last one.
  int next = h2->module() - step;
  // Count modules without hits found.
  unsigned int nbMissed = 0;
  while (next >= 0) {
    PrPixelModule *module = m_hitManager->module(next);
    // Attempt to add new hits from this module with given tolerances
    PrPixelHit *h3 = bestHit(module, m_extraTol, m_maxScatter, h1, h2);
    if (h3) {
      m_track.addHit(h3);
      // Reset missed hit counter.
      nbMissed = 0;
      // Update the pair of hits to be used for extrapolating.
      h1 = h2;
      h2 = h3;
    } else {
      // No hits found.
      if (step == 2) {
        // Look on the other side.
        module = m_hitManager->module(next + 1);
        h3 = bestHit(module, m_extraTol, m_maxScatter, h1, h2);
        if (!h3) {
          nbMissed += step;
        } else {
          m_track.addHit(h3);
          h1 = h2;
          h2 = h3;
        }
        // Switch to scanning every module (left and right).
        step = 1;
      } else {
        ++nbMissed;
      }
    }
    if (m_maxMissed < nbMissed) break;
    next -= step;
  }
}

//=========================================================================
//  Search starting with a pair of consecutive modules.
//=========================================================================
void PrPixelTracking::searchByPair() {

  // Get the range of modules to start search on,
  // starting with the one at largest Z.
  const int lastModule = m_hitManager->lastModule();
  const int firstModule = m_hitManager->firstModule() + 4;
  for (int sens0 = lastModule; firstModule <= sens0; --sens0) {
    // Pick-up the "paired" module one station backwards
    const int sens1 = sens0 - 2;
    PrPixelModule *module0 = m_hitManager->module(sens0);
    PrPixelModule *module1 = m_hitManager->module(sens1);
    const float z0 = module0->z();
    const float z1 = module1->z();
    const float dz = z0 - z1;
    // Calculate the search window from the slope limits.
    const float dxMax = m_maxXSlope * fabs(dz);
    const float dyMax = m_maxYSlope * fabs(dz);
    // Loop over hits in the first module (larger Z) in the pair.
    auto end0 = module0->hits().cend();
    auto end1 = module1->hits().cend();
    auto first1 = module1->hits().cbegin();
    for (auto ith0 = module0->hits().cbegin(); end0 != ith0; ++ith0) {
      // Skip hits already assigned to tracks.
      if ((*ith0)->isUsed()) continue;
      const float x0 = (*ith0)->x();
      const float y0 = (*ith0)->y();
      // Calculate x-pos. limits on the other module.
      const float xMin = x0 - dxMax;
      const float xMax = x0 + dxMax;
      if (m_debugTool && matchKey(*ith0)) {
        info() << format("s1%3d xMin%9.3f xMax%9.3f ", sens1, xMin, xMax);
        printHit(*ith0, "St0");
      }
      // Loop over hits in the second module (smaller Z) in the pair.
      for (auto ith1 = first1; end1 != ith1; ++ith1) {
        const float x1 = (*ith1)->x();
        // Skip hits below the X-pos. limit.
        if (x1 < xMin) {
          first1 = ith1 + 1;
          continue;
        }
        // Stop search when above the X-pos. limit.
        if (x1 > xMax) break;
        // Skip hits already assigned to tracks.
        if ((*ith1)->isUsed()) continue;
        // Check y compatibility.
        const float y1 = (*ith1)->y();
        // Skip hits out of Y-pos. limit.
        if (fabs(y1 - y0) > dyMax) continue;

        m_debug = false;
        if (m_debugTool) {
          if (matchKey(*ith0) && matchKey(*ith1)) m_debug = true;
          if (m_debug) {
            info() << format("s1%3d dxRel %7.3f dyRel %7.3f    ", sens1,
                             (x1 - xMin) / (xMax - xMin),
                             fabs((*ith1)->y() - y0) / dyMax);
            printHit(*ith1);
          }
        }
        // Make a seed track out of these two hits.
        m_track.set(*ith0, *ith1);
        // Extend the seed track towards smaller Z.
        extendTrack(*ith0, *ith1);
        const unsigned int nHits = m_track.hits().size();
        if (nHits < 3) continue;

        // Final checks
        if (nHits == 3) {
          // In case of short tracks, all three hits should be unused.
          if (m_track.nbUnused() != 3) {
            if (m_debug) {
              info() << "  -- reject, only " << m_track.nbUnused()
                     << " unused hits." << endmsg;
              printTrack(m_track);
            }
            continue;
          }
          // Fit the track and apply a cut on the chi2.
          m_track.fit();
          if (m_track.chi2() > m_maxChi2Short) {
            if (m_debug) {
              info() << " -- reject, chi2 " << m_track.chi2() << " too high."
                     << endmsg;
              printTrack(m_track);
            }
            continue;
          }
        } else {
          if (m_track.nbUnused() < m_fractionUnused * nHits) {
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
        if (nHits > 3) {
          m_track.tagUsedHits();
          break;
        }
      }
    }
  }
}

//=========================================================================
// Convert the local tracks to LHCb tracks
//=========================================================================
void PrPixelTracking::makeLHCbTracks() {

  LHCb::Tracks *outputTracks = new LHCb::Tracks();
  put(outputTracks, m_outputLocation);
  unsigned int key = 0;
  auto endTracks = m_tracks.end(); 
  for (auto itt = m_tracks.begin(); itt != endTracks; ++itt) {
    // Skip 3-hit tracks with float-used hits.
    if ((*itt).hits().size() == 3) {
      if ((*itt).nbUnused() != 3) continue;
    } else {
      (*itt).fit();
    }
    // Create a new LHCb track.
    LHCb::Track *newTrack = new LHCb::Track(key++);
    newTrack->setType(LHCb::Track::Velo);
    newTrack->setHistory(LHCb::Track::PatFastVelo);
    newTrack->setPatRecStatus(LHCb::Track::PatRecIDs);
    if (m_debug) {
      info() << "=== Store track Nb " << outputTracks->size() << "\tnhits "
             << (*itt).hits().size() << endmsg;
      printTrack(*itt);
    }
    // Sort the hits back by decreasing z.
    std::sort((*itt).hits().begin(), (*itt).hits().end(),
              PrPixelHit::DecreasingByZ());
 
    // Loop over the hits and add their LHCbIDs to the LHCb track.
    auto endHits = (*itt).hits().cend();
    for (auto ith = (*itt).hits().cbegin(); ith != endHits; ++ith) {
      newTrack->addToLhcbIDs((*ith)->id());
    }
    // Decide if this is a forward or backward track.
    // Calculate z where the track passes closest to the beam.
    const float zBeam = (*itt).zBeam();
    // Define backward as z closest to beam downstream of hits.
    const bool backward = zBeam > (*itt).hits().front()->z();
    newTrack->setFlag(LHCb::Track::Backward, backward);

    // Get the state at zBeam from the straight line fit.
    LHCb::State state;
    state.setLocation(LHCb::State::ClosestToBeam);
    state.setState((*itt).state(zBeam));
    state.setCovariance((*itt).covariance(zBeam));

    // Parameters for kalmanfit scattering. calibrated on MC, shamelessly
    // hardcoded:
    const float tx = state.tx();
    const float ty = state.ty();
    const float scat2 = 1e-8 + 7e-6 * (tx * tx + ty * ty);

    // The logic is a bit messy in the following, so I hope we got all cases
    // right
    if (m_stateClosestToBeamKalmanFit ||
        m_addStateFirstLastMeasurementKalmanFit) {
      // Run a K-filter with scattering to improve IP resolution
      LHCb::State upstreamstate;
      (*itt).fitKalman(upstreamstate, backward ? 1 : -1, scat2);
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
    if ((!backward && m_stateEndVeloKalmanFit) ||
        m_addStateFirstLastMeasurementKalmanFit) {
      LHCb::State downstreamstate;
      (*itt).fitKalman(downstreamstate, backward ? -1 : +1, scat2);
      if (m_addStateFirstLastMeasurementKalmanFit) {
        downstreamstate.setLocation(LHCb::State::LastMeasurement);
        newTrack->addToStates(downstreamstate);
      }
      if (m_stateEndVeloKalmanFit) {
        state = downstreamstate;
      }
    }

    // Add state at end of velo
    if (!backward) {
      state.setLocation(LHCb::State::EndVelo);
      state.linearTransportTo(StateParameters::ZEndVelo);
      newTrack->addToStates(state);
    }

    // Set the chi2/dof
    newTrack->setNDoF(2 * ((*itt).hits().size() - 2));
    newTrack->setChi2PerDoF((*itt).chi2());
    // Add the LHCb track to the list.
    outputTracks->insert(newTrack);
  }
  m_tracks.clear();

}

//=========================================================================
// Add hits from the specified module to the track
//=========================================================================
PrPixelHit *PrPixelTracking::bestHit(PrPixelModule *module, const float xTol,
                                     const float maxScatter, 
                                     const PrPixelHit *h1,
                                     const PrPixelHit *h2) const {
  if (module->empty()) return NULL;
  const float x1 = h1->x();
  const float x2 = h2->x();
  const float y1 = h1->y();
  const float y2 = h2->y();
  const float z1 = h1->z();
  const float z2 = h2->z();
  const float td = 1.0 / (z2 - z1);
  const float txn = (x2 - x1);
  const float tx = txn * td;
  const float tyn = (y2 - y1);
  const float ty = tyn * td;
  // Extrapolate to the z-position of the module
  const float xGuess = x1 + tx * (module->z() - z1);

  // If the first hit is already below this limit we can stop here.
  if (module->lastHitX() < xGuess - xTol) return NULL;
  if (module->firstHitX() > xGuess + xTol) return NULL;

  // Do a binary search through the hits.
  unsigned int hit_start(0);
  unsigned int step(module->hits().size());
  const unsigned int module_nhits(step);
  const PrPixelHits &module_hits(module->hits());
  while (2 < step) {  // quick skip of hits that are above the X-limit
    step /= 2;
    if ((module_hits[hit_start + step])->x() < xGuess - xTol) hit_start += step;
  }

  // Find the hit that matches best.
  unsigned int nFound = 0;
  float bestScatter = maxScatter;
  PrPixelHit *bestHit = NULL;
  PrPixelHit *hit(NULL);
  for (unsigned int i = hit_start; i < module_nhits; ++i) {
    hit = module_hits[i];
    const float hit_z = hit->z();
    const float hit_x = hit->x();
    const float hit_y = hit->y();
    const float dz = hit_z - z1;
    const float xPred = x1 + tx * dz;
    const float yPred = y1 + ty * dz;
#ifdef DEBUG_HISTO
    plot((hit->x() - xPred) / xTol, "HitExtraErrPerTol",
         "Hit X extrapolation error / tolerance", -4.0, +4.0, 400);
#endif
    // If x-position is above prediction + tolerance, keep looking.
    if (hit_x + xTol < xPred) continue;
    // If x-position is below prediction - tolerance, stop the search.
    if (hit_x - xTol > xPred) break;
    const float dy = yPred - hit_y;
    // Skip hits outside the y-position tolerance.
    if (fabs(dy) > xTol) continue;
    const float scatterDenom = 1.0 / (hit_z - z2);
    const float dx = xPred - hit_x;
    const float scatterNum = (dx * dx) + (dy * dy);
    const float scatter = scatterNum * scatterDenom * scatterDenom;
    if (scatter < bestScatter) {
      bestHit = hit;
      bestScatter = scatter;
    }
    if (scatter < maxScatter) ++nFound;
#ifdef DEBUG_HISTO
    plot(sqrt(scatter), "HitScatter", "hit scatter [rad]", 0.0, 0.5, 500);
    plot2D(dx, dy, "Hit_dXdY",
           "Difference between hit and prediction in x and y [mm]", -1, 1, -1,
           1, 500, 500);
#endif
  }
#ifdef DEBUG_HISTO
  plot(nFound, "HitExtraCount",
       "Number of hits within the extrapolation window with chi2 within limits",
       0.0, 10.0, 10);
#endif
  if (bestHit) {
#ifdef DEBUG_HISTO
    plot(sqrt(bestScatter), "HitBestScatter", "best hit scatter [rad]", 0.0,
         0.1, 100);
#endif
    if (m_debug) printHitOnTrack(bestHit, false);
  }
  return bestHit;
}

//=========================================================================
// Debug the content of a hit
//=========================================================================
void PrPixelTracking::printHit(const PrPixelHit *hit, 
                               const std::string& title) const {
  info() << title;
  info() << format(" module%3d x%8.3f y%8.3f z%8.2f used%2d", hit->module(),
                   hit->x(), hit->y(), hit->z(), hit->isUsed());
  if (m_debugTool) {
    LHCb::LHCbID id = hit->id();
    info() << " MC: ";
    m_debugTool->printKey(info(), id);
    if (matchKey(hit)) info() << " ***";
  }
  info() << endmsg;
}

//=========================================================================
// Print all hits on a track.
//=========================================================================
void PrPixelTracking::printTrack(PrPixelTrack& track) const {
  auto end = track.hits().cend();
  for (auto it = track.hits().cbegin(); it != end; ++it) {
    printHit(*it);
  }
}

//=========================================================================
// Print a hit on a track.
//=========================================================================
void PrPixelTracking::printHitOnTrack(const PrPixelHit* hit, 
                                      const bool ifMatch) const {
  bool isMatching = matchKey(hit);
  isMatching = (isMatching && ifMatch) || (!isMatching && !ifMatch);
  if (isMatching) printHit(hit, "   ");
}
