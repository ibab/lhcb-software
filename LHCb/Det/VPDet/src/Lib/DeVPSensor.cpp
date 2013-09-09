// Gaudi
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IUpdateManagerSvc.h"

// LHCb
// Det/DetDesc
#include "DetDesc/Condition.h"
// Kernel/LHCbKernel
#include "Kernel/VPChannelID.h"

// Local 
#include "VPDet/DeVPSensor.h"

/** @file DeVPSensor.cpp
 *
 *  Implementation of class DeVPSensor
 *
 *  @author Victor Coco victor.coco@cern.ch
 *  
 */

//==============================================================================
/// Standard constructor
//==============================================================================
DeVPSensor::DeVPSensor(const std::string& name) : 
    DetectorElement(name),
    m_geometry(NULL) {
 
}

//==============================================================================
/// Destructor
//==============================================================================
DeVPSensor::~DeVPSensor() {}

//==============================================================================
/// Object identification
//==============================================================================
const CLID& DeVPSensor::clID() const { 
  return DeVPSensor::classID(); 
}

//==============================================================================
/// Initialisation method
//==============================================================================
StatusCode DeVPSensor::initialize() {

  // Set the output level.
  PropertyMgr* pmgr = new PropertyMgr();
  int outputLevel = 0;
  pmgr->declareProperty("OutputLevel", outputLevel);
  IJobOptionsSvc* jobSvc;
  ISvcLocator* svcLoc = Gaudi::svcLocator();
  StatusCode sc = svcLoc->service("JobOptionsSvc", jobSvc);
  if (sc.isSuccess()) sc = jobSvc->setMyProperties("DeVPSensor", pmgr);
  if (outputLevel > 0) {
    msgSvc()->setOutputLevel("DeVPSensor", outputLevel);
  }
  delete pmgr;
  if (!sc) return sc;
  m_debug = (msgSvc()->outputLevel("DeVPSensor") == MSG::DEBUG);
  m_verbose = (msgSvc()->outputLevel("DeVPSensor") == MSG::VERBOSE);
  if (m_verbose) m_debug = true;

  MsgStream msg(msgSvc(), "DeVPSensor");
  sc = DetectorElement::initialize();
  if (!sc.isSuccess()) {
    msg << MSG::ERROR << "Cannot initialise DetectorElement" << endmsg;
    return sc;
  }

  // Get the information we need from the DDDB.
  m_sensorNumber = param<int>("SensorNumber");
  m_module = param<int>("Module");
  std::string side = param<std::string>("Side");
  m_isLeft = side.find("Left") == 0;
  m_isDownstream = 0 != param<int>("Downstream");

  m_geometry = geometry();
  sc = cacheGeometry();
  if (!sc) {    
    msg << MSG::ERROR << "Cannot cache geometry" << endmsg; 
    return sc;
  }
  return StatusCode::SUCCESS;

}

//==============================================================================
/// Cache geometry parameters
//==============================================================================
StatusCode DeVPSensor::cacheGeometry() {

  m_z = m_geometry->toGlobal(Gaudi::XYZPoint(0, 0, 0)).z();
  return StatusCode::SUCCESS;

}
