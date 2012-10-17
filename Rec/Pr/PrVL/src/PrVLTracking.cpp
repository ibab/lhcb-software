// Gaudi
#include "GaudiKernel/AlgFactory.h"
// Event/TrackEvent
#include "Event/Track.h"
// Event/RecEvent
#include "Event/ProcStatus.h" 
// Event/DigiEvent
#include "Event/VLLiteCluster.h"
// Det/VLDet
#include "VLDet/DeVL.h"
// Local
#include "PrVLTracking.h"

using namespace LHCb;

/** @file PrVLTracking.cpp
 *
 *  Implementation of class: PrVLTracking 
 *
 */

DECLARE_ALGORITHM_FACTORY(PrVLTracking)

//============================================================================
/// Standard constructor
//============================================================================
PrVLTracking::PrVLTracking(const std::string& name, ISvcLocator* pSvcLocator) :
    GaudiAlgorithm(name, pSvcLocator),
    m_det(0),
    m_debugTool(0) {

  declareProperty("TrackLocation", m_trackLocation = TrackLocation::Velo);

  declareProperty("Forward", m_forward = true);
  declareProperty("Backward", m_backward = true);

  declareProperty("ZVertexMin", m_zVertexMin = -170. * Gaudi::Units::mm);
  declareProperty("ZVertexMax", m_zVertexMax = +120. * Gaudi::Units::mm);
  declareProperty("ZVertexMaxBack", 
                  m_zVertexMaxBack = +1200. * Gaudi::Units::mm);
  // Parameters for RZ tracking
  declareProperty("MaxRSlope",   m_maxRSlope = 0.45);
  declareProperty("rTol4",       m_rTol4  = 0.7);
  declareProperty("rTol3",       m_rTol3  = 1.1);
  declareProperty("rTolExtra",   m_rTolExtra   = 3.0);
  declareProperty("rTolOverlap", m_rTolOverlap = 0.9);
  declareProperty("MaxMissed", m_maxMissed = 1);
  declareProperty("MinToTag",  m_minToTag = 4);
  // Parameters for space tracking
  declareProperty("PhiTol",        m_phiTol = 0.32);
  declareProperty("PhiTolOverlap", m_phiTolOverlap = 0.08); 
  declareProperty("MaxD2",         m_d2Max = 0.06);
  declareProperty("FractionFound", m_fractionFound = 0.6);
  declareProperty("MaxChi2PerHit", m_maxChi2PerHit = 12.);
  declareProperty("MaxChi2ToAdd",  m_maxChi2ToAdd = 40.);
  declareProperty("MaxQFactor",    m_maxQFactor  = 6.);
  declareProperty("MaxQFactor3",   m_maxQFactor3 = 3.);
  declareProperty("DeltaQuality",  m_deltaQuality = 0.6);
  // Parameters for merging clones
  declareProperty("FractionForMerge", m_fractionForMerge = 0.6);
  declareProperty("MaxDistToMerge", m_maxDistToMerge = 0.1 * Gaudi::Units::mm);
  declareProperty("MaxDeltaSlopeToMerge", m_maxDeltaSlopeToMerge = 0.002);
  // Parameters for recovery
  declareProperty("MaxRZForExtra", m_maxRZForExtra = 200);
  declareProperty("PhiUnusedFirstTol",  m_phiUnusedFirstTol  =  5.);
  declareProperty("PhiUnusedSecondTol", m_phiUnusedSecondTol = 10.);
  // Parameters for debugging
  declareProperty("DebugToolName", m_debugToolName = "");
  declareProperty("WantedKey", m_wantedKey = -100);
  declareProperty("TimingMeasurement", m_doTiming = false);

}

//============================================================================
/// Initialization
//============================================================================
StatusCode PrVLTracking::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;
  m_debug = msgLevel(MSG::DEBUG);
  if (m_debug) debug() << " ==> initialize()" << endmsg;

  // Get the detector element. 
  m_det = getDet<DeVL>(DeVLLocation::Default);
  // Get the number of R and phi sectors.
  m_nZonesR = m_det->rSensors().front()->numberOfZones();
  m_nZonesPhi = m_det->phiSensors().front()->numberOfZones();
  // Determine the highest sensor number.
  unsigned int maxSensorNumber = 0;
  const std::vector<DeVLSensor*>& sensors = m_det->sensors();
  std::vector<DeVLSensor*>::const_iterator it;
  for (it = sensors.begin(); it != sensors.end(); ++it) {
    if ((*it)->sensorNumber() > maxSensorNumber) {
      maxSensorNumber = (*it)->sensorNumber();
    }
  }
  // Setup the hit map.
  m_hits.resize(maxSensorNumber + 1);
  for (unsigned int i = 0; i <= maxSensorNumber; ++i) {
    if (!m_det->sensor(i)) continue;
    if (m_det->sensor(i)->isPhi()) {
      m_hits[i].resize(m_nZonesPhi, PrVLHits());
      for (unsigned int j = 0; j < m_nZonesPhi; ++j) {
        m_hits[i][j].reserve(100);
      }
    } else {
      m_hits[i].resize(m_nZonesR, PrVLHits());
      for (unsigned int j = 0; j < m_nZonesR; ++j) {
        m_hits[i][j].reserve(100);
      } 
    }
  }
  m_pool.resize(10000);
  m_nextInPool = m_pool.begin();

  // Debugging
  if ("" != m_debugToolName) {
    if (m_wantedKey >= 0) {
      m_debugTool = tool<IPrDebugTool>(m_debugToolName);
    }
  } else if (m_debug) {
    m_debug = false;
  }

  // Setup the timing tool.
  if (m_doTiming) {
    m_timerTool = tool<ISequencerTimerTool>("SequencerTimerTool/Timer", this);
    m_timeTotal = m_timerTool->addTimer("PrVL");
    m_timerTool->increaseIndent();
    m_timePrepare = m_timerTool->addTimer("PrVL prepare");
    m_timeFwd4    = m_timerTool->addTimer("PrVL find fwd. quadruplets");
    m_timeBwd4    = m_timerTool->addTimer("PrVL find bwd. quadruplets");
    m_timeFwd3    = m_timerTool->addTimer("PrVL find fwd. triplets");
    m_timeBwd3    = m_timerTool->addTimer("PrVL find bwd. triplets");
    m_timeSpace   = m_timerTool->addTimer("PrVL find space tracks");
    m_timeUnused  = m_timerTool->addTimer("PrVL unused phi hits");
    m_timeFinal   = m_timerTool->addTimer("PrVL make LHCb tracks");
    m_timerTool->decreaseIndent();
  }
  return StatusCode::SUCCESS;

}

//=============================================================================
/// Main execution
//=============================================================================
StatusCode PrVLTracking::execute() {

  if (msgLevel(MSG::DEBUG)) debug() << " ==> execute()" << endmsg;
  if (m_doTiming) {
    m_timerTool->start(m_timeTotal);
    m_timerTool->start(m_timePrepare);
  }

  Tracks* lhcbTracks;
  if (exist<Tracks>(m_trackLocation)) {
    lhcbTracks = get<Tracks>(m_trackLocation);
  } else {
    lhcbTracks = new Tracks();
    lhcbTracks->reserve(500);
    put(lhcbTracks, m_trackLocation);
  }
  m_rzTracks.clear();
  m_spaceTracks.clear();
  buildHits();

  const double minZ = m_zVertexMin + 200.;
  const double maxZ = m_zVertexMax - 200.;
  const std::vector<DeVLRSensor*>& rSensors = m_det->rSensors();

  if (m_doTiming) m_timerTool->stop(m_timePrepare);
  // Look for quadruplets.
  if (m_forward) {
    const bool forward = true;
    if (m_debug) {
      debug() << "Looking for forward quadruplets" << endmsg;
    }
    if (m_doTiming) m_timerTool->start(m_timeFwd4);
    std::vector<DeVLRSensor*>::const_reverse_iterator it;
    for (it = rSensors.rbegin(); it != rSensors.rend(); ++it) {
      if ((*it)->z() < minZ) break;
      findQuadruplets(*it, forward);
    }
    if (m_doTiming) m_timerTool->stop(m_timeFwd4);
  }
  if (m_backward) {
    const bool forward = false;
    if (m_debug) {
      debug() << "Looking for backward quadruplets" << endmsg;
    }
    if (m_doTiming) m_timerTool->start(m_timeBwd4);
    std::vector<DeVLRSensor*>::const_iterator it;
    for (it = rSensors.begin(); it != rSensors.end(); ++it) {
      if ((*it)->z() > maxZ) break;
      findQuadruplets(*it, forward);
    }
    if (m_doTiming) m_timerTool->stop(m_timeBwd4);
  }  
  // Look for unused triplets.
  if (m_forward) {
    const bool forward = true;
    if (m_debug) {
      debug() << "Looking for forward triplets" << endmsg;
    }
    if (m_doTiming) m_timerTool->start(m_timeFwd3);
    std::vector<DeVLRSensor*>::const_reverse_iterator it;
    for (it = rSensors.rbegin(); it != rSensors.rend(); ++it) {
      if ((*it)->z() < minZ) break;
      findTriplets(*it, forward);
    }
    if (m_doTiming) m_timerTool->stop(m_timeFwd3);
  }
  if (m_backward) {
    const bool forward = false;
    if (m_debug) {
      debug() << "Looking for backward triplets" << endmsg;
    }
    if (m_doTiming) m_timerTool->start(m_timeBwd3);
    std::vector<DeVLRSensor*>::const_iterator it;
    for (it = rSensors.begin(); it != rSensors.end(); ++it) {
      if ((*it)->z() > maxZ) break;
      findTriplets(*it, forward);
    }
    if (m_doTiming) m_timerTool->stop(m_timeBwd3);
  }
  if (m_doTiming) m_timerTool->start(m_timeSpace);
  std::stable_sort(m_rzTracks.begin(), m_rzTracks.end(), 
                   PrVLTrack::DecreasingByRLength());
  if (m_debug) {
    debug() << "Found " << m_rzTracks.size() << " RZ tracks" << endmsg;
  }
  PrVLTracks::iterator itt;
  for (itt = m_rzTracks.begin(); itt != m_rzTracks.end(); ++itt) {
    if (m_debug) {
      debug() << "Track " << itt - m_rzTracks.begin() << endmsg;
      printRZTrack(*itt);
    }
    findSpaceTracks(*itt);
  }
  if (m_doTiming) m_timerTool->stop(m_timeSpace);
  if (m_debug) {
    debug() << "Made " << m_spaceTracks.size() << " space tracks" << endmsg;
  }
  // Perform the recovery.
  if (m_rzTracks.size() < m_maxRZForExtra &&
      (m_rzTracks.size() * 0.5 <= m_spaceTracks.size() ||
       m_rzTracks.size() < 20)) {
    if (m_doTiming) m_timerTool->start(m_timeUnused);
    findUnusedPhi();
    if (m_doTiming) m_timerTool->stop(m_timeUnused);
  }
  if (m_debug) {
    debug() << m_spaceTracks.size() << " tracks after recovery" << endmsg;
  }
  if (m_doTiming) m_timerTool->start(m_timeFinal);
  // Cleanup tracks with different R, same Phi hits.
  if (m_spaceTracks.size() >= 2) mergeSpaceClones();
  // Store the track candidates as LHCb tracks.
  makeLHCbTracks(lhcbTracks);
  if (m_doTiming) {
    m_timerTool->stop(m_timeFinal);
    m_timerTool->stop(m_timeTotal);
  }
  if (m_debug) {
    for (itt = m_spaceTracks.begin(); itt != m_spaceTracks.end(); ++itt) {
      if ((*itt).valid()) printTrack(*itt);
    }
  } 
  return StatusCode::SUCCESS;

}

