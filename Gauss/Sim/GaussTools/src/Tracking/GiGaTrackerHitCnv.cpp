//  ===========================================================================
#define GIGACNV_GiGaTrackerHitCnv_CPP 1 
// ============================================================================
#include "CLHEP/Geometry/Point3D.h"
/// STL 
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
/// GaudiKernel
#include "GaudiKernel/CnvFactory.h" 
#include "GaudiKernel/IAddressCreator.h" 
#include "GaudiKernel/IOpaqueAddress.h" 
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
/// GaudiKernel
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/ParticleProperty.h"
/// GiGa 
#include "GiGa/IGiGaSvc.h" 
#include "GiGaCnv/IGiGaHitsCnvSvc.h"
#include "GiGa/GiGaTrajectory.h" 
#include "GiGa/GiGaUtil.h"
#include "TrackerHit.h"
/// GiGaCnv  
#include "GiGaCnv/GiGaKineRefTable.h" 
#include "GiGaCnv/GiGaCnvUtils.h"
/// LHCbEvent
#include "Event/MCHit.h" 
/// Geant4 includes
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
// local 
#include "GiGaTrackerHitCnv.h"

// ======================================================================

static const  CnvFactory<GiGaTrackerHitCnv>         s_Factory ;
const        ICnvFactory&GiGaTrackerHitCnvFactory = s_Factory ;

// ======================================================================

GiGaTrackerHitCnv::GiGaTrackerHitCnv( ISvcLocator* Locator ) 
  : GiGaCnvBase( storageType() , classID() , Locator ) 
{
  ///
  setNameOfGiGaConversionService( "GiGaHitsCnvSvc" ); 
  setConverterName              ( "GiGaTrHitCnv"     );
  ///
  GiGaLeaf::Pars pars1;
  pars1.push_back("OTSDet/Hits");
  declareObject(GiGaLeaf( MCHitLocation::OTHits , objType(), pars1));

  GiGaLeaf::Pars pars2;
  pars2.push_back("ITSDet/Hits");
  declareObject( GiGaLeaf( MCHitLocation::ITHits , objType(),pars2));
  ///
}; 

// ======================================================================

GiGaTrackerHitCnv::~GiGaTrackerHitCnv(){}; 

// ======================================================================

const CLID&         GiGaTrackerHitCnv::classID     () 
{ return MCHit::classID(); }

// ======================================================================

const unsigned char GiGaTrackerHitCnv::storageType () 
{ return GiGaHits_StorageType; }

// ======================================================================

StatusCode GiGaTrackerHitCnv::initialize()
{
  /// initialize the base class 
  StatusCode sc = GiGaCnvBase::initialize();
  if( sc.isFailure() )
    { return Error("Could not initialze the base class!",sc);}
  /// check for nesessary services 
  if( 0 == hitsSvc() ) 
    { return Error("IGiGaHitsCnvSvc* points to NULL!");}
  ///
  return StatusCode::SUCCESS;
  ///
};

// ======================================================================

StatusCode GiGaTrackerHitCnv::finalize  () 
{
  return GiGaCnvBase::finalize();
};

// ======================================================================

