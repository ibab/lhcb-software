

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/IAddressCreator.h" 
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IService.h"

#include "GaudiKernel/DetectorElement.h"
#include "GaudiKernel/IGeometryInfo.h"
#include "GaudiKernel/ILVolume.h"
#include "GaudiKernel/ISolid.h"
#include "DetDesc/Solids.h"
#include "DetDesc/LVolume.h"
#include "DetDesc/PVolume.h"

// G4 include files
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PVPlacement.hh"
#include "G4Material.hh"
#include "G4MaterialTable.hh"
#include "G4Element.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Trd.hh"
#include "G4Sphere.hh"
#include "G4Cons.hh"
#include "G4UnionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4IntersectionSolid.hh"

#include "GiGa/GiGaDetElemCnv.h"
#include "GiGa/GiGaGeomCnvSvc.h"

static CnvFactory<GiGaDetElemCnv> s_factory;
extern const ICnvFactory& GiGaDetElemCnvFactory = s_factory;

extern const CLID& CLID_DetectorElement;
extern unsigned char GiGaGeom_StorageType;
extern const IID& IID_IGiGaGeomCnvSvc;

GiGaDetElemCnv::GiGaDetElemCnv( ISvcLocator* aSvcLoc ):GiGaCnv(GiGaGeom_StorageType, CLID_DetectorElement, aSvcLoc)
{
  setNameOfGiGaConversionService( "GiGaGeomCnvSvc" ); 
  setConverterName              ( "GiGaDetElemCnv" );
}


StatusCode GiGaDetElemCnv::initialize()
{

  MsgStream log( msgSvc(), "GiGaDetElemCnv" ); 
  StatusCode sc = GiGaCnv::initialize() ; 
  if( sc.isFailure() ) { return Error("Initialize::unable to initialize GiGaCnv base class"); } 

  IService* pISvc;
  sc = serviceLocator()->getService("GiGaGeomCnvSvc", pISvc);
  //sc = serviceLocator()->getService("GiGaGeomCnvSvc", 
	//			    IID_IGiGaGeomCnvSvc, 
	//		    reinterpret_cast<IInterface*&>(m_GiGaGeomCnvSvc));
  if( sc.isFailure() ) {
    log << MSG::ERROR << "Unable to locate service GiGaGeomCnvSvc" << endreq;
    return sc;
  }
  m_GiGaGeomCnvSvc = dynamic_cast<GiGaGeomCnvSvc*>(pISvc);
  return sc;
}



StatusCode GiGaDetElemCnv::createRep( DataObject*     Object  , IOpaqueAddress*& Address ) 
{
  ///
  Address = 0 ; 
  ///
  if( 0 == Object        ) { return Error("CreateRep::DataObject* point to NULL!");} 
  ///
  ///
  DetectorElement* obj = 0 ; 
  try        { obj = dynamic_cast<DetectorElement*>( Object ) ; } 
  catch(...) { obj =                                0 ; } 
  ///
  if( 0 == obj           ) { return Error("CreateRep::Bad cast to DetectorElement*");}
  ///
  if( 0 == cnvSvc()      ) { return Error("CreateRep::Conversion Service is unavailble ") ;} 
  ///
  /// create IOpaqueAddress
  IAddressCreator* addrCreator = 0 ; 
  try        { addrCreator = dynamic_cast<IAddressCreator*> ( cnvSvc() ) ; } 
  catch(...) { addrCreator =                                           0 ; } 
  ///
  if( 0 == addrCreator   ) { return Error("CreateRep::Address Creator is unavailable   ") ;} 
  ///
  StatusCode status = addrCreator->createAddress( repSvcType() , classID() , "GiGaGeom" , "GiGaElementObject" , -1 , Address );   
  ///
  if( status.isFailure() ) { return Error("CreateRep::Error in Address Creator"); }
  ///
  if( 0 == Address       ) { return Error("CreateRep::Address is not created" ); }   
  ///
  return updateRep( Object , Address ) ; 
  /// 
}; 