//============================================================================
/// Finalization
//============================================================================
StatusCode PrVLTracking::finalize() {

  return GaudiAlgorithm::finalize();

}

//============================================================================
///  Find 4 hits in RZ, with optionally a hole.
//============================================================================
void PrVLTracking::findQuadruplets(DeVLRSensor* sensor0, const bool forward) {

  if (!sensor0) return;
  const unsigned int s0 = sensor0->sensorNumber();
  const double z0 = sensor0->z();
  for (unsigned int iz = 0; iz < m_nZonesR; ++iz) {
    if (m_hits[s0][iz].empty()) continue;
    const DeVLRSensor* sensor1 = 0;
    if (forward) {
      sensor1 = sensor0->previousRSensor();
    } else {
      sensor1 = sensor0->nextRSensor();
    }
    if (!sensor1) return;
    const DeVLRSensor* sensor2 = 0;
    if (forward) {
      sensor2 = sensor1->previousRSensor();
    } else {
      sensor2 = sensor1->nextRSensor();
    }
    if (!sensor2) return;
    const DeVLRSensor* sensor3 = 0;
    if (forward) {
      sensor3 = sensor2->previousRSensor();
    } else {
      sensor3 = sensor2->nextRSensor();
    }
    if (!sensor3) return;
    const unsigned int nCases = 4;
    for (unsigned int ic = 0; ic < nCases; ++ic) {
      // Case 0: x x x x
      if (ic == 1) {
        // Case 1: x x x o x
        if (forward) {
          sensor3 = sensor3->previousRSensor();
        } else {
          sensor3 = sensor3->nextRSensor();
        }
        if (!sensor3) break;
      } else if (ic == 2) {
        // Case 2: x x o x x
        if (forward) {
          sensor2 = sensor2->previousRSensor();
        } else {
          sensor2 = sensor2->nextRSensor();
        }
      } else if (ic == 3) {
        // Case 3: x o x x x
        if (forward) {
          sensor1 = sensor1->previousRSensor();
        } else {
          sensor1 = sensor1->nextRSensor();
        }
      }
      const unsigned int s1 = sensor1->sensorNumber();
      const unsigned int s2 = sensor2->sensorNumber();
      const unsigned int s3 = sensor3->sensorNumber();
      if (m_hits[s1][iz].empty()) continue;
      if (m_hits[s2][iz].empty()) continue;
      if (m_hits[s3][iz].empty()) continue;
      const double z1 = sensor1->z();
      const double z2 = sensor2->z();
      const double z3 = sensor3->z();
      const double zFrac1 = (z1 - z0) / (z3 - z0);
      const double zFrac2 = (z2 - z0) / (z3 - z0);
      // Loop over the hits in the first sensor.
      PrVLHits::const_iterator it0;
      PrVLHits::const_iterator begin3 = m_hits[s3][iz].begin();
      for (it0 = m_hits[s0][iz].begin(); it0 != m_hits[s0][iz].end(); ++it0) {
        // Skip used hits.
        if ((*it0)->nUsed() > 0) continue;
        const double r0 = (*it0)->rGlobal();
        double rMin = r0 + m_maxRSlope * (z3 - z0);
        // Allow parallel tracks.
        double rMax = r0 + 1.;
        double rMaxNormal = r0 * (z3 - m_zVertexMin) / (z0 - m_zVertexMin);
        if (!forward) {
          rMin = r0 - m_maxRSlope * (z3 - z0);
          rMax = r0 * (z3 - m_zVertexMaxBack) / (z0 - m_zVertexMaxBack);
          rMaxNormal = rMax;
        }
        if (m_debug && matchKey(*it0)) {
          printHit(*it0, "S0 ");
          debug() << format("  %8.3f < r < %8.3f", rMin, rMax) << endmsg;
        }
        // Loop over the hits in the last sensor to find a possible match.
        PrVLHits::const_iterator it3;
        for (it3 = begin3; it3 != m_hits[s3][iz].end(); ++it3) {
          // Skip used hits.
          if (ic != 0 && (*it3)->nUsed() > 0) continue;
          const double r3 = (*it3)->rGlobal();
          if (r3 < rMin) {
            begin3 = it3;
            continue;
          }
          if (r3 >= rMax) break;
          const double rPred1 = r0 + zFrac1 * (r3 - r0);
          const double tol1 = m_rTol4 * sensor1->rPitchOfRadius(rPred1);
          if (m_debug && (matchKey(*it0) || matchKey(*it3))) {
            printHit(*it3, "  S3 ");
            debug() << format("    rPred %8.3f (tol %6.3f)", rPred1, tol1) 
                    << endmsg;
          }
          // Check if the second sensor has a matching hit.
          PrVLHit* h1 = closestHit(m_hits[s1][iz], rPred1, tol1, false);
          if (!h1) continue;
          const double rPred2 = r0 + zFrac2 * (r3 - r0);
          const double tol2 = m_rTol4 * sensor2->rPitchOfRadius(rPred2);
          if (m_debug && 
              (matchKey(*it0) || matchKey(*it3) || matchKey(h1))) {
            printHit(h1, "    S1 ");
            debug() << format("    rPred %8.3f (tol %6.3f)", rPred2, tol2) 
                    << endmsg;
          }
          // Check if the third sensor has a matching hit. 
          PrVLHit* h2 = closestHit(m_hits[s2][iz], rPred2, tol2, false);
          if (!h2) continue;
          if (m_debug) {
            debug() << "    === valid quadruplet ===" << endmsg;
            printHit(*it0, "    S0 ");
            printHit(h1,   "    S1 ");
            printHit(h2,   "    S2 ");
            printHit(*it3, "    S3 ");
          }
          // Create a track.
          PrVLTrack newTrack;
          newTrack.addRHit(*it0);
          newTrack.addRHit(h1);
          newTrack.addRHit(h2);
          newTrack.addRHit(*it3);
          newTrack.setBackward(!forward);
           
          // Try to add hits on adjacent sensors on the same side.
          int nMiss = extendTrack(newTrack, s3, iz, forward, false);
          // Require at least two unused hits on the track.
          if (newTrack.nbRHits() - newTrack.nbUsedRHits() <= 1) {
            if (m_debug) {
              debug() << "    " << newTrack.nbUsedRHits() << "/"
                      << newTrack.nbRHits() << " are used." << endmsg;
            }
            continue;
          }
          // Store the number of missed stations.
          newTrack.setMissedSensors(nMiss);  
          // Set the zone number.
          if (sensor0->isRight()) {
            newTrack.setZone(iz + m_nZonesR);
          } else {
            newTrack.setZone(iz);
          }
          // In case of outer zones, try to add hits on the other side.
          if (0 == iz || m_nZonesR - 1 == iz) {
            extendTrackOtherSide(newTrack, s0, iz, forward, false);
            extendTrackOtherSide(newTrack, s0, iz, !forward, false);
          }
          if (forward && (s0 <= 33 && s0 >= 30)) {
            // Large Z gap 
            extendTrack(newTrack, s0, iz, !forward, false);
          }
          if (forward) {
            std::sort(newTrack.rHits().begin(), newTrack.rHits().end(), 
                      PrVLHit::IncreasingByZ());
          } else {
            std::sort(newTrack.rHits().begin(), newTrack.rHits().end(), 
                      PrVLHit::DecreasingByZ());
          }
          m_rzTracks.push_back(newTrack);
          if (m_debug) {
            debug() << "    Adding track " << m_rzTracks.size() - 1 << endmsg;
            // printRZTrack(newTrack, "");
          }
          if (newTrack.nbRHits() >= m_minToTag) {
            newTrack.tagUsedRHits();
            rMax = rMaxNormal;
          }
        }
      }
    }
  }
 
}

