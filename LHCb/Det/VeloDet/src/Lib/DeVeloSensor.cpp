// $Id: DeVeloSensor.cpp,v 1.9 2005-07-07 16:09:14 mtobin Exp $
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

  initSensor();
  geometryCache();
  
  msg << MSG::DEBUG << "Sensor full type " << m_fullType << " z= " << m_z
      << " R " << this->isR() 
      << " Phi " << this->isPhi()  
      << " PU " << this->isPileUp()
      << " Left " << m_isLeft
      << " Right " << this->isRight() 
      << " Downstream " << this->isDownstream() << endreq;
  return StatusCode::SUCCESS;
}
//==============================================================================
/// Cache geometry parameters
//==============================================================================
void DeVeloSensor::geometryCache() 
{
  IGeometryInfo* geom = this->geometry();
  m_geometry = geom;
  m_z = m_geometry->toGlobal(HepPoint3D(0,0,0)).z();
}
//==============================================================================
/// Convert local position to global position
//==============================================================================
StatusCode DeVeloSensor::localToGlobal(const HepPoint3D& localPos, 
                                       HepPoint3D& globalPos) const
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
                         HepPoint3D& localPos) const
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

//=============================================================================
/// Returns the number of channels between two channels
//=============================================================================
StatusCode DeVeloSensor::channelDistance(const VeloChannelID& start,
                                         const VeloChannelID& end,
                                         int& nOffset) const
{
  nOffset = 0;
  unsigned int startStrip = start.strip();
  unsigned int endStrip = end.strip();

  if(this->numberOfStrips()<startStrip || this->numberOfStrips()<endStrip) {
    return StatusCode::FAILURE;
  }
  // put in some checks for boundaries etc...
  unsigned int startZone = zoneOfStrip(startStrip);
  unsigned int endZone = zoneOfStrip(endStrip);
  if(startZone != endZone) {
    return StatusCode::FAILURE;
  }
  nOffset = endStrip-startStrip;
  return StatusCode::SUCCESS;
}
//=============================================================================
void DeVeloSensor::initSensor()
{
  // Set the sensor type from name in XML
  m_numberOfStrips = 2048;
  m_innerRadius = this->param<double>("InnerRadius");
  m_outerRadius = this->param<double>("OuterRadius");
  m_siliconThickness = this->param<double>("SiThick");

  m_type     = this->param<std::string>("Type");
  m_isPileUp = m_type.find("Veto")==0;
  m_isR      = m_type.find("R")==0;
  m_isPhi    = m_type.find("Phi")==0;

  if ( m_type == "R" || m_type == "Phi" || m_type == "Veto" ) {
    m_isLeft = this->param<int>("Left");
    m_isDownstream=this->param<int>("Downstream");
    m_fullType = m_type + ( (m_isDownstream) ? "DnStrm" : "UpStrm" )
      + ( (m_isLeft) ? "Left" : "Right");
  } else {
    m_isLeft = (m_type == "PhiDL" || m_type == "PhiUL"
                || m_type == "RLeft" || m_type == "VetoL");
    m_isDownstream = (m_type == "PhiDL" || m_type == "PhiDR");
    m_fullType = m_type;
  }

  m_xSide = (m_isLeft) ? 1 : -1;

}
