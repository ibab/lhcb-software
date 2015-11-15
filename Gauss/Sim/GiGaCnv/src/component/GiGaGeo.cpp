// $Id: GiGaGeo.cpp,v 1.21 2007-08-02 15:03:24 gcorti Exp $
// Include files

// from STL
#include <string>
#include <algorithm>

// from Gaudi
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IConverter.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/GenericAddress.h"

// MathCore -> CLHEP
#include "ClhepTools/MathCore2Clhep.h"

// from DetDesc
#include "DetDesc/DetectorElement.h"
#include "DetDesc/Solids.h"

// from Geant4
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4Trap.hh"
#include "G4Tubs.hh"
#include "G4Polycone.hh"
#include "G4IntersectionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4GeometryManager.hh"
#include "G4SolidStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PhysicalVolumeStore.hh"

#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4SDManager.hh"
#include "G4MagIntegratorStepper.hh"

// from GiGa
#include "GiGa/IGiGaSensDet.h"
#include "GiGa/IGiGaMagField.h"
#include "GiGa/IGiGaFieldMgr.h"
#include "GiGa/GiGaUtil.h"
#include "GiGaCnv/GiGaAssembly.h"
#include "GiGaCnv/GiGaAssemblyStore.h"
#include "GiGaCnv/GiGaVolumeUtils.h"
#include "GiGa/IGDMLReader.h"

// local
#include "GiGaGeo.h"

// ============================================================================
// Implementation file for class : GiGaGeo
//
// 2000-08-07 : Vanya Belyaev
// 2007-07-10 : Sajan Easo, Gloria Corti
// ============================================================================

// Declaration of the Service Factory
DECLARE_SERVICE_FACTORY( GiGaGeo )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GiGaGeo::GiGaGeo( const std::string& serviceName,
                  ISvcLocator* serviceLocator )
  : GiGaCnvSvcBase( serviceName, serviceLocator, GiGaGeom_StorageType )
  , m_worldPV( 0 )
  , m_worldMagField( "" )   // check below for double properties
  , m_SDs()
  , m_MFs()
  , m_FMs()
{

  setNameOfDataProviderSvc("DetectorDataSvc");

  declareProperty( "WorldPhysicalVolumeName", m_worldNamePV = "Universe" );
  declareProperty( "WorldLogicalVolumeName",  m_worldNameLV = "World" );
  declareProperty( "WorldMaterial",   m_worldMaterial = "/dd/Materials/Air");

  declareProperty( "XsizeOfWorldVolume" , m_worldX = 50. * m );
  declareProperty( "YsizeOfWorldVolume" , m_worldY = 50. * m );
  declareProperty( "ZsizeOfWorldVolume" , m_worldZ = 50. * m );

  declareProperty( "GlobalSensitivity" , m_budget = "");
  // Probably obsolete: need to check if WorldMagneticField can be removed
  declareProperty( "WorldMagneticField", m_worldMagField );
  declareProperty( "FieldManager"      , m_worldMagField );

  declareProperty( "ClearStores", m_clearStores = true );

  declareProperty ( "UseAlignment",      m_useAlignment = false ) ;
  declareProperty ( "AlignAllDetectors", m_alignAll = false );
  m_alignDets.clear();
  declareProperty ( "AlignDetectors",    m_alignDets );

  declareProperty( "GdmlReaders", m_gdmlReaderNames );

}

//=============================================================================
// Retrieve the pointer for G4 materials from G4MaterialTable,
//  (could trigger the conversion of the (DetDesc) Material)
//=============================================================================
G4Material* GiGaGeo::material( const std::string& Name ) {

  G4bool warning = false;
  /// first look throught G4MaterialTable
  {
    G4Material* mat = G4Material::GetMaterial( Name , warning );
    if( 0 != mat ) { return mat ; }
  }
  /// retrieve material by name and convert it to G4 representation
  SmartDataPtr<DataObject>  object( detSvc() , Name );
  if( !object ) {
    Error("Failed to locate DataObject with name:" + Name );
    return 0;
  }
  IOpaqueAddress* Address = 0 ;
  StatusCode sc = createRep( object , Address );
  if( sc.isFailure() ) {
    Error("Failed to create G4 representation of Material=" + Name, sc);
    return 0;
  }
  /// update the registry
  object->registry()->setAddress( Address );
  /// look throught G4MaterialTable
  {
    G4Material* mat = G4Material::GetMaterial( Name , warning) ;
    if( 0 != mat ) { return mat ; }
  }

  Error("Failed to find G4Material with name '" + Name + "'" );

  return 0;
}

