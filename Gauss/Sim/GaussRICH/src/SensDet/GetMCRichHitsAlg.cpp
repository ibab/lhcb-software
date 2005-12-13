// $Id: GetMCRichHitsAlg.cpp,v 1.1 2005-12-13 17:31:17 seaso Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
// from GiGa
#include "GiGa/IGiGaSvc.h"
#include "GiGa/GiGaHitsByName.h"
 
// from GiGaCnv
#include "GiGaCnv/IGiGaKineCnvSvc.h"
#include "GiGaCnv/IGiGaCnvSvcLocation.h"
#include "GiGaCnv/GiGaKineRefTable.h"
 
// from Geant4
#include "G4VHitsCollection.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh" 
// from LHCb
#include "Event/MCRichHit.h"
#include "Event/MCParticle.h"
// RichKernel
#include "RichKernel/RichStatDivFunctor.h"
 
// local
#include "GetMCRichHitsAlg.h"
#include "RichG4HitCollName.h"
#include "RichG4Hit.h"
 

//-----------------------------------------------------------------------------
// Implementation file for class : GetMCRichHitsAlg
//
// 2005-12-06 : Sajan EASO
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<GetMCRichHitsAlg>          s_factory ;
const        IAlgFactory& GetMCRichHitsAlgFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GetMCRichHitsAlg::GetMCRichHitsAlg( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_gigaSvc      ( 0 )
  , m_gigaKineCnvSvc ( 0 )
  , m_colRange(std::vector<int> (2))
  ,  m_nEvts                   ( 0 )
  ,  m_hitTally                ( Rich::NRiches, 0 )
  ,  m_radHits                 ( Rich::NRadiatorTypes, 0 )
  ,  m_invalidRadHits          ( Rich::NRiches, 0 )
  ,  m_invalidRichHits         ( 0 )
  ,  m_ctkHits                 ( Rich::NRadiatorTypes, 0 )
  ,  m_scatHits                ( Rich::NRadiatorTypes, 0 )
  ,  m_bkgHits                 ( Rich::NRadiatorTypes, 0 )
  ,  m_nomcpHits               ( Rich::NRadiatorTypes, 0 )
{
  declareProperty( "GiGaService",    m_gigaSvcName    = "GiGa" );
  declareProperty( "KineCnvService", m_kineSvcName    = IGiGaCnvSvcLocation::Kine );
  declareProperty( "MCRichHitsLocation", m_richHitsLocation   = MCRichHitLocation::Default );
  //  declareProperty( "MCRichHitsLocation", m_richHitsLocation   = "/Event/MC/Rich/Hits" );
  m_RichG4HitCollectionName = new RichG4HitCollName();

}
//=============================================================================
// Destructor
//=============================================================================
GetMCRichHitsAlg::~GetMCRichHitsAlg() {
  delete m_RichG4HitCollectionName;
}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode GetMCRichHitsAlg::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize GetMCRichHitsAlg " << endmsg;
  m_gigaSvc = svc<IGiGaSvc>( m_gigaSvcName ); // GiGa has to already exist!
  // get kineCnv service that hold the MCParticle/Geant4 table list
  m_gigaKineCnvSvc = svc<IGiGaKineCnvSvc>( m_kineSvcName );
  m_colRange = getRichG4CollectionRange();
  
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode GetMCRichHitsAlg::execute() {

  debug() << "==> Execute GetMCRichHitsAlg " << endmsg;
  if( 0 == gigaSvc() ) {
    return Error( " execute GetMCRichHitsAlg: IGiGaSvc* points to NULL" );
  }

   
 // Create the MCHits and put them in the TES
 // Cannot use
 // MCHits* hits = getOrCreate<MCHits,MCHits>( m_hitsLocation );
 // because triggers conversion

  MCRichHits* hits = new MCRichHits();
  StatusCode sc = put( hits, m_richHitsLocation );
  if( sc.isFailure() ) {
    return Error( " Unable to register MCRichHits in " + m_richHitsLocation );
  }
      
  // Get the G4 necessary hit collections from GiGa
  G4HCofThisEvent* hitscollections = 0 ;
  try {
 
    // get hitscollections from GiGa
    *gigaSvc() >> hitscollections;
 
   if ( 0 != hitscollections ) {

      // note this key is need for consistency with MCRichOpticalPhoton converter
      int globalKey( 0 ), totalSize( 0 );
      ++m_nEvts; // Count events
      // now check the existence of MC particles and get their table.
       if( !( exist<MCParticles>( MCParticleLocation::Default ) ) ) {
         return Error( " GetMCRichHitsAlg :  MCParticles do not exist at'"
                  + MCParticleLocation::Default +"'" );
       }
      // Get the Geant4->MCParticle table
      GiGaKineRefTable& table = kineSvc()->table();
   
      // now loop through the collections.
      for ( int iii= (m_colRange [0])  ; iii< ( (m_colRange [1])  +1)  ; ++iii ) {
        std::string colName = m_RichG4HitCollectionName->RichHCName(iii);
        G4SDManager * fSDM = G4SDManager::GetSDMpointer();
        if ( !fSDM ) return Error( " GetMCRichHitsAlg : NULL G4SDManager pointer !!" );
        int collectionID = fSDM->GetCollectionID(colName);
        if ( -1 == collectionID ) { 
            info()<<"For Collection "<<colName<<"  No collection id "<<endmsg;
            return Warning( "RICH CollectionID = -1", StatusCode::SUCCESS );
	}
        RichG4HitsCollection* myCollection =
          dynamic_cast<RichG4HitsCollection*>(hitscollections->GetHC(collectionID));
        if ( 0 == myCollection ) {
           info()<<"For Collection "<<colName<<"  No RICH hit collection  "<<endmsg;          
           return Warning( "Null RICH hit collection", StatusCode::SUCCESS );
        }
        const int numberofhits = myCollection->entries();  // num of hits in this collection
        // reserve space
        hits->reserve( numberofhits );
        totalSize += numberofhits;  // count the total num of hits in all collections.
        // now loop through the hits in the current collection.
        for ( int ihit = 0; ihit < numberofhits; ++ihit ) {
         
          // Pointer to G4 hit
          RichG4Hit * g4hit = (*myCollection)[ihit];
          // Make new persistent hit object
          MCRichHit * mchit = new MCRichHit();
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
          // HPD detector number
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
            debug() << " debug history of a richG4Hit " << " :"
            << " RichDetNum=" << g4hit->GetCurRichDetNum()
            << " RadiatorNum=" << g4hit->GetRadiatorNumber()
            << " TrackID=" << g4hit->GetTrackID()
            << " ChTrackID=" << g4hit->GetChTrackID()
            << " OptPhotID=" << g4hit->GetOptPhotID()
            << " PETrackID=" << g4hit->GetPETrackID()
            << " ChTrackPDG=" << g4hit->GetChTrackPDG()
            << " PETrackPDG=" << g4hit->GetPETrackPDG()
            << " OptPhotRayleighFlag=" << g4hit->OptPhotRayleighFlag()
            << endmsg;
            }
          */
         // fill reference to MCParticle
         int trackID = g4hit->GetTrackID();
         const MCParticle * mcPart = table[trackID].particle();
         if( mcPart ) {
          mchit->setMCParticle( mcPart );
         } else {
          warning() << "No pointer to MCParticle for MCRichHit associated to G4 trackID: "
		   << trackID << "   TrackMomentum = "<< g4hit->ChTrackTotMom() 
                   << "  hitEnergy=  "<<g4hit-> GetEdep()  <<endmsg;

         }
 
         // insert it in container
         hits->insert( mchit, globalKey );

       	 // now to bump the various hit counters

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

          // finally increment key for the container
          ++globalKey;

        } // end loop on hits in the collection
        
      } // end loop on collections
      // Verify that all hits are stored for output.
      if( (int) hits->size() != totalSize ) {
       return Error("MCRichHits and RichG4HitCollection have different sizes!");
      }      
   
   } else {
      info() << "No RichG4Hits to be converted since no Collections available"
          << endmsg;
      return StatusCode::SUCCESS;
   }
  } catch( const GaudiException& Excp ) {
    hits->clear();
    return Exception( "GetMCRichHitsAlg: ", Excp ) ;
  }

  return StatusCode::SUCCESS;
};
std::vector<int> GetMCRichHitsAlg::getRichG4CollectionRange() {

      std::vector<int> colRange(2);
  
      // note this key is need for consistency with MCRichOpticalPhoton converter
      int collection_size= m_RichG4HitCollectionName-> RichHCSize();
      int collection_init= m_RichG4HitCollectionName->InitCollectListNumForOutput();
      int collection_final=m_RichG4HitCollectionName->FinalCollectListNumForOutput();
      if( (collection_init < 0) || (collection_init > collection_final) 
         || (collection_init >= collection_size) ) {
        info() <<"Inappropriate setting of Rich Collection list init "<<collection_init<< endmsg;        
        collection_init=0;
      }     
      if( (collection_final < collection_init) || (collection_final < 0) 
          || (collection_final >=  collection_size) ) {
        info() <<"Inappropriate setting of Rich Collection list final "<<collection_init<< endmsg;
        collection_final= collection_size;
      }
      if( (collection_init !=0)  || (collection_final !=  ( collection_size-1 ))) {
	debug()<<"Only partial storage of Rich Hits for Output:  init  final" <<collection_init<<"  "
               <<collection_final<<endmsg;
      }
      colRange[0]=collection_init;
      colRange[1]=collection_final;  

  return  colRange; 
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode GetMCRichHitsAlg::finalize() {

  debug() << "==> GetMCRichHitsAlg Finalize" << endmsg;

  const RichStatDivFunctor occ;

  info() <<"From GetMCRichHitsAlg :   "
         << "Av. # Invalid RICH flags           = " << occ(m_invalidRichHits,m_nEvts)
	 <<endmsg;
 info() << "Av. # Invalid rad. flags   : Rich1 = " << occ(m_invalidRadHits[Rich::Rich1],m_nEvts)
        << " Rich2 = " << occ(m_invalidRadHits[Rich::Rich2],m_nEvts)
        << endmsg;
  info() << "Av. # MCRichHits           : Rich1 = "
      << occ(m_hitTally[Rich::Rich1],m_nEvts)
      << " Rich2 = " << occ(m_hitTally[Rich::Rich2],m_nEvts)
      << endmsg;
  info()<< "Av. # MCRichHits           : Aero  = " << occ(m_radHits[Rich::Aerogel],m_nEvts)
      << " C4F10 = " <<  occ(m_radHits[Rich::C4F10],m_nEvts)
      << " CF4 = "   <<  occ(m_radHits[Rich::CF4],m_nEvts)
      << endmsg;
  info()<< "Av. # Charged Track hits   : Aero  = " << occ(m_ctkHits[Rich::Aerogel],m_nEvts)
      << " C4F10 = " <<  occ(m_ctkHits[Rich::C4F10],m_nEvts)
      << " CF4 = "   <<  occ(m_ctkHits[Rich::CF4],m_nEvts)
      << endmsg;
  info()<< "Av. # Scattered hits       : Aero  = " << occ(m_scatHits[Rich::Aerogel],m_nEvts)
      << " C4F10 = " <<  occ(m_scatHits[Rich::C4F10],m_nEvts)
      << " CF4 = "   <<  occ(m_scatHits[Rich::CF4],m_nEvts)
      << endmsg;
  info()<< "Av. # background hits      : Aero  = " << occ(m_bkgHits[Rich::Aerogel],m_nEvts)
      << " C4F10 = " <<  occ(m_bkgHits[Rich::C4F10],m_nEvts)
      << " CF4 = "   <<  occ(m_bkgHits[Rich::CF4],m_nEvts)
       << endmsg;
  info() << "Av. # MCParticle-less hits : Aero  = " << occ(m_nomcpHits[Rich::Aerogel],m_nEvts)
      << " C4F10 = " <<  occ(m_nomcpHits[Rich::C4F10],m_nEvts)
      << " CF4 = "   <<  occ(m_nomcpHits[Rich::CF4],m_nEvts)
      << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
