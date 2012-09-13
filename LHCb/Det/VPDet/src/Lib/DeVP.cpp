// $Id: DeVP.cpp,v 1.3 2009-11-26 17:04:44 cocov Exp $
//
// ============================================================================
#define  VPDET_DEVP_CPP 1
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
#include "VPDet/DeVP.h"

/** @file DeVP.cpp
 *
 *  Implementation of class :  DeVP
 *
 *  @author Victor Coco Victor.Coco@cern.ch
 */


// **  Standard Constructors

DeVP::DeVP( const std::string& name ) :  
  DetectorElement(name)
{
} 

//
// Standard Destructor
DeVP::~DeVP() {
}

// ============================================================================
// object identification
// ============================================================================ 
const CLID& DeVP::clID () const { return DeVP::classID() ; }


// ============================================================================
// intialization method
// ============================================================================ 
StatusCode DeVP::initialize() {

  // Trick from old DeVP to set the output level
  PropertyMgr* pmgr = new PropertyMgr();
  int outputLevel=0;
  pmgr->declareProperty("OutputLevel", outputLevel);
  IJobOptionsSvc* jobSvc;
  ISvcLocator* svcLoc = Gaudi::svcLocator();
  StatusCode sc = svcLoc->service("JobOptionsSvc", jobSvc);
  if( sc.isSuccess() ) sc = jobSvc->setMyProperties("DeVP", pmgr);
  if ( 0 < outputLevel ) {
    msgSvc()->setOutputLevel("DeVP", outputLevel);
  }
  delete pmgr;
  if( !sc ) return sc;

  MsgStream msg( msgSvc(), "DeVP" );
  msg << MSG::DEBUG << "Initialising DeVP " << endreq;

  // Initialise the detector element
  sc = DetectorElement::initialize();
  if( sc.isFailure() ) { 
    msg << MSG::ERROR << "Failure to initialize DetectorElement" << endreq;
    return sc ; 
  }
  m_debug   = (msgSvc()->outputLevel("DeVP") == MSG::DEBUG  ) ;
  m_verbose = (msgSvc()->outputLevel("DeVP") == MSG::VERBOSE) ;

  // get all of the pointers to the child detector elements
  std::vector<DeVPSensor*> veloSensors = findVPSensors();
  
  msg << MSG::DEBUG << "Found " << veloSensors.size() 
      << " sensors in the XML" << endreq;

  std::vector<DeVPSensor*>::iterator iDESensor;
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
    //Case where the pixel solution uses symetric pixel::  VP detector element is made of DeVPSquareType
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
    for (std::vector<DeVPSensor*>::const_iterator iSquareS=leftSensorsBegin();
         iSquareS != leftSensorsEnd();
         ++iSquareS) {
      // associated sensors on the left side
      DeVPSquareType*   lSquareS = const_cast<DeVPSquareType*>(squareSensor((*iSquareS)->sensorNumber()));
      // associated sensors on the right side
      DeVPSquareType* rSquareS = const_cast<DeVPSquareType*>(squareSensor((*iSquareS)->sensorNumber()+1));
      // other side sensor links
      if (rSquareS) rSquareS->setOtherSideSensor(lSquareS);
    } 
    msg << MSG::INFO << "There are " << m_nSensors << " Square sensors: Left " << m_nLeftSensors
        << " Right " << m_nRightSensors << endreq;
    // Get the value of the sensitiveVolumeCut from the XML.  ??????????if m_sensVolCut>z-zfront+siplanetichness
    m_sensVolCut=param<double>("sensitiveVolumeCut");
  }
  else{
    //Case where the pixel solution uses assymetric pixel:  VP detector element is made of DeVPXType and DeVPYType
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
        m_vpXSensors.push_back(dynamic_cast<DeVPXType*>((*iDESensor)));
        m_nXSensors++;
        if(isLeftSensor){
          m_vpLeftXSensors.push_back(m_vpXSensors.back());
          m_nLeftXSensors++;
        } else {
          m_vpRightXSensors.push_back(m_vpXSensors.back());
          m_nRightXSensors++;
        }

      } else if((*iDESensor)->isY()){
        m_vpYSensors.push_back(dynamic_cast<DeVPYType*>((*iDESensor)));
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
    for (std::vector<DeVPXType*>::const_iterator iXS=leftXSensorsBegin();
         iXS != leftXSensorsEnd();
         ++iXS) {
    
      // associated sensors on the left side
      DeVPXType*   lXS = *iXS;
      DeVPYType* lYS = const_cast<DeVPYType*>(ySensor(lXS->sensorNumber()+1));
      lXS->setAssociatedYSensor(lYS);
      if (lYS) lYS->setAssociatedXSensor(lXS);

      // associated sensors on the right side
      DeVPXType* rXS = const_cast<DeVPXType*>(xSensor(lXS->sensorNumber()+2));
      DeVPYType* rYS = const_cast<DeVPYType*>(ySensor(lYS->sensorNumber()+2));
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
const DeVPSensor* DeVP::sensor(const Gaudi::XYZPoint& point) const {
  return sensor(sensitiveVolumeID(point));
}

// return the sensitive volume if for a point in the global frame =======================================  OK  // if m_sensVolCut>z-zfront+siplanetichness
int DeVP::sensitiveVolumeID(const Gaudi::XYZPoint& point) const {
  
  MsgStream msg(msgSvc(), "DeVP");
  std::vector<DeVPSensor*>::const_iterator iDeVPSensor;
  for(iDeVPSensor=m_vpSensors.begin(); iDeVPSensor!=m_vpSensors.end(); ++iDeVPSensor){
    Gaudi::XYZPoint localPoint=(*iDeVPSensor)->globalToLocal(point);
    double z = localPoint.z();
    msg << MSG::DEBUG << "z = " << point.z() <<"local z ="<<localPoint.z()<<" in sensor: "<<(*iDeVPSensor)->sensorNumber()<< endmsg;
    if(m_sensVolCut > fabs(z)) {
      return ((*iDeVPSensor)->sensorNumber());
    }
  }
  msg << MSG::ERROR << "sensitiveVolumeID: no sensitive volume at z = " 
      << point.z() << endmsg;
  return -999;
}

//============================================================================= 
std::vector<DeVPSensor*> DeVP::findVPSensors()
{

  std::vector<DeVPSensor*> mySensors;
  
  scanDetectorElement(this, mySensors);
  return mySensors;
  
}
//=============================================================================  
void DeVP::scanDetectorElement(IDetectorElement* detElem, 
                                 std::vector<DeVPSensor*>& sensors)
{
  MsgStream msg( msgSvc(), "DeVP" );  
  std::vector<IDetectorElement*> veloSensors =
    detElem->childIDetectorElements();

  msg << MSG::DEBUG << "scanDetectorElement" << endreq;
  
  std::vector<IDetectorElement*>::iterator iVPSensors=veloSensors.begin();

  for (;iVPSensors!=veloSensors.end(); ++iVPSensors ) {
    msg << MSG::DEBUG << std::setw(12) << std::setiosflags(std::ios::left)
        << (*iVPSensors)->name() << endreq;
    DeVPSensor* pSensor = dynamic_cast<DeVPSensor*>((*iVPSensors));
    if (pSensor) {
      sensors.push_back(pSensor);
      msg << MSG::DEBUG << "Storing detector " <<   (*iVPSensors)->name()
          << endreq;
      
    }
    scanDetectorElement(*iVPSensors, sensors);
  }
}
//========================================================== 
StatusCode DeVP::updateLeftHalfBoxOffset() {

  Gaudi::XYZPoint localZero(0.,0.,0.);
 
  Gaudi::XYZPoint global = (*leftSensorsBegin())->vPHalfBoxToGlobal(localZero);
  m_halfBoxOffsets[LeftHalf] = global-localZero;
  
  return StatusCode::SUCCESS;
}

//========================================================== 
StatusCode DeVP::updateRightHalfBoxOffset() {

  Gaudi::XYZPoint localZero(0.,0.,0.);
  
  Gaudi::XYZPoint global = (*rightSensorsBegin())->vPHalfBoxToGlobal(localZero);
  m_halfBoxOffsets[RightHalf] = global-localZero;
  
  return StatusCode::SUCCESS;
}
