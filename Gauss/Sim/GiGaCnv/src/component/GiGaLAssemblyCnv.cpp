// $Id: GiGaLAssemblyCnv.cpp,v 1.6 2009-10-15 10:01:19 silviam Exp $
// Includes 
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IAddressCreator.h"
// GaudiKernel
// DetDesc 
#include "DetDesc/ILVolume.h"
#include "DetDesc/IPVolume.h"
#include "DetDesc/CLIDLAssembly.h"
// GiGaCnv 
#include "GiGaCnv/IGiGaCnvSvc.h"
#include "GiGaCnv/IGiGaGeomCnvSvc.h"
#include "GiGaCnv/GiGaAssembly.h"
#include "GiGaCnv/GiGaVolumeUtils.h"
// local
#include "GiGaLAssemblyCnv.h"

// ============================================================================
/** @file GiGaLAssemblyCnv.cpp
 * 
 *  Implementation file for class : GiGaLAssemblyCnv
 * 
 *  @date 19/01/2002 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 */
// ============================================================================


// ============================================================================
/** factory business
 */
// ============================================================================
DECLARE_CONVERTER_FACTORY( GiGaLAssemblyCnv )

// ============================================================================
/** standard constructor 
 *  @param Locator pointer to service locator 
 */
// ============================================================================
GiGaLAssemblyCnv::GiGaLAssemblyCnv( ISvcLocator* Locator ) 
  : GiGaCnvBase( storageType() , classID() , Locator ) 
  , m_leaf ( "" , classID() )
{
  setNameOfGiGaConversionService( IGiGaCnvSvcLocation::Geo ) ; 
  setConverterName              ( "GiGaLACnv"              ) ; 
} 

// ============================================================================
/** destructor
 */
// ============================================================================
GiGaLAssemblyCnv::~GiGaLAssemblyCnv(){}

// ============================================================================
/** Class ID
 */
// ============================================================================
const CLID& GiGaLAssemblyCnv::classID     () { return CLID_LAssembly ; }

// ============================================================================
/** StorageType
 */ 
// ============================================================================
unsigned char GiGaLAssemblyCnv::storageType ()
{ return GiGaGeom_StorageType ; }

// ============================================================================
/** create the representation]
 *  @param object pointer to object 
 *  @param address address 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaLAssemblyCnv::createRep
( DataObject*      object  , 
  IOpaqueAddress*& address ) 
{
  ///
  address = 0 ;
  if( 0 == object       ) 
    { return Error(" createRep:: DataObject* points to NULL!"       ) ; }
  ///
  const ILVolume* lv = dynamic_cast<const ILVolume*> ( object ) ;
  if( 0 == lv           ) 
    { return Error(" createRep:: bad cast to ILVolume* !"           ) ; }
  if( !lv->isAssembly() ) 
    { return Error(" createRep:: is not an Assembly!"               ) ; }
  ///
  IAddressCreator* addrCreator = addressCreator() ;
  if ( 0 == addrCreator  )
    { return Error(" createRep:: IAddressCreator* points to NULL! " ) ; }
  StatusCode sc = 
    addrCreator->createAddress( repSvcType  () ,
                                classID     () , 
                                m_leaf.par  () , 
                                m_leaf.ipar () , 
                                address        );
  if( sc.isFailure() ) 
    { return Error(" createRep::Error in address creation" , sc     ) ; }
  if( 0 == address       ) 
    { return Error(" createRep::Error address is not created"       ) ; }
  ///
  return updateRep( object , address );
}
  
// ============================================================================
/** update the representation
 *  @param  object pointer to object 
 *  @param  address address 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaLAssemblyCnv::updateRep
( DataObject*        object  , 
  IOpaqueAddress* /* address */ ) 
{
  if( 0 == object       ) 
    { return Error(" createRep:: DataObject* points to NULL!"        ) ; }
  ///
  const ILVolume* lv = dynamic_cast<const ILVolume*> ( object ) ;
  if( 0 == lv           ) 
    { return Error(" createRep:: bad cast to ILVolume* !"            ) ; }
  if( !lv->isAssembly() ) 
    { return Error(" createRep:: is not an Assembly!"                ) ; }
  ///
  GiGaAssembly* assembly = new GiGaAssembly( lv->name() ) ;
  ///
  MsgStream log( msgSvc() , name() );
  log << MSG::DEBUG 
      << " new Volume/GiGaAssembly is created with the name '"
      << lv->name() + "'" << endreq ;  
  ///
  for( ILVolume::PVolumes::const_iterator ipv = lv->pvBegin() ; 
       lv->pvEnd() != ipv ; ++ipv )
    {
      const IPVolume* pv = *ipv ;
      if( 0 == pv ) 
        { return Error(" updateRep:: IPVolume* points to NULL!"      ) ; }
      const GiGaVolume volume = 
        geoSvc()->volume( pv->lvolumeName() ) ;
      if( !volume.valid() ) 
        { return Error(" updateRep:: GiGaVolume is ivalid!" ) ; }
      assembly->addVolume( GiGaVolumePair( volume , pv->matrix() ) , 
                           pv->name()                              );
    }
  ///
  /// sensitivity
  if( !lv->sdName().empty() ) 
    { return Error("LAssembly could not be sensitive (now)"            ) ; }
  /// magnetic field 
  if( !lv->mfName().empty()) 
    { return Error("Magnetic field per volume is not yet implemented!" ) ; }
  ///
  return StatusCode::SUCCESS ;
  ///
}

// ============================================================================
// The End 
// ============================================================================
