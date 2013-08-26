#include <vector>

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/Point3DTypes.h"

#include "LHCbMath/LHCbMath.h"

#include "VPDet/DeVP.h"
#include "Event/VPCluster.h"

#include "VPClusterPosition.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VeloClusterPosition
//
// 2010-02-02 : Victor Coco
// 
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY(VPClusterPosition)

typedef IVPClusterPosition::toolInfo toolInfo;
typedef IVPClusterPosition::Direction Direction;
typedef std::pair<double, double> Pair;

//=============================================================================
/// Standard constructor, initializes variables
//=============================================================================
VPClusterPosition::VPClusterPosition(const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent) :
    GaudiTool(type, name, parent),
    m_det(NULL) {

  declareInterface<IVPClusterPosition>(this);
  m_p.reserve(7);

}

//=============================================================================
/// Destructor
//=============================================================================
VPClusterPosition::~VPClusterPosition() {} 

//=============================================================================
/// Initialisation 
//=============================================================================
StatusCode VPClusterPosition::initialize() {

  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;
  m_det = getDet<DeVP>(DeVPLocation::Default);
  // Set parameters for polynomial fit of resolution as function of angle.
  m_p.resize(7);
  m_p[0] = 11.0187;
  m_p[1] = -0.0487707;
  m_p[2] = -0.0930352;
  m_p[3] = 0.000938413;
  m_p[4] = 0.000709917;
  m_p[5] = -3.85402e-05;
  m_p[6] = 5.72957e-07; 

  return StatusCode::SUCCESS;

}

//=========================================================================
/// Position given cluster only
//=========================================================================
toolInfo VPClusterPosition::position(const LHCb::VPLiteCluster* cluster) const {

  toolInfo info;
  const DeVPSensor* sensor = m_det->sensor(cluster->channelID().sensor());
  Gaudi::XYZPoint point = sensor->globalXYZ(cluster->channelID().pixel(), 
                                            cluster->interPixelFraction());
  Direction direction;
  direction.first = point.x() / point.z();
  direction.second = point.y() / point.z();

  return position(cluster->channelID(), cluster->interPixelFraction(),
                  point, direction);

}

//=========================================================================
/// Position given cluster, point and direction
//=========================================================================
toolInfo VPClusterPosition::position(const LHCb::VPLiteCluster* cluster,
                                     const Gaudi::XYZPoint& point,
                                     const Direction& direction) const {

  return position(cluster->channelID(), cluster->interPixelFraction(),
                  point, direction);

}

//=========================================================================
/// Position given cluster and track state
//=========================================================================
toolInfo VPClusterPosition::position(const LHCb::VPLiteCluster* cluster,
                                     const LHCb::StateVector& state) const {
  
  const DeVPSensor* sensor = m_det->sensor(cluster->channelID().sensor());
  // Get space point in global frame
  Gaudi::XYZPoint point(state.x(), state.y(), sensor->z());
  // Get direction 
  Direction direction;
  direction.first = state.tx();
  direction.second = state.ty();
  
  return position(cluster->channelID(), cluster->interPixelFraction(), 
                  point, direction);

}

//=========================================================================
/// Calculate position and error
//=========================================================================
toolInfo VPClusterPosition::position(const LHCb::VPChannelID& channel,
                                     const Pair& fraction,
                                     const Gaudi::XYZPoint& point,
                                     const Direction& direction) const {

  toolInfo info;
  const DeVPSensor* sensor = m_det->sensor(channel.sensor());
  if (!sensor) {
    Error("No valid pointer to sensor");
    info.pixel = LHCb::VPChannelID(0);
    info.fractionalPosition = std::make_pair(0., 0.);
    info.fractionalError = std::make_pair(0., 0.);
    return info;
  }

  info.pixel = channel;
  info.fractionalPosition = fraction;
  Gaudi::XYZVector track(direction.first, direction.second, 1.);
  // Get the projected angles between track direction and sensor
  Pair angles = projectedAngles(sensor, track, point);
  Pair pixelSize = sensor->pixelSize(channel);
  // Error estimate
  info.fractionalError = errorEstimate(angles, pixelSize);
  return info;

}

//============================================================================
/// Calculate the projected angles (in rad)
//============================================================================
Pair VPClusterPosition::projectedAngles(const DeVPSensor* sensor, 
                                        Gaudi::XYZVector track, 
                                        Gaudi::XYZPoint globalPoint) const {

  // Transform global point to the local reference frame.
  Gaudi::XYZPoint localPoint = sensor->globalToLocal(globalPoint);
  Gaudi::XYZPoint vGlobal(globalPoint.x() - track.x(),
                          globalPoint.y() - track.y(),
                          globalPoint.z() - track.z());
  Gaudi::XYZPoint vLocal = sensor->globalToLocal(vGlobal);
  const double dx = localPoint.x() - vLocal.x();
  const double dy = localPoint.y() - vLocal.y();
  const double dz = localPoint.z() - vLocal.z();
  return std::make_pair(atan(dx / dz), atan(dy / dz));

}

//=========================================================================
/// Projected angles can be the angles between track and sensor
//=========================================================================
Pair VPClusterPosition::errorEstimate(const Pair angles, const Pair pixelSize) const {

  // Convert angles to degrees.
  const double thetax = angles.first / Gaudi::Units::degree;
  const double thetay = angles.second / Gaudi::Units::degree;
  // Calculate resolution in micron (based on fit to testbeam data).
  // TODO: to be improved!
  double errx = m_p[0] + thetax * m_p[1] + thetax * thetax * m_p[2] +
                pow(thetax, 3) * m_p[3] + pow(thetax, 4) * m_p[4] +
                pow(thetax, 5) * m_p[5] + pow(thetax, 6) * m_p[6];
  double erry = m_p[0] + thetay * m_p[1] + thetay * thetay * m_p[2] +
                pow(thetay, 3) * m_p[3] + pow(thetay, 4) * m_p[4] +
                pow(thetay, 5) * m_p[5] + pow(thetay, 6) * m_p[6];
  errx *= 1.e-3 / pixelSize.first;
  erry *= 1.e-3 / pixelSize.second;
  return std::make_pair(errx, erry);

}

