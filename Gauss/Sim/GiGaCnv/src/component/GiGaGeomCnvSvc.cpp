// $Id: GiGaGeomCnvSvc.cpp,v 1.10 2002-05-04 20:53:17 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.9  2002/05/04 20:39:36  ibelyaev
//  see $GIGACNVROOT/release.notes (4 May 2002)
//
// ===========================================================================
#define GIGACNV_GIGAGEOMCNVSVC_CPP 1 
// ============================================================================
#include <string>
#include <algorithm>
// from Gaudi 
#include "GaudiKernel/SvcFactory.h" 
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IDataSelector.h"
#include "GaudiKernel/IConverter.h"
#include "GaudiKernel/IObjManager.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/GenericAddress.h"
// from DetDesc 
#include "DetDesc/DetectorElement.h"
#include "DetDesc/Solids.h"
// Include G4 Stuff
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4Material.hh"
//
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4Trap.hh"
#include "G4Tubs.hh"
#include "G4IntersectionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
//
#include "G4VisAttributes.hh"
#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
// from GiGa 
#include "GiGa/IGiGaSensDet.h"
#include "GiGa/IGiGaSensDetFactory.h"
#include "GiGa/IGiGaMagField.h"
#include "GiGa/IGiGaMagFieldFactory.h"
#include "GiGa/GiGaUtil.h"
// From GiGaCnv 
#include "GiGaCnv/GiGaAssembly.h"
#include "GiGaCnv/GiGaAssemblyStore.h"
// local 
#include "GiGaVolumeUtils.h"
#include "GiGaGeomCnvSvc.h"

// ============================================================================
/** @file GiGaGeomCnvSvc.cpp
 *
 *  Implementation of class GiGaGeomCnvSvc 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   7 Aug 2000
 */
// ============================================================================

// ============================================================================
/** mandatory factory business 
 */
// ============================================================================
static const SvcFactory<GiGaGeomCnvSvc>         s_Factory ; 
const       ISvcFactory&GiGaGeomCnvSvcFactory = s_Factory ;

// ============================================================================
/** standard constructor 
 *  @param ServiceName      name of the service 
 *  @param ServiceLocator   pointer to service locator 
 */
// ============================================================================
GiGaGeomCnvSvc::GiGaGeomCnvSvc
( const std::string&   ServiceName          , 
  ISvcLocator*         ServiceLocator       ) 
  : GiGaCnvSvcBase   ( ServiceName          , 
                       ServiceLocator       , 
                       GiGaGeom_StorageType )
  , m_worldPV        ( 0                    ) 
  , m_worldNamePV    ( "Universe"           )
  , m_worldNameLV    ( "World"              )
  , m_worldMaterial  ( "/dd/Materials/Air"  )
  // parameters of world volume 
  , m_worldX         ( 50. * m              )
  , m_worldY         ( 50. * m              )
  , m_worldZ         ( 50. * m              )
  //
  , m_worldMagField  ( ""                   )
  // special sensitive detector for estimation of material budget 
  , m_budget         ( ""                   )
  //
  , m_SDs            ()
  , m_MFs            ()
  ///
{
  ///
  setNameOfDataProviderSvc("DetectorDataSvc"); 
  ///
  declareProperty("WorldMaterial"             , m_worldMaterial );
  declareProperty("WorldPhysicalVolumeName"   , m_worldNamePV   );
  declareProperty("WorldLogicalVolumeName"    , m_worldNameLV   );
  ///
  declareProperty("XsizeOfWorldVolume"        , m_worldX        );
  declareProperty("YsizeOfWorldVolume"        , m_worldY        );
  declareProperty("XsizeOfWorldVolume"        , m_worldZ        );
  ///
  declareProperty("MaterialBudget"            , m_budget        );
  declareProperty("WorldMagneticField"        , m_worldMagField );
  ///
};

// ============================================================================
/** Retrieve the pointer for G4 materials from G4MaterialTable, 
 *  (could trigger the conversion of the (DetDesc) Material)
 *  @param  Name    name/address/location of Material object 
 *  @return pointer to converted G4Material object 
 */
