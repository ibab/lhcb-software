
#include "STDet/DeITSensor.h"
#include "STDet/DeITSector.h"

/** @file DeITSensor.cpp
*
*  Implementation of class :  DeITSensor
*
*    @author Matthew Needham
*/


using namespace LHCb;

DeITSensor::DeITSensor( const std::string& name ) :
  DeSTSensor( name ),
  m_parent(0)
{ 
  // constructer
}

DeITSensor::~DeITSensor() {
  // destructer
}

const CLID& DeITSensor::clID () const
{
  return DeITSensor::classID() ;
}

StatusCode DeITSensor::initialize() {
  
  // initialize method
  MsgStream msg(msgSvc(), name() );

  StatusCode sc = DeSTSensor::initialize();
  if (sc.isFailure() ){
    msg << MSG::ERROR << "Failed to initialize detector element" << endreq;
  }
  else {
    // get the parent
    m_parent = getParent<DeITSensor>();
    int id = param<int>("sensorID"); 
    setID(id);   
    cacheParentProperties(m_parent);
    sc = initGeometryInfo();  
    if (sc.isFailure()){
      msg<< MSG::ERROR << "failed to cache geometry" << endreq;
      return sc;
    }
  }
  return StatusCode::SUCCESS;
}

