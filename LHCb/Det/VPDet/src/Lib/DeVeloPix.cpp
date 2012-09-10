// $Id: DeVeloPix.cpp,v 1.3 2009-11-26 17:04:44 cocov Exp $
//
// ============================================================================
#define  VELOPIXDET_DEVELOPIX_CPP 1
// ============================================================================
// from STL (for std::sort)
#include <algorithm>

// From Gaudi
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IUpdateManagerSvc.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "DetDesc/Condition.h"

// Local
#include "VeloPixDet/DeVeloPix.h"

/** @file DeVeloPix.cpp
 *
 *  Implementation of class :  DeVeloPix
 *
 *  @author Victor Coco Victor.Coco@cern.ch
 */


// **  Standard Constructors

DeVeloPix::DeVeloPix( const std::string& name ) :  
  DetectorElement(name)
{
} 

//
// Standard Destructor
DeVeloPix::~DeVeloPix() {
}

// ============================================================================
// object identification
// ============================================================================ 
const CLID& DeVeloPix::clID () const { return DeVeloPix::classID() ; }


// ============================================================================
// intialization method
// ============================================================================ 
StatusCode DeVeloPix::initialize() {

  // Trick from old DeVeloPix to set the output level
  PropertyMgr* pmgr = new PropertyMgr();
  int outputLevel=0;
  pmgr->declareProperty("OutputLevel", outputLevel);
  IJobOptionsSvc* jobSvc;
  ISvcLocator* svcLoc = Gaudi::svcLocator();
  StatusCode sc = svcLoc->service("JobOptionsSvc", jobSvc);
  if( sc.isSuccess() ) sc = jobSvc->setMyProperties("DeVeloPix", pmgr);
  if ( 0 < outputLevel ) {
    msgSvc()->setOutputLevel("DeVeloPix", outputLevel);
  }
  delete pmgr;
  if( !sc ) return sc;

  MsgStream msg( msgSvc(), "DeVeloPix" );
  msg << MSG::DEBUG << "Initialising DeVeloPix " << endreq;

  // Initialise the detector element
  sc = DetectorElement::initialize();
  if( sc.isFailure() ) { 
    msg << MSG::ERROR << "Failure to initialize DetectorElement" << endreq;
    return sc ; 
  }
  m_debug   = (msgSvc()->outputLevel("DeVeloPix") == MSG::DEBUG  ) ;
  m_verbose = (msgSvc()->outputLevel("DeVeloPix") == MSG::VERBOSE) ;

  // get all of the pointers to the child detector elements
  std::vector<DeVeloPixSensor*> veloSensors = findVeloPixSensors();
  
  msg << MSG::DEBUG << "Found " << veloSensors.size() 
      << " sensors in the XML" << endreq;

  std::vector<DeVeloPixSensor*>::iterator iDESensor;
  m_nSensors=m_nXSensors=m_nYSensors=0;
  m_nLeftSensors=m_nRightSensors=0;

  std::sort(veloSensors.begin(), veloSensors.end(), less_Z());
 
  // this determines the size of our pseudo map and check type
  unsigned int maxSensorNumber=0;
  bool isSymetric = false;
  for(iDESensor = veloSensors.begin() ; iDESensor != veloSensors.end() ; 
      ++iDESensor){
     if (maxSensorNumber < (*iDESensor)->sensorNumber())
       maxSensorNumber = (*iDESensor)->sensorNumber();
     // Frome the sensor type describe in DDDB determine which type of detector we implement in intialisation
     if ((*iDESensor)->isSquare() ) isSymetric = true;
  }
  
  // ok, now we know the size of our pseudo map
  m_sensors.resize(maxSensorNumber+1,0);

  if(isSymetric == true ){ 
    //Case where the pixel solution uses symetric pixel::  VeloPix detector element is made of DeVeloPixSquareType
    m_nLeftSensors=m_nRightSensors=0;
    for(iDESensor = veloSensors.begin() ; iDESensor != veloSensors.end() ; 
        ++iDESensor,++m_nSensors){
      // Sensors are pre-sorted in XML such that they increase with z position
      m_vpSensors.push_back(*iDESensor);
      unsigned int index=m_vpSensors.size()-1;
      msg << MSG::DEBUG << "type " << (*iDESensor)->fullType() 
          << " index " << index
          << " Square type " << (*iDESensor)->isSquare() 
          << " SNO " <<  (*iDESensor)->sensorNumber()
          << endmsg;
    
      //      bool isLeftSensor=false;
      // Check if sensor is on Left/Right side of LHCb
      if((*iDESensor)->isLeft()){
        //        isLeftSensor=true;
        m_vpLeftSensors.push_back(m_vpSensors.back());
        m_nLeftSensors++;
      } else {
        m_vpRightSensors.push_back(m_vpSensors.back());
        m_nRightSensors++;
      }

      m_sensors[m_vpSensors[index]->sensorNumber()]= m_vpSensors[index];
      msg << MSG::DEBUG << "Module " << m_vpSensors[index]->module()
          << " sensor " << m_vpSensors[index]->sensorNumber()
          << " type " << m_vpSensors[index]->fullType() 
          << " z = " << m_vpSensors[index]->z()
          << " and in VELO frame " 
          << geometry()->toLocal(Gaudi::XYZPoint(0,0,m_vpSensors[index]->z())).z()
          << endreq;
    }
    // Set the associated and other side sensor links.  This makes assumptions about the
    // semantics of sensor number.  While this is a bad idea in general it is
    // defendable inside the detector element itself. 
    for (std::vector<DeVeloPixSensor*>::const_iterator iSquareS=leftSensorsBegin();
         iSquareS != leftSensorsEnd();
         ++iSquareS) {
      // associated sensors on the left side
      DeVeloPixSquareType*   lSquareS = const_cast<DeVeloPixSquareType*>(squareSensor((*iSquareS)->sensorNumber()));
      // associated sensors on the right side
      DeVeloPixSquareType* rSquareS = const_cast<DeVeloPixSquareType*>(squareSensor((*iSquareS)->sensorNumber()+1));
      // other side sensor links
      if (rSquareS) rSquareS->setOtherSideSensor(lSquareS);
    } 
    msg << MSG::INFO << "There are " << m_nSensors << " Square sensors: Left " << m_nLeftSensors
        << " Right " << m_nRightSensors << endreq;
    // Get the value of the sensitiveVolumeCut from the XML.  ??????????if m_sensVolCut>z-zfront+siplanetichness
    m_sensVolCut=param<double>("sensitiveVolumeCut");
  }
  else{
    //Case where the pixel solution uses assymetric pixel:  VeloPix detector element is made of DeVeloPixXType and DeVeloPixYType
    m_nLeftXSensors=m_nRightXSensors=0;
    m_nLeftYSensors=m_nRightYSensors=0;
    for(iDESensor = veloSensors.begin() ; iDESensor != veloSensors.end() ; 
        ++iDESensor,++m_nSensors){
      // Sensors are pre-sorted in XML such that they increase with z position
      m_vpSensors.push_back(*iDESensor);
      unsigned int index=m_vpSensors.size()-1;
      msg << MSG::DEBUG << "type " << (*iDESensor)->fullType() 
          << " index " << index
          << " X " << (*iDESensor)->isX() 
          << " Y " << (*iDESensor)->isY()
          << " SNO " <<  (*iDESensor)->sensorNumber()
          << endmsg;
    
      bool isLeftSensor=false;
      // Check if sensor is on Left/Right side of LHCb
      if((*iDESensor)->isLeft()){
        isLeftSensor=true;
        m_vpLeftSensors.push_back(m_vpSensors.back());
        m_nLeftSensors++;
      } else {
        m_vpRightSensors.push_back(m_vpSensors.back());
        m_nRightSensors++;
      }
      if((*iDESensor)->isX()){
        m_vpXSensors.push_back(dynamic_cast<DeVeloPixXType*>((*iDESensor)));
        m_nXSensors++;
        if(isLeftSensor){
          m_vpLeftXSensors.push_back(m_vpXSensors.back());
          m_nLeftXSensors++;
        } else {
          m_vpRightXSensors.push_back(m_vpXSensors.back());
          m_nRightXSensors++;
        }

      } else if((*iDESensor)->isY()){
        m_vpYSensors.push_back(dynamic_cast<DeVeloPixYType*>((*iDESensor)));
        m_nYSensors++;
        if(isLeftSensor){
          m_vpLeftYSensors.push_back(m_vpYSensors.back());
          m_nLeftYSensors++;
        } else {
          m_vpRightYSensors.push_back(m_vpYSensors.back());
          m_nRightYSensors++;
        }

      } else {
        msg << MSG::ERROR << "Sensor type is unknown" << endreq;
      }
      m_sensors[m_vpSensors[index]->sensorNumber()]= m_vpSensors[index];
      msg << MSG::DEBUG << "Module " << m_vpSensors[index]->module()
          << " sensor " << m_vpSensors[index]->sensorNumber()
          << " type " << m_vpSensors[index]->fullType() 
          << " z = " << m_vpSensors[index]->z()
          << " and in VELO frame " 
          << geometry()->toLocal(Gaudi::XYZPoint(0,0,m_vpSensors[index]->z())).z()
          << endreq;
    }

    // Set the associated and other side sensor links.  This makes assumptions about the
    // semantics of sensor number.  While this is a bad idea in general it is
    // defendable inside the detector element itself. 
    for (std::vector<DeVeloPixXType*>::const_iterator iXS=leftXSensorsBegin();
         iXS != leftXSensorsEnd();
         ++iXS) {
    
      // associated sensors on the left side
      DeVeloPixXType*   lXS = *iXS;
      DeVeloPixYType* lYS = const_cast<DeVeloPixYType*>(ySensor(lXS->sensorNumber()+1));
      lXS->setAssociatedYSensor(lYS);
      if (lYS) lYS->setAssociatedXSensor(lXS);

      // associated sensors on the right side
      DeVeloPixXType* rXS = const_cast<DeVeloPixXType*>(xSensor(lXS->sensorNumber()+2));
      DeVeloPixYType* rYS = const_cast<DeVeloPixYType*>(ySensor(lYS->sensorNumber()+2));
      if (rXS) rXS->setAssociatedYSensor(rYS);
      if (rYS) rYS->setAssociatedXSensor(rXS);

      // other side sensor links
      if (rXS) rXS->setOtherSideXSensor(lXS);
      if (rXS) rXS->setOtherSideYSensor(lYS);
      if (lXS) lXS->setOtherSideXSensor(rXS);
      if (lXS) lXS->setOtherSideYSensor(rYS);
      if (rYS) rYS->setOtherSideYSensor(lYS);
      if (rYS) rYS->setOtherSideXSensor(lXS);
      if (lYS) lYS->setOtherSideYSensor(rYS);
      if (lYS) lYS->setOtherSideXSensor(rXS);
      
    }
  
    msg << MSG::DEBUG << "There are " << m_nSensors << " sensors: Left " << m_nLeftSensors
        << " Right " << m_nRightSensors << endreq;
    msg << MSG::DEBUG << "There are " << m_nXSensors << " X sensors: Left " << m_nLeftXSensors
        << " Right " << m_nRightXSensors << endreq;
    msg << MSG::DEBUG << "There are " << m_nYSensors << " Y sensors: Left " << m_nLeftYSensors
        << " Right " << m_nRightYSensors << endreq;

    // Get the value of the sensitiveVolumeCut from the XML.  ??????????if m_sensVolCut>z-zfront+siplanetichness
    m_sensVolCut=param<double>("sensitiveVolumeCut");
  }
  
  return StatusCode::SUCCESS;
}


