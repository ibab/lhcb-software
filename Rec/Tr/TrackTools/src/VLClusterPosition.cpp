// Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/Point3DTypes.h"
// Math
#include "LHCbMath/LHCbMath.h"
// Det/VLDet
#include "VLDet/DeVL.h"
// Event/DigiEvent
#include "Event/VLCluster.h"
// Local
#include "VLClusterPosition.h"

DECLARE_TOOL_FACTORY(VLClusterPosition)

typedef IVLClusterPosition::toolInfo toolInfo;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VLClusterPosition::VLClusterPosition(const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent)
  : GaudiTool(type, name, parent),
    m_det(0), m_p0(0), m_p1(0), m_minAngle(0.), m_maxAngle(0.) {

  declareInterface<IVLClusterPosition>(this);
  declareProperty("CalculateExactProjAngle", m_exactProjAngle = false);
  declareProperty("AngleBins", m_angleBins = false);
  declareProperty("SplineType", m_splineType = "Cspline");

}

//=============================================================================
StatusCode VLClusterPosition::initialize() {

  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;
  m_det = getDet<DeVL>(DeVLLocation::Default);

  m_p0Default = -6.301;
  m_p1Default = 0.27;
  // Initialise the spline interpolations.
  // TODO: these parameterizations are for the current VELO. To be modified for VL.
  const unsigned int nAngles = 24;
  // Angles (in rad).
  std::vector<double> angles(nAngles);
  for (unsigned int i = 0; i < nAngles; ++i) {
    angles[i] = i * Gaudi::Units::degree;
  }
  m_minAngle = angles.front();
  m_maxAngle = angles.back();
  // Constant terms. 
  const double p0[nAngles] = {
    -2.5,   -3.591, -3.430, -4.112, -4.964, -5.705, -6.301, -7.065,
    -7.353, -7.055, -6.168, -4.814, -3.075, -1.067,  0.798,  2.753,
     4.446,  5.116,  6.411,  7.124,  7.683,  7.573,  9.946, 10.97};
  // Slopes.
  const double p1[nAngles] = {
    0.29,  0.29, 0.28,   0.28,   0.28,   0.27,   0.27,    0.269,
    0.239, 0.23, 0.215,  0.18,   0.15,   0.11,   0.09,    0.06,
    0.04,  0.03, 0.0226, 0.0154, 0.0154, 0.0235, 0.0008, -0.0079};
   std::vector<double> p0Values(p0, p0 + nAngles);
   std::vector<double> p1Values(p1, p1 + nAngles);
  // Check the dimensions.
  GaudiMath::Interpolation::Type spline = GaudiMath::Interpolation::Linear;
  if ("Cspline" == m_splineType) {
    spline = GaudiMath::Interpolation::Cspline;
  } else if ("Polynomial" == m_splineType) {
    spline = GaudiMath::Interpolation::Polynomial;
  }
  m_p0 = new GaudiMath::SimpleSpline(angles, p0Values, spline);
  m_p1 = new GaudiMath::SimpleSpline(angles, p1Values, spline);
  return StatusCode::SUCCESS;

}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode VLClusterPosition::finalize() {

  if (m_p0) {
    delete m_p0; 
    m_p0 = 0;
  }
  if (m_p1) {
    delete m_p1; 
    m_p1 = 0;
  }
  return GaudiTool::finalize();

}

//=========================================================================
double VLClusterPosition::fraction(const LHCb::VLCluster* cluster) const {

  int nStrips = cluster->size();
  if (nStrips <= 1) return 0.;
  std::vector<LHCb::VLChannelID> strips = cluster->channels();
  double centre = 0.;
  double sum = 0.;
  int distance = 0;
  const DeVLSensor* sensor = m_det->sensor(strips[0].sensor());
  for (int i = 0; i < nStrips; ++i) {
    StatusCode sc = sensor->channelDistance(strips[0], strips[i], distance);
    sc.ignore();
    centre += static_cast<float>(distance) * cluster->adcValue(i);
    sum += cluster->adcValue(i);
  }
  centre /= sum;
  return centre - int(LHCb::Math::round(centre));

}