//============================================================================
/// Find three unused hits in RZ
//============================================================================
void PrVLTracking::findTriplets(DeVLRSensor* sensor0, const bool forward) {

  if (!sensor0) return;
  const unsigned int s0 = sensor0->sensorNumber();
  const double z0 = sensor0->z();
  for (unsigned int iz = 0; iz < m_nZonesR; ++iz) {
    if (m_hits[s0][iz].empty()) continue;
    const DeVLRSensor* sensor1 = 0;
    if (forward) {
      sensor1 = sensor0->previousRSensor();
    } else {
      sensor1 = sensor0->nextRSensor();
    }
    if (!sensor1) return;
    const DeVLRSensor* sensor2 = 0;
    if (forward) {
      sensor2 = sensor1->previousRSensor();
    } else {
      sensor2 = sensor1->nextRSensor();
    }
    if (!sensor2) return;
    const unsigned int nCases = 3;
    for (unsigned int ic = 0; ic < nCases; ++ic) {
      // Case 0: x x x
      if (ic == 1) {
        // Case 1: x x o x
        if (forward) {
          sensor2 = sensor2->previousRSensor();
        } else {
          sensor2 = sensor2->nextRSensor();
        }
        if (!sensor2) break;
      } else if (ic == 2) {
        // Case 2: x o x x
        if (forward) {
          sensor2 = sensor2->previousRSensor();
        } else {
          sensor2 = sensor2->nextRSensor();
        }
      }
      const unsigned int s1 = sensor1->sensorNumber();
      const unsigned int s2 = sensor2->sensorNumber();  
      const double z1 = sensor1->z();
      const double z2 = sensor2->z();
      const double zFrac = (z1 - z0) / (z2 - z0);
      // Loop over the hits in the first sensor.
      PrVLHits::const_iterator it0;
      PrVLHits::const_iterator begin2 = m_hits[s2][iz].begin();
      for (it0 = m_hits[s0][iz].begin(); it0 != m_hits[s0][iz].end(); ++it0) {
        // Skip used hits.
        if ((*it0)->nUsed() > 0) continue;
        double r0 = (*it0)->rGlobal();
        double rMin = r0 + m_maxRSlope * (z2 - z0);
        double rMax = r0; 
        if (!forward) {
          rMin = r0 - m_maxRSlope * (z2 - z0);
          rMax = r0 * (z2 - m_zVertexMaxBack) / (z0 - m_zVertexMaxBack);
        }
        if (m_debug && matchKey(*it0)) {
          printHit(*it0, "S0 ");
          debug() << format("  %8.3f < r < %8.3f", rMin, rMax) << endmsg;
        }
        // Loop over the hits in the last sensor to find a possible match.
        PrVLHits::const_iterator it2;
        for (it2 = begin2; it2 != m_hits[s2][iz].end(); ++it2) {
          if ((*it2)->nUsed() > 0) continue;
          const double r2 = (*it2)->rGlobal();
          if (r2 < rMin) {
            begin2 = it2;
            continue;
          }
          if (r2 >= rMax) break;
          const double rPred = r0 + zFrac * (r2 - r0);
          const double tol = m_rTol3 * m_det->rSensor(s1)->rPitchOfRadius(rPred);
          if (m_debug && (matchKey(*it0) || matchKey(*it2))) {
            printHit(*it2, "  S2 ");
            debug() << format("    rPred %8.3f (tol %6.3f)", rPred, tol) << endmsg;
          }
          // Check if the second sensor has a matching hit.
          PrVLHit* h1 = closestHit(m_hits[s1][iz], rPred, tol, true);
          if (!h1) continue;
          if (m_debug) {
            debug() << "    === valid triplet ===" << endmsg;
            printHit(*it0, "    S0 ");
            printHit(h1,   "    S1 ");
            printHit(*it2, "    S2 ");
          }
          // Create a track.
          PrVLTrack newTrack;
          newTrack.addRHit(*it0);
          newTrack.addRHit(h1);
          newTrack.addRHit(*it2);
          newTrack.setBackward(!forward);

          // Try to add hits on adjacent sensors on the same side.
          const int nMiss = extendTrack(newTrack, s2, iz, forward, false);
          // Store the number of missed stations.
          newTrack.setMissedSensors(nMiss);  
          if (newTrack.nbRHits() - newTrack.nbUsedRHits() <= 1) {
            if (m_debug) {
              debug() << "    " << newTrack.nbUsedRHits() << "/"
                      << newTrack.nbRHits() << " are used." << endmsg;
            }
            continue;
          }

          // Tracks with last 3 sensors, a gap, a single other. 
          if (s0 > 39 && newTrack.rHits().size() == 4) {
            if (newTrack.rHits().back()->sensor() < 34) {
              // Remove the last one.
              newTrack.rHits().pop_back();
            }
          }

          if (3 == newTrack.nbRHits()) {
            // Try to extend the track in the other direction.
            int nMissBack = extendTrack(newTrack, s0, iz, !forward, false);
            // Try adjacent zones. 
            if (nMissBack > m_maxMissed) {
              if (iz != 0) {
                nMissBack = extendTrack(newTrack, s0, iz - 1, !forward, false);
              }
            }
            if (nMissBack > m_maxMissed) {
              if (iz != m_nZonesR - 1) {
                nMissBack = extendTrack(newTrack, s0, iz + 1, !forward, false);
              }
            }
            if (nMissBack > m_maxMissed) continue;
          }
        
          if (sensor0->isRight()) {
            newTrack.setZone(iz + m_nZonesR);
          } else {
            newTrack.setZone(iz);
          }
        
          // In case of outer zones, try to add hits on the other side.
          if (0 == iz || m_nZonesR - 1 == iz) {
            extendTrackOtherSide(newTrack, s0, iz, forward, false);
          }

          // If 3 hit tracks share hits, take the best one
          if (3 == newTrack.nbRHits()) {
            bool reject = false;
            PrVLTracks::iterator itt;
            for (itt = m_rzTracks.begin(); itt != m_rzTracks.end(); ++itt) {
              if ((*itt).nbRHits() != 3) continue;
              int nCommon = 0;
              PrVLHits::const_iterator ith;
              for (ith = (*itt).rHits().begin(); ith != (*itt).rHits().end(); ++ith) {
                if (std::find(newTrack.rHits().begin(), 
                              newTrack.rHits().end(), 
                              *ith) != newTrack.rHits().end()) {
                  ++nCommon;
                }
              }
              if (nCommon > 0) {
                if (m_debug) {
                  debug() << "    " << nCommon << " hits shared with track "
                          << itt - m_rzTracks.begin() << endmsg;
                  debug() << "    Chi2: " << newTrack.rChi2() << " (new), " 
                          << (*itt).rChi2() << " (" << itt - m_rzTracks.begin() 
                          << ")" << endmsg;
                }
                if (newTrack.rChi2() < (*itt).rChi2()) {
                  if (m_debug) {
                    debug() << "    Replacing track " 
                            << itt - m_rzTracks.begin() << endmsg;
                  }
                  (*itt) = newTrack;
                } else if (m_debug) {
                  debug() << "    Rejecting new track" << endmsg;
                }
                reject = true;
                break;
              }
            }
            if (reject) continue;
          }

          if (forward) {
            std::sort(newTrack.rHits().begin(), newTrack.rHits().end(), 
                      PrVLHit::IncreasingByZ());
          } else {
            std::sort(newTrack.rHits().begin(), newTrack.rHits().end(), 
                      PrVLHit::DecreasingByZ());
          }
          m_rzTracks.push_back(newTrack);
          if (m_debug) {
            debug() << "    Adding track " << m_rzTracks.size() - 1 << endmsg;
            // printRZTrack(newTrack, "");
          }
          if (m_minToTag <= newTrack.nbRHits()) {
            newTrack.tagUsedRHits();
          }
        }
      }
    }
  }
  
}
//=========================================================================
///  Make space tracks
//=========================================================================
void PrVLTracking::findSpaceTracks(PrVLTrack& seed) {

  const bool forward = !seed.backward();
  const unsigned int rZone = seed.zone() % m_nZonesR;
  bool vertical = false;
  if (0 == rZone || m_nZonesR - 1 == rZone) vertical = true; 

  // Determine the range of R sensors with a hit.
  int nLeftR = 0, nRightR = 0;
  unsigned int minSensor = 1000;
  unsigned int maxSensor = 0;
  PrVLHits::iterator ith;
  for (ith = seed.rHits().begin(); ith != seed.rHits().end(); ++ith) {
    const unsigned int sensorNumber = (*ith)->sensor();
    if (sensorNumber > maxSensor) maxSensor = sensorNumber;
    if (sensorNumber < minSensor) minSensor = sensorNumber;
    unsigned int zoneNumber = (*ith)->zone();
    if (m_det->sensor(sensorNumber)->isRight()) {
      ++nRightR;
      zoneNumber += m_nZonesR;
    } else {
      ++nLeftR;
    }
  }
  const DeVLPhiSensor* first = m_det->rSensor(minSensor)->associatedPhiSensor();
  if (first->previousPhiSensor()) first = first->previousPhiSensor();
  const DeVLPhiSensor* last = m_det->rSensor(maxSensor)->associatedPhiSensor();
  if (last->nextPhiSensor()) last = last->nextPhiSensor();
  if (m_debug) {
    debug() << "Searching phi sensors " 
            << first->sensorNumber() << " - " << last->sensorNumber() << endmsg;
  }
  const bool inOverlap = nLeftR > 2 && nRightR > 2;
  double dphi = Gaudi::Units::pi / double(m_nZonesR);
  double phi = -0.5 * Gaudi::Units::pi + (0.5 + seed.zone()) * dphi;
  double cphi = cos(phi);
  double sphi = sin(phi);
  if (inOverlap) {
    cphi = 0.;
    sphi /= fabs(sphi);
  }
  double signOfSolution = 1.;
  if (seed.zone() < 1 || seed.zone() > m_nZonesR) signOfSolution = -1.;

  // Store the phi hits in the range.
  unsigned int nStations = (m_det->numberRSensors() + 
                            m_det->numberPhiSensors()) / 4;
  std::vector<PrVLHits> phiHits(nStations, PrVLHits());
  for (unsigned int i = first->sensorNumber(); i <= last->sensorNumber(); ++i) {
    const DeVLPhiSensor* sensor = m_det->phiSensor(i);
    const double rPred = seed.rInterpolated(sensor->z());
    const double x0 = sensor->xCentre();
    const double y0 = sensor->yCentre();
    const double dR2 = rPred * rPred - x0 * x0 - y0 * y0;
    const double xPred = x0 + rPred * cphi;
    const double yPred = y0 + rPred * sphi;

    double maxDSin = m_phiTol;
    if (inOverlap) maxDSin = m_phiTolOverlap;
    const double minDist = -maxDSin * rPred;
    const double maxDist =  maxDSin * rPred;

    const unsigned int station = sensor->station();
    for (unsigned int j = 0; j < m_nZonesPhi; ++j) {
      if (rPred < sensor->rMin(j) || 
          rPred > sensor->rMax(j)) {
        continue;
      }
      double minR = sensor->rMin(j);
      for (ith = m_hits[i][j].begin(); ith != m_hits[i][j].end(); ++ith) {
        if (m_debug && matchKey(*ith)) {
          printHit(*ith, "");
        }
        const double dx = (*ith)->xStripCentre() - x0;
        const double dy = (*ith)->yStripCentre() - y0;
        // Check the scalar product.
        const double sp = cphi * dx + sphi * dy;
        if (0 == j) {
          if (sp < 0.71 * (minR + sensor->stripLength((*ith)->strip()))) continue;
        } else if (1 == j) {
          if (sp < 0.74 * (minR + sensor->stripLength((*ith)->strip()))) continue;
        } else if (2 == j) {
          if (sp < 0.72 * (minR + sensor->stripLength((*ith)->strip()))) continue;
        } 
        if (m_debug && matchKey(*ith)) {
          debug() << "    Passed scalar product cut." << endmsg;
        }
        // Compute the distance.
        const double dist = (*ith)->distance(xPred, yPred);
        if (dist < minDist || dist > maxDist) continue;
        if (m_debug && matchKey(*ith)) {
          debug() << "    Passed distance cut." << endmsg;
        }
        const double a = (*ith)->a();
        const double b = (*ith)->b();
        const double c = (*ith)->c();
        double x = 0.;
        double y = 0.;
        if (vertical) {
          const double bEq = b * c + a * b * x0 - a * a * y0;
          const double cEq = c * c + 2 * a * c * x0 - a * a * dR2;
          y = - bEq + signOfSolution * sqrt(bEq * bEq - cEq);
          x = (- c - b * y) / a;
        } else {
          const double bEq = a * c + a * b * y0 - b * b * x0;
          const double cEq = c * c + 2 * b * c * y0 - b * b * dR2;
          x = - bEq + signOfSolution * sqrt(bEq * bEq - cEq);
          y = (- c - a * x) / b;
        }
        (*ith)->setGlobalPosition(x, y);
        // Compute the sine of the angle between hit and R zone centre.
        const double dSin = (y * cphi - x * sphi) / rPred;
        (*ith)->setGlobalDistance(dSin);
        if (m_debug && matchKey(*ith)) {
          // const double xMc = m_debugTool->xTrue(m_wantedKey, sensor->z());
          // const double yMc = m_debugTool->yTrue(m_wantedKey, sensor->z());
          // const double rMc = sqrt(xMc * xMc + yMc * yMc);
          // const double dMc = (*ith)->distance(xMc, yMc);
          // debug() << format("    x%7.3f y%7.3f R%7.3f dSin%7.3f",
          //                   x, y, sqrt(x * x + y * y), dSin) << endmsg;
          // debug() << format("MC: x%7.3f y%7.3f R%7.3f dist%7.3f", 
          //                   xMc, yMc, rMc, dMc) << endmsg;
        }
        if (fabs(dSin) > maxDSin) continue;
        if (m_debug && matchKey(*ith)) {
          debug() << "    Passed angular cut." << endmsg;
        }
        (*ith)->setZ(sensor->z(x, y));
        (*ith)->setPhiWeight(rPred);
        phiHits[station].push_back(*ith);
      }
    }
  }
  unsigned int firstStation = 0;
  unsigned int lastStation = nStations - 1;
  for (unsigned int station = 0; station < nStations; ++station) {
    if (phiHits[station].empty()) {
      if (station == firstStation) ++firstStation;
    } else {
      lastStation = station;
      if (phiHits[station].size() > 1) {
        std::sort(phiHits[station].begin(), phiHits[station].end(),
                  PrVLHit::IncreasingByGlobalDistance());
      }
    }
  }
  int stationStep = 1;
  if (forward) {
    unsigned int tmp = firstStation;
    firstStation = lastStation;
    lastStation = tmp;
    stationStep = -1;
  }
  // Determine the min. number of phi hits required for a track.
  unsigned int minNbPhi = abs(int(lastStation) - int(firstStation)) + 1;
  if (minNbPhi > seed.rHits().size()) minNbPhi = seed.rHits().size();
  if (inOverlap) {
    if (nLeftR > nRightR) {
      minNbPhi = nLeftR;
    } else {
      minNbPhi = nRightR;
    }
  }
  minNbPhi = int(m_fractionFound * minNbPhi);
  if (minNbPhi < 3) minNbPhi = 3;
  if (m_debug) {
    debug() << "Space search from station " << firstStation 
            << " to " << lastStation << endmsg;
    debug() << "Require " << minNbPhi << " phi hits on track" << endmsg;
  }
  PrVLTracks newTracks;
  // Search for triplets with low d(global) / dz
  unsigned int s1 = firstStation;
  unsigned int s2 = s1 + stationStep;
  unsigned int s3 = s2 + stationStep;
  bool hasFullLength = false;
  for (int iCase = 0; iCase < 5; ++iCase) {
    if (2 == iCase) s3 += stationStep;
    if (3 == iCase) s2 += stationStep;
    if (4 == iCase) s1 += stationStep;
    // Protect against accessing non-existent modules.
    if (s1 >= nStations || s2 >= nStations || s3 >= nStations) continue;
    PrVLHits::iterator ith1; 
    for (ith1 = phiHits[s1].begin(); ith1 != phiHits[s1].end(); ++ith1) {
      if (0 == iCase && 0 != (*ith1)->nUsed()) continue;
      const double global1 = (*ith1)->dGlobal();
      const double x1 = (*ith1)->xGlobal();
      const double y1 = (*ith1)->yGlobal();
      const double z1 = (*ith1)->z();
      double rPred = seed.rInterpolated(z1);
      PrVLHits::iterator ith2;
      for (ith2 = phiHits[s2].begin(); ith2 != phiHits[s2].end(); ++ith2) {
        if (0 == iCase && 0 != (*ith2)->nUsed()) continue;
        const double global2 = (*ith2)->dGlobal();
        const double z2 = (*ith2)->z();
        const double d2 = fabs((global2 - global1) * rPred / (z2 - z1));
        if (d2 > m_d2Max) continue;
        if (m_debug) {
          debug() << "    Try pair with d2 = " << d2 << endmsg;
          printHit(*ith1, "      S1 ");
          printHit(*ith2, "      S2 ");
        }
        // Check if this pair is already on an existing candidate.
        bool reject = false;
        PrVLTracks::iterator itt;
        for (itt = newTracks.begin(); itt != newTracks.end(); ++itt) {
          if (std::find((*itt).phiHits().begin(), (*itt).phiHits().end(), 
                        *ith1) != (*itt).phiHits().end() &&
              std::find((*itt).phiHits().begin(), (*itt).phiHits().end(), 
                        *ith2) != (*itt).phiHits().end()) {
            reject = true;
            break;
          }
        }
        if (reject) continue;
        const double x2 = (*ith2)->xGlobal();
        const double y2 = (*ith2)->yGlobal();
        const double dz = z1 - z2;
        const double tx = (x1 - x2) / dz;
        const double ty = (y1 - y2) / dz;
        const double x0 = x1 - tx * z1;
        const double y0 = y1 - ty * z1;

        double minDelta3 = 1.e9;
        PrVLHit* best3 = 0;
        PrVLHits::iterator ith3;
        for (ith3 = phiHits[s3].begin(); ith3 != phiHits[s3].end(); ++ith3) {
          if (0 == iCase && 0 != (*ith3)->nUsed()) continue;
          // Check if already tried in iCase = 0.
          if (1 == iCase && 
              0 == (*ith1)->nUsed() &&
              0 == (*ith2)->nUsed() &&
              0 == (*ith3)->nUsed()) continue; 
          const double z3 = (*ith3)->z();
          const double d3 = (*ith3)->chi2(x0 + z3 * tx, y0 + z3 * ty);
          if (d3 < minDelta3) {
            minDelta3 = d3;
            best3 = *ith3;
          }
        }
        if (minDelta3 > 4 * m_maxChi2ToAdd) continue;
        if (m_debug) {
          debug() << "    ==== triplet with minDelta3 = " 
                  << minDelta3 << " ====" << endmsg;
          printHit(*ith1, "    S1 ");
          printHit(*ith2, "    S2 ");
          printHit(best3, "    S3 ");
        }
        // Update z of the R sensors
        PrVLHits::iterator itr;
        for (itr = seed.rHits().begin(); itr != seed.rHits().end(); ++itr) {
          const DeVLRSensor* rsensor = m_det->rSensor((*itr)->sensor());
          const double xr = x0 + tx * rsensor->z();
          const double yr = y0 + ty * rsensor->z();
          (*itr)->setZ(rsensor->z(xr, yr));
        }
        PrVLTrack trk;
        trk.setPhiClusters(seed, x0, tx, y0, ty, *ith1, *ith2, best3);
        if (!trk.removeWorstMultiple(m_maxChi2PerHit, 3)) {
          if (m_debug) {
            debug() << "    Reject (not enough good phi hits)" << endmsg;
          }
          continue;
        }
        if (inOverlap) {
          addOtherSidePhiHit(phiHits[s1], trk, (*ith1)->sensor());
          addOtherSidePhiHit(phiHits[s2], trk, (*ith2)->sensor());
          addOtherSidePhiHit(phiHits[s3], trk, best3->sensor());
          if (!trk.removeWorstMultiple(m_maxChi2PerHit, 3)) {
            if (m_debug) {
              debug() << "    Reject (not enough good phi hits "
                      << "after adding other side)" << endmsg;
            }
            // After adding other side: not enough good phi
            continue;
          }
        }
        trk.updateRParameters();

        int nbMissed = 0;
        double lastZ = best3->z();
        for (unsigned int s = s3 + stationStep; s != lastStation + stationStep; s += stationStep) {
          if (phiHits[s].size() > 0) {
            // Try to add more on this station.
            double addChi2 = m_maxChi2ToAdd;
            // TODO: check 140 (big gap?)
            if (fabs(phiHits[s].front()->z() - lastZ) > 140.) addChi2 *= 4;
            double bestChi2 = 1.e9;
            PrVLHit* best = 0;
            PrVLHits::const_iterator itp;
            for (itp = phiHits[s].begin(); itp != phiHits[s].end(); ++itp) {
              const double c2 = trk.chi2(*itp);
              if (c2 < bestChi2) {
                best = *itp;
                bestChi2 = c2;
              }
            }
            if (best) {
              if (!trk.addPhiCluster(best, addChi2)) {
                 ++nbMissed;
              } else {
                if (m_debug) printHit(best, "    +  ");
                if (inOverlap) {
                  addOtherSidePhiHit(phiHits[s], trk, best->sensor());
                }
              } 
            } else {
              ++nbMissed;
            }
          } else {
            ++nbMissed;
          }
          if (nbMissed > 2) break;
        }
        trk.updateRParameters();
        if (!trk.removeWorstMultiple(m_maxChi2PerHit, minNbPhi)) {
          if (m_debug) {
            debug() << "    Reject (not enough good phi hits)" << endmsg;
          }
          continue;
        }
        // Check the q factor.
        if (trk.qFactor() > m_maxQFactor) {
          if (m_debug) {
            debug() << "    Reject (q-factor = " << trk.qFactor() 
                    << ")" << endmsg;
          }
          continue;
        }
          
        // Check that there are phi hits on the same side as the R hits.
        int nLeftPhi = 0, nRightPhi = 0;
        PrVLHits::iterator itp;
        for (itp = trk.phiHits().begin(); itp != trk.phiHits().end(); ++itp) {
          if (m_det->sensor((*itp)->sensor())->isRight()) {
            ++nRightPhi;
          } else {
            ++nLeftPhi;
          }
        }
        if (0 == nLeftR * nLeftPhi + nRightR * nRightPhi) {
          if (m_debug) {
            debug() << "    Reject (Phi and R hits all on opposite sides)" 
                    << endmsg;
          }
          continue;
        }
        if (m_debug) {
          debug() << "    ==== accepted q-factor = " 
                  << trk.qFactor() << " ====" << endmsg;
        }
        newTracks.push_back(trk);
        if (trk.phiHits().size() >= trk.rHits().size()) {
          hasFullLength = true;
        }
      }
    }
    if (iCase > 0 && hasFullLength) break;
  }
  // If no track found AND in overlap: try all hits first
  if (inOverlap && newTracks.empty()) {
    s1 = firstStation;
    s2 = s1 + stationStep;
    s3 = s2 + stationStep;
    if (s1 < nStations && s2 < nStations && s3 < nStations) {
      PrVLHits all(phiHits[s1]);
      unsigned int station = s2;
      while (station != lastStation + stationStep) {
        for (ith = phiHits[station].begin(); ith != phiHits[station].end(); ++ith) {
          all.push_back(*ith);
        }
        station += stationStep;
        if (station >= nStations) break;
      }
      PrVLTrack trk;
      trk.setPhiClusters(seed, cphi, sphi, all.begin(), all.end());
      if (trk.removeWorstMultiple(m_maxChi2PerHit, minNbPhi)) {
        // Overall quality should be good enough.
        if (trk.qFactor() < m_maxQFactor) newTracks.push_back(trk);
      }
    }
  }
  
  // Last chance: this is a fake overlap. Split.
  if (inOverlap && newTracks.empty()) {
    PrVLHits other;
    for (ith = seed.rHits().begin(); ith != seed.rHits().end(); ++ith) {
      if ((*ith)->zone() != rZone) {
        other.push_back(*ith);
        ith = seed.rHits().erase(ith);
        ith--;
      }
    }
    // Try with the hits from the same zone.
    if (m_debug) {
      debug() << "Try with hits from main zone" << endmsg;
    }
    findSpaceTracks(seed);
    // Try with the hits from the other zone.
    seed.rHits() = other;
    seed.setZone(2 * m_nZonesR - 1 - seed.zone());
    if (m_debug) {
      debug() << "Try with hits from other zone" << endmsg;
    }
    findSpaceTracks(seed);
    return;
  }

  // Merge tracks if necessary.
  if (newTracks.size() > 1) mergeClones(newTracks);

  double maxQual = 1.e10;
  unsigned int maxLen = 0;
  // Get the properties of the best candidate. 
  // Invalidate tracks with 3 or 4 clusters and all already used.
  PrVLTracks::iterator itt;
  for (itt = newTracks.begin(); itt != newTracks.end(); ++itt) {
    if (!(*itt).valid()) continue;
    if ((*itt).phiHits().size() < 4 &&
        0 == (*itt).nbUnusedPhiHits()) {
      (*itt).setValid(false);
      continue;
    }
    if ((*itt).phiHits().size() >= maxLen) maxLen = (*itt).phiHits().size();
  }
  unsigned int minExpected = maxLen;
  if (maxLen > 4) --minExpected;
  for (itt = newTracks.begin(); itt != newTracks.end(); ++itt) {
    if (!(*itt).valid()) continue;
    if ((*itt).phiHits().size() >= minExpected) {
      if (maxQual > (*itt).qFactor()) maxQual = (*itt).qFactor();
    }
  }
  if (maxLen > 3) maxQual += m_deltaQuality;

  // Store the good candidates
  bool foundSpaceTrack = false;
  for (itt = newTracks.begin(); itt != newTracks.end(); ++itt) {
    if (!(*itt).valid()) continue;
    if ((*itt).phiHits().size() < minExpected || (*itt).qFactor() > maxQual) {
      continue;
    }
    // Stronger test on short tracks (3 + 3 hits).
    if (3 == (*itt).rHits().size() && 3 == (*itt).phiHits().size()) {
      if ((*itt).qFactor() > m_maxQFactor3) continue;
    }
    if (!(*itt).removeWorstRAndPhi(m_maxChi2PerHit, 6)) {
      // Less than three R or Phi hits after cleanup.
      continue;
    }
    (*itt).tagUsedPhiHits();
    (*itt).setBackward(seed.backward());
    m_spaceTracks.push_back(*itt);
    foundSpaceTrack = true;
  }
  if (!foundSpaceTrack && seed.nbRHits() >= m_minToTag) {
    seed.untagUsedRHits();
  }

}

