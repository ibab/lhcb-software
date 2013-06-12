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
// Kernel/LHCbKernel
#include "Kernel/LineTraj.h"
#include "Kernel/VLChannelID.h"
// Local 
#include "VLDet/DeVLPhiSensor.h"

namespace VLDet {
  /** Access to local static data
   *
   *  @see DeVeloPhiSensor
   */
  static std::vector<phiStrip>& deVLPhiSensorStaticPhiStrips() {
    static std::vector<phiStrip> s_strips;
    return s_strips;
  }
  static std::vector<phiZone>& deVLPhiSensorStaticPhiZones() {
    static std::vector<phiZone> s_zones;
    return s_zones;
  }

}

/** @file DeVLPhiSensor.cpp
 *
 *  Implementation of class : DeVLPhiSensor
 *
 */

// ============================================================================
/// Constructor
// ============================================================================
DeVLPhiSensor::DeVLPhiSensor(const std::string& name) : 
    DeVLSensor(name),
    m_zones(VLDet::deVLPhiSensorStaticPhiZones()),
    m_strips(VLDet::deVLPhiSensorStaticPhiStrips()),
    m_associatedRSensor(0),
    m_otherSidePhiSensor(0),
    m_otherSideRSensor(0),
    m_nextPhiSensor(0),
    m_previousPhiSensor(0) {
  
}

// ============================================================================
/// Object identification
// ============================================================================
const CLID& DeVLPhiSensor::clID() const {

  return DeVLPhiSensor::classID();

}