//=============================================================================
// Retrieve the pointer for G4 materials from G4MaterialTable,
// (could trigger the conversion of the (DetDesc) Material)
//=============================================================================
G4Material* GiGaGeo::g4Material( const std::string& Name ) {

  Warning(" g4Material() is the obsolete method, use material()!");
  return material( Name );

}

//=============================================================================
// Retrive the pointer to converter volumes/assemblies
// (could trigger the conversion of the (DetDesc) LVolume/LAssembly)
//=============================================================================
GiGaVolume GiGaGeo::volume( const std::string& Name ) {
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
}


//=============================================================================
// Retrive the pointer to G4LogicalVolume  from converted volumes,
//(could trigger the conversion of the (DetDesc) LVolume)
//=============================================================================
G4LogicalVolume* GiGaGeo::g4LVolume( const std::string& Name )
{
  Warning(" g4LVolume() is the obsolete method, use volume()!");
  const GiGaVolume vol = volume( Name ) ;
  return vol.valid() ? vol.volume() : (G4LogicalVolume*) 0 ;
}


//=============================================================================
// convert (DetDesc) Solid object into (Geant4) G4VSolid object
//=============================================================================
G4VSolid*    GiGaGeo::solid ( const ISolid*      Sd     )
{

  if ( 0 == Sd) { Error(" solid():: ISolid* point to NULL"); return 0;  }

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
  /// polycone ?
  {
    const SolidPolycone* sPolycone = dynamic_cast<const SolidPolycone*>(Sd);
    if (solidType == "SolidPolycone" && 0 != sPolycone )
      {
        const int numberofz = sPolycone->number();
        double* ztemp    = new double[numberofz] ;
        double* rIntemp  = new double[numberofz] ;
        double* rOuttemp = new double[numberofz] ;

        for (int it=0;it<numberofz;it++)
          {
            ztemp    [it]=sPolycone->z(it);
            rIntemp  [it]=sPolycone->RMin(it);
            rOuttemp [it]=sPolycone->RMax(it);
          }
        G4VSolid* solid = new G4Polycone( sPolycone->name          (),
                                          sPolycone->startPhiAngle (),
                                          sPolycone->deltaPhiAngle (),
                                          numberofz                  ,
                                          ztemp                      ,
                                          rIntemp                    ,
                                          rOuttemp                   );
        delete [] ztemp    ;
        delete [] rIntemp  ;
        delete [] rOuttemp ;

        return solid;
      }
  }
  /// boolean ?
  {
    const SolidBoolean* sBool = dynamic_cast<const SolidBoolean*> (Sd);
    if( 0 != sBool ) { return g4BoolSolid( sBool ); }
  }
  /// unknown solid!
  Error( "g4solid(): Unknown Solid=" +
         solidType + "/"             +
         GiGaUtil::ObjTypeName( Sd ) +
         "/" + Sd -> name ()         ) ;
  ///
  return 0;
}


//=============================================================================
// convert (DetDesc) Solid object into (Geant4) G4VSolid object
//=============================================================================
G4VSolid*    GiGaGeo::g4Solid( const ISolid* Sd )
{
  Warning(" g4Solid() is the obsolete method, use solid()!");
  return solid( Sd ) ;
}

