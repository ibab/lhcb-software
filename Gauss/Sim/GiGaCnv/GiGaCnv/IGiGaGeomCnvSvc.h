// $Id: IGiGaGeomCnvSvc.h,v 1.6 2003-04-06 18:55:31 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.5  2002/05/04 20:39:35  ibelyaev
//  see $GIGACNVROOT/release.notes (4 May 2002)
//
// ============================================================================
#ifndef  GIGACNV_IGIGAGEOMCNVSVC_H 
#define  GIGACNV_IGIGAGEOMCNVSVC_H 1

/// GiGa 
#include "GiGa/IGiGaGeoSrc.h" 
/// GiGaCnv
#include "GiGaCnv/IGiGaCnvSvc.h" 
#include "GiGaCnv/IIDIGiGaGeomCnvSvc.h"
#include "GiGaCnv/GiGaVolume.h"

class ISolid;
class IMagneticFieldSvc;

class G4LogicalVolume;
class G4VSolid; 
class G4Material; 

class IGiGaSensDet  ;
class IGiGaMagField ;
class IGiGaFieldMgr ;

/** @class IGiGaGeomCnv IGiGaGeomCnvSvc.h GiGa/IGiGaGeomCnvSvc.h
 *
 *  definition of abstract and non-minimal interface 
 *  to Geant4 geometry conversion service 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 */

class IGiGaGeomCnvSvc: virtual public IGiGaCnvSvc ,
                       virtual public IGiGaGeoSrc 
{
 public:  
  
  /// Retrieve unique interface identifier  
  static const InterfaceID& interfaceID() { return IID_IGiGaGeomCnvSvc; }

  /** Retrieve the pointer for G4 materials from G4MaterialTable, 
   *  (could trigger the conversion of the (DetDesc) Material)
   *  @param  name    name/address/location of Material object 
   *  @return pointer to converted G4Material object 
   */
  virtual G4Material*  material 
  ( const std::string& name ) = 0 ;
  
  /** Retrive the pointer to converter volumes/assemblies 
   *  (could trigger the conversion of the (DetDesc) LVolume/LAssembly)    
   *  @param  name    name/address/location of Volume/Assembly object 
   *  @return converted GiGaVolume  object 
   */
  virtual GiGaVolume   volume          
  ( const std::string& name ) = 0 ;
  
  /** convert (DetDesc) Solid object into (Geant4) G4VSolid object 
   *  @param  Solid pointer to Solid object 
   *  @return pointer to converter G4VSolid object 
   */
  virtual G4VSolid*    solid  
  ( const ISolid*      Solid  ) = 0 ;
  
  /** Instantiate the Sensitive Detector Object 
   *  @param name  Type/Name of the Sensitive Detector Object
   *  @param det   reference to Densitive Detector Object 
   *  @return  status code 
   */
  virtual StatusCode   sensitive   
  ( const std::string& name   , 
    IGiGaSensDet*&     det    ) = 0 ;
  
  /** Instantiate the Field Manager 
   *  @param name  Type/Name of the Field Manager Object
   *  @param mgr   reference to Field Manager Object 
   *  @return  status code 
   */
  virtual StatusCode   fieldMgr 
  ( const std::string& name   , 
    IGiGaFieldMgr*&    mgr    ) = 0 ;
  
  /** Create new G4LogicalVolume. 
   *  All arguments must be valid!
   *  One should not invoke the 
   *  "new" operator for Logical Volumes directly
   *  @param solid    pointer to valid solid    object
   *  @param material pointer to valid material object
   *  @param name     name of logical volume 
   *  @return pointer to new G4LogicalVolume  object 
   */
  virtual G4LogicalVolume*    createG4LV 
  ( G4VSolid*          solid    , 
    G4Material*        material , 
    const std::string& name     ) = 0 ;
  
  /** Retrieve the pointer for G4 materials from G4MaterialTable, 
   *  (could trigger the conversion of the (DetDesc) Material)
   *  @obsolete method!
   *  @param  Name    name/address/location of Material object 
   *  @return pointer to converted G4Material object 
   */
  virtual G4Material*         g4Material 
  ( const std::string& Name ) = 0 ;
  
  /** Retrive the pointer to G4LogicalVolume 
   *  from G4LogicalvolumeStore,
   *  (could trigger the conversion of the (DetDesc) LVolume)    
   *  @att obsolete method! 
   *  @param  Name    name/address/location of LVolume object 
   *  @return pointer to converted G4LogicalVolume object 
   */
  virtual G4LogicalVolume*    g4LVolume  
  ( const std::string& Name ) = 0 ; 
  
  /** convert (DetDesc) Solid object into (Geant4) G4VSolid object 
   *  @att obsolete method!
   *  @param  Solid pointer to Solid object 
   *  @return pointer to converter G4VSolid object 
   */
  virtual G4VSolid*           g4Solid  
  ( const ISolid*      Solid  ) = 0 ; 
  
  /** Instantiate the Sensitive Detector Object 
   *  @att obsolete method 
   *  @param Name  Type/Name of the Sensitive Detector Object
   *  @param Det   reference to Densitive Detector Object 
   *  @return  status code 
   */
  virtual StatusCode          sensDet  
  ( const std::string& Name   , 
    IGiGaSensDet*&     Det    ) = 0 ;
  
  /** Instantiate the Magnetic Field Object 
   *  @obsolete method 
   *  @param name  Type/Name of the Magnetic Field Object
   *  @param mag   reference to Magnetic Field Object 
   *  @return  status code 
   */
  virtual StatusCode   magnetic 
  ( const std::string& name   , 
    IGiGaMagField*&    mag    ) = 0 ;
  
  /** Instantiate the Magnetic Field Object 
   *  @obsolete method 
   *  @param Name  Type/Name of the Magnetic Field Object
   *  @param Mag   reference to Magnetic Field Object 
   *  @return  status code 
   */
  virtual StatusCode          magField 
  ( const std::string& Name   , 
    IGiGaMagField*&    Mag    ) = 0 ;

  /// virtual destructor 
  virtual ~IGiGaGeomCnvSvc(){};
  ///

};

// ============================================================================
#endif    ///<  GIGACNV_IGIGAGEOMCNVSVC_H 
// ============================================================================


