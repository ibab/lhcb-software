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
//    GaudiAlgorithm(name, pSvcLocator),
    GaudiTupleAlg(name, pSvcLocator),
    m_det(0),
    m_debugTool(0) {

  declareProperty("TrackLocation", m_trackLocation = TrackLocation::Velo);

  declareProperty("Forward",  m_forward = true);
  declareProperty("Backward", m_backward = true);

  declareProperty("ZVertexMin", m_zVertexMin = -220. * Gaudi::Units::mm);
  declareProperty("ZVertexMax", m_zVertexMax = +170. * Gaudi::Units::mm);
  // Parameters for RZ tracking
  declareProperty("MaxRSlope4", m_maxRSlope4 = 0.4);
  declareProperty("MaxRSlope3", m_maxRSlope3 = 0.6);
  declareProperty("RTol4",      m_rTol4       = 0.75);
  declareProperty("RTol3",      m_rTol3       = 1.3);
  declareProperty("RTolExtra",  m_rTolExtra   = 3.0);
  declareProperty("RTolOverlap", m_rTolOverlap = 0.6);
  declareProperty("MaxMissed",   m_maxMissed = 1);
  declareProperty("MinToTag",    m_minToTag = 4);
  // Parameters for space tracking
  declareProperty("XTolOverlap", m_xTolOverlap = 0.6);
  declareProperty("PhiTol",      m_phiTol = 0.033);
  declareProperty("XYTol",       m_xyTol = 0.45);
  declareProperty("MaxQXy",      m_maxQXy = 5.5);
  declareProperty("MaxQRPhi",    m_maxQRPhi = 8.);
  declareProperty("MaxQXyFull",  m_maxQXyFull = 10.);
  declareProperty("FractionPhi",    m_fractionPhi = 0.7);
  declareProperty("FractionShared", m_fractionShared = 0.7);
  declareProperty("FractionUsed",   m_fractionUsed = 0.6);
  declareProperty("MaxChi2PerHit",  m_maxChi2PerHit = 14.);
  // Parameters for merging clones
  declareProperty("FractionForMerge", m_fractionForMerge = 0.6);
  declareProperty("MaxChi2ToAdd",     m_maxChi2ToAdd = 20.);
  declareProperty("MaxDistToMerge",   m_maxDistToMerge = 0.1 * Gaudi::Units::mm);
  declareProperty("MaxDeltaSlopeToMerge", m_maxDeltaSlopeToMerge = 0.002);
  // Parameters for recovery
  declareProperty("MaxRZForExtra", m_maxRZForExtra = 200);
  declareProperty("PhiUnusedFirstTol",  m_phiUnusedFirstTol  = 2.);
  declareProperty("PhiUnusedSecondTol", m_phiUnusedSecondTol = 10.);
  // Parameters for debugging
  declareProperty("DebugToolName", m_debugToolName = "");
  declareProperty("WantedKey", m_wantedKey = -100);
  declareProperty("TimingMeasurement", m_timing = false);

}

