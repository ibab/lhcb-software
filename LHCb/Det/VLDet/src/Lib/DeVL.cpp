// STL
#include <algorithm>
// Gaudi
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IUpdateManagerSvc.h"
#include "GaudiKernel/SystemOfUnits.h"
// Det/DetDesc 
#include "DetDesc/Condition.h"
// Local 
#include "VLDet/DeVL.h"

/** @file DeVL.cpp
 *
 *  Implementation of class : DeVL
 *
 */

//============================================================================
/// Constructor
//============================================================================
DeVL::DeVL(const std::string& name) : DetectorElement(name) {

}

//============================================================================
/// Object identification
//============================================================================
const CLID& DeVL::clID() const { 
  
  return DeVL::classID();

}


//============================================================================
/// Initialisation method
//============================================================================
StatusCode DeVL::initialize() {

  // Set the output level.
  PropertyMgr* pmgr = new PropertyMgr();
  int outputLevel = 0;
  pmgr->declareProperty("OutputLevel", outputLevel);
  IJobOptionsSvc* jobSvc;
  ISvcLocator* svcLoc = Gaudi::svcLocator();
  StatusCode sc = svcLoc->service("JobOptionsSvc", jobSvc);
  if (sc.isSuccess()) sc = jobSvc->setMyProperties("DeVL", pmgr);
  if (outputLevel > 0) {
    msgSvc()->setOutputLevel("DeVL", outputLevel);
  }
  delete pmgr;
  if (!sc) return sc;
  m_debug   = (msgSvc()->outputLevel("DeVL") == MSG::DEBUG);
  m_verbose = (msgSvc()->outputLevel("DeVL") == MSG::VERBOSE);
  if (m_verbose) m_debug = true;
  MsgStream msg(msgSvc(), "DeVL");
  
  // Initialise the base class.
  sc = DetectorElement::initialize();
  if (sc.isFailure()) {
    msg << MSG::ERROR << "Failed to initialise DetectorElement." << endmsg;
    return sc;
  }
  
  // Find DeVLSensor objects in detector element tree.
  std::vector<DeVLSensor*> vlSensors;
  findSensors(this, vlSensors);
  if (m_debug) {
    msg << MSG::DEBUG
        << "Found " << vlSensors.size() 
        << " sensors in the XML" << endmsg;
  }
  // Sort sensors by increasing z.
  std::sort(vlSensors.begin(), vlSensors.end(), lessZ());

  // Determine the highest sensor number in the list of DeVLSensor objects.
  unsigned int maxSensorNumber = 0;
  std::vector<DeVLSensor*>::iterator it;
  for (it = vlSensors.begin(); it != vlSensors.end(); ++it) {
    if (maxSensorNumber < (*it)->sensorNumber()) {
      maxSensorNumber = (*it)->sensorNumber();
    }
  }
  m_sensors.resize(maxSensorNumber + 1, 0);

  m_nSensors = m_nRSensors = m_nPhiSensors = 0;
  m_nLeftSensors    = m_nRightSensors    = 0;
  m_nLeftRSensors   = m_nRightRSensors   = 0;
  m_nLeftPhiSensors = m_nRightPhiSensors = 0;
  for (it = vlSensors.begin(); it != vlSensors.end(); ++it) {
    m_vpSensors.push_back(*it);
    ++m_nSensors;
    const unsigned int index = m_vpSensors.size() - 1;

    // Check if sensor is on left/right side of LHCb
    bool isLeftSensor = false;
    if ((*it)->isLeft()) {
      isLeftSensor = true;
      m_vpLeftSensors.push_back(m_vpSensors.back());
      ++m_nLeftSensors;
    } else {
      m_vpRightSensors.push_back(m_vpSensors.back());
      ++m_nRightSensors;
    }
    
    // Check if sensor is R type or Phi type
    if ((*it)->isR()) {
      m_vpRSensors.push_back(dynamic_cast<DeVLRSensor*>((*it)));
      ++m_nRSensors;
      if (isLeftSensor) {
        m_vpLeftRSensors.push_back(m_vpRSensors.back());
        ++m_nLeftRSensors;
      } else {
        m_vpRightRSensors.push_back(m_vpRSensors.back());
        ++m_nRightRSensors;
      }
    } else if ((*it)->isPhi()) {
      m_vpPhiSensors.push_back(dynamic_cast<DeVLPhiSensor*>((*it)));
      ++m_nPhiSensors;
      if (isLeftSensor) {
        m_vpLeftPhiSensors.push_back(m_vpPhiSensors.back());
        ++m_nLeftPhiSensors;
      } else {
        m_vpRightPhiSensors.push_back(m_vpPhiSensors.back());
        ++m_nRightPhiSensors;
      }
    } else {
      msg << MSG::ERROR << "Sensor type is unknown" << endmsg;
    }
    m_sensors[m_vpSensors[index]->sensorNumber()] = m_vpSensors[index];
    if (m_debug) {
      msg << MSG::DEBUG 
          << "Module " << std::setw(4) << m_vpSensors[index]->module()
          << ", sensor " << std::setw(3) << m_vpSensors[index]->sensorNumber()
          << " (type " << std::setw(15) << m_vpSensors[index]->type() 
          << "): z = " << std::setw(8) << m_vpSensors[index]->z()
          << endmsg;
    }
  }

  // Set the sensor links.
  std::vector<DeVLRSensor*>::const_iterator itr;
  for (itr = m_vpLeftRSensors.begin(); itr != m_vpLeftRSensors.end(); ++itr) {
    DeVLRSensor* leftR = *itr;
    // Phi sensor on the left side
    DeVLPhiSensor* leftPhi = const_cast<DeVLPhiSensor*>(phiSensor(leftR->sensorNumber() + 64));
    if (!leftPhi) {
      msg << MSG::ERROR 
          << "Sensor number " << leftR->sensorNumber() 
          << " has no associated left side Phi sensor." << endmsg;
      continue;
    }
    // Set the associated sensor on the left side.
    leftR->setAssociatedPhiSensor(leftPhi);
    leftPhi->setAssociatedRSensor(leftR);
    // Set the associated sensors on the right side.
    DeVLRSensor* rightR = const_cast<DeVLRSensor*>(rSensor(leftR->sensorNumber() + 1));
    DeVLPhiSensor* rightPhi = const_cast<DeVLPhiSensor*>(phiSensor(leftPhi->sensorNumber() + 1));
    if (!rightR) {
      msg << MSG::ERROR 
          << "Sensor number " << leftR->sensorNumber() 
          << " has no associated right side R sensor." << endmsg;
      continue;
    }
    if (!rightPhi) {
      msg << MSG::ERROR 
          << "Sensor number " << leftR->sensorNumber() 
          << " has no associated right side Phi sensor." << endmsg;
      continue;
    }
    rightR->setAssociatedPhiSensor(rightPhi);
    rightPhi->setAssociatedRSensor(rightR);
    // Set the other side sensor links.
    rightR->setOtherSideRSensor(leftR);
    rightR->setOtherSidePhiSensor(leftPhi);
    leftR->setOtherSideRSensor(rightR);
    leftR->setOtherSidePhiSensor(rightPhi);
    rightPhi->setOtherSidePhiSensor(leftPhi);
    rightPhi->setOtherSideRSensor(leftR);
    leftPhi->setOtherSidePhiSensor(rightPhi);
    leftPhi->setOtherSideRSensor(rightR);
    // Set next R and phi sensors on the same side.
    unsigned int nextSensorNumber = leftR->sensorNumber() + 2;
    if (nextSensorNumber < m_nRSensors) {
      if (rSensor(nextSensorNumber)) {
        DeVLRSensor* next = const_cast<DeVLRSensor*>(rSensor(nextSensorNumber));
        leftR->setNextRSensor(next);
        next->setPreviousRSensor(leftR);
      }
    }
    nextSensorNumber = rightR->sensorNumber() + 2;
    if (nextSensorNumber < m_nRSensors) {
      if (rSensor(nextSensorNumber)) {
        DeVLRSensor* next = const_cast<DeVLRSensor*>(rSensor(nextSensorNumber));
        rightR->setNextRSensor(next);
        next->setPreviousRSensor(rightR);
      }
    }
    nextSensorNumber = leftPhi->sensorNumber() + 2;
    if (nextSensorNumber < 64 + m_nPhiSensors) {
      if (phiSensor(nextSensorNumber)) {
        DeVLPhiSensor* next = const_cast<DeVLPhiSensor*>(phiSensor(nextSensorNumber));
        leftPhi->setNextPhiSensor(next);
        next->setPreviousPhiSensor(leftPhi);
      }
    }
    nextSensorNumber = rightPhi->sensorNumber() + 2;
    if (nextSensorNumber < 64 + m_nPhiSensors) {
      if (phiSensor(nextSensorNumber)) {
        DeVLPhiSensor* next = const_cast<DeVLPhiSensor*>(phiSensor(nextSensorNumber));
        rightPhi->setNextPhiSensor(next);
        next->setPreviousPhiSensor(rightPhi);
      }
    }
  }
  if (m_debug) {
    msg << MSG::DEBUG 
        << "There are " << m_nSensors 
        << " sensors     (left: " << m_nLeftSensors
        << ", right: " << m_nRightSensors << ")" << endmsg;
    msg << MSG::DEBUG 
        << "There are " << m_nRSensors 
        << " R sensors   (left: " << m_nLeftRSensors
        << ", right: " << m_nRightRSensors << ")" << endmsg;
    msg << MSG::DEBUG 
        << "There are " << m_nPhiSensors 
        << " Phi sensors (left: " << m_nLeftPhiSensors
        << ", right: " << m_nRightPhiSensors << ")" << endmsg;
  }
  sc = registerConditionCallBacks();
  if (sc.isFailure()) {
    msg << MSG::ERROR 
        << "Failed to register condition update call backs." << endmsg;
    return sc;
  }

  // Get the value of the sensitiveVolumeCut from the XML.
  m_sensVolCut = param<double>("sensitiveVolumeCut");
  return StatusCode::SUCCESS;
  
}

