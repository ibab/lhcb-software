// $Id: GetMCRichHitsAlg.cpp,v 1.15 2006-03-01 09:31:26 jonrob Exp $

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
  , m_richDets            ( Rich::NRiches           )
{
  declareProperty( "MCRichHitsLocation",
                   m_dataToFill = MCRichHitLocation::Default );
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
  m_richDets[Rich::Rich1] = getDet<DeRich>( DeRichLocation::Rich1 );
  m_richDets[Rich::Rich2] = getDet<DeRich>( DeRichLocation::Rich2 );

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
        const Gaudi::XYZPoint entry = Gaudi::XYZPoint(g4hit->GetGlobalPos());
        mchit->setEntry( entry );

        // energy deposited
        mchit->setEnergy( g4hit->GetEdep() );

        // time of flight
        mchit->setTimeOfFlight( g4hit->RichHitGlobalTime() );

        // Rich detector information
        const Rich::DetectorType rich = g4hit->detectorType();
        mchit->setRich( rich );

        // radiator information
        const Rich::RadiatorType rad = g4hit->radiatorType();
        const int radID              = g4hit->GetRadiatorNumber();
        if ( g4hit->GetChTrackID() > 0 && rad == Rich::InvalidRadiator )
        {
          std::ostringstream mess;
          mess << "track ID > 0 and Unknown radiator ID " << radID
               << " -> MCRichHit history incomplete";
          Warning ( mess.str(), StatusCode::SUCCESS );
        }

        // store radiator info
        mchit->setRadiator( rad );

        // for aerogel, save tile number
        if ( Rich::Aerogel == rad )
        {
          const int aeroID = radID - Rich1AgelTile0CkvRadiatorNum;
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

        // the following case is when a non-pe charged track passes through
        // a Silicon detector and creates a MIP signal  like in  a tracking det.
        // In this case there is no radiator history, since there was
        // no cherenkov radiation at all. hence the 'warning' flag is commeted out.
        // SE 20-2-06.
        // else
        // {
        //   Warning( "Radiator ID < 0 and track ID < 0 -> Radiator history unknown",
        //         StatusCode::SUCCESS );
        // }

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
        const RichSmartID detID = m_richDets[rich]->sensitiveVolumeID( entry );
        if ( !detID.isValid() )
        {
          std::ostringstream mess;
          mess << "Invalid RichSmartID returned for silicon point " << entry;
          Warning( mess.str() );
        }
        // Fill value into hit
        mchit->setSensDetID( detID );

        // fill reference to MCParticle (need to const cast as method is not const !!)
        const int trackID = (const_cast<RichG4Hit*>(g4hit))->GetTrackID();
        const MCParticle * mcPart = table[trackID].particle();
        if ( mcPart )
        {
          mchit->setMCParticle( mcPart );
        }
        else
        {
          warning() << "No MCParticle for MCRichHit associated to G4 Track ID = "
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
          ++m_hitTally[rich];
          if ( mchit->gasQuartzCK()  ) ++m_gasQzHits[rich];
          if ( mchit->hpdQuartzCK()  ) ++m_hpdQzHits[rich];
          if ( mchit->nitrogenCK()   ) ++m_nitroHits[rich];
          if ( mchit->aeroFilterCK() ) ++m_aeroFilterHits[rich];
        }

        // radiator counters
        if ( !mchit->radiatorInfoValid() )
        {
          if ( mchit->richInfoValid() ) ++m_invalidRadHits[rich];
        }
        else
        {
          ++m_radHits[rad];
          if ( Rich::Aerogel == rad )
          {
            ++m_aeroTileHits[mchit->aerogelTileID()];
          }
          if ( mchit->chargedTrack()    ) ++m_ctkHits[rad];
          if ( mchit->scatteredPhoton() ) ++m_scatHits[rad];
          if ( !mcPart                  ) ++m_nomcpHits[rad];
        }

        if ( msgLevel(MSG::VERBOSE) )
        {
          verbose() << "Created MCRichHit " << entry << " energy " << g4hit->GetEdep()
                    << " " << rich << " " << rad << " radID = " << radID
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

  info() << "Av. # Non CK signal hits   : Rich1 = "
         << occ(m_invalidRadHits[Rich::Rich1],m_nEvts)
         << " Rich2 = " << occ(m_invalidRadHits[Rich::Rich2],m_nEvts)
         << endmsg;

  info() << "Av. # MCRichHits           : Aero  = "
         << occ(m_radHits[Rich::Aerogel],m_nEvts)
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

  info() << "Av. # Charged Track hits   : Aero  = " 
         << occ(m_ctkHits[Rich::Aerogel],m_nEvts)
         << " C4F10 = " << occ(m_ctkHits[Rich::C4F10],m_nEvts)
         << " CF4 = "   << occ(m_ctkHits[Rich::CF4],m_nEvts)
         << endmsg;
  info() << "Av. # Scattered hits       : Aero  = " 
         << occ(m_scatHits[Rich::Aerogel],m_nEvts)
         << " C4F10 = " << occ(m_scatHits[Rich::C4F10],m_nEvts)
         << " CF4 = "   << occ(m_scatHits[Rich::CF4],m_nEvts)
         << endmsg;
  info() << "Av. # MCParticle-less hits : Aero  = " 
         << occ(m_nomcpHits[Rich::Aerogel],m_nEvts)
         << " C4F10 = " << occ(m_nomcpHits[Rich::C4F10],m_nEvts)
         << " CF4 = "   << occ(m_nomcpHits[Rich::CF4],m_nEvts)
         << endmsg;

  // number of hits in each aerogel tile
  info() << "Av. # Aero hits per tile   :" << endreq;
  const int maxTileID = Rich1AgelTile15CkvRadiatorNum-Rich1AgelTile0CkvRadiatorNum;
  for ( int iTile = 0; iTile <= maxTileID; ++iTile )
  {
    info() << "          tile = "; if (iTile<10) { info() << " "; }
    info() << iTile << " hits = "
           << occ(m_aeroTileHits[iTile],m_nEvts) << " / event" << endreq;
  }

  return GetMCRichInfoBase::finalize();  // must be called after all other actions
}

//=============================================================================