// ============================================================================
/// Initialisation
// ============================================================================
StatusCode DeVLPhiSensor::initialize() {

  // Set the output level.
  PropertyMgr* pmgr = new PropertyMgr();
  int outputLevel = 0;
  pmgr->declareProperty("OutputLevel", outputLevel);
  IJobOptionsSvc* jobSvc;
  ISvcLocator* svcLoc = Gaudi::svcLocator();
  StatusCode sc = svcLoc->service("JobOptionsSvc", jobSvc);
  if (sc.isSuccess()) sc = jobSvc->setMyProperties("DeVLPhiSensor", pmgr);
  if (outputLevel > 0) {
    msgSvc()->setOutputLevel("DeVLPhiSensor", outputLevel);
  }
  delete pmgr;
  if (!sc) return sc;
  m_debug   = (msgSvc()->outputLevel("DeVLPhiSensor") == MSG::DEBUG);
  m_verbose = (msgSvc()->outputLevel("DeVLPhiSensor") == MSG::VERBOSE);
  if (m_verbose) m_debug = true;
  MsgStream msg(msgSvc(), "DeVLPhiSensor");

  // Initialise the base class.
  sc = DeVLSensor::initialize();
  if (!sc.isSuccess()) {
    msg << MSG::ERROR << "Failed to initialise DeVLSensor." << endmsg;
    return sc;
  }
  // Initialise the sensor from the XML.
  sc = initSensor();
  if (!sc.isSuccess()) {
    msg << MSG::ERROR << "Failed to initialise DeVLPhiSensor." << endmsg;
    return sc;
  }
  // Register geometry conditions, update strip cache.
  updMgrSvc()->registerCondition(this, this->m_geometry,
                                 &DeVLPhiSensor::updateGeometryCache);
  // First update
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
StatusCode DeVLPhiSensor::initSensor() {

  MsgStream msg(msgSvc(), "DeVLPhiSensor");
  // Number of zones
  m_numberOfZones = param<int>("PhiNbZones");
  // Number of strips in each zone
  std::vector<int> nbStrips = paramAsIntVect("PhiNbStrips");
  // Angular coverage in each zone
  std::vector<double> coverage = paramAsDoubleVect("PhiCoverage");
  // Min. and max. radius of each zone
  std::vector<double> rMinZone = paramAsDoubleVect("PhiMinRadius");
  std::vector<double> rMaxZone = paramAsDoubleVect("PhiMaxRadius");
  // Stereo angles of each zone
  std::vector<double> stereoAngles = paramAsDoubleVect("PhiStereoAngles");
  // Phi offset
  m_phiOrigin = param<double>("PhiOrigin");
  m_phiOrigin -= Gaudi::Units::halfpi;
  // Resolution of the sensor
  m_resolution.first = param<double>("PhiResGrad");
  m_resolution.second = param<double>("PhiResConst");

  // Check the size of the vectors retrieved from XML.
  if (nbStrips.size()     != m_numberOfZones ||
      coverage.size()     != m_numberOfZones ||
      rMinZone.size()     != m_numberOfZones ||
      rMaxZone.size()     != m_numberOfZones ||
      stereoAngles.size() != m_numberOfZones) {
    msg << MSG::ERROR 
        << "Mismatch between number of zones and array sizes." << endmsg;
    return StatusCode::FAILURE;
  }

  // Count the total number of strips.
  m_numberOfStrips = 0;
  for (unsigned int zone = 0; zone < m_numberOfZones; ++zone) {
    m_numberOfStrips += nbStrips[zone];
  }
    
  // Setup the zone parameters.
  m_zones.resize(m_numberOfZones);
  int stripIndex = 0;
  for (unsigned int zone = 0; zone < m_numberOfZones; ++zone) {
    m_zones[zone].firstStrip = stripIndex;
    m_zones[zone].nbStrips = nbStrips[zone];
    m_zones[zone].coverage = coverage[zone];
    // Calculate the phi pitch.
    m_zones[zone].pitch = m_zones[zone].coverage / m_zones[zone].nbStrips;
    // Set the inner and outer radius of the zone.
    m_zones[zone].rMin = rMinZone[zone];
    m_zones[zone].rMax = rMaxZone[zone];
    // Set the stereo angle.
    m_zones[zone].tilt = stereoAngles[zone];
    // Calculate dist. to origin 
    // (defined by angle between extrapolated strip and phi)
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
  
  // Setup the strip parameters.
  m_stripLimits.clear();
  m_strips.resize(m_numberOfStrips);
  stripIndex = 0;
  std::ofstream stripfile;
  if (m_debug) stripfile.open("phistrips.txt", std::ios::out);
  for (unsigned int zone = 0; zone < m_numberOfZones; ++zone) {
    for (unsigned int strip = 0; strip < m_zones[zone].nbStrips; ++strip) {
      const double r1 = m_zones[zone].rMin;
      const double r2 = m_zones[zone].rMax;
      // Determine the beginning and end points of the strip.
      double x1 = r1 * cos(phiOfStrip(stripIndex, 0., r1));
      double y1 = r1 * sin(phiOfStrip(stripIndex, 0., r1));
      double x2 = r2 * cos(phiOfStrip(stripIndex, 0., r2));
      double y2 = r2 * sin(phiOfStrip(stripIndex, 0., r2)); 
      const double gradient = (y2 - y1) /  (x2 - x1);
      const double intercept = y2 - (gradient * x2);
      // Check if the end points are outside the bounding box.
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
      // Calculate the length of the strip.
      const double dx = x2 - x1;
      const double dy = y2 - y1;
      const double length = sqrt(dx * dx + dy * dy);
      m_strips[stripIndex].zone = zone;
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
StatusCode DeVLPhiSensor::pointToChannel(const Gaudi::XYZPoint& point,
                                         LHCb::VLChannelID& channel,
                                         double& fraction,
                                         double& pitch) const {

  // Transform to the local frame.                                         
  Gaudi::XYZPoint localPoint = globalToLocal(point);
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
  pitch = phiPitchOfRadius(radius);

  const unsigned int sensor = sensorNumber();
  // Set the VLChannelID.
  channel.setSensor(sensor);
  channel.setStrip(closestStrip);
  channel.setType(LHCb::VLChannelID::PhiType);

  if (m_verbose) {
    MsgStream msg(msgSvc(), "DeVLPhiSensor");
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
StatusCode DeVLPhiSensor::neighbour(const LHCb::VLChannelID& start,
                                    const int& nOffset,
                                    LHCb::VLChannelID& channel) const {

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
StatusCode DeVLPhiSensor::isInActiveArea(const Gaudi::XYZPoint& point) const {

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
  const double phi = atan2(y, x);
  const double tol = 0.05;
  if (y > 0) {
    unsigned int strip = m_zones[zone].firstStrip + m_zones[zone].nbStrips -1;
    const double phiStrip = phiOfStrip(strip, tol, radius);
    if (phiStrip < phi) return StatusCode::FAILURE;
  } else {
    unsigned int strip = m_zones[zone].firstStrip;
    const double phiStrip = phiOfStrip(strip, -tol, radius);
    if (phiStrip > phi) return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
  
}

// ============================================================================
/// Residual of 3-d point to a VLChannelID
// ============================================================================
StatusCode DeVLPhiSensor::residual(const Gaudi::XYZPoint& point,
                                   const LHCb::VLChannelID& channel,
                                   double &residual,
                                   double &chi2) const {

  return this->residual(point, channel, 0., residual, chi2);

}

// ============================================================================
/// Residual of 3-d point to a VLChannelID + interstrip fraction
// ============================================================================
StatusCode DeVLPhiSensor::residual(const Gaudi::XYZPoint& point,
                                   const LHCb::VLChannelID& channel,
                                   const double interStripFraction,
                                   double &residual,
                                   double &chi2) const {

  // Transform to local frame.
  Gaudi::XYZPoint localPoint = this->geometry()->toLocal(point);
  // Check boundaries.
  StatusCode sc = isInActiveArea(localPoint);
  if (!sc.isSuccess()) return sc;

  // Get start/end co-ordinates of channel's strip.
  const unsigned int strip = channel.strip();
  std::pair<Gaudi::XYZPoint,Gaudi::XYZPoint> stripLimits = localStripLimits(strip);
  Gaudi::XYZPoint stripBegin = stripLimits.first;
  Gaudi::XYZPoint stripEnd   = stripLimits.second;

  // Add offset.
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

  residual = sqrt(pow(xNear - x, 2) + pow(yNear - y, 2));

  // Work out how to calculate the sign.
  Gaudi::XYZPoint localNear(xNear, yNear, 0.);
  Gaudi::XYZPoint globalNear = DeVLSensor::localToGlobal(localNear);
  if (point.phi() < globalNear.phi()) residual *= -1.;

  const double radius = localPoint.Rho();
  const double sigma = m_resolution.first * phiPitchOfRadius(radius) - 
                       m_resolution.second;
  chi2 = pow(residual / sigma, 2);

  if (m_verbose) {
    MsgStream msg(msgSvc(), "DeVLPhiSensor");
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
/// Return a trajectory (for track fit) from strip + offset
//=============================================================================
std::auto_ptr<LHCb::Trajectory> DeVLPhiSensor::trajectory(const LHCb::VLChannelID& id,
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

  // Make a trajectory.
  LHCb::Trajectory* tTraj = new LHCb::LineTraj(gEnd1, gEnd2);
  std::auto_ptr<LHCb::Trajectory> autoTraj(tTraj);
  return autoTraj;

}

StatusCode DeVLPhiSensor::updateStripCache() {

  m_stripsCache.resize(m_numberOfStrips);
  for (unsigned int zone = 0; zone < m_numberOfZones; ++zone) {
    unsigned int firstStrip = m_zones[zone].firstStrip;
    for (unsigned int i = 0; i < m_zones[zone].nbStrips; ++i) {
      const unsigned int strip = firstStrip + i;
      Gaudi::XYZPoint begin = localToGlobal(m_stripLimits[strip].first);
      Gaudi::XYZPoint end = localToGlobal(m_stripLimits[strip].second);
      const double dx = begin.x() - end.x();
      const double dy = begin.y() - end.y();
      const double d = sqrt(dx * dx + dy * dy);
      m_stripsCache[strip].c = (begin.y() * end.x() - end.y() * begin.x()) / d;
      m_stripsCache[strip].a = -dy / d;
      m_stripsCache[strip].b =  dx / d;
      m_stripsCache[strip].xs = 0.5 * (begin.x() + end.x());
      m_stripsCache[strip].ys = 0.5 * (begin.y() + end.y());
    }
  }
  return StatusCode::SUCCESS;
  
}

StatusCode DeVLPhiSensor::updateGeometryCache() {

  StatusCode sc = updateStripCache();
  if (!sc.isSuccess()) {
    MsgStream msg(msgSvc(), "DeVLPhiSensor");
    msg << MSG::ERROR << "Failed to update strip cache." << endmsg;
    return sc;
  }
  return StatusCode::SUCCESS;

}
