#include <cmath>

// Gaudi
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/Point3DTypes.h"

// LHCb
// Det/VPDet
#include "VPDet/DeVP.h"
// Event/DigiEvent
#include "Event/VPCluster.h"

// Local
#include "VPClusterPosition.h"

DECLARE_TOOL_FACTORY(VPClusterPosition)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VPClusterPosition::VPClusterPosition(const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent)
    : GaudiTool(type, name, parent), m_det(nullptr) {

  declareInterface<IVPClusterPosition>(this);

  declareProperty("ErrorSinglePixel", 
                  m_errorSinglePixel = 12.5 * Gaudi::Units::micrometer); 
  declareProperty("ErrorTwoPixel", 
                  m_errorTwoPixel = 8. * Gaudi::Units::micrometer);
  declareProperty("ErrorAverage", 
                  m_errorAverage = 12. * Gaudi::Units::micrometer);

}

//=============================================================================
// Destructor
//=============================================================================
VPClusterPosition::~VPClusterPosition() {}

//=============================================================================
// Initialisation
//=============================================================================
StatusCode VPClusterPosition::initialize() {

  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;
  m_det = getDetIfExists<DeVP>(DeVPLocation::Default);
  if (!m_det) {
    return Error("No detector element at " + DeVPLocation::Default);
  }

  // Store the rotations of each sensor.
  const unsigned int nSensors = m_det->numberSensors();
  m_c2.resize(nSensors, 1.);
  m_s2.resize(nSensors, 0.);
  const Gaudi::XYZVector vl(1., 0., 0.);
  for (auto it = m_det->sensorsBegin(); it != m_det->sensorsEnd(); ++it) {
    const unsigned int sensor = (*it)->sensorNumber();
    if (sensor >= m_c2.size()) {
      m_c2.resize(sensor + 1);
      m_s2.resize(sensor + 1);
    }
    const auto vg = (*it)->geometry()->toGlobal(vl);
    const double cphi = vg.x();
    const double phi = acos(cphi);
    const double sphi = sin(phi);
    m_c2[sensor] = cphi * cphi;
    m_s2[sensor] = sphi * sphi;
  }
  return StatusCode::SUCCESS;
}

//=========================================================================
// Position and error given cluster only
//=========================================================================
LHCb::VPPositionInfo VPClusterPosition::position(const LHCb::VPCluster* cluster)
    const {

  Gaudi::XYZPoint point(cluster->x(), cluster->y(), cluster->z());
  // Assume that the track originates from the nominal interaction point.
  const double tx = point.x() / point.z();
  const double ty = point.y() / point.z();
  return position(cluster, point, tx, ty);
}

//=========================================================================
// Position and error given cluster, point and direction
//=========================================================================
LHCb::VPPositionInfo VPClusterPosition::position(const LHCb::VPCluster* cluster,
                                                 const Gaudi::XYZPoint& /*p*/,
                                                 const double& /*tx*/,
                                                 const double& /*ty*/) const {

  // TODO: include track information, parameterise error as function of angle.
  LHCb::VPPositionInfo pos;
  // Get the position directly from the cluster.
  pos.x = cluster->x();
  pos.y = cluster->y();
  // Get the sensor.
  const LHCb::VPChannelID channel = cluster->channelID();
  const DeVPSensor* sensor = m_det->sensorOfChannel(channel);
  if (!sensor) {
    Error("No valid pointer to sensor");
    pos.dx = 0.;
    pos.dy = 0.;
    return pos;
  }

  // Initialise the local error estimate.
  double dx = m_errorAverage;
  double dy = m_errorAverage;
  // Get the inter-pixel fraction.
  const double fx = cluster->fraction().first;
  const double fy = cluster->fraction().second;
  // Get the cluster size.
  const unsigned int nPixels = cluster->pixels().size();
  if (nPixels == 1) {
    // Single-pixel clusters
    dx = m_errorSinglePixel;
    dy = m_errorSinglePixel;
    if (sensor->isLong(channel)) dx *= 2.;
  } else if (nPixels == 2) {
    // Two-pixel clusters
    if (fx < 0.1) {
      dx = m_errorSinglePixel;
      if (sensor->isLong(channel)) dx *= 2.;
    } else {
      dx = m_errorTwoPixel;
      dx *= sensor->xPitch()[channel.scol()] / sensor->xPitch()[0];
    }
    dy = fy < 0.1 ? m_errorSinglePixel : m_errorTwoPixel;
  } else if (nPixels >= 9) {
    dx = 20. * Gaudi::Units::micrometer;
    dy = 20. * Gaudi::Units::micrometer;
  }
  // Transform the error estimate to the global frame.
  const unsigned int sensorNumber = sensor->sensorNumber();
  pos.dx = sqrt(dx * dx * m_c2[sensorNumber] + dy * dy * m_s2[sensorNumber]);
  pos.dy = sqrt(dx * dx * m_s2[sensorNumber] + dy * dy * m_c2[sensorNumber]);
  return pos;
}

