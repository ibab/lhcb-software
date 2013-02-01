// Event/DigiEvent
#include "Event/VLCluster.h"
// Det/VLDet
#include "VLDet/DeVL.h"
// Local
#include "Event/VLMeasurement.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : VLMeasurement
//
//-----------------------------------------------------------------------------

//============================================================================
/// Constructor from VLCluster with ref. vector
//============================================================================
VLMeasurement::VLMeasurement(const VLCluster* cluster, const DeVL* det,
                             const IVLClusterPosition& pos,
                             const LHCb::StateVector& ref) 
  : Measurement(Measurement::VL, cluster->channelID(), 0) { 

  m_channel = cluster->channelID();
  IVLClusterPosition::toolInfo clusInfo = pos.position(cluster, ref.position(),
                                                       ref.tx(), ref.ty());
  init(det, clusInfo);

}
//============================================================================
/// Constructor from VLLiteCluster with ref. vector
//============================================================================
VLMeasurement::VLMeasurement(const VLLiteCluster* cluster, const DeVL* det,
                             const IVLClusterPosition& pos,
                             const LHCb::StateVector& ref) 
  : Measurement(Measurement::VL, cluster->channelID(), 0) {
 
  m_channel = cluster->channelID();
  IVLClusterPosition::toolInfo clusInfo = pos.position(cluster, ref.position(),
                                                       ref.tx(), ref.ty());
  init(det, clusInfo);

}
//============================================================================
/// Constructor from VLCluster without ref. vector
//============================================================================
VLMeasurement::VLMeasurement(const VLCluster* cluster, const DeVL* det,
                             const IVLClusterPosition& pos)  
  : Measurement(Measurement::VL, cluster->channelID(), 0) { 

  m_channel = cluster->channelID();
  IVLClusterPosition::toolInfo clusInfo = pos.position(cluster);
  init(det, clusInfo);

}
//============================================================================
/// Constructor from VLLiteCluster without ref. vector
//============================================================================
VLMeasurement::VLMeasurement(const VLLiteCluster* cluster, const DeVL* det,
                             const IVLClusterPosition& pos)  
  : Measurement(Measurement::VL, cluster->channelID(), 0) { 

  m_channel = cluster->channelID();
  IVLClusterPosition::toolInfo clusInfo = pos.position(cluster);
  init(det, clusInfo);

}

//============================================================================
/// Initialise 
//============================================================================
void VLMeasurement::init(const DeVL* det, const IVLClusterPosition::toolInfo& clusInfo) {

  const unsigned int strip = m_channel.strip();
  const double fraction = clusInfo.fractionalPosition;
  if (m_channel.isPhiType()) {
    const DeVLPhiSensor* sensor = det->phiSensor(m_channel.sensor());
    m_detectorElement = sensor;
    m_z = sensor->z();
    m_origin = Gaudi::XYZPoint(sensor->xCentre(), sensor->yCentre(), m_z);
    m_measure = sensor->distToOrigin(strip);
    // Fix sign convention of d0 of strip
    if (!sensor->isDownstream()) {
      m_measure = -m_measure;
    }
    const double pitch = sensor->phiPitchOfStrip(strip);
    m_errMeasure = clusInfo.fractionalError * pitch;
    m_trajectory = sensor->trajectory(strip, fraction);
  } else {
    const DeVLRSensor* sensor = det->rSensor(m_channel.sensor());
    m_detectorElement = sensor;
    m_z = sensor->z();
    const double pitch = sensor->rPitchOfStrip(m_channel.strip());
    m_measure = sensor->rOfStrip(strip) + pitch * fraction;
    m_errMeasure = pitch;
    m_trajectory = sensor->trajectory(strip, fraction);
  } 

}

//============================================================================
/// Retrieve the measurement error
//============================================================================
double VLMeasurement::resolution(const Gaudi::XYZPoint& p,
                                 const Gaudi::XYZVector& /*v*/) const {

  if (m_channel.isPhiType()) {
    const double radius = (m_origin - p).R();
    return radius * m_errMeasure;
  }
  return m_errMeasure;

}

//============================================================================
/// Retrieve the measurement error squared
//============================================================================
double VLMeasurement::resolution2(const Gaudi::XYZPoint& p,
                                  const Gaudi::XYZVector& v) const {

  const double res = resolution(p, v);
  return res * res;

}