//=============================================================================
// convert (DetDesc) boolean solid into (Geant4) G4 solid object
//=============================================================================
///
G4VSolid*  GiGaGeo::g4BoolSolid( const SolidBoolean* Sd )
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

      // Get a Clhep matrix to use with Geant4
      HepGeom::Transform3D clhepMatrix =
        LHCb::math2clhep::transform3D( child->matrix() );

      if      ( 0 != sSub    )
        {
          double temp[3][4];

          for(int i=0;i<3;i++)
            for(int j=0;j<4;j++)
              if(fabs(clhepMatrix[i][j])<0.0000001)
                {
                  temp[i][j]=0.0;
                }
              else
                temp[i][j]=clhepMatrix[i][j];

          CLHEP::HepRep3x3 trep(temp[0][0],temp[0][1],temp[0][2],
                                temp[1][0],temp[1][1],temp[1][2],
                                temp[2][0],temp[2][1],temp[2][2]);

          CLHEP::HepRotation newrot;
          newrot.set(trep);
          HepGeom::Transform3D newtransf(newrot, CLHEP::Hep3Vector(temp[0][3],
                                                                   temp[1][3],
                                                                   temp[2][3]));
          g4total =
            new G4SubtractionSolid  ( Sd->first()->name()+"-"+child->name() ,
                                      g4total , g4child ,
                                      newtransf.inverse() ) ;

        }
      else if ( 0 != sInt    )
        { g4total =
            new G4IntersectionSolid ( Sd->first()->name()+"*"+child->name() ,
                                      g4total , g4child ,
                                      clhepMatrix.inverse() ) ; }
      else if ( 0 != sUni    )
        { g4total =
            new G4UnionSolid        ( Sd->first()->name()+"+"+child->name() ,
                                      g4total , g4child ,
                                      clhepMatrix.inverse() ) ; }
      else
        { Error("g4BoolSolid, Unknown type of Boolean solid=" +
                Sd->typeName()) ; return 0; }
    }
  ///
  g4total->SetName( Sd->name() ) ;
  ///
  return g4total;
}


