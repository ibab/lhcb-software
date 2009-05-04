#include "BcmDet/DeBcm.h"

DeBcm::DeBcm() :
  DetectorElement(),
  m_StationNumber(0)
{
}

DeBcm::DeBcm(int nStation) :
  DetectorElement(),
  m_StationNumber(nStation)
{
}

DeBcm::~DeBcm()
{
}

StatusCode DeBcm::initialize()
{

  StatusCode sc = DetectorElement::initialize();
  if( sc.isFailure() ) {
    MsgStream msg( msgSvc(), name() );
    msg << MSG::ERROR << "Failure to initialize DetectorElement" << endmsg;
    return sc ;
  }

  int station = param<int>("stationId");
 
  setStationNumber(station);
  
  return sc;
}

int DeBcm::sensitiveVolumeID( const Gaudi::XYZPoint& point )const{
  const IDetectorElement* sensor = childDEWithPoint(point);
  if( sensor ){
    int nSensor = sensor->sensitiveVolumeID(point);
    if( nSensor == -1){
      return nSensor;
    }
    else{
      int identifier = (stationNumber()<<DeBcmShifts::shiftStationID)|(nSensor<<DeBcmShifts::shiftSensorID);
      return identifier;
    }
  }
  else{
    return -1;
  }
}