//============================================================================
///  Merge tracks sharing hits, tag clones.
//============================================================================
void PrVLTracking::mergeClones(PrVLTracks& tracks) {

  PrVLTracks::iterator itt1;
  PrVLTracks::iterator itt2;
  for (itt1 = tracks.begin(); itt1 != tracks.end() - 1; ++itt1) {
    const unsigned int n1 = (*itt1).phiHits().size();
    for (itt2 = itt1 + 1; itt2 != tracks.end(); ++itt2) {
      if (!(*itt2).valid()) continue;
      const unsigned int n2 = (*itt2).phiHits().size();
      const unsigned int nMinCommon = n1 > n2 ? n2 : n1;
      unsigned int nCommon = 0;
      PrVLHits::const_iterator ith;
      for (ith = (*itt1).phiHits().begin(); ith != (*itt1).phiHits().end(); ++ith) {
        if (std::find((*itt2).phiHits().begin(), (*itt2).phiHits().end(), 
                      *ith) != (*itt2).phiHits().end()) {
          ++nCommon;
        }
      }
      if (m_debug) {
        debug() << "Tracks " << itt1 - tracks.begin() 
                << " (" << n1 << " hits, q-factor " << (*itt1).qFactor()
                << ") and " << itt2 - tracks.begin() 
                << " (" << n2 << " hits, q-factor " << (*itt2).qFactor()
                << ") share " << nCommon << " Phi hits (clone criterion: " 
                << nMinCommon * m_fractionForMerge << ")" << endmsg;
      } 
      if (nCommon < nMinCommon * m_fractionForMerge) continue;
      if (n2 > n1) {
        if (m_debug) {
          debug() << "Invalidating track " << itt1 - tracks.begin() << endmsg; 
        }
        (*itt1).setValid(false);
      } else if (n1 > n2) {
        (*itt2).setValid(false);
        if (m_debug) {
          debug() << "Invalidating track " << itt1 - tracks.begin() << endmsg; 
        }
      } else if ((*itt1).qFactor() < (*itt2).qFactor()) {
        if (m_debug) {
          debug() << "Invalidating track " << itt2 - tracks.begin() << endmsg;
        }
        (*itt2).setValid(false);
      } else {
        if (m_debug) {
          debug() << "Invalidating track " << itt1 - tracks.begin() << endmsg; 
        }
        (*itt1).setValid(false);
      }
    }
  }

}