//=========================================================================
toolInfo VLClusterPosition::position(const LHCb::VLChannelID& channel,
                                     const double& fraction) const {

  toolInfo info;
  const DeVLSensor* sens = m_det->sensor(channel.sensor());
  if (!sens) {
    error() << "No valid pointer to sensor" << endmsg;
    info.strip = LHCb::VLChannelID(0);
    info.fractionalPosition = 0.;
    info.fractionalError = 0.;
    return info;
  }
  info.strip = channel;
  info.fractionalPosition = fraction;
  if (sens->isR()) {
    const DeVLRSensor* rSens = dynamic_cast<const DeVLRSensor*>(sens);
    const double r = rSens->rOfStrip(channel.strip(), fraction);
    const double pitch = rSens->rPitchOfRadius(r) / Gaudi::Units::micrometer;
    info.fractionalError = (m_p0Default + m_p1Default * pitch) / pitch;
  } else if (sens->isPhi()) {
    const DeVLPhiSensor* phiSens = dynamic_cast<const DeVLPhiSensor*>(sens);
    const unsigned int zone = sens->zoneOfStrip(channel.strip());
    const double r = 0.5 * (sens->rMin(zone) + sens->rMax(zone));
    const double pitch = phiSens->phiPitchOfRadius(r) / Gaudi::Units::micrometer;
    info.fractionalError = (m_p0Default + m_p1Default * pitch) / pitch;
  }
  return info;

}  

//=========================================================================
//=========================================================================
toolInfo VLClusterPosition::position(const LHCb::VLChannelID& channel,
                                     const double& fraction,
                                     const Gaudi::XYZPoint& point,
                                     const double tx, const double ty) const {
  
  toolInfo info;
  const DeVLSensor* sensor = m_det->sensor(channel.sensor());
  if (!sensor) {
    Error("No valid pointer to sensor");
    info.strip = LHCb::VLChannelID(0);
    info.fractionalPosition = 0.;
    info.fractionalError = 0.;
    return info;
  }
  info.strip = channel;
  info.fractionalPosition = fraction;
  // Calculate projected angle and local pitch.
  double angle = 0.;
  double pitch = 0.;
  projectedAngle(sensor, channel, tx, ty, point, angle, pitch);
  angle /= Gaudi::Units::rad;
  pitch /= Gaudi::Units::micrometer;
  info.fractionalError = errorEstimate(angle, pitch);
  return info;

}

//============================================================================
//============================================================================
double VLClusterPosition::angleOfTrack(const double tx, const double ty,
                                       Gaudi::XYZVector& parallel2Track) const {

  Gaudi::XYZVector perpPi1(1., 0., -tx);
  Gaudi::XYZVector perpPi2(0., 1., -ty);
  // Normalized vector parallel to the track
  Gaudi::XYZVector parallel = perpPi1.Cross(perpPi2);
  const double norm = sqrt(parallel.Mag2());
  parallel2Track = Gaudi::XYZVector(parallel.x() / norm, 
                                    parallel.y() / norm,
                                    parallel.z() / norm);
  return acos(parallel2Track.Dot(Gaudi::XYZVector(0., 0., 1.)));

}

//============================================================================
//============================================================================
double VLClusterPosition::errorEstimate(double angle,
                                        const double pitch) const {

  // Cut-off at unphysical angles
  if (angle < m_minAngle) angle = -angle;
  if (!m_angleBins) {
    // Use pitch bins 
    if (angle > m_maxAngle) angle = m_maxAngle;
    const double p0 = m_p0->eval(angle);
    const double p1 = m_p1->eval(angle);
    return (p0 + p1 * pitch) / pitch;
  }
  // Use angle bins 
  if (angle > m_maxAngle) return 0.28;
  const double p0 =   0.038 + 0.00033 * pitch;
  const double p1 = -25.52  + 0.167   * pitch;
  const double p2 =   1.447 + 0.0069  * pitch;
  const double p3 =   0.121 + 0.00032 * pitch;
  return 1.08 * (p0 * sin(p1 * angle + p2) + p3); 

}

//=========================================================================
//=========================================================================
toolInfo VLClusterPosition::position(const LHCb::VLCluster* cluster,
                                     const LHCb::StateVector& state) const {

  const unsigned int sensorNumber = cluster->channelID().sensor();
  const DeVLSensor* sensor = m_det->sensor(sensorNumber);
  // Build space point in global frame
  Gaudi::XYZPoint point(state.x(), state.y(), sensor->z());
  return position(cluster->channelID(), cluster->interStripFraction(), 
                  point, state.tx(), state.ty());

}