//============================================================================
/// Recursive navigation through DeVelo detector element tree
//============================================================================
void DeVL::findSensors(IDetectorElement* detElem,
                       std::vector<DeVLSensor*>& sensors) {
                                 
  std::vector<IDetectorElement*> vlSensors = detElem->childIDetectorElements();
  MsgStream msg(msgSvc(), "DeVL");
  std::vector<IDetectorElement*>::iterator it; 
  for (it = vlSensors.begin(); it != vlSensors.end(); ++it) {
    DeVLSensor* pSensor = dynamic_cast<DeVLSensor*>((*it));
    if (pSensor) {
      sensors.push_back(pSensor);
      if (m_debug) {
        msg << MSG::DEBUG 
            << "Storing detector " << (*it)->name() << endmsg;
      }
    }
    findSensors(*it, sensors);
  }
  
}

//============================================================================
/// Return the sensor number for a point in the global frame. 
//============================================================================
const DeVLSensor* DeVL::sensor(const Gaudi::XYZPoint& point) const {

  return sensor(sensitiveVolumeID(point));

}

//============================================================================
/// Return the sensitive volume ID for a point in the global frame.
//============================================================================
int DeVL::sensitiveVolumeID(const Gaudi::XYZPoint& point) const {

  std::vector<DeVLSensor*>::const_iterator it; 
  for (it = m_vpSensors.begin(); it != m_vpSensors.end(); ++it) {
    Gaudi::XYZPoint localPoint = (*it)->globalToLocal(point);
    const double z = localPoint.z();
    if (m_sensVolCut > fabs(z)) return ((*it)->sensorNumber());
  }
  MsgStream msg(msgSvc(), "DeVL");
  msg << MSG::ERROR
      << "No sensitive volume at z = " << point.z() << endmsg;
  return -999;

}