StatusCode GiGaTrackerHitCnv::createObj
( IOpaqueAddress*  address ,
  DataObject*&     object  )
{
  ///
  object = 0 ;
  if( 0 ==  address  ) 
    { return Error("IOpaqueAddress* points to NULL!" ) ; }
  ///
  object        = new MCHits();
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

StatusCode GiGaTrackerHitCnv::fillObjRefs
( IOpaqueAddress*  address , 
  DataObject*      object  ) 
{
  if( 0 ==   address   ) { return Error(" IOpaqueAddress* points to NULL" );}
  if( 0 ==   object    ) { return Error(" DataObject* points to NULL"     );}   
  MCHits* hits = dynamic_cast<MCHits*> ( object ); 
  if( 0 ==   hits ) { return Error(" DataObject* (of type '"       + 
                                        GiGaUtil::ObjTypeName( object ) + 
                                        "*') is not 'MCHits*'!"      );}  
  ///
  return updateObjRefs( address , object );
  ///
};

// ============================================================================

StatusCode GiGaTrackerHitCnv::updateObj
( IOpaqueAddress*  address , 
  DataObject*      object  ) 
{

  ///
  if( 0 ==   address   ) { return Error(" IOpaqueAddress* points to NULL");}
  if( 0 ==   object    ) { return Error(" DataObject* points to NULL"    );}
  MCHits* hits = dynamic_cast<MCHits*> ( object ); 
  if( 0 ==   hits ) { return Error(" DataObject*(of type '"      + 
                                        GiGaUtil::ObjTypeName(object) + 
                                        "*') is not 'MCHits*'! "   );}  
  // clear the object 
  hits->clear(); 
  
  // retrieve the hits container from GiGa Service

  G4HCofThisEvent* hitscollections = 0 ; 

  std::string colname = *(address->par());
  
  try
    {
        // get hitscollections from GiGa 
        *gigaSvc() >> hitscollections ; 

        //
        if( 0 != hitscollections ) 
          { 

          G4SDManager* fSDM=G4SDManager::GetSDMpointer();
          int collectionID=fSDM->GetCollectionID(colname);
          
          TrackerHitsCollection* myCollection = 
            (TrackerHitsCollection*)(hitscollections->GetHC(collectionID));
          
          if(0==myCollection || collectionID==-1) 
            {
              return StatusCode::SUCCESS;
            }
          int numberofhits=myCollection->entries();

          // reserve elements on object container 
          hits->reserve( numberofhits );

          //convert hits          
          for( int itr = 0 ; itr<numberofhits ; ++itr ) 
           {
             HepPoint3D pentry=(*myCollection)[itr]->GetEntryPos();
             HepPoint3D pexit=(*myCollection)[itr]->GetExitPos();
             double edep=(*myCollection)[itr]->GetEdep();
             double toffl=(*myCollection)[itr]->GetTimeOfFlight();

             MCHit* mchit =new MCHit();
             mchit->setEntry(pentry);
             mchit->setExit(pexit);
             mchit->setEnergy(edep);
             mchit->setTimeOfFlight(toffl);             
             
             hits->insert( mchit);
           }      
          }
        else
          {
            MsgStream log(   msgSvc(), name());
            log << MSG::INFO << "No TrackerHits to be converted" 
                <<endreq;        
            return StatusCode::SUCCESS; 
          }
        
   }
  
  catch( const GaudiException& Excp )
    {
      hits->clear(); 
      return Exception("updateObj: " , Excp ) ;
    }  
  catch( const std::exception& Excp )
    {
      hits->clear(); 
      return Exception("updateObj: " , Excp ) ;  
    }
  catch( ...  )
    {
      hits->clear(); 
      return Exception("updateObj: " ) ;  
    }
  ///
  return StatusCode::SUCCESS;
};

// ======================================================================

StatusCode GiGaTrackerHitCnv::updateObjRefs
( IOpaqueAddress*  address , 
  DataObject*      object  ) 
{  
  if( 0 ==   address   ) { return Error(" IOpaqueAddress* points to NULL " );}
  if( 0 ==   object    ) { return Error(" DataObject* points to NULL"      );}
  MCHits* hits = dynamic_cast<MCHits*> ( object ); 
  if( 0 ==   hits ) { return Error(" DataObject*(of type '"      + 
                                        GiGaUtil::ObjTypeName(object) + 
                                        "*') is not 'Hits*'!"    );}  
  /// get hits 
  G4HCofThisEvent* hitscollections = 0 ;

  try{ *gigaSvc() >> hitscollections ; }
  catch( const GaudiException& Excpt  ) 
    { return Exception("UpdateObjRefs: " , Excpt ) ; }  
  catch( const std::exception& Excpt  ) 
    { return Exception("UpdateObjRefs: " , Excpt ) ; }  
  catch( ... )                          
    { return Exception("UpdateObjRefs: "         ) ; }  
  if( 0 == hitscollections      ) 
    {             
      MsgStream logg(   msgSvc(), name());
      logg << MSG::INFO 
           << "No G4HCOfThisEvent found" << endreq;      
      return StatusCode::SUCCESS;
    }
  
  std::string colname = *(address->par());  
  
  /// get table 
  GiGaKineRefTable& table = hitsSvc()->table();

  G4SDManager* fSDM=G4SDManager::GetSDMpointer();
  int collectionID=fSDM->GetCollectionID(colname);
  
  TrackerHitsCollection* myCollection = 
    (TrackerHitsCollection*)(hitscollections->GetHC(collectionID));
  if(0==myCollection || collectionID==-1) 
    {
      return StatusCode::SUCCESS;
    }

  int numberofhits=myCollection->entries();
  int itr=0;

  if( (size_t) hits->size() != (size_t) myCollection->entries() ) 
    {
      return Error("MCHits and G4TrackerHitsCollection have different sizes!");
    }
  
  // fill the references
  for (MCHits::const_iterator iter=hits->begin() ; hits->end()!=iter ; ++iter)
    { 
      int traid=(*myCollection)[itr]->GetTrackID();

      if(table[traid].particle())
        {
          (*iter)->setMCParticle(table[traid].particle());
        }
      else
        {
          MsgStream log(   msgSvc(), name());
          log << MSG::WARNING 
              << "No pointer to MCParticle for MCHit associated to trackID: " 
              << traid << endreq;
          
        }      
      itr++;
    }

  ///
  return StatusCode::SUCCESS;
  ///
};

















