// ============================================================================
G4Material* GiGaGeomCnvSvc::material ( const std::string& Name )
{
  /// first look throught G4MaterialTable
  {
    G4Material* mat = G4Material::GetMaterial( Name ) ;
    if( 0 != mat ) { return mat ; }
  }
  /// retrieve material by name and convert it to G4 representation  
  SmartDataPtr<DataObject>  object( detSvc() , Name ); 
  if( !object ) 
    { Error("Failed to locate DataObject with name:" + Name ) ; return 0 ;  } 
  IOpaqueAddress* Address = 0 ;
  StatusCode sc = createRep( object , Address );
  if( sc.isFailure() ) 
    { Error("Failed to create G4 representation of Material=" + 
            Name,sc) ; return 0 ; }
  /// update the registry
  object->registry()->setAddress( Address );
  /// look throught G4MaterialTable
  {
    G4Material* mat = G4Material::GetMaterial( Name ) ;
    if( 0 != mat ) { return mat ; } 
  }
  ///
  Error("Failed to find G4Material with name '" + Name + "'" );
  ///
  return 0;     
};

// ============================================================================
/** Retrieve the pointer for G4 materials from G4MaterialTable, 
 *  (could trigger the conversion of the (DetDesc) Material)
 *  @att obsolete method 
 *  @param  Name    name/address/location of Material object 
 *  @return pointer to converted G4Material object 
 */
// ============================================================================
G4Material*    GiGaGeomCnvSvc::g4Material( const std::string& Name )
{
  Warning(" g4Material() is the obsolete method, use material()!");
  return material( Name );
};

// ============================================================================
/** Retrive the pointer to converter volumes/assemblies 
 *  (could trigger the conversion of the (DetDesc) LVolume/LAssembly)    
 *  @param  name    name/address/location of Volume/Assembly object 
 *  @return pointer to converted GiGaVolume  object 
 */
// ============================================================================
GiGaVolume GiGaGeomCnvSvc::volume( const std::string& Name )
{
  { /// first look through converted volumes   
    G4LogicalVolume* lv = GiGaVolumeUtils::findLVolume   ( Name );
    if( 0 != lv ) { return GiGaVolume( lv ,  0 ) ; }
  }
  { /// look through converted assemblies 
    GiGaAssembly*    la = GiGaVolumeUtils::findLAssembly ( Name );
    if( 0 != la ) { return GiGaVolume( 0  , la ) ; }
  }
  /// convert the object 
  SmartDataPtr<DataObject> object( detSvc() , Name );
  if( !object )
    { 
      Error("There is no DataObject with name '" + Name + "'") ; 
      return GiGaVolume() ; 
    } 
  /// create the representation
  IOpaqueAddress* address = 0 ;
  StatusCode sc = createRep( object , address );
  if( sc.isFailure() ) 
    {
      Error("Could not create the representation of '" + Name + "'",sc);
      return GiGaVolume() ;
    }
  /// update the registry
  object->registry()->setAddress( address );
  ///
  { 
    /// again look through converted volumes 
    G4LogicalVolume* lv = GiGaVolumeUtils::findLVolume   ( Name );
    if( 0 != lv ) { return GiGaVolume( lv ,  0 ) ; }
  }
  { 
    /// again look through converted assemblies 
    GiGaAssembly*    la = GiGaVolumeUtils::findLAssembly ( Name );
    if( 0 != la ) { return GiGaVolume( 0  , la ) ; }
  }
  ///
  Error("Failed to find GiGaVolume with name '" + Name + "'");
  ///
  return GiGaVolume();
};

// ============================================================================
/** Retrive the pointer to G4LogicalVolume  from converted volumes,
 * (could trigger the conversion of the (DetDesc) LVolume)    
 *  @att obsolete method 
 *  @param  Name    name/address/location of LVolume object 
 *  @return pointer to converted G4LogicalVolume object 
 */
// ============================================================================
G4LogicalVolume* GiGaGeomCnvSvc::g4LVolume( const std::string& Name )
{
  Warning(" g4LVolume() is the obsolete method, use volume()!");
  const GiGaVolume vol = volume( Name ) ;
  return vol.valid() ? vol.volume() : (G4LogicalVolume*) 0 ;
};

