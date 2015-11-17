// $Id: GetMCCkvHitsAlg.cpp,v 1.36 2009-07-17 13:46:12 jonrob Exp $
// Include files

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// local
#include "GaussCherenkov/GetMCCkvHitsAlg.h"
#include "GaussRICH/RichG4RadiatorMaterialIdValues.h"

#include "RichDet/DeRichPMT.h"
#include "RichDet/DeRich.h"
#include "RichDet/DeRich1.h"
#include "RichDet/DeRich2.h"
#include "RichDet/DeRichPDPanel.h"
#include "RichDet/DeRichPMTPanel.h"
#include "RichDet/DeRichSystem.h"
#include "GaussCherenkov/CherenkovPmtLensUtil.h"

// namespaces
using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : GetMCCkvHitsAlg
//
// 2011-03-06 : Sajan EASO
// 2007-01-11 : Gloria Corti, adapt to Gaudi v19 (also compatible with v18)
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
// Moved to Factories.cpp
//DECLARE_ALGORITHM_FACTORY( GetMCCkvHitsAlg );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GetMCCkvHitsAlg::GetMCCkvHitsAlg( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : GetMCCkvInfoBase     ( name , pSvcLocator )
  , m_nEvts               ( 0                  )
  , m_invalidRichHits     ( 0                  )
  , m_richDets            ( Rich::NRiches      )
  , m_NumPmtInAModule     (16 )
{
  declareProperty( "MCRichHitsLocation",
                   m_dataToFill = MCRichHitLocation::Default );
  declareProperty( "MCParticles" ,
                   m_mcParticles = MCParticleLocation::Default ) ;
}

//=============================================================================
// Destructor
//=============================================================================
GetMCCkvHitsAlg::~GetMCCkvHitsAlg() { }

