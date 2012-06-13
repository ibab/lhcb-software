#define VELOLITEDET_DEVELOLITEPHITYPE_CPP 1

#include "gsl/gsl_math.h"
#include <fstream>
// Gaudi
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/IUpdateManagerSvc.h"
// LHCb
#include "LHCbMath/LHCbMath.h"
#include "Kernel/LineTraj.h"
#include "Kernel/VeloLiteChannelID.h"
// Local 
#include "VeloLiteDet/DeVeloLitePhiType.h"

namespace VeloLiteDet {
  /** Access to local static data
   *
   *  @see DeVeloPhiType
   */
  static std::vector<phiStrip>& deVeloLitePhiTypeStaticPhiStrips() {
    static std::vector<phiStrip> s_strips;
    return s_strips;
  }
  static std::vector<phiZone>& deVeloLitePhiTypeStaticPhiZones() {
    static std::vector<phiZone> s_zones;
    return s_zones;
  }

}

/** @file DeVeloLitePhiType.cpp
 *
 *  Implementation of class : DeVeloLitePhiType
 *
 */

// ============================================================================
/// Constructor
// ============================================================================
DeVeloLitePhiType::DeVeloLitePhiType(const std::string& name) : 
  DeVeloLiteSensor(name),
  m_zones(VeloLiteDet::deVeloLitePhiTypeStaticPhiZones()),
  m_strips(VeloLiteDet::deVeloLitePhiTypeStaticPhiStrips()) {
  
}

// ============================================================================
/// Object identification
// ============================================================================
const CLID& DeVeloLitePhiType::clID() const {

  return DeVeloLitePhiType::classID();

}

// ============================================================================
/// Initialisation
// ============================================================================
StatusCode DeVeloLitePhiType::initialize() {

  /// Set the output level.
  PropertyMgr* pmgr = new PropertyMgr();
  int outputLevel = 0;
  pmgr->declareProperty("OutputLevel", outputLevel);
  IJobOptionsSvc* jobSvc;
  ISvcLocator* svcLoc = Gaudi::svcLocator();
  StatusCode sc = svcLoc->service("JobOptionsSvc", jobSvc);
  if (sc.isSuccess()) sc = jobSvc->setMyProperties("DeVeloLitePhiType", pmgr);
  if (outputLevel > 0) {
    msgSvc()->setOutputLevel("DeVeloLitePhiType", outputLevel);
  }
  delete pmgr;
  if (!sc) return sc;
  m_debug   = (msgSvc()->outputLevel("DeVeloLitePhiType") == MSG::DEBUG);
  m_verbose = (msgSvc()->outputLevel("DeVeloLitePhiType") == MSG::VERBOSE);
  if (m_verbose) m_debug = true;
  MsgStream msg(msgSvc(), "DeVeloLitePhiType");

  /// Initialise the base class.
  sc = DeVeloLiteSensor::initialize();
  if (!sc.isSuccess()) {
    msg << MSG::ERROR << "Failed to initialise DeVeloLiteSensor." << endmsg;
    return sc;
  }
  /// Initialise the sensor from the XML.
  sc = initSensor();
  if (!sc.isSuccess()) {
    msg << MSG::ERROR << "Failed to initialise DeVeloLitePhiType." << endmsg;
  }
  /// Build up map of strips to routing lines.
  buildRoutingLineMap();
  /// Register geometry conditions, update strip cache.
  updMgrSvc()->registerCondition(this, this->m_geometry,
                                 &DeVeloLitePhiType::updateGeometryCache);
  /// First update
  sc = updMgrSvc()->update(this);
  if (!sc.isSuccess()) {
    msg << MSG::ERROR << "Failed to update geometry cache." << endmsg;
    return sc;
  }
  return StatusCode::SUCCESS;

}