//============================================================================
/// Initialization
//============================================================================
StatusCode PrVLTracking::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;
  if (msgLevel(MSG::DEBUG)) debug() << " ==> initialize()" << endmsg;

  // Get the detector element. 
  m_det = getDet<DeVL>(DeVLLocation::Default);
  // Get the number of stations and set up the container of Phi hits.
  m_nStations = (m_det->numberRSensors() + m_det->numberPhiSensors()) / 4;
  m_phiHits.reserve(m_nStations);
  m_phiHits.resize(m_nStations);
  // Get the number of R and phi sectors.
  const DeVLRSensor* rSensor = m_det->rSensors().front();
  const DeVLPhiSensor* phiSensor = m_det->phiSensors().front();
  m_nZonesR = rSensor->numberOfZones();
  m_nZonesPhi = phiSensor->numberOfZones();
  // Calculate the bisectors of the R zones.
  m_ctrPhi.resize(2 * m_nZonesR);
  m_tolPhi.resize(2 * m_nZonesR);
  const double dphi = Gaudi::Units::pi / double(m_nZonesR);
  // Get the angle of the first strip to determine the overlap phi range.
  const double phiFirst = rSensor->phiMinZone(0, rSensor->rMin(0));
  const double dphiOverlap = fabs(phiFirst) - Gaudi::Units::halfpi;
  for (unsigned int i = 0; i < 2 * m_nZonesR; ++i) {
    double phi = -0.5 * Gaudi::Units::pi + (0.5 + i) * dphi;
    if (0 == i % m_nZonesR) {
      phi -= dphiOverlap / 2.;
      m_tolPhi[i] = 0.5 * (dphi + dphiOverlap);
    } else if (m_nZonesR - 1 == i % m_nZonesR) {
      phi += dphiOverlap / 2.;
      m_tolPhi[i] = 0.5 * (dphi + dphiOverlap);
    } else {
      m_tolPhi[i] = 0.5 * dphi;
    }
    m_ctrPhi[i] = phi;
    info() << "Zone " << i << ": phi = " << phi <<  " +/- " << m_tolPhi[i] << endmsg;
  }
  // Determine the highest sensor number.
  unsigned int maxSensor = 0;
  const std::vector<DeVLSensor*>& sensors = m_det->sensors();
  std::vector<DeVLSensor*>::const_iterator it;
  for (it = sensors.begin(); it != sensors.end(); ++it) {
    if ((*it)->sensorNumber() > maxSensor) maxSensor = (*it)->sensorNumber();
  }
  // Setup the hit map.
  m_hits.resize(maxSensor + 1);
  for (unsigned int i = 0; i <= maxSensor; ++i) {
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
  if ("" != m_debugToolName && m_wantedKey >= 0) {
    m_debug = true;
    m_debugTool = tool<IPrDebugTool>(m_debugToolName);
  } else {
    m_debug = false;
  }

  // Setup the timing tool.
  if (m_timing) {
    m_timerTool = tool<ISequencerTimerTool>("SequencerTimerTool/Timer", this);
    m_timeTotal = m_timerTool->addTimer("PrVL");
    m_timerTool->increaseIndent();
    m_timePrepare = m_timerTool->addTimer("PrVL prepare");
    m_timeFwd4    = m_timerTool->addTimer("PrVL find fwd. quadruplets");
    m_timeBwd4    = m_timerTool->addTimer("PrVL find bwd. quadruplets");
    m_timeFwd3    = m_timerTool->addTimer("PrVL find fwd. triplets");
    m_timeBwd3    = m_timerTool->addTimer("PrVL find bwd. triplets");
    m_timeSpace   = m_timerTool->addTimer("PrVL find space tracks");
    m_timeUnused  = m_timerTool->addTimer("PrVL unused hits");
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
  if (m_timing) {
    m_timerTool->start(m_timeTotal);
    m_timerTool->start(m_timePrepare);
  }
  // Reset the containers.
  m_rzTracks.clear();
  m_spaceTracks.clear();
  buildHits();

  const double zMin = m_zVertexMin + 200.;
  const double zMax = m_zVertexMax - 200.;
  const std::vector<DeVLRSensor*>& rSensors = m_det->rSensors();

  if (m_timing) m_timerTool->stop(m_timePrepare);
  // Look for quadruplets.
  if (m_forward) {
    const bool forward = true;
    if (m_debug) info() << "Looking for forward quadruplets" << endmsg;
    if (m_timing) m_timerTool->start(m_timeFwd4);
    std::vector<DeVLRSensor*>::const_reverse_iterator it;
    for (it = rSensors.rbegin(); it != rSensors.rend(); ++it) {
      if ((*it)->z() < zMin) break;
      findQuadruplets(*it, forward);
    }
    if (m_timing) m_timerTool->stop(m_timeFwd4);
  }
  if (m_backward) {
    const bool forward = false;
    if (m_debug) info() << "Looking for backward quadruplets" << endmsg;
    if (m_timing) m_timerTool->start(m_timeBwd4);
    std::vector<DeVLRSensor*>::const_iterator it;
    for (it = rSensors.begin(); it != rSensors.end(); ++it) {
      if ((*it)->z() > zMax) break;
      findQuadruplets(*it, forward);
    }
    if (m_timing) m_timerTool->stop(m_timeBwd4);
  }  
  // Look for unused triplets.
  if (m_forward) {
    const bool forward = true;
    if (m_debug) info() << "Looking for forward triplets" << endmsg;
    if (m_timing) m_timerTool->start(m_timeFwd3);
    std::vector<DeVLRSensor*>::const_reverse_iterator it;
    for (it = rSensors.rbegin(); it != rSensors.rend(); ++it) {
      if ((*it)->z() < zMin) break;
      findTriplets(*it, forward);
    }
    if (m_timing) m_timerTool->stop(m_timeFwd3);
  }
  if (m_backward) {
    const bool forward = false;
    if (m_debug) info() << "Looking for backward triplets" << endmsg;
    if (m_timing) m_timerTool->start(m_timeBwd3);
    std::vector<DeVLRSensor*>::const_iterator it;
    for (it = rSensors.begin(); it != rSensors.end(); ++it) {
      if ((*it)->z() > zMax) break;
      findTriplets(*it, forward);
    }
    if (m_timing) m_timerTool->stop(m_timeBwd3);
  }
  if (m_timing) m_timerTool->start(m_timeSpace);
  // Sort tracks and try removing fake hits.
  cleanupRZ();
  if (m_debug) {
    info() << "Found " << m_rzTracks.size() << " RZ tracks" << endmsg;
  }
  // Look for matching Phi hits.
  const unsigned int nTracksRZ = m_rzTracks.size();
  PrVLTracks::iterator itt;
  for (itt = m_rzTracks.begin(); itt != m_rzTracks.end(); ++itt) {
    if (m_debug) {
      info() << "Track " << itt - m_rzTracks.begin() << endmsg;
      printRZTrack(*itt);
    }
    findPhiHits(*itt, false);
    const int status = findSpaceTracksPhi(*itt);
    if (2 == status) findSpaceTracksXY(*itt);
  }
  m_rzTracks.clear();
  if (m_timing) m_timerTool->stop(m_timeSpace);
  if (m_debug) {
    info() << "Made " << m_spaceTracks.size() << " space tracks" << endmsg;
  }

  if (m_timing) m_timerTool->start(m_timeUnused);
  // Repeat the search for R tracks allowing for zone-crossings.
  if (m_forward) {
    const bool forward = true;
    if (m_debug) info() << "Looking for unused R hits" << endmsg;
    std::vector<DeVLRSensor*>::const_iterator it;
    for (it = rSensors.begin(); it != rSensors.end(); ++it) {
      if ((*it)->z() < zMin) continue;
      findUnusedR(*it, forward);
    }
    cleanupRZ();
  }
  for (itt = m_rzTracks.begin(); itt != m_rzTracks.end(); ++itt) {
    findPhiHits(*itt, true);
    int status = findSpaceTracksPhi(*itt);
    if (2 == status) findSpaceTracksXY(*itt);
  } 

  // Try to find track seeds from unused Phi hits.
  if (nTracksRZ < m_maxRZForExtra && 
      (m_rzTracks.size() * 0.5 <= m_spaceTracks.size() ||
       m_rzTracks.size() < 20)) {
    if (m_debug) info() << "Looking for unused Phi hits" << endmsg;
    findUnusedPhi();
  }
  if (m_timing) m_timerTool->stop(m_timeUnused);

  if (m_debug) {
    info() << m_spaceTracks.size() << " tracks after recovery" << endmsg;
  }
  if (m_timing) m_timerTool->start(m_timeFinal);
  // Cleanup tracks with different R, same Phi hits.
  if (m_spaceTracks.size() >= 2) {
    if (m_forward) mergeSpaceClones(true);
    // if (m_backward) mergeSpaceClones(false);
  }
  // Store the track candidates as LHCb tracks.
  makeLHCbTracks();
  if (m_timing) {
    m_timerTool->stop(m_timeFinal);
    m_timerTool->stop(m_timeTotal);
  }
  if (m_debug) {
    // Print all tracks which have hits from the wanted particle.
    for (itt = m_spaceTracks.begin(); itt != m_spaceTracks.end(); ++itt) {
      bool foundKey = false;
      PrVLHits::const_iterator ith;
      for (ith = (*itt).rHits().begin(); ith != (*itt).rHits().end(); ++ith) {
        if (matchKey(*ith)) {
          foundKey = true;
          break;
        }
      }
      if (foundKey) {
        printTrack(*itt);
        continue;
      }
      for (ith = (*itt).phiHits().begin(); ith != (*itt).phiHits().end(); ++ith) {
        if (matchKey(*ith)) {
          foundKey = true;
          break;
        }
      }
      if (foundKey) printTrack(*itt);
    }
  } 
  return StatusCode::SUCCESS;

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
      const double zFrac = (z2 - z0) / (z3 - z0);
      double rMin = 0., rMax = 0.;
      // Loop over the hits in the first sensor.
      PrVLHits::const_iterator it0;
      for (it0 = m_hits[s0][iz].begin(); it0 != m_hits[s0][iz].end(); ++it0) {
        // Skip used hits.
        if ((*it0)->nUsed() >= (*it0)->nMaxUsed()) continue;
        const double r0 = (*it0)->r();
        if (forward) {
          rMin = r0 + m_maxRSlope4 * (z3 - z0);
          rMax = r0 * (z3 - m_zVertexMin) / (z0 - m_zVertexMin);
        } else {
          rMin = r0 - m_maxRSlope4 * (z3 - z0);
          rMax = r0 * (z3 - m_zVertexMax) / (z0 - m_zVertexMax);
        }
        if (m_debug && matchKey(*it0)) {
          printHit(*it0, "S0 ");
          info() << format("%8.3f < r3 < %8.3f", rMin, rMax) << endmsg;
        }
        // Loop over the hits in the fourth sensor to find a possible match.
        PrVLHits::const_iterator it3;
        for (it3 = m_hits[s3][iz].begin(); it3 != m_hits[s3][iz].end(); ++it3) {
          // Skip used hits.
          if (ic > 0 && (*it3)->nUsed() >= (*it3)->nMaxUsed()) continue;
          const double r3 = (*it3)->r();
          if (r3 < rMin) continue;
          if (r3 > rMax) break;
          // Calculate expected radius and tolerance at the third sensor.
          const double r2 = r0 + zFrac * (r3 - r0);
          double tol2 = m_rTol4 * sensor2->rPitchOfRadius(r2);
          if (m_debug && (matchKey(*it0) || matchKey(*it3))) {
            printHit(*it3, "  S3 ");
            info() << format("  r2 = %8.3f +/- %6.3f", r2, tol2) << endmsg;
          }
          // Check if the third sensor has a hit within the window.
          PrVLHit* h2 = closestRHit(m_hits[s2][iz], r2, tol2, true);
          if (!h2) continue;
          // Create a track.
          PrVLTrack newTrack;
          newTrack.addRHit(*it0);
          newTrack.addRHit(h2);
          newTrack.addRHit(*it3);
          // Set the direction.
          newTrack.setBackward(!forward);
          // Set the zone number.
          if (sensor0->isRight()) {
            newTrack.setRZone(iz + m_nZonesR);
          } else {
            newTrack.setRZone(iz);
          }
          // Calculate expected radius and tolerance at the second sensor.
          const double r1 = newTrack.rInterpolated(z1);
          double tol1 = m_rTol4 * sensor1->rPitchOfRadius(r1);
          if (m_debug && (matchKey(*it0) || matchKey(*it3) || matchKey(h2))) {
            printHit(h2, "    S2 ");
            info() << format("    r1 = %8.3f +/- %6.3f", r1, tol1) << endmsg;
          }
          // Check if the second sensor has a matching hit. 
          PrVLHit* h1 = closestRHit(m_hits[s1][iz], r1, tol1, false);
          if (!h1) continue;
          newTrack.addRHit(h1);
          if (m_debug) {
            info() << "    === Valid R quadruplet ===" << endmsg;
            printHit(*it0, "    S0 ");
            printHit(h1,   "    S1 ");
            printHit(h2,   "    S2 ");
            printHit(*it3, "    S3 ");
          }
          // Extrapolate towards the vertex.
          extendTrack(newTrack, s3, iz, forward);
          // Require at least two unused hits on the track.
          const unsigned int nUsed = newTrack.nbUsedRHits();
          if (newTrack.nbRHits() - nUsed <= 1) {
            if (m_debug) {
              info() << "    " << newTrack.nbUsedRHits() << "/"
                      << newTrack.nbRHits() << " are used." << endmsg;
            }
            continue;
          }
          // In case of outer zones, try to add hits on the other side.
          if (0 == iz || m_nZonesR - 1 == iz) {
            extendTrackOtherSide(newTrack, s0, iz, forward, false);
            extendTrackOtherSide(newTrack, s0, iz, !forward, true);
          }
          m_rzTracks.push_back(newTrack);
          if (m_debug) {
            info() << "    Adding track " << m_rzTracks.size() - 1 << endmsg;
          }
          if (newTrack.nbRHits() >= m_minToTag) newTrack.tagUsedRHits();
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
      if (1 == ic) {
        // Case 1: x x o x
        if (forward) {
          sensor2 = sensor2->previousRSensor();
        } else {
          sensor2 = sensor2->nextRSensor();
        }
        if (!sensor2) break;
      } else if (2 == ic) {
        // Case 2: x o x x
        if (forward) {
          sensor1 = sensor1->previousRSensor();
        } else {
          sensor1 = sensor1->nextRSensor();
        }
      }
      const unsigned int s1 = sensor1->sensorNumber();
      const unsigned int s2 = sensor2->sensorNumber();  
      const double z1 = sensor1->z();
      const double z2 = sensor2->z();
      // Avoid extrapolation over too large distance.
      const double zFrac = (z1 - z0) / (z2 - z0);
      double rMin = 0., rMax = 0.;
      // Loop over the hits in the first sensor.
      PrVLHits::const_iterator it0;
      for (it0 = m_hits[s0][iz].begin(); it0 != m_hits[s0][iz].end(); ++it0) {
        // Skip used hits.
        if ((*it0)->nUsed() >= (*it0)->nMaxUsed()) continue;
        double r0 = (*it0)->r();
        if (forward) {
          rMin = r0 + m_maxRSlope3 * (z2 - z0);
          rMax = r0 * (z2 - m_zVertexMin) / (z0 - m_zVertexMin);
        } else {
          rMin = r0 - m_maxRSlope3 * (z2 - z0);
          rMax = r0 * (z2 - m_zVertexMax) / (z0 - m_zVertexMax);
        }
        if (m_debug && matchKey(*it0)) {
          printHit(*it0, "S0 ");
          info() << format("%8.3f < r2 < %8.3f", rMin, rMax) << endmsg;
        }
        // Loop over the hits in the third sensor to find a possible match.
        PrVLHits::const_iterator it2;
        for (it2 = m_hits[s2][iz].begin(); it2 != m_hits[s2][iz].end(); ++it2) {
          if ((*it2)->nUsed() >= (*it2)->nMaxUsed()) continue;
          const double r2 = (*it2)->r();
          if (r2 < rMin) continue;
          if (r2 > rMax) break;
          // Calculate expected radius and tolerance at the second sensor.
          const double r1 = r0 + zFrac * (r2 - r0);
          double tol = m_rTol3 * sensor1->rPitchOfRadius(r1);
          if (m_debug && (matchKey(*it0) || matchKey(*it2))) {
            printHit(*it2, "  S2 ");
            info() << format("  r1 = %8.3f +/- %6.3f", r1, tol) << endmsg;
          }
          // Check if the second sensor has a matching hit.
          PrVLHit* h1 = closestRHit(m_hits[s1][iz], r1, tol, true);
          if (!h1) continue;
          if (m_debug) {
            info() << "    === Valid R triplet ===" << endmsg;
            printHit(*it0, "    S0 ");
            printHit(h1,   "    S1 ");
            printHit(*it2, "    S2 ");
          }
          // Create a track.
          PrVLTrack newTrack;
          newTrack.addRHit(*it0);
          newTrack.addRHit(h1);
          newTrack.addRHit(*it2);
          // Set the direction.
          newTrack.setBackward(!forward);
          // Set the zone number.
          if (sensor0->isRight()) {
            newTrack.setRZone(iz + m_nZonesR);
          } else {
            newTrack.setRZone(iz);
          }
          // Extrapolate towards the vertex.
          extendTrack(newTrack, s2, iz, forward);
          if (newTrack.nbRHits() - newTrack.nbUsedRHits() <= 1) {
            if (m_debug) {
              info() << "    " << newTrack.nbUsedRHits() << "/"
                     << newTrack.nbRHits() << " are used." << endmsg;
            }
            continue;
          }

          // In case of outer zones, try to add hits on the other side.
          if (0 == iz || m_nZonesR - 1 == iz) {
            extendTrackOtherSide(newTrack, s0, iz, forward, false);
          }

          // In case of short tracks, try to add hits in the other direction.
          if (3 == newTrack.nbRHits()) {
            extendTrack(newTrack, s0, iz, !forward);
          }
          // Try adjacent zones in the other direction.
          if (3 == newTrack.nbRHits()) { 
            if (iz != 0) {
              extendTrack(newTrack, s0, iz - 1, !forward);
            }
          }
          if (3 == newTrack.nbRHits()) {
            if (iz != m_nZonesR - 1) {
              extendTrack(newTrack, s0, iz + 1, !forward);
            }
          }

          // Additional checks on 3 hit tracks.
          if (3 == newTrack.nbRHits()) {
            // Reject tracks with a missed station.
            if (ic > 0) continue;
            // If hits are shared with another track, take the best one.
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
                  info() << "    " << nCommon << " hits shared with track "
                          << itt - m_rzTracks.begin() << endmsg;
                  info() << "    Chi2: " << newTrack.rChi2() << " (new), " 
                          << (*itt).rChi2() << " (" << itt - m_rzTracks.begin() 
                          << ")" << endmsg;
                }
                if (newTrack.rChi2() < (*itt).rChi2()) {
                  if (m_debug) {
                    info() << "    Replacing track " 
                            << itt - m_rzTracks.begin() << endmsg;
                  }
                  (*itt) = newTrack;
                } else if (m_debug) {
                  info() << "    Rejecting new track" << endmsg;
                }
                reject = true;
                break;
              }
            }
            if (reject) continue;
          }
          m_rzTracks.push_back(newTrack);
          if (m_debug) {
            info() << "    Adding track " << m_rzTracks.size() - 1 << endmsg;
          }
          if (m_minToTag <= newTrack.nbRHits()) newTrack.tagUsedRHits();
        }
      }
    }
  }
  
}