//=============================================================================
// Initialization
//=============================================================================
StatusCode GetMCCkvHitsAlg::initialize()
{
  const StatusCode sc = GetMCCkvInfoBase::initialize();
  if ( sc.isFailure() ) return sc;

  // Get RichDet objects
  // if ( richIsActive(Rich::Rich1) )
  //   m_richDets[Rich::Rich1] = getDet<DeRich>( DeRichLocations::Rich1 );
  // if ( richIsActive(Rich::Rich2) )
  //  m_richDets[Rich::Rich2] = getDet<DeRich>( DeRichLocations::Rich2 );

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode GetMCCkvHitsAlg::execute()
{

  // Create the MCHits and put them in the TES
  MCRichHits* hits = new MCRichHits();
  put( hits, dataLocationInTES() );
  hits->reserve( 1500 ); // rough guess at number of hits

  // Get the G4 necessary hit collections from GiGa
  G4HCofThisEvent* hitscollections = 0;

  // get hitscollections from GiGa
  *gigaSvc() >> hitscollections;

  // relations betwen RichG4Hit and MCRichHit
  G4HitTable * relationTable = new G4HitTable();
  put ( relationTable, g4HitToMCRichHitRelLoc() );

  if ( NULL != hitscollections )
  {

    // note this key is need for consistency with MCRichOpticalPhoton converter
    unsigned int  totalSize( 0 );
    ++m_nEvts; // Count events
    // now check the existence of MC particles and get their table.
    if( !( exist<MCParticles>( m_mcParticles ) ) )
    {
      return Error( "GetMCCkvHitsAlg : MCParticles do not exist at '"
                    + m_mcParticles +"'" );
    }
    // Get the Geant4->MCParticle table
    GiGaKineRefTable& table = kineSvc()->table();
    RichG4RadiatorMaterialIdValues* aRMIdValues= 
            RichG4RadiatorMaterialIdValues::RichG4RadiatorMaterialIdValuesInstance();

    // now loop through the collections.
    
    for ( int iii= colRange()[0]; iii < colRange()[1]+1 ; ++iii )
    {
      // collection name
      const std::string & colName = RichG4HitCollectionName()->RichHCName(iii);
      
      G4SDManager * fSDM = G4SDManager::GetSDMpointer();
      if ( !fSDM ) return Error( "NULL G4SDManager pointer !!" );
      const int collectionID = fSDM->GetCollectionID(colName);
      if ( -1 == collectionID )
      {
        return Warning( "RICH Collection "+colName+" : ID = -1", StatusCode::SUCCESS );
      }
      CkvG4HitsCollection* myCollection =
        dynamic_cast<CkvG4HitsCollection*>(hitscollections->GetHC(collectionID));
      if ( 0 == myCollection )
      {
        return Warning( "Null RICH hit collection "+colName, StatusCode::SUCCESS );
      }
      const int numberofhits = myCollection->entries();  // num of hits in this collection
      //info()<<"GetMcHitsAlg Numhits "<<numberofhits<<"  "<<colName<<endreq;
      
      // reserve space
      totalSize += numberofhits;  // count the total num of hits in all collections.

      // GrandPmtFlag
      bool isGrandPmtHitFlag = ( (iii >= RichG4HitCollectionName() ->InitCollectListNumForGrandPmtOutput()) && 
                                (iii <=  RichG4HitCollectionName() -> FinalCollectListNumForGrandPmtOutput())) ? true : false;
      

      // now loop through the hits in the current collection.
      for ( int ihit = 0; ihit < numberofhits; ++ihit )
      {
        // Pointer to G4 hit
        const CkvG4Hit * g4hit = (*myCollection)[ihit];
        if ( !g4hit ) { Error( "Null CkvG4Hit pointer" ); continue; }

        // Rich detector information
        const Rich::DetectorType rich = g4hit->detectorType();
        if ( !richIsActive(rich) ) { continue; }

        // Make new persistent hit object
        MCRichHit * mchit = new MCRichHit();
        // add to container
        hits->push_back( mchit );

        // store relation between G4 hit and this MCRichHit
        relationTable->relate( g4hit, mchit );

        // hit position
        mchit->setEntry( Gaudi::XYZPoint(g4hit->GetGlobalPos()) );

        // energy deposited
        mchit->setEnergy( g4hit->GetEdep() );

        // time of flight
        mchit->setTimeOfFlight( g4hit->RichHitGlobalTime() );

        // Store Rich detector information
        mchit->setRich( rich );
        if ( !mchit->richInfoValid() )
        {
          Warning( "Invalid RICH detector from G4Hit" );
        }

        // radiator information
        const Rich::RadiatorType rad = g4hit->radiatorType();
        const int radID              = g4hit->GetRadiatorNumber();
        if ( g4hit->GetChTrackID() > 0 && rad == Rich::InvalidRadiator )
        {
          std::ostringstream mess;
          mess << "track ID > 0 and unknown radiator ID " << radID
               << " -> MCRichHit history incomplete";
          Warning ( mess.str(), StatusCode::SUCCESS ).ignore();
        }

        // store radiator info
        mchit->setRadiator( rad );

        // for aerogel, save tile number
        if ( Rich::Aerogel == rad )
        {
          const int aeroID =  aRMIdValues->Rich1AgelRadiatorNumToFullTileNum(radID);
          const int aeroSubTileID=  
                    aRMIdValues->Rich1AgelRadiatorNumToSubTileNumInFullTile(radID);          

          if ( aeroID < 2*2*2*2*2 ) // Aerogel tile ID field has 5 bits allocated
          {
            mchit->setAerogelTileID( aeroID );
          }
          else
          {
            std::ostringstream mess;
            mess << "Aerogel ID " << aeroID
                 << " too large to pack into MCRichHit !!";
            Warning ( mess.str(), StatusCode::FAILURE );
          }

          if( (aeroSubTileID >=0)   && (aeroSubTileID < 2*2*2*2*2*2) ) 
                 // Aerogel Sub tile ID has 6 bits allocated
            // commented out temporarily until new MCRichHit is available.
          {
            //  mchit->setAerogelSubTileID ( aeroSubTileID);
          }
          
        }
        

        // charged track hitting HPD flag
        mchit->setChargedTrack( g4hit->GetChTrackID() < 0 );
        
        // Rayleigh scattered flag
        //mchit->setScatteredPhoton( g4hit->OptPhotRayleighFlag() > 0 );
        
        // Back scattered electrons
        mchit->setHpdSiBackscatter( g4hit->ElectronBackScatterFlag() );

        // Photon produced by Scintilation process 
        mchit->setRadScintillation   ( ( g4hit-> PhotonSourceProcessInfo()) == 2 );

        // The hit went through the curvsed surface of a PMT lens, theirby got focussed onto a PMT.
        bool refractAtPdLens=  (g4hit->pdWithLens() > 0 ) ? true : false ;
        mchit-> setPmtLensFlag(refractAtPdLens);
        

        // HPD reflections
        //  mchit->setHpdReflQWPC   ( g4hit->isHpdQwPCRefl()     );
        //  mchit->setHpdReflChr    ( g4hit->isHpdChromiumRefl() );
        //  mchit->setHpdReflAirQW  ( g4hit->isHpdAirQwRefl()    );
        // mchit->setHpdReflAirPC  ( g4hit->isHpdAirPCRefl()    );
        //  mchit->setHpdReflSi     ( g4hit->isHpdSiliconRefl()  );
        //  mchit->setHpdReflKovar  ( g4hit->isHpdKovarRefl()    );
        //  mchit->setHpdReflKapton ( g4hit->isHpdKaptonRefl()   );
        // mchit->setHpdReflPCQW   ( g4hit->isHpdPCQwRefl()     );


        // the following commented out temporarily until all the versions
        // of other related packages are upated. SE 28-3-2011.
        // get sensitive detector identifier from det elem
        // end of temporary commenting out.
        // const RichSmartID detID( m_richDets[rich]->sensitiveVolumeID(mchit->entry()) );
        // test smartid val
        // const LHCb::RichSmartID detID_test( m_richDets[rich]->sensitivePhDetVolumeID(mchit->entry()) );
        // const LHCb::RichSmartID detID_test( m_richDets[rich]->sensitiveVolumeID(mchit->entry()) );
        //  int aM_test = detID_test.pdNumInCol();
        // int aP_test = detID_test.pdCol();
        // int PixX_test =detID_test.pixelCol();
        // int PixY_test =detID_test.pixelRow();

         // verbose()<<" SmartId_test  rich side  aM_test aP_test PixX_test   PixY_test "    << detID_test.rich() <<"   "
         //     <<detID_test.panel()<<"   "<<aM_test <<"   "<<aP_test<<"   "<<PixX_test<<"   "<< PixY_test<<endmsg;
        // end test smartid val
        // info()<<" SmartID test Hit coord "<<mchit->entry()<<endmsg;
         

        // fill reference to MCParticle (need to const cast as method is not const !!)
        CkvG4Hit* nonconstg4hit = const_cast<CkvG4Hit*>(g4hit);  


        const LHCb::RichSmartID detID = assembleMCPmtRichSmartID(g4hit,isGrandPmtHitFlag); 
        
        if ( !detID.isValid() )
         {     
           warning()<<" Invalid richsmartID for Pmt from GaussCherenkov "<<endmsg; 
           if( msgLevel(MSG::VERBOSE) ) {
             nonconstg4hit->Print();
           }
         }else{
          // Fill value into hit
           mchit->setSensDetID( detID );
         }


        
        
        const int trackID = nonconstg4hit->GetTrackID();

        const MCParticle * mcPart = table[trackID].particle();
        if ( mcPart )
        {
          mchit->setMCParticle( mcPart );
        }
        else
        {
          warning() << "No MCParticle for MCRichHit associated to G4 Track ID = "
                    << trackID << " Track Momentum = " << g4hit->ChTrackTotMom()
                    << " Hit Energy = " << g4hit->GetEdep() << endmsg;
        }

        // now increment the various hit counters

        // RICH counters
        if ( !mchit->richInfoValid() )
        {
          ++m_invalidRichHits;
        }
        else
        {
          ++m_hitTally[rich];
          if ( mchit->isSignal()         ) ++m_signalTally[rich];
          if ( mchit->gasQuartzCK()      ) ++m_gasQzHits[rich];
          if ( mchit->hpdQuartzCK()      ) ++m_hpdQzHits[rich];
          if ( mchit->nitrogenCK()       ) ++m_nitroHits[rich];
          if ( mchit->aeroFilterCK()     ) ++m_aeroFilterHits[rich];
          if ( mchit->hpdSiBackscatter() ) ++m_siBackScatt[rich];
          if ( mchit->radScintillation()    ) ++m_scintillationHits[rich];
          if ( mchit->chargedTrack()     ) ++m_ctkHits[rich];
          // if ( mchit->hpdReflection()    )  
          //  {
          //  ++m_hpdReflHits[rich];
            //        if ( mchit->hpdReflQWPC()   ) ++m_hpdReflHitslQWPC[rich];
            // if ( mchit->hpdReflChr()    ) ++m_hpdReflHitslChr[rich];
            // if ( mchit->hpdReflAirQW()  ) ++m_hpdReflHitsAirQW[rich];
            //  if ( mchit->hpdReflAirPC()  ) ++m_hpdReflHitsAirPC[rich];
            //  if ( mchit->hpdReflSi()     ) ++m_hpdReflHitsSi[rich];
            // if ( mchit->hpdReflKovar()  ) ++m_hpdReflHitsKovar[rich];
            // if ( mchit->hpdReflKapton() ) ++m_hpdReflHitsKapton[rich];
            // if ( mchit->hpdReflPCQW()   ) ++m_hpdReflHitsPCQW[rich];
          //  }
        }
        

        // radiator counters
        if ( !mchit->radiatorInfoValid() )
        {
          if ( mchit->richInfoValid() ) ++m_invalidRadHits[rich];
        }
        else
        {
          if ( mchit->isSignal() ) ++m_radHits[rad];
          //          if ( Rich::Aerogel == rad )
          //  {
          //  ++m_aeroTileHits[mchit->aerogelTileID()];
          // }
          if ( mchit->scatteredPhoton() ) ++m_scatHits[rad];
          if ( !mcPart                  ) ++m_nomcpHits[rad];
        }

        if ( msgLevel(MSG::VERBOSE) )
        {
          verbose() << "Created MCRichHit " << *mchit << endreq;
        }

      } // end loop on hits in the collection

    } // end loop on collections

    // Verify that all hits are stored for output.
    if ( hits->size() != totalSize )
    {
      return Error("MCRichHits and RichG4HitCollection have different sizes !");
    }

  }
  else
  {
    info() << "No RichG4Hits to be converted since no collections available"
           << endmsg;
  }

  return StatusCode::SUCCESS;
}
LHCb::RichSmartID GetMCCkvHitsAlg::assembleMCPmtRichSmartID(const CkvG4Hit * aHit , bool isAGrandPmtHitFlag) 
{
  const int CurRich= aHit ->GetCurRichDetNum();
  const int CurSide = aHit ->GetCurSectorNum();
  const int CurM = aHit ->CurModuleNum();
  const int CurP = aHit ->CurPmtNum();
  const int CurMA = (int) CurP/m_NumPmtInAModule;
  const int CurPInM  = CurP - ( CurMA * m_NumPmtInAModule);
  
  const int CurPiX= aHit ->GetCurPixelXNum();
  const int CurPiY = aHit ->GetCurPixelYNum();  
  Rich::DetectorType iRich = Rich::Rich1;
  Rich::Side iSide = Rich::top;
  //   const int CurPdLensFlag=(int)  aHit->pdWithLens() ;
   
  // CherenkovPmtLensUtil * aCherenkovPmtLensUtil =  CherenkovPmtLensUtil::getInstance();
  // bool CurModuleWithLensFlag =aCherenkovPmtLensUtil->isPmtModuleWithLens(CurM);
  //  const int CurPdLensFlag =  (CurModuleWithLensFlag) ? 1 : 0;
  
  if(CurRich == 0 ) {
    iRich = Rich::Rich1;
    if(CurSide == 0 ) {
      iSide = Rich::top;  
    }else if (CurSide == 1 ) {
      iSide = Rich::bottom;
    }    
  }  else if ( CurRich == 1 ) {
    iRich = Rich::Rich2;
    if(CurSide == 0 ) {
      iSide = Rich::left;  
    }else if (CurSide == 1 ) {
      iSide = Rich::right;
    }
    
  }else if( CurRich == 2 ) {
  
    iRich = Rich::Rich2;
    //    iRich = Rich::SuperRich;
    if(CurSide == 0 ) {
      iSide = Rich::left;  
    }else if (CurSide == 1 ) {
      iSide = Rich::right;
    }
    
  }
  
  // test print
  //  info()<<"Local hit pos "<<aHit-> GetLocalPos() <<endmsg;
  // info()<<"Global hit pos "<< aHit->GetGlobalPos() <<endmsg;

  // info()<<"smartIdAssembly  RichDet side "<<   CurRich <<"   "<<CurSide<<endreq;
  
  // info()<<"smartIdInput from Pmt  rich side M P PxX PxY lensflag "<<iRich<<"   "<<iSide<<"  "
  //       <<CurPInM<<"   "<<CurM<<"   "<<CurPiX<<"   "<<CurPiY<<" CurPdLensFlag  "<<CurPdLensFlag<<endmsg; 


  //   info()<<"smartIdInput from Pmt  rich side M P PxX PxY lensflag "<<iRich<<"   "<<iSide<<"  "
  //         <<CurPInM<<"   "<<CurM<<"   "<<CurPiX<<"   "<<CurPiY
  //         <<"   GrandpmtFlag  "<< isAGrandPmtHitFlag   <<endmsg;
  //
       //<<" CurPdLensFlag  "<<CurPdLensFlag

     // LHCb::RichSmartID asmartID_PM_test = LHCb::RichSmartID (iRich,iSide, CurPInM, CurM,
     //                                            LHCb::RichSmartID::MaPMTID );


     //   LHCb::RichSmartID asmartID_withPixelNum_test = LHCb::RichSmartID (iRich,iSide, CurPInM, CurM,CurPiX,CurPiY,
     //                                             LHCb::RichSmartID::MaPMTID );
     //  if(isAGrandPmtHitFlag) asmartID_withPixelNum_test.setLargePMT(true);
     //
  //     asmartID_PM_test.setRichLensFlag(CurPdLensFlag );
  //     asmartID_withPixelNum_test.setRichLensFlag(CurPdLensFlag ); 
     //    info()<<" RichSmarid Pixel "<<asmartID_withPixelNum_test.rich()  <<"  "<<asmartID_withPixelNum_test.panel()   <<"  "
     //        << asmartID_withPixelNum_test.pdNumInCol()   <<"   "<<asmartID_withPixelNum_test.pdCol()  <<"   "
     //      <<"  "<<asmartID_withPixelNum_test.key()
     //      << "    GrandpmtfromID   "<<asmartID_withPixelNum_test.isLargePMT()    <<endmsg;
  //      
  //  info()<<" now det richdet info "<<endmsg;
    
    
    //    SmartDataPtr<DeRichSystem> richsys( detSvc(), DeRichLocations::RichSystem );
  //   const DeRich1 * adeRich1 = getDet<DeRich1>(DeRichLocations::Rich1);
  //  const DeRich2 * adeRich2 = getDet<DeRich2>(DeRichLocations::Rich2);
    
  //  const std::vector<std::string>& panelLocR1 
  //        = adeRich1 -> paramVect<std::string>("Rich1PMTPanelDetElemLocations");
  //  const std::vector<std::string>& panelLocR2 
  //        = adeRich2 -> paramVect<std::string>("Rich2PMTPanelDetElemLocations");
     
  //  SmartDataPtr<DeRichPMTPanel> iDePmtPanelR1T (detSvc(), panelLocR1 [0]);
  //  SmartDataPtr<DeRichPMTPanel> iDePmtPanelR1B (detSvc(), panelLocR1 [1]);
  //  SmartDataPtr<DeRichPMTPanel> iDePmtPanelR2L (detSvc(), panelLocR2 [0]);
  //  SmartDataPtr<DeRichPMTPanel> iDePmtPanelR2R (detSvc(), panelLocR2 [1]);
            
  //  if(adeRich1 && adeRich2 ) {      


         

  //      if(  iDePmtPanelR1T  &&  iDePmtPanelR1B && iDePmtPanelR2L && iDePmtPanelR2R) {
        
      //    std::string aPmtLocationStr= richsys->  getDePDLocation( asmartID );
        //info()<<"Current pmt location string "<<aPmtLocationStr<<endmsg;
    

    //SmartDataPtr<DeRichPMT> iDePmt( detSvc(), aPmtLocationStr );
  //        Gaudi::XYZPoint aPointOnAnode;
        
     
        //    if(iDePmt) {
      
        //Gaudi::XYZPoint aPointOnAnode= iDePmt->detPointOnAnode(asmartID);
     
  //    if(CurRich == 0 ) {
  //        if( CurSide == 0 ) {
  //          aPointOnAnode= iDePmtPanelR1T->detPointOnAnode(asmartID);
  //        }else if (  CurSide == 1 ) {
            
  //          aPointOnAnode= iDePmtPanelR1B->detPointOnAnode(asmartID);
  //        }
  //      }else {
          
  //        if( CurSide == 0 ) {
  //          aPointOnAnode= iDePmtPanelR2L->detPointOnAnode(asmartID);
  //        }else if (  CurSide == 1 ) {
  //          
  //          aPointOnAnode= iDePmtPanelR2R->detPointOnAnode(asmartID);
  //        }
  //        
  //      }        
          
  //          info()<<" Anode Coord from smartID " << aPointOnAnode<<endmsg;
      
      //    }else {
      
      // info()<<" No pmt location obtained "<<endmsg;
      
      // }
  //    }else {
  //    
  //      info()<<" No pmt panel obtained "<<endmsg;
  //      
  //    }
      
      
    
  //  }else {
  //    info()<<" no richdet info  obtained "<<endmsg;
      
      
  //   }
    
    

  //end test print

  //   LHCb::RichSmartID asmartID_withPixelNum = LHCb::RichSmartID (iRich,iSide, CurPInM, CurM,CurPiY,CurPiX,
  //                                               LHCb::RichSmartID::MaPMTID );
   // asmartID_withPixelNum.setRichLensFlag(CurPdLensFlag );

   //  return ( LHCb::RichSmartID (iRich,iSide, CurPInM, CurM,
   //                           CurPiY,CurPiX,CurPdLensFlag,LHCb::RichSmartID::MaPMTID ) );
  //  return ( asmartID_withPixelNum  );


    LHCb::RichSmartID a_detID= LHCb::RichSmartID (iRich,iSide, CurPInM, CurM,
                                                  CurPiY,CurPiX,LHCb::RichSmartID::MaPMTID );
    if(isAGrandPmtHitFlag) a_detID.setLargePMT(true);

   return (a_detID );
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode GetMCCkvHitsAlg::finalize()
{
  // info()<<" Now start GetMCCkvHitsAlgFinalize" <<endmsg;
  
  const Rich::StatDivFunctor occ("%7.2f +-%5.2f");
  //   RichG4RadiatorMaterialIdValues* aRMIdValues= 
  //          RichG4RadiatorMaterialIdValues::RichG4RadiatorMaterialIdValuesInstance();
  //info()<<" Now in GetMCCkvHitsAlgFinalize   "<<m_nEvts<<endmsg;
  

  info() << "Av. # Invalid RICH flags              = "
         << occ(m_invalidRichHits,m_nEvts)
         << endmsg;
  //  info()<<" Now in GetMCCkvHitsAlgFinalize doing printstat   "<<endmsg;
  
  printStat( "Av. # MCRichHits",              m_hitTally );
  printStat( "Av. # Invalid radiator hits",   m_invalidRadHits );
  printStat( "Av. # Signal Hits",             m_signalTally );
  printStat( "Av. # Gas Quartz CK hits",      m_gasQzHits );
  printStat( "Av. # HPD Quartz CK hits",      m_hpdQzHits ); 
  printStat( "Av. # Nitrogen CK hits",        m_nitroHits );
  // printStat( "Av. # Aero Filter CK hits",     m_aeroFilterHits );
  printStat( "Av. # Si back-scattering",      m_siBackScatt );
  printStat( "Av. # Charged Track hits",      m_ctkHits );
  printStat( "Av. # Scintillation hits",       m_scintillationHits );
  printStat( "Av. # All HPD reflection hits", m_hpdReflHits );
  //  printStat( "  Av. # QW/PC refl. hits",      m_hpdReflHitslQWPC );
  // printStat( "  Av. # Chromium refl. hits",   m_hpdReflHitslChr );
  //  printStat( "  Av. # Air/QW refl. hits",     m_hpdReflHitsAirQW );
  // printStat( "  Av. # Air/PC refl. hits",     m_hpdReflHitsAirPC );
  // printStat( "  Av. # PC/QW refl. hits",      m_hpdReflHitsPCQW );
  // printStat( "  Av. # Silicon refl. hits",    m_hpdReflHitsSi );
  // printStat( "  Av. # Kovar refl. hits",      m_hpdReflHitsKovar );
  // printStat( "  Av. # Kapton refl. hits",     m_hpdReflHitsKapton );

  printStat( "Av. # Signal CK MCRichHits",    m_radHits );
  //  printStat( "Av. # Rayleigh scattered hits", m_scatHits );
  printStat( "Av. # MCParticle-less hits",    m_nomcpHits );

  // number of hits in each aerogel tile
  //  info() << "Av. # Aero hits per tile     :" << endreq;
  //   const int maxTileID =
  //  (aRMIdValues-> Rich1AgelTile15CkvRadiatorNum()) - (aRMIdValues-> Rich1AgelTile0CkvRadiatorNum());
  // for ( int iTile = 0; iTile <= maxTileID; ++iTile )
  //  {
  //  info() << "          tile = "; 
  //   if (iTile<10) { info() << " "; }
  //  info() << iTile << " hits = "
  //         << occ(m_aeroTileHits[iTile],m_nEvts) << " / event" << endreq;
  //  }

  return GetMCCkvInfoBase::finalize();
}

void GetMCCkvHitsAlg::printStat( std::string name, DMap & a )
{
  const Rich::StatDivFunctor    occ("%7.2f +-%5.2f");
  const Rich::PoissonEffFunctor eff("%6.2f +-%4.2f");
  //  info() << "Now in printStat Dmap  "<<SuperRichFlag()<<"  "<<  name <<"   "<<m_nEvts<<endreq;
  
  name.resize(30,' ');
  // if(!SuperRichFlag() ) {
    
  info() << name 
         << ": Rich1 = " << occ(a[Rich::Rich1],m_nEvts)
         << " (" << eff(a[Rich::Rich1],m_hitTally[Rich::Rich1]) << "%)"
         << " Rich2 = "  << occ(a[Rich::Rich2],m_nEvts)
         << " (" << eff(a[Rich::Rich2],m_hitTally[Rich::Rich2]) << "%)"
         << endmsg;
  // }else {
    
    // info() << name 
    //      << ": SuperRich = " 
    //       << occ(a[Rich::Rich2],m_nEvts)
    //     << " (" << eff(a[Rich::Rich2],m_hitTally[Rich::Rich2]) << "%)"
    //     << endmsg;
    
    // }
  
  
  
}


void GetMCCkvHitsAlg::printStat( std::string name, RMap & a )
{
  const Rich::StatDivFunctor    occ("%7.2f +-%5.2f");
  const Rich::PoissonEffFunctor eff("%6.2f +-%4.2f");
  //  info() << "Now in printStat Rmap "<<SuperRichFlag()<<"  "<< name <<"  "<<m_nEvts<<endreq;

  name.resize(30,' ');
  //  info() << name
  //       << ": Aero  = " << occ(a[Rich::Aerogel],m_nEvts)
  //        << " (" << eff(a[Rich::Aerogel],m_hitTally[Rich::Rich1]) << "%)"
  // if(!SuperRichFlag() ) {

  info() << name
         << " C4F10 = "  << occ(a[Rich::C4F10],m_nEvts)
         << " (" << eff(a[Rich::C4F10],m_hitTally[Rich::Rich1]) << "%)"
         << " CF4 = "    << occ(a[Rich::CF4],m_nEvts)
         << " (" << eff(a[Rich::CF4],m_hitTally[Rich::Rich2]) << "%)"
         << endmsg;
  //  }else {
    
  //  info() << name
  //       << " C4F10 = "  << occ(a[Rich::C4F10],m_nEvts)
  //       << " (" << eff(a[Rich::C4F10],m_hitTally[Rich::Rich2]) << "%)"
  //       << " CF4 = "    << occ(a[Rich::CF4],m_nEvts)
  //       << " (" << eff(a[Rich::CF4],m_hitTally[Rich::Rich2]) << "%)"
  //       << endmsg;
  //  
  // }
  
  
}



//=============================================================================