StatusCode GiGaDetElemCnv::updateRep(DataObject * dataObj, IOpaqueAddress *& address) {
//  address = 0;
  StatusCode sc;  
  MsgStream log( msgSvc(), "GiGaDetElemCnv" ); 
  DetectorElement* DE = 0;
  // Check if it is a detector element.
  try {  
    DE = dynamic_cast<DetectorElement*>(dataObj); 
  } catch(...) {
    std::string errorMess = " Detector element " + DE->fullpath() + " cannot be converted to G4. It should be in the Data Store before trying the conversion";
    return Error(errorMess, StatusCode::FAILURE);
  }
  // G4 places phys. vol. wrt mother phys. volum.
  // I need a dummy physical volume to place sub-volumes in it
  G4LogicalVolume* G4LVRoot = 0;
  bool G4LVExist = false;
  sc = createG4DetElemRoot(DE, G4LVRoot, G4LVExist); 
  if(sc.isFailure()){
    std::string errorMess = " Unable to create top Physical and Logical Volumes for Det Element: "
       + DE->fullpath();
    return Error(errorMess, StatusCode::FAILURE);
  }
  
  // If the LV was already converted the whole substructure was converted too.
  // do not do it again.
  if(!G4LVExist){
    sc = convertSubVolumes(DE,G4LVRoot);
    if(sc.isFailure()){
    std::string errorMess = " Unable to convert list of daughter volumes for det. element: " 
          + DE->fullpath();
    return Error(errorMess, StatusCode::FAILURE);
    }
  }

  // In Gaudi Detector Description one can follow the geometry structure 
  // starting from a LV down: LV -> list of PV -> Lv for each PV ...
  // We have in subPhysVolumes the complete list of PV and we can get 
  // the corresponding LV for each of them. 
  return StatusCode::SUCCESS;
}

StatusCode GiGaDetElemCnv::createG4DetElemRoot(const DetectorElement* DE, G4LogicalVolume*& G4LV,  bool& G4LVExist) {
  MsgStream log( msgSvc(), "GiGaDetElemCnv" );
  StatusCode sc = createRootPairLVPV(DE, G4LV, G4LVExist);
  if(sc.isFailure()){
    std::string errorMess =     " Unable to create pair LV - PV for detector element " 
        + DE->fullpath();
    return Error(errorMess, StatusCode::FAILURE);
  }
  return StatusCode::SUCCESS;
}

StatusCode GiGaDetElemCnv::createRootPairLVPV(const DetectorElement* DE, G4LogicalVolume*& G4LV, bool& G4LVExist) {
  MsgStream log( msgSvc(), "GiGaDetElemCnv" );   
  LVolume* LV = dynamic_cast<LVolume*>(DE->geometry()->lvolume());
  StatusCode sc;
  bool converted = false;
  // Check first if the Logical volume was already converted 
  G4LV = alreadyConverted(LV, G4LVExist);
  if(G4LV == 0){
    sc = createG4LogVol(LV,G4LV);
    if(sc.isFailure()){
    std::string errorMess = " Failed to create G4 Log Vol for detector element "  
        + DE->fullpath();
    return Error(errorMess, StatusCode::FAILURE);
    }
  }
  G4LogicalVolume* G4RootLV = m_GiGaGeomCnvSvc->G4WorldLV();
  sc = createG4RootPhysVol(DE,G4LV,G4RootLV);
  if(sc.isFailure()){
    std::string errorMess =     " Failed to create G4 Phys Vol for detector element "  
        + DE->fullpath();
    return Error(errorMess, StatusCode::FAILURE);
  }
  return StatusCode::SUCCESS;
}


G4LogicalVolume* GiGaDetElemCnv::alreadyConverted(const LVolume* LV, bool& converted) {
  MsgStream log( msgSvc(), "GiGaDetElemCnv" ); 
  G4LogicalVolume* G4LV = 0;
  G4LogicalVolumeStore* store = G4LogicalVolumeStore::GetInstance();
  for(int i = 0; i < store->entries(); i++)
    {
      G4LogicalVolume* lv = (*store)[i];
      if(LV->fullpath() == lv->GetName() )
      {
        G4LV = (*store)[i];
        converted = true;
        break;
      }
   }
  return G4LV;
}


