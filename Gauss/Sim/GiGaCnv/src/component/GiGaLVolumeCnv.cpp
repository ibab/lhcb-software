// $Id: GiGaLVolumeCnv.cpp,v 1.8 2002-01-22 18:24:44 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.7  2001/08/15 14:48:05  ibelyaev
// update from DetDesc v7 to DetDesc v8
//
// Revision 1.6  2001/08/12 17:24:53  ibelyaev
// improvements with Doxygen comments
//
// Revision 1.5  2001/07/25 17:19:32  ibelyaev
// all conversions now are moved from GiGa to GiGaCnv
//
// Revision 1.4  2001/07/24 11:13:55  ibelyaev
// package restructurization(III) and update for newer GiGa
//
// Revision 1.3  2001/07/15 20:45:11  ibelyaev
// the package restructurisation
// 
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
#include "DetDesc/Surface.h"
/// Geant4
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
/// GiGa & GiGaCnv 
#include "GiGaCnv/IGiGaGeomCnvSvc.h"
#include "GiGaCnv/GiGaCnvUtils.h"
/// local
#include "GiGaVolumeUtils.h"
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
static const  CnvFactory<GiGaLVolumeCnv>         s_Factory ;
const        ICnvFactory&GiGaLVolumeCnvFactory = s_Factory ;

// ============================================================================
/** standard constructor 
 *  @param Locator pointer to service locator 
 */
// ============================================================================
GiGaLVolumeCnv::GiGaLVolumeCnv( ISvcLocator* Locator ) 
  : GiGaCnvBase( storageType() , classID() , Locator ) 
  , m_leaf ( "" , classID() )
{
  setNameOfGiGaConversionService( "GiGaGeomCnvSvc" ); 
  setConverterName              ( "GiGaLVCnv"      ); 
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
    new G4LogicalVolume( solid , material , lv->name() , 0 , 0 , 0 );
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
  /// convert surfaces (if any) 
//    {
//      for( LVolume::Surfaces::iterator it = lv->surfaces().begin() ; 
//           lv->surfaces().end() != it ; ++it )
//        { 
//          StatusCode sc = GiGaCnvUtils::createRep( cnvSvc() , *it );
//          if( sc.isFailure() ) { return Error("Could not convert surfaces!"); }
//        }
//    }
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










