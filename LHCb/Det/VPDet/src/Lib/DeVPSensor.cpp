// $Id: DeVPSensor.cpp,v 1.3 2009-10-21 11:19:28 cocov Exp $
//==============================================================================
#define VPDET_DEVPSENSOR_CPP 1
//==============================================================================
// Include files 

// From Gaudi
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IUpdateManagerSvc.h"


#include "DetDesc/Condition.h"

// From VP 
#include "VPDet/DeVPSensor.h"
#include "Kernel/VPChannelID.h"

/** @file DeVPSensor.cpp
 *
 *  Implementation of class : DeVPSensor
 *
 *  @author Victor Coco victor.coco@cern.ch
 *  
 */

//==============================================================================
/// Standard constructor
//==============================================================================
DeVPSensor::DeVPSensor(const std::string& name) : 
  DetectorElement(name)
{
  ;
}
//==============================================================================
/// Destructor
//==============================================================================
DeVPSensor::~DeVPSensor() {}
//==============================================================================
/// Object identification
//==============================================================================
const CLID& DeVPSensor::clID() 
  const { return DeVPSensor::classID(); }
//==============================================================================
/// Initialisation method
//==============================================================================
StatusCode DeVPSensor::initialize() 
{
  // Trick from old DeVP to set the output level
  PropertyMgr* pmgr = new PropertyMgr();
  int outputLevel=0;
  pmgr->declareProperty("OutputLevel", outputLevel);
  IJobOptionsSvc* jobSvc;
  ISvcLocator* svcLoc = Gaudi::svcLocator();
  StatusCode sc = svcLoc->service("JobOptionsSvc", jobSvc);
  if( sc.isSuccess() ) sc = jobSvc->setMyProperties("DeVPSensor", pmgr);
  if ( 0 < outputLevel ) {
    msgSvc()->setOutputLevel("DeVPSensor", outputLevel);
  }
  delete pmgr;
  if( !sc ) return sc;
  MsgStream msg(msgSvc(), "DeVPSensor");
  sc = DetectorElement::initialize();
  if(!sc.isSuccess()) {
    msg << MSG::ERROR << "Failed to initialise DetectorElement" << endreq;
    return sc;
  }
  m_debug   = (msgSvc()->outputLevel("DeVPSensor") == MSG::DEBUG  ) ;
  m_verbose = (msgSvc()->outputLevel("DeVPSensor") == MSG::VERBOSE) ;
  if( m_verbose ){
    m_debug = true;
  }
  // initSenesor is the real initialisation from xml
  initSensor();
  IGeometryInfo* geom = geometry();
  m_geometry = geom;
  sc = cacheGeometry();
  if(!sc){    
    msg << MSG::ERROR <<"Failed to cache geometry"<<endmsg; 
    return sc;
  }
  // get parent VP Half box for pattern recognition alignment purposes
  // heirarchy should be sensor -> Module -> VP(Left|Right)
  IDetectorElement* halfBox = 
    this->parentIDetectorElement()->parentIDetectorElement();
  if(m_debug)
    msg << MSG::DEBUG 
        << "Great grandparent of " << this->name() << " is " 
        << halfBox->name() <<endreq;
  m_halfBoxGeom = halfBox->geometry();
  
  if(m_debug)
    msg << MSG::DEBUG
        << "Module " << m_module << " Sensor " << m_sensorNumber 
        << " full type " << m_fullType << " z= " << m_z
        << " X " << isX() 
        << " Y " << isY()
        << " Square " << isSquare()
        << " Left " << isLeft()
        << " Right " << isRight() 
        << " Downstream " << isDownstream() << endreq;

  return StatusCode::SUCCESS;
}
//==============================================================================
/// Cache geometry parameters
//==============================================================================
StatusCode DeVPSensor::cacheGeometry() 
{
  m_z = m_geometry->toGlobal(Gaudi::XYZPoint(0,0,0)).z();
  return StatusCode::SUCCESS;
}

//=============================================================================
void DeVPSensor::initSensor()
{
  // Get all the information we need from the DDDB
  m_module   = param<int>("Module");
  m_type     = param<std::string>("Type");
  m_side     = param<std::string>("Side");
  m_sensorNumber= param<int>("SensorNumber");

  m_isX     = m_type.find("X"    ) ==0 ;
  m_isY     = m_type.find("Y"    ) ==0 ;
  m_isSquare  = m_type.find("TIMEPIX") ==0 ;
  m_isLeft  = m_side.find("Left" ) ==0 ;
  m_isRight = m_side.find("Right") ==0 ;
  m_isDownstream = 0 != param<int>("Downstream");
  m_fullType = m_type + m_side + ( (m_isDownstream) ? "DnStrm" : "UpStrm" );
  m_ladderNumber  = param<int>("LadderNumber");
  m_siliconThickness = param<double>("SiThick");
  m_chipWidth = param<double>("ActiveWidth");
  m_chipLength = param<double>("ChipLength");
  m_interChipDist = param<double>("InterChipDist");
  m_hpSize = param<double>("HighPrecisionSize");
  m_lpSize = param<double>("LowPrecisionSize");
  m_nPixCol = param<int>("NPixelColumn");
  m_nPixRow = param<int>("NPixelRow");
  m_interchipPixSize = param<double>("InterChipPixelSize");

}
