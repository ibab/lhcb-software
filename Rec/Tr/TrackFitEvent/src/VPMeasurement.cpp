#include "VPDet/DeVP.h"
#include "Kernel/LineTraj.h"

#include "Event/VPMeasurement.h"

using namespace LHCb;

//=============================================================================
// Constructor
//=============================================================================
VPMeasurement::VPMeasurement(const VPCluster& cluster,
                             const VPPositionInfo& info,
                             VPMeasurement::VPMeasurementType& xy) :
    Measurement(Measurement::VP, cluster.channelID(), 0),
    m_projection(xy),
    m_cluster(&cluster) {

  Gaudi::XYZPoint position(info.x, info.y, m_cluster->z());
  setZ(m_cluster->z());
  if (m_projection == X) {
    setMeasure(info.x);
    setErrMeasure(info.dx);
    Gaudi::XYZVector yDir(0, 1, 0);
    std::pair<double, double> yRange(-info.dy, info.dy);
    m_trajectory.reset( new LineTraj(position, yDir, yRange, true) );
  } else {
    setMeasure(info.y);
    setErrMeasure(info.dy);
    Gaudi::XYZVector xDir(1, 0, 0);
    std::pair<double, double> xRange(-info.dx, info.dx);
    m_trajectory.reset( new LineTraj(position, xDir, xRange, true) );
  }

}

//=============================================================================
// Copy constructor
//=============================================================================
VPMeasurement::VPMeasurement(const VPMeasurement& other) :
    Measurement(other),
    m_projection(other.m_projection),
    m_cluster(other.m_cluster) {

}

