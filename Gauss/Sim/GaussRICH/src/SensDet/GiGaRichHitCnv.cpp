//  ===========================================================================
#define GIGACNV_GiGaRichHitCnv_CPP 1
// ============================================================================
#include "CLHEP/Geometry/Point3D.h"
// STL
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
// GaudiKernel
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/IAddressCreator.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
// GaudiKernel
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/GaudiException.h"
// GiGa
#include "GiGa/IGiGaSvc.h"
#include "GiGaCnv/IGiGaHitsCnvSvc.h"
#include "GiGa/GiGaTrajectory.h"
#include "GiGa/GiGaUtil.h"
#include "GiGa/GiGaHitsByID.h"
#include "GiGa/GiGaHitsByName.h"

// GiGaCnv
#include "GiGaCnv/GiGaKineRefTable.h"
#include "GiGaCnv/GiGaCnvUtils.h"
#include "GiGaCnv/IGiGaHitsCnvSvc.h"
#include "GiGaCnv/IGiGaKineCnvSvc.h"
#include "GiGaCnv/GiGaKineRefTable.h"

// LHCbEvent
#include "Event/MCRichHit.h"
// Geant4 includes
#include "G4VHitsCollection.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
// local
#include "GiGaRichHitCnv.h"
#include "RichG4HitCollName.h"
#include "RichG4Hit.h"

// ======================================================================

static const  CnvFactory<GiGaRichHitCnv>         s_Factory ;
const        ICnvFactory&GiGaRichHitCnvFactory = s_Factory ;

// ======================================================================

GiGaRichHitCnv::GiGaRichHitCnv( ISvcLocator* Locator )
  : GiGaCnvBase( storageType() , classID() , Locator ),
    m_RichG4HitCollectionName(0) {

  setNameOfGiGaConversionService( IGiGaCnvSvcLocation::Hits ) ;
  setConverterName              ( "GiGaRichHitCnv"          ) ;

  GiGaLeaf::Pars pars1;
  pars1.push_back("");
  declareObject(GiGaLeaf( MCRichHitLocation::Default, objType(), pars1));

  m_RichG4HitCollectionName = new RichG4HitCollName();

};

// ======================================================================

GiGaRichHitCnv::~GiGaRichHitCnv(){

  delete m_RichG4HitCollectionName;
  
};

// ======================================================================

const CLID& GiGaRichHitCnv::classID() { return MCRichHit::classID(); }

// ======================================================================

const unsigned char GiGaRichHitCnv::storageType() { 
   return GiGaHits_StorageType; }

// ======================================================================

StatusCode GiGaRichHitCnv::initialize() {

  // initialize the base class
  StatusCode sc = GiGaCnvBase::initialize();
  if ( sc.isFailure() ) return Error("Could not initialize the base class!",sc);
  // check for necessary services
  if ( 0 == hitsSvc() ) return Error("IGiGaHitsCnvSvc* points to NULL!");

  return StatusCode::SUCCESS;
}

// ======================================================================

StatusCode GiGaRichHitCnv::finalize() { return GiGaCnvBase::finalize(); }

// ======================================================================

StatusCode GiGaRichHitCnv::createObj( IOpaqueAddress*  address ,
                                      DataObject*&     object  ) {

  object = 0;
  if ( 0 == address ) return Error("IOpaqueAddress* points to NULL!" );

  object = new MCRichHits();
  StatusCode sc = updateObj(address,object);
  
  if ( sc.isFailure() ) {
    if ( 0 != object ) { delete object; object = 0; }
    return Error( "Could not create and update Object", sc );
  };
  

  return StatusCode::SUCCESS;
};

// ============================================================================

StatusCode GiGaRichHitCnv::fillObjRefs
( IOpaqueAddress*  address , 
  DataObject*      object  ) 
{
  // This is completely dummy at the moment since the updateObjREf is
  //  dummy. But for now kept for future if the UpdateObjref becomes not
  // dummy. If in the final version it is dummy, this can be removed.
  //
  if( 0 ==   address   ) { return Error(" IOpaqueAddress* points to NULL" );}
  if( 0 ==   object    ) { return Error(" DataObject* points to NULL"     );}   

  MCRichHits* hits = dynamic_cast<MCRichHits*> ( object );

  if ( 0 == hits    ) {
    return Error( " GiGaRichHitCnv: DataObject*(of type '" + 
                   GiGaUtil::ObjTypeName(object) +
                  "*') is not 'MCRichHits*'! "   );
  }

  return updateObjRefs( address , object );

  
};