// return the sensor number for a point (global frame) ======================================= 
const DeVeloPixSensor* DeVeloPix::sensor(const Gaudi::XYZPoint& point) const {
  return sensor(sensitiveVolumeID(point));
}

// return the sensitive volume if for a point in the global frame =======================================  OK  // if m_sensVolCut>z-zfront+siplanetichness
int DeVeloPix::sensitiveVolumeID(const Gaudi::XYZPoint& point) const {
  
  MsgStream msg(msgSvc(), "DeVeloPix");
  std::vector<DeVeloPixSensor*>::const_iterator iDeVeloPixSensor;
  for(iDeVeloPixSensor=m_vpSensors.begin(); iDeVeloPixSensor!=m_vpSensors.end(); ++iDeVeloPixSensor){
    Gaudi::XYZPoint localPoint=(*iDeVeloPixSensor)->globalToLocal(point);
    double z = localPoint.z();
    msg << MSG::DEBUG << "z = " << point.z() <<"local z ="<<localPoint.z()<<" in sensor: "<<(*iDeVeloPixSensor)->sensorNumber()<< endmsg;
    if(m_sensVolCut > fabs(z)) {
      return ((*iDeVeloPixSensor)->sensorNumber());
    }
  }
  msg << MSG::ERROR << "sensitiveVolumeID: no sensitive volume at z = " 
      << point.z() << endmsg;
  return -999;
}

