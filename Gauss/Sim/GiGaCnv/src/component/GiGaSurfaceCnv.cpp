// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Sim/GiGaCnv/src/component/GiGaSurfaceCnv.cpp,v 1.1.1.1 2001-04-23 08:34:15 ibelyaev Exp $ 
// GaudiKernel
#include "GaudiKernel/CnvFactory.h" 
#include "GaudiKernel/IAddressCreator.h" 
#include "GaudiKernel/IOpaqueAddress.h" 
#include "GaudiKernel/IGiGaSetUpSvc.h" 
#include "GaudiKernel/MsgStream.h"
// DetDesc 
#include "DetDesc/Surface.h"
// GiGa 
#include "GiGa/GiGaException.h" 
// Geant4
#include  "G4LogicalVolume.hh"
#include  "G4LogicalVolumeStore.hh"
#include  "G4VPhysicalVolume.hh"
#include  "G4PhysicalVolumeStore.hh"
#include  "G4OpticalSurface.hh"
#include  "G4LogicalSurface.hh"
#include  "G4LogicalSkinSurface.hh"
#include  "G4LogicalBorderSurface.hh"
// local 
#include "AddTabulatedProperties.h"
#include "GiGaSurfaceCnv.h" 


//////////////////////////////////////////////////////////////////////////////////////////
static const  CnvFactory<GiGaSurfaceCnv>                         s_GiGaSurfaceCnvFactory ;
const        ICnvFactory&                GiGaSurfaceCnvFactory = s_GiGaSurfaceCnvFactory ;
/// constructor //////////////////////////////////////////////////////////////////////////
GiGaSurfaceCnv::GiGaSurfaceCnv( ISvcLocator* Locator ) 
  : GiGaCnvBase( storageType() , classID() , Locator ) 
  , m_surfaces()
{
  setNameOfGiGaConversionService( "GiGaGeomCnvSvc" ); 
  setConverterName              ( "GiGaSurfaceCnv" ); 
}; 
/// destructor ///////////////////////////////////////////////////////////////////////////
GiGaSurfaceCnv::~GiGaSurfaceCnv(){}; 
/// Class ID /////////////////////////////////////////////////////////////////////////////
const CLID&  GiGaSurfaceCnv::classID            () { return Surface::classID() ; }
/// StorageType //////////////////////////////////////////////////////////////////////////
const unsigned char GiGaSurfaceCnv::storageType () { return GiGaGeom_StorageType; } 
/// Create representation ////////////////////////////////////////////////////////////////
StatusCode GiGaSurfaceCnv::createRep( DataObject*     Object  , IOpaqueAddress*& Address ) 
{
  ///
  Address = 0 ; 
  ///
  if( 0 == Object      ) { return Error("CreateRep::DataObject* points to NULL"); } 
  Surface* obj = 0 ; 
  try        { obj = dynamic_cast<Surface*>( Object ) ; } 
  catch(...) { obj =                                        0 ; } 
  if( 0 == obj         ) { return Error("CreateRep::Bad cast to Surface*"); }
  if( 0 == cnvSvc()    ) { return Error("CreateRep::Conversion Service is unavailable"); } 
  /// create IOpaqueAddress
  IAddressCreator* addrCreator = 0 ; 
  try        { addrCreator = dynamic_cast<IAddressCreator*> ( cnvSvc() ) ; } 
  catch(...) { addrCreator =                                           0 ; } 
  if( 0 == addrCreator ) { return Error("CreateRep::Address Creator is unavailable"); } 
  StatusCode st = addrCreator->createAddress( repSvcType() , classID() , "GiGaGeom" , "GiGaSurfaceObject" , -1 , Address );   
  if( st.isFailure()   ) { return Error("CreateRep::Error in Address Creation", st); }
  if( 0 == Address     ) { return Error("CreateRep::Error Address is created"        ); }
  ///
  return updateRep( Object , Address ) ; 
  /// 
}; 
////////////////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaSurfaceCnv::updateRep( DataObject*     Object  , IOpaqueAddress* Address  ) 
{
  ///
  { MsgStream log( msgSvc() , name() ); log << MSG::VERBOSE << "UpdateRep::start" << endreq; } 
  ///
  if( 0 == Object    ) { return Error("UpdateRep::DataObject* points to NULL"); } 
  Surface* surface = 0 ; 
  try        { surface = dynamic_cast<Surface*>( Object ) ; } 
  catch(...) { surface =                                0 ; } 
  if( 0 == surface   ) { return Error("UpdateRep::Bad cast to Surface*"); }
  if( 0 == cnvSvc()  ) { return Error("UpdateRep::Conversion Service is unavailable"); } 
  ///
  G4LogicalSurface* logSurf = 0 ;
  StatusCode         status = StatusCode::SUCCESS ; 
  if      ( surface->firstVol  ().empty () ) 
    { status = Error("UpdateRep: the firstVol name is EMPTY!") ; }
  else if ( surface->secondVol ().empty () ) 
    { status = createSkinSurface  (  surface   , logSurf     ) ; } /// SKIN   surface 
  else                                        
    { status = createBorderSurface(  surface   , logSurf     ) ; } /// BORDER surface
  ///
  if( status.isFailure() || 0 == logSurf ) 
    { return Error("Could not create G4LogicalSurface!", status ) ;} 
  /// create optical surface 
  G4OpticalSurface* optSurf = 
    new G4OpticalSurface( surface->fullpath () ,
                          ( G4OpticalSurfaceModel  ) surface->model   () ,
                          ( G4OpticalSurfaceFinish ) surface->finish  () ,
                          ( G4OpticalSurfaceType   ) surface->type    () ,
                          surface->value    ()                           );
  ///
  {
    if( 0 == optSurf->GetMaterialPropertiesTable() ) 
      { optSurf->SetMaterialPropertiesTable( new G4MaterialPropertiesTable() ) ; }
    StatusCode sc = 
      AddTabulatedProperties ( surface->tabulatedProperties        ()  ,
                               optSurf->GetMaterialPropertiesTable ()  ) ;
    if( sc.isFailure() ) { return Error("Could not set tabulated properties forG4OpticalSurface!") ; }
  }
  /// 
  if( 0 != logSurf->GetOpticalSurface() ) 
    {  
      G4OpticalSurface* oldOptSurf = logSurf->GetOpticalSurface();
      Error("Existing Optical surface would be replaced! Old one :"); 
      oldOptSurf->DumpInfo();
      Error("Existing Optical surface would be replaced! New one :"); 
      optSurf->DumpInfo();
      /// delete oldOptSurf;  /// DO NOT DELETE!!!! why?? is it correct? 
      logSurf->SetOpticalSurface( 0 ) ;
    }
  ///
  logSurf->SetOpticalSurface( optSurf ); 
  ///
  return StatusCode::SUCCESS;
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaSurfaceCnv::createSkinSurface( const Surface    *  surface , 
                                              G4LogicalSurface *& logsurf )
{ 
  ///
  logsurf = 0 ; 
  if( 0 == surface ) { return Error("CreateSkinSurface: Surface* points to NULL") ; }
  /// first look through G4LogicalVolumeStore
  G4LogicalVolume* lv = 0; 
  {
    G4LogicalVolumeStore& store = *G4LogicalVolumeStore::GetInstance();
    for( unsigned int indx = 0 ; indx < store.entries() ; ++indx )
      { if( surface->firstVol() == store[indx]->GetName() ) { lv = store[indx] ; break ; }  }    
  }
  if( 0 == lv ) { return Error("Could Not locate G4LogicalVolume by name '"+surface->firstVol()+"'" ); }
  /// look through existing LogicalSkinSurfaces
  G4LogicalSkinSurface* surf = G4LogicalSkinSurface::GetSurface( lv ); 
  if( 0 != surf &&  ( surf->GetName() != surface->fullpath() )  )
    { return Error("UpdateRep: surface with this log volume already exists!") ; }
  /// create new surface 
  if( 0 == surf ) { logsurf = new G4LogicalSkinSurface( surface->fullpath() , lv , 0 ); }
  else            { logsurf = surf ; }  
  ///
  return StatusCode::SUCCESS;
};
/////////////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaSurfaceCnv::createBorderSurface( const Surface    *  surface , 
                                                G4LogicalSurface *& logsurf )
{
  ///
  logsurf = 0 ;
  /// locate PhysVolumes
  G4VPhysicalVolume* pv1 = 0; 
  G4VPhysicalVolume* pv2 = 0; 
  {
    G4PhysicalVolumeStore& store = *G4PhysicalVolumeStore::GetInstance();
    for( unsigned int indx = 0 ; indx < store.entries() ; ++indx )
      { 
        if     ( surface->firstVol ()  == store[indx]->GetName() ) { pv1 = store[indx] ; }
        else if( surface->secondVol()  == store[indx]->GetName() ) { pv2 = store[indx] ; }
      }
    if( 0 == pv1 || 0 == pv2 ) 
      { return Error("Could not locatePhysical Volumes '"+surface->firstVol()+"' and '"+surface->secondVol()+"'");}
  }
  /// locate Logical Border Surface 
  G4LogicalBorderSurface* surf = G4LogicalBorderSurface::GetSurface( pv1 , pv2 );
  if( 0 != surf && ( surf->GetName() != surface->fullpath() ) )
    { return Error("UpdateRep: surface with this pv1/pv2 volumes already exists!") ; }
  if( 0 == surf ) { logsurf = new G4LogicalBorderSurface( surface->fullpath() , pv1 , pv2  , 0 ); }
  else            { logsurf = surf ; }  
  ///  
  return StatusCode::SUCCESS;
};  














