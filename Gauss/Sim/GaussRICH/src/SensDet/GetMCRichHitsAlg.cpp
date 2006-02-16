// $Id: GetMCRichHitsAlg.cpp,v 1.11 2006-02-16 16:17:23 jonrob Exp $

// local
#include "GetMCRichHitsAlg.h"
#include "RichG4RadiatorMaterialIdValues.h"

// namespaces
using namespace LHCb;

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
  : GetMCRichInfoBase     ( name , pSvcLocator      )
  , m_nEvts               ( 0                       )
  , m_hitTally            ( Rich::NRiches, 0        )
  , m_radHits             ( Rich::NRadiatorTypes, 0 )
  , m_invalidRadHits      ( Rich::NRiches, 0        )
  , m_invalidRichHits     ( 0                       )
  , m_ctkHits             ( Rich::NRadiatorTypes, 0 )
  , m_scatHits            ( Rich::NRadiatorTypes, 0 )
  , m_gasQzHits           ( Rich::NRiches, 0 )
  , m_hpdQzHits           ( Rich::NRiches, 0 )
  , m_nitroHits           ( Rich::NRiches, 0 )
  , m_aeroFilterHits      ( Rich::NRiches, 0 )
  , m_nomcpHits           ( Rich::NRadiatorTypes, 0 )
  , m_richDets            ( Rich::NRiches           )
  , m_richDetsLoc         ( Rich::NRiches           )
{
  declareProperty( "MCRichHitsLocation",
                   m_richHitsLocation = MCRichHitLocation::Default );
  m_richDetsLoc[Rich::Rich1] = DeRichLocation::Rich1;
  m_richDetsLoc[Rich::Rich2] = DeRichLocation::Rich2;
  declareProperty( "DetectorLocations", m_richDetsLoc );
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
  m_richDets[Rich::Rich1] = getDet<DeRich1>( m_richDetsLoc[Rich::Rich1] );
  m_richDets[Rich::Rich2] = getDet<DeRich2>( m_richDetsLoc[Rich::Rich2] );

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode GetMCRichHitsAlg::execute()
{
  debug() << "Execute" << endmsg;

  // Create the MCHits and put them in the TES
  MCRichHits* hits = new MCRichHits();
  put( hits, m_richHitsLocation );

  // Get the G4 necessary hit collections from GiGa
  G4HCofThisEvent* hitscollections = 0;

  // get hitscollections from GiGa
  *gigaSvc() >> hitscollections;

  if ( 0 != hitscollections )
  {

    // note this key is need for consistency with MCRichOpticalPhoton converter
    unsigned int globalKey( 0 ), totalSize( 0 );
    ++m_nEvts; // Count events
    // now check the existence of MC particles and get their table.
    if( !( exist<MCParticles>( MCParticleLocation::Default ) ) )
    {
      return Error( " GetMCRichHitsAlg :  MCParticles do not exist at'"
                    + MCParticleLocation::Default +"'" );
    }
    // Get the Geant4->MCParticle table
    GiGaKineRefTable& table = kineSvc()->table();

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
      hits->reserve( numberofhits );
      totalSize += numberofhits;  // count the total num of hits in all collections.

      // now loop through the hits in the current collection.
      for ( int ihit = 0; ihit < numberofhits; ++ihit )
      {

        // Pointer to G4 hit
        const RichG4Hit * g4hit = (*myCollection)[ihit];
        if ( !g4hit ) return Error( "Null RichG4Hit pointer" );

        // Make new persistent hit object
        MCRichHit * mchit = new MCRichHit();
        // add to container
        hits->push_back( mchit );
        if ( hits->size() != globalKey+1 )
        {
          return Error( "Mis-match in MCRichHit keys" );
        }

        // hit position
        const Gaudi::XYZPoint entry = Gaudi::XYZPoint( g4hit->GetGlobalPos());
        mchit->setEntry( entry );

        // energy deposited
        mchit->setEnergy( g4hit->GetEdep() );

        // time of flight
        mchit->setTimeOfFlight( g4hit->RichHitGlobalTime() );

        // Rich detector information
        Rich::DetectorType rich = Rich::InvalidDetector;
        if ( g4hit->GetCurRichDetNum() < 0 )
        {
          Warning( "Found RichG4Hit with invalid RICH flag. Using z coordinate as fix-up" );
          rich = ( entry.z() < 4000 ? Rich::Rich1 : Rich::Rich2 );
        }
        else
        {
          rich = static_cast<Rich::DetectorType>(g4hit->GetCurRichDetNum());
        }
        mchit->setRichInfoValid( true );
        mchit->setRich(rich);

        // Default radiator information
        Rich::RadiatorType rad = Rich::InvalidRadiator;
        mchit->setRadiatorInfoValid( false );
        if ( g4hit->GetRadiatorNumber() >= 0 )
        {
          // Decode the radiator number in the G4 hit
          if      ( Rich1C4F10CkvRadiatorNum == g4hit->GetRadiatorNumber() )
          {
            // Signal C4F10 CK hit
            rad = Rich::C4F10;
          }
          else if ( Rich2CF4CkvRadiatorNum == g4hit->GetRadiatorNumber() )
          {
            // Signal CF4 CK hit
            rad = Rich::CF4;
          }
          else if ( Rich1AgelTile0CkvRadiatorNum <= g4hit->GetRadiatorNumber() &&
                    Rich1AgelTile15CkvRadiatorNum >= g4hit->GetRadiatorNumber() )
          {
            // Signal aerogel hit
            rad = Rich::Aerogel;
            const int aeroID = g4hit->GetRadiatorNumber() - Rich1AgelTile0CkvRadiatorNum;
            if ( aeroID < 2*2*2*2*2 )
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
          }
          else if (Rich1GasQWindowCkvRadiatorNum  == g4hit->GetRadiatorNumber() )
          {
            // background RICH1 Gas Quartz window CK hit
            mchit->setGasQuartzCK( true );
          }
          else if (Rich2GasQWindowCkvRadiatorNum  == g4hit->GetRadiatorNumber() )
          {
            // background RICH2 Gas Quartz window CK hit
            mchit->setGasQuartzCK( true );
          }
          else if ( RichHpdQuartzWindowCkvRadiatorNum == g4hit->GetRadiatorNumber() )
          {
            // background HPD Quartz window CK hit
            mchit->setHpdQuartzCK( true );
          }
          else if ( RichFilterGenericCkvRadiatorNum == g4hit->GetRadiatorNumber() ||
                    RichFilterD263CkvRadiatorNum    == g4hit->GetRadiatorNumber() )
          {
            // Aerogel filter CK hit
            mchit->setAeroFilterCK( true );
          }
          else if ( Rich1NitrogenCkvRadiatorNum == g4hit->GetRadiatorNumber() )
          {
            // RICH1 nitrogen CK hit
            mchit->setNitrogenCK( true );
          }
          else if ( Rich2NitrogenCkvRadiatorNum == g4hit->GetRadiatorNumber() )
          {
            // RICH2 nitrogen CK hit
            mchit->setNitrogenCK( true );
          }
          else
          {
            std::ostringstream mess;
            mess << "Unknown radiator ID " << g4hit->GetRadiatorNumber()
                 << " -> MCRichHit history incomplete";
            Warning ( mess.str(), StatusCode::SUCCESS );
          }
        }
        else if ( g4hit->GetChTrackID() > 0 )
        {
          Warning( "Radiator ID < 0 and track ID > 0 -> Radiator history unknown",
                   StatusCode::SUCCESS );
        }
        else
        {
          Warning( "Radiator ID < 0 and track ID < 0 -> Radiator history unknown",
                   StatusCode::SUCCESS );
        }

        // If a signel hit, store info
        if ( rad != Rich::InvalidRadiator )
        {
          mchit->setRadiatorInfoValid( true );
          mchit->setRadiator( rad );
        }

        // charged track hitting HPD flag
        if ( g4hit->GetChTrackID() < 0 )
        {
          mchit->setChargedTrack( true );
        }
        // Rayleigh scattered flag
        if ( g4hit->OptPhotRayleighFlag() > 0 )
        {
          mchit->setScatteredPhoton( true );
        }

        // get sensitive detector identifier from det elem
        // const int detID = m_richDets[rich]->sensitiveVolumeID( entry );
        // get HPD number from hit
        const int detID = g4hit->GetCurHpdNum();
        // Photon detector number
        mchit->setSensDetID( detID );

        // fill reference to MCParticle (need to const cast as method is not const !!)
        const int trackID = const_cast<RichG4Hit*>(g4hit)->GetTrackID();
        const MCParticle * mcPart = table[trackID].particle();
        if ( mcPart )
        {
          mchit->setMCParticle( mcPart );
        }
        else
        {
          warning() << "No pointer to MCParticle for MCRichHit associated to G4 Track ID = "
                    << trackID << " Track Momentum = " << g4hit->ChTrackTotMom()
                    << " Hit Energy = " << g4hit->GetEdep()  <<endmsg;
        }

        // now increment the various hit counters

        // RICH counters
        if ( !mchit->richInfoValid() )
        {
          ++m_invalidRichHits;
        }
        else
        {
          ++m_hitTally[mchit->rich()];
          if ( mchit->gasQuartzCK()  ) ++m_gasQzHits[mchit->rich()];
          if ( mchit->hpdQuartzCK()  ) ++m_hpdQzHits[mchit->rich()];
          if ( mchit->nitrogenCK()   ) ++m_nitroHits[mchit->rich()];
          if ( mchit->aeroFilterCK() ) ++m_aeroFilterHits[mchit->rich()];
        }

        // radiator counters
        if ( !mchit->radiatorInfoValid() )
        {
          if ( mchit->richInfoValid() ) ++m_invalidRadHits[mchit->rich()];
        }
        else
        {
          ++m_radHits[mchit->radiator()];
          if ( mchit->chargedTrack()    ) ++m_ctkHits[mchit->radiator()];
          if ( mchit->scatteredPhoton() ) ++m_scatHits[mchit->radiator()];
          if ( !mcPart                  ) ++m_nomcpHits[mchit->radiator()];
        }

        if ( msgLevel(MSG::DEBUG) )
        {
          debug() << "Created MCRichHit " << entry << " energy " << g4hit->GetEdep()
                  << " " << rich << " " << rad << " radID = " << g4hit->GetRadiatorNumber()
                  << " sensDetID " << detID
                  << " MCParticle " << mcPart << endreq;
        }

        // finally increment key for the container
        ++globalKey;

      } // end loop on hits in the collection

    } // end loop on collections

    // Verify that all hits are stored for output.
    if ( hits->size() != totalSize )
    {
      return Error("MCRichHits and RichG4HitCollection have different sizes!");
    }

  }
  else
  {
    info() << "No RichG4Hits to be converted since no Collections available"
           << endmsg;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode GetMCRichHitsAlg::finalize()
{
  const RichStatDivFunctor occ;

  info() << "Av. # Invalid RICH flags           = " << occ(m_invalidRichHits,m_nEvts)
         << endmsg;

  info() << "Av. # MCRichHits           : Rich1 = "
         << occ(m_hitTally[Rich::Rich1],m_nEvts)
         << " Rich2 = " << occ(m_hitTally[Rich::Rich2],m_nEvts)
         << endmsg;
  info() << "Av. # Non CK signal hits   : Rich1 = " << occ(m_invalidRadHits[Rich::Rich1],m_nEvts)
         << " Rich2 = " << occ(m_invalidRadHits[Rich::Rich2],m_nEvts)
         << endmsg;
  info() << "Av. # MCRichHits           : Aero  = " << occ(m_radHits[Rich::Aerogel],m_nEvts)
         << " C4F10 = " <<  occ(m_radHits[Rich::C4F10],m_nEvts)
         << " CF4 = "   <<  occ(m_radHits[Rich::CF4],m_nEvts)
         << endmsg;

  info() << "Av. # Gas Quartz CK hits   : Rich1 = "
         << occ(m_gasQzHits[Rich::Rich1],m_nEvts)
         << " Rich2 = " << occ(m_gasQzHits[Rich::Rich2],m_nEvts)
         << endmsg;
  info() << "Av. # HPD Quartz CK hits   : Rich1 = "
         << occ(m_hpdQzHits[Rich::Rich1],m_nEvts)
         << " Rich2 = " << occ(m_hpdQzHits[Rich::Rich2],m_nEvts)
         << endmsg;
  info() << "Av. # Nitrogen CK hits     : Rich1 = "
         << occ(m_nitroHits[Rich::Rich1],m_nEvts)
         << " Rich2 = " << occ(m_nitroHits[Rich::Rich2],m_nEvts)
         << endmsg;
  info() << "Av. # Aero Filter CK hits  : Rich1 = "
         << occ(m_aeroFilterHits[Rich::Rich1],m_nEvts)
         << " Rich2 = " << occ(m_aeroFilterHits[Rich::Rich2],m_nEvts)
         << endmsg;

  info() << "Av. # Charged Track hits   : Aero  = " << occ(m_ctkHits[Rich::Aerogel],m_nEvts)
         << " C4F10 = " <<  occ(m_ctkHits[Rich::C4F10],m_nEvts)
         << " CF4 = "   <<  occ(m_ctkHits[Rich::CF4],m_nEvts)
         << endmsg;
  info() << "Av. # Scattered hits       : Aero  = " << occ(m_scatHits[Rich::Aerogel],m_nEvts)
         << " C4F10 = " <<  occ(m_scatHits[Rich::C4F10],m_nEvts)
         << " CF4 = "   <<  occ(m_scatHits[Rich::CF4],m_nEvts)
         << endmsg;
  info() << "Av. # MCParticle-less hits : Aero  = " << occ(m_nomcpHits[Rich::Aerogel],m_nEvts)
         << " C4F10 = " <<  occ(m_nomcpHits[Rich::C4F10],m_nEvts)
         << " CF4 = "   <<  occ(m_nomcpHits[Rich::CF4],m_nEvts)
         << endmsg;

  return GetMCRichInfoBase::finalize();  // must be called after all other actions
}

//=============================================================================
