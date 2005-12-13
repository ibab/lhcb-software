
/** @file GiGaMCRichHitCnv.cpp
 *
 *  Implementation file for GiGa converter : GiGaMCRichHitCnv
 *
 *  CVS History :
 *  $Id: GiGaMCRichHitCnv.cpp,v 1.14 2005-12-13 17:31:17 seaso Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2004-03-29
 */

//  ===========================================================================
#define GIGACNV_GiGaMCRichHitCnv_CPP 1
// ============================================================================

// CLHEP
#include "CLHEP/Geometry/Point3D.h"

// GaudiKernel
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/IAddressCreator.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/GaudiException.h"

// GiGa
#include "GiGa/IGiGaSvc.h"
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

// RichKernel
#include "RichKernel/RichStatDivFunctor.h"

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
    m_RichG4HitCollectionName ( 0 ),
    m_nEvts                   ( 0 ),
    m_hitTally                ( Rich::NRiches, 0 ),
    m_radHits                 ( Rich::NRadiatorTypes, 0 ),
    m_invalidRadHits          ( Rich::NRiches, 0 ),
    m_invalidRichHits         ( 0 ),
    m_ctkHits                 ( Rich::NRadiatorTypes, 0 ),
    m_scatHits                ( Rich::NRadiatorTypes, 0 ),
    m_bkgHits                 ( Rich::NRadiatorTypes, 0 ),
    m_nomcpHits               ( Rich::NRadiatorTypes, 0 )
{

  setNameOfGiGaConversionService( IGiGaCnvSvcLocation::Hits ) ;
  setConverterName              ( "GiGaMCRichHitCnv"        ) ;

  GiGaLeaf::Pars pars1;
  pars1.push_back("");
  declareObject(GiGaLeaf( MCRichHitLocation::Default, objType(), pars1));

  m_RichG4HitCollectionName = new RichG4HitCollName();

};

// ======================================================================

GiGaMCRichHitCnv::~GiGaMCRichHitCnv()
{
  delete m_RichG4HitCollectionName;
};

// ======================================================================

const CLID& GiGaMCRichHitCnv::classID()
{
  return MCRichHit::classID();
}

// ======================================================================

const unsigned char GiGaMCRichHitCnv::storageType()
{
  return GiGaHits_StorageType;
}

// ======================================================================

StatusCode GiGaMCRichHitCnv::initialize()
{
  // initialize the base class
  const StatusCode sc = GiGaCnvBase::initialize();
  if ( sc.isFailure() ) return Error("Could not initialize the base class!",sc);
  // check for necessary services
  if ( 0 == hitsSvc() ) return Error("IGiGaHitsCnvSvc* points to NULL!");

  return sc;
}

// ======================================================================

StatusCode GiGaMCRichHitCnv::finalize()
{
  // Printout final numbers
  const RichStatDivFunctor occ;
  MsgStream msg( msgSvc(), name() );

  msg << MSG::ALWAYS 
      << "Av. # Invalid RICH flags           = " << occ(m_invalidRichHits,m_nEvts)
      << endreq
      << "Av. # Invalid rad. flags   : Rich1 = " << occ(m_invalidRadHits[Rich::Rich1],m_nEvts)
      << " Rich2 = " << occ(m_invalidRadHits[Rich::Rich2],m_nEvts)
      << endreq
      << "Av. # MCRichHits           : Rich1 = "
      << occ(m_hitTally[Rich::Rich1],m_nEvts)
      << " Rich2 = " << occ(m_hitTally[Rich::Rich2],m_nEvts)
      << endreq
      << "Av. # MCRichHits           : Aero  = " << occ(m_radHits[Rich::Aerogel],m_nEvts)
      << " C4F10 = " <<  occ(m_radHits[Rich::C4F10],m_nEvts)
      << " CF4 = "   <<  occ(m_radHits[Rich::CF4],m_nEvts)
      << endreq
      << "Av. # Charged Track hits   : Aero  = " << occ(m_ctkHits[Rich::Aerogel],m_nEvts)
      << " C4F10 = " <<  occ(m_ctkHits[Rich::C4F10],m_nEvts)
      << " CF4 = "   <<  occ(m_ctkHits[Rich::CF4],m_nEvts)
      << endreq
      << "Av. # Scattered hits       : Aero  = " << occ(m_scatHits[Rich::Aerogel],m_nEvts)
      << " C4F10 = " <<  occ(m_scatHits[Rich::C4F10],m_nEvts)
      << " CF4 = "   <<  occ(m_scatHits[Rich::CF4],m_nEvts)
      << endreq
      << "Av. # background hits      : Aero  = " << occ(m_bkgHits[Rich::Aerogel],m_nEvts)
      << " C4F10 = " <<  occ(m_bkgHits[Rich::C4F10],m_nEvts)
      << " CF4 = "   <<  occ(m_bkgHits[Rich::CF4],m_nEvts)
      << endreq
      << "Av. # MCParticle-less hits : Aero  = " << occ(m_nomcpHits[Rich::Aerogel],m_nEvts)
      << " C4F10 = " <<  occ(m_nomcpHits[Rich::C4F10],m_nEvts)
      << " CF4 = "   <<  occ(m_nomcpHits[Rich::CF4],m_nEvts)
      << endreq;

  // finalise base class and return
  return GiGaCnvBase::finalize();
}

// ======================================================================

StatusCode GiGaMCRichHitCnv::createObj( IOpaqueAddress*  address ,
                                        DataObject*&     object  )
{
  object = 0;
  if ( 0 == address ) return Error("IOpaqueAddress* points to NULL!" );

  object = new MCRichHits();
  const StatusCode sc = updateObj(address,object);

  if ( sc.isFailure() ) {
    if ( 0 != object ) { delete object; object = 0; }
    return Error( "Could not create and update Object", sc );
  };

  return StatusCode::SUCCESS;
};