StatusCode GiGaDetElemCnv::createG4LogVol(const LVolume* LV, G4LogicalVolume*& G4LV)  {
  MsgStream log( msgSvc(), "GiGaDetElemCnv" ); 
  G4VSolid* G4Sd = 0;
  StatusCode sc = createG4Solid(LV->solid(), LV->fullpath(), G4Sd);
  if(sc.isFailure()){
    std::string errorMess =  " Error creating G4 Solid, unable to creat Logical volume" 
      + LV->fullpath();
    return Error(errorMess, StatusCode::FAILURE);
  }

  // Three last arguments are set to 0: 
  // G4FieldManager, G4VSensitiveDetector, G4UserLimits
  G4Material* material = G4Material::GetMaterial(LV->materialName());
  if (material == 0) {
    log << MSG::WARNING << "Material " << LV->materialName() <<
      " Needed for LV " << LV->fullpath() << " Has not been converted to G4 " << endreq; 
  }
  G4LV = new G4LogicalVolume(G4Sd,material,LV->fullpath(),0,0,0);
  log << MSG::DEBUG << " Creating new G4 Log. Vol. " << LV->fullpath()  
    << " Material is: " << LV->materialName() << endreq;
  // sc = m_GiGaGeomCnvSvc->insertLV(LV->fullpath(),G4LV);
  //if (sc.isFailure()){
  //  log << MSG::ERROR << "Unable to insert created G4 Log. Vol in book-keeping map" << endreq;
  //  return StatusCode::FAILURE;
  //}
  return StatusCode::SUCCESS;
}

