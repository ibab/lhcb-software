//  ===========================================================================
#define GIGACNV_GiGaRichHitCnv_CPP 1 
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
#include "RichG4Hit.h"
#include "GiGa/GiGaHitsByID.h"
#include "GiGa/GiGaHitsByName.h"
/// GiGaCnv  
#include "GiGaCnv/GiGaKineRefTable.h" 
#include "GiGaCnv/GiGaCnvUtils.h"
#include "GiGaCnv/IGiGaHitsCnvSvc.h"
#include "GiGaCnv/IGiGaKineCnvSvc.h"
#include "GiGaCnv/GiGaKineRefTable.h"
/// LHCbEvent
#include "Event/MCRichPhotodetectorHit.h" 
/// Geant4 includes
#include "G4VHitsCollection.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
// local 
#include "GiGaRichHitCnv.h"
#include "RichG4HitCollName.h"
// ======================================================================

static const  CnvFactory<GiGaRichHitCnv>         s_Factory ;
const        ICnvFactory&GiGaRichHitCnvFactory = s_Factory ;

// ======================================================================

GiGaRichHitCnv::GiGaRichHitCnv( ISvcLocator* Locator ) 
  : GiGaCnvBase( storageType() , classID() , Locator ),
    m_RichG4HitCollectionName(0)
{
  ///
  setNameOfGiGaConversionService( IGiGaCnvSvcLocation::Hits ) ; 
  setConverterName              ( "GiGaRichHitCnv"          ) ;
  ///
  GiGaLeaf::Pars pars1;
  pars1.push_back("");
  declareObject(GiGaLeaf( MCRichPhotodetectorHitLocation::Default, 
                          objType(), pars1));
  ///

  m_RichG4HitCollectionName= new RichG4HitCollName();
  
  
}; 

// ======================================================================

GiGaRichHitCnv::~GiGaRichHitCnv(){
  
  delete m_RichG4HitCollectionName;
  
}; 

// ======================================================================

const CLID&         GiGaRichHitCnv::classID     () 
{ return MCRichPhotodetectorHit::classID(); }

// ======================================================================

const unsigned char GiGaRichHitCnv::storageType () 
{ return GiGaHits_StorageType; }

// ======================================================================

StatusCode GiGaRichHitCnv::initialize()
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

StatusCode GiGaRichHitCnv::finalize  () 
{
  return GiGaCnvBase::finalize();
};

// ======================================================================

