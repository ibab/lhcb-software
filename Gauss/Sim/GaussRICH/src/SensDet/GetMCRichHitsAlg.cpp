// $Id: GetMCRichHitsAlg.cpp,v 1.36 2009-07-17 13:46:12 jonrob Exp $
// Include files

// from Gaudi
//#include "GaudiKernel/DeclareFactoryEntries.h"

// local
#include "GaussRICH/GetMCRichHitsAlg.h"
#include "GaussRICH/RichG4RadiatorMaterialIdValues.h"

// namespaces
using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : GetMCRichHitsAlg
//
// 2005-12-06 : Sajan EASO
// 2007-01-11 : Gloria Corti, adapt to Gaudi v19 (also compatible with v18)
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory moved to Factories.cpp
//DECLARE_ALGORITHM_FACTORY( GetMCRichHitsAlg );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GetMCRichHitsAlg::GetMCRichHitsAlg( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : GetMCRichInfoBase     ( name , pSvcLocator )
  , m_nEvts               ( 0                  )
  , m_invalidRichHits     ( 0                  )
  , m_richDets            ( Rich::NRiches      )
{
  declareProperty( "MCRichHitsLocation",
                   m_dataToFill = MCRichHitLocation::Default );
  declareProperty( "MCParticles" ,
                   m_mcParticles = MCParticleLocation::Default ) ;
}

//=============================================================================
// Destructor
//=============================================================================
GetMCRichHitsAlg::~GetMCRichHitsAlg() { }

