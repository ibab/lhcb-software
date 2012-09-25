#include <fstream>
// Gaudi
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/IUpdateManagerSvc.h"
#include "LHCbMath/LHCbMath.h"
// Kernel/LHCbKernel
#include "Kernel/CircleTraj.h"
#include "Kernel/VLChannelID.h"
// Local
#include "VLDet/DeVLRSensor.h"

namespace VLDet {

  /** These functions simply provide access to local static
   *  data which are used to initialize references in each instance
   *  of DeVeloRType.
   *  The purpose of these functions is to work around
   *  a Microsoft(tm) specific extension in VC++ that makes
   *  awkward to have static data members accessed by inline
   *  functions.
   *
   *  @see DeVeloRType
   */
  static std::vector<rStrip>& deVLRSensorStaticRStrips() {
    static std::vector<rStrip> s_strips;
    return s_strips;
  }
  static std::vector<rZone>& deVLRSensorStaticRZones() {
    static std::vector<rZone> s_zones;
    return s_zones;
  }
  
}

/** @file DeVLRSensor.cpp
 *
 *  Implementation of class : DeVLRSensor
 *
 */

//=============================================================================
/// Standard constructor
//=============================================================================
DeVLRSensor::DeVLRSensor(const std::string& name) : 
    DeVLSensor(name),
    m_zones(VLDet::deVLRSensorStaticRZones()),
    m_strips(VLDet::deVLRSensorStaticRStrips()),
    m_associatedPhiSensor(0),
    m_otherSideRSensor(0),
    m_otherSidePhiSensor(0),
    m_nextRSensor(0),
    m_previousRSensor(0) {
 
}

//=============================================================================
/// Object identification
//=============================================================================
const CLID& DeVLRSensor::clID() const {

  return DeVLRSensor::classID(); 

}

