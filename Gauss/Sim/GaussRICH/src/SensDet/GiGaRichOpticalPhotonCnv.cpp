//  ===========================================================================
#define GIGACNV_GiGaRichOpticalPhotonCnv_CPP 1
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
// Rich Event
#include "Event/MCRichOpticalPhoton.h"
// Geant4 includes
#include "G4VHitsCollection.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
// local
#include "GiGaRichOpticalPhotonCnv.h"
#include "RichG4HitCollName.h"
#include "RichG4Hit.h"

// ======================================================================

static const  CnvFactory<GiGaRichOpticalPhotonCnv>         s_Factory ;
const        ICnvFactory&GiGaRichOpticalPhotonCnvFactory = s_Factory ;

// ========================================================================

GiGaRichOpticalPhotonCnv::GiGaRichOpticalPhotonCnv( ISvcLocator* Locator )
  : GiGaCnvBase( storageType() , classID() , Locator ),
    m_RichG4HitCollectionName(0) {

  setNameOfGiGaConversionService( IGiGaCnvSvcLocation::Hits ) ;
  setConverterName              ( "GiGaRichOpticalPhotonCnv"          ) ;

  GiGaLeaf::Pars pars1;
  pars1.push_back("");
  declareObject(GiGaLeaf( MCRichOpticalPhotonLocation::Default, 
         objType(), pars1));
  

  m_RichG4HitCollectionName = new RichG4HitCollName();

};

// ======================================================================

GiGaRichOpticalPhotonCnv::~GiGaRichOpticalPhotonCnv(){
  delete  m_RichG4HitCollectionName;
  
};

// ======================================================================

const CLID& GiGaRichOpticalPhotonCnv::classID() { 
  return MCRichOpticalPhoton::classID(); }

// ======================================================================

const unsigned char GiGaRichOpticalPhotonCnv::storageType() { 
    return GiGaHits_StorageType; }

// ======================================================================

StatusCode GiGaRichOpticalPhotonCnv::initialize() {

  // initialize the base class
  StatusCode sc = GiGaCnvBase::initialize();
  if ( sc.isFailure() ) return Error("Could not initialize the base class!",sc);
  // check for necessary services
  if ( 0 == hitsSvc() ) return Error("IGiGaHitsCnvSvc* points to NULL!");

  return StatusCode::SUCCESS;
}

// ======================================================================

StatusCode GiGaRichOpticalPhotonCnv::finalize() { 
            return GiGaCnvBase::finalize(); }

// ======================================================================

StatusCode GiGaRichOpticalPhotonCnv::createObj( IOpaqueAddress*  address ,
                                                DataObject*&     object  ) {

  object = 0;
  if ( 0 == address ) return Error("IOpaqueAddress* points to NULL!" );

  object = new MCRichOpticalPhotons();
  StatusCode sc = updateObj( address, object );
  if ( !sc ) {
    if ( 0 != object ) { delete object; object = 0; }
    return Warning( "Could not create and update Object", sc );
  }


  return StatusCode::SUCCESS;
};

// ============================================================================

