// $Id: GiGaCatalogCnv.cpp,v 1.7 2009-10-15 10:00:05 silviam Exp $
// Include files

// from Gaudi
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/IDataSelector.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IAddressCreator.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/SmartDataPtr.h"
// from GiGaCnv
#include "GiGaCnv/IGiGaCnvSvc.h"
#include "GiGaCnv/GiGaCnvUtils.h"
#include "GiGaCnv/IGiGaCnvSvcLocation.h"
// local
#include "GiGaCatalogCnv.h"

// ============================================================================
/** @file GiGaCatalogCnv.cpp 
 *
 *  Implementation file for class : GiGaCatalogCnv
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 19/11/2001 
 */
// ============================================================================

// ============================================================================
/** Factory stuff 
 */
// ============================================================================
DECLARE_CONVERTER_FACTORY( GiGaCatalogCnv )

// ============================================================================
/** Standard Constructor
 *  @param loc pointer to service locator 
 */
// ============================================================================
GiGaCatalogCnv::GiGaCatalogCnv( ISvcLocator* Locator ) 
  : GiGaCnvBase( storageType() , classID() , Locator ) 
{
  setNameOfGiGaConversionService( IGiGaCnvSvcLocation::Geo ) ; 
  setConverterName              ( "GiGaDirCnv"             ) ;
}

// ============================================================================
/** destructor, virtual and protected 
 */ 
// ============================================================================
GiGaCatalogCnv::~GiGaCatalogCnv(){}

// ============================================================================
// Class ID
// ============================================================================
const CLID&  GiGaCatalogCnv::classID () { 
  return DataObject::classID() ; 
}

// ============================================================================
// StorageType
// ============================================================================
//const unsigned char GiGaCatalogCnv::storageType () 
long GiGaCatalogCnv::storageType (){ 
  return GiGaGeom_StorageType; 
} 

//=============================================================================
/** initialize the converter 
 *  @return status code
 */
//=============================================================================
StatusCode GiGaCatalogCnv::initialize() { 
  return GiGaCnvBase::initialize() ; 
}

//=============================================================================
/** finalize  the converter 
 *  @return status code
 */
//=============================================================================
StatusCode GiGaCatalogCnv::finalize  () { 
  return GiGaCnvBase::finalize  () ; 
}

// ============================================================================
/** create the representation of the Object 
 *  @param  Object  object 
 *  @param  Address address 
 *  @return status  code 
 */
// ============================================================================
StatusCode GiGaCatalogCnv::createRep
( DataObject*      Object  , 
  IOpaqueAddress*& Address ) 
{
  /// check parameters
  Address = 0 ;
  if( 0 == Object ) { return Error("DataObject* points to NULL!"); }
  /// create IOpaqueAddress
  IAddressCreator* addrCreator = addressCreator() ;
  if( 0 == addrCreator ) 
    { return Error("CreateRep::Address Creator is unavailable"); } 
  StatusCode st = 
    addrCreator->createAddress( repSvcType  () , 
                                classID     () , 
                                m_leaf.par  () ,
                                m_leaf.ipar () , 
                                Address        );   
  if( st.isFailure()   )
    { return Error("CreateRep::Error in Address Creation", st); }
  if( 0 == Address     ) 
    { return Error("CreateRep::Error Address is created"        ); }
  ///
  return updateRep( Object , Address ) ;
  ///
}

// ============================================================================
/** update the representation of Object 
 *  @param Object   object
 *  @param Address  address 
 *  @return status  code 
 */
// ============================================================================
StatusCode GiGaCatalogCnv::updateRep
( DataObject*         Object  , 
  IOpaqueAddress*  /* Address */ ) 
{
  ///
  if( 0 == Object    ) { return Error("DataObject*      points to NULL!") ; }
  ///
  IRegistry* registry = Object->registry() ;
  if( 0 == registry  ) { return Error("IRegistry*       points to NULL!") ; }
  SmartIF<IDataManagerSvc> dataMgr( registry->dataSvc() );
  if( !dataMgr       ) { return Error("IDataManagersvc* points to NULL!") ; }
  typedef std::vector<IRegistry*> Leaves;
  IDataSelector dataSelector;
  Leaves leaves ;
  StatusCode sc = dataMgr->objectLeaves( registry , leaves );
  if( sc.isFailure() ) { return Error("Could not get leaves!" , sc      ) ; }
  for( Leaves::iterator leaf = leaves.begin() ; leaves.end() != leaf ; ++leaf )
    {
      if( 0 == *leaf ) { return Error("IRegistry* for leaf is NULL!"    ) ; }
      SmartDataPtr<DataObject> obj( registry->dataSvc() , *leaf ) ;
      if( !obj       ) { return Error("The leaf is not accessible!"     ) ;}
      dataSelector.push_back( obj );
    }
  sc = GiGaCnvUtils::createReps( cnvSvc() , &dataSelector );
  if( sc.isFailure() )
    { return Error("Coud not create the representation of the leaves!",sc);}
  ///
  return StatusCode::SUCCESS ;
}

// ============================================================================
// The End 
// ============================================================================
