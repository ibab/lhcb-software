// $Id: GiGaLVolumeCnv.cpp,v 1.15 2007-01-12 15:45:57 ranjard Exp $ 
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
#include "DetDesc/Surface.h"
/// Geant4
#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4PVPlacement.hh"
/// GiGa & GiGaCnv 
#include "GiGa/IGiGaSensDet.h"
#include "GiGa/IGiGaMagField.h"
#include "GiGa/GiGaUtil.h"
// GiGaCnv
#include "GiGaCnv/IGiGaGeomCnvSvc.h"
#include "GiGaCnv/GiGaCnvUtils.h"
#include "GiGaCnv/GiGaVolumeUtils.h"
/// local
#include "GiGaInstall.h"
#include "GiGaLVolumeCnv.h"

// ============================================================================
/** @file GiGaLVolumeCnv.cpp 
 * 
 *  implementation of class GiGaLVolumeCnv
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 */
// ============================================================================

// ============================================================================
/** mandatory factory business 
 */
// ============================================================================
DECLARE_CONVERTER_FACTORY( GiGaLVolumeCnv );

// ============================================================================
/** standard constructor 
 *  @param Locator pointer to service locator 
 */
// ============================================================================
GiGaLVolumeCnv::GiGaLVolumeCnv( ISvcLocator* Locator ) 
  : GiGaCnvBase( storageType() , classID() , Locator ) 
  , m_leaf       ( "" , classID()     ) 
{
  setNameOfGiGaConversionService( IGiGaCnvSvcLocation::Geo ) ; 
  setConverterName              ( "GiGaLVCnv"              ) ; 
}; 

// ============================================================================
/** destructor
 */
// ============================================================================
GiGaLVolumeCnv::~GiGaLVolumeCnv(){};

// ============================================================================
/** Class ID
 */
// ============================================================================
const CLID&         GiGaLVolumeCnv::classID     () 
{ return CLID_LVolume         ; }

