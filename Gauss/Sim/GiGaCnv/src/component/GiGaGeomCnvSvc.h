// $Id: GiGaGeomCnvSvc.h,v 1.7 2002-05-07 12:24:50 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef  GIGACNV_GIGAGEOMCNVSVC_H 
#define  GIGACNV_GIGAGEOMCNVSVC_H 1 
// ============================================================================
/// STL 
#include <string> 
#include <vector> 
/// GiGa 
#include "GiGaCnv/GiGaCnvSvcBase.h" 
#include "GiGaCnv/IGiGaGeomCnvSvc.h" 
///
class G4VPhysicalVolume; 
class G4VSolid;
class G4LogicalVolume; 
///
class IDataSelector; 
class SolidBoolean;
///
class IGiGaSensDet;
///
class IGiGaMagField;
///
template <class SERVICE> 
class SvcFactory;

/** @class GiGaGeomCnvSvc GiGaGeomCnvSvc.h
 *  
 *  Convertersion service for convertiong Gaugi detector 
 *  and geometry description into Geant4 geometry and 
 *  detector description 
 *  
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date    07/08/2000
 */

class GiGaGeomCnvSvc:  
  public virtual  IGiGaGeomCnvSvc , 
  public           GiGaCnvSvcBase    
{ 
  ///
  friend class SvcFactory<GiGaGeomCnvSvc>;
  ///  
public:
  ///
  typedef  std::vector<IGiGaSensDet*>                SDobjects; 
  typedef  std::vector<IGiGaMagField*>               MFobjects; 
  ///
protected:
  
  /** standard constructor 
   *  @param name  name of the service 
   *  @param loc   pointer to service locator 
   */
  GiGaGeomCnvSvc( const std::string& name , 
                  ISvcLocator* loc );
  /// virtual destructor
  virtual ~GiGaGeomCnvSvc(){};
  
public: 
  
  /** standard initialization method 
   *  @see GiGaCnvSvcBase 
   *  @see  ConversionSvc 
   *  @see IConversionSvc 
   *  @see  Service  
   *  @see IService  
   *  @return status code 
   */
  virtual StatusCode initialize    ()                              ;
  
  /** standard finalization method 
   *  @see GiGaCnvSvcBase 
   *  @see  ConversionSvc 
   *  @see IConversionSvc 
   *  @see  Service  
   *  @see IService  
   *  @return status code 
   */
  virtual StatusCode finalize      ()                              ; 
  
  /** Convert the transient object to the requested representation.
   *  e.g. conversion to persistent objects.
   *  @param     object  Pointer to location of the object 
   *  @param     address Reference to location of pointer with the 
   *                     object address.
   *  @return    status code indicating success or failure
   */
  virtual StatusCode createRep     
  ( DataObject*      object ,
    IOpaqueAddress*& address ) ;
  
  /** standard method foe query the interface
   *  @return status code 
   */
  virtual StatusCode queryInterface 
  ( const InterfaceID& , 
    void**             ) ;
  
  /** Retrieve the pointer to top-level "world" volume,
   *  @see IGiGaGeomCnvSvc 
   *  needed for Geant4 - root for the whole Geant4 geometry tree 
   *  @see class IGiGaGeoSrc 
   *  @return pointer to constructed(converted) geometry tree 
   */  
  virtual G4VPhysicalVolume*  world    () ;
  
  /** Retrieve the pointer for G4 materials from G4MaterialTable, 
   *  @see IGiGaGeomCnvSvc 
   *  (could trigger the conversion of the (DetDesc) Material)
   *  @param  name    name/address/location of Material object 
   *  @return pointer to converted G4Material object 
   */
  virtual G4Material*  material 
  ( const std::string& name      )  ;
  
  /** Retrive the pointer to converter volumes/assemblies 
   *  @see IGiGaGeomCnvSvc 
   *  (could trigger the conversion of the (DetDesc) LVolume/LAssembly)    
   *  @param  name    name/address/location of Volume/Assembly object 
   *  @return pointer to converted GiGaVolume  object 
   */
  virtual GiGaVolume   volume          
  ( const std::string& name      )  ;
  
  /** convert (DetDesc) Solid object into (Geant4) G4VSolid object 
   *  @see IGiGaGeomCnvSvc 
   *  @param  Solid pointer to Solid object 
   *  @return pointer to converter G4VSolid object 
   */
  virtual G4VSolid*    solid  
  ( const ISolid*      Solid     )  ;
  
  /** Instantiate the Sensitive Detector Object 
   *  @see IGiGaGeomCnvSvc 
   *  @param name  Type/Name of the Sensitive Detector Object
   *  @param det   reference to Densitive Detector Object 
   *  @return  status code 
   */
  virtual StatusCode   sensitive   
  ( const std::string& name      , 
    IGiGaSensDet*&     det       )  ;
  
  /** Instantiate the Magnetic Field Object 
   *  @see IGiGaGeomCnvSvc 
   *  @param name  Type/Name of the Magnetic Field Object
   *  @param mag   reference to Magnetic Field Object 
   *  @return  status code 
   */
  virtual StatusCode   magnetic 
  ( const std::string& name      , 
    IGiGaMagField*&    mag       )  ;

  /** Create new G4LogicalVolume. All arguments must be valid!
   *  One should not invoke the 
   *  "new" operator for Logical Volumes directly
   *  @see IGiGaGeomCnvSvc 
   *  @param solid    pointer to valid solid    object
   *  @param material pointer to valid material object
   *  @param name     name of logical volume 
   *  @return pointer to new G4LogicalVolume  object 
   */
  virtual G4LogicalVolume*    createG4LV 
  ( G4VSolid*          solid     , 
    G4Material*        material  , 
    const std::string& name      )  ;
  
  /** Retrieve the pointer to top-level "world" volume,
   *  needed for Geant4 - root for the whole Geant4 geometry tree 
   *  @att obsolete method!
   *  @see class IGiGaGeoSrc 
   *  @return pointer to constructed(converted) geometry tree 
   */  
  virtual G4VPhysicalVolume*  G4WorldPV() ;

  /** Retrieve the pointer for G4 materials from G4MaterialTable, 
   *  (could trigger the conversion of the (DetDesc) Material)
   *  @att obsolete method 
   *  @param  Name    name/address/location of Material object 
   *  @return pointer to converted G4Material object 
   */
  virtual G4Material*         g4Material 
  ( const std::string& Name ) ;
  
  /** Retrive the pointer to G4LogicalVolume  from G4LogicalvolumeStore,
   * (could trigger the conversion of the (DetDesc) LVolume)    
   *  @att obsolete method 
   *  @param  Name    name/address/location of LVolume object 
   *  @return pointer to converted G4LogicalVolume object 
   */
  virtual G4LogicalVolume*    g4LVolume  
  ( const std::string& Name )  ; 
  
  /** convert (DetDesc) Solid object into (Geant4) G4VSolid object 
   *  @att obsolete method 
   *  @param  Solid pointer to Solid object 
   *  @return pointer to converter G4VSolid object 
   */
  virtual G4VSolid*           g4Solid    
  ( const ISolid*  Solid     ) ; 
  
  /** Instantiate the Sensitive Detector Object 
   *  @att obsolete method 
   *  @param Name  Type/Name of the Sensitive Detector Object
   *  @param Det   reference to Densitive Detector Object 
   *  @return  status code 
   */
  virtual StatusCode sensDet   
  ( const std::string& Name , 
    IGiGaSensDet*&     Det  ) ;
  
  /** Instantiate the Magnetic Field Object 
   *  @att obsolete method 
   *  @param Name  Type/Name of the Magnetic Field Object
   *  @param Mag   reference to Magnetic Field Object 
   *  @return  status code 
   */
  virtual StatusCode magField  
  ( const std::string& Name , 
    IGiGaMagField*&    Mag  ) ;

protected:
  ///
  G4VSolid* g4BoolSolid( const SolidBoolean * );
  ///
private:
  ///
  GiGaGeomCnvSvc()                                  ;
  GiGaGeomCnvSvc           ( const GiGaGeomCnvSvc& );
  GiGaGeomCnvSvc& operator=( const GiGaGeomCnvSvc& );
  ///
private:
  
  // the world wolume 
  G4VPhysicalVolume*               m_worldPV       ;
  // identification of world volume 
  std::string                      m_worldNamePV   ; 
  std::string                      m_worldNameLV   ; 
  std::string                      m_worldMaterial ; 
  // world volume parameters 
  float                            m_worldX        ;
  float                            m_worldY        ;
  float                            m_worldZ        ;
  // global magnetic field 
  std::string                      m_worldMagField ; ///< global mag field  
  // special sensitive detector for estimation of material budget 
  std::string                      m_budget        ;
  // list of all sensitive detector
  SDobjects                        m_SDs  ; ///< created sensitive detectors 
  // list of all magnetic fields
  MFobjects                        m_MFs  ; ///< created magnetic field objects 

};        


// ============================================================================
// end 
// ============================================================================
#endif  //   GIGACNV_GIGAGEOMCNVSVC_H__ 
// ============================================================================








