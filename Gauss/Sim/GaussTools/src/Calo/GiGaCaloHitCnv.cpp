// $Id: GiGaCaloHitCnv.cpp,v 1.2 2002-12-07 21:19:14 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/12/07 14:41:45  ibelyaev
//  add new Calo stuff
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
#include "Event/MCCaloHit.h"
#include "Event/MCParticle.h"
// G4 
#include "G4VHitsCollection.hh"
#include "G4HCofThisEvent.hh"
// local
#include "CaloHit.h"
#include "CaloSubHit.h"
#include "GiGaCaloHitCnv.h"

/** @file 
 *  Implementation file for class GiGaCaloHitCnv
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   2002-12-06
 */

// ============================================================================
/** @var GiGaCaloHitCnvFactory
 *  declaration of Converter Factory
 */
// ============================================================================
static const  CnvFactory<GiGaCaloHitCnv>         s_Factory ;
const        ICnvFactory&GiGaCaloHitCnvFactory = s_Factory ;
// ============================================================================

// ============================================================================
/** Standard constructor
 *  @param svc pointer to Service Locator 
 */
// ============================================================================ 
GiGaCaloHitCnv::GiGaCaloHitCnv( ISvcLocator* svc ) 
  : GiGaCnvBase( storageType() , classID() , svc ) 
{
  ///
  setNameOfGiGaConversionService( IGiGaCnvSvcLocation::Hits ) ; 
  setConverterName              ( "GiGaCaloHitCnv"          ) ;
  ///
  GiGaLeaf::Pars spd  ( 1 , "Spd/Hits"  ) ;
  declareObject( GiGaLeaf( MCCaloHitLocation::Spd  , objType() , spd  ) );
  
  GiGaLeaf::Pars prs  ( 1 , "Prs/Hits"  ) ;
  declareObject( GiGaLeaf( MCCaloHitLocation::Prs  , objType() , prs  ) );
  
  GiGaLeaf::Pars ecal ( 1 , "Ecal/Hits" ) ;
  declareObject( GiGaLeaf( MCCaloHitLocation::Ecal , objType() , ecal ) );
  
  GiGaLeaf::Pars hcal ( 1 , "Hcal/Hits" ) ;
  declareObject( GiGaLeaf( MCCaloHitLocation::Hcal , objType() , hcal ) );
  ///
};
// ============================================================================

// ============================================================================
/// destructor virtual and protected 
// ============================================================================
GiGaCaloHitCnv::~GiGaCaloHitCnv() {};
// ============================================================================

// ============================================================================
/** retrieve the class identifier  for created object 
 *  @return class idenfifier for created object 
 */
// ============================================================================ 
const CLID&         GiGaCaloHitCnv::classID     () 
{ return MCCaloHit::classID(); }
// ============================================================================

// ============================================================================
/** retrieve the storage type for created object 
 *  @return storage type  for created object 
 */
// ============================================================================
const unsigned char GiGaCaloHitCnv::storageType () 
{ return GiGaHits_StorageType; }
// ============================================================================

// ============================================================================
/** initialize the converter 
 *  @return status code
 */
// ============================================================================
StatusCode GiGaCaloHitCnv::initialize()
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
StatusCode GiGaCaloHitCnv::createObj
( IOpaqueAddress*  address ,
  DataObject*&     object  )
{
  ///
  object = 0 ;
  if( 0 ==  address  ) { return Error("IOpaqueAddress* points to NULL!" ) ; }
  ///
  object        = new MCCaloHits();
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
StatusCode GiGaCaloHitCnv::updateObj     
( IOpaqueAddress*     address   , 
  DataObject*         object    ) 
{ 
  // avoid long names 
  typedef CaloHitsCollection            CaloHits   ;
  // check arguments 
  if( 0 ==   address   ) { return Error(" IOpaqueAddress* points to NULL" ) ; }
  if( 0 ==   object    ) { return Error(" DataObject* points to NULL"     ) ; }
  MCCaloHits* hits = dynamic_cast<MCCaloHits*> ( object ); 
  if( 0 ==   hits ) { return Error(" DataObject*(of type '"      + 
                                   GiGaUtil::ObjTypeName(object) + 
                                   "*') is not 'MCCaloHits*'! "   );}
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
  const CaloHits* g4hits = caloHits( col.hits() );
  if( 0 == g4hits    ) { return Error(" Wrong Collection type" ) ; } // RETURN
  
  /// get the reference table between G4 tracks/trajectories and MC particles
  const GiGaKineRefTable& table = hitsSvc() -> table() ;
  
  // explicit loop over all hits 
  const size_t size = g4hits->entries();
  for( size_t  g4hit = 0 ; g4hit < size ; ++g4hit ) 
    {
      // get the hit 
      const CaloHit* hit =  (*g4hits)[g4hit];
      if( 0 == hit ) { continue ; }                                 // CONTINUE 
      // explict loop over all subhits 
      for( CaloHit::iterator ihit = hit->begin() ; hit->end() != ihit ; ++ihit )
        {
          const CaloSubHit* subhit = ihit->second ;
          if( 0 == subhit ) { continue ; }                         // CONTINUE 
          // get the MC particle of the sub hit
          const MCParticle* mcp = table( subhit->trackID() ).particle() ;
          if( 0 == mcp ) { Warning( "MCParticle* points to NULL!" ) ; }
          // explicit loop over all time/energy entries of the subhit 
          for( CaloSubHit::iterator entry = subhit->begin() ; 
               subhit->end() != entry ; ++entry )
            {
              // create new MC hit
              MCCaloHit* mchit = new MCCaloHit() ;                 
              // fill it 
              mchit->setCellID   ( hit    -> cellID() ) ;
              mchit->setTime     ( entry  -> first    ) ;
              mchit->setActiveE  ( entry  -> second   ) ;
              mchit->setParticle ( mcp                ) ;
              // add hit to the container 
              hits->insert( mchit );
            } // loop over all time/energy entries of the subhit 
        } // loop over all subhits of the hit 
    } // loop over all hits
  
  return StatusCode::SUCCESS ;
};
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