StatusCode GiGaRichHitCnv::createObj
( IOpaqueAddress*  address ,
  DataObject*&     object  )
{
  ///
  object = 0 ;
  if( 0 ==  address  ) 
    { return Error("IOpaqueAddress* points to NULL!" ) ; }
  ///
  object        = new MCRichPhotodetectorHits();
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

StatusCode GiGaRichHitCnv::fillObjRefs
( IOpaqueAddress*  address , 
  DataObject*      object  ) 
{
  if( 0 ==   address   ) { return Error(" IOpaqueAddress* points to NULL" );}
  if( 0 ==   object    ) { return Error(" DataObject* points to NULL"     );}   
  MCRichPhotodetectorHits* hits = 
                 dynamic_cast<MCRichPhotodetectorHits*> ( object ); 
  if( 0 ==   hits ) { return Error(" DataObject* (of type '"       + 
                                        GiGaUtil::ObjTypeName( object ) +
                           "*') is not 'MCRichPhotodetectorHits*'!"      );}  
  ///
  return updateObjRefs( address , object );
  ///
};

// ============================================================================

StatusCode GiGaRichHitCnv::updateObj
( IOpaqueAddress*  address , 
  DataObject*      object  ) 
{

  ///
  if( 0 ==   address   ) { return Error(" IOpaqueAddress* points to NULL");}
  if( 0 ==   object    ) { return Error(" DataObject* points to NULL"    );}

  MCRichPhotodetectorHits* hits = 
                dynamic_cast<MCRichPhotodetectorHits*> ( object ); 

  
  if( 0 ==   hits ) { return Error(" DataObject*(of type '"      + 
                                        GiGaUtil::ObjTypeName(object) + 
                                        "*') is not 'MCRichPhotodetectorHits*'! "   );}  
  // clear the object 
  hits->clear(); 

  // get the registry
  const IRegistry* registry = address->registry();
  if( 0 == registry ) { return Error("IRegistry* points to NULL!");}

  const std::string& location = registry->identifier();

  //  { // try to locate MCParticles explicitely
    // just to force the loading of the reference table 
        std::string mcpath = 
        location.substr( 0, location.find( "/MC" ) ) + "/" + 
        MCParticleLocation::Default;
    // get MCparticles 
       const MCParticles* mcps = get( dataProvider() , mcpath , mcps );
     if( 0 == mcps ) 
      { return Error("Can not locate MCparticles at '" + mcpath + "'");}  
  //  }
  
  

  // retrieve the hits container from GiGa Service

    G4HCofThisEvent* hitscollections = 0 ;

  //std::string colname = *(address->par());
  // std::string colname [2];
    //  colname[0] = "Rich1TopHC";
    // colname[1] = "Rich1BotHC";

  try
    {
      // get hitscollections from GiGa 
      *gigaSvc() >> hitscollections ; 

        //
      if( 0 != hitscollections ) 
        { 

          for (int iii=0;iii<m_RichG4HitCollectionName->RichHCSize() ;iii++)
            {
              std::string colName = m_RichG4HitCollectionName->RichHCName(iii);
              G4SDManager* fSDM=G4SDManager::GetSDMpointer();
              int collectionID=fSDM->GetCollectionID(colName);
          
              RichG4HitsCollection* myCollection = 
                (RichG4HitsCollection*)(hitscollections->GetHC(collectionID));

             // get the hits collection from GiGa              
              //              GiGaHitsByName myCollection(colName );

             if(0==myCollection || collectionID==-1) 
              {
                // return Error("No RichG4HitsCollection* object found");
                                 return StatusCode::SUCCESS;
              }



              int numberofhits=myCollection->entries();

              // reserve elements on object container 
              hits->reserve( numberofhits );

               /// get the reference table between 
              // G4 tracks/trajectories and MC particles
               
                
               const GiGaKineRefTable& table = hitsSvc() -> table() ;

             //convert hits          
              for( int ihit = 0 ; ihit<numberofhits ; ++ihit ) 
                {
                  HepPoint3D pentry=(*myCollection)[ihit]->GetGlobalPos();
                  double edep=(*myCollection)[ihit]->GetEdep();
                  double toffl=(*myCollection)[ihit]-> RichHitGlobalTime();

                  MCRichPhotodetectorHit* mchit =new MCRichPhotodetectorHit();
                  mchit->setEntry(pentry);
                  mchit->setEnergy(edep);
                  mchit->setTimeOfFlight(toffl);             
             

                 int traid=(*myCollection)[ihit]->GetTrackID();

                 //                 cout << " trackid " << traid << endl;
                 

                  if(table[traid].particle())              
                   {
                     mchit->setMCParticle(table[traid].particle());

                   }
                 
                   else
                   {
                       MsgStream log(   msgSvc(), name());
                           log << MSG::INFO 
                               << "No pointer to MCParticle for " 
                               <<" MCRichPhotodetectorHit associated to trackID: " 
                      <<iii<<"  "<<ihit<< "   "<< traid << endreq;
                    }



                  hits->insert( mchit);
                }
              
              
            }
          
          
        }
      
      else
        {
          MsgStream log(   msgSvc(), name());
          log << MSG::INFO << "No RichHits to be converted" 
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

StatusCode GiGaRichHitCnv::updateObjRefs
( IOpaqueAddress*  address , 
  DataObject*      object  ) {


  return StatusCode::SUCCESS;
  
};


  
  //  if( 0 ==   address   ) { return Error(" IOpaqueAddress* points to NULL " );}
  //  if( 0 ==   object    ) { return Error(" DataObject* points to NULL"      );}
  //  MCRichPhotodetectorHits* hits = 
  //              dynamic_cast<MCRichPhotodetectorHits*> ( object ); 
  //  if( 0 ==   hits ) { return Error(" DataObject*(of type '"      + 
  //                                        GiGaUtil::ObjTypeName(object) + 
  //                                      "*') 
  // is not 'Hits*'!"  
  //                                        //  );}  
  // Total number of hits in the container

//  int TotNumHitsInRich=  hits->entries();
  
  /// get hits 
// G4HCofThisEvent* hitscollections = 0 ;

//  try{ *gigaSvc() >> hitscollections ; }
//  catch( const GaudiException& Excpt  ) 
//    { return Exception("UpdateObjRefs: " , Excpt ) ; }  
//  catch( const std::exception& Excpt  ) 
//    { return Exception("UpdateObjRefs: " , Excpt ) ; }  
//  catch( ... )                          
//    { return Exception("UpdateObjRefs: "         ) ; }  
//  if( 0 == hitscollections      ) 
//    {             
//      MsgStream logg(   msgSvc(), name());
//      logg << MSG::INFO 
//           << "No G4HCOfThisEvent found" << endreq;      
//      return StatusCode::SUCCESS;
//    }
//  
//  { // be sure that MC particles are already converted! 
    // just to force the loading of the reference table 
    // get the registry
//    const IRegistry* registry   = address->registry();
//    if( 0 == registry ) { return Error("IRegistry* points to NULL!");}
//    const std::string& location = registry->identifier();
//    std::string mcpath = 
//      location.substr( 0, location.find( "/MC" ) ) + "/" + 
//      MCParticleLocation::Default;
    // get MCparticles 
//    const MCParticles* mcps = get( dataProvider() , mcpath , mcps );
//   if( 0 == mcps ) 
//    { return Error("Can not locate MCparticles at '" + mcpath + "'");}   
//  }
                                        //
                                        // 
  //  std::string colname = *(address->par());  
  //  std::string colname[2];
  // colname[0] = "Rich1TopHC";
  // colname[1] = "Rich1BotHC";
                                        //  
  /// get table 
  //  GiGaKineRefTable& table = hitsSvc()->table();
                                        //
  //  for (int ii=0;ii<m_RichG4HitCollectionName->RichHCSize();ii++)
  //  {
  //   G4SDManager* fSDM=G4SDManager::GetSDMpointer();
  //  int collectionID=fSDM->
  //      GetCollectionID(m_RichG4HitCollectionName->RichHCName(ii));
                                        // 
  //    RichG4HitsCollection* myCollection = 
  //    (RichG4HitsCollection*)(hitscollections->GetHC(collectionID));
  //  if(0==myCollection || collectionID==-1) 
  //    {
        //      return Error("No RichG4HitsCollection* object found");
  //       return StatusCode::SUCCESS;
  //    }
                                        //
  //      int numberofhits=myCollection->entries();
  //
  //
//        if( (size_t) hits->size() != (size_t) myCollection->entries() ) 
//          {
//            return Error
//         ("MCRichPhotodetectorHits and G4RichG4HitsCollection have different sizes!");
//          }
  // 
      // fill the references
  //     
  //
      //        for( int itr = 0 ; itr<numberofhits ; ++itr )
      //       {
      //          int traid=(*myCollection)[itr]->GetTrackID();
      //           if(table[traid].particle())              
      //           {
      //            ((*myCollection)[itr])->
      //              setMCParticle(table[traid].particle());
      //           }
                 
      //       else
      //         {
                  //           MsgStream log(   msgSvc(), name());
                  //         log << MSG::INFO 
                  //    << "No pointer to MCParticle for MCRichPhotodetectorHit associated to trackID: " 
                  //    <<ii<<"  "<<itr<< "   "<< traid << endreq;
          
                  //       }
                                        //                

      //                 
                                        //                
      //     }
        
                                        //        
        
      //  }
                                        //
              
                                        //
              //      for (MCRichPhotodetectorHits::const_iterator iter=hits->begin() ; 
              //                           hits->end()!=iter ; ++iter)
              //     { 
//            int traid=(*myCollection)[itr]->GetTrackID();

//            if(table[traid].particle())
//              {
//                (*iter)->setMCParticle(table[traid].particle());
//              }
//            else
//              {
//                MsgStream log(   msgSvc(), name());
//                log << MSG::INFO 
//                    << "No pointer to MCParticle for MCRichPhotodetectorHit associated to trackID: " 
//                    << traid << endreq;
          
//              }
      
//            std::cout << "TrackID: " << traid << " entry of MCParticle: " 
//                      << (*iter)->entry()  << std::endl;    
//      
//            itr++;
//        }
  //    }
  ///
                                          //  return StatusCode::SUCCESS;
  ///
                                        //};

















