//=============================================================================
// standard service initialization method
//=============================================================================
StatusCode GiGaGeo::initialize()
{
  //
  StatusCode sc = GiGaCnvSvcBase::initialize();
  if(sc.isFailure() )
    { return Error ( " Failed to initialize GiGaCnvSvc", sc      ) ; }
  // we explicitely need detSvc(), check it!
  if( 0 == detSvc() )
    { return Error ( " DetectorProviderSvc is not located! "     ) ; }

  // check for special run with  material budget counters:
  if( !m_budget.empty() )
    { Warning(" Special run for material budget calculation! " ) ; }

  // retrieve GDMLReader tools
  m_gdmlReaders.clear();
  std::vector<std::string>::iterator it;
  for (it = m_gdmlReaderNames.begin(); it != m_gdmlReaderNames.end(); ++it) {
    if ("" == (*it)) continue;
    IGDMLReader* gdmlTool = 0;
    sc = toolSvc()->retrieveTool("GDMLReader/" + (*it), gdmlTool, this);
    if (!sc.isSuccess()) return sc;
    if (gdmlTool) m_gdmlReaders.push_back(gdmlTool);
  }
  if (!m_gdmlReaders.empty()) {
    info() << "Retrieved " << m_gdmlReaders.size()
           << " GDMLReader tools" << endmsg;
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// standard service finalization method
//=============================================================================
StatusCode GiGaGeo::finalize()
{
  // manually finalize all created sensitive detectors
  { for( SDobjects::iterator imf = m_SDs.begin() ; m_SDs.end() != imf ; ++imf )
  {
      IAlgTool* mf = *imf ;
      if( 0 != mf && 0 != toolSvc() ) { toolSvc()->releaseTool( mf ) ; }
    }
  }
  m_SDs.clear() ;

  // finalize all created magnetic field objects
  { for( MFobjects::iterator imf = m_MFs.begin() ; m_MFs.end() != imf ; ++imf )
  {
      IAlgTool* mf = *imf ;
      if( 0 != mf && 0 != toolSvc() ) { toolSvc()->releaseTool( mf ) ; }
    }
  }
  m_MFs.clear() ;

  // finalize all created field managers objects
  // std::for_each( m_FMs.begin () ,
  //               m_FMs.end   () ,
  //               std::mem_fun( &IGiGaFieldMgr::release ) );
  { for( FMobjects::iterator imf = m_FMs.begin() ; m_FMs.end() != imf ; ++imf )
    {
      IAlgTool* mf = *imf ;
      if( 0 != mf && 0 != toolSvc() ) { toolSvc()->releaseTool( mf ) ; }
    }
  }
  m_FMs.clear() ;

  // clear store of assemblies!
  /* StatusCode sc = */
  GiGaAssemblyStore::store()->clear();

  // delete of volume stores
  if( m_clearStores )
    {
      G4GeometryManager* geo = G4GeometryManager::GetInstance() ;
      if      (  0 == geo                )
        { Warning(" G4GeometryManager* points to NULL!"); }
      else if (  geo -> IsGeometryClosed() )
        {
          geo -> OpenGeometry() ;
          Print( " G4 Geometry is opened " ,
                 MSG::VERBOSE , StatusCode::SUCCESS ) ;
        }
      G4LogicalVolumeStore  :: Clean     () ;
      Print( " G4LogicalVolumeStore  is cleaned "  ,
             MSG::VERBOSE , StatusCode::SUCCESS ) ;
      G4PhysicalVolumeStore :: Clean     () ;
      Print( " G4PhysicalVolumeStore is cleaned "  ,
             MSG::VERBOSE , StatusCode::SUCCESS ) ;
      // ugly trick to "hide" some erros
      //        if( 0 != geo && ! geo->IsGeometryClosed() )
      //          {
      //            geo->CloseGeometry( false , false ) ;
      //            Warning( " G4 Geometry is closed " +
      //  std::string(" (temporary trick, to be fixed soon) " ) );
      //          }
      G4SolidStore          :: Clean     () ;
      Print( " G4SolidStore          is cleaned only partially " ,
             MSG::VERBOSE , StatusCode::SUCCESS ) ;
    }

  // finalize the base class
  return GiGaCnvSvcBase::finalize();
}

//=============================================================================
// Retrieve the pointer to top-level "world" volume,
// needed for Geant4 - root for the whole Geant4 geometry tree
//=============================================================================
G4VPhysicalVolume* GiGaGeo::world ()
{
  // already created?
  if( 0 != m_worldPV ) { return m_worldPV ; } /// already created
  ///
  { MsgStream log(msgSvc(),name());
  log << MSG::DEBUG << " Create the WORLD volume!" << endreq; }
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
    new G4PVPlacement( 0 , CLHEP::Hep3Vector() , m_worldNamePV , LV , 0 , false , 0 );
  ///

  /// import GDML geometries (if any)
  std::vector<IGDMLReader*>::iterator it;
  for (it = m_gdmlReaders.begin(); it != m_gdmlReaders.end(); ++it) {
    (*it)->import(m_worldPV);
  }

  // create the magnetic field for world volume
  if( !m_worldMagField.empty() )
    {
      ///
      IGiGaFieldMgr* mf = 0;
      StatusCode sc = fieldMgr( m_worldMagField , mf );
      if( sc.isFailure() )
        { Error   ( "world():: could not construct GiGa Field Manager=" +
                    m_worldMagField, sc ) ; return 0 ; }
      if( 0 == mf        )
        { Error   ( "world():: could not construct GiGa Field Manager=" +
                    m_worldMagField, sc ) ; return 0 ; }
      if( ! mf->global() )
        { Error   ( "world():: GiGa Field Manager '" + m_worldMagField +
                    "' is not 'global'  "                   ) ; return 0 ; }
      if( 0 == mf -> fieldMgr () )
        { Error   ( "world():: GiGa Field Manager '" + m_worldMagField +
                    "' has invalid G4FieldManager  "        ) ; return 0 ; }
      if( 0 == mf -> field    () )
        { Error   ( "world():: GiGa Field Manager '" + m_worldMagField +
                    "' has invalid G4MagneticField "        ) ; return 0 ; }
      if( 0 == mf -> stepper  () )
        { Error   ( "world():: GiGa Field Manager '" + m_worldMagField +
                    "' has invalid G4MagIntegratorStepper " ) ; return 0 ; }
      /// one more check
      G4TransportationManager* trMgr =
        G4TransportationManager::GetTransportationManager () ;
      if( 0 == trMgr )
        { Error ( "world():: G4TransportationMgr is invalid!" ) ; return 0 ; }
      if( trMgr->GetFieldManager () != mf -> fieldMgr () )
        { Error ( "world():: Mismatch in the G4FieldManager!" ) ; return 0 ; }
      ///
      Print("world():: Global          'Field Manager' is set to be = " +
            GiGaUtil::ObjTypeName ( mf ) +"/" + mf -> name () ) ;
      Print("world():: Global        'G4Field Manager' is set to be = " +
            GiGaUtil::ObjTypeName ( mf -> fieldMgr () )          ) ;
      Print("world():: Global        'G4MagneticField' is set to be = " +
            GiGaUtil::ObjTypeName ( mf -> field    () )          ) ;
      Print("world():: Global 'G4MagIntegratorStepper' is set to be = " +
            GiGaUtil::ObjTypeName ( mf -> stepper  () )          ) ;
    }
  else { Warning("world():: Magnetic Field is not requested to be loaded "); }
  ///
  return m_worldPV ;
}

//=============================================================================
// Retrieve the pointer to top-level "world" volume,
// needed for Geant4 - root for the whole Geant4 geometry tree
//=============================================================================
G4VPhysicalVolume* GiGaGeo::G4WorldPV()
{
  Warning(" G4WorldPV() is the obsolete method, use world()!");
  return world() ;
}


//=============================================================================
// Convert the transient object to the requested representation.
//  e.g. conversion to persistent objects.
//=============================================================================
StatusCode GiGaGeo::createRep
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
}

//=============================================================================
// Resolve the references of the converted object.
// After the requested representation was created the references in this
// representation must be resolved.
//=============================================================================
StatusCode GiGaGeo::fillRepRefs
( IOpaqueAddress* address ,
  DataObject*     object  )
{
  ///
  if( 0 == object )
    { return Error(" fillRepRefs:: DataObject* points to NULL!");}
  ///
  const IDetectorElement* de = dynamic_cast<IDetectorElement*> ( object ) ;
  IConverter* cnv =
    converter( 0 == de ? object->clID() : CLID_DetectorElement );
  if( 0 == cnv )
    { return Error(" fillRepRefs:: converter is not found for '"
                   + object->registry()->identifier() + "'" );}
  ///
  return cnv->fillRepRefs( address , object );
}


//=============================================================================
// Update the converted representation of a transient object.
//=============================================================================
StatusCode GiGaGeo::updateRep
( IOpaqueAddress* address ,
  DataObject*     object  )
{
  ///
  if( 0 == object )
    { return Error(" updateRep:: DataObject* points to NULL!");}
  ///
  const IDetectorElement* de = dynamic_cast<IDetectorElement*> ( object ) ;
  IConverter* cnv =
    converter( 0 == de ? object->clID() : CLID_DetectorElement );
  if( 0 == cnv )
    { return Error(" updateRep:: converter is not found for '"
                   + object->registry()->identifier() + "'" );}
  ///
  return cnv->updateRep( address , object );
}


//=============================================================================
// Update the references of an already converted object.
//  The object must be retrieved before it can be updated.
//=============================================================================
StatusCode GiGaGeo::updateRepRefs
( IOpaqueAddress* address ,
  DataObject*     object  )
{
  ///
  if( 0 == object )
    { return Error(" updateRepRefs:: DataObject* points to NULL!");}
  ///
  const IDetectorElement* de = dynamic_cast<IDetectorElement*> ( object ) ;
  IConverter* cnv =
    converter( 0 == de ? object->clID() : CLID_DetectorElement );
  if( 0 == cnv )
    { return Error(" updateRepRefs:: converter is not found for '"
                   + object->registry()->identifier() + "'" );}
  ///
  return cnv->updateRepRefs( address , object );
}


//=============================================================================
StatusCode GiGaGeo::queryInterface( const InterfaceID& iid , void** ppI )
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
  else if ( INamedInterface ::interfaceID ()  == iid )
    { *ppI  = static_cast<INamedInterface*>   ( this )   ; }
  else if ( IInterface      ::interfaceID ()  == iid )
    { *ppI  = static_cast<IInterface*>        ( this )   ; }
  else
    { return GiGaCnvSvcBase::queryInterface( iid , ppI ) ; }
  ///
  addRef();
  ///
  return StatusCode::SUCCESS;
  ///
}