// ============================================================================
/** convert (DetDesc) Solid object into (Geant4) G4VSolid object 
 *  @param  Sd        pointer to Solid object 
 *  @return pointer to converter G4VSolid object 
 */
// ============================================================================
G4VSolid*    GiGaGeomCnvSvc::solid ( const ISolid*      Sd     )  
{
  ///
  if ( 0 == Sd) { Error(" solid():: ISolid* point to NULL"); return 0;  }
  ///
  const std::string solidType( Sd->typeName() ); 
  ///
  /// box?  
  {
    const SolidBox* sBox = dynamic_cast<const SolidBox*>(Sd);
    if( solidType == "SolidBox" && 0 != sBox ) 
      { return new G4Box( sBox->name(),
                          sBox->xHalfLength(),
                          sBox->yHalfLength(),
                          sBox->zHalfLength()); } 
  }
  /// cons?
  {
    const SolidCons* sCons = dynamic_cast<const SolidCons*>(Sd);
    if( solidType == "SolidCons" && 0 != sCons ) 
      { return new G4Cons( sCons->name(),
                           sCons->innerRadiusAtMinusZ(),
                           sCons->outerRadiusAtMinusZ(),
                           sCons->innerRadiusAtPlusZ(),
                           sCons->outerRadiusAtPlusZ(),
                           sCons->zHalfLength(),
                           sCons->startPhiAngle(),
                           sCons->deltaPhiAngle() ); }
  }
  /// sphere?
  {
    const SolidSphere* sSphere = dynamic_cast<const SolidSphere*>(Sd);
    if ( solidType == "SolidSphere" && 0 != sSphere ) 
      { return new G4Sphere( sSphere->name            (),
                             sSphere->insideRadius    (),
                             sSphere->outerRadius     (), 
                             sSphere->startPhiAngle   (),
                             sSphere->deltaPhiAngle   (),
                             sSphere->startThetaAngle (),
                             sSphere->deltaThetaAngle () ); }
  }
  /// trd? 
  {
    const SolidTrd* sTrd = dynamic_cast<const SolidTrd*>(Sd);
    if (solidType == "SolidTrd" && 0 != sTrd ) 
      { return new G4Trd( sTrd->name(),
                          sTrd->xHalfLength1(),
                          sTrd->xHalfLength2(),
                          sTrd->yHalfLength1(),
                          sTrd->yHalfLength2(),
                          sTrd->zHalfLength()); }
  }
  /// tubs ?   
  { 
    const SolidTubs* sTubs = dynamic_cast<const SolidTubs*>(Sd);
    if (solidType == "SolidTubs" && 0 != sTubs ) 
      { return new G4Tubs( sTubs->name(),
                           sTubs->innerRadius(),
                           sTubs->outerRadius(),
                           sTubs->zHalfLength(),
                           sTubs->startPhiAngle(),
                           sTubs->deltaPhiAngle()); }
  }
  /// trap ?   
  {  
    const SolidTrap* sTrap = dynamic_cast<const SolidTrap*>(Sd);
    if (solidType == "SolidTrap" && 0 != sTrap ) 
      { return new G4Trap( sTrap->name             (),
                           sTrap->zHalfLength      (),
                           sTrap->theta            (),
                           sTrap->phi              (),
                           sTrap->dyAtMinusZ       (),
                           sTrap->dxAtMinusZMinusY (),
                           sTrap->dxAtMinusZPlusY  (),
                           sTrap->alphaAtMinusZ    (),
                           sTrap->dyAtPlusZ        (),
                           sTrap->dxAtPlusZMinusY  (),
                           sTrap->dxAtPlusZPlusY   (),
                           sTrap->alphaAtPlusZ     () ); }
  }
  /// boolean ? 
  {
    const SolidBoolean* sBool = dynamic_cast<const SolidBoolean*> (Sd);
    if( 0 != sBool ) { return g4BoolSolid( sBool ); }
  }
  /// unknown solid!
  Error("g4solid Unknown Solid="+solidType+"/" + 
        System::typeinfoName(typeid(Sd))+"/"+Sd->name());
  ///
  return 0;
};