//============================================================================= 
std::vector<DeVeloPixSensor*> DeVeloPix::findVeloPixSensors()
{

  std::vector<DeVeloPixSensor*> mySensors;
  
  scanDetectorElement(this, mySensors);
  return mySensors;
  
}
//=============================================================================  
void DeVeloPix::scanDetectorElement(IDetectorElement* detElem, 
                                 std::vector<DeVeloPixSensor*>& sensors)
{
  MsgStream msg( msgSvc(), "DeVeloPix" );  
  std::vector<IDetectorElement*> veloSensors =
    detElem->childIDetectorElements();

  msg << MSG::DEBUG << "scanDetectorElement" << endreq;
  
  std::vector<IDetectorElement*>::iterator iVeloPixSensors=veloSensors.begin();

  for (;iVeloPixSensors!=veloSensors.end(); ++iVeloPixSensors ) {
    msg << MSG::DEBUG << std::setw(12) << std::setiosflags(std::ios::left)
        << (*iVeloPixSensors)->name() << endreq;
    DeVeloPixSensor* pSensor = dynamic_cast<DeVeloPixSensor*>((*iVeloPixSensors));
    if (pSensor) {
      sensors.push_back(pSensor);
      msg << MSG::DEBUG << "Storing detector " <<   (*iVeloPixSensors)->name()
          << endreq;
      
    }
    scanDetectorElement(*iVeloPixSensors, sensors);
  }
}
//========================================================== 
StatusCode DeVeloPix::updateLeftHalfBoxOffset() {

  Gaudi::XYZPoint localZero(0.,0.,0.);
 
  Gaudi::XYZPoint global = (*leftSensorsBegin())->veloPixHalfBoxToGlobal(localZero);
  m_halfBoxOffsets[LeftHalf] = global-localZero;
  
  return StatusCode::SUCCESS;
}

//========================================================== 
StatusCode DeVeloPix::updateRightHalfBoxOffset() {

  Gaudi::XYZPoint localZero(0.,0.,0.);
  
  Gaudi::XYZPoint global = (*rightSensorsBegin())->veloPixHalfBoxToGlobal(localZero);
  m_halfBoxOffsets[RightHalf] = global-localZero;
  
  return StatusCode::SUCCESS;
}
