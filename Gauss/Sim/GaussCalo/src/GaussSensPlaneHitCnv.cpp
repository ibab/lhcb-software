// $Id: GaussSensPlaneHitCnv.cpp,v 1.2 2003-07-08 19:40:58 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2003/07/07 16:09:39  ibelyaev
//  add Calorimeter Sensitive Palne hits and the converter
//
// ============================================================================
// STD and STL
#include <iostream> 
// Gaudi 
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/DataObject.h"
// GiGa
#include "GiGa/IGiGaSvc.h"
#include "GiGa/GiGaUtil.h"
#include "GiGa/GiGaHitsByID.h"
#include "GiGa/GiGaHitsByName.h"
// GiGaCnv
#include "GiGaCnv/IGiGaHitsCnvSvc.h"
#include "GiGaCnv/IGiGaKineCnvSvc.h"
#include "GiGaCnv/GiGaKineRefTable.h"
// Event 
#include "Event/MCCaloSensPlaneHit.h"
#include "Event/MCParticle.h"
// G4 
#include "G4VHitsCollection.hh"
#include "G4HCofThisEvent.hh"
// local
#include "CaloHit.h"
#include "CaloSubHit.h"
#include "GaussSensPlaneHit.h"
#include "GaussSensPlaneHitCnv.h"

/** @file 
 *  Implementation file for class GaussSensPlaneHitCnv
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   2002-12-06
 */

// ============================================================================
/** @var GaussSensPlaneHitCnvFactory
 *  declaration of Converter Factory
 */
// ============================================================================
static const  CnvFactory<GaussSensPlaneHitCnv>         s_Factory ;
const        ICnvFactory&GaussSensPlaneHitCnvFactory = s_Factory ;
// ============================================================================

// ============================================================================
/** Standard constructor
 *  @param svc pointer to Service Locator 
 */
// ============================================================================ 
GaussSensPlaneHitCnv::GaussSensPlaneHitCnv( ISvcLocator* svc ) 
  : GiGaCnvBase( storageType() , classID() , svc ) 
{
  ///
  setNameOfGiGaConversionService( IGiGaCnvSvcLocation::Hits ) ; 
  setConverterName              ( "GaussSensPlaneHitCnv"          ) ;
  ///
  GiGaLeaf::Pars pars ( 1 , "CaloSP/CaloSPHits" ) ;
  declareObject( GiGaLeaf( MCCaloSensPlaneHitLocation::Default , 
                           objType() , pars  ) );  
  ///
};
// ============================================================================

// ============================================================================
/// destructor virtual and protected 
// ============================================================================
GaussSensPlaneHitCnv::~GaussSensPlaneHitCnv() {};
// ============================================================================

// ============================================================================
/** retrieve the class identifier  for created object 
 *  @return class idenfifier for created object 
 */
// ============================================================================ 
const CLID&         GaussSensPlaneHitCnv::classID     () 
{ return MCCaloSensPlaneHit::classID(); }
// ============================================================================

// ============================================================================
/** retrieve the storage type for created object 
 *  @return storage type  for created object 
 */
// ============================================================================
const unsigned char GaussSensPlaneHitCnv::storageType () 
{ return GiGaHits_StorageType; }
// ============================================================================

// ============================================================================
/** initialize the converter 
 *  @return status code
 */
// ============================================================================
StatusCode GaussSensPlaneHitCnv::initialize()
{
  // initialize the base class 
  StatusCode sc = GiGaCnvBase::initialize();
  if( sc.isFailure() )
    { return Error("Could not initialze the base class GiGaCnvBase!",sc);}
  // check for nesessary services 
  if( 0 == hitsSvc() ) { return Error("IGiGaHitsCnvSvc* points to NULL!");}
  //
  return StatusCode::SUCCESS;
};
// ============================================================================

// ======================================================================
/** create the Object 
 *  @param Address address 
 *  @param Object object itself 
 *  @return status code 
 */