// ============================================================================
/** convert (DetDesc) Solid object into (Geant4) G4VSolid object 
 *  @att obsolete method 
 *  @param  Solid pointer to Solid object 
 *  @return pointer to converter G4VSolid object 
 */
// ============================================================================
G4VSolid*    GiGaGeomCnvSvc::g4Solid( const ISolid* Sd )
{
  Warning(" g4Solid() is the obsolete method, use solid()!");
  return solid( Sd ) ;
};

///
G4VSolid*  GiGaGeomCnvSvc::g4BoolSolid( const SolidBoolean* Sd ) 
{
  ///
  if( 0 == Sd )
    { Error("g4BoolSolid, SolidBoolean* point to NULL!") ; return 0; }
  const SolidSubtraction*  sSub = 
    dynamic_cast<const SolidSubtraction*>  ( Sd );
  const SolidIntersection* sInt = 
    dynamic_cast<const SolidIntersection*> ( Sd );
  const SolidUnion*        sUni = 
    dynamic_cast<const SolidUnion*>        ( Sd );
  if( 0 == sSub && 0 == sInt && 0 == sUni ) 
    { Error("g4BoolSolid, Unknown type of Boolean solid=" + 
            Sd->typeName())                    ; return 0; }
  if( 0 == Sd->first() ) 
    { Error("g4BoolSolid, Wrong MAIN/FIRST solid for Boolean solid=" + 
            Sd->name())             ; return 0; }
  ///
  G4VSolid* first = solid( Sd->first() ); 
  if( 0 == first      )
    { Error("g4BoolSolid, could not convert MAIN solid for Boolean solid=" + 
            Sd->name()) ; return 0; }
  ///
  G4VSolid* g4total = first;
  typedef SolidBoolean::SolidChildrens::const_iterator CI;
  for( CI it = Sd->childBegin() ; Sd->childEnd() != it ; ++it )
    {
      const SolidChild* child = *it ; 
      G4VSolid* g4child = solid( child->solid() );
      if( 0 == g4child ) 
        { Error("g4BoolSolid, could not convert solid for Boolean solid=" + 
                Sd->name())  ; return 0; }
      if      ( 0 != sSub    ) 
        { g4total = 
            new G4SubtractionSolid  ( Sd->first()->name()+"-"+child->name() , 
                                      g4total , g4child ,
                                      child->matrix().inverse() ) ; }
      else if ( 0 != sInt    )
        { g4total = 
            new G4IntersectionSolid ( Sd->first()->name()+"*"+child->name() , 
                                      g4total , g4child , 
                                      child->matrix().inverse() ) ; }
      else if ( 0 != sUni    )
        { g4total = 
            new G4UnionSolid        ( Sd->first()->name()+"+"+child->name() , 
                                      g4total , g4child , 
                                      child->matrix().inverse() ) ; }
      else
        { Error("g4BoolSolid, Unknown type of Boolean solid=" + 
                Sd->typeName()) ; return 0; }
    } 
  ///
  g4total->SetName( Sd->name() ) ;
  ///
  return g4total;
};

// ============================================================================
/** standard initialization method 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaGeomCnvSvc::initialize() 
{ 
  //
  StatusCode sc = GiGaCnvSvcBase::initialize();
  if(sc.isFailure() ) 
    { return Error(" Failed to initialize GiGaCnvSvc", sc      ) ; }
  // we explicitely need detSvc(), check it! 
  if( 0 == detSvc() ) 
    { return Error(" DetectorProviderSvc is not located! "     ) ; }
  // check for special run with  material budget counters:
  if( !m_budget.empty() )
    { Warning(" Special run for material budget calculation! " ) ; }
  // 
  return StatusCode::SUCCESS;
};

// ============================================================================
/** standard finalization method 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaGeomCnvSvc::finalize()   
{ 
  // finalize all created sensitive detectors 
  std::for_each( m_SDs.begin () , 
                 m_SDs.end   () , 
                 std::mem_fun( &IGiGaSensDet::finalize ) );
  // finalize all created mag field objects 
  std::for_each( m_MFs.begin () , 
                 m_MFs.end   () , 
                 std::mem_fun( &IGiGaMagField::finalize ) );
  // clear store of assemblies!
  StatusCode sc = GiGaAssemblyStore::store()->clear();
  //
  return GiGaCnvSvcBase::finalize(); 
};

// ============================================================================
/** Retrieve the pointer to top-level "world" volume,
 *  needed for Geant4 - root for the whole Geant4 geometry tree 
 *  @see class IGiGaGeoSrc 
 *  @return pointer to constructed(converted) geometry tree 
 */  
