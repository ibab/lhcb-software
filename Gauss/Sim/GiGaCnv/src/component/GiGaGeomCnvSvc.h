// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Sim/GiGaCnv/src/component/GiGaGeomCnvSvc.h,v 1.1.1.1 2001-04-23 08:34:15 ibelyaev Exp $ 
#ifndef      GIGA_GIGAGEOMCNVSVC_H 
#define      GIGA_GIGAGEOMCNVSVC_H 1 
/// STL 
#include <string> 
#include <vector> 
/// GiGa 
#include "GiGa/IGiGaGeomCnvSvc.h" 
/// GiGaCnv 
#include "GiGaCnv/GiGaCnvSvcBase.h" 
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


/** @class GiGaGeomCnvSvc GiGaGeomCnvSvc.h GiGa/GiGaGeomCnvSvc.h
    
    Convertersion service for convertiong Gaugi detector and geometry description 
    into Geant4 geometry and detectro description 
    
    @author  Vanya Belyaev
    @date    07/08/2000
*/

class GiGaGeomCnvSvc:  virtual public  IGiGaGeomCnvSvc , 
                       public   GiGaCnvSvcBase    
{ 
  ///
  friend class SvcFactory<GiGaGeomCnvSvc>;
  ///  
public:
  ///
  typedef  std::vector<IGiGaSensDet*>                     SDobjects; 
  typedef  std::vector<const IGiGaSensDetFactory*>        SDfactories; 
  typedef  std::vector<IGiGaMagField*>               MFobjects; 
  typedef  std::vector<const IGiGaMagFieldFactory*>  MFfactories; 
  ///
 protected: 
  /// constructor
  GiGaGeomCnvSvc( const std::string& , ISvcLocator* );
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

  /**
     Get Physical Volume World (top Logical volume in the G4 structure)
  */
  virtual G4VPhysicalVolume*  G4WorldPV() ;
  ///
  
  /** 
      Retrieve pointer for G4 materials from G4MaterialTable, 
      (could trigger the conversion of the (DetDesc) Material )
  */
  virtual G4Material*         g4Material ( const std::string& ) ;
  ///
  
  /** 
      Retrive pointer to G4LogicalVolume  from G4LogicalvolumeStore,
      ( could trigger the conversion of the (DetDesc) LVolume)    
  */
  virtual G4LogicalVolume*    g4LVolume  ( const std::string& )  ; 
  ///
  
  /** 
      Return  pointer to G4Vsolid, corresponding to (DetDesc) Solid,
      ( could trigger the conversion of the (DetDesc) LVolume)    
  */
  virtual G4VSolid*           g4Solid    ( const ISolid*      ) ; 
  ///
  
  /** 
      retrieve pointer to Sensitive Detector Object 
  */
  virtual StatusCode sensDet   ( const std::string& , IGiGaSensDet*& ) ;

  /** 
      retrieve pointer to Magnetic Field Object 
  */
  virtual StatusCode magField  ( const std::string& , IGiGaMagField*& ) ;

   
 protected:
  ///
  G4VSolid*     g4BoolSolid( const SolidBoolean * );
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
  std::string                      m_worldMagField ; /// global magnetic field 
  ////
  SDobjects                        m_SDs           ; /// created sensitive detectors 
  SDfactories                      m_SDFs          ; /// factories for created sensitive detectors 
  ///
  MFobjects                        m_MFs           ; /// created magnetic field objects 
  MFfactories                      m_MFFs          ; /// factories for created magnetic field objects 
  ///
};        


#endif  //   __GIGA_GEOMETRYCONVERSIONSERVICE_GIGAGEOMCNVSVC_H__ 








