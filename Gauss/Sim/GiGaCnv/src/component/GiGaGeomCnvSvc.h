// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.3  2001/07/24 11:13:55  ibelyaev
/// package restructurization(III) and update for newer GiGa
///
/// Revision 1.2  2001/07/15 20:45:10  ibelyaev
/// the package restructurisation
/// 
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
class IGiGaSensDetFactory;
///
class IGiGaMagField;
class IGiGaMagFieldFactory;
///
template <class SERVICE> 
class SvcFactory;

/** @class GiGaGeomCnvSvc GiGaGeomCnvSvc.h
 *  
 *  Convertersion service for convertiong Gaugi detector 
 *  and geometry description into Geant4 geometry and 
 *  detector description 
 *  
 *  @author  Vanya Belyaev
 *  @date    07/08/2000
 */

class GiGaGeomCnvSvc:  virtual public  IGiGaGeomCnvSvc , 
                       public   GiGaCnvSvcBase    
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
  
  /** constructor 
   *  @param name  name of the service 
   *  @param loc   pointer to service locator 
   */
  GiGaGeomCnvSvc( const std::string& name , 
                  ISvcLocator* loc );
  /// virtual destructor
  virtual ~GiGaGeomCnvSvc(){};
  
public: 

  /// from ISConversionSvc:
  virtual StatusCode initialize    ()                              ;
  ///
  virtual StatusCode finalize      ()                              ; 
  ///
  virtual StatusCode createReps    ( IDataSelector* pSelector    ) ;
  ///
  virtual StatusCode queryInterface( const InterfaceID& , void** ) ;
  ///
  
  /** Retrieve the pointer to top-level "world" volume,
   *  needed for Geant4 - root for the whole Geant4 geometry tree 
   *  @return pointer to constructed(converted) geometry tree 
   */  
  virtual G4VPhysicalVolume*  G4WorldPV() ;
  
  /** Retrieve the pointer for G4 materials from G4MaterialTable, 
   *  (could trigger the conversion of the (DetDesc) Material)
   *  @param  Name    name/address/location of Material object 
   *  @return pointer to converted G4Material object 
   */
  virtual G4Material*         g4Material ( const std::string& ) ;
  
  /** Retrive the pointer to G4LogicalVolume  from G4LogicalvolumeStore,
   * (could trigger the conversion of the (DetDesc) LVolume)    
   *  @param  Name    name/address/location of LVolume object 
   *  @return pointer to converted G4LogicalVolume object 
   */
  virtual G4LogicalVolume*    g4LVolume  ( const std::string& )  ; 
  
  /** convert (DetDesc) Solid object into (Geant4) G4VSolid object 
   *  @param  Solid pointer to Solid object 
   *  @return pointer to converter G4VSolid object 
   */
  virtual G4VSolid*           g4Solid    ( const ISolid*      ) ; 
  
  /** Instantiate the Sensitive Detector Object 
   *  @param Name  Type/Name of the Sensitive Detector Object
   *  @param Det   reference to Densitive Detector Object 
   *  @return  status code 
   */
  virtual StatusCode sensDet   ( const std::string& , IGiGaSensDet*& ) ;
  
  /** Instantiate the Magnetic Field Object 
   *  @param Name  Type/Name of the Magnetic Field Object
   *  @param Mag   reference to Magnetic Field Object 
   *  @return  status code 
   */
  virtual StatusCode magField  ( const std::string& , IGiGaMagField*& ) ;
   
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
  ///
  G4VPhysicalVolume*               m_worldPV       ;
  ///
  std::string                      m_worldNamePV   ; 
  std::string                      m_worldNameLV   ; 
  std::string                      m_worldMaterial ; 
  ///
  float                            m_worldX        ;
  float                            m_worldY        ;
  float                            m_worldZ        ;
  ///
  std::string  m_worldMagField ; ///< global magnetic field 
  ////
  SDobjects    m_SDs  ; ///< created sensitive detectors 
  ///
  MFobjects    m_MFs  ; ///< created magnetic field objects 
  ///
};        


#endif  //   __GIGA_GEOMETRYCONVERSIONSERVICE_GIGAGEOMCNVSVC_H__ 








