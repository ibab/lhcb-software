//  ===========================================================================
#define GIGACNV_GiGaMCRichOpticalPhotonCnv_CPP 1
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
#include "GiGaMCRichOpticalPhotonCnv.h"
#include "RichG4HitCollName.h"
#include "RichG4Hit.h"

// ======================================================================

static const  CnvFactory<GiGaMCRichOpticalPhotonCnv>         s_Factory ;
const        ICnvFactory&GiGaMCRichOpticalPhotonCnvFactory = s_Factory ;

// ========================================================================

GiGaMCRichOpticalPhotonCnv::GiGaMCRichOpticalPhotonCnv( ISvcLocator* Locator )
  : GiGaCnvBase( storageType() , classID() , Locator ),
    m_RichG4HitCollectionName(0) {

  setNameOfGiGaConversionService( IGiGaCnvSvcLocation::Hits    ) ;
  setConverterName              ( "GiGaMCRichOpticalPhotonCnv" ) ;

  GiGaLeaf::Pars pars1;
  pars1.push_back("");
  declareObject( GiGaLeaf( MCRichOpticalPhotonLocation::Default,
                           objType(), pars1) );


  m_RichG4HitCollectionName = new RichG4HitCollName();

};

// ======================================================================

GiGaMCRichOpticalPhotonCnv::~GiGaMCRichOpticalPhotonCnv()
{
  delete m_RichG4HitCollectionName;
};

// ======================================================================

const CLID& GiGaMCRichOpticalPhotonCnv::classID()
{
  return MCRichOpticalPhoton::classID();
}

// ======================================================================

const unsigned char GiGaMCRichOpticalPhotonCnv::storageType()
{
  return GiGaHits_StorageType;
}

// ======================================================================

StatusCode GiGaMCRichOpticalPhotonCnv::initialize() {

  // initialize the base class
  StatusCode sc = GiGaCnvBase::initialize();
  if ( sc.isFailure() ) return Error("Could not initialize the base class!",sc);

  // check for necessary services
  if ( 0 == hitsSvc() ) return Error("IGiGaHitsCnvSvc* points to NULL!");

  return StatusCode::SUCCESS;
}

// ======================================================================

StatusCode GiGaMCRichOpticalPhotonCnv::finalize()
{
  return GiGaCnvBase::finalize();
}

// ======================================================================