//=========================================================================
/// Find possible Phi hits matching a given RZ track 
//=========================================================================
void PrVLTracking::findPhiHits(PrVLTrack& seed, const bool unused) {

  // Reset the list of hits.
  for (int i = m_nStations; i--;) m_phiHits[i].clear();
  if (seed.overlap()) {
    findPhiHitsOverlap(seed, unused);
    return;
  } 

  // Determine the range of Phi sensors to search.
  const DeVLRSensor* firstR = m_det->rSensor(seed.rHits().front()->sensor());
  const DeVLRSensor* lastR = m_det->rSensor(seed.rHits().back()->sensor());
  const DeVLPhiSensor* firstPhi = firstR->associatedPhiSensor();
  const DeVLPhiSensor* lastPhi = lastR->associatedPhiSensor();
  unsigned int snMin = firstPhi->sensorNumber();
  unsigned int snMax = lastPhi->sensorNumber();
  if (snMin > snMax) {
    const unsigned int tmp = snMin;
    snMin = snMax;
    snMax = tmp; 
  }

  // Store the Phi hits in the range.
  PrVLHits::iterator ith;
  for (unsigned int i = snMin; i <= snMax; ++i) {
    const DeVLPhiSensor* sensor = m_det->phiSensor(i);
    const double rPred = seed.rInterpolated(sensor->z());
    bool right = false;
    unsigned int rZone = seed.rZone(sensor->z(), right);
    if (rZone != 0 && rZone != m_nZonesR - 1) {
      // Consider only Phi hits on the same side as the R hit.
      if (right != sensor->isRight()) continue;
    }
    if (right) rZone += m_nZonesR;
    // Get the bisector of the R zone.
    const double phiR = m_ctrPhi[rZone];
    const unsigned int station = sensor->station();
    for (unsigned int j = 0; j < m_nZonesPhi; ++j) {
      if (rPred < sensor->rMin(j)) break;
      if (rPred > sensor->rMax(j)) continue;
      for (ith = m_hits[i][j].begin(); ith != m_hits[i][j].end(); ++ith) {
        if (m_debug && matchKey(*ith)) printHit(*ith, "");
        if (unused && (*ith)->nUsed() >= (*ith)->nMaxUsed()) continue;
        // Skip hits which are incompatible with the R sector of the track.
        if ((*ith)->rZone() < 900 && (*ith)->rZone() != rZone) continue; 
        // Calculate global phi at the predicted radius.
        float phiS = sensor->phiOfStrip((*ith)->strip(), (*ith)->fraction(), rPred);
        if (!sensor->isDownstream()) phiS = -phiS;
        if ((*ith)->right()) phiS += Gaudi::Units::pi;
        // Check if the global phi is compatible with the R sector.
        if (fabs(phiS - phiR) > m_tolPhi[rZone]) continue;
        if (m_debug && matchKey(*ith)) {
          info() << "  Passed angular cut." << endmsg;
        }
        const double x = rPred * cos(phiS);
        const double y = rPred * sin(phiS);
        (*ith)->setX(x);
        (*ith)->setY(y);
        (*ith)->setZ(sensor->z(x, y));
        (*ith)->setPhi(phiS);
        (*ith)->setWeight(rPred);
        m_phiHits[station].push_back(*ith);
      }
    }
  }

}

//=========================================================================
/// Find possible Phi hits matching a given RZ track in the overlap region 
//=========================================================================
void PrVLTracking::findPhiHitsOverlap(PrVLTrack& seed, const bool unused) {

  // Determine the range of Phi sensors to search.
  const DeVLRSensor* firstR = m_det->rSensor(seed.rHits().front()->sensor());
  const DeVLRSensor* lastR = m_det->rSensor(seed.rHits().back()->sensor());
  const DeVLPhiSensor* firstPhi = firstR->associatedPhiSensor();
  const DeVLPhiSensor* lastPhi = lastR->associatedPhiSensor();
  unsigned int snMin = firstPhi->sensorNumber();
  unsigned int snMax = lastPhi->sensorNumber();
  if (snMin > snMax) {
    const unsigned int tmp = snMin;
    snMin = snMax;
    snMax = tmp; 
  }

  // Store the Phi hits in the range.
  PrVLHits::iterator ith;
  for (unsigned int i = snMin; i <= snMax; ++i) {
    const DeVLPhiSensor* sensor = m_det->phiSensor(i);
    const double rPred = seed.rInterpolated(sensor->z());
    const unsigned int station = sensor->station();
    for (unsigned int j = 0; j < m_nZonesPhi; ++j) {
      if (rPred < sensor->rMin(j)) break;
      if (rPred > sensor->rMax(j)) continue;
      for (ith = m_hits[i][j].begin(); ith != m_hits[i][j].end(); ++ith) {
        if (m_debug && matchKey(*ith)) printHit(*ith, "");
        if (unused && (*ith)->nUsed() >= (*ith)->nMaxUsed()) continue;
        if ((*ith)->yStripCentre() < 0.) {
          if (seed.rZone() != 0 && seed.rZone() != 2 * m_nZonesR - 1) continue;
        } else {
          if (seed.rZone() != m_nZonesR - 1 && seed.rZone() != m_nZonesR) continue;
        }
        if (fabs((*ith)->xStripCentre()) > 5 * m_xTolOverlap) continue;
        // Calculate global phi at the predicted radius.
        float phiS = sensor->phiOfStrip((*ith)->strip(), (*ith)->fraction(), rPred);
        if (!sensor->isDownstream()) phiS = -phiS; 
        if ((*ith)->right()) phiS += Gaudi::Units::pi;
        const double x = rPred * cos(phiS);
        const double y = rPred * sin(phiS);
        if (fabs(x) > m_xTolOverlap) continue;
        (*ith)->setX(x);
        (*ith)->setY(y);
        (*ith)->setZ(sensor->z(x, y));
        // For lower half tracks shift angle to handle left/right transitions.
        if (y < 0.) phiS += Gaudi::Units::pi;
        (*ith)->setPhi(phiS);
        (*ith)->setWeight(rPred);
        m_phiHits[station].push_back(*ith);
      }
    }
  }

}

