//  ===========================================================================
#define GIGACNV_GiGaMCRichHitCnv_CPP 1
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
#include "GiGaMCRichHitCnv.h"
#include "RichG4HitCollName.h"
#include "RichG4Hit.h"

// ======================================================================

static const  CnvFactory<GiGaMCRichHitCnv>         s_Factory ;
const        ICnvFactory&GiGaMCRichHitCnvFactory = s_Factory ;

// ======================================================================

GiGaMCRichHitCnv::GiGaMCRichHitCnv( ISvcLocator* Locator )
  : GiGaCnvBase( storageType() , classID() , Locator ),
    m_RichG4HitCollectionName(0) {

  setNameOfGiGaConversionService( IGiGaCnvSvcLocation::Hits ) ;
  setConverterName              ( "GiGaMCRichHitCnv"        ) ;

  GiGaLeaf::Pars pars1;
  pars1.push_back("");
  declareObject(GiGaLeaf( MCRichHitLocation::Default, objType(), pars1));

  m_RichG4HitCollectionName = new RichG4HitCollName();

};

// ======================================================================

GiGaMCRichHitCnv::~GiGaMCRichHitCnv(){

  delete m_RichG4HitCollectionName;

};

// ======================================================================

const CLID& GiGaMCRichHitCnv::classID() { return MCRichHit::classID(); }

// ======================================================================

const unsigned char GiGaMCRichHitCnv::storageType() {
  return GiGaHits_StorageType; }

// ======================================================================

StatusCode GiGaMCRichHitCnv::initialize() {

  // initialize the base class
  StatusCode sc = GiGaCnvBase::initialize();
  if ( sc.isFailure() ) return Error("Could not initialize the base class!",sc);
  // check for necessary services
  if ( 0 == hitsSvc() ) return Error("IGiGaHitsCnvSvc* points to NULL!");

  return StatusCode::SUCCESS;
}

// ======================================================================

StatusCode GiGaMCRichHitCnv::finalize() { return GiGaCnvBase::finalize(); }

// ======================================================================

StatusCode GiGaMCRichHitCnv::createObj( IOpaqueAddress*  address ,
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

StatusCode GiGaMCRichHitCnv::fillObjRefs
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
    return Error( " GiGaMCRichHitCnv: DataObject*(of type '" +
                  GiGaUtil::ObjTypeName(object) +
                  "*') is not 'MCRichHits*'! "   );
  }
  return updateObjRefs( address , object );
};


StatusCode GiGaMCRichHitCnv::updateObj ( IOpaqueAddress*  address ,
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
  msg << MSG::DEBUG << "Creating MCRichHits" << endreq;
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
  { return Error( "Can not locate MCparticles at '" + mcpath + "'");}
  //  }

  // retrieve the hits container from GiGa Service
  G4HCofThisEvent* hitscollections = 0 ;

  try {

    // get hitscollections from GiGa
    *gigaSvc() >> hitscollections;

    if ( 0 != hitscollections ) {

      // note this key is need for consistency with MCRichOpticalPhoton converter
      int globalKey = 0;
      for ( int iii=0; iii<m_RichG4HitCollectionName->RichHCSize(); ++iii ) {

        std::string colName = m_RichG4HitCollectionName->RichHCName(iii);

        G4SDManager * fSDM = G4SDManager::GetSDMpointer();
        if ( !fSDM ) return Error( "GiGaMCRichHitCnv : NULL G4SDManager pointer !!" );
        int collectionID = fSDM->GetCollectionID(colName);
        if ( -1 == collectionID ) return StatusCode::SUCCESS;

        RichG4HitsCollection* myCollection =
          dynamic_cast<RichG4HitsCollection*>(hitscollections->GetHC(collectionID));

        if ( 0 == myCollection ) return StatusCode::SUCCESS;

        const int numberofhits = myCollection->entries();

        // reserve space
        hits->reserve( numberofhits );

        // Reference table between G4 tracks/trajectories and MC particles
        const GiGaKineRefTable & table = hitsSvc()->table() ;

        //convert hits
        for ( int ihit = 0; ihit < numberofhits; ++ihit ) {

          // Pointer to G4 hit
          RichG4Hit * g4hit = (*myCollection)[ihit];

          // Make new persistent hit object
          MCRichHit * mchit = new MCRichHit();

          // Set data
          mchit->setEntry( g4hit->GetGlobalPos() );
          mchit->setEnergy( g4hit->GetEdep() );
          mchit->setTimeOfFlight( g4hit->RichHitGlobalTime() );

          // Now for the Rich Specific word.
          mchit->setHistoryCode(0);
          if ( g4hit->GetCurRichDetNum() < 0 ) {
            mchit->setRichInfoValid( false );
          } else {
            mchit->setRichInfoValid( true );
            mchit->setRich(static_cast<Rich::DetectorType>(g4hit->GetCurRichDetNum()));
          }
          mchit->setPhotoDetector(g4hit->GetCurHpdNum());
          if ( g4hit->GetRadiatorNumber() < 0 ) {
            mchit->setRadiatorInfoValid( false );
          } else {
            mchit->setRadiatorInfoValid( true );
            mchit->setRadiator(static_cast<Rich::RadiatorType>(g4hit->GetRadiatorNumber()));
          }
          mchit->setChargedTrack( g4hit->GetChTrackID() < 0 );
          //end of filling the Rich Specific word.

          // get MCParticle information
          const int traid = g4hit->GetTrackID();
          if ( table[traid].particle() ) {
            mchit->setMCParticle( table[traid].particle() );
          } else {
            msg << MSG::INFO
                << "No pointer to MCParticle for "
                <<" MCRichHit associated to trackID: "
                <<iii<<"  "<<ihit<< "   "<< traid << endreq;
          }

          // add to container
          hits->insert( mchit, globalKey );
          ++globalKey;

        }
      }
      //           msg << MSG::INFO << "Built " << hits->size()
      //           << " MCRichHits at " << MCRichHitLocation::Default << endreq;
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
}

StatusCode GiGaMCRichHitCnv::updateObjRefs( IOpaqueAddress*,
                                            DataObject*  )
{
  return StatusCode::SUCCESS;
}
