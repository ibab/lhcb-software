// $Id: DeVeloSensor.cpp,v 1.1 2004-01-29 01:02:18 mtobin Exp $
//==============================================================================
#define VELODET_DEVELOSENSOR_CPP 1
//==============================================================================
// Include files 
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
  MsgStream msg(msgSvc(), "DeVeloSensor");
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
  return StatusCode::SUCCESS;
}
//==============================================================================
/// Convert local phi to global phi (xml describes Left sensors)
//==============================================================================
double DeVeloSensor::localPhiToGlobal(double phiLocal)
{
  if(m_isLeft) return phiLocal;
  phiLocal += pi;
  return phiLocal;
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
//==============================================================================
/// Return the z position of the sensor
//==============================================================================
double DeVeloSensor::z()
{
  /*  HepPoint3D local(0,0,0);
  HepPoint3D global(0,0,0);
  global = m_geometry->toGlobal(local);
  m_z = global.z();*/
  return m_z;
}
//==============================================================================
/// Return true for X<0 side of the detector (-ve x is Right)
//==============================================================================
bool DeVeloSensor::isRight()
{
  return m_isRight;
}
//==============================================================================
/// Returns true if sensor is downstream
//==============================================================================
bool DeVeloSensor::isDownstream()
{
  return m_isDownstream;
}
//==============================================================================
/// Return true for Pile Up sensor
//==============================================================================
bool DeVeloSensor::isPileUp()
{
  return m_isPileUp;
}
//==============================================================================
/// Return true for Pile Up sensor
//==============================================================================
bool DeVeloSensor::isR()
{
  return m_isR;
}
//==============================================================================
/// Return true for Phi sensor
//==============================================================================
bool DeVeloSensor::isPhi()
{
  return m_isPhi;
}
//==============================================================================
/// Return +1 for X>0 side of detector [+ve x is Left/L]
//==============================================================================
int DeVeloSensor::xSide()
{
  return m_xSide;
}
//==============================================================================
/// Set the sensor attributes 
//==============================================================================
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
}
//==============================================================================
/// Return the sensor type
//==============================================================================
std::string DeVeloSensor::type()
{
  return m_type;
}
//==============================================================================
/// Set the sensor number
//==============================================================================
void DeVeloSensor::sensorNumber(unsigned int sensor)
{
  m_sensorNumber=sensor;
}
//==============================================================================
/// Return the sensor number
//==============================================================================
unsigned int DeVeloSensor::sensorNumber()
{
  return m_sensorNumber;
}
//==============================================================================
/// Set the (phi) sensors associated with this (R) sensor
//==============================================================================
void DeVeloSensor::associateSensor(unsigned int sensor)
{
  m_associated.push_back(sensor);
}
//==============================================================================
/// Return the (phi) sensors associated with this (R) sensor
//==============================================================================
std::vector<unsigned int> DeVeloSensor::associatedSensors()
{
  return m_associated;
}
//==============================================================================
/// The minimum radius for the sensitive area of the sensor
//==============================================================================
double DeVeloSensor::innerRadius()
{
  return m_innerRadius;
}
//==============================================================================
/// The maximum radius for the sensitive area of the sensor
//==============================================================================
double DeVeloSensor::outerRadius()
{
  return m_outerRadius;
}
//==============================================================================
/// The thickness of the sensor in mm
//==============================================================================
double DeVeloSensor::siliconThickness()
{
  return m_siliconThickness;
}
//==============================================================================
/// Returns the number of strips
//==============================================================================
unsigned int DeVeloSensor::numberOfStrips()
{
  return m_numberOfStrips;
}