// =========================================================================
/// Members related to condition caching
// =========================================================================
StatusCode DeVL::registerConditionCallBacks() {
  
  // Half box offsets
  if (m_nLeftSensors > 0) {
    updMgrSvc()->registerCondition(this,
                                   m_vpLeftSensors.front()->geometry(),
                                   &DeVL::updateLeftHalfBoxOffset);
  }
  if (m_nRightSensors > 0) {
    updMgrSvc()->registerCondition(this,
                                   m_vpRightSensors.front()->geometry(),
                                   &DeVL::updateRightHalfBoxOffset);
  }
  StatusCode sc = updMgrSvc()->update(this);
  if (!sc.isSuccess()) {
    MsgStream msg(msgSvc(), "DeVL");
    msg << MSG::ERROR
        << "Failed to update conditions." << endmsg;
    return sc;
  }
  return StatusCode::SUCCESS;

}

StatusCode DeVL::updateLeftHalfBoxOffset() {

  // Get the half-box detector element. 
  IDetectorElement* pair = m_vpLeftSensors.front()->parentIDetectorElement();
  IDetectorElement* module = pair->parentIDetectorElement();
  IDetectorElement* halfbox = module->parentIDetectorElement();
  // Get the half-box centre in the global frame.
  Gaudi::XYZPoint localZero(0., 0., 0.);
  Gaudi::XYZPoint globalZero = halfbox->geometry()->toGlobal(localZero);
  m_halfBoxOffsets[LeftHalf] = globalZero - localZero;
  return StatusCode::SUCCESS;

}

StatusCode DeVL::updateRightHalfBoxOffset() {

  // Get the half-box detector element. 
  IDetectorElement* pair = m_vpRightSensors.front()->parentIDetectorElement();
  IDetectorElement* module = pair->parentIDetectorElement();
  IDetectorElement* halfbox = module->parentIDetectorElement();
  // Get the half-box centre in the global frame.
  Gaudi::XYZPoint localZero(0., 0., 0.);
  Gaudi::XYZPoint globalZero = halfbox->geometry()->toGlobal(localZero);
  m_halfBoxOffsets[RightHalf] = globalZero - localZero;
  return StatusCode::SUCCESS;

}