//=========================================================================
/// Make space tracks.
//=========================================================================
int PrVLTracking::findSpaceTracksPhi(PrVLTrack& seed) { 

  int firstStation = 0;
  int lastStation = m_nStations - 1;
  for (int station = 0; station < m_nStations; ++station) {
    if (m_phiHits[station].empty()) {
      if (station == firstStation) ++firstStation;
    } else {
      lastStation = station;
      if (m_phiHits[station].size() > 1) {
        std::sort(m_phiHits[station].begin(), m_phiHits[station].end(),
                  PrVLHit::IncreasingByPhi());
      }
    }
  }
  if (firstStation > lastStation) return 1;
  int step = 1;
  if (!seed.backward()) {
    const int tmp = firstStation;
    firstStation = lastStation;
    lastStation = tmp;
    step = -1;
  }

  // Determine the min. number of Phi hits required for a track.
  unsigned int nStations = abs(lastStation - firstStation) + 1;
  if (nStations < 3) return 1;
  unsigned int minNbPhi = std::min(nStations, seed.nbRHits());
  if (seed.overlap()) {
    minNbPhi = std::min(nStations, (seed.nbRHits() / 2));
  }
  minNbPhi = int(m_fractionPhi * minNbPhi);
  if (minNbPhi < 3) minNbPhi = 3;
  if (m_debug) {
    info() << "Searching from station " << firstStation 
            << " to " << lastStation << endmsg;
    info() << "Require " << minNbPhi << " phi hits on track" << endmsg;
  }

  // Check if the seed track could be shared by two particles.
  double sumFraction = 0.;
  double sumAdc = 0.;
  PrVLHits::iterator ithr;
  for (ithr = seed.rHits().begin(); ithr != seed.rHits().end(); ++ithr) {
    sumFraction += (*ithr)->fraction();
    if ((*ithr)->adcHigh()) sumAdc += 1.;
  }
  bool shared = false;
  if (seed.nbRHits() > 4 && 
      (sumFraction / seed.nbRHits() > m_fractionShared ||
       sumAdc / seed.nbRHits() > m_fractionShared)) {
    shared = true;
  }

  PrVLTracks tracks;
  // Loop over the hits in the first/last station.
  int s1 = firstStation;
  PrVLHits::iterator ith1; 
  for (ith1 = m_phiHits[s1].begin(); ith1 != m_phiHits[s1].end(); ++ith1) {
    PrVLHits hits;
    hits.push_back(*ith1);
    double phi = (*ith1)->phi();
    unsigned int nMiss = 0;
    // Add hits within the same Phi wedge.
    for (int s2 = s1 + step; s2 != lastStation + step; s2 += step) {
      if (nMiss > 1) break;
      if (m_phiHits[s2].empty()) continue;
      PrVLHit* best = closestPhiHit(m_phiHits[s2], phi, m_phiTol, false);
      if (!best) {
        ++nMiss;
        continue;
      }
      nMiss = 0;
      hits.push_back(best);
      phi = best->phi();
      if (fabs(best->x()) < m_xTolOverlap) {
        PrVLHit* other = closestPhiHitOtherSide(m_phiHits[s2], phi, m_phiTol, false, best->sensor());
        if (other) {
          hits.push_back(other);
        }
      }
    }
    if (hits.size() < minNbPhi) continue; 
    // Fit the proto-track.
    double x0 = 0., y0 = 0.;
    double tx = 0., ty = 0.;
    double qxy = 0.;
    if (!fitProtoTrack(hits, 3 * minNbPhi, seed.nbRHits(), qxy, x0, y0, tx, ty)) {
      continue;
    }
    PrVLTrack track;
    track.setPhiClusters(seed, x0, tx, y0, ty, hits);
    track.updateRParameters();
    PrVLHits::iterator ith;
    while (track.phiHits().size() > minNbPhi && track.qFactor() > m_maxQRPhi) {
      double worstChi2 = -1.;
      PrVLHits::iterator worst = track.phiHits().end();
      for (ith = track.phiHits().begin(); ith != track.phiHits().end(); ++ith) {
        const double chi2 = track.chi2(*ith);
        if (chi2 > worstChi2) {
          worstChi2 = chi2;
          worst = ith;
        }
      }
      if (m_debug) printHit(*worst, " - ");
      track.removePhiHit(*worst);
    }
    const double fUsed = double(track.nbUsedPhiHits()) / track.nbPhiHits();
    if (!shared && fUsed > m_fractionUsed) {
      if (m_debug) info() << " >> too many used (" << fUsed << ")" << endmsg;
      continue;
    }
    if (track.qFactor() > m_maxQRPhi) {
      if (m_debug) info() << " >> q = " << track.qFactor() << endmsg;
      continue;
    }
    if (m_debug) info() << " >>>> q = " << track.qFactor() << endmsg;
    if (m_debug) info() << " >>>> qxy = " << qxy << endmsg;
    track.setPhiChi2(qxy);
    tracks.push_back(track);
  }

  if (m_debug) info() << tracks.size() << " candidates" << endmsg;
  if (tracks.empty()) return 2;
  // Merge tracks if necessary.
  if (tracks.size() > 1) mergeClones(tracks);

  PrVLTracks::iterator itt;
  // Sort the track candidates by the number of Phi hits.
  std::stable_sort(tracks.begin(), tracks.end(),
                   PrVLTrack::DecreasingByPhiLength());

  int nExpected = 1;
  if (shared) ++nExpected;
  int nFound = 0;
  // Start with the best candidate.
  for (itt = tracks.begin(); itt != tracks.end(); ++itt) {
    if (!(*itt).valid()) continue;
    if (!(*itt).removeWorstRAndPhi(m_maxChi2PerHit, 6)) {
      // Less than three R or Phi hits after cleanup.
      continue;
    }
    if ((*itt).nbRHits() < m_minToTag) (*itt).tagUsedRHits();
    (*itt).tagUsedPhiHits();
    (*itt).setBackward(seed.backward());
    m_spaceTracks.push_back(*itt);
    ++nFound;
    if (nFound >= nExpected) return 0;
  }
  if (nFound > 0) return 0; 
  // No candidate found.
  return 1;

}

