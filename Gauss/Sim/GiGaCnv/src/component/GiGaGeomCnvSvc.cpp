// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Sim/GiGaCnv/src/component/GiGaGeomCnvSvc.cpp,v 1.2 2001-04-26 21:01:41 ibelyaev Exp $
#include <string>
#include <algorithm>
// from Gaudi 
#include "GaudiKernel/AddrFactory.h" 
#include "GaudiKernel/SvcFactory.h" 
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IDataSelector.h"
#include "GaudiKernel/IConverter.h"
#include "GaudiKernel/IObjManager.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/DataObject.h"
// from DetDesc 
#include "DetDesc/DetectorElement.h"
#include "DetDesc/Solids.h"
// G4Wrapper
#include "G4Wrapper/Material.h"
#include "G4Wrapper/PhysicalVolume.h"
#include "G4Wrapper/LogicalVolume.h"
#include "G4Wrapper/Transportation.h"
#include "G4Wrapper/Solids.h"
// Include G4 Stuff
#include "G4VSolid.hh"
#include "G4VisAttributes.hh"
// from GiGa 
#include "GiGa/IGiGaSensDet.h"
#include "GiGa/IGiGaSensDetFactory.h"
#include "GiGa/IGiGaMagField.h"
#include "GiGa/IGiGaMagFieldFactory.h"
#include "GiGa/SplitTypeAndName.h"
// local 
#include "GiGaGeomCnvSvc.h" 


/**  Implementation of GiGaGeomCnvSvc 
     @author Vanya Belyaev
     @date   7 Aug 2000
*/

/// factory 
extern const IAddrFactory& GiGaGeomAddressFactory ; 

/// factory 
static const  SvcFactory<GiGaGeomCnvSvc>                         s_GiGaGeomCnvSvcFactory ; 
const        ISvcFactory&                GiGaGeomCnvSvcFactory = s_GiGaGeomCnvSvcFactory ; 