// ============================================================================
G4VPhysicalVolume* GiGaGeomCnvSvc::world () 
{
  // already created? 
  if( 0 != m_worldPV ) { return m_worldPV ; } /// already created 
  ///
  { MsgStream log(msgSvc(),name()); 
  log << MSG::INFO << " Create the WORLD volume!" << endreq; } 
  // create it!
  G4Material* MAT = material ( m_worldMaterial );   
  if( 0 == MAT )
    { Error("world():: could not locate/convert material=" + 
            m_worldMaterial) ; return 0 ; }
  //
  G4VSolid*        SD = 
    new G4Box("WorldBox", m_worldX , m_worldY , m_worldZ ) ; 
  // create the world volume  
  G4LogicalVolume* LV = createG4LV ( SD , MAT , m_worldNameLV ); 
  /// make it invisible 
  LV -> SetVisAttributes ( G4VisAttributes::Invisible );
  ///
  m_worldPV           = 
    new G4PVPlacement( 0 , Hep3Vector() , m_worldNamePV , LV , 0 , false , 0 );
  ///
  // create the magnetic field for world volume
  if( !m_worldMagField.empty() )
    {
      ///
      IGiGaMagField* mf = 0;
      StatusCode sc = magnetic( m_worldMagField , mf );  
      if( sc.isFailure() ) 
        { Error("world():: could not construct Global Magnetic Field=" + 
                m_worldMagField, sc ) ; return 0 ; }  
      if( 0 == mf        ) 
        { Error("world():: could not construct Global Magnetic Field=" +
                m_worldMagField, sc ) ; return 0 ; }  
      ///
      {
        G4TransportationManager* trnspMgr = 
          G4TransportationManager::GetTransportationManager() ; 
        if( 0 == trnspMgr  )
          { Error("world():: could not locate G4TranspostationManager*") ; 
          return 0 ; }         
        G4FieldManager*          fieldMgr = trnspMgr->GetFieldManager(); 
        if( 0 == fieldMgr  ) 
          { Error("world():: could not locate G4FieldManager* object ") ; 
          return 0 ; }         
        fieldMgr->SetDetectorField  ( mf );
        fieldMgr->CreateChordFinder ( mf );  
      }
      ///
      Print("worl():: World Magnetic Field is set to be = " + 
            System::typeinfoName( typeid( *mf ) )+"/"+mf->name() );
    }
  else { Warning("world():: Magnetic Field is not requested to be loaded "); }
  ///
  return m_worldPV ; 
}; 

// ============================================================================
/** Retrieve the pointer to top-level "world" volume,
 *  needed for Geant4 - root for the whole Geant4 geometry tree 
 *  @att obsolete method!
 *  @see class IGiGaGeoSrc 
 *  @return pointer to constructed(converted) geometry tree 
 */  
// ============================================================================
G4VPhysicalVolume* GiGaGeomCnvSvc::G4WorldPV() 
{  
  Warning(" G4WorldPV() is the obsolete method, use world()!");
  return world() ;  
};


// ============================================================================
/** Convert the transient object to the requested representation.
 *  e.g. conversion to persistent objects.
 *  @param     object  Pointer to location of the object 
 *  @param     address Reference to location of pointer with the 
 *                     object address.
 *  @return    status code indicating success or failure
 */
