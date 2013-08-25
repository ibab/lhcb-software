#include "VPDet/DeVP.h"
#include "Kernel/LineTraj.h"
#include "LHCbMath/LHCbMath.h"

#include "Event/VPLiteMeasurement.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : VPLiteMeasurement
//
// Author: Victor Coco, based on solution used in MuonMeasurement by Jose Hernando, 
// Miriam Gandelman, Erica Polycarpo, Silvia Pozzi
// Created: 12-02-2010
//-----------------------------------------------------------------------------

/// Constructor from VPLiteCluster
VPLiteMeasurement::VPLiteMeasurement(const DeVP& det,const VPLiteCluster& cluster,
                                     const IVPClusterPosition& clusPosTool, 
                                     VPLiteMeasurement::VPLiteMeasurementType& xy) :
    Measurement(Measurement::VPLite, cluster.channelID(), 0),
    m_projection(xy),
    m_cluster(&cluster) {
  IVPClusterPosition::toolInfo info = clusPosTool.position(this->cluster()); 
  init(det, info);
  
}

/// Constructor from VPLiteCluster and known track state
VPLiteMeasurement::VPLiteMeasurement(const DeVP& det,const VPLiteCluster& cluster,
                                     const IVPClusterPosition& clusPosTool, 
                                     VPLiteMeasurement::VPLiteMeasurementType& xy,
                                     const LHCb::StateVector& ref) :
    Measurement(Measurement::VPLite, cluster.channelID(), 0),
    m_projection(xy),
    m_cluster(&cluster) {

  IVPClusterPosition::toolInfo info = 
    clusPosTool.position(this->cluster(), ref.position(),
                         std::pair<double, double>(ref.tx(), ref.ty())); 
  init(det, info);  

}

void VPLiteMeasurement::init(const DeVP& det, const IVPClusterPosition::toolInfo& info) {

  const DeVPSensor* sensor = det.sensor(info.pixel.sensor()); 
  const unsigned long pixel = info.pixel.pixel(); 
  m_position = sensor->globalXYZ(pixel, info.fractionalPosition);
  std::pair<double, double> pixelSize = sensor->pixelSize(pixel);
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
VPLiteMeasurement::VPLiteMeasurement(const VPLiteMeasurement& other) : 
    Measurement(other), m_projection(other.m_projection), m_cluster(other.m_cluster) {

}