//=============================================================================
/// Initialisation from XML
//=============================================================================
StatusCode DeVeloLitePhiType::initSensor() {

  MsgStream msg(msgSvc(), "DeVeloLitePhiType");

  /// Number of zones
  m_numberOfZones = param<int>("PhiNbZones");
  /// Number of strips in each zone
  std::vector<int> nbStrips = paramAsIntVect("PhiNbStrips");
  /// Angular coverage in each zone
  std::vector<double> coverage = paramAsDoubleVect("PhiCoverage");
  /// Min. and max. radius of each zone
  std::vector<double> rMinZone = paramAsDoubleVect("PhiMinRadius");
  std::vector<double> rMaxZone = paramAsDoubleVect("PhiMaxRadius");
  /// Stereo angles of each zone
  std::vector<double> stereoAngles = paramAsDoubleVect("PhiStereoAngles");
  /// Phi offset
  m_phiOrigin = param<double>("PhiOrigin");
  m_phiOrigin -= Gaudi::Units::halfpi;
  /// Resolution of the sensor
  m_resolution.first = param<double>("PhiResGrad");
  m_resolution.second = param<double>("PhiResConst");

  /// Check the size of the vectors retrieved from XML.
  if (nbStrips.size()     != m_numberOfZones ||
      coverage.size()     != m_numberOfZones ||
      rMinZone.size()     != m_numberOfZones ||
      rMaxZone.size()     != m_numberOfZones ||
      stereoAngles.size() != m_numberOfZones) {
    msg << MSG::ERROR 
        << "Mismatch between number of zones and array sizes." << endmsg;
    return StatusCode::FAILURE;
  }

  /// Count the total number of strips.
  m_numberOfStrips = 0;
  for (unsigned int zone = 0; zone < m_numberOfZones; ++zone) {
    m_numberOfStrips += nbStrips[zone];
  }
    
  /// Setup the zone parameters.
  m_zones.resize(m_numberOfZones);
  int stripIndex = 0;
  for (unsigned int zone = 0; zone < m_numberOfZones; ++zone) {
    m_zones[zone].firstStrip = stripIndex;
    m_zones[zone].nbStrips = nbStrips[zone];
    m_zones[zone].coverage = coverage[zone];
    /// Calculate the phi pitch.
    m_zones[zone].pitch = m_zones[zone].coverage / m_zones[zone].nbStrips;
    /// Set the inner and outer radius of the zone.
    m_zones[zone].rMin = rMinZone[zone];
    m_zones[zone].rMax = rMaxZone[zone];
    /// Set the stereo angle.
    m_zones[zone].tilt = stereoAngles[zone];
    /// Calculate dist. to origin 
    /// (defined by angle between extrapolated strip and phi)
    m_zones[zone].distToOrigin = m_zones[zone].rMin * sin(stereoAngles[zone]);
    double phiInner = 0.;
    if (0 == zone) {
      phiInner = m_phiOrigin;
    } else {
      phiInner = m_zones[zone - 1].tilt - 
                 asin(m_zones[zone - 1].distToOrigin / m_zones[zone].rMin);
    }
    m_zones[zone].tilt += phiInner;
    double phiOuter = m_zones[zone].tilt - 
                      asin(m_zones[zone].distToOrigin / m_zones[zone].rMax);
    if (m_debug) {
      msg << MSG::DEBUG 
          << "Zone " << zone << ": Phi (degree)"
          << " inner " << phiInner / Gaudi::Units::degree
          << " outer " << phiOuter / Gaudi::Units::degree << endmsg;
    }
    stripIndex += m_zones[zone].nbStrips;
  }
  
  /// Setup the strip parameters.
  m_stripLimits.clear();
  m_strips.resize(m_numberOfStrips);
  stripIndex = 0;
  std::ofstream stripfile;
  if (m_debug) stripfile.open("phistrips.txt", std::ios::out);
  for (unsigned int zone = 0; zone < m_numberOfZones; ++zone) {
    for (unsigned int strip = 0; strip < m_zones[zone].nbStrips; ++strip) {
      const double r1 = m_zones[zone].rMin;
      const double r2 = m_zones[zone].rMax;
      /// Determine the beginning and end points of the strip.
      double x1 = r1 * cos(phiOfStrip(stripIndex, 0., r1));
      double y1 = r1 * sin(phiOfStrip(stripIndex, 0., r1));
      double x2 = r2 * cos(phiOfStrip(stripIndex, 0., r2));
      double y2 = r2 * sin(phiOfStrip(stripIndex, 0., r2)); 
      const double gradient = (y2 - y1) /  (x2 - x1);
      const double intercept = y2 - (gradient * x2);
      /// Check if the end points are outside the bounding box.
      if (y2 > boundingBoxY()) {
        y2 = boundingBoxY();
        // Determine the x coordinate.
        x2 = (y2 - intercept) / gradient;
      } else if (y2 < -boundingBoxY()) {
        y2 = -boundingBoxY();
        // Determine the x coordinate.
        x2 = (y2 - intercept) / gradient;
      }
      if (x2 > boundingBoxX()) {
        x2 = boundingBoxX();
        // Determine the y coordinate.
        y2 = gradient * x2 + intercept;
      }
      /// Calculate the length of the strip.
      const double dx = x2 - x1;
      const double dy = y2 - y1;
      const double length = sqrt(dx * dx + dy * dy);
      m_strips[stripIndex].zone = zone;
      m_strips[stripIndex].gradient = gradient;
      m_strips[stripIndex].intercept = intercept;
      m_strips[stripIndex].length = length;
      if (m_debug) {
        stripfile << stripIndex << "  " << x1 << "  " << y1 << "  "
                                        << x2 << "  " << y2 << "\n";
      }
      Gaudi::XYZPoint begin(x1, y1, 0.);
      Gaudi::XYZPoint end(x2, y2, 0.);
      if (m_verbose) {
        msg << MSG::VERBOSE 
            << "Sensor " << sensorNumber() << " " 
            << x1 << " " << y1 << " " << x2 << " " << y2 << endmsg;
      }
      m_stripLimits.push_back(std::pair<Gaudi::XYZPoint, Gaudi::XYZPoint>(begin, end));
      ++stripIndex;
    }
  }
  if (m_debug) stripfile.close();
  return StatusCode::SUCCESS;

}