//============================================================================
///  Merge space track clones
//============================================================================
void PrVLTracking::mergeSpaceClones() {

  PrVLTracks::iterator it1;
  PrVLTracks::iterator it2;
  for (it1 = m_spaceTracks.begin(); it1 != m_spaceTracks.end() - 1; ++it1) {
    if ((*it1).backward()) continue;
    if (!(*it1).valid()) continue;
    std::vector<int> strip1(128, -1);
    PrVLHits::const_iterator ith;
    for (ith = (*it1).rHits().begin(); ith != (*it1).rHits().end(); ++ith) {
      const int sensor = (*ith)->sensor();
      const int strip = (*ith)->cluster().channelID().strip();
      if (sensor < 128) strip1[sensor] = strip;
    }
    for (ith = (*it1).phiHits().begin(); ith != (*it1).phiHits().end(); ++ith) {
      const int sensor = (*ith)->sensor();
      const int strip = (*ith)->cluster().channelID().strip();
      if (sensor < 128) strip1[sensor] = strip;
    }
    const double zMid = 0.5 * ((*it1).rHits().front()->z() + 
                               (*it1).rHits().back()->z());
    const double slx = 0.01 * ((*it1).xAtZ(zMid + 100.) - (*it1).xAtZ(zMid));
    const double sly = 0.01 * ((*it1).yAtZ(zMid + 100.) - (*it1).yAtZ(zMid));

    for (it2 = it1 + 1; it2 != m_spaceTracks.end(); ++it2) {
      if ((*it2).backward()) continue;
      if (!(*it2).valid()) continue;
 
      double slx2 = 0.01 * ((*it2).xAtZ(zMid + 100.) - (*it2).xAtZ(zMid));
      double sly2 = 0.01 * ((*it2).yAtZ(zMid + 100.) - (*it2).yAtZ(zMid));

      double dx = (*it1).xAtZ(0.) - (*it2).xAtZ(0.);
      double dy = (*it1).yAtZ(0.) - (*it2).yAtZ(0.);
      double dslx = slx - slx2;
      double dsly = sly - sly2;
      if (fabs(dslx) > m_maxDeltaSlopeToMerge || 
          fabs(dsly) > m_maxDeltaSlopeToMerge) {
        continue;
      }
      double zC = -(dx * dslx + dy * dsly) / (dslx * dslx + dsly * dsly); 
      if (fabs((*it1).xAtZ(zC) - (*it2).xAtZ(zC)) > m_maxDistToMerge) continue;
      if (fabs((*it1).yAtZ(zC) - (*it2).yAtZ(zC)) > m_maxDistToMerge) continue;

      std::vector<int> strip2(128, -1);
      for (ith = (*it2).rHits().begin(); ith != (*it2).rHits().end(); ++ith) {
        const int sensor = (*ith)->sensor();
        const int strip  = (*ith)->cluster().channelID().strip();
        if (sensor < 128) strip2[sensor] = strip;
      }
      for (ith = (*it2).phiHits().begin(); ith != (*it2).phiHits().end(); ++ith) {
        const int sensor = (*ith)->sensor();
        const int strip  = (*ith)->cluster().channelID().strip();
        if (sensor < 128) strip2[sensor] = strip;
      }

      // Count how many sensors have hits on both, and how many are in commmon.
      int nRInBoth = 0;
      int nRCommon = 0;
      for (int k = 0; k < 64; ++k) {
        if (strip1[k] >= 0 && strip2[k] >= 0) {
          ++nRInBoth;
          if (strip1[k] == strip2[k]) ++nRCommon;
        }
      }
      int nPhiInBoth = 0;
      int nPhiCommon = 0;
      for (int k = 64; k < 128; ++k) {
        if (strip1[k] >= 0 && strip2[k] >= 0) {
          ++nPhiInBoth;
          if (strip1[k] == strip2[k]) ++nPhiCommon;
        }
      }

      if (m_debug) {
        debug() << nRInBoth << " sensors with R hits on both, " 
                << nRCommon << " with same cluster" << endmsg;
        debug() << nPhiInBoth << " sensors with Phi hits on both, " 
                << nPhiCommon << " with same cluster" << endmsg;
      }

      if ((0 == nRInBoth || nRCommon > 4 * nRInBoth) &&
          (0 == nPhiInBoth || nPhiCommon > 4 * nPhiInBoth)) {
        if (m_debug) info() << " === merge ===" << endmsg;
        for (ith = (*it2).rHits().begin(); ith != (*it2).rHits().end(); ++ith) {
          if (strip1[(*ith)->sensor()] < 0) (*it1).addRHit(*ith);
        }
        for (ith = (*it2).phiHits().begin(); ith != (*it2).phiHits().end(); ++ith) {
          if (strip1[(*ith)->sensor()] < 0) (*it1).addPhiCluster(*ith, m_maxChi2ToAdd);
        }
        std::sort((*it1).rHits().begin(), (*it1).rHits().end(),
                  PrVLHit::DecreasingByZ());
        std::sort((*it1).phiHits().begin(), (*it1).phiHits().end(), 
                  PrVLHit::DecreasingByZ());
        (*it1).updateRParameters();
        (*it1).updatePhiWeights();
        (*it1).fitTrack();
        (*it1).removeWorstRAndPhi(m_maxChi2PerHit, 6);
        (*it2).setValid(false);
      }     
    }
  }

}