//=========================================================================
/// Make space tracks.
//=========================================================================
bool PrVLTracking::findSpaceTracksXY(PrVLTrack& seed) { 

  int firstStation = 0;
  int lastStation = m_nStations - 1;
  for (int station = 0; station < m_nStations; ++station) {
    if (m_phiHits[station].empty()) {
      if (station == firstStation) ++firstStation;
    } else {
      lastStation = station;
    }
  }
  if (firstStation > lastStation) return false;
  int step = 1;
  if (seed.backward()) {
    const int tmp = firstStation;
    firstStation = lastStation;
    lastStation = tmp;
    step = -1;
  }
 
  // Determine the min. number of Phi hits required for a track.
  unsigned int nStations = abs(lastStation - firstStation) + 1;
  if (nStations < 3) return false;
  unsigned int minNbPhi = std::min(nStations, seed.nbRHits());
  if (seed.overlap()) {
    minNbPhi = std::min(nStations, (seed.nbRHits() / 2));
  }
  minNbPhi = int(m_fractionPhi * minNbPhi);
  if (minNbPhi < 3) minNbPhi = 3;
  if (m_debug) {
    info() << "Searching from station " << firstStation 
            << " to " << lastStation << endmsg;
    info() << "Require " << minNbPhi << " phi hits on track" << endmsg;
  }

  // Find suitable pairs of hits as starting point.
  int s1 = firstStation;
  int s2 = s1 + 4 * step;
  if (s2 < 0 || s2 >= m_nStations) {
    s2 = lastStation;
  } else if (m_phiHits[s2].empty()) {
    s2 = lastStation; 
  }
  PrVLTracks tracks; 
  const unsigned int nCases = 2;
  for (unsigned int ic = 0; ic < nCases; ++ic) {
    PrVLHits::iterator ith1;
    for (ith1 = m_phiHits[s1].begin(); ith1 != m_phiHits[s1].end(); ++ith1) {
      if ((*ith1)->nUsed() >= (*ith1)->nMaxUsed()) continue; 
      const double x1 = (*ith1)->x();
      const double y1 = (*ith1)->y();
      const double z1 = (*ith1)->z();
      PrVLHits::iterator ith2;
      for (ith2 = m_phiHits[s2].begin(); ith2 != m_phiHits[s2].end(); ++ith2) {
        if (0 == ic && (*ith2)->nUsed() >= (*ith2)->nMaxUsed()) continue; 
        double x2 = (*ith2)->x();
        double y2 = (*ith2)->y();
        double z2 = (*ith2)->z();
        double dz = z2 - z1;
        double tx = (x2 - x1) / dz;
        double ty = (y2 - y1) / dz;
        if (fabs(tx) > m_maxRSlope4) continue;
        if (fabs(ty) > m_maxRSlope4) continue;
        double x0 = x1 - tx * z1;
        double y0 = y1 - ty * z1;
        // Try adding compatible hits.
        std::vector<PrVLHits> hits(m_nStations, PrVLHits());
        PrVLHits allHits; 
        if (m_debug) {
          info() << "  === Seed Phi Pair ===" << endmsg;
          printHit(*ith1, "S1 ");
          printHit(*ith2, "S2 ");
        }
        unsigned int nHitsFound = 0;
        for (int s3 = firstStation; s3 != lastStation + step; s3 += step) {
          if (m_phiHits[s3].empty()) continue;
          PrVLHits::iterator ith3;
          for (ith3 = m_phiHits[s3].begin(); ith3 != m_phiHits[s3].end(); ++ith3) {
            const double x3 = (*ith3)->x();
            const double y3 = (*ith3)->y();
            const double z3 = (*ith3)->z();
            const double dx = x0 + tx * z3 - x3;
            const double dy = y0 + ty * z3 - y3;
            if (fabs(dx) > m_xyTol || fabs(dy) > m_xyTol) continue;
            hits[s3].push_back(*ith3);
            allHits.push_back(*ith3);
            ++nHitsFound;
          } 
        }
        if (nHitsFound < minNbPhi) continue;
        PrVLTrack track;
        track.setPhiClusters(seed, x0, tx, y0, ty, allHits);
        track.updateRParameters();
        // In case of multiple hits on a station, select the best one.
        for (int s3 = firstStation; s3 != lastStation + step; s3 += step) {
          if (hits[s3].size() < 2) continue;
          unsigned int nMax = 1;
          if (fabs(track.xAtHit(hits[s3].front())) < m_xTolOverlap) {
            nMax = 2;
          }
          PrVLHits::iterator ith3;
          PrVLHit* best = hits[s3].front();
          double bestChi2 = track.chi2(best);
          for (ith3 = hits[s3].begin() + 1; ith3 != hits[s3].end(); ++ith3) {
            if (track.nbPhiHits() <= nMax) break;
            if (track.chi2(*ith3) > bestChi2) {
              if (m_debug) printHit(*ith3, " - ");
              track.removePhiHit(*ith3);
            } else {
              if (m_debug) printHit(best, " - ");
              track.removePhiHit(best);
              best = *ith3;
              bestChi2 = track.chi2(best);
            }
          }
        }
        if (track.nbPhiHits() < minNbPhi) continue;
        PrVLHits::iterator ith;
        while (track.phiHits().size() > minNbPhi && track.qFactor() > m_maxQRPhi) {
          double worstChi2 = -1.;
          PrVLHit* worst = 0;
          for (ith = track.phiHits().begin(); ith != track.phiHits().end(); ++ith) {
            const double chi2 = track.chi2(*ith);
            if (chi2 > worstChi2) {
              worstChi2 = chi2;
              worst = *ith;
            }
          }
          if (worst) {
            if (m_debug) printHit(worst, " - ");
            track.removePhiHit(worst);
          }
        }
        const double fUsed = double(track.nbUsedPhiHits()) / track.nbPhiHits();
        if (fUsed > m_fractionUsed) {
          if (m_debug) info() << " >> too many used (" << fUsed << ")" << endmsg;
          continue;
        }
        if (track.qFactor() > m_maxQRPhi) {
          if (m_debug) info() << " >> q = " << track.qFactor() << endmsg;
          continue;
        }
        track.setPhiChi2(track.qFactor());
        tracks.push_back(track);
      }
    }
    if (!tracks.empty() || seed.nbRHits() < 5) break;
  }

  if (tracks.empty()) {
    if (seed.nbRHits() >= m_minToTag) seed.untagUsedRHits();
    return false;
  }
  // Merge tracks if necessary.
  if (tracks.size() > 1) mergeClones(tracks);

  PrVLTracks::iterator itt;
  // Sort the track candidates by the number of Phi hits.
  std::stable_sort(tracks.begin(), tracks.end(),
                   PrVLTrack::DecreasingByPhiLength());

  // Start with the best candidate.
  for (itt = tracks.begin(); itt != tracks.end(); ++itt) {
    if (!(*itt).valid()) continue;
    if (!(*itt).removeWorstRAndPhi(m_maxChi2PerHit, 6)) {
      // Less than three R or Phi hits after cleanup.
      continue;
    }
    if ((*itt).nbRHits() < m_minToTag) (*itt).tagUsedRHits();
    (*itt).tagUsedPhiHits();
    (*itt).setBackward(seed.backward());
    m_spaceTracks.push_back(*itt);
    return true;
  } 
  // No candidate found.
  if (seed.nbRHits() >= m_minToTag) seed.untagUsedRHits();
  return false;

}

//============================================================================
/// Fit the Phi hits in phi/z and xy (r set from RZ track).
/// Can delete hits from the track if bad fit initially.
//============================================================================
bool PrVLTracking::fitProtoTrack(PrVLHits& hits, const unsigned int nMin,
                                 const unsigned int nR,
                                 double& qxy, double& x0, double& y0,
                                 double& tx, double& ty) {

  double qmax = m_maxQXy;
  if (nR > 5 && hits.size() >= nR) qmax = m_maxQXyFull;

  double sx  = 0., sy  = 0.;
  double sxz = 0., syz = 0.;
  double s0  = 0., sz  = 0., szz = 0.;
  if (m_debug) {
    info() << "  === " << hits.size() << " Phi hits ===" << endmsg;
  }
  PrVLHits::iterator ith;
  for (ith = hits.begin(); ith != hits.end(); ++ith) {
    if (m_debug) printHit(*ith, "  ");
    const double x = (*ith)->x();
    const double y = (*ith)->y();
    const double z = (*ith)->z();
    const double w = (*ith)->weight();
    sx += w * x;
    sy += w * y;
    sxz += w * x * z;
    syz += w * y * z;
    s0 += w;
    sz += w * z;
    szz += w * z * z;
  }
  const double det = s0 * szz - sz * sz;
  tx = (sxz * s0 - sx * sz) / det;
  ty = (syz * s0 - sy * sz) / det;
  x0 = (sx * szz - sxz * sz) / det;
  y0 = (sy * szz - syz * sz) / det;
  // Calculate the chi2.
  qxy = 0.;
  double worstQXy = 0.;
  PrVLHits::iterator worstHitXy = hits.end();
  for (ith = hits.begin(); ith != hits.end(); ++ith) {
    const double z = (*ith)->z();
    const double w = (*ith)->weight();
    const double dx = x0 + tx * z - (*ith)->x();
    const double dy = y0 + ty * z - (*ith)->y();
    const double dd = w * (dx * dx + dy * dy);
    qxy += dd;
    if (dd > worstQXy) {
      worstQXy = dd;
      worstHitXy = ith;
    }
  }
  qxy /= (2. * hits.size() - 4.);

  if (qxy > qmax) {
    if (m_debug) info() << "  >> chi2 (x, y) = " << qxy << endmsg;
    if (nMin <= hits.size()) {
      // Try to rescue this combination by dropping one hit.
      hits.erase(worstHitXy);
      // Recurse with shorter phi list.
      return fitProtoTrack(hits, nMin, nR, qxy, x0, y0, tx, ty);
    } else {
      return false;
    }
  }
  // Found OK phi combination
  return true;

}

//============================================================================
/// Sort RZ tracks.
//============================================================================
void PrVLTracking::cleanupRZ() {

  PrVLTracks::iterator itt;
  for (itt = m_rzTracks.begin(); itt != m_rzTracks.end(); ++itt) {
    if ((*itt).backward()) {
      std::sort((*itt).rHits().begin(), (*itt).rHits().end(),
                PrVLHit::DecreasingByZ());
    } else {
      std::sort((*itt).rHits().begin(), (*itt).rHits().end(),
                PrVLHit::IncreasingByZ());
    }
    PrVLHits::iterator ith;
    unsigned int nRight = 0;
    unsigned int nLeft = 0;
    for (ith = (*itt).rHits().begin(); ith != (*itt).rHits().end(); ++ith) {
      if ((*ith)->right()) {
        ++nRight;
      } else {
        ++nLeft;
      }
    }
    if (nLeft > 2 && nRight > 2) {
      (*itt).setOverlap(true);
    } else {
      (*itt).setOverlap(false);
    }
  }
  std::stable_sort(m_rzTracks.begin(), m_rzTracks.end(), 
                   PrVLTrack::DecreasingByRLength());

}