StatusCode GiGaDetElemCnv::createG4Solid(const ISolid* Sd, std::string name, G4VSolid*& G4Sd) {
  MsgStream log( msgSvc(), "GiGaDetElemCnv" );   
  StatusCode sc;
  // write createLogVol(DetEle* ), and createSolid functions
  std::string solidType = Sd->typeName();
  log << MSG::DEBUG << " Creating new G4 solid " << name <<
                       " of type " << solidType << endreq;
  if( solidType == "SolidBox") {
      const SolidBox* sBox = dynamic_cast<const SolidBox*>(Sd);
      G4Sd = new G4Box(name+"_Box",sBox->xHalfLength(),
                                             sBox->yHalfLength(),
                                             sBox->zHalfLength());
  } else if (solidType == "SolidCons") {
      const SolidCons* sCons = dynamic_cast<const SolidCons*>(Sd);
      G4Sd = new G4Cons(name+"_Cons",sCons->innerRadiusAtMinusZ(),
                                               sCons->outerRadiusAtMinusZ(),
                                               sCons->innerRadiusAtPlusZ(),
                                               sCons->outerRadiusAtPlusZ(),
                                               sCons->zHalfLength(),
                                               sCons->startPhiAngle(),
                                               sCons->deltaPhiAngle());
  } else if (solidType == "SolidSphere") {
      const SolidSphere* sSphere = dynamic_cast<const SolidSphere*>(Sd);
      G4Sd = new G4Sphere(name+"_Sphere",sSphere->insideRadius(),
                                                   sSphere->outerRadius(), 
                                                   sSphere->startPhiAngle(),
                                                   sSphere->deltaPhiAngle(),
                                                   sSphere->startThetaAngle(),
                                                   sSphere->deltaThetaAngle());
  } else if (solidType == "SolidTrd") {
      const SolidTrd* sTrd = dynamic_cast<const SolidTrd*>(Sd);
      G4Sd = new G4Trd(name+"_Trd",sTrd->xHalfLength1(),
                                                sTrd->xHalfLength2(),
                                                sTrd->yHalfLength1(),
                                                sTrd->yHalfLength2(),
                                                sTrd->zHalfLength());
   } else if (solidType == "SolidTubs") {
      const SolidTubs* sTubs = dynamic_cast<const SolidTubs*>(Sd);
      G4Sd = new G4Tubs(name+"_Tubs",sTubs->innerRadius(),
                                                 sTubs->outerRadius(),
                                                 sTubs->zHalfLength(),
                                                 sTubs->startPhiAngle(),
                                                 sTubs->deltaPhiAngle());
   } else if (solidType == "SolidUnion") {
      const SolidUnion* sUnion = dynamic_cast<const SolidUnion*>(Sd);
      const ISolid* first = sUnion->first();
      sc = createG4Solid(first,name+"_union",G4Sd);
      if (sc.isFailure()){
        std::string errorMess = " Failed to create first solid "
          + first->name() + " in Solid Union " + sUnion->name();
        return Error(errorMess, StatusCode::FAILURE);
      }
      for (int iDaughter = 0; iDaughter < sUnion->noChildrens(); ++iDaughter){
        const ISolid* daughter = (*sUnion)[iDaughter];
        G4VSolid* G4Daughter = 0;
        sc = createG4Solid(dynamic_cast<const SolidChild*>(daughter)->solid(),name+"_child", G4Daughter);
        if (sc.isFailure()){
          std::string errorMess = " Failed to create daughter solid " + daughter-> name() 
             + "in Solid Union "  + sUnion->name();
          return Error(errorMess, StatusCode::FAILURE);
        }
        const SolidChild* child = dynamic_cast<const SolidChild*>(daughter);
        G4UnionSolid* tmpG4Bool =0;
        if(child->simple()){
           tmpG4Bool = new G4UnionSolid( G4Sd->GetName() + "/" + child->name(), G4Sd, G4Daughter);
        } else {
           tmpG4Bool = new G4UnionSolid( G4Sd->GetName() + "/" + child->name(), G4Sd, G4Daughter, child->matrix() );
        }
        G4Sd = tmpG4Bool;
      }
   } else if (solidType == "SolidSubtraction") {
      const SolidSubtraction* sSubt = dynamic_cast<const SolidSubtraction*>(Sd);
      const ISolid* first = sSubt->first();
      sc = createG4Solid(first,name+"_Subt", G4Sd);
      if (sc.isFailure()){
         std::string errorMess = " Failed to create first solid " 
           + first->name() + " in Solid Subtraction " + sSubt->name();
         return Error(errorMess, StatusCode::FAILURE);
      }
      for (int iDaughter = 0; iDaughter < sSubt->noChildrens(); ++iDaughter){
        const ISolid* daughter = (*sSubt)[iDaughter];
        G4VSolid* G4Daughter = 0;
        sc = createG4Solid(dynamic_cast<const SolidChild*>(daughter)->solid(), name+"_child", G4Daughter);
        if (sc.isFailure()){
          std::string errorMess = " Failed to create daughter solid " + daughter-> name() 
            + "in Solid Subtraction "  + sSubt->name();
          return Error(errorMess, StatusCode::FAILURE);
        }
        const SolidChild* child = dynamic_cast<const SolidChild*>(daughter);
        G4SubtractionSolid*  tmpG4Bool;
        if(child->simple()){
           tmpG4Bool = new G4SubtractionSolid( G4Sd->GetName() + "/" + child->name(), G4Sd, G4Daughter);
        } else {
           tmpG4Bool = new G4SubtractionSolid( G4Sd->GetName() + "/" + child->name(), G4Sd, G4Daughter, child->matrix() );
        }
        G4Sd = tmpG4Bool;
      }
   } else if (solidType == "SolidIntersection") {
      const SolidIntersection* sInters = dynamic_cast<const SolidIntersection*>(Sd);
      const ISolid* first = sInters->first();
      sc = createG4Solid(first, name+"_Inter", G4Sd);
      if (sc.isFailure()){
        std::string errorMess = " Failed to create first solid "
          + first->name() + " in Solid Intersection " + sInters->name();
        return Error(errorMess, StatusCode::FAILURE);
      }
      for (int iDaughter = 0; iDaughter < sInters->noChildrens(); ++iDaughter){
        const ISolid* daughter = (*sInters)[iDaughter];
        G4VSolid* G4Daughter;
        sc = createG4Solid(dynamic_cast<const SolidChild*>(daughter)->solid(), name+"_child", G4Daughter);
        if (sc.isFailure()){
           std::string errorMess = " Failed to create daughter solid " + daughter-> name() 
            + "in Solid Intersection "  + sInters->name();
           return Error(errorMess, StatusCode::FAILURE);
        }
        const SolidChild* child = dynamic_cast<const SolidChild*>(daughter);
        G4IntersectionSolid* tmpG4Bool;
        if(child->simple()){
           tmpG4Bool = new G4IntersectionSolid( G4Sd->GetName() + "/" + child->name(), G4Sd, G4Daughter);
        } else {
           tmpG4Bool = new G4IntersectionSolid( G4Sd->GetName() + "/" + child->name(), G4Sd, G4Daughter, child->matrix() );
        }
        G4Sd = tmpG4Bool;
      }
   } else {
      std::string errorMess =  "Invalid Solid type, cannot create G4 solid ";
      return Error(errorMess, StatusCode::FAILURE);
   }
  return StatusCode::SUCCESS;
}

