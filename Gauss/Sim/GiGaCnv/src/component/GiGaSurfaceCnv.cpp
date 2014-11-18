// $Id: GiGaSurfaceCnv.cpp,v 1.15 2009-10-15 10:02:15 silviam Exp $
#define GIGACNV_GIGASURFACECNV_CPP 1 
// ============================================================================
// GaudiKernel
#include "GaudiKernel/CnvFactory.h" 
#include "GaudiKernel/IAddressCreator.h" 
#include "GaudiKernel/IOpaqueAddress.h" 
#include "GaudiKernel/MsgStream.h"
// DetDesc 
#include "DetDesc/Surface.h"
// GiGa 
#include  "GiGa/GiGaException.h" 
#include  "GiGa/IGiGaSetUpSvc.h" 
// GiGaCnv 
#include  "GiGaCnv/GiGaVolumeUtils.h" 
// Geant4
#include  "G4LogicalVolume.hh"
#include  "G4VPhysicalVolume.hh"
#include  "G4OpticalSurface.hh"
#include  "G4LogicalSurface.hh"
#include  "G4LogicalSkinSurface.hh"
#include  "G4LogicalBorderSurface.hh"
// local 
#include "AddTabulatedProperties.h"
#include "GiGaSurfaceCnv.h"

// ============================================================================
/** @file
 * 
 *  implementation of class GiGaSurfaceCnv 
 *  
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date xx/xx/xxxx
 */
// ============================================================================

// ============================================================================
/** mandatory factory busines 
 */
// ============================================================================
DECLARE_CONVERTER_FACTORY( GiGaSurfaceCnv )

// ============================================================================
/** constructor 
 */
// ============================================================================
GiGaSurfaceCnv::GiGaSurfaceCnv( ISvcLocator* Locator ) 
  : GiGaCnvBase( storageType() , classID() , Locator ) 
  , m_surfaces()
  , m_leaf ( "" , classID() ) 
{
  setNameOfGiGaConversionService( IGiGaCnvSvcLocation::Geo ) ; 
  setConverterName              ( "GiGaSurfaceCnv"         ) ; 
}

// ============================================================================
/// destructor 
// ============================================================================
GiGaSurfaceCnv::~GiGaSurfaceCnv(){}

// ============================================================================
/// Class ID //
// ============================================================================
const CLID&  GiGaSurfaceCnv::classID() { 
  return Surface::classID() ; 
}

// ============================================================================
/// StorageType
// ============================================================================
unsigned char GiGaSurfaceCnv::storageType () { 
  return GiGaGeom_StorageType; 
} 

// ============================================================================
/// Create representation 
// ============================================================================
StatusCode GiGaSurfaceCnv::createRep
( DataObject*      Object  , 
  IOpaqueAddress*& Address ) 
{
  ///
  Address = 0 ; 
  ///
  if( 0 == Object      ) 
    { return Error("CreateRep::DataObject* points to NULL"); } 
  Surface* obj = 0 ; 
  try        { obj = dynamic_cast<Surface*>( Object ) ; } 
  catch(...) { obj =                                        0 ; } 
  if( 0 == obj         )
    { return Error("CreateRep::Bad cast to Surface*"); }
  if( 0 == cnvSvc()    ) 
    { return Error("CreateRep::Conversion Service is unavailable"); } 
  /// create IOpaqueAddress
  IAddressCreator* addrCreator = addressCreator() ;
  if( 0 == addrCreator ) 
    { return Error("CreateRep::Address Creator is unavailable"); } 
  StatusCode st = 
    addrCreator->createAddress( repSvcType  () , 
                                classID     () , 
                                m_leaf.par  () , 
                                m_leaf.ipar () , Address );   
  if( st.isFailure()   )
    { return Error("CreateRep::Error in Address Creation", st); }
  if( 0 == Address     ) 
    { return Error("CreateRep::Error Address is created"        ); }
  ///
  return updateRep( Object , Address ) ; 
  /// 
}

// ============================================================================
/** update the representation
 */
