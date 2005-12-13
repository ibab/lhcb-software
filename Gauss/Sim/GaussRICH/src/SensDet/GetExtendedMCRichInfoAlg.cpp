// $Id: GetExtendedMCRichInfoAlg.cpp,v 1.1 2005-12-13 17:31:17 seaso Exp $
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
#include "G4TrajectoryContainer.hh"
// from LHCb
#include "Event/MCRichHit.h"
// LHCbEvent
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
// RichKernel
#include "RichKernel/RichStatDivFunctor.h"
// Rich Event
#include "Event/MCRichOpticalPhoton.h"
#include "Event/MCRichSegment.h"
#include "Event/MCRichTrack.h"
 
 
// local
#include "GetExtendedMCRichInfoAlg.h"
#include "RichG4HitCollName.h"
#include "RichG4Hit.h"
 
//-----------------------------------------------------------------------------
// Implementation file for class : GetExtendedMCRichInfoAlg
//
// 2005-12-12 : Sajan EASO
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<GetExtendedMCRichInfoAlg>          s_factory ;
const        IAlgFactory& GetExtendedMCRichInfoAlgFactory = s_factory ; 

// Containers and sorting functions
namespace GiGaRich {

  typedef std::pair<HepVector3D,HepPoint3D> MomentumAtZ;
  struct SortMomentumAtZ
  {
    bool operator() ( const MomentumAtZ & p1, const MomentumAtZ & p2 ) const
    {
      return ( p1.second.z() < p2.second.z() );
    }
  };
  typedef std::set<MomentumAtZ,GiGaRich::SortMomentumAtZ> MomentaAtZ;

}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GetExtendedMCRichInfoAlg::GetExtendedMCRichInfoAlg( const std::string& name,
                                                    ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_gigaSvc      ( 0 )
  , m_gigaKineCnvSvc ( 0 )
  , m_colRange(std::vector<int> (2))
  ,  m_nEvts                   ( 0 )
  ,  m_hitTallyPhot                ( 2, 0 )
  ,  m_hitTallySeg                ( 2, 0 )
  ,  m_hitTallyTrk                ( 0 )
{
  declareProperty( "GiGaService",    m_gigaSvcName    = "GiGa" );
  declareProperty( "KineCnvService", m_kineSvcName    = IGiGaCnvSvcLocation::Kine );
  declareProperty( "convertMCOpticalPhotonInfo", m_convertMCOpticalPhotonInfo =false);
  declareProperty( "convertMCRichTrackSegmentInfo" ,m_convertMCRichTrackSegmentInfo=false);
  declareProperty( "convertMCRichTrackInfo", m_convertMCRichTrackInfo=false);
  declareProperty( "convertMCRichExtendedInfo", m_convertMCRichExtendedInfo=false);
  declareProperty( "MCRichOpticalPhotonsLocation", 
                    m_richOpticalPhotonsLocation = MCRichOpticalPhotonLocation::Default);
  declareProperty( "MCRichSegmentsLocation", 
                    m_richSegmentsLocation=MCRichSegmentLocation::Default);
  declareProperty( "MCRichTracksLocation",
		   m_richTracksLocation=MCRichTrackLocation::Default);
  declareProperty( "MCRichHitsLocation", m_richHitsLocation   = MCRichHitLocation::Default );
   m_RichG4HitCollectionName = new RichG4HitCollName();

}
//=============================================================================
// Destructor
//=============================================================================
GetExtendedMCRichInfoAlg::~GetExtendedMCRichInfoAlg() {
  delete m_RichG4HitCollectionName;

}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode GetExtendedMCRichInfoAlg::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> GetExtendedMCRichInfoAlg Initialize" << endmsg;
  m_gigaSvc = svc<IGiGaSvc>( m_gigaSvcName ); // GiGa has to already exist!
  // get kineCnv service that hold the MCParticle/Geant4 table list
  m_gigaKineCnvSvc = svc<IGiGaKineCnvSvc>( m_kineSvcName );
  if( m_convertMCRichExtendedInfo) {
    m_convertMCOpticalPhotonInfo=true;
    m_convertMCRichTrackSegmentInfo=true;
    m_convertMCRichTrackInfo=true;
  }

   m_colRange = getRichG4CollectionRange();

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode GetExtendedMCRichInfoAlg::execute() {

  debug() << "==>  GetExtendedMCRichInfoAlg Execute" << endmsg;
  if( 0 == gigaSvc() ) {
    return Error( " execute GetExtendedMCRichInfoAlg: IGiGaSvc* points to NULL" );
  }

  if( !( m_convertMCRichExtendedInfo)  ) {
    debug() <<" No Extended Rich Info stored in the output "<< endmsg;
    
   return  StatusCode::SUCCESS;
    
  }
  
 // Create the MCOpticalPhotons, segments and richtracks and put them in the TES
  MCRichOpticalPhotons* photons  = new MCRichOpticalPhotons();
  MCRichSegments * segments = new  MCRichSegments ();
  MCRichTracks * richTracks = new  MCRichTracks();
  
  StatusCode sc1 = put( photons,  m_richOpticalPhotonsLocation );
  StatusCode sc2 = put( segments, m_richSegmentsLocation);
  StatusCode sc3 = put( richTracks, m_richTracksLocation);
  

  if( sc1.isFailure() ) {
    return Error( " Unable to register MCRichOpticalPhotons in " +  m_richOpticalPhotonsLocation );
  }
  if( sc2.isFailure() ) {
    return Error( " Unable to register  in MCRichSegments " + m_richSegmentsLocation  );
  }
  if( sc3.isFailure() ) {
    return Error( " Unable to register  in MCRichTracks  " + m_richTracksLocation );
  }

  
  // Get the G4 necessary hit collections from GiGa
  G4HCofThisEvent* hitscollections = 0 ;
  try {
   // get hitscollections from GiGa
    *gigaSvc() >> hitscollections;
  
   if ( 0 != hitscollections ) {
      // Get MCRichHits eventSvc()
      SmartDataPtr<MCRichHits> mcHits( evtSvc(),  m_richHitsLocation );
      if ( !mcHits ) {
        return Warning( "Could not locate MCRichHits from GiGaMCRichOpticalPhotonCnv" );
      }

     // note this key is need for consistency with MCRichHit converter
      int globalKey = 0;
      int totalSize=0;
      
      ++m_nEvts; // Count events
      // now check the existence of MC particles and get their table.
       if( !( exist<MCParticles>( MCParticleLocation::Default ) ) ) {
         return Error( "GetExtendedMCRichInfoAlg:  MCParticles do not exist at'"
                  + MCParticleLocation::Default +"'" );
       }
      // Get the Geant4->MCParticle table
      GiGaKineRefTable& table = kineSvc()->table();
       // map of vectors to store Hits associated to each MCParticle
      //      typedef std::pair< const MCParticle*, Rich::RadiatorType> HitDataListKey;
      //  typedef std::pair< const RichG4Hit*, int > HitDataType;
      // typedef std::vector< HitDataType > G4HitList;
      // typedef std::map< HitDataListKey, G4HitList > HitDataList;
      HitDataList sortedHits;
   

      // now loop through the collections.
      for ( int iii= (m_colRange [0])  ; iii< ( (m_colRange [1])  +1)  ; ++iii ) {
        std::string colName = m_RichG4HitCollectionName->RichHCName(iii);
        G4SDManager * fSDM = G4SDManager::GetSDMpointer();
        if ( !fSDM ) return Error( "GiGaMCRichHitCnv : NULL G4SDManager pointer !!" );
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
         
         totalSize += numberofhits;  // count the total num of hits in all collections.
         photons->reserve( numberofhits );
        // now loop through the hits in the current collection.
        for ( int ihit = 0; ihit < numberofhits; ++ihit ) {
         // Pointer to G4 hit
          RichG4Hit * g4hit = (*myCollection)[ihit];
          // Now for the extented photon objects
          MCRichOpticalPhoton * mcPhoton = StoreExtendedPhotonInfo( g4hit);
          mcPhoton->setMcRichHit( mcHits->object(globalKey) ); // SmartRef to associated MCRichHit         
          photons->insert( mcPhoton, globalKey );  // insert in container       

          // Now for the Richtrack segments
          if ( g4hit->GetRadiatorNumber() >= 0 ) {
            Rich::RadiatorType rad =
              static_cast<Rich::RadiatorType>(g4hit->GetRadiatorNumber());
            // get MCParticle information
            const int traid = g4hit->GetTrackID();
            const MCParticle * mcPart = table[traid].particle();
            if ( !mcPart ) {
                warning() << "GetExtendedMCRichInfoAlg: No pointer to MCParticle for " 
                          <<"   MCRichHit associated to G4 trackID: "
                          << traid << "   TrackMomentum = "<< g4hit->ChTrackTotMom()
                          << "  hitEnergy=  "<<g4hit-> GetEdep()  <<endmsg;  
            }
            
            
            // add to sorted list
            HitDataListKey key(mcPart,rad);
            HitDataType data(g4hit,globalKey);
            sortedHits[key].push_back( data );

          } else {
            debug() << "RichG4Hit " << ihit << " has no radiator information. Possibly a charged track hit"
                << endmsg;
          }




         // finally, increment the key
          ++globalKey;          
         
        } // end loop hits
        
      } // end loop collections
      
      // test on the photons
      // Should have one opticalphoton for each and every RichHit to be stored.
      if ( ( int ) photons->size() !=  totalSize ) {
       return Error("MCRichOpticalPhotons and RichG4HitCollection have different sizes!");       
      }
      // Now for the rich segments
      
    	StatusCode sc = StoreExtendedSegmentInfo( sortedHits,segments,photons  );
                  
      if( sc.isFailure() ) {
        return Error( " Unable to store  RichExtended Segment info " );
       }          


  // Now for the richtracks
  // retrieve the  trajectory container from GiGa Service
  G4TrajectoryContainer* trajectories = 0 ;

  try {


    // get trajectories from GiGa
    *gigaSvc() >> trajectories;
    if ( 0 == trajectories ) { return Error("No G4TrajectoryContainer* object is found !"); }
    StatusCode sc = StoreExtendedRichTrackInfo(trajectories, segments, richTracks, table);
    if( sc.isFailure() ) {
        return Error( " Unable to store  RichExtended richtrack info " );
    }          
   
    
  }catch ( const GaudiException& Excp )  {
    richTracks->clear();
    return Exception( "RichExtendedInfo: RichTracks ", Excp ) ;
  }  catch( const std::exception& Excp ){
    richTracks->clear();
    return Exception( "RichExtendedInfo: RichTracks  ", Excp ) ;
  }
   
  } else {
      info() << "No RichG4Hits to be converted into ExtendedRichInfo since no Collections available"
          << endmsg;
      return StatusCode::SUCCESS;
  }

   
 } catch( const GaudiException& Excp ) {
    photons->clear();
    return Exception( "GetExtendedMCRichInfAlg: ", Excp ) ;
 } catch( const std::exception& Excp ) {
    photons->clear();
    return Exception( "GetExtendedMCRichInfAlg: ", Excp ) ;
 }
  

  return StatusCode::SUCCESS;
};

StatusCode GetExtendedMCRichInfoAlg::StoreExtendedRichTrackInfo(   G4TrajectoryContainer* trajectories ,
                                                                   MCRichSegments * segments,
                                                                   MCRichTracks * richTracks,
                                                                        GiGaKineRefTable& table )
{
  try {
    
    // loop over trajectories and form links from MCP to trajectories
    typedef std::map<const MCParticle*, const GiGaTrajectory*> MCPartToGiGaTraj;
    MCPartToGiGaTraj mcpToTraj;
    TrajectoryVector * tv = trajectories->GetVector();
    for ( TrajectoryVector::const_iterator iTr = tv->begin();
          tv->end() != iTr; ++iTr ) {
      // get the GiGa trajectory
      const GiGaTrajectory * traj = gigaTrajectory(*iTr);
      if ( !traj ) continue;
      mcpToTraj[table[traj->trackID()].particle()] = traj;
    }
    // map of vectors to store segments associated to each MCParticle
    typedef const MCParticle* HitListKey;
    typedef std::vector<MCRichSegment*> SegmentList;
    typedef std::map< HitListKey, SegmentList > SortedSegments;
    SortedSegments sortedSegs;

    // Iterate over segments and sort according to MCParticle
    for ( MCRichSegments::iterator iSeg = segments->begin();
          iSeg != segments->end(); ++iSeg ) {
      if ( !(*iSeg) ) {
        warning() << "Null RichRecSegment pointer" << endmsg;
        continue;
      }
      if ( !(*iSeg)->mcParticle() ) {
        warning() << "RichRecSegment has null MCParticle pointer" << endmsg;
        continue;
      }
      sortedSegs[(*iSeg)->mcParticle()].push_back( *iSeg );
    }
    // Loop over sorted segments
    for ( SortedSegments::iterator iList = sortedSegs.begin();
          iList != sortedSegs.end(); ++iList ) {
      const MCParticle * mcPart = (*iList).first;
      if ( !mcPart ) continue;
      // new MCRichTrack
      MCRichTrack * mcTrack = new MCRichTrack();
      // data
      mcTrack->setMcParticle( mcPart );
      debug()<< "Creating MCRichTrack for MCParticle " << mcPart->key()
          << endmsg;

      // Loop over segments for this track
      for ( SegmentList::iterator iSeg = (*iList).second.begin();
            iSeg != (*iList).second.end(); ++iSeg ) {
        if ( !(*iSeg) ) continue;
        mcTrack->addToMcSegments( *iSeg );
        (*iSeg)->setMCRichTrack( mcTrack );
          debug() << " Adding " << (*iSeg)->radiator()
            << " MCRichSegment " << (*iSeg)->key() << endmsg;
      }
      // Locate GiGaTrajectory for this MCParticle
      const GiGaTrajectory * traj = mcpToTraj[mcPart];
      if ( !traj ) {
        warning()<< "Failed to find trajectory for MCParticle " << mcPart->key()
            << endmsg;
        continue;
      }
      // Loop over points
      debug()<< " Total number of trajectory points = " << traj->GetPointEntries()
          << endmsg;
      for ( int iPoint = 0; iPoint < traj->GetPointEntries(); ++iPoint ) {

        const G4VTrajectoryPoint * G4point = traj->point(iPoint);
        const HepVector3D & point = G4point->GetPosition();
        debug() << "  TrajPoint " << iPoint << " "
            << point << endmsg;

      }
      
      richTracks->insert( mcTrack, mcPart->key() );
    }
    

    // count tracks
    m_hitTallyTrk += richTracks->size();

  }catch ( const GaudiException& Excp )  {
    richTracks->clear();
    return Exception( "RichExtendedInfoStore: RichTracks ", Excp ) ;
  }  catch( const std::exception& Excp ){
    richTracks->clear();
    return Exception( "RichExtendedInfoStore: RichTracks  ", Excp ) ;
  }
  
  return StatusCode::SUCCESS;
  
};

StatusCode GetExtendedMCRichInfoAlg::StoreExtendedSegmentInfo( HitDataList sortedHits,  
          MCRichSegments * segments,MCRichOpticalPhotons* photons)
{

 try{

      for ( HitDataList::const_iterator iList = sortedHits.begin();
            iList != sortedHits.end(); ++iList ) {
        const MCParticle * mcPart    = ((*iList).first).first;
        const Rich::RadiatorType rad = ((*iList).first).second;
        const G4HitList & g4hitList  = (*iList).second;

        MCRichSegment * mcSeg = new MCRichSegment();

        mcSeg->setRadiator( rad );
        mcSeg->setRich( Rich::CF4 == rad ? Rich::Rich2 : Rich::Rich1 );
        mcSeg->setMcParticle( mcPart );
        debug() << "Creating MCRichSegment " << mcSeg->key()
               << " for MCParticle " << mcPart->key()
               << " in " << rad << endmsg;
        // Count segments
        if      ( Rich::Rich1 == mcSeg->rich() ) ++m_hitTallySeg[Rich::Rich1];
        else if ( Rich::Rich2 == mcSeg->rich() ) ++m_hitTallySeg[Rich::Rich2];
        // loop over hits
        GiGaRich::MomentaAtZ momenta;
        for ( G4HitList::const_iterator iHit = g4hitList.begin();
              iHit != g4hitList.end(); ++iHit ) {
              debug()<< " Hit : Prod-pnt  " << (HepPoint3D)(*iHit).first->GetPhotEmisPt() << endreq
              << "     : Pre-step  " << (HepPoint3D)(*iHit).first->ChTrackCkvPreStepPos() << endreq
              << "     : post-step " << (HepPoint3D)(*iHit).first->ChTrackCkvPostStepPos() << endreq
              << "     : tk Mon.   " << (HepVector3D)(*iHit).first->ChTrackMomVect() << endmsg;
          // Add momentum to list of momenta
          momenta.insert( GiGaRich::MomentumAtZ( (*iHit).first->ChTrackMomVect(),
                                                 (*iHit).first->ChTrackCkvPreStepPos() )  );
          momenta.insert( GiGaRich::MomentumAtZ( (*iHit).first->ChTrackMomVect(),
                                                 (*iHit).first->ChTrackCkvPostStepPos() ) );

          // Add associated MCRichOpticalPhoton to this segment
          mcSeg->addToMCRichOpticalPhotons( photons->object((*iHit).second) );
          
        }
        
        // Finally, loop over sorted trajectory points and add to segment in order
          debug()  << " Found " << momenta.size() << " trajectory points" << endmsg;

        for ( GiGaRich::MomentaAtZ::const_iterator iMom = momenta.begin();
              iMom != momenta.end(); ++iMom ) {
          mcSeg->trajectoryMomenta().push_back( (*iMom).first );
          mcSeg->trajectoryPoints().push_back( (*iMom).second );
              debug()<< "  Added trajectory momentum " << (*iMom).first 
                     << " at " << (*iMom).second << endmsg;
        }
        

        segments->insert( mcSeg );
        
      }
      
} catch (const GaudiException& Excp ) {
    segments->clear();
    return Exception( "GetExtendedMCRichInfoAlg : For RichSegments ", Excp ) ;

}
 return StatusCode::SUCCESS;

}
MCRichOpticalPhoton *  GetExtendedMCRichInfoAlg::StoreExtendedPhotonInfo( RichG4Hit * g4hit) 
{

           // New optical photon object
          MCRichOpticalPhoton * mcPhoton = new MCRichOpticalPhoton();
  
         // Incidence point on HPD photocathode.
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
            //  mcPhoton->setRichInfoValid( false );
          } else {
            //  mcPhoton->setRichInfoValid( true );
            mcPhoton->setRich(static_cast<Rich::DetectorType>(g4hit->GetCurRichDetNum()));
          }
          // Photon detector number
          //mcPhoton->setPhotoDetector(g4hit->GetCurHpdNum());

          // Radiator information
          if ( g4hit->GetRadiatorNumber() < 0 ) {
            //  mcPhoton->setRadiatorInfoValid( false );
          } else {
            //  mcPhoton->setRadiatorInfoValid( true );
            mcPhoton->setRadiator(static_cast<Rich::RadiatorType>(g4hit->GetRadiatorNumber()));
          }
          // charged track hitting HPD flag
          mcPhoton->setChargedTrack( g4hit->GetChTrackID() < 0 );

          // Rayleigh scattered flag
          mcPhoton->setScatteredPhoton( g4hit->OptPhotRayleighFlag() > 0 );

          // Overall background flag
          mcPhoton->setBackgroundHit( mcPhoton->chargedTrack() ||
                                      mcPhoton->scatteredPhoton());
                                      // || !mcPhoton->richInfoValid() );

         // Count photons
          if      ( Rich::Rich1 == mcPhoton->rich() ) ++m_hitTallyPhot[Rich::Rich1];
          else if ( Rich::Rich2 == mcPhoton->rich() ) ++m_hitTallyPhot[Rich::Rich2];
          
          return  mcPhoton;          
}

std::vector<int> GetExtendedMCRichInfoAlg::getRichG4CollectionRange() {

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
StatusCode GetExtendedMCRichInfoAlg::finalize() {

  debug() << "==> GetExtendedMCRichInfoAlg  Finalize" << endmsg;
  const RichStatDivFunctor occ;
 

  info() << "Av. # MCRichOpticalPhotons : Rich1 = " 
      << occ(m_hitTallyPhot[Rich::Rich1],m_nEvts) 
      << " Rich2 = " << occ(m_hitTallyPhot[Rich::Rich2],m_nEvts)
      << endmsg;
  info()  << "Av. # MCRichSegments       : Rich1 = " 
      << occ(m_hitTallySeg[Rich::Rich1],m_nEvts) 
      << " Rich2 = " << occ(m_hitTallySeg[Rich::Rich2],m_nEvts)
      << endmsg;
   info()  << "Av. # MCRichTracks         : Overall = " 
      << occ(m_hitTallyTrk,m_nEvts) << endmsg;
        
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