StatusCode GiGaDetElemCnv::createG4RootPhysVol(const DetectorElement* DE, G4LogicalVolume* G4LV, G4LogicalVolume* G4MotherLV) {
  // Note that two arguments are hard coded:
  //      pMany = false, 
  //      last argument = placement number = 0 for Det. Element Root
  MsgStream log( msgSvc(), "GiGaDetElemCnv" );   
  std::string PVName = dynamic_cast<LVolume*>(DE->geometry()->lvolume())->fullpath();
  G4VPhysicalVolume* G4PV = new G4PVPlacement(DE->geometry()->matrixInv(), G4LV,"G4"+PVName+"_PV",G4MotherLV, false, 0);
  log << MSG::DEBUG << " Creating Root G4 Phys. Vol. for Det. Elem. " << PVName << endreq;
  return StatusCode::SUCCESS;
}


StatusCode GiGaDetElemCnv::createG4PhysVol(const IPVolume* PV, G4LogicalVolume* G4LV, G4LogicalVolume* motherG4LV, const int pCopy) {

  //  note: pMany = false, 
  MsgStream log( msgSvc(), "GiGaDetElemCnv" );   
  std::string PVName = PV->name();                                                   
//  G4PV = new G4PVPlacement(PV->matrix(), "G4"+PVName+"_PV",G4LV,motherG4PV, false, 0);
  G4VPhysicalVolume* G4PV = new G4PVPlacement(PV->matrix(),G4LV,"G4"+PVName+"_PV",motherG4LV, false, pCopy);
  log << MSG::DEBUG << " Creating new G4 Phys. Vol. " << PVName << endreq;
  return StatusCode::SUCCESS;
}

StatusCode GiGaDetElemCnv::convertSubVolumes(const DetectorElement* DE, G4LogicalVolume* G4MotherLV) {
  ILVolume* LV = DE->geometry()->lvolume();
  if(LV->noPVolumes() != 0){
    StatusCode sc = convertSubVolumes (LV, G4MotherLV);
    if (!sc.isSuccess()) return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}


StatusCode GiGaDetElemCnv::convertSubVolumes(const ILVolume* LVMother, G4LogicalVolume* G4MotherLV ) {
  ILVolume::PVolumes::const_iterator PVIt;
  int pCopy = 0;
  for( PVIt = LVMother->pvBegin(); PVIt < LVMother->pvEnd(); PVIt ++){
     StatusCode sc = createPairLVPV(*PVIt, G4MotherLV, pCopy);
     pCopy++;
  }
  return StatusCode::SUCCESS;
}


StatusCode GiGaDetElemCnv::createPairLVPV(const IPVolume* PV, G4LogicalVolume* G4MotherLV, const int pCopy) {
  MsgStream log( msgSvc(), "GiGaDetElemCnv" ); 
  bool converted = false;
  StatusCode sc;
  LVolume* LV = dynamic_cast<LVolume*>(PV->lvolume());
  // Check first if the Logical volume was already converted 
  G4LogicalVolume* G4LV = 0;
  bool G4LVExist = false;
  G4LV = alreadyConverted(LV, G4LVExist);
  if(G4LV == 0){
    sc = createG4LogVol(LV , G4LV);
    if(sc.isFailure()){
      log << MSG::ERROR << "Failed to create G4 Log. Volume for " << LV->fullpath() << endreq;
      return StatusCode::FAILURE;
    }
  }
  // Create the corresponding Phys. Vol.
  sc = createG4PhysVol(PV,G4LV,G4MotherLV,pCopy);
  if(sc.isFailure()){
    log << MSG::ERROR << "Failed to create G4 Phys. Volume for " << LV->fullpath() << endreq;
    return StatusCode::FAILURE;
  }
  // If the Log. Vol was already converted the sub-structure is converted too
  // No need to re do it!
  if(!G4LVExist) {
    if(LV->noPVolumes() != 0){
      StatusCode sc = convertSubVolumes(LV, G4LV);
      if (!sc.isSuccess()) return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}

// Class ID
const CLID&  GiGaDetElemCnv::classID            () { return DetectorElement::classID() ; }

// StorageType 
const unsigned char GiGaDetElemCnv::storageType () { return GiGaGeom_StorageType; } 


