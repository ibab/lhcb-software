#ifndef           __GIGA_CONVERSION_IGiGaGeomCnvSvc_H__ 
#define           __GIGA_CONVERSION_IGiGaGeomCnvSvc_H__  1 



#include "GaudiKernel/IConversionSvc.h" 

class G4LogicalVolume;
class G4VPhysicalVolume;

extern const IID& IID_IGiGaGeomCnvSvc; 

class IGiGaGeomCnvSvc: virtual public IConversionSvc
{
  
 public: 
  
  virtual G4LogicalVolume* G4WorldLV() = 0;
  virtual G4VPhysicalVolume* G4WorldPV() = 0;
};


#endif     //     __GIGA_CONVERSION_IGiGaGeomCnvSvc_H__ 