//=========================================================================
///  Convert the local track to LHCb tracks
//=========================================================================
void PrVLTracking::makeLHCbTracks(Tracks* lhcbTracks) {

  PrVLTracks::iterator itt;
  for (itt = m_spaceTracks.begin(); itt != m_spaceTracks.end(); ++itt) {
    if ((*itt).rHits().size() < 3 || (*itt).phiHits().size() < 3) {
      (*itt).setValid(false);
    }
    if (!(*itt).valid()) continue; 
    LHCb::Track* newTrack = new LHCb::Track();
    newTrack->setType(LHCb::Track::Velo);
    newTrack->setHistory(LHCb::Track::PatFastVelo);
    newTrack->setPatRecStatus(LHCb::Track::PatRecIDs);
    double zMin =  1.e9;
    double zMax = -1.e9;
    PrVLHits::iterator ith;
    for (ith = (*itt).rHits().begin(); ith != (*itt).rHits().end(); ++ith) {
      newTrack->addToLhcbIDs((*ith)->lhcbID());
      const double z = (*ith)->z();
      if (z > zMax) zMax = z;
      if (z < zMin) zMin = z;
    }
    for (ith = (*itt).phiHits().begin(); ith != (*itt).phiHits().end(); ++ith) {
      newTrack->addToLhcbIDs((*ith)->lhcbID());
      const double z = (*ith)->z();
      if (z > zMax) zMax = z;
      if (z < zMin) zMin = z;
    }

    // Define backward as z closest to beam downstream of hits
    const double zBeam = (*itt).zBeam();
    const bool backward = zBeam > zMax;
    newTrack->setFlag(LHCb::Track::Backward, backward);
    LHCb::State state;
    state.setLocation(LHCb::State::ClosestToBeam);
    state.setState((*itt).state(zBeam));
    state.setCovariance((*itt).covariance(zBeam));
    newTrack->addToStates(state);
    if (!backward) {
      state.setLocation(LHCb::State::EndVelo);
      state.setState((*itt).state(zMax));
      state.setCovariance((*itt).covariance(zMax));
      newTrack->addToStates(state);
    }
    lhcbTracks->insert(newTrack);
  }
  
}