// ============================================================================
StatusCode GiGaGeomCnvSvc::createRep
( DataObject*      object  , 
  IOpaqueAddress*& address ) 
{ 
  ///
  if( 0 == object ) 
    { return Error(" createRep:: DataObject* points to NULL!");}
  ///
  const IDetectorElement* de = dynamic_cast<IDetectorElement*> ( object ) ;
  IConverter* cnv = 
    converter( 0 == de ? object->clID() : CLID_DetectorElement );
  if( 0 == cnv ) 
    { return Error(" createRep:: converter is not found for '" 
                   + object->registry()->identifier() + "'" );}
  ///
  return cnv->createRep( object , address );
};

StatusCode GiGaGeomCnvSvc::queryInterface( const IID& iid , void** ppI )
{ 
  ///
  if  ( 0 == ppI ) { return StatusCode::FAILURE ; }
  /// 
  *ppI = 0 ; 
  ///
  if      ( IGiGaGeomCnvSvc ::interfaceID ()  == iid ) 
    { *ppI  = static_cast<IGiGaGeomCnvSvc*>   ( this )   ; } 
  else if ( IGiGaGeoSrc     ::interfaceID ()  == iid ) 
    { *ppI  = static_cast<IGiGaGeoSrc*>       ( this )   ; } 
  else if ( IGiGaCnvSvc     ::interfaceID ()  == iid ) 
    { *ppI  = static_cast<IGiGaCnvSvc*>       ( this )   ; } 
  else if ( IConversionSvc  ::interfaceID ()  == iid )
    { *ppI  = static_cast<IConversionSvc* >   ( this )   ; }
  else if ( IService        ::interfaceID ()  == iid )
    { *ppI  = static_cast<IService*>          ( this )   ; }
  else if ( IInterface      ::interfaceID ()  == iid )
    { *ppI  = static_cast<IInterface*>        ( this )   ; }
  else                                           
    { return GiGaCnvSvcBase::queryInterface( iid , ppI ) ; } 
  ///
  addRef();
  ///
  return StatusCode::SUCCESS;  
  ///
};

// ============================================================================
/** Instantiate the Sensitive Detector Object 
 *  @param name  Type/Name of the Sensitive Detector Object
 *  @param det   reference to Densitive Detector Object 
 *  @return  status code 
 */
// ============================================================================
StatusCode   GiGaGeomCnvSvc::sensitive   
( const std::string& TypeNick , 
  IGiGaSensDet*&     SD       )  
{
  ///
  SD = 0 ; /// reset the output value 
  ///
  std::string Type,Nick;
  StatusCode sc = GiGaUtil::SplitTypeAndName( TypeNick , Type, Nick ); 
  if( sc.isFailure() ) 
    { return Error("Could not interprete name of SensDet="+TypeNick, sc ) ; }
  /// look at the local storage:
  for( SDobjects::const_iterator it = m_SDs.begin() ; 
       m_SDs.end() != it ; ++it ) 
    {
      if( 0 != *it && (*it)->name() == Nick && 
          GiGaUtil::ObjTypeName( *it )  == Type ) 
        { (*it)->addRef() ; SD = *it ; return StatusCode::SUCCESS ; }  
    } 
  /// create the creator 
  GiGaUtil::SensDetCreator creator( objMgr() , serviceLocator() );
  /// create the object
  SD = creator( Type , Nick ) ;
  if( 0 == SD ) 
    { Error("Could Not Create  the SD Object Type/Nick=" + 
            Type + "/" + Nick ); }
  ///
  StatusCode st = SD->initialize(); 
  if( st.isFailure() ) 
    { Error("Could Not Initialize the SD Object " + 
            TypeNick, st ); delete SD ; SD = 0 ; } 
  ///
  m_SDs.push_back( SD );
  ///
  return StatusCode::SUCCESS;
  ///
};

// ============================================================================
/** Instantiate the Sensitive Detector Object 
 *  @att obsolete method 
 *  @param Type/Nick  Type/Name of the Sensitive Detector Object
 *  @param SD          reference to Densitive Detector Object 
 *  @return  status code 
 */
// ============================================================================
StatusCode GiGaGeomCnvSvc::sensDet
( const std::string& TypeNick , 
  IGiGaSensDet*&     SD )
{
  Warning(" sensDet() is the obsolete method, use sensitive()!");
  return sensitive( TypeNick , SD ) ;  
};

