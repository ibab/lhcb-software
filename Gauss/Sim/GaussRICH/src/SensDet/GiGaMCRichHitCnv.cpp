
/** @file GiGaMCRichHitCnv.cpp
 *
 *  Implementation file for GiGa converter : GiGaMCRichHitCnv
 *
 *  CVS History :
 *  $Id: GiGaMCRichHitCnv.cpp,v 1.12 2005-02-17 13:32:16 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.11  2005/01/19 10:38:52  jonrob
 *  add simple printout to GiGa converters
 *
 *  Revision 1.10  2004/07/30 13:42:13  jonrob
 *  Add doxygen file documentation and CVS information
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
    m_hitTally                ( 2, 0 )
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
  msg << MSG::ALWAYS << "Av. # MCRichHits           : Rich1 = "
      << occ(m_hitTally[Rich::Rich1],m_nEvts)
      << " Rich2 = " << occ(m_hitTally[Rich::Rich2],m_nEvts)
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
          } else {
            mchit->setRichInfoValid( true );
            mchit->setRich(static_cast<Rich::DetectorType>(g4hit->GetCurRichDetNum()));
          }

          // Photon detector number
          //mchit->setPhotoDetector(g4hit->GetCurHpdNum());

          // Radiator information
          if ( g4hit->GetRadiatorNumber() < 0 ) {
            mchit->setRadiatorInfoValid( false );
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
          if      ( Rich::Rich1 == mchit->rich() ) ++m_hitTally[Rich::Rich1];
          else if ( Rich::Rich2 == mchit->rich() ) ++m_hitTally[Rich::Rich2];

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