//============================================================================
/// Try to find tracks with unused phi clusters, only forward
//============================================================================
void PrVLTracking::findUnusedPhi() {

  if (m_debug) debug() << " ==> findUnusedPhi()" << endmsg;
  double tol1 = m_phiUnusedFirstTol;
  double tol2 = m_phiUnusedSecondTol;
  double maxQFactor = m_maxQFactor;
  unsigned int firstSensor = m_det->phiSensors().back()->sensorNumber();
  unsigned int lastSensor = firstSensor - 15;
  if (m_rzTracks.size() < 10) {
    // Clean event (e.g. Velo open)
    tol1 = tol2 = 20.;
    maxQFactor = 2.;
    lastSensor = firstSensor - 30;
  }

  PrVLHits::iterator ith0;
  PrVLHits::iterator ith1;
  PrVLHits::iterator ith2;
  PrVLHits::iterator itr0;
  for (unsigned int p0 = firstSensor; p0 >= lastSensor; --p0) {
    const DeVLPhiSensor* phiSensor0 = m_det->phiSensor(p0);
    const DeVLPhiSensor* phiSensor1 = phiSensor0->previousPhiSensor();
    const DeVLPhiSensor* phiSensor2 = phiSensor1->previousPhiSensor();
    const unsigned int p1 = phiSensor1->sensorNumber();
    const unsigned int p2 = phiSensor2->sensorNumber();
    const DeVLRSensor* rSensor0 = phiSensor0->associatedRSensor();
    const DeVLRSensor* rSensor1 = phiSensor1->associatedRSensor();
    const DeVLRSensor* rSensor2 = phiSensor2->associatedRSensor();
    const unsigned int r0 = rSensor0->sensorNumber();
    const unsigned int r1 = rSensor1->sensorNumber();
    const unsigned int r2 = rSensor2->sensorNumber();
    for (unsigned int phiZone0 = 0; phiZone0 < m_nZonesPhi; ++phiZone0) {
      PrVLHits& hits0 = m_hits[p0][phiZone0]; 
      for (ith0 = hits0.begin(); ith0 != hits0.end(); ++ith0) {
        if (0 != (*ith0)->nUsed()) continue;
        const double x0 = (*ith0)->xStripCentre();
        const double y0 = (*ith0)->yStripCentre();
        for (unsigned int phiZone1 = 0; phiZone1 <= phiZone0; ++phiZone1) {
          PrVLHits& hits1 = m_hits[p1][phiZone1];
          for (ith1 = hits1.begin(); ith1 != hits1.end(); ++ith1) {
            if (0 != (*ith1)->nUsed()) continue;
            if (fabs((*ith1)->distance(x0, y0)) > tol1) continue;
            const double xSeed = 0.5 * (x0 + (*ith1)->xStripCentre());
            const double ySeed = 0.5 * (y0 + (*ith1)->yStripCentre());
            for (unsigned int phiZone2 = 0; phiZone2 <= phiZone1; ++phiZone2) {
              PrVLHits& hits2 = m_hits[p2][phiZone2];
              for (ith2 = hits2.begin(); ith2 != hits2.end(); ++ith2) {
                if (0 != (*ith2)->nUsed()) continue;
                if (fabs((*ith2)->distance(xSeed, ySeed)) > tol2) continue;
                // Determine the zone in R sensor zone.
                Gaudi::XYZPoint point0(xSeed, ySeed, rSensor0->z());
                point0 = rSensor0->globalToLocal(point0);
                unsigned int rZone0 = rSensor0->zoneOfPhiAndR(point0.phi(), 
                                                              point0.Rho());
                PrVLHits& rHits0 = m_hits[r0][rZone0];
                int spaceZone = rZone0;
                if (xSeed < 0) spaceZone += m_nZonesR;
                for (itr0 = rHits0.begin(); itr0 != rHits0.end(); ++itr0) {
                  if ((*itr0)->rLocal() < phiSensor0->rMin(phiZone0) ||
                      (*itr0)->rLocal() > phiSensor0->rMax(phiZone0)) {
                    continue;
                  }
                  (*itr0)->setStartingPoint(xSeed, ySeed);
                  int nbUnused = 0;
                  if (0 == (*itr0)->nUsed()) ++nbUnused;
                  PrVLTrack trk;
                  trk.setPhiClusters(*itr0, spaceZone, *ith0, *ith1, *ith2);
                  trk.updatePhiWeights();
                  const double z1 = rSensor1->z();
                  const double x1 = trk.xAtZ(z1) - rSensor1->xCentre();
                  const double y1 = trk.yAtZ(z1) - rSensor1->yCentre();
                  Gaudi::XYZPoint point1(x1, y1, z1);
                  point1 = rSensor1->globalToLocal(point1);
                  unsigned int rZone1 = rSensor1->zoneOfPhiAndR(point1.phi(),
                                                                point1.Rho());
                  PrVLHits& rHits1 = m_hits[r1][rZone1]; 
                  // TODO: why 400 and not m_maxChi2ToAdd
                  if (!trk.addBestRCluster(rHits1, z1, 400.)) continue;
                  trk.updatePhiWeights();
                  if (0 == trk.rHits().back()->nUsed()) ++nbUnused;
                  const double z2 = rSensor2->z();
                  const double x2 = trk.xAtZ(z2) - rSensor2->xCentre();
                  const double y2 = trk.yAtZ(z2) - rSensor2->yCentre();
                  Gaudi::XYZPoint point2(x2, y2, z2);
                  point2 = rSensor2->globalToLocal(point2);
                  unsigned int rZone2 = rSensor2->zoneOfPhiAndR(point2.phi(),
                                                                point2.Rho());
                  PrVLHits& rHits2 = m_hits[r2][rZone2]; 
                  if (!trk.addBestRCluster(rHits2, z2, m_maxChi2ToAdd)) continue;
                  if (0 == trk.rHits().back()->nUsed()) ++nbUnused;
                  if (nbUnused < 2) continue;
                  if (trk.qFactor() > maxQFactor) continue;
                  const DeVLRSensor* rSensor = rSensor2;
                  int nMiss = 0;
                  while (rSensor && nMiss <= m_maxMissed) {
                    rSensor = rSensor->previousRSensor();
                    if (!rSensor) break;
                    const double z = rSensor->z();
                    if (trk.rAtZ(z) < rSensor->rMin(rZone0)) break;
                    const double x = trk.xAtZ(z) - rSensor->xCentre();
                    const double y = trk.yAtZ(z) - rSensor->yCentre();
                    Gaudi::XYZPoint point(x, y, z);
                    point = rSensor->globalToLocal(point);
                    unsigned int rZone = rSensor->zoneOfPhiAndR(point.phi(),
                                                                point.Rho());
                    PrVLHits& rHits = m_hits[rSensor->sensorNumber()][rZone]; 
                    if (!trk.addBestRCluster(rHits, z, m_maxChi2ToAdd)) ++nMiss;
                    const DeVLPhiSensor* phiSensor = rSensor->associatedPhiSensor();
                    const double rInPhi = trk.rAtZ(phiSensor->z());
                    const unsigned int phiZone = phiSensor->zoneOfRadius(rInPhi); 
                    PrVLHits& phiHits = m_hits[phiSensor->sensorNumber()][phiZone];
                    PrVLHits::iterator itp;
                    for (itp = phiHits.begin(); itp != phiHits.end(); ++itp) {
                      (*itp)->setPhiWeight(rInPhi);
                    }
                    if (!addPhiHit(phiHits, trk)) ++nMiss;
                  }
                  if (!trk.removeWorstRAndPhi(m_maxChi2PerHit, 6)) continue;
                  if (m_debug) {
                    debug() << "    ==== accepted q-factor = " 
                            << trk.qFactor() << " ====" << endmsg;
                  }
                  trk.tagUsedRHits();
                  trk.tagUsedPhiHits();
                  m_spaceTracks.push_back(trk);
                }
              }
            }
          }
        }
      }
    }
  }
 
}