// ============================================================================
/** Instantiate the Magnetic Field Object 
 *  @param TypeNick  Type/Name of the Magnetic Field Object
 *  @param MF   reference to Magnetic Field Object 
 *  @return  status code 
 */
// ============================================================================
StatusCode   GiGaGeomCnvSvc::magnetic 
( const std::string& TypeNick      , 
  IGiGaMagField*&    MF            )  
{
  ///
  MF = 0 ; /// reset the output value 
  ///
  std::string Type,Nick;
  StatusCode sc = GiGaUtil::SplitTypeAndName( TypeNick , Type, Nick ); 
  if( sc.isFailure() ) 
    { return Error("Could not interprete name of MagField=" + 
                   TypeNick, sc ) ; }
  /// look at the local storage:
  for( MFobjects::const_iterator it = m_MFs.begin() ; 
       m_MFs.end() != it ; ++it ) 
    {
      if( 0 != *it && (*it)->name() == Nick && 
          GiGaUtil::ObjTypeName( *it ) == Type ) 
        { (*it)->addRef() ; MF = *it ; return StatusCode::SUCCESS ; }  
    } 
  /// create the creator 
  GiGaUtil::MagFieldCreator creator( objMgr() , serviceLocator() );
  /// create the object
  MF = creator( Type , Nick ) ;
  if( 0 == MF ) 
    { return Error("Could not create the MagField Object Type/Nick=" + 
                   Type + "/" + Nick ); }  
  ///
  StatusCode st = MF->initialize(); 
  if( st.isFailure() ) 
    { delete MF ; MF = 0 ; 
    return Error("Could not Initialize the MagField Object=" + 
                 TypeNick, st ); } 
  ///
  m_MFs.push_back( MF );
  ///
  return StatusCode::SUCCESS;
  ///
};

// ============================================================================
/** Instantiate the Magnetic Field Object 
 *  @att obsolete method 
 *  @param TypeNick  Type/Name of the Magnetic Field Object
 *  @param MF   reference to Magnetic Field Object 
 *  @return  status code 
 */
// ============================================================================
StatusCode GiGaGeomCnvSvc::magField
( const std::string& TypeNick , 
  IGiGaMagField*&    MF       )
{
  Warning(" magField() is the obsolete method, use magnetic()!");
  return magnetic( TypeNick , MF ) ;  
};
// ============================================================================

// ============================================================================
/** Create new G4LogicalVolume. All arguments must be valid!
 *  One should not invoke the "new" operator for Logical Volumes directly
 *  @param solid    pointer to valid solid    object
 *  @param material pointer to valid material object
 *  @param Name     name of logical volume 
 *  @return pointer to new G4LogicalVolume  object 
 */
// ============================================================================
G4LogicalVolume* GiGaGeomCnvSvc::createG4LV 
( G4VSolid*          solid    , 
  G4Material*        material , 
  const std::string& Name     )
{
  // check arguments 
  Assert( 0 != solid    , "createG4LV(): G4VSolid points to NULL!"      ) ;
  Assert( 0 != material , "createG4LV(): G4VSolid points to NULL!"      ) ;
  Assert( 0 == GiGaVolumeUtils::findLVolume( Name ) , 
          "createG4LV(): G4LogicalVolume with name '" +Name+"' exists!" ) ;
  // create 
  G4LogicalVolume* G4LV = 
    new G4LogicalVolume( solid , material , Name , 0 , 0 , 0 );
  // look for global material budget counter 
  if( !m_budget.empty() )
    {
      IGiGaSensDet* budget = 0 ;
      StatusCode sc = sensitive( m_budget , budget );
      Assert( sc.isSuccess() , 
              "createG4LV(): could not get Material Budget '"+m_budget+"'",sc);
      Assert( 0 != budget    , 
              "createG4LV(): could not get Material Budget '"+m_budget+"'"   );
      G4LV->SetSensitiveDetector( budget );
    }
  ///
  return G4LV ;
};
// ============================================================================

// ============================================================================
// End 
// ============================================================================









