#ifndef GiGaDetElemCnv_h
#define GiGaDetElemCnv_h

#include "GiGa/GiGaCnv.h"

template <class T> class CnvFactory;

class DetectorElement;
class IGiGaConversionSvc;
class GiGaGeomCnvSvc;
class LVolume;
class PVolume;
class G4VPhysicalVolume;
class G4Box;
class G4Solid;
class G4LogicalVolume;
class G4Material;

class GiGaDetElemCnv : public GiGaCnv {
 friend class CnvFactory<GiGaDetElemCnv>;
public:
  // Constructor
  GiGaDetElemCnv(ISvcLocator*);
  // Initialize converter
  virtual StatusCode initialize();
  // Create G4 representation of a Data object
  virtual StatusCode createRep(DataObject* object ,IOpaqueAddress*& address);
  // Update G4 representation of a Data object
  virtual StatusCode updateRep(DataObject* object ,IOpaqueAddress*& address);
private: 
  GiGaGeomCnvSvc* m_GiGaGeomCnvSvc;
  // Create G4 representation of a given detector element.
  // A detector element is associated with a Log. Vol. which
  // is converted into a G4 Log. Vol. A G4 Physical volume placed
  // wrt the G4 geometry root is also created
  StatusCode createG4DetElemRoot( const DetectorElement* DE, G4LogicalVolume*& G4LV, bool& G4LVExist) ;
  // Create pair G4 LV,  G4 PV for a det. element
  StatusCode createRootPairLVPV(const DetectorElement* DE, G4LogicalVolume*& G4LV, bool& G4LVExist);
  // Create pair G4 LV,  G4 PV for a Gaudi PV
  StatusCode createPairLVPV(const IPVolume* PV, G4LogicalVolume* G4LV, const int copyN );
  // Check if a Log. Vol. was already converted to G4 representation
  G4LogicalVolume* alreadyConverted(const LVolume* LV, bool& G4LVExist) ;
  // create G4 Log Volume for a given log volume in the store
  StatusCode createG4LogVol(const LVolume* LV, G4LogicalVolume*& G4LV) ;
  // Create G4 Solid needed for a Log. Vol.
  StatusCode createG4Solid(const ISolid* Sd, std::string name, G4VSolid*& G4Sd) ;
  // Create G4 representation of a Physical Volume
  StatusCode createG4RootPhysVol(const DetectorElement* DE, G4LogicalVolume* G4LV, G4LogicalVolume* G4MotherLV);
  // Create G4 representation of a Physical Volume
  StatusCode createG4PhysVol(const IPVolume* PV, G4LogicalVolume* LV, G4LogicalVolume* G4MotherLV, const int copyN);
  // Create G4 geometry sub-tree for a detector element
  StatusCode convertSubVolumes(const DetectorElement* DE, G4LogicalVolume* G4MotherLV);
  // Create G4 geometry sub-tree for a Log. vol.
  StatusCode convertSubVolumes(const ILVolume* LV, G4LogicalVolume* G4MotherLV);
public:
  static const CLID& classID();
  static const unsigned char storageType();
};

#endif   //GiGaDetElemCnv_h