StatusCode GiGaRichOpticalPhotonCnv::updateObj ( IOpaqueAddress*  address ,
                                                 DataObject*      object  ) {

  if ( 0 == address ) { return Error(" IOpaqueAddress* points to NULL"); }
  if ( 0 == object  ) { return Error(" DataObject* points to NULL"    ); }
  MCRichOpticalPhotons* photons = dynamic_cast<MCRichOpticalPhotons*> ( object );
  if ( 0 == photons ) {
    return Error( " DataObject*(of type '" + GiGaUtil::ObjTypeName(object) +
                  "*') is not 'MCRichOpticalPhotons*'! "   );
  }


  // Initialise
  MsgStream msg( msgSvc(), name() );
  photons->clear();

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


    G4HCofThisEvent* hitscollections = 0 ;

  try {

    // retrieve the hits container from GiGa Service

    // get hitscollections from GiGa
    
    *gigaSvc() >> hitscollections;

    if ( 0 != hitscollections ) {

      // Get MCRichHits eventSvc()
      SmartDataPtr<MCRichHits> mcHits( evtSvc(), MCRichHitLocation::Default );
      if ( !mcHits ) {
      return Warning( "Could not locate  MCRichHits from GiGaRichOpticalPhoton" );
      }
      
//       msg << MSG::INFO << "Located " << mcHits->size() << " MCHits at " 
//           << MCRichHitLocation::Default << endreq;

      // note this key is need for consistency with MCRichOpticalPhoton converter
      int globalKey = 0;
      for (int iii=0;iii<m_RichG4HitCollectionName->RichHCSize() ;iii++) {
           std::string colName = m_RichG4HitCollectionName->RichHCName(iii);


        G4SDManager * fSDM = G4SDManager::GetSDMpointer();
        if ( !fSDM ) return Error( "NULL G4SDManager pointer !!" );
        int collectionID = fSDM->GetCollectionID(colName);
        if ( -1 == collectionID ) return StatusCode::SUCCESS;

        RichG4HitsCollection* myCollection =
          (RichG4HitsCollection*)(hitscollections->GetHC(collectionID));
        if ( 0 == myCollection ) return StatusCode::SUCCESS;

          int numberofhits = myCollection->entries();
       // reserve space
        photons->reserve(  numberofhits );

        // Reference table between G4 tracks/trajectories and MC particles
        const GiGaKineRefTable& table = hitsSvc()->table() ;


        //convert hits
        for ( int ihit = 0 ; ihit< numberofhits  ; ++ihit ) {
          RichG4Hit * g4hit = (*myCollection)[ihit];

          MCRichOpticalPhoton * mcPhoton = new MCRichOpticalPhoton();

          // Copy required info from RichG4Hit to RichMCOpticalPhoton
          // More info may be copied in the future.
          //

          mcPhoton-> setPdIncidencePoint( HepPoint3D(
                                   g4hit->GetGlobalPEOriginPos().x(),  
                                   g4hit->GetGlobalPEOriginPos().y(),
                                   g4hit->GetGlobalPEOriginPos().z() ) );
          mcPhoton->setEnergyAtProduction( (float) g4hit-> PhotEnergyAtProd());
          
          mcPhoton->setEmissionPoint
             ( HepPoint3D(g4hit->GetPhotEmisPt().x(),
                                g4hit->GetPhotEmisPt().y(),
                                g4hit->GetPhotEmisPt().z())  );
          mcPhoton->setParentMomentum
             (HepPoint3D( g4hit->ChTrackMomVect().x(),
                                g4hit->ChTrackMomVect().y(),
                                g4hit->ChTrackMomVect().z() ));
          
          mcPhoton->setCherenkovTheta( g4hit->ThetaCkvAtProd() );
          mcPhoton->setCherenkovPhi( g4hit->PhiCkvAtProd() );
          mcPhoton->setMcRichHit( mcHits->object(globalKey) );

          photons->insert( mcPhoton, globalKey );
          globalKey++;
          
        }
        
        
      }
      
//       msg << MSG::INFO 
//           << "Built " << photons->size() << " MCRichOpticalPhotons at "
//           << MCRichOpticalPhotonLocation::Default << endreq;

      // Should have one opticalphoton for each and every MCRichHit
      if ( photons->size() != mcHits->size() ) {
        msg << MSG::ERROR << "Created " << photons->size() << " photons and"
            << mcHits->size() << " MCRichHits !!" << endreq;
      }
      
    } else {
      msg << MSG::INFO << "No RichG4OpticalPhotons to be converted" << endreq;
      return StatusCode::SUCCESS;
    }

  } // end try

  catch( const GaudiException& Excp ) {
    photons->clear();
    return Exception( "updateObj: ", Excp ) ;
  }
  catch( const std::exception& Excp ) {
    photons->clear();
    return Exception( "updateObj: ", Excp ) ;
  }
  catch( ... ) {
    photons->clear();
    return Exception( "updateObj: " ) ;
  }

  return StatusCode::SUCCESS;
};