// ============================================================================
/** StorageType
 */
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
StatusCode GiGaLVolumeCnv::createRep
( DataObject*     Object  , 
  IOpaqueAddress*& Address ) 
{
  ///
  Address = 0 ; 
  if( 0 == Object                 ) 
    { return Error("CreateRep::DataObject* points to NULL"); } 
  ///
  const ILVolume* lv = dynamic_cast<const ILVolume*>( Object ) ; 
  if( 0 == lv            ) 
    { return Error("CreateRep::Bad cast to ILVolume*"); }
  if( 0 == geoSvc()      ) 
    { return Error("CreateRep::Conversion Service is unavailable"); }   
  /// look at the store  ( already converted? )
  if( 0 != GiGaVolumeUtils::findLVolume( lv->name() ) ) 
    { return StatusCode::SUCCESS ; }                   ///< RETURN !
  /// create IOpaqueAddress
  IAddressCreator* addrCreator = addressCreator() ;
  if( 0 == addrCreator   ) 
    { return Error("CreateRep::Address Creator is unavailable"); } 
  StatusCode status = 
    addrCreator->createAddress( repSvcType  () , 
                                classID     () , 
                                m_leaf.par  () ,
                                m_leaf.ipar () , 
                                Address        );   
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
StatusCode GiGaLVolumeCnv::updateRep
( DataObject*        Object  , 
  IOpaqueAddress* /* Address */ ) 
{
  ///
  MsgStream log( msgSvc() , name() ); 
  log << MSG::VERBOSE << "UpdateRep::start" << endreq;  
  ///
  if( 0 == Object                 ) 
    { return Error("UpdateRep::DataObject* points to NULL"); } 
  ///
  ILVolume* lv = dynamic_cast<ILVolume*>( Object ) ;  
  if( 0 == lv                     ) 
    { return Error("CreateRep::Bad cast to ILVolume*"); }
  if( 0 == cnvSvc()               ) 
    { return Error("UpdateRep::Conversion Service is unavailable"); } 
  /// look at the G4 static store
  if( 0 != GiGaVolumeUtils::findLVolume( lv->name() ) ) 
    { return StatusCode::SUCCESS ; }                         ///< RETURN!!!
  ///
  G4VSolid*   solid    = geoSvc()->solid    ( lv->solid       () );
  if( 0 == solid    ) 
    { return Error("CreateRep::Could not create Solid!") ; } 
  G4Material* material = geoSvc()->material ( lv->materialName() );
  if( 0 == material ) 
    { return Error("CreateRep::Could not locate Material=" + 
                   lv->materialName() ) ; } 
  G4LogicalVolume* G4LV = 
    geoSvc()->createG4LV ( solid , material , lv->name() );
  /// printout 
  log << MSG::DEBUG 
      << " new Volume/G4LogicalVolume is created with the name '"
      << lv->name() + "'" << endreq ;  
  /// convert daugthers (if any) 
  for( ILVolume::PVolumes::const_iterator iPV = lv->pvBegin() ;  
       lv->pvEnd() != iPV ; ++iPV )
    {
      const IPVolume*     pv = *iPV ; 
      if( 0 == pv ) 
        { return Error("updateRep:: IPVolume* points to NULL for " 
                       + lv->name() );}
      const GiGaVolume vol = geoSvc()->volume( pv->lvolumeName() );
      if( !vol.valid() )
        { return Error("updateRep:: Could not convert DPVolume for " 
                       + lv->name() );}
      StatusCode sc = 
        GiGaInstall::installVolume( vol                               , 
                                    lv->name   () + "#" + pv->name () , 
                                    pv->matrix ()                     , 
                                    G4LV                              , 
                                    log                               ) ;
      if( sc.isFailure() ) 
        { return Error("updateRep:: coudl not install DPV for " 
                       + lv->name() ) ; }
    }
  // sensitivity
  if( !lv->sdName().empty() ) 
    {
      if( 0 == G4LV->GetSensitiveDetector() ) 
        {
          IGiGaSensDet* det = 0 ;
          StatusCode sc = geoSvc()->sensitive ( lv->sdName(),  det );
          if( sc.isFailure() ) 
            { return Error("updateRep:: Could no create SensDet " , sc ) ; }
          if( 0 == det ) 
            { return Error("updateRep:: Could no create SensDet "      ) ; }
          // set sensitive detector 
          G4LV->SetSensitiveDetector( det );
        }
      else 
        {
          Warning( "SensDet is already defined to be '" +
                   GiGaUtil::ObjTypeName( G4LV->GetSensitiveDetector() ) +"'");
        }
    }
  /// magnetic field 
  if( !lv->mfName().empty() ) 
    { 
      IGiGaFieldMgr* mgr = 0 ;
      StatusCode sc = geoSvc() -> fieldMgr ( lv -> mfName() ,  mgr  ) ;
      if( sc.isFailure() ) 
        { return Error ( "updateRep:: Could no create FieldMgr " , sc ) ; }
      if( 0 == mgr          ) 
        { return Error ( "updateRep:: Could no create FieldMgr "      ) ; }
      if( mgr -> global ()  )
        { return Error ( "updateRep:: FieldMgr is 'global'"           ) ; }
      if( 0 == mgr -> fieldMgr() ) 
        { return Error ( "updateRep:: FieldMgr is invalid "           ) ; }
      // set sensitive detector 
      G4LV -> SetFieldManager( mgr -> fieldMgr() , false ) ;
    }
  /// look again at the Geant4 static store
  if( 0 != GiGaVolumeUtils::findLVolume( lv->name() ) ) 
    { return StatusCode::SUCCESS ; }                          ///< RETURN !!!
  ///
  ///
  return Error("UpdateRep:: failure in conversion of "+lv->name() );
  ///
};

// ============================================================================
// The End 
// ============================================================================