// ============================================================================
/// Calculate the nearest channel to a 3-d point.
// ============================================================================
StatusCode DeVeloLitePhiType::pointToChannel(const Gaudi::XYZPoint& point,
                                             LHCb::VeloLiteChannelID& channel,
                                             double& fraction,
                                             double& pitch) const {

  // Transform to the local frame.                                         
  Gaudi::XYZPoint localPoint = this->geometry()->toLocal(point);
  // Check boundaries.
  StatusCode sc = isInActiveArea(localPoint);
  if (!sc.isSuccess()) return sc;
  
  const double radius = localPoint.Rho();
  const double phi = localPoint.phi() - phiOffset(radius);
  const unsigned int zone = zoneOfRadius(radius);

  // Find the nearest strip.
  const double strip = m_zones[zone].firstStrip + phi / m_zones[zone].pitch;
  int closestStrip = LHCb::Math::round(strip);
  fraction = strip - closestStrip;
  pitch = phiPitch(radius);

  const unsigned int sensor = sensorNumber();
  // Set the VeloLiteChannelID.
  channel.setSensor(sensor);
  channel.setStrip(closestStrip);
  channel.setType(LHCb::VeloLiteChannelID::PhiType);

  if (m_verbose) {
    MsgStream msg(msgSvc(), "DeVeloLitePhiType");
    msg << MSG::VERBOSE 
        << "pointToChannel; local phi " << localPoint.phi() / Gaudi::Units::degree
        << " radius " << localPoint.Rho()
        << " phiOffset " << phiOffset(radius) / Gaudi::Units::degree
        << " phi corrected " << phi / Gaudi::Units::degree << endmsg;
    msg << MSG::VERBOSE 
        << " strip " << strip << " closest strip " << closestStrip
        << " fraction " << fraction << endmsg;
  }
  return StatusCode::SUCCESS;

}

