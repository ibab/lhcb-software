//  ===========================================================================
#define GIGACNV_GiGaMuonHitCnv_CPP 1 
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
#include "MuonHit.h"
/// GiGaCnv  
#include "GiGaCnv/GiGaKineRefTable.h" 
#include "GiGaCnv/GiGaCnvUtils.h"
/// LHCbEvent
#include "Event/MCMuonHit.h" 
#include "Event/MCMuonHitPath.h"
/// Geant4 includes
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
// local 
#include "GiGaMuonHitCnv.h"

// ======================================================================

static const  CnvFactory<GiGaMuonHitCnv>         s_Factory ;
const        ICnvFactory&GiGaMuonHitCnvFactory = s_Factory ;

// ======================================================================

GiGaMuonHitCnv::GiGaMuonHitCnv( ISvcLocator* Locator ) 
  : GiGaCnvBase( storageType() , classID() , Locator ) 
{
  ///
  setNameOfGiGaConversionService( IGiGaCnvSvcLocation::Hits ) ; 
  setConverterName              ( "GiGaMuonHitCnv"          ) ;
  ///
    
    GiGaLeaf::Pars pars[15];
    std::string MuonSD[] = {"MuonSDM1R1","MuonSDM1R2","MuonSDM1R34",
                            "MuonSDM2R1","MuonSDM2R2","MuonSDM2R34",
                            "MuonSDM3R1","MuonSDM3R2","MuonSDM3R34",
                            "MuonSDM4R1","MuonSDM4R2","MuonSDM4R34",
                            "MuonSDM5R1","MuonSDM5R2","MuonSDM5R34"};
  for(int i=0;i<15;i++)
    {
      pars[i].push_back(MuonSD[i]);
      declareObject(GiGaLeaf( MCMuonHitPath::MCMuonHitPath[i], 
                              objType(),pars[i]));
    }
  ///
}; 

// ======================================================================

GiGaMuonHitCnv::~GiGaMuonHitCnv(){}; 

// ======================================================================

const CLID&         GiGaMuonHitCnv::classID     () 
{ return MCMuonHit::classID(); }

// ======================================================================

const unsigned char GiGaMuonHitCnv::storageType () 
{ return GiGaHits_StorageType; }

// ======================================================================

StatusCode GiGaMuonHitCnv::initialize()
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

StatusCode GiGaMuonHitCnv::finalize  () 
{
  return GiGaCnvBase::finalize();
};

// ======================================================================

StatusCode GiGaMuonHitCnv::createObj
( IOpaqueAddress*  address ,
  DataObject*&     object  )
{
  ///
  object = 0 ;
  if( 0 ==  address  ) 
    { return Error("IOpaqueAddress* points to NULL!" ) ; }
  ///
  object        = new MCMuonHits();
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

StatusCode GiGaMuonHitCnv::fillObjRefs
( IOpaqueAddress*  address , 
  DataObject*      object  ) 
{
  if( 0 ==   address   ) { return Error(" IOpaqueAddress* points to NULL" );}
  if( 0 ==   object    ) { return Error(" DataObject* points to NULL"     );}   
  MCMuonHits* hits = dynamic_cast<MCMuonHits*> ( object ); 
  if( 0 ==   hits ) { return Error(" DataObject* (of type '"       + 
                                        GiGaUtil::ObjTypeName( object ) + 
                                        "*') is not 'MCMuonHits*'!"      );}  
  ///
  return updateObjRefs( address , object );
  ///
};

// ============================================================================

StatusCode GiGaMuonHitCnv::updateObj
( IOpaqueAddress*  address , 
  DataObject*      object  ) 
{

  ///
  if( 0 ==   address   ) { return Error(" IOpaqueAddress* points to NULL");}
  if( 0 ==   object    ) { return Error(" DataObject* points to NULL"    );}
  MCMuonHits* hits = dynamic_cast<MCMuonHits*> ( object ); 
  if( 0 ==   hits ) { return Error(" DataObject*(of type '"      + 
                                        GiGaUtil::ObjTypeName(object) + 
                                        "*') is not 'MCMuonHits*'! "   );}  
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
          
          MuonHitsCollection* myCollection = 
            (MuonHitsCollection*)(hitscollections->GetHC(collectionID));
          
          if(collectionID==-1 || 0==myCollection) 
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
             int chamber=(*myCollection)[itr]->ChamberID();
             int gap=(*myCollection)[itr]->GapID();

             MCMuonHit* mchit =new MCMuonHit();
             mchit->setEntry(pentry);
             mchit->setExit(pexit);
             mchit->setEnergy(edep);
             mchit->setTimeOfFlight(toffl);             
             mchit->setVolumeID(chamber,gap);

             hits->insert( mchit);
           }      
          }
        else
          {
            MsgStream log(   msgSvc(), name());
            log << MSG::INFO << "No MuonHits to be converted" 
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

StatusCode GiGaMuonHitCnv::updateObjRefs
( IOpaqueAddress*  address , 
  DataObject*      object  ) 
{  
  if( 0 ==   address   ) { return Error(" IOpaqueAddress* points to NULL " );}
  if( 0 ==   object    ) { return Error(" DataObject* points to NULL"      );}
  MCMuonHits* hits = dynamic_cast<MCMuonHits*> ( object ); 
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
  
  MuonHitsCollection* myCollection = 
    (MuonHitsCollection*)(hitscollections->GetHC(collectionID));
  if(0==myCollection || collectionID==-1) 
    {
      return StatusCode::SUCCESS;
    }

  int numberofhits=myCollection->entries();
  int itr=0;

  if( (size_t) hits->size() != (size_t) myCollection->entries() ) 
    {
      return 
        Error("MCMuonHits and G4MuonHitsCollection have different sizes!" ) ;
    }
  // fill the references
  for (MCMuonHits::const_iterator iter=hits->begin();hits->end()!=iter; ++iter)
    { 
      int traid=(*myCollection)[itr]->GetTrackID();

      if(table[traid].particle())
        {
          (*iter)->setMcParticle(table[traid].particle());
        }
      else
        {
          MsgStream log(   msgSvc(), name());
          log << MSG::WARNING 
              << "No pointer to MCParticle "  
              << " for MCMuonHit associated to trackID:" 
              << traid << endreq;
        }
      itr++;
    }

  ///
  return StatusCode::SUCCESS;
  ///
};

















































