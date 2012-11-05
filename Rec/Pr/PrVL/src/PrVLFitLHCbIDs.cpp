// Gaudi
#include "GaudiKernel/ToolFactory.h" 
// Event/TrackEvent
#include "Event/Track.h"
// Event/DigiEvent
#include "Event/VLLiteCluster.h"
// Det/VLDet
#include "VLDet/DeVL.h"
// Local
#include "PrVLFitLHCbIDs.h"
#include "PrVLTrack.h"

DECLARE_TOOL_FACTORY(PrVLFitLHCbIDs)

/** @file PrVLFitLHCbIDs.cpp
 *
 *  Implementation of class : PrVLFitLHCbIDs
 *
 */

//=============================================================================
/// Standard constructor, initializes variables
//=============================================================================
PrVLFitLHCbIDs::PrVLFitLHCbIDs(const std::string& type,
                               const std::string& name,
                               const IInterface* parent)
  : GaudiTool(type, name, parent) {

  declareInterface<ITrackFitter>(this);
  declareProperty("MSFactor", m_msFactor    = 0.95);
  declareProperty("StateAtBeam", m_stateAtBeam = true);

  m_pool.resize(1000);
  m_nextInPool = m_pool.begin();

}

//=============================================================================
/// Initialization
//=============================================================================
StatusCode PrVLFitLHCbIDs::initialize() {

  StatusCode sc = GaudiTool::initialize();
  if (!sc) return sc;

  m_det = getDet<DeVL>(DeVLLocation::Default);
  // Get the number of R and phi sectors.
  m_nZonesR = m_det->rSensors().front()->numberOfZones();
  m_nZonesPhi = m_det->phiSensors().front()->numberOfZones();
  return StatusCode::SUCCESS;

}

//=============================================================================
/// Fit a single track. The second argument is ignored.
//=============================================================================
StatusCode PrVLFitLHCbIDs::fit(LHCb::Track & track, LHCb::ParticleID) {

  int nRight = 0, nLeft = 0;
  PrVLHits phiHits;
  std::vector<int> nHitsInZone(m_nZonesR, 0);
  PrVLTrack fastTrack;
  double zMin =  1.e9;
  double zMax = -1.e9;
  std::vector<LHCb::LHCbID>::const_iterator it;
  m_nextInPool = m_pool.begin();
  if (m_pool.size() > track.lhcbIDs().size()) {
    m_pool.resize(track.lhcbIDs().size());
  } 
  for (it = track.lhcbIDs().begin(); it != track.lhcbIDs().end(); ++it) {
    if (!it->isVL()) {
      error() << "Invalid ID (" << *it << ")" << endmsg;
      continue;
    }
    LHCb::VLChannelID channel = (*it).vlID();
    unsigned int sensor = channel.sensor();
    unsigned int strip = channel.strip();
    if (m_det->sensor(sensor)->isRight()) {
      ++nRight;
    } else {
      ++nLeft;
    }
    // Make a lite cluster.
    const LHCb::VLLiteCluster liteCluster(channel, 0., false);
    PrVLHit* hit = &(*(m_nextInPool++));
    if (channel.isPhiType()) {
      const DeVLPhiSensor* phiSensor = m_det->phiSensor(sensor);
      const unsigned int zone = phiSensor->globalZoneOfStrip(strip);
      const double pitch = phiSensor->phiPitchOfStrip(strip);
      const double weight = 12. / (pitch * pitch);
      hit->setHit(liteCluster, zone, phiSensor->z(), 0, weight);
      double a, b, c;
      double xs, ys;
      phiSensor->lineParameters(strip, a, b, c, xs, ys);
      hit->setLineParameters(a, b, c, xs, ys);
      phiHits.push_back(hit);
    } else if (channel.isRType()) {
      const DeVLRSensor* rSensor = m_det->rSensor(sensor);
      const unsigned int zone = rSensor->globalZoneOfStrip(strip);
      const double radius = rSensor->rOfStrip(strip);
      double pitch = rSensor->rPitchOfStrip(strip);
      const double weight = 12. / (pitch * pitch);
      hit->setHit(liteCluster, zone, rSensor->z(), radius, weight);
      hit->setGlobalRadius(radius);
      ++nHitsInZone[zone];
      fastTrack.addRHit(hit);
    }
    if (hit->z() > zMax) zMax = hit->z();
    if (hit->z() < zMin) zMin = hit->z();
  }
  // Get the zone of the track (majority of R hits).
  unsigned int zone = 0;
  for (unsigned int i = 0; i < m_nZonesR; ++i) {
    if (nHitsInZone[i] > nHitsInZone[zone]) zone = i;
  }
  if (nRight > nLeft) zone += m_nZonesR;
  const double dphi = Gaudi::Units::pi / double(m_nZonesR);
  double phi = -0.5 * Gaudi::Units::pi + (0.5 + zone) * dphi;
  double cphi = cos(phi);
  double sphi = sin(phi);
  if (nLeft > 2 && nRight > 2) {
    // Overlap region
    cphi = 0.;
    sphi /= fabs(sphi);
  }
  fastTrack.setSpaceParametersFromR(cphi, sphi);
  fastTrack.addPhiClusters(phiHits);
  fastTrack.updateRParameters();
  fastTrack.fitTrack( );
  double zBeam = fastTrack.zBeam();
  LHCb::State state;
  if (m_stateAtBeam) {
    state.setLocation(LHCb::State::ClosestToBeam);
    state.setState(fastTrack.state(zBeam));
    state.setCovariance(fastTrack.covariance(zBeam));
  } else {
    state.setLocation(LHCb::State::FirstMeasurement);
    state.setState(fastTrack.state(zMin));
    state.setCovariance(fastTrack.covariance(zMin));
  }
  track.addToStates(state);

  if (!track.checkFlag(LHCb::Track::Backward)) {
    state.setLocation(LHCb::State::EndVelo);
    state.setState(fastTrack.state(zMax));
    state.setCovariance(fastTrack.covariance(zMax));
    track.addToStates(state);
  }
  return StatusCode::SUCCESS;

}
