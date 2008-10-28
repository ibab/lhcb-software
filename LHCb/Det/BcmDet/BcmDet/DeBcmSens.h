#ifndef BCMDET_DEBCMSENS_H
#define BCMDET_DEBCMSENS_H 1

// Include files
#include <vector>
#include <string>

#include "DetDesc/DetectorElement.h"
#include "DetDesc/IGeometryInfo.h"
 
 
static const CLID& CLID_DEBcmSens = 14000;  

class DeBcmSens: public DetectorElement {
 
public:
 
  DeBcmSens();

  DeBcmSens(int nSensor);

  ~DeBcmSens();

  inline static const CLID& classID(){
    return CLID_DEBcmSens;
  }

  inline virtual const CLID& clID() const {
    return classID();
  }

  virtual StatusCode initialize();

  inline int sensorNumber() const {
    return m_SensorNumber;
  }

  void setSensorNumber(int nSensor){
    m_SensorNumber = nSensor;
  }

  int sensitiveVolumeID( const Gaudi::XYZPoint& point )const;
  
private:
  int m_SensorNumber;

};

#endif