// ============================================================================
/// Get the nth nearest neighbour for a given channel
// ============================================================================
StatusCode DeVeloLitePhiType::neighbour(const LHCb::VeloLiteChannelID& start,
                                        const int& nOffset,
                                        LHCb::VeloLiteChannelID& channel) const {

  unsigned int strip = start.strip();
  unsigned int startZone = zoneOfStrip(strip);
  strip += nOffset;
  unsigned int endZone = zoneOfStrip(strip);
  // Check boundaries.
  if (numberOfStrips() <= strip || startZone != endZone) {
    return StatusCode::FAILURE;
  }
  channel = start;
  channel.setStrip(strip);
  return StatusCode::SUCCESS;
  
}

// ============================================================================
/// Check if a local point is inside the sensor
// ============================================================================
StatusCode DeVeloLitePhiType::isInActiveArea(const Gaudi::XYZPoint& point) const {

  // Check boundaries.
  const double x = point.x();
  const double y = point.y();
  if (x >  boundingBoxX() ||
      y >  boundingBoxY() ||
      y < -boundingBoxY()) {
    return StatusCode::FAILURE;
  }
  const double radius = point.Rho();
  if (radius < innerRadius() || 
      radius > outerRadius()) {
    return StatusCode::FAILURE;
  }

  // Check if the point is in the dead region between zones.
  unsigned int zone = zoneOfRadius(radius);
  if (radius > m_zones[zone].rMax) {
    return StatusCode::FAILURE;
  }
        
  // Work out if x/y is outside first/last strip in zone.
  unsigned int endStrip = m_zones[zone].firstStrip;
  if (y > 0) endStrip += m_zones[zone].nbStrips - 1;
  const double phiStrip = phiOfStrip(endStrip, 0., radius);
  const double phi = atan2(y, x);
  if (y < 0) {
    if (phiStrip > phi) {
      return StatusCode::FAILURE;
    }
  } else {
    if (phiStrip < phi) {
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
  
}

// ============================================================================
/// Residual of 3-d point to a VeloLiteChannelID
// ============================================================================
StatusCode DeVeloLitePhiType::residual(const Gaudi::XYZPoint& point,
                                       const LHCb::VeloLiteChannelID& channel,
                                       double &residual,
                                       double &chi2) const {

  return this->residual(point, channel, 0., residual, chi2);

}

// ============================================================================
/// Residual of 3-d point to a VeloLiteChannelID + interstrip fraction
// ============================================================================
StatusCode DeVeloLitePhiType::residual(const Gaudi::XYZPoint& point,
                                       const LHCb::VeloLiteChannelID& channel,
                                       const double interStripFraction,
                                       double &residual,
                                       double &chi2) const {

  /// Transform to local frame.
  Gaudi::XYZPoint localPoint = this->geometry()->toLocal(point);
  /// Check boundaries.
  StatusCode sc = isInActiveArea(localPoint);
  if (!sc.isSuccess()) return sc;

  /// Get start/end co-ordinates of channel's strip.
  const unsigned int strip = channel.strip();
  std::pair<Gaudi::XYZPoint,Gaudi::XYZPoint> stripLimits = localStripLimits(strip);
  Gaudi::XYZPoint stripBegin = stripLimits.first;
  Gaudi::XYZPoint stripEnd   = stripLimits.second;

  /// Add offset.
  Gaudi::XYZPoint nextStripBegin, nextStripEnd;
  if (interStripFraction > 0.) {
    stripLimits = localStripLimits(strip + 1);
    nextStripBegin = stripLimits.first;
    nextStripEnd   = stripLimits.second;
    stripBegin += (nextStripBegin - stripBegin) * interStripFraction;
    stripEnd   += (nextStripEnd   - stripEnd)   * interStripFraction;
  } else if (interStripFraction < 0.) { 
    // This should never happen for clusters.
    stripLimits = localStripLimits(strip - 1);
    nextStripBegin = stripLimits.first;
    nextStripEnd   = stripLimits.second;
    stripBegin += (stripBegin - nextStripBegin) * interStripFraction;
    stripEnd   += (stripEnd   - nextStripEnd)   * interStripFraction;
  }
  /// Calculate the equation of line for the strip.
  // TODO: why not use values stored in statics?
  const double gradient  = (stripEnd.y() - stripBegin.y()) / 
                           (stripEnd.x() - stripBegin.x());
  const double intercept = stripBegin.y() - gradient * stripBegin.x();
  const double x = localPoint.x();
  const double y = localPoint.y();

  double xNear = (gradient * y + x - gradient * intercept);
  xNear /= (1. + gradient * gradient);
  double yNear = gradient * xNear + intercept;

  residual = sqrt(gsl_pow_2(xNear - x) + gsl_pow_2(yNear - y));

  // Work out how to calculate the sign!
  Gaudi::XYZPoint localNear(xNear, yNear, 0.);
  Gaudi::XYZPoint globalNear = DeVeloLiteSensor::localToGlobal(localNear);
  if (point.phi() < globalNear.phi()) residual *= -1.;

  const double radius = localPoint.Rho();
  const double sigma = m_resolution.first * phiPitch(radius) - 
                       m_resolution.second;
  chi2 = gsl_pow_2(residual / sigma);

  if (m_verbose) {
    MsgStream msg(msgSvc(), "DeVeloLitePhiType");
    msg << MSG::VERBOSE << "Residual; sensor " << channel.sensor()
        << " strip " << strip
        << " x " << x << " y " << y << endmsg;
    msg << MSG::VERBOSE << " xNear " << xNear << " yNear " << yNear
        << " residual " << residual << " sigma = " << sigma
        << " chi2 = " << chi2 << endmsg;
  }
  return StatusCode::SUCCESS;

}

//=============================================================================
// Build map of strips to routing line and back
//=============================================================================
void DeVeloLitePhiType::buildRoutingLineMap() {

  MsgStream msg(msgSvc(), "DeVeloLitePhiType");
  if (m_debug) {
    msg << MSG::DEBUG << "Building routing line map for sensor " 
        << sensorNumber() << endmsg;
  }

  const unsigned int nStripsPerGroup = 128;
  const unsigned int nGroups = 20;
  // Mapping from strip groups to pad groups.
  std::vector<int> padGroup(nGroups);
  // Zone 0
  padGroup[0]  = 16;
  padGroup[1]  = 15;
  padGroup[2]  = 10;
  padGroup[3]  =  9;
  padGroup[4]  =  4;
  padGroup[5]  =  3;
  // Zone 1
  padGroup[6]  = 18;
  padGroup[7]  = 13;
  padGroup[8]  = 12;
  padGroup[9]  =  7;
  padGroup[10] =  6;
  padGroup[11] =  1;
  // Zone 2
  padGroup[12] = 19;
  padGroup[13] = 17;
  padGroup[14] = 14;
  padGroup[15] = 11;
  padGroup[16] =  8;
  padGroup[17] =  5;
  padGroup[18] =  2;
  padGroup[19] =  0;   

  // Loop over the strip groups.
  for (unsigned int isg = 0; isg < nGroups; ++isg) {
    // Get the corresponding pad group.
    const unsigned int ipg = padGroup[isg];
    // Loop over the strips in this group.
    for (unsigned int js = 0; js < nStripsPerGroup; ++js) {
      const unsigned int strip = isg * nStripsPerGroup + js;
      const unsigned int pad = (ipg + 1) * nStripsPerGroup - js - 1;
      m_mapStripToRoutingLine[strip] = pad;
      m_mapRoutingLineToStrip[pad]   = strip;
      if (m_verbose) {
        msg << MSG::VERBOSE 
            << "Pad (routing line) " << pad
            << " strip " << strip
            << endmsg;
      }
    }
  }
  
}

// ============================================================================
/// Return a trajectory (for track fit) from strip + offset
// ============================================================================
std::auto_ptr<LHCb::Trajectory> DeVeloLitePhiType::trajectory(const LHCb::VeloLiteChannelID& id,
                                                              const double offset) const {

  // Trajectory is a line.
  const unsigned int strip = id.strip();
  std::pair<Gaudi::XYZPoint,Gaudi::XYZPoint> localCoords = localStripLimits(strip);
  Gaudi::XYZPoint lEnd1 = localCoords.first;
  Gaudi::XYZPoint lEnd2 = localCoords.second;
  // Need to also grab next strip in local frame to get offset effect
  // Check direction of offset, do nothing if offset == 0.
  if (offset > 0.) {
    localCoords = localStripLimits(strip + 1);
    Gaudi::XYZPoint lNextEnd1 = localCoords.first;
    Gaudi::XYZPoint lNextEnd2 = localCoords.second;
    lEnd1 += (lNextEnd1 - lEnd1) * offset;
    lEnd2 += (lNextEnd2 - lEnd2) * offset;
  } else if (offset < 0.) {
    localCoords = localStripLimits(strip - 1);
    Gaudi::XYZPoint lNextEnd1 = localCoords.first;
    Gaudi::XYZPoint lNextEnd2 = localCoords.second;
    lEnd1 += (lEnd1 - lNextEnd1) * offset;
    lEnd2 += (lEnd2 - lNextEnd2) * offset;
  }
  // Convert to global coordinates.
  Gaudi::XYZPoint gEnd1 = localToGlobal(lEnd1);
  Gaudi::XYZPoint gEnd2 = localToGlobal(lEnd2);

  // Put into trajectory.
  LHCb::Trajectory* tTraj = new LHCb::LineTraj(gEnd1, gEnd2);
  std::auto_ptr<LHCb::Trajectory> autoTraj(tTraj);
  return autoTraj;

}

StatusCode DeVeloLitePhiType::updateStripCache() {

  m_zonesCache.resize(m_numberOfZones);
  m_stripsCache.resize(m_numberOfStrips);
  for (unsigned int zone = 0; zone < m_numberOfZones; ++zone) {
    unsigned int firstStrip = m_zones[zone].firstStrip;
    std::pair<Gaudi::XYZPoint,Gaudi::XYZPoint> limits = localStripLimits(firstStrip);
    double r0 = (limits.first.rho() + limits.second.rho()) / 2.0;
    double d0 = m_zones[zone].distToOrigin;
    if (!isDownstream()) d0 = -d0;
    m_zonesCache[zone].idealDistToOrigin = d0;
    m_zonesCache[zone].idealOffsetAtR0 = asin(d0 / r0);;

    Gaudi::XYZPoint begin = localToGlobal(limits.first);
    Gaudi::XYZPoint end   = localToGlobal(limits.second);
    r0 = (begin.rho() + end.rho()) / 2.0;
    Gaudi::XYZVector dx = end - begin;
    Gaudi::XYZPoint center = begin + 0.5 * dx;
    d0 = r0 * sin(center.phi() - dx.phi());
    m_zonesCache[zone].globalDistToOrigin = d0;
    m_zonesCache[zone].globalOffsetAtR0 = asin(d0 / r0);

    begin = localToVeloHalfBox(limits.first);
    end   = localToVeloHalfBox(limits.second);
    r0 = (begin.rho() + end.rho()) / 2.0;
    dx = end - begin;
    center = begin + 0.5 * dx;
    d0 = r0 * sin(center.phi() - dx.phi());
    m_zonesCache[zone].halfboxDistToOrigin = d0;
    m_zonesCache[zone].halfboxOffsetAtR0 = asin(d0/r0);

    for (unsigned int i = 0; i < m_zones[zone].nbStrips; ++i) {
      const unsigned int strip = firstStrip + i;
      const double phi0 = phiOfStrip(strip, 0., r0);
      const double x = r0 * cos(phi0);
      const double y = r0 * sin(phi0);
      Gaudi::XYZPoint lp(x, y, 0.);
      m_stripsCache[strip].idealPhi = localPhiToGlobal(lp.phi());
      Gaudi::XYZPoint gp = localToGlobal(lp);
      m_stripsCache[strip].globalPhi = gp.phi();
      Gaudi::XYZPoint hbp = localToVeloHalfBox(lp);
      m_stripsCache[strip].halfboxPhi = hbp.phi();
    }
  }
  return StatusCode::SUCCESS;
  
}

StatusCode DeVeloLitePhiType::updateZoneCache() {

  for (unsigned int zone = 0; zone < m_numberOfZones; ++zone) {
    const unsigned int minStrip = m_zones[zone].firstStrip;
    const unsigned int maxStrip = minStrip + m_zones[zone].nbStrips - 1;
    const unsigned int midStrip = (minStrip + maxStrip) / 2;

    // Determine the ranges of the zones in global frame.
    std::pair<Gaudi::XYZPoint, Gaudi::XYZPoint> globalLimitsMin = globalStripLimits(minStrip);
    std::pair<Gaudi::XYZPoint, Gaudi::XYZPoint> globalLimitsMax = globalStripLimits(maxStrip);
    std::pair<Gaudi::XYZPoint, Gaudi::XYZPoint> globalLimitsMid = globalStripLimits(midStrip);
    
    std::vector<double> rLimits;
    rLimits.clear();
    rLimits.push_back(globalLimitsMin.first.rho()); 
    rLimits.push_back(globalLimitsMin.second.rho());
    rLimits.push_back(globalLimitsMax.first.rho()); 
    rLimits.push_back(globalLimitsMax.second.rho());
    rLimits.push_back(globalLimitsMid.first.rho()); 
    rLimits.push_back(globalLimitsMid.second.rho());
    m_zonesCache[zone].globalRLimits.first  = *std::min_element(rLimits.begin(), rLimits.end());
    m_zonesCache[zone].globalRLimits.second = *std::max_element(rLimits.begin(), rLimits.end());

    std::vector<double> phiLimits;
    phiLimits.clear();
    phiLimits.push_back(globalLimitsMin.first.phi()); 
    phiLimits.push_back(globalLimitsMin.second.phi());
    phiLimits.push_back(globalLimitsMax.first.phi()); 
    phiLimits.push_back(globalLimitsMax.second.phi());
    // Map to [0,2pi] for right hand side sensors.
    if (isRight()) {
      for (unsigned int i = 0; i < phiLimits.size(); ++i) {
        if (phiLimits[i] < 0) phiLimits[i] += 2 * Gaudi::Units::pi;
      }
    }
    m_zonesCache[zone].globalPhiLimits.first  = *std::min_element(phiLimits.begin(), phiLimits.end());
    m_zonesCache[zone].globalPhiLimits.second = *std::max_element(phiLimits.begin(), phiLimits.end());
    // Map back to [-pi,pi]
    if (isRight()) {
      if (m_zonesCache[zone].globalPhiLimits.first  > Gaudi::Units::pi) {
        m_zonesCache[zone].globalPhiLimits.first  -= 2 * Gaudi::Units::pi;
      }
      if (m_zonesCache[zone].globalPhiLimits.second > Gaudi::Units::pi) {
        m_zonesCache[zone].globalPhiLimits.second -= 2 * Gaudi::Units::pi;
      }
    }

    // Determine the ranges of the zones in the Velo half box frame.
    std::pair<Gaudi::XYZPoint, Gaudi::XYZPoint> halfBoxLimitsMin
      (globalToVeloHalfBox(globalLimitsMin.first),
       globalToVeloHalfBox(globalLimitsMin.second));
    std::pair<Gaudi::XYZPoint, Gaudi::XYZPoint> halfBoxLimitsMax
      (globalToVeloHalfBox(globalLimitsMax.first),
       globalToVeloHalfBox(globalLimitsMax.second));
    std::pair<Gaudi::XYZPoint, Gaudi::XYZPoint> halfBoxLimitsMid
      (globalToVeloHalfBox(globalLimitsMid.first),
       globalToVeloHalfBox(globalLimitsMid.second));
    rLimits.clear();
    rLimits.push_back(halfBoxLimitsMin.first.rho()); 
    rLimits.push_back(halfBoxLimitsMin.second.rho());
    rLimits.push_back(halfBoxLimitsMax.first.rho()); 
    rLimits.push_back(halfBoxLimitsMax.second.rho());
    rLimits.push_back(halfBoxLimitsMid.first.rho()); 
    rLimits.push_back(halfBoxLimitsMid.second.rho());
    m_zonesCache[zone].halfboxRLimits.first  = *std::min_element(rLimits.begin(), rLimits.end());
    m_zonesCache[zone].halfboxRLimits.second = *std::max_element(rLimits.begin(), rLimits.end());

    phiLimits.clear();
    phiLimits.push_back(halfBoxLimitsMin.first.phi()); 
    phiLimits.push_back(halfBoxLimitsMin.second.phi());
    phiLimits.push_back(halfBoxLimitsMax.first.phi()); 
    phiLimits.push_back(halfBoxLimitsMax.second.phi());
    // Map to [0,2pi] for right hand side sensors
    if (isRight()) {
      for (unsigned int i = 0; i < phiLimits.size(); ++i) {
        if (phiLimits[i] < 0) phiLimits[i] += 2 * Gaudi::Units::pi;
      }
    }
    m_zonesCache[zone].halfboxPhiLimits.first  = *std::min_element(phiLimits.begin(), phiLimits.end());
    m_zonesCache[zone].halfboxPhiLimits.second = *std::max_element(phiLimits.begin(), phiLimits.end());
    // Map back to [-pi,pi]
    if (isRight()) {
      if (m_zonesCache[zone].halfboxPhiLimits.first  > Gaudi::Units::pi) {
        m_zonesCache[zone].halfboxPhiLimits.first  -= 2 * Gaudi::Units::pi;
      }
      if (m_zonesCache[zone].halfboxPhiLimits.second > Gaudi::Units::pi) {
        m_zonesCache[zone].halfboxPhiLimits.second -= 2 * Gaudi::Units::pi;
      }
    }

    // Extend the phi ranges by half a strip pitch
    const double pitch = fabs(phiPitch(minStrip));
    m_zonesCache[zone].globalPhiLimits.first   -= pitch / 2.;
    m_zonesCache[zone].globalPhiLimits.second  += pitch / 2.;
    m_zonesCache[zone].halfboxPhiLimits.first  -= pitch / 2.;
    m_zonesCache[zone].halfboxPhiLimits.second += pitch / 2.;
  }
  return StatusCode::SUCCESS;
  
}

StatusCode DeVeloLitePhiType::updateGeometryCache() {

  StatusCode sc = updateStripCache();
  if (!sc.isSuccess()) {
    MsgStream msg(msgSvc(), "DeVeloLitePhiType");
    msg << MSG::ERROR << "Failed to update strip cache." << endmsg;
    return sc;
  }
  sc = updateZoneCache();
  if (!sc.isSuccess()) {
    MsgStream msg(msgSvc(), "DeVeloLitePhiType");
    msg << MSG::ERROR << "Failed to update zone limit cache." << endmsg;
    return sc;
  }
  return StatusCode::SUCCESS;

}