// ============================================================================
StatusCode GiGaSurfaceCnv::updateRep
( DataObject*        Object  , 
  IOpaqueAddress* /* Address */ ) 
{
  ///
  { MsgStream log( msgSvc() , name() ); 
  log << MSG::VERBOSE << "UpdateRep::start" << endreq; } 
  ///
  if( 0 == Object    ) 
    { return Error("UpdateRep::DataObject* points to NULL"); } 
  Surface* surface = dynamic_cast<Surface*>( Object ) ;  
  if( 0 == surface   ) 
    { return Error("UpdateRep::Bad cast to Surface*"); }
  if( 0 == cnvSvc()  ) 
    { return Error("UpdateRep::Conversion Service is unavailable"); } 
  const IRegistry* registry = Object->registry();
  if( 0 == registry ) 
    { return Error("UpdateRep::IRegistry* is NULL!"); }
  const std::string& surfaceName = registry->identifier();
  ///
  G4LogicalSurface* logSurf = 0 ;
  StatusCode         status = StatusCode::SUCCESS ; 
  if      ( surface->firstVol  ().empty () ) 
    { status = Error("UpdateRep: the firstVol name is EMPTY!") ; }
  else if ( surface->secondVol ().empty () )           /// SKIN   surface 
    { status = createSkinSurface  (  surface   , logSurf     ) ; } 
  else                                                 /// BORDER surface
    { status = createBorderSurface(  surface   , logSurf     ) ; } 
  ///
  if( status.isFailure() || 0 == logSurf ) 
    { return Error("Could not create G4LogicalSurface!", status ) ;} 
  ///
  if( 0 == logSurf->GetSurfaceProperty() )
    {    
      /// create optical surface 
      G4OpticalSurface* optSurf = 
        new G4OpticalSurface( surfaceName                                    ,
                              ( G4OpticalSurfaceModel  ) surface->model   () ,
                              ( G4OpticalSurfaceFinish ) surface->finish  () ,
                              ( G4SurfaceType   ) surface->type    () ,
                              surface->value                              () );
      ///
      if( 0 == optSurf->GetMaterialPropertiesTable() ) 
        { optSurf-> 
            SetMaterialPropertiesTable( new G4MaterialPropertiesTable() ) ; }
      StatusCode sc = 
        AddTabulatedProperties ( surface->tabulatedProperties        ()  ,
                                 optSurf->GetMaterialPropertiesTable ()  ) ;
      if( sc.isFailure() ) 
        { return Error("Could not set properties forG4OpticalSurface!") ; }
      /// 
      logSurf->SetSurfaceProperty( optSurf ); 
    }
  ///
  ///
  return StatusCode::SUCCESS;
}

// ============================================================================
//
// ============================================================================
StatusCode GiGaSurfaceCnv::createSkinSurface
( const Surface    *  surface , 
  G4LogicalSurface *& logsurf )
{ 
  ///
  logsurf = 0 ; 
  if( 0 == surface ) 
    { return Error("CreateSkinSurface: Surface* points to NULL") ; }
  /// first look through G4LogicalVolumeStore
  G4LogicalVolume* lv = GiGaVolumeUtils::findLVolume( surface->firstVol() ) ;
  if( 0 == lv ) 
    { return Error("Could not locate G4LogicalVolume by name '" + 
                   surface->firstVol()+"'" ); }
  /// look through existing LogicalSkinSurfaces
  G4LogicalSkinSurface* surf = G4LogicalSkinSurface::GetSurface( lv ); 
  if ( 0 != surf ) {
    std::string G4surfName = surf->GetName();
    if( G4surfName != surface->registry()->identifier() ) {
      return Error("UpdateRep: surface with this logvol exists!");
    }
  }
  
  /// create new surface 
  if( 0 == surf ) 
    { logsurf = new G4LogicalSkinSurface( surface->registry()->identifier() , 
                                          lv , 0 ); }
  else            
    { logsurf = surf ; }  
  ///
  return StatusCode::SUCCESS;
}

// ============================================================================
//
// ============================================================================
StatusCode GiGaSurfaceCnv::createBorderSurface
( const Surface    *  surface , 
  G4LogicalSurface *& logsurf )
{
  ///
  logsurf = 0 ;
  /// locate PhysVolumes
  G4VPhysicalVolume* pv1 = 
    GiGaVolumeUtils::findPVolume( surface->firstVol  () );
  if( 0 == pv1 ) { return Error("Could not locate physical volume '" 
                                + surface->firstVol  () + "'" ) ; }
  G4VPhysicalVolume* pv2 = 
    GiGaVolumeUtils::findPVolume( surface->secondVol () );
  if( 0 == pv2 ) { return Error("Could not locate physical volume '" 
                                + surface->secondVol () + "'" ) ; }
  /// locate Logical Border Surface
  G4LogicalBorderSurface* surf = 
    G4LogicalBorderSurface::GetSurface( pv1 , pv2 );
  if ( 0 != surf ) {
    std::string G4surfName = surf->GetName();
    if( G4surfName != surface->registry()->identifier() ) {
      return Error("UpdateRep: surface with this pv1/pv2 lready exists!") ;
    }
  }
  
  ///
  if( 0 == surf ) 
    { logsurf = new G4LogicalBorderSurface( surface->registry()->identifier() , 
                                            pv1 , pv2  , 0 ); }
  else            
    { logsurf = surf ; }  
  ///  
  return StatusCode::SUCCESS;
}

// ============================================================================
// The End 
// ============================================================================











