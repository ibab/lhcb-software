
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
  StatusCode sc = DeSTSensor::initialize();
  if (sc.isFailure() ){
    MsgStream msg(msgSvc(), name() );
    msg << MSG::ERROR << "Failed to initialize detector element" << endmsg;
  }
  else {
    // get the parent
    m_parent = getParent<DeITSensor>();
    int id = param<int>("sensorID"); 
    setID(id);   
    cacheParentProperties(m_parent);
    sc = initGeometryInfo();  
    if (sc.isFailure()){
      MsgStream msg(msgSvc(), name() );
      msg<< MSG::ERROR << "failed to cache geometry" << endmsg;
      return sc;
    }
  }
  return StatusCode::SUCCESS;
}

