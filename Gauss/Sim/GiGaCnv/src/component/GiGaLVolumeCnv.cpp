// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.5  2001/07/25 17:19:32  ibelyaev
/// all conversions now are moved from GiGa to GiGaCnv
///
/// Revision 1.4  2001/07/24 11:13:55  ibelyaev
/// package restructurization(III) and update for newer GiGa
///
/// Revision 1.3  2001/07/15 20:45:11  ibelyaev
/// the package restructurisation
/// 
// ============================================================================
#define GIGACNV_GIGALVOLUMECNV_CPP 1 
// ============================================================================
/// GaudiKernel
#include "GaudiKernel/IAddressCreator.h"
#include "GaudiKernel/IDataSelector.h"
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/DataObject.h"
/// DetDesc
#include "DetDesc/ILVolume.h"
#include "DetDesc/IPVolume.h"
#include "DetDesc/CLIDLVolume.h"
#include "DetDesc/LVolume.h"
/// Geant4
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PVPlacement.hh"
/// GiGa & GiGaCnv 
#include "GiGaCnv/IGiGaGeomCnvSvc.h"
/// local
#include "GiGaLVolumeCnv.h" 


static const  CnvFactory<GiGaLVolumeCnv> s_GiGaLVolumeCnvFactory ;
const        ICnvFactory&  GiGaLVolumeCnvFactory = s_GiGaLVolumeCnvFactory ;

// ============================================================================
  /** standard constructor 
   *  @param Locator pointer to service locator 
   */
// ============================================================================
GiGaLVolumeCnv::GiGaLVolumeCnv( ISvcLocator* Locator ) 
  : GiGaCnvBase( storageType() , classID() , Locator ) 
{
  setNameOfGiGaConversionService( "GiGaGeomCnvSvc" ); 
  setConverterName              ( "GiGaLVCnv"      ); 
}; 

// ============================================================================
/// destructor 
// ============================================================================
GiGaLVolumeCnv::~GiGaLVolumeCnv(){}; 

// ============================================================================
/// Class ID
// ============================================================================
const CLID&         GiGaLVolumeCnv::classID     () 
{ return CLID_LVolume         ; }

// ============================================================================
/// StorageType 
// ============================================================================
const unsigned char GiGaLVolumeCnv::storageType () 
{ return GiGaGeom_StorageType ; } 