//============================================================================
/// Get the clusters and their coordinates
//============================================================================
void PrVLTracking::buildHits() {

  if (m_debug) debug() << " ==> buildHits()" << endmsg;
  // Reset the hit map.
  for (unsigned int i = 0; i < m_hits.size(); ++i) {
    if (!m_det->sensor(i)) continue;
    if (m_det->sensor(i)->isPhi()) {
      for (unsigned int j = 0; j < m_nZonesPhi; ++j) {
        m_hits[i][j].clear();
      }
    } else {
      for (unsigned int j = 0; j < m_nZonesR; ++j) {
        m_hits[i][j].clear();
      }
    }
  }
  m_nextInPool = m_pool.begin();

  // Get the clusters.
  VLLiteCluster::VLLiteClusters* liteClusters = get<VLLiteCluster::VLLiteClusters>(VLLiteClusterLocation::Default);
  if (m_debug) {
    debug() << "Retrieved " << liteClusters->size() << " clusters" << endmsg;
  }
  if (liteClusters->size() > m_pool.size()) m_pool.resize(liteClusters->size() + 100);
  VLLiteCluster::VLLiteClusters::iterator it;
  for (it = liteClusters->begin(); it != liteClusters->end(); ++it) {
    // Get the next object in the pool.
    PrVLHit* hit = &(*(m_nextInPool++));
    VLChannelID channel = (*it).channelID();
    const unsigned int sensor = channel.sensor(); 
    const unsigned int strip = channel.strip();
    double frac = (*it).interStripFraction();
    unsigned int zone = 0;
    if (channel.isPhiType()) {
      const DeVLPhiSensor* phiSensor = m_det->phiSensor(sensor);
      zone = phiSensor->globalZoneOfStrip(strip);
      const double pitch = phiSensor->phiPitchOfStrip(strip);
      double weight = 12. / (pitch * pitch);
      hit->setHit(*it, zone, phiSensor->z(), 0, weight);
      double a, b, c;
      double xs, ys;
      if (frac > 0.5) {
        phiSensor->lineParameters(strip + 1, a, b, c, xs, ys);
      } else {
        phiSensor->lineParameters(strip, a, b, c, xs, ys);
      }
      hit->setLineParameters(a, b, c, xs, ys);
    } else if (channel.isRType()) {
      const DeVLRSensor* rSensor = m_det->rSensor(sensor);
      zone = rSensor->globalZoneOfStrip(strip);
      const double radius = rSensor->rOfStrip(strip, frac);
      double pitch = rSensor->rPitchOfStrip(strip);
      if (frac > 0.) {
        pitch = frac * pitch + (1. - frac) * rSensor->rPitchOfStrip(strip + 1);
      }
      const double weight = 12. / (pitch * pitch);
      hit->setHit(*it, zone, rSensor->z(), radius, weight);
      hit->setGlobalRadius(radius);
    } 
    m_hits[sensor][zone].push_back(hit);
  }

  if (!m_debug) return;
  const std::vector<DeVLRSensor*>& rSensors = m_det->rSensors();
  const std::vector<DeVLPhiSensor*>& phiSensors = m_det->phiSensors();
  if (m_debug && m_wantedKey >= 0) {
    // Print the hits associated to the requested track.
    debug() << "R hits on track " << m_wantedKey << endmsg;
    std::vector<DeVLRSensor*>::const_iterator itr;
    for (itr = rSensors.begin(); itr != rSensors.end(); ++itr) {
      const unsigned int i = (*itr)->sensorNumber();
      for (unsigned int j = 0; j < m_nZonesR; ++j) {
        PrVLHits::iterator ith;
        for (ith = m_hits[i][j].begin(); ith != m_hits[i][j].end(); ++ith) {
          if (matchKey(*ith)) printHit(*ith, "");
        }
      }
    }
    debug() << "Phi hits on track " << m_wantedKey << endmsg;
    std::vector<DeVLPhiSensor*>::const_iterator itp;
    for (itp = phiSensors.begin(); itp != phiSensors.end(); ++itp) {
      const unsigned int i = (*itp)->sensorNumber();
      for (unsigned int j = 0; j < m_nZonesPhi; ++j) {
        PrVLHits::iterator ith;
        for (ith = m_hits[i][j].begin(); ith != m_hits[i][j].end(); ++ith) {
          if (matchKey(*ith)) printHit(*ith, "");
        }
      }
    }
  } 

}

//============================================================================
/// Extend a track towards the vertex
//============================================================================
int PrVLTracking::extendTrack(PrVLTrack& track, unsigned int sensor,
                              const unsigned int zone, const bool forward,
                              const bool unused) {

  // Extrapolate until too many consecutive hits missed.
  int nMiss = 0;
  const DeVLRSensor* rSensor = m_det->rSensor(sensor);
  while (nMiss <= m_maxMissed) {
    if (forward) {
      rSensor = rSensor->previousRSensor();
    } else {
      rSensor = rSensor->nextRSensor();
    }
    if (!rSensor) break;
    sensor = rSensor->sensorNumber();
    const double z = rSensor->z();
    // Calculate the extrapolated radius.
    const double rPred = track.rPred(z);
    if (rPred < rSensor->rMin(zone) ||
        rPred > rSensor->rMax(zone)) {
      // Track intercept is outside the zone.
      break;
    }
    const double rPitch = rSensor->rPitchOfRadius(rPred);
    double tol = m_rTolExtra * sqrt(track.rErr2(z)) + rPitch;
    // Check if the sensor has a matching hit.
    PrVLHit* best = closestHit(m_hits[sensor][zone], rPred, tol, unused);
    if (!best) {
      // No matching hit.
      ++nMiss;
      continue;
    }
    nMiss = 0;
    track.addRHit(best);
    if (m_debug) printHit(best, "    +  ");
  }
  return nMiss;
  
}

//============================================================================
/// Add hits from the other side of the detector (overlap tracks)
//============================================================================
void PrVLTracking::extendTrackOtherSide(PrVLTrack& track, unsigned int sensor,
                                        unsigned int zone, const bool forward,
                                        const bool unused) {
   
  // Get the R sensor on the opposite side.
  const DeVLRSensor* rSensor = m_det->rSensor(sensor)->otherSideRSensor();
  // Since zones are ordered in global phi, we have to flip.
  zone = m_nZonesR - 1 - zone;
  // Extrapolate until too many consecutive hits missed.
  PrVLHits extraHits;
  while (rSensor) {
    sensor = rSensor->sensorNumber();
    // Calculate the extrapolated radius.
    const double rPred = track.rPred(rSensor->z());
    if (rPred < rSensor->rMin(zone) || rPred > rSensor->rMax(zone)) {
      // Track intercept is outside the zone.
      break;
    }
    const double rPitch = rSensor->rPitchOfRadius(rPred);
    const double tol = m_rTolOverlap * rPitch;
    // Check if the sensor has a matching hit.
    PrVLHit* best = closestHit(m_hits[sensor][zone], rPred, tol, unused);
    if (best) {
      extraHits.push_back(best);
    }
    if (forward) {
      rSensor = rSensor->previousRSensor();
    } else {
      rSensor = rSensor->nextRSensor();
    }
  }
  if (extraHits.size() > 2) {
    PrVLHits::const_iterator ith;
    for (ith = extraHits.begin(); ith != extraHits.end(); ++ith) {
      track.addRHit(*ith);
      if (m_debug) printHit(*ith, "   ++  ");
    }
  }

}

//============================================================================
/// Find the best phi cluster from a list of hits.
//============================================================================
bool PrVLTracking::addPhiHit(PrVLHits& hits, PrVLTrack& track) {

  double bestChi2 = 1.e9;
  PrVLHit* best = 0;
  PrVLHits::const_iterator it;
  for (it = hits.begin(); it != hits.end(); ++it) {
    const double chi2 = track.chi2(*it);
    if (chi2 < bestChi2) {
      bestChi2 = chi2;
      best = *it;
    }
  }
  if (bestChi2 < m_maxChi2ToAdd) {
    track.addPhiCluster(best);
    if (m_debug) printHit(best, "   ++  ");
    return true;
  } 
  return false;

}

//============================================================================
/// Find the best phi cluster on the other sensor of the same station.
//============================================================================
bool PrVLTracking::addOtherSidePhiHit(PrVLHits& hits, PrVLTrack& track,
                                      unsigned int sensor) {

  PrVLHits::const_iterator it;
  PrVLHit* best = 0;
  double bestChi2 = 1.e9;
  for (it = hits.begin(); it != hits.end(); ++it) {
    if (sensor == (*it)->sensor()) continue;
    const double chi2 = track.chi2(*it);
    if (chi2 < bestChi2) {
      bestChi2 = chi2;
      best = *it;
    }
  }
  if (bestChi2 < m_maxChi2ToAdd) {
    track.addPhiCluster(best);
    if (m_debug) printHit(best, "   ++  ");
    return true;
  }
  return false;

}

//============================================================================
/// Get the closest hit in the specified window
//============================================================================
PrVLHit* PrVLTracking::closestHit(PrVLHits& hits, 
                                  const double r, const double tol,
                                  const bool unused) {

  double minDist = tol;
  PrVLHit* best = 0;
  for (PrVLHits::const_iterator it = hits.begin(); it != hits.end(); ++it) {
    const double d = (*it)->rGlobal() - r;
    if (d < -minDist) continue;
    if (d >  minDist) break;
    if (unused && (*it)->nUsed() != 0) continue;
    minDist = fabs(d);
    best = *it;
  }
  return best;

}

//=============================================================================
/// Print coordinates of a hit
//=============================================================================
void PrVLTracking::printHit(const PrVLHit* hit, const std::string title) {

  if (!m_debug) return;
  debug() << title 
          << format("sensor %3d z %7.1f zone %1d strip %4d frac %3.2f radius %8.3f dsin %5.3f used %2d ",
                    hit->sensor(), hit->z(), hit->zone(), 
                    hit->cluster().channelID().strip(), 
                    hit->cluster().interStripFraction(), 
                    hit->rGlobal(), hit->dGlobal(),
                    hit->nUsed());
  LHCbID id = hit->lhcbID();
  if (m_debugTool) {
    m_debugTool->printKey(debug(), id);
    if (matchKey(hit)) debug() << " ***";
  }
  debug() << endmsg;

}

//=============================================================================
/// Print hits on an RZ track 
//=============================================================================
void PrVLTracking::printRZTrack(PrVLTrack& track) {

  PrVLHits::iterator it;
  for (it = track.rHits().begin(); it != track.rHits().end(); ++it) {
    const double d = track.rPred((*it)->z()) - (*it)->rGlobal();
    info() << format("Dist%8.3f Chi2%8.2f", d, d * d * (*it)->weight());
    printHit(*it, " ");
  }

}

void PrVLTracking::printTrack(PrVLTrack& track) {

  debug() << "Zone " << track.zone() << ", ";
  if (track.backward()) {
    debug() << "backward";
  } else {
    debug() << "forward";
  }
  debug() << endmsg;
  PrVLHits::iterator it;
  for (it = track.rHits().begin(); it != track.rHits().end(); ++it) {
    printHit(*it, "R   ");
  }
  for (it = track.phiHits().begin(); it != track.phiHits().end(); ++it) {
    printHit(*it, "PHI ");
  }

} 
