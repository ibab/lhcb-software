#include "BcmDet/DeBcmSens.h"

DeBcmSens::DeBcmSens() :
  DetectorElement(),
  m_SensorNumber(0)
{
}

DeBcmSens::DeBcmSens(int nSensor) :
  DetectorElement(),
  m_SensorNumber(nSensor)
{
}

DeBcmSens::~DeBcmSens()
{
}

StatusCode DeBcmSens::initialize()
{
  StatusCode sc = DetectorElement::initialize();
  if( sc.isFailure() ) {
    MsgStream msg( msgSvc(), name() );
    msg << MSG::ERROR << "Failure to initialize DetectorElement" << endmsg;
    return sc ;
  }

  int sensor = param<int>("sensId");
 
  setSensorNumber(sensor);
  
  return sc;
}

int DeBcmSens::sensitiveVolumeID( const Gaudi::XYZPoint& point )const{
  if( isInside( point ) ){
    return sensorNumber();
  }
  else{
    return -1;
  }
}


