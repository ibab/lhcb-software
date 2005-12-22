// $Id: GetMCRichHitsAlg.cpp,v 1.2 2005-12-22 16:42:43 jonrob Exp $

// local
#include "GetMCRichHitsAlg.h"

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
  , m_bkgHits             ( Rich::NRadiatorTypes, 0 )
  , m_nomcpHits           ( Rich::NRadiatorTypes, 0 )
{
  declareProperty( "MCRichHitsLocation", m_richHitsLocation = MCRichHitLocation::Default );
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

  // add custom initialisations here if needed

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
      int collectionID = fSDM->GetCollectionID(colName);
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
        RichG4Hit * g4hit = (*myCollection)[ihit];

        // Make new persistent hit object
        MCRichHit * mchit = new MCRichHit();

        // add to container
        hits->push_back( mchit );
        if ( hits->size() != globalKey+1 )
        {
          return Error( "Mis-match in MCRichHit keys" );
        }

        // hit position
        mchit->setEntry( Gaudi::XYZPoint(g4hit->GetGlobalPos()) );

        // energy deposited
        mchit->setEnergy( g4hit->GetEdep() );

        // time of flight
        mchit->setTimeOfFlight( g4hit->RichHitGlobalTime() );

        // Rich detector information
        if ( g4hit->GetCurRichDetNum() < 0 )
        {
          mchit->setRichInfoValid( false );
          Warning( "Found RichG4Hit with invalid RICH flag" );
        } else {
          mchit->setRichInfoValid( true );
          mchit->setRich(static_cast<Rich::DetectorType>(g4hit->GetCurRichDetNum()));
        }

        // Photon detector number
        // CRJ : Note to Sajan - Need to add something meaningfull here.
        mchit->setSensDetID( -1 );

        // Radiator information
        if ( g4hit->GetRadiatorNumber() < 0 ) 
        {
          mchit->setRadiatorInfoValid( false );
          Warning( "Found RichG4Hit with default radiator flag. Possibly direct charged track hit" );
        } 
        else 
        {
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

  return GetMCRichInfoBase::finalize();  // must be called after all other actions
}

//=============================================================================
