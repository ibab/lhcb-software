#include "VPDet/DeVP.h"
#include "Kernel/LineTraj.h"
#include "LHCbMath/LHCbMath.h"

#include "Event/VPMeasurement.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : VPMeasurement
//
//-----------------------------------------------------------------------------

/// Constructor from VPCluster
VPMeasurement::VPMeasurement(const DeVP& det,const VPCluster& cluster,
                             const IVPClusterPosition& clusPosTool, 
                             VPMeasurement::VPMeasurementType& xy) :
    Measurement(Measurement::VP, cluster.channelID(), 0),
    m_projection(xy),
    m_cluster(&cluster) {
  IVPClusterPosition::toolInfo info = clusPosTool.position(this->cluster()); 
  init(det, info);
  
}

/// Constructor from VPCluster and known track state
VPMeasurement::VPMeasurement(const DeVP& det,const VPCluster& cluster,
                             const IVPClusterPosition& clusPosTool, 
                             VPMeasurement::VPMeasurementType& xy,
                             const LHCb::StateVector& ref) :
    Measurement(Measurement::VP, cluster.channelID(), 0),
    m_projection(xy),
    m_cluster(&cluster) {

  IVPClusterPosition::toolInfo info = 
    clusPosTool.position(this->cluster(), ref.position(),
                         std::pair<double, double>(ref.tx(), ref.ty())); 
  init(det, info);  

}

void VPMeasurement::init(const DeVP& det, const IVPClusterPosition::toolInfo& info) {

  const DeVPSensor* sensor = det.sensorOfChannel(info.pixel); 
  m_position = sensor->channelToPoint(info.pixel, info.fractionalPosition);
  std::pair<double, double> pixelSize = sensor->pixelSize(info.pixel);
  double dx = pixelSize.first * info.fractionalError.first ;
  double dy = pixelSize.second * info.fractionalError.second;
  setZ(m_position.z());
  if (m_projection == X) {
    setMeasure(m_position.x());
    setErrMeasure(dx);
    Gaudi::XYZVector yDir(0, 1, 0); 
    std::pair<double, double> yRange(-dy, dy);
    m_trajectory = std::auto_ptr<Trajectory>(new LineTraj(m_position, yDir, yRange, true));
  } else {
    setMeasure(m_position.y());
    setErrMeasure(dy);
    Gaudi::XYZVector xDir(1, 0, 0); 
    std::pair<double, double> xRange(-dx, dx);
    m_trajectory = std::auto_ptr<Trajectory>(new LineTraj(m_position, xDir, xRange, true));  
  }

}

/// Copy constructor
VPMeasurement::VPMeasurement(const VPMeasurement& other) : 
    Measurement(other), m_projection(other.m_projection), m_cluster(other.m_cluster) {

}

