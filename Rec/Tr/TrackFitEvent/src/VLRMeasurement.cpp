// Det/VLDet
#include "VLDet/DeVL.h"
// Event/DigiEvent
#include "Event/VLCluster.h"
// Local
#include "Event/VLRMeasurement.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : VLRMeasurement
//
//-----------------------------------------------------------------------------

//============================================================================
/// Constructor from VLCluster with ref. vector
//============================================================================
VLRMeasurement::VLRMeasurement(const VLCluster& cl,
                               const DeVL& det,
                               const IVLClusterPosition& posTool,
                               const LHCb::StateVector& ref)
  : VLMeasurement(Measurement::VLR, cl) {

  init(det, posTool.position(cluster(), ref.position(), ref.tx(), ref.ty()));

}

//============================================================================
/// Constructor from VLCluster without ref. vector
//============================================================================
VLRMeasurement::VLRMeasurement(const VLCluster& cl,
                               const DeVL& det, 
                               const IVLClusterPosition& posTool) 
  : VLMeasurement(Measurement::VLR, cl) {

  init(det, posTool.position(cluster()));

}

//============================================================================
/// Initialise data members
//============================================================================
void VLRMeasurement::init(const DeVL& det, const IVLClusterPosition::toolInfo& clusInfo) {
  
  const DeVLRSensor* rDet = det.rSensor(channelID().sensor());
  m_detectorElement = rDet;
  m_z = rDet->z();

  const unsigned int strip = clusInfo.strip.strip();  
  m_measure = rDet->rOfStrip(strip) +
              rDet->rPitchOfStrip(strip) * clusInfo.fractionalPosition;
  m_errMeasure = rDet->rPitchOfStrip(strip) * clusInfo.fractionalError;
  m_trajectory = rDet->trajectory(clusInfo.strip, clusInfo.fractionalPosition);

}

//============================================================================
/// Return pointer to detector element
//============================================================================
const DeVLRSensor& VLRMeasurement::sensor() const {
  return *static_cast<const DeVLRSensor*>(detectorElement());
}