//============================================================================
/// Merge tracks sharing hits, tag clones.
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
        info() << "Tracks " << itt1 - tracks.begin() 
                << " (" << n1 << " hits, q-factor " << (*itt1).qFactor()
                << ") and " << itt2 - tracks.begin() 
                << " (" << n2 << " hits, q-factor " << (*itt2).qFactor()
                << ") share " << nCommon << " Phi hits (clone criterion: " 
                << nMinCommon * m_fractionForMerge << ")" << endmsg;
      } 
      if (nCommon < nMinCommon * m_fractionForMerge) continue;
      if (n2 > n1) {
        if (m_debug) {
          info() << "Invalidating track " << itt1 - tracks.begin() << endmsg; 
        }
        (*itt1).setValid(false);
      } else if (n1 > n2) {
        (*itt2).setValid(false);
        if (m_debug) {
          info() << "Invalidating track " << itt1 - tracks.begin() << endmsg; 
        }
      } else if ((*itt1).qFactor() < (*itt2).qFactor()) {
        if (m_debug) {
          info() << "Invalidating track " << itt2 - tracks.begin() << endmsg;
        }
        (*itt2).setValid(false);
      } else {
        if (m_debug) {
          info() << "Invalidating track " << itt1 - tracks.begin() << endmsg; 
        }
        (*itt1).setValid(false);
      }
    }
  }

}