// ======================================================================
StatusCode GaussSensPlaneHitCnv::createObj
( IOpaqueAddress*  address ,
  DataObject*&     object  )
{
  ///
  object = 0 ;
  if( 0 ==  address  ) { return Error("IOpaqueAddress* points to NULL!" ) ; }
  ///
  object        = new MCCaloSensPlaneHits();
  /// 
  StatusCode sc = updateObj( address , object );
  if( sc.isFailure() ) 
    { 
      if( 0 != object ) { delete object ; object = 0 ; }
      return Error("Could not create and update Object" , sc );
    }; 
  ///
  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
/** update the Object 
 *  @param address address 
 *  @param object object itself 
 *  @return status code 
 */
// ============================================================================
StatusCode GaussSensPlaneHitCnv::updateObj     
( IOpaqueAddress*     address   , 
  DataObject*         object    ) 
{ 
  // avoid long names
  typedef GaussSensPlaneHit                       Hit    ;
  typedef GaussSensPlaneHitsCollection            Hits   ;
  // check arguments 
  if( 0 ==   address   ) { return Error(" IOpaqueAddress* points to NULL" ) ; }
  if( 0 ==   object    ) { return Error(" DataObject* points to NULL"     ) ; }
  MCCaloSensPlaneHits* hits = dynamic_cast<MCCaloSensPlaneHits*> ( object ); 
  if( 0 ==   hits ) { return Error(" DataObject*(of type '"      + 
                                   GiGaUtil::ObjTypeName(object) + 
                                   "*') is not 'MCCaloSensPlaneHits*'! "   );}
  // clear the object 
  hits->clear();
  
  // get the registry
  const IRegistry* registry = address->registry();
  if( 0 == registry ) { return Error("IRegistry* points to NULL!");}
  
  const std::string& location = registry->identifier();
  
  { // try to locate MCParticles explicitely
    // just to force the loading of the reference table 
    std::string mcpath = 
      location.substr( 0, location.find( "/MC" ) ) + "/" + 
      MCParticleLocation::Default;
    // get MCparticles 
    const MCParticles* mcps = get( dataProvider() , mcpath , mcps );
    if( 0 == mcps ) 
      { return Error("Can not locate MCparticles at '" + mcpath + "'");}  
  }
  
  const std::string* par = address->par();
  if( 0 == par || par->empty() ) 
    { return Error( " Illegal/Empty IOpaqueAddress "); }              // RETURN
  const std::string& colName = (*par) ;
  
  // get the hits collection from GiGa 
  GiGaHitsByName col( colName );
  *gigaSvc() >> col ;
  if( 0 == col.hits() ) 
    { 
      return Warning(" The hit collection='" + colName             + 
                     "' is not found! "                            + 
                     " Return *EMPTY* hit container '"             + 
                     location + "'"          , StatusCode::SUCCESS ) ; 
    } // RETURN
  
  // cast to concrete hit collection 
  const Hits* g4hits = gaussSensPlaneHits( col.hits() );
  if( 0 == g4hits    ) { return Error(" Wrong Collection type" ) ; } // RETURN
  
  /// get the reference table between G4 tracks/trajectories and MC particles
  const GiGaKineRefTable& table = hitsSvc() -> table() ;
  
  // explicit loop over all hits 
  const size_t size = g4hits->entries();
  for( size_t  g4hit = 0 ; g4hit < size ; ++g4hit ) 
    {
      // get the hit 
      const Hit* hit =  (*g4hits)[g4hit];
      if( 0 == hit ) { continue ; }                                 // CONTINUE 
      // grab all information 
      const int               trackID  = hit -> trackID  () ;
      const ParticleID&       pid      = hit -> pid      () ;
      const HepLorentzVector& position = hit -> position () ;
      const HepLorentzVector& momentum = hit -> momentum () ;
      
      // get the MC particle of the hit 
      const MCParticle* mcp = table( hit->trackID() ).particle() ;
      if( 0 == mcp ) { Warning( "MCParticle* points to NULL!" ) ; }
      
      // create new MC hit
      MCCaloSensPlaneHit* mchit = new MCCaloSensPlaneHit() ;                 
      // fill it 
      mchit -> setParticle ( mcp                 ) ;
      mchit -> setPosition ( hit -> position  () ) ;
      mchit -> setMomentum ( hit -> momentum  () ) ;
      mchit -> setType     ( hit -> pid().pid () ) ;
      // add hit to the container 
      hits->insert( mchit );
    } 
 
  return StatusCode::SUCCESS ;
};
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