StatusCode GiGaRichHitCnv::updateObj ( IOpaqueAddress*  address ,
                                       DataObject*      object  ) {

  if ( 0 == address ) { return Error(" IOpaqueAddress* points to NULL"); }
  if ( 0 == object  ) { return Error(" DataObject* points to NULL"    ); }
  MCRichHits* hits = dynamic_cast<MCRichHits*> ( object );
  if ( 0 == hits    ) {
    return Error( " DataObject*(of type '" + GiGaUtil::ObjTypeName(object) +
                  "*') is not 'MCRichHits*'! "   );
  }

  // Initialise
  MsgStream msg( msgSvc(), name() );
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

  try {

    // get hitscollections from GiGa
    *gigaSvc() >> hitscollections;

    if ( 0 != hitscollections ) {

      // note this key is need for consistency with MCRichOpticalPhoton converter
      int globalKey = 0; 
      for (int iii=0;iii<m_RichG4HitCollectionName->RichHCSize() ;iii++) {
        

              std::string colName = m_RichG4HitCollectionName->RichHCName(iii);

              G4SDManager * fSDM = G4SDManager::GetSDMpointer();
              if ( !fSDM ) return Error( "GiGaRichHitCnv : NULL G4SDManager pointer !!" );
              int collectionID = fSDM->GetCollectionID(colName);
              if ( -1 == collectionID ) return StatusCode::SUCCESS;

        RichG4HitsCollection* myCollection =
          (RichG4HitsCollection*)(hitscollections->GetHC(collectionID));

        if ( 0 == myCollection ) return StatusCode::SUCCESS;
        
         int numberofhits=myCollection->entries();

        // reserve space
        hits->reserve(  numberofhits );

        // Reference table between G4 tracks/trajectories and MC particles
        const GiGaKineRefTable& table = hitsSvc()->table() ;

        //convert hits
        for ( int ihit = 0 ; ihit< numberofhits ; ++ihit ) {

          RichG4Hit * g4hit = (*myCollection)[ihit];
          HepPoint3D pentry=(*myCollection)[ihit]->GetGlobalPos();
          double edep=(*myCollection)[ihit]->GetEdep();
          double toffl=(*myCollection)[ihit]-> RichHitGlobalTime();

          MCRichHit * mchit = new MCRichHit();
          mchit->setEntry(pentry);
          mchit->setEnergy(edep);
          mchit->setTimeOfFlight(toffl);
          // Now for the Rich Specific word. SE July 2003.
          // first reset the value.
          mchit->setHistoryCode(0);
          mchit->setRich((g4hit->GetCurRichDetNum() ) < 0 ? 2 :
                   ((unsigned long int ) g4hit->GetCurRichDetNum()) );
          mchit->setPhotoDetector((unsigned long int )g4hit->  GetCurHpdNum());
          mchit->setRadiator((g4hit-> GetRadiatorNumber() )< 0 ? 3 :
                          ((unsigned long int ) g4hit-> GetRadiatorNumber()) );
          
          mchit->setChargedTrack( (g4hit->GetChTrackID() ) < 0 ? 1 : 0);
          

          //end of filling the Rich Specific word.
          int traid=(*myCollection)[ihit]->GetTrackID();

          if(table[traid].particle())
           {
             mchit->setMCParticle(table[traid].particle());
           }else{
             MsgStream log(   msgSvc(), name());
                              log << MSG::INFO 
                               << "No pointer to MCParticle for " 
                               <<" MCRichHit associated to trackID: " 
                      <<iii<<"  "<<ihit<< "   "<< traid << endreq;
            }


          hits->insert( mchit, globalKey );
          globalKey++;
          
        }
        
        
      }
      
    
    
          msg << MSG::INFO << "Built " << hits->size()
          << " MCRichHits at " << MCRichHitLocation::Default << endreq;

    } else {
      msg << MSG::INFO << "No RichG4Hits to be converted since no Collections" 
          << endreq;
      return StatusCode::SUCCESS;
    }

  } // end try

  catch( const GaudiException& Excp ) {
    hits->clear();
    return Exception( "updateObj: ", Excp ) ;
  }
  catch( const std::exception& Excp ) {
    hits->clear();
    return Exception( "updateObj: ", Excp ) ;
  }
  catch( ... ) {
    hits->clear();
    return Exception( "updateObj: " ) ;
  }

  return StatusCode::SUCCESS;
};

StatusCode GiGaRichHitCnv::updateObjRefs
( IOpaqueAddress*  address , 
  DataObject*      object  ) {


  return StatusCode::SUCCESS;
  
};