//=============================================================================
// Instantiate the Sensitive Detector Object
//=============================================================================
StatusCode   GiGaGeo::sensitive
( const std::string& name  ,
  IGiGaSensDet*&     det   )
{
  // reset the output value
  det = 0 ;
  // locate the detector
  det = tool( name , det , this );
  if( 0 == det )
    { return Error( "Could not locate Sensitive Detector='" + name + "'" ) ; }
  // inform Geant4 sensitive detector manager
  if( m_SDs.end() == std::find( m_SDs.begin() , m_SDs.end  () , det ) )
    {
      G4SDManager* SDman = G4SDManager::GetSDMpointer();
      if( 0 == SDman ) { return Error( "Could not locate G4SDManager" ) ; }
      SDman -> AddNewDetector( det );
    }
  // keep local copy
  m_SDs.push_back( det );
  ///
  return StatusCode::SUCCESS;
}


//=============================================================================
// Instantiate the Sensitive Detector Object
//=============================================================================
StatusCode GiGaGeo::sensDet
( const std::string& TypeNick ,
  IGiGaSensDet*&     SD )
{
  Warning(" sensDet() is the obsolete method, use sensitive()!");
  return sensitive( TypeNick , SD ) ;
}

//=============================================================================
// Instantiate the Magnetic Field Object
//=============================================================================
StatusCode   GiGaGeo::magnetic
( const std::string& name ,
  IGiGaMagField*&    mag  )
{
  Warning(" magnetic() is the obsolete method, use fieldMgr()!");
  // reset the output value
  mag = 0 ;
  // locate the magnetic field
  mag = tool( name , mag , this );
  if( 0 == mag )
    { return Error( "Could not locate Magnetic Field='" + name + "'" ) ; }
  // keep local copy
  m_MFs.push_back( mag );
  ///
  return StatusCode::SUCCESS;
  ///
}