//============================================================================
/// Merge space track clones
//============================================================================
void PrVLTracking::mergeSpaceClones(const bool forward) {

  const unsigned int nSensors = m_hits.size();
  PrVLTracks::iterator it1;
  PrVLTracks::iterator it2;
  for (it1 = m_spaceTracks.begin(); it1 != m_spaceTracks.end() - 1; ++it1) {
    if ((*it1).backward() == forward) continue;
    if (!(*it1).valid()) continue;
    std::vector<int> strip1(nSensors, -1);
    PrVLHits::const_iterator ith;
    for (ith = (*it1).rHits().begin(); ith != (*it1).rHits().end(); ++ith) {
      const unsigned int sensor = (*ith)->sensor();
      if (sensor < nSensors) strip1[sensor] = (*ith)->strip();
    }
    for (ith = (*it1).phiHits().begin(); ith != (*it1).phiHits().end(); ++ith) {
      const unsigned int sensor = (*ith)->sensor();
      if (sensor < nSensors) strip1[sensor] = (*ith)->strip();
    }
    const double zMid = 0.5 * ((*it1).rHits().front()->z() + 
                               (*it1).rHits().back()->z());
    const double slx = 0.01 * ((*it1).xAtZ(zMid + 100.) - (*it1).xAtZ(zMid));
    const double sly = 0.01 * ((*it1).yAtZ(zMid + 100.) - (*it1).yAtZ(zMid));

    for (it2 = it1 + 1; it2 != m_spaceTracks.end(); ++it2) {
      if ((*it2).backward() == forward) continue;
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

      std::vector<int> strip2(nSensors, -1);
      for (ith = (*it2).rHits().begin(); ith != (*it2).rHits().end(); ++ith) {
        const unsigned int sensor = (*ith)->sensor();
        if (sensor < nSensors) strip2[sensor] = (*ith)->strip();
      }
      for (ith = (*it2).phiHits().begin(); ith != (*it2).phiHits().end(); ++ith) {
        const unsigned int sensor = (*ith)->sensor();
        if (sensor < nSensors) strip2[sensor] = (*ith)->strip();
      }

      // Count how many sensors have hits on both, and how many are in commmon.
      int nRInBoth = 0;
      int nRCommon = 0;
      for (unsigned int k = 0; k < 64; ++k) {
        if (strip1[k] >= 0 && strip2[k] >= 0) {
          ++nRInBoth;
          if (strip1[k] == strip2[k]) ++nRCommon;
        }
      }
      int nPhiInBoth = 0;
      int nPhiCommon = 0;
      for (unsigned int k = 64; k < nSensors; ++k) {
        if (strip1[k] >= 0 && strip2[k] >= 0) {
          ++nPhiInBoth;
          if (strip1[k] == strip2[k]) ++nPhiCommon;
        }
      }

      if (m_debug) {
        info() << nRInBoth << " sensors with R hits on both, " 
                << nRCommon << " with same cluster" << endmsg;
        info() << nPhiInBoth << " sensors with Phi hits on both, " 
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
void PrVLTracking::makeLHCbTracks() {

  Tracks* lhcbTracks = getIfExists<Tracks>(m_trackLocation);
  if (!lhcbTracks) {
    lhcbTracks = new Tracks();
    lhcbTracks->reserve(500);
    put(lhcbTracks, m_trackLocation);
  }
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
/// Try to find tracks with unused R clusters
//============================================================================
void PrVLTracking::findUnusedR(DeVLRSensor* sensor0, const bool forward) {

  if (!sensor0) return;
  const DeVLRSensor* sensor1 = 0;
  if (forward) {
    sensor1 = sensor0->nextRSensor();
  } else {
    sensor1 = sensor0->previousRSensor();
  }
  if (!sensor1) return;
  const unsigned int s0 = sensor0->sensorNumber();
  const unsigned int s1 = sensor1->sensorNumber();
  const double z0 = sensor0->z();
  const double z1 = sensor1->z();
  const int nZones = m_nZonesR;
  for (int zone0 = 0; zone0 < nZones; ++zone0) {
    if (m_hits[s0][zone0].empty()) continue;
    // Loop over the hits in the first sensor.
    PrVLHits::const_iterator it0;
    for (it0 = m_hits[s0][zone0].begin(); it0 != m_hits[s0][zone0].end(); ++it0) {
      // Skip used hits.
      if ((*it0)->nUsed() > 0) continue;
      const double r0 = (*it0)->r();
      // Loop over the hits in the second sensor.
      for (int zone1 = zone0 - 1; zone1 <= zone0 + 1; ++zone1) {
        if (zone1 < 0 || zone1 >= nZones) continue;
        if (m_hits[s1][zone1].empty()) continue;
        PrVLHits::const_iterator it1;
        for (it1 = m_hits[s1][zone1].begin(); it1 != m_hits[s1][zone1].end(); ++it1) {
          // Skip used hits.
          if ((*it1)->nUsed() > 0) continue;
          const double r1 = (*it1)->r();
          if (r1 < r0) continue;
          double slope = (r1 - r0) / (z1 - z0);
          if (fabs(slope) > m_maxRSlope3) continue;
          PrVLHits rHits;
          rHits.push_back(*it0);
          rHits.push_back(*it1);
          // Try to extrapolate from this pair of hits.
          const DeVLRSensor* sensor2 = sensor1;
          const DeVLRSensor* sensor3 = sensor1;
          double r2 = r1;
          double r3 = r1; 
          bool ok = true;
          while (ok) {
            sensor2 = sensor3;
            if (forward) {
              sensor3 = sensor3->nextRSensor();
            } else {
              sensor3 = sensor3->previousRSensor();
            }
            if (!sensor3) break;
            const unsigned int s3 = sensor3->sensorNumber();
            const double z2 = sensor2->z();
            const double z3 = sensor3->z();
            PrVLHit* h2 = rHits.back();
            PrVLHit* h3 = 0;
            r2 = r3;
            r3 += slope * (z3 - z2);
            const double tol = sensor3->rPitchOfRadius(r3);
            double minDist = 0.;
            const int zone2 = h2->rZone();
            for (int zone3 = zone2 - 1; zone3 <= zone2 + 1; ++zone3) {
              if (zone3 < 0 || zone3 >= nZones) continue;
              if (m_hits[s3][zone3].empty()) continue;
              PrVLHit* best = closestRHit(m_hits[s3][zone3], r3, tol, true);
              if (!best) continue;
              if (!h3) {
                h3 = best;
                const double d = fabs(r3 - best->r());
                minDist = d; 
              } else {
                const double d = fabs(r3 - best->r());
                if (d < minDist) {
                  minDist = d;
                  h3 = best;
                }
              }
            }
            // Stop search if no hit found.
            if (!h3) break;
            rHits.push_back(h3);
            r3 = h3->r();
            slope = (r3 - r2) / (z3 - z2);
          }
          if (rHits.size() < 3) continue;
          PrVLTrack newTrack;
          PrVLHits::iterator ith;
          for (ith = rHits.begin(); ith != rHits.end(); ++ith) {
            newTrack.addRHit(*ith);
          }
          newTrack.setBackward(!forward);
          newTrack.tagUsedRHits();
          m_rzTracks.push_back(newTrack);
        }
      }
    }
  } 

}

//============================================================================
/// Try to find tracks with unused phi clusters, only forward
//============================================================================
void PrVLTracking::findUnusedPhi() {

  if (msgLevel(MSG::DEBUG)) debug() << " ==> findUnusedPhi()" << endmsg;
  double tol1 = m_phiUnusedFirstTol;
  double tol2 = m_phiUnusedSecondTol;
  unsigned int firstSensor = m_det->phiSensors().back()->sensorNumber();
  unsigned int lastSensor = firstSensor - 15;

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
    for (unsigned int phiZone0 = 0; phiZone0 < m_nZonesPhi - 1; ++phiZone0) {
      PrVLHits& hits0 = m_hits[p0][phiZone0]; 
      for (ith0 = hits0.begin(); ith0 != hits0.end(); ++ith0) {
        if ((*ith0)->nUsed() >= (*ith0)->nMaxUsed()) continue;
        const double x0 = (*ith0)->xStripCentre();
        const double y0 = (*ith0)->yStripCentre();
        for (unsigned int phiZone1 = 0; phiZone1 <= phiZone0; ++phiZone1) {
          PrVLHits& hits1 = m_hits[p1][phiZone1];
          for (ith1 = hits1.begin(); ith1 != hits1.end(); ++ith1) {
            if ((*ith1)->nUsed() >= (*ith0)->nMaxUsed()) continue;
            if (fabs((*ith1)->distance(x0, y0)) > tol1) continue;
            const double xSeed = 0.5 * (x0 + (*ith1)->xStripCentre());
            const double ySeed = 0.5 * (y0 + (*ith1)->yStripCentre());
            for (unsigned int phiZone2 = 0; phiZone2 <= phiZone1; ++phiZone2) {
              PrVLHits& hits2 = m_hits[p2][phiZone2];
              for (ith2 = hits2.begin(); ith2 != hits2.end(); ++ith2) {
                if ((*ith2)->nUsed() >= (*ith2)->nMaxUsed()) continue;
                if (fabs((*ith2)->distance(xSeed, ySeed)) > tol2) continue;
                if (m_debug) {
                  info() << "  === Seed Phi Triplet ===" << endmsg;
                  printHit(*ith0, "S0 ");
                  printHit(*ith1, "S1 ");
                  printHit(*ith2, "S2 ");
                }
                // Determine the zone in the R sensor.
                Gaudi::XYZPoint point0(xSeed, ySeed, rSensor0->z());
                point0 = rSensor0->globalToLocal(point0);
                unsigned int rZone0 = rSensor0->zoneOfPhiAndR(point0.phi(), 
                                                              point0.Rho());
                PrVLHits& rHits0 = m_hits[r0][rZone0];
                int spaceZone = rZone0;
                if (xSeed < 0) spaceZone += m_nZonesR;
                for (itr0 = rHits0.begin(); itr0 != rHits0.end(); ++itr0) {
                  if ((*itr0)->r() < phiSensor0->rMin(phiZone0) ||
                      (*itr0)->r() > phiSensor0->rMax(phiZone0)) {
                    continue;
                  }
                  (*itr0)->setStartingPoint(xSeed, ySeed);
                  int nbUnused = 0;
                  if (0 == (*itr0)->nUsed()) ++nbUnused;
                  PrVLTrack track;
                  track.setPhiClusters(*itr0, spaceZone, *ith0, *ith1, *ith2);
                  track.updatePhiWeights();
                  const double z1 = rSensor1->z();
                  const double x1 = track.xAtZ(z1) - rSensor1->xCentre();
                  const double y1 = track.yAtZ(z1) - rSensor1->yCentre();
                  Gaudi::XYZPoint point1(x1, y1, z1);
                  point1 = rSensor1->globalToLocal(point1);
                  unsigned int rZone1 = rSensor1->zoneOfPhiAndR(point1.phi(),
                                                                point1.Rho());
                  PrVLHits& rHits1 = m_hits[r1][rZone1]; 
                  if (!track.addBestRCluster(rHits1, z1, m_maxChi2ToAdd)) continue;
                  track.updatePhiWeights();
                  if (0 == track.rHits().back()->nUsed()) ++nbUnused;
                  const double z2 = rSensor2->z();
                  const double x2 = track.xAtZ(z2) - rSensor2->xCentre();
                  const double y2 = track.yAtZ(z2) - rSensor2->yCentre();
                  Gaudi::XYZPoint point2(x2, y2, z2);
                  point2 = rSensor2->globalToLocal(point2);
                  unsigned int rZone2 = rSensor2->zoneOfPhiAndR(point2.phi(),
                                                                point2.Rho());
                  PrVLHits& rHits2 = m_hits[r2][rZone2]; 
                  if (!track.addBestRCluster(rHits2, z2, m_maxChi2ToAdd)) continue;
                  if (0 == track.rHits().back()->nUsed()) ++nbUnused;
                  if (nbUnused < 2) continue;
                  if (track.qFactor() > m_maxQRPhi) continue;
                  const DeVLRSensor* rSensor = rSensor2;
                  int nMiss = 0;
                  while (rSensor && nMiss <= m_maxMissed) {
                    rSensor = rSensor->previousRSensor();
                    if (!rSensor) break;
                    const double z = rSensor->z();
                    if (track.rAtZ(z) < rSensor->rMin(rZone0)) break;
                    const double x = track.xAtZ(z) - rSensor->xCentre();
                    const double y = track.yAtZ(z) - rSensor->yCentre();
                    Gaudi::XYZPoint point(x, y, z);
                    point = rSensor->globalToLocal(point);
                    unsigned int rZone = rSensor->zoneOfPhiAndR(point.phi(),
                                                                point.Rho());
                    PrVLHits& rHits = m_hits[rSensor->sensorNumber()][rZone]; 
                    if (!track.addBestRCluster(rHits, z, m_maxChi2ToAdd)) ++nMiss;
                    const DeVLPhiSensor* phiSensor = rSensor->associatedPhiSensor();
                    const double rInPhi = track.rAtZ(phiSensor->z());
                    const unsigned int phiZone = phiSensor->zoneOfRadius(rInPhi); 
                    PrVLHits& phiHits = m_hits[phiSensor->sensorNumber()][phiZone];
                    PrVLHits::iterator itp;
                    for (itp = phiHits.begin(); itp != phiHits.end(); ++itp) {
                      (*itp)->setWeight(rInPhi);
                    }
                    if (!addPhiHit(phiHits, track, m_maxChi2ToAdd)) ++nMiss;
                  }
                  if (!track.removeWorstRAndPhi(m_maxChi2PerHit, 6)) continue;
                  if (m_debug) {
                    info() << "    ==== accepted q-factor = " 
                            << track.qFactor() << " ====" << endmsg;
                  }
                  track.tagUsedRHits();
                  track.tagUsedPhiHits();
                  m_spaceTracks.push_back(track);
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

  if (msgLevel(MSG::DEBUG)) debug() << " ==> buildHits()" << endmsg;
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
    info() << "Retrieved " << liteClusters->size() << " clusters" << endmsg;
  }
  if (liteClusters->size() > m_pool.size()) m_pool.resize(liteClusters->size() + 100);
  VLLiteCluster::VLLiteClusters::iterator it;
  for (it = liteClusters->begin(); it != liteClusters->end(); ++it) {
    // Get the next object in the pool.
    PrVLHit* hit = &(*(m_nextInPool++));
    VLChannelID channel = (*it).channelID();
    const unsigned int sensor = channel.sensor();
    const unsigned int strip = channel.strip();
    double f = (*it).interStripFraction();
    hit->setMaxUsed(1);
    unsigned int zone = 0;
    if (channel.isPhiType()) {
      const DeVLPhiSensor* phiSensor = m_det->phiSensor(sensor);
      zone = phiSensor->globalZoneOfStrip(strip);
      const bool right = phiSensor->isRight();
      const double pitch = phiSensor->phiPitchOfStrip(strip);
      const double weight = 12. / (pitch * pitch);
      // Get the line parameterisation and strip centre in global coordinates.
      double a, b, c;
      double xs, ys;
      if (f > 0.) {
        double a1, b1, c1;
        double xs1, ys1;
        phiSensor->lineParameters(strip, a1, b1, c1, xs1, ys1);
        double a2, b2, c2;
        double xs2, ys2;
        phiSensor->lineParameters(strip, a2, b2, c2, xs2, ys2);
        a = (1. - f) * a1 + f * a2;
        b = (1. - f) * b1 + f * b2;
        c = (1. - f) * c1 + f * c2;
        xs = (1. - f) * xs1 + f * xs2;
        ys = (1. - f) * ys1 + f * ys2;
      } else {
        phiSensor->lineParameters(strip, a, b, c, xs, ys);
      }
      // Find the R sector corresponding to this strip.
      unsigned int rzone = 999;
      if (fabs(xs) > 2.) {
        const double rs = sqrt(xs * xs + ys * ys);
        float phiS = phiSensor->phiOfStrip(strip, f, rs);
        if (!phiSensor->isDownstream()) phiS = -phiS;
        if (phiSensor->isRight()) phiS += Gaudi::Units::pi;
        for (unsigned int i = 2 * m_nZonesR; i--;) {
          // Get the bisector of the R zone.
          const double phiR = m_ctrPhi[i];
          // Compute the sine of the angle between hit centre and R zone bisector.
          if (fabs(phiR - phiS) < 0.8 * m_tolPhi[i]) {
            rzone = i;
            break;
          }
        }
      }
      hit->setHit(*it, rzone, zone, right, phiSensor->z(), 0, weight);
      hit->setLineParameters(a, b, c, xs, ys);
    } else if (channel.isRType()) {
      const DeVLRSensor* rSensor = m_det->rSensor(sensor);
      zone = rSensor->globalZoneOfStrip(strip);
      const unsigned int phizone = 0;
      const bool right = rSensor->isRight();
      const double radius = rSensor->rOfStrip(strip, f);
      double pitch = rSensor->rPitchOfRadius(radius);
      const double weight = 12. / (pitch * pitch);
      hit->setHit(*it, zone, phizone, right, rSensor->z(), radius, weight);
    } 
    m_hits[sensor][zone].push_back(hit);
  }
  // Sort the R hits.
  for (unsigned int i = 0; i < m_hits.size(); ++i) {
    if (!m_det->sensor(i)) continue;
    if (m_det->sensor(i)->isPhi()) continue;
    for (unsigned int j = 0; j < m_nZonesR; ++j) {
      std::sort(m_hits[i][j].begin(), m_hits[i][j].end(),
                PrVLHit::IncreasingByR());
    }
  }
  m_nextInPool = m_pool.begin();

  if (!m_debug) return;
  const std::vector<DeVLRSensor*>& rSensors = m_det->rSensors();
  const std::vector<DeVLPhiSensor*>& phiSensors = m_det->phiSensors();
  if (m_debug && m_wantedKey >= 0) {
    // Print the hits associated to the requested track.
    info() << "R hits on track " << m_wantedKey << endmsg;
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
    info() << "Phi hits on track " << m_wantedKey << endmsg;
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
void PrVLTracking::extendTrack(PrVLTrack& track, unsigned int sensor,
                               const unsigned int zone, const bool fwd) {

  // Extrapolate until too many consecutive hits missed.
  int nMiss = 0;
  const DeVLRSensor* rSensor = m_det->rSensor(sensor);
  while (nMiss <= m_maxMissed) {
    if (fwd) {
      rSensor = rSensor->previousRSensor();
    } else {
      rSensor = rSensor->nextRSensor();
    }
    if (!rSensor) break;
    sensor = rSensor->sensorNumber();
    const double z = rSensor->z();
    // Calculate the extrapolated radius.
    const double rPred = track.rPred(z);
    if (rPred < rSensor->rMin(zone) || rPred > rSensor->rMax(zone)) {
      // Track intercept is outside the zone.
      break;
    }
    const double rPitch = rSensor->rPitchOfRadius(rPred);
    double tol = m_rTolExtra * sqrt(track.rErr2(z)) + rPitch;
    // Check if the sensor has a matching hit.
    PrVLHit* best = closestRHit(m_hits[sensor][zone], rPred, tol, false);
    if (!best) {
      // No matching hit.
      ++nMiss;
      continue;
    }
    nMiss = 0;
    track.addRHit(best);
    if (m_debug) printHit(best, "    +  ");
  }
  track.setMissedSensors(nMiss + track.missedSensors());
  
}

//============================================================================
/// Add hits from the other side of the detector (overlap tracks)
//============================================================================
void PrVLTracking::extendTrackOtherSide(PrVLTrack& track, unsigned int sensor,
                                        unsigned int zone, const bool fwd, 
                                        const bool next) {
   
  int nMiss = 0;
  // Get the R sensor on the opposite side.
  const DeVLRSensor* rSensor = m_det->rSensor(sensor)->otherSideRSensor();
  if (next) { 
    if (fwd) {
      rSensor = rSensor->previousRSensor();
    } else {
      rSensor = rSensor->nextRSensor();
    }
  }
  // Since zones are ordered in global phi, we have to flip.
  zone = m_nZonesR - 1 - zone;
  // Extrapolate until too many consecutive hits missed.
  PrVLHits extraHits;
  while (nMiss <= m_maxMissed) {
    if (!rSensor) break;
    sensor = rSensor->sensorNumber();
    const double z = rSensor->z();
    // Calculate the extrapolated radius.
    const double rPred = track.rPred(z);
    if (rPred < rSensor->rMin(zone) || rPred > rSensor->rMax(zone)) {
      // Track intercept is outside the zone.
      break;
    }
    const double rPitch = rSensor->rPitchOfRadius(rPred);
    double tol = m_rTolOverlap * rPitch;
    // Check if the sensor has a matching hit.
    PrVLHit* best = closestRHit(m_hits[sensor][zone], rPred, tol, false);
    if (best) {
      extraHits.push_back(best);
      nMiss = 0;
    } else {
      ++nMiss;
    }
    if (fwd) {
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
bool PrVLTracking::addPhiHit(PrVLHits& hits, PrVLTrack& track, 
                             const double maxChi2) {

  if (hits.empty()) return false;
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
  if (bestChi2 > maxChi2) return false;
  track.addPhiHit(best);
  if (m_debug) printHit(best, "   + ");
  return true;

}

//============================================================================
/// Get the closest R hit in the specified window.
//============================================================================
PrVLHit* PrVLTracking::closestRHit(PrVLHits& hits, 
                                   const double r, const double tol,
                                   const bool unused) {

  double minDist = tol;
  PrVLHit* best = 0;
  for (PrVLHits::const_iterator it = hits.begin(); it != hits.end(); ++it) {
    const double d = (*it)->r() - r;
    if (d < -minDist) continue;
    if (d >  minDist) break;
    if (unused && (*it)->nUsed() >= (*it)->nMaxUsed()) continue;
    minDist = fabs(d);
    best = *it;
  }
  return best;

}

//============================================================================
/// Get the closest Phi hit in the specified window.
//============================================================================
PrVLHit* PrVLTracking::closestPhiHit(PrVLHits& hits, 
                                     const double phi, const double tol,
                                     const bool unused) {

  double minDist = tol;
  PrVLHit* best = 0;
  for (PrVLHits::const_iterator it = hits.begin(); it != hits.end(); ++it) {
    const double d = (*it)->phi() - phi;
    if (d < -minDist) continue;
    if (d >  minDist) break;
    if (unused && (*it)->nUsed() >= (*it)->nMaxUsed()) continue;
    minDist = fabs(d);
    best = *it;
  }
  return best;

}

//============================================================================
/// Get the closest Phi hit in the specified window on the other half.
//============================================================================
PrVLHit* PrVLTracking::closestPhiHitOtherSide(PrVLHits& hits, 
                                              const double phi, const double tol,
                                              const bool unused, 
                                              const unsigned int sensor) {

  double minDist = tol;
  PrVLHit* best = 0;
  for (PrVLHits::const_iterator it = hits.begin(); it != hits.end(); ++it) {
    if ((*it)->sensor() == sensor) continue;
    const double d = (*it)->phi() - phi;
    if (d < -minDist) continue;
    if (d >  minDist) break;
    if (unused && (*it)->nUsed() >= (*it)->nMaxUsed()) continue;
    minDist = fabs(d);
    best = *it;
  }
  return best;

}

//============================================================================
/// Get the closest Phi hit in the specified window.
//============================================================================
PrVLHit* PrVLTracking::closestPhiHit(PrVLHits& hits, 
                                     const double x, const double y,
                                     const double tol,
                                     const bool unused) {

  const double tol2 = tol * tol;
  double d2Min = 2 * tol2;
  PrVLHit* best = 0;
  for (PrVLHits::const_iterator it = hits.begin(); it != hits.end(); ++it) {
    const double dx = (*it)->x() - x;
    if (fabs(dx) > tol) continue;
    const double dy = (*it)->y() - y;
    if (fabs(dy) > tol) continue;
    const double d2 = dx * dx + dy * dy;
    if (d2 > tol2) continue;
    if (d2 > d2Min) continue;
    if (unused && (*it)->nUsed() >= (*it)->nMaxUsed()) continue;
    d2Min = d2;
    best = *it;
  }
  return best;

}

//=============================================================================
/// Print coordinates of a hit
//=============================================================================
void PrVLTracking::printHit(const PrVLHit* hit, const std::string title) {

  if (!m_debug) return;
  const unsigned int zone = hit->sensor() < 64 ? hit->rZone() : hit->phiZone();
  info() << title 
         << format("sensor %3d z %5.1f zone %1d strip %4d + %3.2f radius %6.3f phi %5.3f x %6.3f y %6.3f used %1d/%1d ",
                   hit->sensor(), hit->z(), zone, hit->strip(), 
                   hit->fraction(), hit->r(), hit->phi(),
                   hit->x(), hit->y(),
                   hit->nUsed(), hit->nMaxUsed());
  LHCbID id = hit->lhcbID();
  if (m_debugTool) {
    m_debugTool->printKey(info(), id);
    if (matchKey(hit)) info() << " ***";
  }
  if (hit->adcHigh()) info() << " H";
  info() << endmsg;

}

//=============================================================================
/// Print hits on an RZ track 
//=============================================================================
void PrVLTracking::printRZTrack(PrVLTrack& track) {

  PrVLHits::iterator it;
  for (it = track.rHits().begin(); it != track.rHits().end(); ++it) {
    const double d = track.rPred((*it)->z()) - (*it)->r();
    info() << format("Dist%8.3f Chi2%8.2f", d, d * d * (*it)->weight());
    printHit(*it, " ");
  }

}

//=============================================================================
/// Print hits on a track 
//=============================================================================
void PrVLTracking::printTrack(PrVLTrack& track) {

  info() << "Zone " << track.rZone() << ", ";
  if (track.backward()) {
    info() << "backward";
  } else {
    info() << "forward";
  }
  info() << endmsg;
  PrVLHits::iterator it;
  for (it = track.rHits().begin(); it != track.rHits().end(); ++it) {
    printHit(*it, "R   ");
  }
  for (it = track.phiHits().begin(); it != track.phiHits().end(); ++it) {
    printHit(*it, "PHI ");
  }

} 

