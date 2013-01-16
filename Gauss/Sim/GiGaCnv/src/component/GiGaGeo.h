// $Id: GiGaGeo.h,v 1.5 2007-08-02 15:03:24 gcorti Exp $
#ifndef  GIGACNV_GiGaGeo_H
#define  GIGACNV_GiGaGeo_H 1

// Include files
// from STL
#include <string>
#include <vector>

// from GiGa
#include "GiGaCnv/GiGaCnvSvcBase.h"
#include "GiGaCnv/IGiGaGeomCnvSvc.h"

// Forward declarations
class G4VPhysicalVolume;
class G4VSolid;
class G4LogicalVolume;

class IDataSelector;
class SolidBoolean;

class IGiGaSensDet;
class IGiGaMagField;
class IGiGaFieldMgr;

class IGDMLReader;

template <class SERVICE>
class SvcFactory;

/** @class GiGaGeo GiGaGeo.h
 *
 *  Conversion service for transforming Gaudi detector
 *  and geometry description into Geant4 geometry and
 *  detector description
 *
 *  @author  Vanya Belyaev
 *  @author  Gonzalo Gracia
 *  @author  Sajan Easo, Gloria Corti
 *  @date    2000-08-07, Last modified: 2007-07-10
 */

class GiGaGeo : public           GiGaCnvSvcBase,
                public virtual  IGiGaGeomCnvSvc {

  friend class SvcFactory<GiGaGeo>;

public:

  typedef  std::vector<IGiGaSensDet*>  SDobjects;
  typedef  std::vector<IGiGaMagField*> MFobjects;
  typedef  std::vector<IGiGaFieldMgr*> FMobjects;

protected:

  /** standard constructor
   *  @param name  name of the service
   *  @param loc   pointer to service locator
   */
  GiGaGeo( const std::string& name,
           ISvcLocator* loc );

  virtual ~GiGaGeo(){}; ///< Destructor

public:

  /** standard initialization method
   *  @see GiGaCnvSvcBase
   *  @see  ConversionSvc
   *  @see IConversionSvc
   *  @see  Service
   *  @see IService
   *  @return status code
   */
  virtual StatusCode initialize();

  /** standard finalization method
   *  @see GiGaCnvSvcBase
   *  @see  ConversionSvc
   *  @see IConversionSvc
   *  @see  Service
   *  @see IService
   *  @return status code
   */
  virtual StatusCode finalize();

  /** Convert the transient object to the requested representation.
   *  e.g. conversion to persistent objects.
   *  @see GiGaCnvSvcBase
   *  @see  ConversionSvc
   *  @see IConversionSvc
   *  @see IConverter
   *  @param     object  Pointer to location of the object
   *  @param     address Reference to location of pointer with the
   *                     object address.
   *  @return    status code indicating success or failure
   */
  virtual StatusCode createRep( DataObject*      object,
                                IOpaqueAddress*& address );

  /** Resolve the references of the converted object.
   *  After the requested representation was created the references in this
   *  representation must be resolved.
   *  @see GiGaCnvSvcBase
   *  @see  ConversionSvc
   *  @see IConversionSvc
   *  @see IConverter
   *  @param     address object address.
   *  @param     object  pointer to location of the object
   *  @return    Status code indicating success or failure
   */
  virtual StatusCode fillRepRefs( IOpaqueAddress* address,
                                  DataObject*     object );

  /** Update the converted representation of a transient object.
   *  @see GiGaCnvSvcBase
   *  @see  ConversionSvc
   *  @see IConversionSvc
   *  @see IConverter
   *  @param     address object address.
   *  @param     object     Pointer to location of the object
   *  @return    Status code indicating success or failure
   */
  virtual StatusCode updateRep( IOpaqueAddress* address,
                                DataObject*     object );

  /** Update the references of an already converted object.
   *  The object must be retrieved before it can be updated.
   *  @see GiGaCnvSvcBase
   *  @see  ConversionSvc
   *  @see IConversionSvc
   *  @see IConverter
   *  @param     address object address.
   *  @param     object     Pointer to location of the object
   *  @return    Status code indicating success or failure
   */
  virtual StatusCode updateRepRefs( IOpaqueAddress* address,
                                    DataObject*     object );

  /** standard method for query the interface
   *  @return status code
   */
  virtual StatusCode queryInterface( const InterfaceID&,
                                     void**            );

  /** Retrieve the pointer to top-level "world" volume,
   *  @see IGiGaGeo
   *  needed for Geant4 - root for the whole Geant4 geometry tree
   *  @see class IGiGaGeoSrc
   *  @return pointer to constructed(converted) geometry tree
   */
  virtual G4VPhysicalVolume* world();

  /** Retrieve the pointer for G4 materials from G4MaterialTable,
   *  @see IGiGaGeo
   *  (could trigger the conversion of the (DetDesc) Material)
   *  @param  name    name/address/location of Material object
   *  @return pointer to converted G4Material object
   */
  virtual G4Material* material( const std::string& name );

  /** Retrive the pointer to converter volumes/assemblies
   *  @see IGiGaGeo
   *  (could trigger the conversion of the (DetDesc) LVolume/LAssembly)
   *  @param  name    name/address/location of Volume/Assembly object
   *  @return pointer to converted GiGaVolume  object
   */
  virtual GiGaVolume volume( const std::string& name );

  /** convert (DetDesc) Solid object into (Geant4) G4VSolid object
   *  @see IGiGaGeo
   *  @param  Solid pointer to Solid object
   *  @return pointer to converter G4VSolid object
   */
  virtual G4VSolid* solid( const ISolid* Solid );

  /** Instantiate the Sensitive Detector Object
   *  @see IGiGaGeo
   *  @param name  Type/Name of the Sensitive Detector Object
   *  @param det   reference to Densitive Detector Object
   *  @return  status code
   */
  virtual StatusCode sensitive( const std::string& name,
                                IGiGaSensDet*&     det );

  /** Instantiate the Field Manager Object
   *  @see IGiGaGeo
   *  @param name  Type/Name of the Field Manager Object
   *  @param mag   reference to Field Manager Object
   *  @return  status code
   */
  virtual StatusCode fieldMgr( const std::string& name,
                               IGiGaFieldMgr*&    mgr );

  /** Create new G4LogicalVolume. All arguments must be valid!
   *  One should not invoke the
   *  "new" operator for Logical Volumes directly
   *  @see IGiGaGeo
   *  @param solid    pointer to valid solid    object
   *  @param material pointer to valid material object
   *  @param name     name of logical volume
   *  @return pointer to new G4LogicalVolume  object
   */
  virtual G4LogicalVolume* createG4LV( G4VSolid*          solid,
                                       G4Material*        material,
                                       const std::string& name    );

  /** Retrieve the pointer to top-level "world" volume,
   *  needed for Geant4 - root for the whole Geant4 geometry tree
   *  @att obsolete method!
   *  @see class IGiGaGeoSrc
   *  @return pointer to constructed(converted) geometry tree
   */
  virtual G4VPhysicalVolume* G4WorldPV();

  /** Retrieve the pointer for G4 materials from G4MaterialTable,
   *  (could trigger the conversion of the (DetDesc) Material)
   *  @att obsolete method
   *  @param  Name    name/address/location of Material object
   *  @return pointer to converted G4Material object
   */
  virtual G4Material* g4Material( const std::string& Name );

  /** Retrive the pointer to G4LogicalVolume  from G4LogicalvolumeStore,
   * (could trigger the conversion of the (DetDesc) LVolume)
   *  @att obsolete method
   *  @param  Name    name/address/location of LVolume object
   *  @return pointer to converted G4LogicalVolume object
   */
  virtual G4LogicalVolume* g4LVolume( const std::string& Name );

  /** convert (DetDesc) Solid object into (Geant4) G4VSolid object
   *  @att obsolete method
   *  @param  Solid pointer to Solid object
   *  @return pointer to converter G4VSolid object
   */
  virtual G4VSolid* g4Solid( const ISolid* Solid );

  /** Instantiate the Sensitive Detector Object
   *  @att obsolete method
   *  @param Name  Type/Name of the Sensitive Detector Object
   *  @param Det   reference to Densitive Detector Object
   *  @return  status code
   */
  virtual StatusCode sensDet( const std::string& Name,
                              IGiGaSensDet*&     Det );

  /** Instantiate the Magnetic Field Object
   *  @att obsolete method
   *  @see IGiGaGeo
   *  @param name  Type/Name of the Magnetic Field Object
   *  @param mag   reference to Magnetic Field Object
   *  @return  status code
   */
  virtual StatusCode magnetic( const std::string& name,
                               IGiGaMagField*&    mag );

  /** Instantiate the Magnetic Field Object
   *  @att obsolete method
   *  @param Name  Type/Name of the Magnetic Field Object
   *  @param Mag   reference to Magnetic Field Object
   *  @return  status code
   */
  virtual StatusCode magField( const std::string& Name,
                               IGiGaMagField*&    Mag );

  /// Use or not information from detector elements geometry info
  /// (i.e. alignment)
  inline bool useAlignment() {
    return m_useAlignment;
  }

  /// Use or not information from detector elements for all volumes
  /// (i.e. alignment)
  inline bool alignAll() {
    return m_alignAll;
  }

  /// List of paths in TDS to which to apply misalignemt
  inline const std::vector<std::string>& alignDets() {
    return m_alignDets;
  }

protected:

  /** convert Gaudi/DetDesc boolean solid into Geant4 boolean solid
   *  @param  solid boolean solid to be converted
   *  @return pointer to converted solid
   */
  G4VSolid* g4BoolSolid( const SolidBoolean * solid );

private:

  GiGaGeo();
  GiGaGeo( const GiGaGeo& );
  GiGaGeo& operator=( const GiGaGeo& );

private:

  G4VPhysicalVolume* m_worldPV; ///< the world wolume

  std::string m_worldNamePV;    ///< identification of world volume - Phys Vol
  std::string m_worldNameLV;    ///< identification of world volume - Log Vol
  std::string m_worldMaterial;  ///< identification of world volume - material

  float m_worldX;       ///< world volume parameters (Box) - Size in X
  float m_worldY;       ///< world volume parameters (Box) - Size in Y
  float m_worldZ;       ///< world volume parameters (Box) - Size in Z

  std::string m_worldMagField;   ///< global magnetic field

  /// GDML readers to be called when creating world volume
  std::vector<std::string> m_gdmlReaderNames;
  std::vector<IGDMLReader*> m_gdmlReaders;

  /// special sensitive detector for estimation of material budget
  std::string m_budget;

  bool m_clearStores;            ///< flag for clearing all G4 geometry stores

  SDobjects m_SDs;               ///< list of all sensitive detector
  MFobjects m_MFs;               ///< list of all magnetic fields
  FMobjects m_FMs;               ///< list of all field managers

  /// Flag to switch on use of condDB info for children detector elements
  bool m_useAlignment;

  /// Flag to switch on for which detector to use condDB info for
  /// children detector elements
  bool m_alignAll;
  /// List of paths in TES to which to apply condDB info
  std::vector<std::string> m_alignDets;

};

#endif  //   GIGACNV_GiGaGeo_H








