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

/** @file DeVP.cpp
 *
 *  Implementation of class DeVP
 *
 *  @author Victor Coco Victor.Coco@cern.ch
 */


// ============================================================================
/// Constructor
// ============================================================================ 
DeVP::DeVP(const std::string& name) :  
    DetectorElement(name),
    m_nSensors(0), m_msg(NULL) {

} 

// ============================================================================
/// Destructor
// ============================================================================ 
DeVP::~DeVP() {

  if (m_msg) delete m_msg;

}

// ============================================================================
/// Object identification
// ============================================================================ 
const CLID& DeVP::clID() const {return DeVP::classID();}

// ============================================================================
/// Initialisation method
// ============================================================================ 
StatusCode DeVP::initialize() {

  // Set the output level.
  PropertyMgr* pmgr = new PropertyMgr();
  int outputLevel = 0;
  pmgr->declareProperty("OutputLevel", outputLevel);
  IJobOptionsSvc* jobSvc;
  ISvcLocator* svcLoc = Gaudi::svcLocator();
  StatusCode sc = svcLoc->service("JobOptionsSvc", jobSvc);
  if (sc.isSuccess()) sc = jobSvc->setMyProperties("DeVP", pmgr);
  if (outputLevel > 0) msgSvc()->setOutputLevel("DeVP", outputLevel);
  delete pmgr;
  if (!sc) return sc;
  m_debug = false;
  if (msgSvc()->outputLevel("DeVP") == MSG::DEBUG ||
      msgSvc()->outputLevel("DeVP") == MSG::VERBOSE) {
    m_debug = true;
  }

  // Initialise the base class.
  sc = DetectorElement::initialize();
  if (sc.isFailure()) { 
    msg() << MSG::ERROR << "Cannot initialize DetectorElement" << endmsg;
    return sc ; 
  }

  // Get all child detector elements
  std::vector<DeVPSensor*> sensors;
  findSensors(this, sensors);
  if (m_debug) {
    msg() << MSG::DEBUG << "Found " << sensors.size() << " sensors" << endmsg; 
  }
  std::sort(sensors.begin(), sensors.end(), less_Z());

  // Find the highest sensor number.
  std::vector<DeVPSensor*>::iterator it;
  unsigned int maxSensorNumber = 0;
  for (it = sensors.begin(); it != sensors.end(); ++it) { 
    if (maxSensorNumber < (*it)->sensorNumber()) {
       maxSensorNumber = (*it)->sensorNumber();
    }
  } 
  m_sensors.resize(maxSensorNumber + 1, 0);

  m_nSensors = 0;
  unsigned int nLeftSensors = 0;
  unsigned int nRightSensors = 0;
  for (it = sensors.begin(); it != sensors.end(); ++it) {
    m_vpSensors.push_back(*it);
    ++m_nSensors;
    if ((*it)->isLeft()) {
      m_leftSensors.push_back(*it);
      ++nLeftSensors;
    } else {
      m_rightSensors.push_back(*it);
      ++nRightSensors;
    }
    m_sensors[(*it)->sensorNumber()] = *it;
  }
  msg() << MSG::INFO << "There are " << m_nSensors << " sensors "
                     << "(left: " << nLeftSensors 
                     << ", right: " << nRightSensors << ")" << endmsg;
  return StatusCode::SUCCESS;

}

// ============================================================================
/// Get sensitive volume identifier for a given point in the global frame
// ============================================================================ 
int DeVP::sensitiveVolumeID(const Gaudi::XYZPoint& point) const {
  
  std::vector<DeVPSensor*>::const_iterator it;
  for (it = m_vpSensors.begin(); it != m_vpSensors.end(); ++it) {
    if ((*it)->isInside(point)) return (*it)->sensorNumber();
  }
  msg() << MSG::ERROR << "No sensitive volume at (" 
        << point.x() << ", " << point.y() << ", " << point.z() << ")" << endmsg;
  return -999;

}

// ============================================================================
/// Return pointer to sensor for a given point in the global frame
// ============================================================================ 
const DeVPSensor* DeVP::sensor(const Gaudi::XYZPoint& point) const {
  
  const int sensorNumber = sensitiveVolumeID(point);
  if (sensorNumber >= 0) { 
    return m_sensors[sensitiveVolumeID(point)];
  }
  return NULL;

}

// ============================================================================
/// Scan detector element tree for VP sensors
// ============================================================================ 
void DeVP::findSensors(IDetectorElement* det,
                       std::vector<DeVPSensor*>& sensors) {

  std::vector<IDetectorElement*> elements = det->childIDetectorElements();
  std::vector<IDetectorElement*>::iterator it;
  for (it = elements.begin(); it != elements.end(); ++it) {
    if (m_debug) {
      msg() << MSG::DEBUG << std::setw(12) << std::setiosflags(std::ios::left)
                          << (*it)->name() << endmsg;
    }
    DeVPSensor* p = dynamic_cast<DeVPSensor*>(*it);
    if (p) {
      sensors.push_back(p);
      if (m_debug) {
        msg() << MSG::DEBUG << "Storing detector " << (*it)->name() << endmsg;
      } 
    }
    findSensors(*it, sensors);
  }

}