//=============================================================================
// Initialization
//=============================================================================
StatusCode GetMCRichHitsAlg::initialize()
{
  const StatusCode sc = GetMCRichInfoBase::initialize();
  if ( sc.isFailure() ) return sc;

  // Get RichDet objects
  if ( richIsActive(Rich::Rich1) )
    m_richDets[Rich::Rich1] = getDet<DeRich>( DeRichLocations::Rich1 );
  if ( richIsActive(Rich::Rich2) )
    m_richDets[Rich::Rich2] = getDet<DeRich>( DeRichLocations::Rich2 );

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode GetMCRichHitsAlg::execute()
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
      return Error( "GetMCRichHitsAlg : MCParticles do not exist at '"
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
      RichG4HitsCollection* myCollection =
        dynamic_cast<RichG4HitsCollection*>(hitscollections->GetHC(collectionID));
      if ( 0 == myCollection )
      {
        return Warning( "Null RICH hit collection "+colName, StatusCode::SUCCESS );
      }
      const int numberofhits = myCollection->entries();  // num of hits in this collection

      // reserve space
      totalSize += numberofhits;  // count the total num of hits in all collections.

      // now loop through the hits in the current collection.
      for ( int ihit = 0; ihit < numberofhits; ++ihit )
      {
        // Pointer to G4 hit
        const RichG4Hit * g4hit = (*myCollection)[ihit];
        if ( !g4hit ) { Error( "Null RichG4Hit pointer" ); continue; }

        // Rich detector information
        const Rich::DetectorType rich = g4hit->detectorType();
        if ( !richIsActive(rich) ) { continue; }

        // Hit position
        const Gaudi::XYZPoint hitPosition(g4hit->GetGlobalPos());

        // get sensitive detector identifier from det elem
        const RichSmartID detID( m_richDets[rich]->sensitiveVolumeID(hitPosition) );
        if ( !detID.isValid() )
        {
          std::ostringstream mess;
          mess << "Invalid RichSmartID returned for silicon point " << hitPosition;
          Warning( mess.str() );
          continue;
        }

        // Check HPD is active. If not, skip making any MCRichHits for it.
        const RichSmartID hpdID = detID.pdID();
        if ( !deRichSystem()->pdIsActive(hpdID) )
        {
          std::ostringstream mess;
          mess << "Inactive HPD skipped " << hpdID;
          Warning( mess.str(), StatusCode::SUCCESS, 1 );
          continue;
        }

        // Make new persistent hit object
        MCRichHit * mchit = new MCRichHit();
        // add to container
        hits->push_back( mchit );

        // store relation between G4 hit and this MCRichHit
        relationTable->relate( g4hit, mchit );

        // Fill sensitive det ID into hit
        mchit->setSensDetID( detID );

        // hit position
        mchit->setEntry( hitPosition );

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
          const int aeroID = 
                    aRMIdValues->Rich1AgelRadiatorNumToFullTileNum(radID);
          const int aeroSubTileID=  
                    aRMIdValues->Rich1AgelRadiatorNumToSubTileNumInFullTile(radID);          

          if ( aeroID >=0 && aeroID < 2*2*2*2*2 ) // Aerogel tile ID field has 5 bits allocated
          {
            mchit->setAerogelTileID( aeroID );
            
          }
          else
          {
            //  std::ostringstream mess;
            // mess << "Aerogel ID " << aeroID
            //     << " too large to pack into MCRichHit !!";
            // Warning ( mess.str(), StatusCode::FAILURE );
          }
          if( (aeroSubTileID >=0)   && (aeroSubTileID < 2*2*2*2*2*2) ) 
                 // Aerogel Sub tile ID has 6 bits allocated
          {
              mchit->setAerogelSubTileID ( aeroSubTileID);
          }
          
        }

        // charged track hitting HPD flag
        mchit->setChargedTrack( g4hit->GetChTrackID() < 0 );
        
        // Rayleigh scattered flag
        mchit->setScatteredPhoton( g4hit->OptPhotRayleighFlag() > 0 );
        
        // Back scattered electrons
        mchit->setHpdSiBackscatter( g4hit->ElectronBackScatterFlag() );

        // Photon produced by Scintilation process 
        mchit->setRadScintillation( g4hit->isScintillation() );        

        // HPD reflections
        mchit->setHpdReflQWPC   ( g4hit->isHpdQwPCRefl()     );
        mchit->setHpdReflChr    ( g4hit->isHpdChromiumRefl() );
        mchit->setHpdReflAirQW  ( g4hit->isHpdAirQwRefl()    );
        mchit->setHpdReflAirPC  ( g4hit->isHpdAirPCRefl()    );
        mchit->setHpdReflSi     ( g4hit->isHpdSiliconRefl()  );
        mchit->setHpdReflKovar  ( g4hit->isHpdKovarRefl()    );
        mchit->setHpdReflKapton ( g4hit->isHpdKaptonRefl()   );
        mchit->setHpdReflPCQW   ( g4hit->isHpdPCQwRefl()     );

        // fill reference to MCParticle (need to const cast as method is not const !!)
        RichG4Hit* nonconstg4hit = const_cast<RichG4Hit*>(g4hit);  
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
          if ( mchit->radScintillation() ) ++m_scintillationHits[rich];
          if ( mchit->chargedTrack()     ) ++m_ctkHits[rich];
          if ( mchit->hpdReflection()    )  
          {
            ++m_hpdReflHits[rich];
            if ( mchit->hpdReflQWPC()   ) ++m_hpdReflHitslQWPC[rich];
            if ( mchit->hpdReflChr()    ) ++m_hpdReflHitslChr[rich];
            if ( mchit->hpdReflAirQW()  ) ++m_hpdReflHitsAirQW[rich];
            if ( mchit->hpdReflAirPC()  ) ++m_hpdReflHitsAirPC[rich];
            if ( mchit->hpdReflSi()     ) ++m_hpdReflHitsSi[rich];
            if ( mchit->hpdReflKovar()  ) ++m_hpdReflHitsKovar[rich];
            if ( mchit->hpdReflKapton() ) ++m_hpdReflHitsKapton[rich];
            if ( mchit->hpdReflPCQW()   ) ++m_hpdReflHitsPCQW[rich];
          }
        }

        // radiator counters
        if ( !mchit->radiatorInfoValid() )
        {
          if ( mchit->richInfoValid() ) ++m_invalidRadHits[rich];
        }
        else
        {
          if ( mchit->isSignal() ) ++m_radHits[rad];
          if ( Rich::Aerogel == rad )
          {
            ++m_aeroTileHits[mchit->aerogelTileID()];
          }
          if ( mchit->scatteredPhoton() ) ++m_scatHits[rad];
          if ( !mcPart                  ) ++m_nomcpHits[rad];
        }

        if ( msgLevel(MSG::VERBOSE) )
        {
          verbose() << "Created MCRichHit " << *mchit << endreq;
        }

      } // end loop on hits in the collection

    } // end loop on collections

  }
  else
  {
    info() << "No RichG4Hits to be converted since no collections available"
           << endmsg;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode GetMCRichHitsAlg::finalize()
{
  const Rich::StatDivFunctor occ("%7.2f +-%5.2f");

    RichG4RadiatorMaterialIdValues* aRMIdValues= 
            RichG4RadiatorMaterialIdValues::RichG4RadiatorMaterialIdValuesInstance();

  info() << "Av. # Invalid RICH flags              = "
         << occ(m_invalidRichHits,m_nEvts)
         << endmsg;

  printStat( "Av. # MCRichHits",              m_hitTally );
  printStat( "Av. # Invalid radiator hits",   m_invalidRadHits );
  printStat( "Av. # Signal Hits",             m_signalTally );
  printStat( "Av. # Gas Quartz CK hits",      m_gasQzHits );
  printStat( "Av. # HPD Quartz CK hits",      m_hpdQzHits ); 
  printStat( "Av. # Nitrogen CK hits",        m_nitroHits );
  printStat( "Av. # Aero Filter CK hits",     m_aeroFilterHits );
  printStat( "Av. # Si back-scattering",      m_siBackScatt );
  printStat( "Av. # Charged Track hits",      m_ctkHits );
  printStat( "Av. # Scintillation hits",       m_scintillationHits );
  printStat( "Av. # All HPD reflection hits", m_hpdReflHits );
  printStat( "  Av. # QW/PC refl. hits",      m_hpdReflHitslQWPC );
  printStat( "  Av. # Chromium refl. hits",   m_hpdReflHitslChr );
  printStat( "  Av. # Air/QW refl. hits",     m_hpdReflHitsAirQW );
  printStat( "  Av. # Air/PC refl. hits",     m_hpdReflHitsAirPC );
  printStat( "  Av. # PC/QW refl. hits",      m_hpdReflHitsPCQW );
  printStat( "  Av. # Silicon refl. hits",    m_hpdReflHitsSi );
  printStat( "  Av. # Kovar refl. hits",      m_hpdReflHitsKovar );
  printStat( "  Av. # Kapton refl. hits",     m_hpdReflHitsKapton );

  printStat( "Av. # Signal CK MCRichHits",    m_radHits );
  printStat( "Av. # Rayleigh scattered hits", m_scatHits );
  printStat( "Av. # MCParticle-less hits",    m_nomcpHits );

  // number of hits in each aerogel tile
  info() << "Av. # Aero hits per tile     :" << endreq;
  const int maxTileID =
    (aRMIdValues-> Rich1AgelTile15CkvRadiatorNum()) - (aRMIdValues-> Rich1AgelTile0CkvRadiatorNum());
  for ( int iTile = 0; iTile <= maxTileID; ++iTile )
  {
    info() << "          tile = "; 
    if (iTile<10) { info() << " "; }
    info() << iTile << " hits = "
           << occ(m_aeroTileHits[iTile],m_nEvts) << " / event" << endreq;
  }

  return GetMCRichInfoBase::finalize();
}

void GetMCRichHitsAlg::printStat( std::string name, DMap & a )
{
  const Rich::StatDivFunctor    occ("%7.2f +-%5.2f");
  const Rich::PoissonEffFunctor eff("%6.2f +-%4.2f");
  name.resize(30,' ');
  info() << name 
         << ": Rich1 = " << occ(a[Rich::Rich1],m_nEvts)
         << " (" << eff(a[Rich::Rich1],m_hitTally[Rich::Rich1]) << "%)"
         << " Rich2 = "  << occ(a[Rich::Rich2],m_nEvts)
         << " (" << eff(a[Rich::Rich2],m_hitTally[Rich::Rich2]) << "%)"
         << endmsg;
}

void GetMCRichHitsAlg::printStat( std::string name, RMap & a )
{
  const Rich::StatDivFunctor    occ("%7.2f +-%5.2f");
  const Rich::PoissonEffFunctor eff("%6.2f +-%4.2f");
  name.resize(30,' ');
  info() << name
         << ": Aero  = " << occ(a[Rich::Aerogel],m_nEvts)
         << " (" << eff(a[Rich::Aerogel],m_hitTally[Rich::Rich1]) << "%)"
         << " Rich1Gas = "  << occ(a[Rich::Rich1Gas],m_nEvts)
         << " (" << eff(a[Rich::Rich1Gas],m_hitTally[Rich::Rich1]) << "%)"
         << " Rich2Gas = "    << occ(a[Rich::Rich2Gas],m_nEvts)
         << " (" << eff(a[Rich::Rich2Gas],m_hitTally[Rich::Rich2]) << "%)"
         << endmsg;
}

//=============================================================================
