// $Id: DeVeloSensor.cpp,v 1.5 2004-02-28 21:43:43 mtobin Exp $
//==============================================================================
#define VELODET_DEVELOSENSOR_CPP 1
//==============================================================================
// Include files 

// From Gaudi
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/MsgStream.h"

// From Velo 
#include "VeloDet/DeVeloSensor.h"

/** @file DeVeloSensor.cpp
 *
 *  Implementation of class : DeVeloSensor
 *
 *  @author Mark Tobin  Mark.Tobin@cern.ch
 */

//==============================================================================
/// Standard constructor
//==============================================================================
DeVeloSensor::DeVeloSensor(const std::string& name) : DetectorElement(name){
  //DeVeloSensor::DeVeloSensor(const std::string& name) : DeVelo(name){

}
//==============================================================================
/// Destructor
//==============================================================================
DeVeloSensor::~DeVeloSensor() {}; 
//==============================================================================
/// Object identification
//==============================================================================
const CLID& DeVeloSensor::clID() 
  const { return DeVeloSensor::classID(); }
//==============================================================================
/// Initialisation method
//==============================================================================
StatusCode DeVeloSensor::initialize() 
{
  // Trick from old DeVelo to set the output level
  PropertyMgr* pmgr = new PropertyMgr();
  int outputLevel=0;
  pmgr->declareProperty("OutputLevel", outputLevel);
  IJobOptionsSvc* jobSvc;
  ISvcLocator* svcLoc = Gaudi::svcLocator();
  StatusCode sc = svcLoc->service("JobOptionsSvc", jobSvc);
  jobSvc->setMyProperties("DeVeloSensor", pmgr);
  if ( 0 < outputLevel ) {
    msgSvc()->setOutputLevel("DeVeloSensor", outputLevel);
  }
  delete pmgr;
  MsgStream msg(msgSvc(), "DeVeloSensor");
  sc = DetectorElement::initialize();
  if(!sc.isSuccess()) {
    msg << MSG::ERROR << "Failed to initialise DetectorElement" << endreq;
    return sc;
  }
  m_numberOfStrips = 2048;
  m_innerRadius = this->userParameterAsDouble("InnerRadius");
  m_outerRadius = this->userParameterAsDouble("OuterRadius");
  m_siliconThickness = this->userParameterAsDouble("SiThick");
  m_type = this->userParameterAsString("Type");
  initSensor();
  IGeometryInfo* geom = this->geometry();
  m_geometry = geom;
  m_z = m_geometry->toGlobal(HepPoint3D(0,0,0)).z();
  msg << MSG::DEBUG << "Sensor type " << m_type << " z= " << m_z
      << " R " << m_isR << " Phi " << m_isPhi  << " PU " << m_isPileUp
      << " Left " << m_isLeft
      << " Right " << m_isRight << " Downstream " << m_isDownstream << endreq;
  return StatusCode::SUCCESS;
}
//==============================================================================
/// Convert local position to global position
//==============================================================================
StatusCode DeVeloSensor::localToGlobal(const HepPoint3D& localPos, 
                                       HepPoint3D& globalPos)
{
  MsgStream msg(msgSvc(), "DeVeloSensor");
  globalPos = m_geometry->toGlobal(localPos);
  msg << MSG::VERBOSE << "localToGlobal for sensor " << m_sensorNumber
      << " Local x " << localPos.x() << " y " << localPos.y() 
      << " z " << localPos.z()
      << " Global x " << globalPos.x() << " y " << globalPos.y()
      << " z " << globalPos.z()
      << endreq;
  return StatusCode::SUCCESS;
}
//==============================================================================
/// Convert global position to local position 
//==============================================================================
StatusCode DeVeloSensor::globalToLocal(const HepPoint3D& globalPos, 
                         HepPoint3D& localPos)
{
  MsgStream msg(msgSvc(), "DeVeloSensor");
  localPos = m_geometry->toLocal(globalPos);
  msg << MSG::VERBOSE << "GlobalToLocal for sensor " << m_sensorNumber
      << " Global x " << globalPos.x() << " y " << globalPos.y()
      << " z " << globalPos.z()
      << " Local x " << localPos.x() << " y " << localPos.y() 
      << " z " << localPos.z()
      << endreq;
  return StatusCode::SUCCESS;
}
void DeVeloSensor::initSensor()
{
  // Set the sensor type from name in XML
  m_isR=m_isPileUp=m_isPhi=false;
  if(m_type == "VetoL" || m_type == "VetoR"){
    m_isPileUp = true;
  } else if(m_type == "RRigh" || m_type == "RLeft") {
    m_isR = true;
  } else if(m_type == "PhiUR" || m_type == "PhiUL"
            || m_type == "PhiDR" || m_type == "PhiDL") {
    m_isPhi = true;
  }
  // Set the side of the sensor
  m_xSide = 0;
  m_isRight=m_isLeft=false;
  if (m_type == "PhiDR" || m_type == "PhiUR"
      || m_type == "RRigh" || m_type == "VetoR") {
    m_xSide = -1;
    m_isRight = true;
  } else   if (m_type == "PhiDL" || m_type == "PhiUL"
      || m_type == "RLeft" || m_type == "VetoL") {
    m_xSide = 1;
    m_isLeft = true;
  }
  // Set the stereo angle for phi sensors
  m_isDownstream=false;
  if(m_type == "PhiDL" || m_type == "PhiDR") m_isDownstream=true;
}
