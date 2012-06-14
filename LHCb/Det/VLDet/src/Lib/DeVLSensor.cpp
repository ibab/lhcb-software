// Gaudi
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IUpdateManagerSvc.h"
// Det/DetDesc
#include "DetDesc/Condition.h"
// Kernel/LHCbKernel
#include "Kernel/VLChannelID.h"
// Local 
#include "VLDet/DeVLSensor.h"

/** @file DeVLSensor.cpp
 *
 *  Implementation of class : DeVLSensor
 *
 */

// ============================================================================
/// Constructor
// ============================================================================
DeVLSensor::DeVLSensor(const std::string& name) : DetectorElement(name) {

}

// ============================================================================
/// Object identification
// ============================================================================
const CLID& DeVLSensor::clID() const {

  return DeVLSensor::classID();
  
}

// ============================================================================
/// Initialisation method
// ============================================================================
StatusCode DeVLSensor::initialize() {

  // Set the output level.
  PropertyMgr* pmgr = new PropertyMgr();
  int outputLevel = 0;
  pmgr->declareProperty("OutputLevel", outputLevel);
  IJobOptionsSvc* jobSvc;
  ISvcLocator* svcLoc = Gaudi::svcLocator();
  StatusCode sc = svcLoc->service("JobOptionsSvc", jobSvc);
  if (sc.isSuccess()) sc = jobSvc->setMyProperties("DeVLSensor", pmgr);
  if (outputLevel > 0) {
    msgSvc()->setOutputLevel("DeVLSensor", outputLevel);
  }
  delete pmgr;
  if (!sc) return sc;
  m_debug   = (msgSvc()->outputLevel("DeVLSensor") == MSG::DEBUG);
  m_verbose = (msgSvc()->outputLevel("DeVLSensor") == MSG::VERBOSE);
  if (m_verbose) m_debug = true;
  MsgStream msg(msgSvc(), "DeVLSensor");

  /// Initialise the base class.
  sc = DetectorElement::initialize();
  if (!sc.isSuccess()) {
    msg << MSG::ERROR << "Failed to initialise DetectorElement" << endmsg;
    return sc;
  }
  /// Initialise the sensor from the XML.
  sc = initSensor();
  if (!sc.isSuccess()) {
    msg << MSG::ERROR << "Failed to initialise DeVLSensor." << endmsg;
    return sc;
  }
  m_geometry = geometry();
  sc = cacheGeometry();
  if (!sc) {
    msg << MSG::ERROR << "Failed to cache geometry." << endmsg;
    return sc;
  }
  // Get parent half box for pattern recognition alignment purposes.
  // Hierarchy should be sensor -> R/Phi Pair -> Module -> VeloLite(Left|Right)
  IDetectorElement* halfBox = 
    this->parentIDetectorElement()->parentIDetectorElement()->parentIDetectorElement();
  if (m_debug) {
    msg << MSG::DEBUG
        << "Great grandparent of " << this->name() << " is "
        << halfBox->name() << endmsg;
  }
  m_halfBoxGeom = halfBox->geometry();

  sc = registerConditionCallBacks();
  if (sc.isFailure()) {
    msg << MSG::ERROR
        << "Failure to register condition update call backs." << endmsg;
    return sc;
  }
  return StatusCode::SUCCESS;
  
}

//=============================================================================
/// Get the number of channels between two channels
//=============================================================================
StatusCode DeVLSensor::channelDistance(const LHCb::VLChannelID& start,
                                       const LHCb::VLChannelID& end,
                                       int& nOffset) const {
                                             
  nOffset = 0;
  const unsigned int startStrip = start.strip();
  const unsigned int endStrip = end.strip();
  if (start.sensor() != end.sensor()) {
    nOffset = m_numberOfStrips;
    return StatusCode::FAILURE;
  }
  if (numberOfStrips() < startStrip || 
      numberOfStrips() < endStrip) {
    return StatusCode::FAILURE;
  }
  // Check for zone crossing.
  const unsigned int startZone = zoneOfStrip(startStrip);
  const unsigned int endZone = zoneOfStrip(endStrip);
  if (startZone != endZone) {
    return StatusCode::FAILURE;
  }
  nOffset = endStrip - startStrip;
  return StatusCode::SUCCESS;

}

// ===========================================================================
/// Initialisation from XML
// ===========================================================================
StatusCode DeVLSensor::initSensor() {

  m_sensorNumber     = param<int>("SensorNumber");
  m_innerRadius      = param<double>("InnerRadius");
  m_boundingBoxX     = param<double>("BoundingBoxX");
  m_boundingBoxY     = param<double>("BoundingBoxY");
  m_siliconThickness = param<double>("SiThick");
  m_module = param<std::string>("Module");
  m_type   = param<std::string>("Type");
  m_isPileUp = m_type.find("Veto") == 0;
  m_isR      = m_type.find("R") == 0;
  m_isPhi    = m_type.find("Phi") == 0;
  if (m_isR || m_isPileUp) {
    m_outerRadius = param<double>("ROuterRadius");
  } else {
    m_outerRadius = param<double>("PhiOuterRadius");
  }
  m_isLeft       = 0 != param<int>("Left");
  m_isDownstream = 0 != param<int>("Downstream");
  m_fullType = m_type;
  if (m_isDownstream) {
    m_fullType += "DnStrm";
  } else {
    m_fullType += "UpStrm";
  }
  if (m_isLeft) {
    m_fullType += "Left";
  } else {
    m_fullType += "Right";
  }
  return StatusCode::SUCCESS;

}

// ============================================================================
/// Members related to condition caching
// ============================================================================
StatusCode DeVLSensor::registerConditionCallBacks() {

  // Geometry conditions (z position)
  updMgrSvc()->registerCondition(this, this->m_geometry, 
                                 &DeVLSensor::cacheGeometry);
  StatusCode sc = updMgrSvc()->update(this);
  if (!sc.isSuccess()) {
    MsgStream msg(msgSvc(), "DeVLSensor");
    msg << MSG::ERROR << "Failed to update sensor conditions." << endmsg;
    return sc;
  }
  return StatusCode::SUCCESS;
  
}

// ============================================================================
/// Cache geometry parameters
// ============================================================================
StatusCode DeVLSensor::cacheGeometry() {

  m_z = m_geometry->toGlobal(Gaudi::XYZPoint(0, 0, 0)).z();
  return StatusCode::SUCCESS;

}