//=============================================================================
// Instantiate the Magnetic Field Object
//=============================================================================
StatusCode   GiGaGeo::fieldMgr
( const std::string& name ,
  IGiGaFieldMgr*&    mgr  )
{
  // reset the output value
  mgr = 0 ;
  // locate the magnetic field

//   std::cout << "GiGaGeo::fieldMgr Arguments passed " << name << " " << mgr
//             << " " << this << std::endl;
//   StatusCode sc = toolSvc()-> retrieveTool( name, mgr, this, true );
//   if( sc.isSuccess() ) std::cout << "Success " << std::endl;
  mgr = tool( name , mgr , this );
  if( 0 == mgr )
    { return Error( "Could not locate Field Manager'" + name + "'" ) ; }
  // keep local copy
  m_FMs.push_back( mgr );
  ///
  return StatusCode::SUCCESS;
  ///
}


//=============================================================================
// Instantiate the Magnetic Field Object
//=============================================================================
StatusCode GiGaGeo::magField
( const std::string& TypeNick ,
  IGiGaMagField*&    MF       )
{
  Warning(" magField() is the obsolete method, use fieldMgr()!");
  return magnetic( TypeNick , MF ) ;
}


//=============================================================================
// Create new G4LogicalVolume. All arguments must be valid!
// One should not invoke the "new" operator for Logical Volumes directly
//=============================================================================
G4LogicalVolume* GiGaGeo::createG4LV
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
}