// ============================================================================
/** create the representation]
 *  @param Object pointer to object 
 *  @param Address address 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaLVolumeCnv::createRep( DataObject*     Object  , 
                                      IOpaqueAddress*& Address ) 
{
  ///
  Address = 0 ; 
  if( 0 == Object                 ) 
    { return Error("CreateRep::DataObject* points to NULL"); } 
  ///
  ILVolume* lv = 0 ; 
  try        { lv = dynamic_cast<ILVolume*>( Object ) ; } 
  catch(...) { lv =                                 0 ; } 
  if( 0 == lv            ) 
    { return Error("CreateRep::Bad cast to ILVolume*"); }
  if( 0 == geoSvc()      ) 
    { return Error("CreateRep::Conversion Service is unavailable"); }   
  /// look at the store 
  {
    G4LogicalVolume* LV = 0; 
    G4LogicalVolumeStore& store = *G4LogicalVolumeStore::GetInstance();
    for( unsigned int indx = 0 ; indx < store.size() ; ++indx )
      { if( lv->name() == store[indx]->GetName() ) 
        { return StatusCode::SUCCESS ; } }    /// RETURN !!!
  }
  /// create IOpaqueAddress
  IAddressCreator* addrCreator = 0 ; 
  try        
    { addrCreator = dynamic_cast<IAddressCreator*> ( cnvSvc() ) ; } 
  catch(...) 
    { addrCreator =                                           0 ; } 
  if( 0 == addrCreator   ) 
    { return Error("CreateRep::Address Creator is unavailable"); } 
  StatusCode status = 
    addrCreator->createAddress( repSvcType() , 
                                classID   () , 
                                "GiGaGeom"   , 
                                "GiGaLVolumeObject" , -1 , Address );   
  if( status.isFailure() ) 
    { return Error("CreateRep::Error in Address Creation",status); }
  if( 0 == Address       ) 
    { return Error("CreateRep::Error Address is created"        ); }
  ///
  return updateRep( Object , Address ) ; 
  /// 
}; 

// ============================================================================
/** update the representation]
 *  @param Object pointer to object 
 *  @param Address address 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaLVolumeCnv::updateRep( DataObject*     Object  , 
                                      IOpaqueAddress* /* Address */ ) 
{
  ///
  { MsgStream log( msgSvc() , name() ); 
  log << MSG::VERBOSE << "UpdateRep::start" << endreq; } 
  ///
  if( 0 == Object                 ) 
    { return Error("UpdateRep::DataObject* points to NULL"); } 
  ///
  ILVolume* lv = 0 ; 
  try        { lv = dynamic_cast<ILVolume*>( Object ) ; } 
  catch(...) { lv =                                 0 ; } 
  if( 0 == lv                     ) 
    { return Error("CreateRep::Bad cast to ILVolume*"); }
  if( 0 == cnvSvc()               ) 
    { return Error("UpdateRep::Conversion Service is unavailable"); } 
  /// look at the G4 static store 
  {
    G4LogicalVolumeStore& store = *G4LogicalVolumeStore::GetInstance();
    for( unsigned int indx = 0 ; indx < store.size() ; ++indx )
      { if( lv->name() == store[indx]->GetName() ) 
        { return StatusCode::SUCCESS ; } }    /// RETURN !!!
  }
  ///
  G4VSolid*   solid    = geoSvc()->g4Solid   ( lv->solid       () );
  if( 0 == solid    ) 
    { return Error("CreateRep::Could not create Solid!") ; } 
  G4Material* material = geoSvc()->g4Material( lv->materialName() );
  if( 0 == material ) 
    { return Error("CreateRep::Could not locate Material=" + 
                   lv->materialName() ) ; } 
  G4LogicalVolume* G4LV = 
    new G4LogicalVolume( solid , material , lv->name() , 0 , 0 , 0 );
  /// convert daugthers (if any) 
  for( ILVolume::PVolumes::const_iterator iPV = lv->pvBegin() ;  
       lv->pvEnd() != iPV ; ++iPV )
    {
      const IPVolume*     pv = *iPV ; 
      if( 0 == pv ) 
        { return Error("updateRep:: IPVolume* point to NULL for " + 
                       lv->name() );}
      G4LogicalVolume*    LV = geoSvc()->g4LVolume( pv->lvolumeName() ); 
      if( 0 == LV ) 
        { return Error("updateRep:: Could not convert DLVolume for " +
                       lv->name() );}
      G4VPhysicalVolume*  PV = 
	/// new G4PVPlacement( pv->matrix() , LV , 
        /// lv->name()+"#"+pv->name() , G4LV , false , 0 );
        new G4PVPlacement( pv->matrix().inverse() , LV , 
                           lv->name()+"#"+pv->name() , G4LV , 
                           false , iPV - lv->pvBegin() );
    }
  /// convert surfaces (if any) 
  { 
    IDataSelector ds; 
    LVolume* doLV = dynamic_cast<LVolume*>(lv); 
    if( 0 == doLV ) 
      { return Error("Base clas LVolume is not available!") ; }
    for( LVolume::Surfaces::iterator it = doLV->surfaces().begin() ; 
         doLV->surfaces().end() != it ; ++it )
      { ds.push_back( *it ); }
    StatusCode sc = cnvSvc()->createReps( &ds );
    if( sc.isFailure() ) { return Error("Could not convert surfaces!"); }
  }
  /// look again at the G4 static store
  {
    G4LogicalVolumeStore& store = *G4LogicalVolumeStore::GetInstance();
    for( unsigned int indx = 0 ; indx < store.size() ; ++indx )
      { if( lv->name() == store[indx]->GetName() ) 
        { return StatusCode::SUCCESS ; } }    /// RETURN !!!
  }
  ///
  return Error("UpdateRep:: failure in convertion of "+lv->name() );
  ///
};

// ============================================================================