//=========================================================================
//=========================================================================
toolInfo VLClusterPosition::position(const LHCb::VLLiteCluster* cluster,
                                     const LHCb::StateVector& state) const {

  const unsigned int sensorNumber = cluster->channelID().sensor();
  const DeVLSensor* sensor = m_det->sensor(sensorNumber);
  // Build space point in global frame
  Gaudi::XYZPoint point(state.x(), state.y(), sensor->z());
  return position(cluster->channelID(), cluster->interStripFraction(), 
                  point, state.tx(), state.ty());

}

//=========================================================================
//=========================================================================
void VLClusterPosition::projectedAngle(const DeVLSensor* sensor,
                                       const LHCb::VLChannelID channel,
                                       double tx, double ty, 
                                       Gaudi::XYZPoint point,
                                       double& angle, double& pitch) const {

  // Transform global point to the local reference frame.
  Gaudi::XYZPoint localpoint = sensor->globalToLocal(point);
  unsigned int strip = channel.strip();
  // Get local track direction.
  if (sensor->isLeft()) {
    if (sensor->isDownstream()) tx = -tx;
  } else {
    ty = -ty;
    if (!sensor->isDownstream()) {
      tx = -tx;
    }
  }
  if (sensor->isR()) {
    // R sensor
    const DeVLRSensor* rSensor = dynamic_cast<const DeVLRSensor*>(sensor);
    const double r = rSensor->rOfStrip(strip);
    pitch = rSensor->rPitchOfRadius(r);
    if (m_exactProjAngle) {  
      Gaudi::XYZVector parallel2Track;
      double alphaOfTrack = angleOfTrack(tx, ty, parallel2Track);
      // Vector normal to the strip.
      Gaudi::XYZVector perp2RStrip(localpoint.x(), localpoint.y(), 0.);
      double cosTrackOnNormal = parallel2Track.Dot(perp2RStrip.Unit());
      // Projection of track on normal to local strip.
      double trackOnNormal = fabs(cosTrackOnNormal);
      // Projection of track on Z axis
      double trackOnZ = cos(alphaOfTrack);
      angle = atan(trackOnNormal / trackOnZ);
    } else { 
      angle = sqrt(tx * tx + ty * ty);
    }
  } else {
    // Phi sensor
    const DeVLPhiSensor* phiSensor = dynamic_cast<const DeVLPhiSensor*>(sensor);
    double r = localpoint.rho();
    std::auto_ptr<LHCb::Trajectory> traj = m_det->trajectory(LHCb::LHCbID(channel), 0.);
    double trajEnd = traj->endPoint().rho();
    double trajBeg = traj->beginPoint().rho();
    double rMax = trajEnd;
    if (trajEnd < trajBeg) rMax = trajBeg;
    double rMin = trajEnd;
    if (trajEnd > trajBeg) rMin = trajBeg;
    if (r > rMax) r = rMax;
    if (r < rMin) r = rMin;       
    pitch = phiSensor->phiPitchOfRadius(r);
    if (m_exactProjAngle) {
      Gaudi::XYZVector parallel2Track;
      std::pair<Gaudi::XYZPoint, Gaudi::XYZPoint> limits = phiSensor->localStripLimits(strip);
      Gaudi::XYZVector perp2PhiStrip((limits.first.y() - limits.second.y()),
                                     (limits.second.x() - limits.first.x()), 0.);
      double alphaOfTrack = angleOfTrack(tx, ty, parallel2Track);
      double cosTrackOnNormal = parallel2Track.Dot(perp2PhiStrip.Unit());
      double trackOnNormal = fabs(cosTrackOnNormal);
      double trackOnZ = cos(alphaOfTrack);
      angle = atan(trackOnNormal / trackOnZ);
    } else { 
      // Approximation
      Gaudi::XYZVector stripdir = traj->direction(0.5).Unit();
      double cosangle = (stripdir.x() * point.x() + stripdir.y() * point.y()) / point.rho();
      double stereoAngle = std::abs(cosangle) < 1 ? acos(cosangle) : 0;
      // for phi sensors, projection angle is diluted by stereo angle
      angle = sqrt(tx * tx + ty * ty) * stereoAngle;
    }
  }

}

