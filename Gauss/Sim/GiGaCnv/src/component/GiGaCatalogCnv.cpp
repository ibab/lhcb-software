// $Id: GiGaCatalogCnv.cpp,v 1.1 2001-11-19 18:27:00 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files
// GaudiKernel
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/IDataDirectory.h"
#include "GaudiKernel/IDataSelector.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IAddressCreator.h"
#include "GaudiKernel/DataObject.h"
// local
#include "GiGaCnv/IGiGaCnvSvc.h"
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
static const  CnvFactory<GiGaCatalogCnv>         s_Factory ;
const        ICnvFactory&GiGaCatalogCnvFactory = s_Factory ;


// ============================================================================
/** Standard Constructor
 *  @param loc pointer to service locator 
 */
// ============================================================================
GiGaCatalogCnv::GiGaCatalogCnv( ISvcLocator* Locator ) 
  : GiGaCnvBase( storageType() , classID() , Locator ) 
{
  setNameOfGiGaConversionService( "GiGaGeomCnvSvc" ); 
  setConverterName              ( "GiGaCatalogCnv" );
}; 

// ============================================================================
/** destructor, virtual and protected 
 */ 
// ============================================================================
GiGaCatalogCnv::~GiGaCatalogCnv(){}; 

// ============================================================================
// Class ID
// ============================================================================
const CLID&  GiGaCatalogCnv::classID () { return DataObject::classID() ; }

// ============================================================================
// StorageType
// ============================================================================
const unsigned char GiGaCatalogCnv::storageType () 
{ return GiGaGeom_StorageType; } 

//=============================================================================
/** initialize the converter 
 *  @return status code
 */
//=============================================================================
StatusCode GiGaCatalogCnv::initialize() { return GiGaCnvBase::initialize() ; }

//=============================================================================
/** finalize  the converter 
 *  @return status code
 */
//=============================================================================
StatusCode GiGaCatalogCnv::finalize  () { return GiGaCnvBase::finalize  () ; }

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
  IAddressCreator* addrCreator = 0 ; 
  try        { addrCreator = dynamic_cast<IAddressCreator*> ( cnvSvc() ) ; } 
  catch(...) { addrCreator =                                           0 ; } 
  if( 0 == addrCreator ) 
    { return Error("CreateRep::Address Creator is unavailable"); } 
  StatusCode st = 
    addrCreator->createAddress( repSvcType() , 
                                classID   () , 
                                "GiGaGeom"   , 
                                "CatalogObject" , -1 , Address );   
  if( st.isFailure()   )
    { return Error("CreateRep::Error in Address Creation", st); }
  if( 0 == Address     ) 
    { return Error("CreateRep::Error Address is created"        ); }
  ///
  return updateRep( Object , Address ) ;
  ///
};

// ============================================================================
/** update the representation of Object 
 *  @param Object   object
 *  @param Address  address 
 *  @return status  code 
 */
// ============================================================================
StatusCode GiGaCatalogCnv::updateRep
( DataObject*      Object  , 
  IOpaqueAddress*  Address ) 
{
  ///
  if( 0 == Object  ) { return Error("DataObject*     points to NULL!") ; }
  if( 0 == Address ) { return Error("IOpaqueAddress* points to NULL!") ; }
  ///
  IDataDirectory* dir = Object->directory();
  if( 0 == dir     ) { return Error("IDataDirectory* points to NULL!") ; }
  ///
  IDataSelector dSel;
  for( IDataDirectory::Iterator idir = dir->begin() ;
       dir->end() != idir ; ++idir )
    {    
      // skip invalids 
      if( 0 ==*idir ) { continue ; }
      DataObject* obj =  (*idir)->object();
      if( 0 == obj  ) { continue ; }
      dSel.push_back( obj ) ;
      StatusCode sc = cnvSvc()->createReps( &dSel );
      if( sc.isFailure() )
        { return Error("Coudl not create the representation of the leaf!",sc);}
      dSel.clear();
    }
  ///
  return StatusCode::SUCCESS ;
};

// ============================================================================
// The End 
// ============================================================================
