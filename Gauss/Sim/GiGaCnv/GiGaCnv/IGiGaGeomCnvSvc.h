/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.1  2001/07/24 11:13:54  ibelyaev
/// package restructurization(III) and update for newer GiGa
/// 
/// ===========================================================================
#ifndef  GIGACNV_IGIGAGEOMCNVSVC_H 
#define  GIGACNV_IGIGAGEOMCNVSVC_H 1

/// GiGa 
#include "GiGa/IGiGaGeoSrc.h" 
/// GiGaCnv
#include "GiGaCnv/IGiGaCnvSvc.h" 
#include "GiGaCnv/IIDIGiGaGeomCnvSvc.h"

class ISolid;
class IMagneticFieldSvc;

class G4LogicalVolume;
class G4VSolid; 
class G4Material; 

class IGiGaSensDet;
class IGiGaMagField;


/** @class IGiGaGeomCnv     IGiGaGeomCnvSvc.h GiGa/IGiGaGeomCnvSvc.h
 *
 *  definition of abstract and non-minimal interface 
 *     to Geant4 geometry conversion service 
 *
 *  @author Vanya Belyaev
 */

class IGiGaGeomCnvSvc: virtual public IGiGaCnvSvc ,
                       virtual public IGiGaGeoSrc 
{
 public:  
  
  /// Retrieve unique interface identifier  
  static const InterfaceID& interfaceID() { return IID_IGiGaGeomCnvSvc; }
  
  /** Retrieve the pointer for G4 materials from G4MaterialTable, 
   *  (could trigger the conversion of the (DetDesc) Material)
   *  @param  Name    name/address/location of Material object 
   *  @return pointer to converted G4Material object 
   */
  virtual G4Material*         g4Material ( const std::string& Name ) = 0 ;
  
  /** Retrive the pointer to G4LogicalVolume  from G4LogicalvolumeStore,
   * (could trigger the conversion of the (DetDesc) LVolume)    
   *  @param  Name    name/address/location of LVolume object 
   *  @return pointer to converted G4LogicalVolume object 
   */
  virtual G4LogicalVolume*    g4LVolume  ( const std::string& Name ) = 0 ; 
  
  /** convert (DetDesc) Solid object into (Geant4) G4VSolid object 
   *  @param  Solid pointer to Solid object 
   *  @return pointer to converter G4VSolid object 
   */
  virtual G4VSolid*           g4Solid  ( const ISolid*      Solid  ) = 0 ; 
  
  /** Instantiate the Sensitive Detector Object 
   *  @param Name  Type/Name of the Sensitive Detector Object
   *  @param Det   reference to Densitive Detector Object 
   *  @return  status code 
   */
  virtual StatusCode          sensDet  ( const std::string& Name   , 
                                         IGiGaSensDet*&     Det    ) = 0 ;
  
  /** Instantiate the Magnetic Field Object 
   *  @param Name  Type/Name of the Magnetic Field Object
   *  @param Mag   reference to Magnetic Field Object 
   *  @return  status code 
   */
  virtual StatusCode          magField ( const std::string& Name   , 
                                         IGiGaMagField*&    Mag    ) = 0 ;
  
  /// virtual destructor 
  virtual ~IGiGaGeomCnvSvc(){};
  ///

};

/// ===========================================================================
#endif    ///<  GIGACNV_IGIGAGEOMCNVSVC_H 
/// ===========================================================================