//=============================================================================
/// Initialisation
//=============================================================================
StatusCode DeVLRSensor::initialize() {

  // Set the output level.
  PropertyMgr* pmgr = new PropertyMgr();
  int outputLevel = 0;
  pmgr->declareProperty("OutputLevel", outputLevel);
  IJobOptionsSvc* jobSvc;
  ISvcLocator* svcLoc = Gaudi::svcLocator();
  StatusCode sc = svcLoc->service("JobOptionsSvc", jobSvc);
  if (sc.isSuccess()) sc = jobSvc->setMyProperties("DeVLRSensor", pmgr);
  if (outputLevel > 0) {
    msgSvc()->setOutputLevel("DeVLRSensor", outputLevel);
  }
  delete pmgr;
  if (!sc) return sc;
  m_debug   = (msgSvc()->outputLevel("DeVLRSensor") == MSG::DEBUG);
  m_verbose = (msgSvc()->outputLevel("DeVLRSensor") == MSG::VERBOSE);
  if (m_verbose) m_debug = true;
  MsgStream msg(msgSvc(), "DeVLRSensor");

  // Initialise the base class.
  sc = DeVLSensor::initialize();
  if (!sc.isSuccess()) {
    msg << MSG::ERROR << "Failed to initialise DeVLSensor." << endmsg;
    return sc;
  }
  // Initialise the sensor from the XML.
  sc = initSensor();
  if (!sc.isSuccess()) {
    msg << MSG::ERROR << "Failed to initialise DeVLRSensor." << endmsg;
    return sc;
  }
  // Build up map of strips to routing lines.
  buildRoutingLineMap();
  // Register geometry conditions, update strip cache.
  updMgrSvc()->registerCondition(this, this->m_geometry,
                                 &DeVLRSensor::updateGeometryCache);
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
StatusCode DeVLRSensor::initSensor() {
  
  MsgStream msg(msgSvc(), "DeVLRSensor");
  // Number of zones
  m_numberOfZones = param<int>("RNbZones");
  // Number of strips in each zone
  std::vector<int> nbStrips = paramAsIntVect("RNbStrips");
  // Max. radius of each zone
  std::vector<double> rMaxZone = paramAsDoubleVect("RMaxRadius");
  // Inner pitch
  m_innerPitch = param<double>("RInnerPitch");
  // Radius up to which the pitch is constant
  m_rLogPitch = param<double>("RConstPitchRadius");
  // Pitch slope (logarithmic)
  m_pitchSlope = param<double>("RPitchSlope");
  // Overhang
  m_overhang = param<double>("ROverhang");
  // Dead region from bias line
  m_phiGap = param<double>("RPhiGap") / 2;
  // Resolution of the sensor
  m_resolution.first = param<double>("RResGrad");
  m_resolution.second = param<double>("RResConst");

  // Check the size of the vectors retrieved from XML.
  if (nbStrips.size() != m_numberOfZones ||
      rMaxZone.size() != m_numberOfZones) {
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
  unsigned int stripIndex = 0;
  for (unsigned int zone = 0; zone < m_numberOfZones; ++zone) {
    m_zones[zone].firstStrip = stripIndex;
    m_zones[zone].firstStripLogPitch = stripIndex;
    m_zones[zone].nbStrips = nbStrips[zone];
    m_zones[zone].rMin = innerRadius();
    m_zones[zone].rMax = rMaxZone[zone];
    // Determine the angular limits.
    const double phiStart    = -90. * Gaudi::Units::degree;
    const double phiCoverage = 180. * Gaudi::Units::degree / m_numberOfZones;
    m_zones[zone].phiMin = phiStart + zone * phiCoverage;
    m_zones[zone].phiMax = m_zones[zone].phiMin + phiCoverage;
    if (0 == zone) {
      const double deltaPhi = asin(fabs(m_overhang / m_zones[zone].rMin));
      m_zones[zone].phiMin -= deltaPhi;
    } else if (m_numberOfZones - 1 == zone) {
      const double deltaPhi = asin(fabs(m_overhang / m_zones[zone].rMin));
      m_zones[zone].phiMax += deltaPhi;
    }
    stripIndex += m_zones[zone].nbStrips;
  }

  unsigned int firstStripLowerEar = m_zones[2].firstStrip;
  unsigned int firstStripUpperEar = m_zones[2].firstStrip + m_zones[2].nbStrips;

  // Setup the strip parameters.
  m_stripLimits.clear();
  m_strips.resize(m_numberOfStrips);
  stripIndex = 0;
  std::ofstream stripfile;
  if (m_debug) stripfile.open("rstrips.txt", std::ios::out);
  for (unsigned int zone = 0; zone < m_numberOfZones; ++zone) {
    double radius = m_zones[zone].rMin;
    for (unsigned int strip = 0; strip < m_zones[zone].nbStrips; ++strip) {
      double pitch = m_innerPitch;
      if (radius > m_rLogPitch) {
        pitch *= pow(radius / m_rLogPitch, m_pitchSlope);
        if (m_zones[zone].firstStripLogPitch <= m_zones[zone].firstStrip) {
          m_zones[zone].firstStripLogPitch = stripIndex;
        }
      }
      double phiMin = phiMinZone(zone, radius);
      double phiMax = phiMaxZone(zone, radius);
      double x1 = radius * cos(phiMin);
      double y1 = radius * sin(phiMin);
      double x2 = radius * cos(phiMax);
      double y2 = radius * sin(phiMax);
      // Check if the end points are outside the bounding box.
      if (x1 < m_overhang) {
        x1 = m_overhang;
        phiMin = acos(x1 / radius);
        if (y1 < 0.) phiMin = -phiMin;
        y1 = radius * sin(phiMin);
      }
      if (y1 < -boundingBoxY()) {
        y1 = -boundingBoxY();
        phiMin = asin(y1 / radius);
        x1 = radius * cos(phiMin);
      }
      if (x2 > boundingBoxX()) {
        x2 = boundingBoxX();
        phiMax = acos(x2 / radius);
        if (y2 < 0.) phiMax = -phiMax;
        y2 = radius * sin(phiMax);
      }
      if (x1 > boundingBoxX()) {
        x1 = boundingBoxX();
        phiMin = acos(x1 / radius);
        if (y1 < 0.) phiMin = -phiMin;
        y1 = radius * sin(phiMin);
      }
      if (y2 > boundingBoxY()) {
        y2 = boundingBoxY();
        phiMax = asin(y2 / radius);
        x2 = radius * cos(phiMax);
      }
      if (x2 < m_overhang) {
        x2 = m_overhang;
        phiMax = acos(x2 / radius);
        if (y2 < 0.) phiMax = -phiMax;
        y2 = radius * sin(phiMax);
      }
      // Check if the strip is split in two due to the bounding box.
      if (zone == 2 && radius > boundingBoxX()) {
        if (stripIndex < firstStripUpperEar) {
          x2 = boundingBoxX();
          phiMax = -acos(x2 / radius);
          y2 = radius * sin(phiMax);
          if (firstStripLowerEar <= m_zones[zone].firstStrip) {
            firstStripLowerEar = stripIndex;
          }
        } else {
          x1 = boundingBoxX();
          phiMin = acos(x1 / radius);
          y1 = radius * sin(phiMin);
        } 
      }
      m_strips[stripIndex].zone = zone;
      m_strips[stripIndex].r = radius;
      m_strips[stripIndex].pitch = pitch;
      m_strips[stripIndex].phiMin = phiMin;
      m_strips[stripIndex].phiMax = phiMax;
      if (m_debug) {
        stripfile << stripIndex << "  " << radius * 1000. << "  " 
                  << phiMin * 180. / Gaudi::Units::pi + 180. << "  " 
                  << phiMax * 180. / Gaudi::Units::pi + 180. << "\n";
      }
      Gaudi::XYZPoint begin(x1, y1, 0.);
      Gaudi::XYZPoint end(x2, y2, 0.);
      m_stripLimits.push_back(std::pair<Gaudi::XYZPoint, Gaudi::XYZPoint>(begin, end));
      ++stripIndex;
      radius += pitch;
      if (radius > m_zones[zone].rMax + pitch / 2. && 2 == zone) {
        radius = m_strips[firstStripLowerEar].r;
        if (firstStripUpperEar >= m_zones[zone].firstStrip + m_zones[zone].nbStrips) {
          firstStripUpperEar = stripIndex;
          m_numberOfStripsPerEar = firstStripUpperEar - firstStripLowerEar;
        }
      }
    }
  }
  if (m_debug) stripfile.close();
  return StatusCode::SUCCESS;

}

//=============================================================================
/// Calculate the nearest channel to a 3-d point.
//=============================================================================
StatusCode DeVLRSensor::pointToChannel(const Gaudi::XYZPoint& point,
                                       LHCb::VLChannelID& channel,
                                       double& fraction,
                                       double& pitch) const {
                                       
  // Transform to the local frame.
  Gaudi::XYZPoint localPoint = globalToLocal(point);
  // Check boundaries.
  StatusCode sc = isInActiveArea(localPoint);
  if (!sc.isSuccess()) return sc;
  const double radius = localPoint.Rho();
  const double phi    = localPoint.phi();
  const unsigned int zone = zoneOfPhiAndR(phi, radius);
  // Work out the closest channel.
  unsigned int closestStrip = 0;
  if (radius > m_rLogPitch) {
    int iLo = m_zones[zone].firstStrip;
    int iUp = m_zones[zone].firstStrip + m_zones[zone].nbStrips - 1;
    // Special treatment for "ears" in central sector.
    if (radius > boundingBoxX() && localPoint.y() > 0. && zone == 2) {
      iUp -= m_numberOfStripsPerEar;
    }
    int iM;
    while (iUp - iLo > 1) {
      iM = (iUp + iLo) >> 1;
      if (radius >= m_strips[iM].r) {
        iLo = iM;
      } else {
        iUp = iM;
      }
    }
    double strip = iLo + (radius - m_strips[iLo].r) / m_strips[iLo].pitch;
    if (radius > boundingBoxX() && localPoint.y() > 0. && zone == 2) {
      strip += m_numberOfStripsPerEar;
    }
    closestStrip = LHCb::Math::round(strip);
    fraction = strip - closestStrip;
  } else {
    const double strip = (radius - innerRadius()) / m_innerPitch;
    closestStrip = LHCb::Math::round(strip);
    fraction = strip - closestStrip;
    closestStrip += m_zones[zone].firstStrip;
  }
  const unsigned int sensor = sensorNumber();
  // Set the VLChannelID.
  channel.setSensor(sensor);
  channel.setStrip(closestStrip);
  channel.setType(LHCb::VLChannelID::RType);
  // Calculate the pitch.
  pitch = rPitch(channel.strip());
  return StatusCode::SUCCESS;
  
}

//=============================================================================
/// Get the nth nearest neighbour within a sector for a given channel
//=============================================================================
StatusCode DeVLRSensor::neighbour(const LHCb::VLChannelID& start,
                                  const int& nOffset,
                                  LHCb::VLChannelID& channel) const {

  unsigned int strip = start.strip();
  unsigned int startZone = zoneOfStrip(strip);
  strip += nOffset;
  unsigned int endZone = zoneOfStrip(strip);
  // Check boundaries.
  if (numberOfStrips() <= strip) return StatusCode::FAILURE;
  if (startZone != endZone) return StatusCode::FAILURE;
  channel = start;
  channel.setStrip(strip);
  return StatusCode::SUCCESS;

}

//=============================================================================
/// Check if a local point is inside the sensor
//=============================================================================
StatusCode DeVLRSensor::isInActiveArea(const Gaudi::XYZPoint& point) const {

  // Check boundaries.
  const double x = point.x();
  const double y = point.y();
  if (x >  boundingBoxX() ||
      y >  boundingBoxY() ||
      y < -boundingBoxY()) {
    return StatusCode::FAILURE;
  }
  if (x < m_overhang) return StatusCode::FAILURE;
  const double radius = point.Rho();
  if (radius < innerRadius() || 
      radius > outerRadius()) {
    return StatusCode::FAILURE;
  }
  // Dead region from bias lines
  // TODO
  if (m_phiGap > y && -m_phiGap < y) {
    // return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
  
}

//=============================================================================
/// Return the minimum phi in a zone at given radius
//=============================================================================
double DeVLRSensor::phiMinZone(unsigned int zone, double radius) const {

  double phiMin = phiMinZone(zone);
  if (1 == zone) {
    if (radius > m_zones[0].rMax) {
      phiMin = m_zones[0].phiMin;
    }
  }
  return phiMin;
  
}
//=============================================================================
/// Return the maximum phi in a zone at given radius
//=============================================================================
double DeVLRSensor::phiMaxZone(unsigned int zone, double radius) const {

  double phiMax = phiMaxZone(zone);
  if (m_numberOfZones - 2 == zone) {
    if (radius > m_zones[m_numberOfZones - 1].rMax) {
      phiMax = m_zones[m_numberOfZones - 1].phiMax;
    }
  }
  return phiMax;

}

//=============================================================================
/// Return the length of a strip
//=============================================================================
double DeVLRSensor::stripLength(const unsigned int strip) const {

  const double phiMin = m_strips[strip].phiMin;
  const double phiMax = m_strips[strip].phiMax;
  const double radius = m_strips[strip].r;
  return (phiMax - phiMin) * radius;

}

//=============================================================================
/// Residual of 3-d point to a VLChannelID
//=============================================================================
StatusCode DeVLRSensor::residual(const Gaudi::XYZPoint& point,
                                 const LHCb::VLChannelID& channel,
                                 double &residual,
                                 double &chi2) const {

  return this->residual(point, channel, 0., residual, chi2);

}
//=============================================================================
///Residual of 3-d point to a VLChannelID + interstrip fraction
//=============================================================================
StatusCode DeVLRSensor::residual(const Gaudi::XYZPoint& point,
                                 const LHCb::VLChannelID& channel,
                                 const double interStripFraction,
                                 double &residual,
                                 double &chi2) const {

  Gaudi::XYZPoint localPoint = globalToLocal(point);
  // Check boundaries.
  StatusCode sc = isInActiveArea(localPoint);
  if (!sc.isSuccess()) return sc;

  const unsigned int strip = channel.strip();
  const double offset = interStripFraction * rPitch(strip);

  const double rPoint = localPoint.Rho();
  const double rStrip = rOfStrip(strip);
  residual = rStrip + offset - rPoint;
  const double sigma = m_resolution.first * rPitch(strip) - m_resolution.second;
  chi2 = pow(residual / sigma, 2);
  return StatusCode::SUCCESS;

}

//=============================================================================
/// Build up routing line map
//=============================================================================
void DeVLRSensor::buildRoutingLineMap() {

  // TODO: to be checked and refined
  const int nChannelsPerChip = 128;
  int firstPad = 0;
  for (unsigned int zone = 0; zone < m_numberOfZones; ++zone) {
    switch (zone) {
      case 0:
        firstPad = 20 * nChannelsPerChip - 1;
        break;
      case 1:
        firstPad = 16 * nChannelsPerChip - 1;
        break;
      case 2:
        firstPad = 12 * nChannelsPerChip - 11;
        break;
      case 3:
        firstPad = 4 * nChannelsPerChip;
        break;
      case 4:
        firstPad = 0;
        break;
      default:
        break;
    }
    for (unsigned int strip = 0; strip < m_zones[zone].nbStrips; ++strip) {
      int pad = firstPad;
      if (zone < 2) {
        pad -= strip;
      } else if (zone > 2) {
        pad += strip;
      } else {
        if (strip < 491) {
          pad -= strip;  
        }
      }
      m_mapStripToRoutingLine[m_zones[zone].firstStrip + strip] = pad;
      m_mapRoutingLineToStrip[pad] = m_zones[zone].firstStrip + strip;
      
      if (m_verbose) {
        MsgStream msg(msgSvc(), "DeVLRSensor");
        msg << MSG::VERBOSE 
            << "Pad (routing line) " << pad
            << " strip " << strip
            << endmsg;
      }
    }
  }
  
}

//==============================================================================
/// Return a trajectory (for track fit) from strip + offset
//==============================================================================
std::auto_ptr<LHCb::Trajectory> DeVLRSensor::trajectory(const LHCb::VLChannelID& id,
                                                        const double offset) const {

  // Trajectory is a circle.
  double z = 0.;
  const unsigned int strip = id.strip();
  double radius = rOfStrip(strip);
  double phiMin = phiMinStrip(strip);
  double phiMax = phiMaxStrip(strip);
  // Offset is offset on R
  radius += rPitch(strip) * offset;

  // Start with coordinates of center and both ends in local frame
  Gaudi::XYZPoint lOrigin(0., 0., 0.);
  Gaudi::XYZPoint lBegin(radius * cos(phiMin), radius * sin(phiMin), z);
  Gaudi::XYZPoint lEnd(radius * cos(phiMax), radius * sin(phiMax), z);

  // Move to global frame
  Gaudi::XYZPoint gOrigin = localToGlobal(lOrigin);
  Gaudi::XYZPoint gBegin  = localToGlobal(lBegin);
  Gaudi::XYZPoint gEnd    = localToGlobal(lEnd);
  // Convert phi range to [0, 2pi] on the right (C) side only 
  // to make sure trajectories run in right direction
  // and protect against crossing the -pi / pi boundary 
  double phiBeginTmp = gBegin.phi();
  double phiEndTmp   = gEnd.phi();
  if (isRight() && phiBeginTmp < 0) phiBeginTmp += 2 * Gaudi::Units::pi;
  if (isRight() && phiEndTmp < 0)   phiEndTmp   += 2 * Gaudi::Units::pi;
  if (phiBeginTmp > phiEndTmp) {
    Gaudi::XYZPoint gTmp = gBegin;
    gBegin = gEnd;
    gEnd = gTmp;
  }

  // Put into trajectory
  LHCb::Trajectory* tTraj = new LHCb::CircleTraj(gOrigin, gBegin - gOrigin, 
                                                 gEnd - gOrigin, radius);
  std::auto_ptr<LHCb::Trajectory> autoTraj(tTraj);
  return autoTraj;

}

StatusCode DeVLRSensor::updateStripCache() {

  m_stripsCache.resize(m_numberOfStrips);
  /// Register geometry conditions, update strip cache.
  // We do not use the radius at the centre of the strip, because in
  // the global frame this is no longer the occupancy weighted mean.
  // Instead we integrate over the strip wighted by occupancy which is
  // o = const./r.  The constant cancels in the weighted mean.
  // TODO: check
  for (unsigned int strip = 0; strip < m_numberOfStrips; ++strip) {
    double phiMin = phiMaxStrip(strip) > phiMinStrip(strip) ? 
                    phiMinStrip(strip) : phiMaxStrip(strip);
    double phiMax = phiMaxStrip(strip) > phiMinStrip(strip) ? 
                    phiMaxStrip(strip) : phiMinStrip(strip);
    double rLocal = rOfStrip(strip);

    double dphi  = (phiMax - phiMin) / 10.0;
    double num   = 0.;
    double hbden = 0.;
    double gden  = 0.;
    double phiLocal = phiMin;
    // Integrate over strip
    for (; phiLocal < phiMax; phiLocal += dphi) {
      Gaudi::XYZPoint lp(rLocal * cos(phiLocal), rLocal * sin(phiLocal), 0.);
      num += dphi;
      Gaudi::XYZPoint hbp = localToVeloHalfBox(lp);
      hbden += dphi / hbp.rho();
      Gaudi::XYZPoint gp = localToGlobal(lp);
      gden += dphi / gp.rho();
    }
    // Deal with the last interval, it might be shorter than the original dphi.
    dphi = phiMax - phiLocal + dphi;
    num += dphi;
    Gaudi::XYZPoint lp(rLocal * cos(phiMax), rLocal * sin(phiMax), 0.);
    Gaudi::XYZPoint hbp = localToVeloHalfBox(lp);
    hbden += dphi / hbp.rho();
    Gaudi::XYZPoint gp = localToGlobal(lp);
    gden += dphi / gp.rho();
    // Store the results.
    m_stripsCache[strip].globalR  = num / gden;
  }
  return StatusCode::SUCCESS;

}

StatusCode DeVLRSensor::updateZoneCache() {

  m_zonesCache.resize(m_numberOfZones);
  for (unsigned int lzone = 0; lzone < m_numberOfZones; ++lzone) {
    const unsigned int minStrip = m_zones[lzone].firstStrip;
    const unsigned int maxStrip = minStrip + m_zones[lzone].nbStrips - 1;
    const unsigned int midStrip = (minStrip + maxStrip) / 2;
    unsigned int zone = lzone;
    if (isDownstream()) {
      zone = m_numberOfZones - 1 - zone;
    }
    // Determine the phi ranges of the zone in the global frame.
    std::pair<Gaudi::XYZPoint, Gaudi::XYZPoint> globalLimitsMin = globalStripLimits(minStrip);
    std::pair<Gaudi::XYZPoint, Gaudi::XYZPoint> globalLimitsMax = globalStripLimits(maxStrip);
    std::pair<Gaudi::XYZPoint, Gaudi::XYZPoint> globalLimitsMid = globalStripLimits(midStrip);
    std::vector<double> phiLimits;
    phiLimits.push_back(globalLimitsMin.first.phi()); 
    phiLimits.push_back(globalLimitsMin.second.phi());
    phiLimits.push_back(globalLimitsMax.first.phi()); 
    phiLimits.push_back(globalLimitsMax.second.phi());
    phiLimits.push_back(globalLimitsMid.first.phi()); 
    phiLimits.push_back(globalLimitsMid.second.phi());
    // Map to [0, 2pi] for right hand side sensors.
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

    // R limits are the radii of the outer strip + local pitch / 2 and the inner strip - local pitch / 2
    m_zonesCache[zone].globalRLimits.first   = globalROfStrip(minStrip)  - rPitch(minStrip) / 2.;
    m_zonesCache[zone].globalRLimits.second  = globalROfStrip(maxStrip)  + rPitch(maxStrip) / 2.;
  }
  return StatusCode::SUCCESS;
  
}

StatusCode DeVLRSensor::updateGeometryCache() {

  StatusCode sc = updateStripCache();
  if (!sc.isSuccess()) {
    MsgStream msg(msgSvc(), "DeVLRSensor");
    msg << MSG::ERROR << "Failed to update strip cache." << endmsg;
    return sc;
  }

  sc = updateZoneCache();
  if (!sc.isSuccess()) {
    MsgStream msg(msgSvc(), "DeVLRSensor");
    msg << MSG::ERROR << "Failed to update zone limit cache." << endmsg;
    return sc;
  }
  return StatusCode::SUCCESS;
  
}
