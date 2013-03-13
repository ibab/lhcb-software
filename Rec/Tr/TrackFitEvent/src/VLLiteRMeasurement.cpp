// Event/DigiEvent
#include "Event/VLCluster.h"
#include "Event/VLLiteCluster.h"
// Det/VLDet
#include "VLDet/DeVL.h"
// Local
#include "Event/VLLiteRMeasurement.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : VLLiteRMeasurement
//
//-----------------------------------------------------------------------------

//============================================================================
/// Constructor with ref. vector
//============================================================================
VLLiteRMeasurement::VLLiteRMeasurement(const VLLiteCluster& lcl,
                                       const DeVL& det,
                                       const IVLClusterPosition& posTool,
                                       const LHCb::StateVector& ref)
  : VLLiteMeasurement(Measurement::VLLiteR, lcl) {
  
  init(det, posTool.position(cluster(), ref.position(), ref.tx(), ref.ty()));

}

//============================================================================
/// Constructor without ref. vector
//============================================================================
VLLiteRMeasurement::VLLiteRMeasurement(const VLLiteCluster& lcl,
                                       const DeVL& det, 
                                       const IVLClusterPosition& posTool) 
  : VLLiteMeasurement(Measurement::VLLiteR, lcl) {

  init(det, posTool.position(cluster()));

}

//============================================================================
/// Initialise data members
//============================================================================
void VLLiteRMeasurement::init(const DeVL& det, const IVLClusterPosition::toolInfo& clusInfo) {

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
const DeVLRSensor& VLLiteRMeasurement::sensor() const {
  return *static_cast<const DeVLRSensor*>(detectorElement());
}