/// constructor
GiGaGeomCnvSvc::GiGaGeomCnvSvc( const std::string&   ServiceName          , 
                                  ISvcLocator*         ServiceLocator       ) 
  : GiGaCnvSvcBase(                                  ServiceName          , 
                                                     ServiceLocator       , 
                                                     GiGaGeom_StorageType )
  , m_worldPV       ( 0                   ) 
  , m_worldNamePV   ( "Universe"          )
  , m_worldNameLV   ( "World"             )
  , m_worldMaterial ( "/dd/Materials/Air" )
  , m_worldX        ( 50. * m             )
  , m_worldY        ( 50. * m             )
  , m_worldZ        ( 50. * m             )
  ///
  , m_worldMagField ( ""                  )
  ///
  , m_SDs           ()
  , m_SDFs          ()
  , m_MFs           ()
  , m_MFFs          ()
  ///
{
  ///
  setAddressFactory(&GiGaGeomAddressFactory) ;
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
  declareProperty("WorldMagneticField"        , m_worldMagField );
  ///
};
///
G4Material*    GiGaGeomCnvSvc::g4Material( const std::string& Name )
{
  /// first look throught MaterialTable
  {
    G4Material* mat = G4Wrapper::getG4Material( Name ) ;
    if( 0 != mat ) { return mat ; } 
  }
  /// retrieve material by name and convert it to G4 representation  
  {
    SmartDataPtr<DataObject>  so( detSvc() , Name ); 
    DataObject* Obj = so.operator->();  
    if( 0 == Obj       ) { Error("Failed to locate DataObject with name:"+Name) ; return 0 ;  } 
    IDataSelector dS; dS.push_back( Obj ) ; 
    StatusCode sc = createReps( &dS );
    if( sc.isFailure() ) { Error("Failed to create G4 representation of Material="+Name,sc) ; return 0 ; }
  }
  /// look throught G4MaterialTable
  {
    G4Material* mat = G4Wrapper::getG4Material( Name ) ;
    if( 0 != mat ) { return mat ; } 
  }
  ///
  Error("Failed to find G4Material with name="+Name);
  ///
  return 0;     
};
/// 
G4LogicalVolume* GiGaGeomCnvSvc::g4LVolume( const std::string& Name )
{
  /// first look through G4LogicalVolumeStore
  {
    G4LogicalVolume* lv = G4Wrapper::getG4LogicalVolume( Name ) ; 
    if( 0 != lv ) { return lv ; }                                       ///< RETURN !!!
  }
  /// locate the object in the transient store and convert it! 
  {
    SmartDataPtr<DataObject>  so( detSvc() , Name ); 
    DataObject* Obj = so.operator->();  
    if( 0 == Obj       ) { Error("Failed to locate DataObject with name:"+Name) ;  return 0 ; } 
    ///
    IDataSelector dS; dS.push_back( Obj ) ; 
    StatusCode sc = createReps( &dS );
    if( sc.isFailure() ) { Error("Failed to create G4 representation of LOgicalVolume="+Name,sc); return 0 ; }
  } 
  /// again look through G4LogicalVolumeStore
  {
    G4LogicalVolume* lv = G4Wrapper::getG4LogicalVolume( Name ) ; 
    if( 0 != lv ) { return lv ; }                                       ///< RETURN !!!
  }
  ///
  Error("Failed to find G4LogicalVolume with name="+Name);
  ///
  return 0;      
};
///
G4VSolid*    GiGaGeomCnvSvc::g4Solid( const ISolid* Sd )
{
  ///
  if ( 0 == Sd) { Error("g4Solid ISolid* point to NULL"); return 0;  }  /// RETURN !!!
  ///
  const std::string solidType( Sd->typeName() ); 
  ///
  /// box?  
  {
    const SolidBox* sBox = dynamic_cast<const SolidBox*>(Sd);
    if( solidType == "SolidBox" && 0 != sBox ) 
      { return G4Wrapper::createG4Box( sBox->name(),
				       sBox->xHalfLength(),
				       sBox->yHalfLength(),
				       sBox->zHalfLength()); } 
  }
  /// cons?
  {
    const SolidCons* sCons = dynamic_cast<const SolidCons*>(Sd);
    if( solidType == "SolidCons" && 0 != sCons ) 
      { return G4Wrapper::createG4Cons( sCons->name(),
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
      { return G4Wrapper::createG4Sphere( sSphere->name            (),
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
      { return G4Wrapper::createG4Trd( sTrd->name(),
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
      { return G4Wrapper::createG4Tubs( sTubs->name(),
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
      { return G4Wrapper::createG4Trap( sTrap->name            (),
					sTrap->zHalfLength     (),
					sTrap->theta           (),
					sTrap->phi             (),
					sTrap->dyAtMinusZ      (),
					sTrap->dxAtMinusZMinusY(),
					sTrap->dxAtMinusZPlusY (),
					sTrap->alphaAtMinusZ   (),
					sTrap->dyAtPlusZ       (),
					sTrap->dxAtPlusZMinusY (),
					sTrap->dxAtPlusZPlusY  (),
					sTrap->alphaAtPlusZ    () ); }
  }
  /// boolean ? 
  {
    const SolidBoolean* sBool = dynamic_cast<const SolidBoolean*> (Sd);
    if( 0 != sBool ) { return g4BoolSolid( sBool ); }
  }
  /// unknown solid!
  Error("g4solid Unknown Solid="+solidType+"/"+System::typeinfoName(typeid(Sd))+"/"+Sd->name());
  ///
  return 0;
};
///
G4VSolid*  GiGaGeomCnvSvc::g4BoolSolid( const SolidBoolean* Sd ) 
{
  ///
  if( 0 == Sd )
    { Error("g4BoolSolid, SolidBoolean* point to NULL!")                                     ; return 0; }
  const SolidSubtraction*  sSub = dynamic_cast<const SolidSubtraction*>  ( Sd );
  const SolidIntersection* sInt = dynamic_cast<const SolidIntersection*> ( Sd );
  const SolidUnion*        sUni = dynamic_cast<const SolidUnion*>        ( Sd );
  if( 0 == sSub && 0 == sInt && 0 == sUni ) 
    { Error("g4BoolSolid, Unknown type of Boolean solid="+Sd->typeName())                    ; return 0; }
  if( 0 == Sd->first() ) 
    { Error("g4BoolSolid, Wrong MAIN/FIRST solid for Boolean solid="+Sd->name())             ; return 0; }
  ///
  G4VSolid* first = g4Solid( Sd->first() ); 
  if( 0 == first      )
    { Error("g4BoolSolid, could not convert MAIN/FIRST solid for Boolean solid="+Sd->name()) ; return 0; }
  ///
  G4VSolid* g4total = first;
  typedef SolidBoolean::SolidChildrens::const_iterator CI;
  for( CI it = Sd->childBegin() ; Sd->childEnd() != it ; ++it )
    {
      const SolidChild* child = *it ; 
      G4VSolid* g4child = g4Solid( child->solid() );
      if( 0 == g4child ) 
        { Error("g4BoolSolid, could not convert CHILD solid for Boolean solid="+Sd->name())  ; return 0; }
      if      ( 0 != sSub    ) 
        { g4total = G4Wrapper::createG4SubtractionSolid  ( Sd->first()->name()+"-"+child->name() , g4total , g4child , child->matrix().inverse() ) ; }
      else if ( 0 != sInt    )
        { g4total = G4Wrapper::createG4IntersectionSolid ( Sd->first()->name()+"*"+child->name() , g4total , g4child , child->matrix().inverse() ) ; }
      else if ( 0 != sUni    )
        { g4total = G4Wrapper::createG4UnionSolid        ( Sd->first()->name()+"+"+child->name() , g4total , g4child , child->matrix().inverse() ) ; }
      else
        { Error("g4BoolSolid, Unknown type of Boolean solid="+Sd->typeName())                 ; return 0; }
    } 
  ///
  g4total->SetName( Sd->name() ) ;
  ///
  return g4total;
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaGeomCnvSvc::initialize() 
{ 
  ///
  StatusCode sc = GiGaCnvSvcBase::initialize();
  if(sc.isFailure() ) { return Error(" Failed to initialize GiGaCnvSvc", sc  ) ;}
  /// we explicitely need detSvc(), check it! 
  if( 0 == detSvc() ) { return Error(" DetectorProviderSvc is not located! " ) ;}
  ///
  return sc;
  ///
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaGeomCnvSvc::finalize()   
{ return GiGaCnvSvcBase::finalize(); };
////////////////////////////////////////////////////////////////////////////////////////////////////////////
G4VPhysicalVolume* GiGaGeomCnvSvc::G4WorldPV() 
{
  /// already created? 
  if( 0 != m_worldPV ) { return m_worldPV ; } /// already created 
  ///
  { MsgStream log(msgSvc(),name()); log << MSG::INFO << " Create the WORLD volume!" << endreq; } 
  /// create it!
  G4Material* MAT = g4Material ( m_worldMaterial );   
  if( 0 == MAT ) { Error("G4WorldPV:: could not locate/convert material="+m_worldMaterial) ; return 0 ; }
  ///
  G4VSolid*        SD = G4Wrapper::createG4Box("WorldBox", m_worldX , m_worldY , m_worldZ ) ; 
  ///
  G4LogicalVolume* LV = G4Wrapper::createG4LogicalVolume( SD , MAT , m_worldNameLV ); 
  /// make it invisible 
  LV -> SetVisAttributes ( G4VisAttributes::Invisible );
  ///
  m_worldPV           = G4Wrapper::createG4PVPlacement( HepTransform3D() , LV , m_worldNamePV , 0 , false , 0 );
  //  m_worldPV           = G4Wrapper::createG4PVPlacement( 0 , Hep3Vector() , m_worldNamePV , LV , 0 , false , 0 );
  ///
  /// create the magnetic field for world volume
  if( !m_worldMagField.empty() )
    {
      ///
      IGiGaMagField* mf = 0;
      StatusCode sc = magField( m_worldMagField , mf );  
      if( sc.isFailure() ) { Error("G4WorldPV:: could not construct Global Magnetic Field="+m_worldMagField, sc ) ; return 0 ; }  
      if( 0 == mf        ) { Error("G4WorldPV:: could not construct Global Magnetic Field="+m_worldMagField, sc ) ; return 0 ; }  
      ///
      bool res1 = G4Wrapper::setDetectorField ( mf ) ;
      if( !res1 ) { Error("G4WorldPV:: could not set detector field! ") ; return 0 ; } 
      bool res2 = G4Wrapper::createChordFinder( mf ) ;
      if( !res2 ) { Error("G4WorldPV:: could not create chord finder!") ; return 0 ; } 
      ///
      Print("G4WorlPV:: World Magnetic Field is set to be = "+ System::typeinfoName( typeid( *mf ) )+"/"+mf->name() );
    }
  else { Warning("G4WorldPV:: Global Magnetic Field is not requetsed to be loaded "); }
  ///
  return m_worldPV ; 
}; 
/// Convert a collection of transient data objects into another representation ///////////////////////////////////////////
StatusCode GiGaGeomCnvSvc::createReps(IDataSelector* pSelector)  
{
  Context::iterator       i;
  IDataSelector::iterator j;
  Context                 context;
  StatusCode              status = NO_CONVERTER;
  StatusCode              iret   = StatusCode::SUCCESS;
  // Create context 
  for ( j = pSelector->begin(); j != pSelector->end(); j++ )   
    {    
      // check if it is a Det Elem.
      DetectorElement* de = 0 ;
      try        { de = dynamic_cast<DetectorElement*>(*j); } 
      catch(...) { de = 0 ;}
      IConverter*    cnv = converter( 0 != de ? de->clID() : (*j)->clID() );
      context.push_back(ContextEntry(cnv, 0, (*j)));
      if ( 0 == cnv )   { iret = NO_CONVERTER; }
    }
  // Update primitive representation
  for ( i = context.begin(); i != context.end(); i++ )   {
    ContextEntry& e = (*i);
    if ( 0 != e.converter() )  {
      status = e.converter()->createRep(e.object(), e.address());
      if ( status.isSuccess() )  {
        status = m_dataSvc->updateRegistryEntry(e.address(), e.object());
        if ( !status.isSuccess() )  {
          iret = status;
        }
      }
      else  {
        iret = status;
      }
    }
  }
  // Finally update local representation references which are unknown to the data model
  for ( i = context.begin(); i != context.end(); i++ )   {
    ContextEntry& e = (*i);
    // e.address()==0 -> representation was NOT created in the first go!
    if ( 0 != e.converter() && 0 != e.address() )  {
      status = e.converter()->fillRepRefs(e.address(), e.object());
      if ( !status.isSuccess() )  {
        iret = status;
      }
    }
  }
  return iret;
}
///////////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaGeomCnvSvc::queryInterface( const IID& iid , void** ppI )
{ 
  ///
  if  ( 0 == ppI                    ) { return StatusCode::FAILURE                     ; } 
  *ppI = 0 ; 
  if  ( IGiGaGeomCnvSvc::interfaceID()  == iid ) { *ppI  = static_cast<IGiGaGeomCnvSvc*> (this)   ; } 
  else                                           { return GiGaCnvSvcBase::queryInterface( iid , ppI ) ; } 
  ///
  addRef();
  ///
  return StatusCode::SUCCESS;  
  ///
};
///////////////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaGeomCnvSvc::sensDet( const std::string& TypeNick , IGiGaSensDet*& SD )
{
  ///
  SD = 0 ; /// reset the output value 
  ///
  std::string Type,Nick;
  StatusCode sc = SplitTypeAndName( TypeNick , Type, Nick ); 
  if( sc.isFailure() ) { return Error("Could not interprete name of SensDet="+TypeNick, sc ) ; }
  /// look at the local storage:
  for( SDobjects::const_iterator it = m_SDs.begin() ; m_SDs.end() != it ; ++it ) 
    {
      if( 0 != *it && (*it)->name() == Nick && System::typeinfoName( typeid( *it ) ) == Type ) 
        { (*it)->addRef() ; SD = *it ; return StatusCode::SUCCESS ; }  
    } 
  /// look at local storage of factories: 
  const IGiGaSensDetFactory* SDfac = 0 ; 
  for( SDfactories::const_iterator fit = m_SDFs.begin() ; m_SDFs.end() != fit ; ++fit )
    { if( 0 != *fit  && (*fit)->ident() == Type ) { SDfac = *fit ; break ; } }
  /// 
  if( 0 == SDfac )   
    {
      if( 0 == objMgr()                       ) { return Error("IObjectManager*    points to NULL!"  ); }
      if( !objMgr()->existsObjFactory( Type ) ) { return Error("Could not locate  factory for "+Type ); }
      const IFactory* fac = objMgr()->objFactory( Type ); 
      if( 0 == fac                            ) { return Error("Could not locate  factory for "+Type ); }
      SDfac = dynamic_cast<const IGiGaSensDetFactory*> (fac) ; 
      if( 0 == SDfac                          ) { return Error("Could not cast the actory for "+Type ); }
      m_SDFs.push_back( SDfac );
    }
  /// 
  SD = SDfac->instantiate( Nick , serviceLocator() );
  ///
  StatusCode st = SD->initialize(); 
  if( st.isFailure() ) { Error("Could Not Initialize the SD Object "+TypeNick, st ); delete SD ; SD = 0 ; } 
  ///
  return StatusCode::SUCCESS;
  ///
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaGeomCnvSvc::magField( const std::string& TypeNick , IGiGaMagField*& MF )
{
  ///
  MF = 0 ; /// reset the output value 
  ///
  std::string Type,Nick;
  StatusCode sc = SplitTypeAndName( TypeNick , Type, Nick ); 
  if( sc.isFailure() ) { return Error("Could not interprete name of MagField="+TypeNick, sc ) ; }
  /// look at the local storage:
  for( MFobjects::const_iterator it = m_MFs.begin() ; m_MFs.end() != it ; ++it ) 
    {
      if( 0 != *it && (*it)->name() == Nick && System::typeinfoName( typeid( *it ) ) == Type ) 
        { (*it)->addRef() ; MF = *it ; return StatusCode::SUCCESS ; }  
    } 
  /// look at local storage of factories: 
  const IGiGaMagFieldFactory* MFfac = 0 ; 
  for( MFfactories::const_iterator fit = m_MFFs.begin() ; m_MFFs.end() != fit ; ++fit )
    { if( 0 != *fit  && (*fit)->ident() == Type ) { MFfac = *fit ; break ; } }
  /// 
  if( 0 == MFfac )   
    {
      if( 0 == objMgr()                       ) { return Error("IObjectManager*    points to NULL!"  ); }
      if( !objMgr()->existsObjFactory( Type ) ) { return Error("Could not locate  factory for "+Type ); }
      const IFactory* fac = objMgr()->objFactory( Type ); 
      if( 0 == fac                            ) { return Error("Could not locate  factory for "+Type ); }
      MFfac = dynamic_cast<const IGiGaMagFieldFactory*> (fac) ; 
      if( 0 == MFfac                          ) { return Error("Could not cast the actory for "+Type ); }
      m_MFFs.push_back( MFfac );
    }
  /// 
  MF = MFfac->instantiate( Nick , serviceLocator() );
  ///
  StatusCode st = MF->initialize(); 
  if( st.isFailure() ) { delete MF ; MF = 0 ; return Error("Could not Initialize the MagField Object="+TypeNick, st ); } 
  ///
  return StatusCode::SUCCESS;
  ///
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
















