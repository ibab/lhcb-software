#ifndef      __GIGA_GEOMETRYCONVERSIONSERVICE_GIGAGEOMCNVSVC_H__ 
#define      __GIGA_GEOMETRYCONVERSIONSERVICE_GIGAGEOMCNVSVC_H__  1 

///
/// from STL 
#include <string> 

///
/// from GiGa 
#include "GiGa/GiGaCnvSvc.h" 
#include "GiGa/IGiGaGeomCnvSvc.h" 

///
/// forward declarations
///

class G4VPhysicalVolume; 
class G4Box;
class G4LogicalVolume; 
class IDataSelector; 

template <class SERVICE> class SvcFactory; 


///
///  GiGaGeomCnvSvc: conversion service  for converting of Gaudi 
///                  Geometry to Geant4 Geometry  
///  
///  Author: Vanya Belyaev 
///  Date    7 Aug 2000 


class GiGaGeomCnvSvc:  public GiGaCnvSvc , virtual public  IGiGaGeomCnvSvc
{ 
  
  friend class SvcFactory<GiGaGeomCnvSvc>;
  
 protected: 
  
  ///
  /// constructor
  /// 
  GiGaGeomCnvSvc( const std::string&  ServiceName          , 
		  ISvcLocator*        ServiceLocator       );
  
  /// 
  /// virtual destructor
  ///
  
  virtual ~GiGaGeomCnvSvc(){};
 
 public: 

  ///
  virtual StatusCode initialize() ;
  virtual StatusCode finalize  () ; 
  // Get Log. Volume World (top Logical volume in the G4 structure)
  virtual G4LogicalVolume* G4WorldLV()   {return m_G4WorldLV; };
  virtual G4VPhysicalVolume* G4WorldPV() {return m_G4WorldPV; };

  virtual StatusCode createReps(IDataSelector* pSelector) ;
  ///

 private:

  G4Box*                m_G4WorldSd;
  G4LogicalVolume*      m_G4WorldLV;
  G4VPhysicalVolume*    m_G4WorldPV;
  StatusCode createAir();
  StatusCode createG4WorldVol();
};        


#endif  //   __GIGA_GEOMETRYCONVERSIONSERVICE_GIGAGEOMCNVSVC_H__ 





