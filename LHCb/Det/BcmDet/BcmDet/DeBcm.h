#ifndef BCMDET_DEBCM_H
#define BCMDET_DEBCM_H 1

// Include files
#include <vector>
#include <string>

#include "DetDesc/DetectorElement.h"
#include "DetDesc/IGeometryInfo.h"
 
 
static const CLID& CLID_DeBcm = 14001;  

namespace DeBcmLocation{
  static const std::string& BcmUp="/dd/Structure/LHCb/BeforeMagnetRegion/BcmUp";
  static const std::string& BcmDown="/dd/Structure/LHCb/MagnetRegion/BcmDown";
  static const std::string& Default=BcmUp;
  
}

namespace DeBcmShifts{
    static const unsigned int shiftStationID         = 3;
    static const unsigned int shiftSensorID          = 0;
}

class DeBcm: public DetectorElement {
 
public:
 
  DeBcm();

  DeBcm(int nStation);

  ~DeBcm();

  inline static const CLID& classID(){
    return CLID_DeBcm;
  }

  inline virtual const CLID& clID() const {
    return classID();
  }

  virtual StatusCode initialize();

  inline int stationNumber() const {
    return m_StationNumber;
  }

  void setStationNumber(int nStation){
    m_StationNumber = nStation;
  }

  const int sensitiveVolumeID( const Gaudi::XYZPoint& point )const;
  
private:
  int m_StationNumber;

};

#endif