StatusCode GiGaMCRichOpticalPhotonCnv::createObj( IOpaqueAddress*  address ,
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

StatusCode GiGaMCRichOpticalPhotonCnv::updateObj ( IOpaqueAddress*  address ,
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
  msg << MSG::DEBUG << "Creating MCRichOpticalPhotons" << endreq;
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
        return Warning( "Could not locate MCRichHits from GiGaMCRichOpticalPhotonCnv" );
      }

      // note this key is need for consistency with MCRichHit converter
      int globalKey = 0;
      for ( int iii=0; iii<m_RichG4HitCollectionName->RichHCSize(); ++iii ) {
        std::string colName = m_RichG4HitCollectionName->RichHCName(iii);

        G4SDManager * fSDM = G4SDManager::GetSDMpointer();
        if ( !fSDM ) return Error( "NULL G4SDManager pointer !!" );
        int collectionID = fSDM->GetCollectionID(colName);
        if ( -1 == collectionID ) return StatusCode::SUCCESS;

        RichG4HitsCollection * myCollection =
          dynamic_cast<RichG4HitsCollection*>(hitscollections->GetHC(collectionID));
        if ( 0 == myCollection ) return StatusCode::SUCCESS;

        int numberofhits = myCollection->entries();
        // reserve space
        photons->reserve( numberofhits );

        // Reference table between G4 tracks/trajectories and MC particles
        //const GiGaKineRefTable& table = hitsSvc()->table();

        //convert hits
        for ( int ihit = 0; ihit < numberofhits; ++ihit ) {

          // Pointer to G4 hit
          RichG4Hit * g4hit = (*myCollection)[ihit];

          // New optical photon object
          MCRichOpticalPhoton * mcPhoton = new MCRichOpticalPhoton();

          // insert in container
          photons->insert( mcPhoton, globalKey );

          // Copy required info from RichG4Hit to RichMCOpticalPhoton

          // Incidence point on HPD
          mcPhoton-> setPdIncidencePoint( HepPoint3D(g4hit->GetGlobalPEOriginPos().x(),
                                                     g4hit->GetGlobalPEOriginPos().y(),
                                                     g4hit->GetGlobalPEOriginPos().z() ) );

          // Photon energy at production
          mcPhoton->setEnergyAtProduction( static_cast<float>(g4hit->PhotEnergyAtProd()) );

          // Emission point on track
          mcPhoton->setEmissionPoint( HepPoint3D(g4hit->GetPhotEmisPt().x(),
                                                 g4hit->GetPhotEmisPt().y(),
                                                 g4hit->GetPhotEmisPt().z()) );

          // The momentum of the parent track at production
          mcPhoton->setParentMomentum( HepPoint3D( g4hit->ChTrackMomVect().x(),
                                                   g4hit->ChTrackMomVect().y(),
                                                   g4hit->ChTrackMomVect().z() ) );

          // Cherenkov theta and phi at production
          mcPhoton->setCherenkovTheta ( g4hit->ThetaCkvAtProd() );
          mcPhoton->setCherenkovPhi   ( g4hit->PhiCkvAtProd()   );

          // Spherical mirror reflection point
          mcPhoton->setSphericalMirrorReflectPoint
            ( HepPoint3D( g4hit->Mirror1PhotonReflPosition().x(),
                          g4hit->Mirror1PhotonReflPosition().y(),
                          g4hit->Mirror1PhotonReflPosition().z() ) );

          // Flat mirror reflection point
          mcPhoton->setFlatMirrorReflectPoint
            ( HepPoint3D( g4hit->Mirror2PhotonReflPosition().x(),
                          g4hit->Mirror2PhotonReflPosition().y(),
                          g4hit->Mirror2PhotonReflPosition().z() ) );

          // Rich detector information
          if ( g4hit->GetCurRichDetNum() < 0 ) {
            //            mcPhoton->setRichInfoValid( false );
          } else {
            //   mcPhoton->setRichInfoValid( true );
            mcPhoton->setRich(static_cast<Rich::DetectorType>(g4hit->GetCurRichDetNum()));
          }

          // Photon detector number
          mcPhoton->setPhotoDetector(g4hit->GetCurHpdNum());

          // Radiator information
          if ( g4hit->GetRadiatorNumber() < 0 ) {
            //       mcPhoton->setRadiatorInfoValid( false );
          } else {
            //   mcPhoton->setRadiatorInfoValid( true );
            mcPhoton->setRadiator(static_cast<Rich::RadiatorType>(g4hit->GetRadiatorNumber()));
          }

          // charged track hitting HPD flag
          mcPhoton->setChargedTrack( g4hit->GetChTrackID() < 0 );

          // Rayleigh scattered flag
          mcPhoton->setScatteredPhoton( g4hit->OptPhotRayleighFlag() > 0 );

          // Overall background flag
          mcPhoton->setBackgroundHit( mcPhoton->chargedTrack() ||
                                      mcPhoton->scatteredPhoton());
          
                                      //   ||
//                                      !mcPhoton->richInfoValid() );

          // SmartRef to associated MCRichHit
          mcPhoton->setMcRichHit( mcHits->object(globalKey) );

          // finally, increment the key
          ++globalKey;

        } // loop over g4 hits

      }

      // Should have one opticalphoton for each and every MCRichHit
      if ( photons->size() != mcHits->size() ) {
        msg << MSG::ERROR << "Created " << photons->size()
            << " MCRichOpticalPhotons and"
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

