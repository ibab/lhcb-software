// STL (for std::sort)
#include <algorithm>

// Gaudi
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IUpdateManagerSvc.h"
#include "GaudiKernel/SystemOfUnits.h"

// Det/DetDesc
#include "DetDesc/Condition.h"

// Local
#include "VPDet/DeVP.h"

//============================================================================
// Constructor
//============================================================================
DeVP::DeVP(const std::string& name)
    : DetectorElement(name) {
}

//============================================================================
// Destructor
//============================================================================
DeVP::~DeVP() = default;

//============================================================================
// Object identification
//============================================================================
const CLID& DeVP::clID() const { 

  return DeVP::classID(); 

}

//============================================================================
// Initialisation method
//============================================================================
StatusCode DeVP::initialize() {

  // Set the output level.
  std::unique_ptr<PropertyMgr> pmgr{ new PropertyMgr() };
  int outputLevel = 0;
  pmgr->declareProperty("OutputLevel", outputLevel);
  ISvcLocator* svcLoc = Gaudi::svcLocator();
  auto jobSvc = svcLoc->service<IJobOptionsSvc>("JobOptionsSvc");
  if (jobSvc) {
      auto sc = jobSvc->setMyProperties("DeVP", pmgr.get());
      if (!sc) return sc;
  }
  if (outputLevel > 0) msgSvc()->setOutputLevel("DeVP", outputLevel);
  m_debug = (msgSvc()->outputLevel("DeVP") == MSG::DEBUG ||
             msgSvc()->outputLevel("DeVP") == MSG::VERBOSE);

  // Initialise the base class.
  auto sc = DetectorElement::initialize();
  if (sc.isFailure()) {
    msg() << MSG::ERROR << "Cannot initialize DetectorElement" << endmsg;
    return sc;
  }

  // Get all daughter detector elements.
  std::vector<DeVPSensor*> sensors;
  findSensors(this, sensors);
  if (m_debug) {
    msg() << MSG::DEBUG << "Found " << sensors.size() << " sensors" << endmsg;
  }
  std::sort(sensors.begin(), sensors.end(), less_SensorNumber());

  m_nSensors = 0;
  unsigned int nLeftSensors = 0;
  unsigned int nRightSensors = 0;
  for (auto it = sensors.begin(), end = sensors.end(); it != end; ++it) {
    m_sensors.push_back(*it);
    ++m_nSensors;
    if ((*it)->isLeft()) {
      ++nLeftSensors;
    } else {
      ++nRightSensors;
    }
  }
  msg() << MSG::INFO << "There are " << m_nSensors << " sensors "
        << "(left: " << nLeftSensors << ", right: " << nRightSensors << ")"
        << endmsg;
  return StatusCode::SUCCESS;

}

//============================================================================
// Get sensitive volume identifier for a given point in the global frame
//============================================================================
int DeVP::sensitiveVolumeID(const Gaudi::XYZPoint& point) const {

  const double z = point.z();
  const double tol = 10 * Gaudi::Units::mm; 
  auto i = std::find_if( m_sensors.begin(), m_sensors.end(),
                         [&]( const DeVPSensor* s) {
    // Quickly skip sensors which are far away in z, only
    // then check whether point is inside this sensor
    return (fabs(z - s->z()) < tol) && s->isInside(point);
  });
  if (i!=m_sensors.end()) return (*i)->sensorNumber();
  msg() << MSG::ERROR << "No sensitive volume at (" << point.x() << ", "
        << point.y() << ", " << point.z() << ")" << endmsg;
  return -999;

}

//============================================================================
// Scan detector element tree for VP sensors
//============================================================================
void DeVP::findSensors(IDetectorElement* det,
                       std::vector<DeVPSensor*>& sensors) {

  // Get the daughter detector elements.
  auto elements = det->childIDetectorElements();
  for (auto it = elements.begin(), end = elements.end(); it != end; ++it) {
    if (m_debug) {
      msg() << MSG::DEBUG << std::setw(12) << std::setiosflags(std::ios::left)
            << (*it)->name() << endmsg;
    }
    // Check if this is a VP sensor.
    DeVPSensor* p = dynamic_cast<DeVPSensor*>(*it);
    if (p) {
      // Add the sensor to the list.
      sensors.push_back(p);
      if (m_debug) {
        msg() << MSG::DEBUG << "Storing detector " << (*it)->name() << endmsg;
      }
    } else {
      // Recurse.
      findSensors(*it, sensors);
    }
  }
}
