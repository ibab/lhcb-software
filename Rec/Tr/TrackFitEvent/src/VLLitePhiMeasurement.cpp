// Event/DigiEvent
#include "Event/VLCluster.h"
#include "Event/VLLiteCluster.h"
// Det/VLDet
#include "VLDet/DeVL.h"
// Local
#include "Event/VLLitePhiMeasurement.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : VLLitePhiMeasurement
//
//-----------------------------------------------------------------------------

//============================================================================
/// Constructor with ref. vector
//============================================================================
VLLitePhiMeasurement::VLLitePhiMeasurement(const VLLiteCluster& lcl,
                                           const DeVL& det,
                                           const IVLClusterPosition& posTool,
                                           const LHCb::StateVector& ref)
  : VLLiteMeasurement(Measurement::VLLitePhi, lcl) {

  init(det, posTool.position(cluster(), ref.position(), ref.tx(), ref.ty()));

}

//============================================================================
/// Constructor without ref. vector
//============================================================================
VLLitePhiMeasurement::VLLitePhiMeasurement(const VLLiteCluster& lcl,
                                           const DeVL& det,
                                           const IVLClusterPosition& posTool) 
  : VLLiteMeasurement(Measurement::VLLitePhi, lcl) {
  
  init(det, posTool.position(cluster()));

}

//============================================================================
/// Initialise data members
//============================================================================
void VLLitePhiMeasurement::init(const DeVL& det, const IVLClusterPosition::toolInfo& clusInfo) {

  const DeVLPhiSensor* phiDet = det.phiSensor(channelID());
  m_detectorElement = phiDet;
  m_z = phiDet->z();
  m_origin = Gaudi::XYZPoint(phiDet->xCentre(), phiDet->yCentre(), m_z);
  // Store the signed distance from strip to origin.
  m_measure = phiDet->distToOrigin(channelID().strip());
  // Fix sign convention of d0 of strip
  if (!phiDet->isDownstream()) {
    m_measure = -m_measure;
  }
  
  m_errMeasure = clusInfo.fractionalError * phiDet->phiPitchOfStrip(clusInfo.strip.strip());  
  m_trajectory = phiDet->trajectory(clusInfo.strip, clusInfo.fractionalPosition);

}

//============================================================================
/// Calculate the measurement error 
//============================================================================
double VLLitePhiMeasurement::resolution(const Gaudi::XYZPoint& point,
                                        const Gaudi::XYZVector& /*vec*/) const {

  const double radius = (m_origin - point).R();
  return radius * m_errMeasure;

}

//============================================================================
/// Calculate the measurement error squared 
//============================================================================
double VLLitePhiMeasurement::resolution2(const Gaudi::XYZPoint& point,
                                         const Gaudi::XYZVector& /*vec*/) const {

  const double radius = (m_origin - point).R();
  return radius * m_errMeasure * radius * m_errMeasure;

}
//============================================================================
/// Return pointer to detector element
//============================================================================
const DeVLPhiSensor& VLLitePhiMeasurement::sensor() const { 
  return *static_cast<const DeVLPhiSensor *>(detectorElement()); 
}                                   


