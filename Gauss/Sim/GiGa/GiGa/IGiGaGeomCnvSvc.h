#ifndef           __GIGA_CONVERSION_IGIGAGEOMCNVSVC_H__ 
#define           __GIGA_CONVERSION_IGIGAGEOMCNVSVC_H__  1 



#include "GiGa/IGiGaCnvSvc.h" 

class ISolid;
class IMagneticFieldSvc;

class G4LogicalVolume;
class G4VPhysicalVolume;
class G4VSolid; 
class G4Material; 

class IGiGaSensDet;
class IGiGaMagField;


#include "GiGa/IIDIGiGaGeomCnvSvc.h" 


/** @class IGiGaGeomCnv     IGiGaGeomCnvSvc.h GiGa/IGiGaGeomCnvSvc.h

    definition of abstract interface to Geant 4 conversion service 

    @author Vanya Belyaev
*/


class IGiGaGeomCnvSvc: virtual public IGiGaCnvSvc
{
  ///
  ///  
 public: 
  
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IGiGaGeomCnvSvc; }
  
  /**
     Retrieve the pointer to artificial "World" volume, needed for Geant4
  */  
  virtual G4VPhysicalVolume*  G4WorldPV()                            = 0 ;
  
  /** 
      Retrieve pointer for G4 materials from G4MaterialTable, 
      (could trigger the conversion of the (DetDesc) Material )
  */
  virtual G4Material*         g4Material ( const std::string& Name ) = 0 ;
  
  /** 
      Retrive pointer to G4LogicalVolume  from G4LogicalvolumeStore,
      ( could trigger the conversion of the (DetDesc) LVolume)    
  */
  virtual G4LogicalVolume*    g4LVolume        ( const std::string& Name ) = 0 ; 

  /** 
      Return  pointer to G4Vsolid, corresponding to (DetDesc) Solid,
      ( could trigger the conversion of the (DetDesc) LVolume)    
  */
  virtual G4VSolid*           g4Solid          ( const ISolid*      Name ) = 0 ; 
  
  /** 
      retrieve pointer to Sensitive Detector Object 
  */
  virtual StatusCode          sensDet    ( const std::string& , IGiGaSensDet*& ) = 0 ;
  
  /** 
      retrieve pointer to Magnetic Field Object 
  */
  virtual StatusCode          magField   ( const std::string& , IGiGaMagField*& ) = 0 ;
  
  ///
  virtual ~IGiGaGeomCnvSvc(){};
  ///
};

///
///
///

#endif    //      __GIGA_CONVERSION_IGIGAGEOMCNVSVC_H__ 


