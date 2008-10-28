#include "BcmDet/DeBcmSens.h"

DeBcmSens::DeBcmSens() :
  m_SensorNumber(0)
{
}

DeBcmSens::DeBcmSens(int nSensor) :
  m_SensorNumber(nSensor)
{
}

DeBcmSens::~DeBcmSens()
{
}

StatusCode DeBcmSens::initialize()
{
  MsgStream msg( msgSvc(), name() );

  StatusCode sc = DetectorElement::initialize();
  if( sc.isFailure() ) {
    msg << MSG::ERROR << "Failure to initialize DetectorElement" << endreq;
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