// ============================================================================

StatusCode GiGaMCRichHitCnv::fillObjRefs( IOpaqueAddress*  address ,
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
                                         DataObject*      object  )
{

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
      int globalKey( 0 ), totalSize( 0 );
      ++m_nEvts; // Count events
      for ( int iii=0; iii<m_RichG4HitCollectionName->RichHCSize(); ++iii ) {

        std::string colName = m_RichG4HitCollectionName->RichHCName(iii);

        G4SDManager * fSDM = G4SDManager::GetSDMpointer();
        if ( !fSDM ) return Error( "GiGaMCRichHitCnv : NULL G4SDManager pointer !!" );
        int collectionID = fSDM->GetCollectionID(colName);
        if ( -1 == collectionID ) 
        {
          return Warning( "RICH CollectionID = -1", StatusCode::SUCCESS );
        }
        RichG4HitsCollection* myCollection =
          dynamic_cast<RichG4HitsCollection*>(hitscollections->GetHC(collectionID));
        if ( 0 == myCollection ) 
        {
          return Warning( "Null RICH hit collection", StatusCode::SUCCESS );
        }

        const int numberofhits = myCollection->entries();

        // reserve space
        hits->reserve( numberofhits );

        // Reference table between G4 tracks/trajectories and MC particles
        const GiGaKineRefTable & table = hitsSvc()->table() ;

        //convert hits
        totalSize += numberofhits;
        for ( int ihit = 0; ihit < numberofhits; ++ihit ) {

          // Pointer to G4 hit
          RichG4Hit * g4hit = (*myCollection)[ihit];

          // Make new persistent hit object
          MCRichHit * mchit = new MCRichHit();

          // add to container
          hits->insert( mchit, globalKey );

          // hit position
          mchit->setEntry( g4hit->GetGlobalPos() );

          // energy deposited
          mchit->setEnergy( g4hit->GetEdep() );

          // time of flight
          mchit->setTimeOfFlight( g4hit->RichHitGlobalTime() );

          // Rich detector information
          if ( g4hit->GetCurRichDetNum() < 0 ) {
            mchit->setRichInfoValid( false );
            Warning( "Found RichG4Hit with invalid RICH flag" );
          } else {
            mchit->setRichInfoValid( true );
            mchit->setRich(static_cast<Rich::DetectorType>(g4hit->GetCurRichDetNum()));
          }

          // Photon detector number
          //mchit->setPhotoDetector(g4hit->GetCurHpdNum());

          // Radiator information
          if ( g4hit->GetRadiatorNumber() < 0 ) {
            mchit->setRadiatorInfoValid( false );
            Warning( "Found RichG4Hit with default radiator flag. Possibly direct charged track hit" );
          } else {
            mchit->setRadiatorInfoValid( true );
            mchit->setRadiator(static_cast<Rich::RadiatorType>(g4hit->GetRadiatorNumber()));
          }

          // charged track hitting HPD flag
          mchit->setChargedTrack( g4hit->GetChTrackID() < 0 );

          // Rayleigh scattered flag
          mchit->setScatteredPhoton( g4hit->OptPhotRayleighFlag() > 0 );

          // Overall background flag
          mchit->setBackgroundHit( mchit->chargedTrack() ||
                                   mchit->scatteredPhoton() ||
                                   !mchit->richInfoValid() );

          /*
            if ( mchit->chargedTrack() || mchit->scatteredPhoton() ||
            !mchit->richInfoValid() || !mchit->radiatorInfoValid() ) {
            msg << MSG::DEBUG << "G4Hit " << globalKey << " :"
            << " RichDetNum=" << g4hit->GetCurRichDetNum()
            << " RadiatorNum=" << g4hit->GetRadiatorNumber()
            << " TrackID=" << g4hit->GetTrackID()
            << " ChTrackID=" << g4hit->GetChTrackID()
            << " OptPhotID=" << g4hit->GetOptPhotID()
            << " PETrackID=" << g4hit->GetPETrackID()
            << " ChTrackPDG=" << g4hit->GetChTrackPDG()
            << " PETrackPDG=" << g4hit->GetPETrackPDG()
            << " OptPhotRayleighFlag=" << g4hit->OptPhotRayleighFlag()
            << endreq;
            }
          */

          // get MCParticle information
          const MCParticle * mcPart = table[g4hit->GetTrackID()].particle();
          if ( mcPart ) {
            mchit->setMCParticle( mcPart );
          } else {
            Warning( "Found RichG4Hit with no MCParticle parent !" );
          }

          // Count hits
          if ( !mchit->richInfoValid() )
          {
            ++m_invalidRichHits;
          }
          else
          {
            ++m_hitTally[mchit->rich()];
          }
          if ( !mchit->radiatorInfoValid() )
          {
            if ( mchit->richInfoValid() ) ++m_invalidRadHits[mchit->rich()];
          }
          else
          {
            ++m_radHits[mchit->radiator()];
            if ( mchit->chargedTrack()    ) ++m_ctkHits[mchit->radiator()];
            if ( mchit->scatteredPhoton() ) ++m_scatHits[mchit->radiator()];
            if ( mchit->backgroundHit()   ) ++m_bkgHits[mchit->radiator()];
            if ( !mcPart                  ) ++m_nomcpHits[mchit->radiator()];
          }

          // finally increment key
          ++globalKey;

        } // loop over hits

      } // loop over collections

      // Double check all G4 hits where convertered
      if ( totalSize != hits->size() )
      {
        return Warning( "Problem converting RichG4Hits" );
      }

    } else {
      msg << MSG::INFO << "No RichG4Hits to converted since no  Collections available"
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
