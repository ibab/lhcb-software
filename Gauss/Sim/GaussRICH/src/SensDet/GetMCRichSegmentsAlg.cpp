// $Id: GetMCRichSegmentsAlg.cpp,v 1.13 2009-07-17 13:46:12 jonrob Exp $
// Include files 

// from Gaudi
//#include "GaudiKernel/DeclareFactoryEntries.h" 

// local
#include "GaussRICH/GetMCRichSegmentsAlg.h"

// namespaces
using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : GetMCRichSegmentsAlg
//
// 2005-12-06 : Sajan EASO
// 2007-01-11 : Gloria Corti, adapt to Gaudi v19 (also compatible with v18)
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory moved to Factories.cpp
//DECLARE_ALGORITHM_FACTORY( GetMCRichSegmentsAlg );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GetMCRichSegmentsAlg::GetMCRichSegmentsAlg( const std::string& name,
                                            ISvcLocator* pSvcLocator)
  : GetMCRichInfoBase ( name , pSvcLocator )
  , m_nEvts           ( 0 )
{
  declareProperty( "MCRichOpticalPhotonsLocation",
                   m_richPhotonsLocation  = MCRichOpticalPhotonLocation::Default );
  declareProperty( "MCRichHitsLocation",
                   m_richHitsLocation  = MCRichHitLocation::Default );
  declareProperty( "MCRichSegmentsLocation",
                   m_dataToFill = MCRichSegmentLocation::Default);
}

//=============================================================================
// Destructor
//=============================================================================
GetMCRichSegmentsAlg::~GetMCRichSegmentsAlg() { }

//=============================================================================
// Main execution
//=============================================================================
StatusCode GetMCRichSegmentsAlg::execute()
{
  debug() << "Execute" << endmsg;

  // new container for segments
  MCRichSegments * segments = new  MCRichSegments ();
  put( segments, dataLocationInTES() );
  segments->reserve( 100 );

  // Get the G4 necessary hit collections from GiGa
  G4HCofThisEvent* hitscollections = 0;

  // get hitscollections from GiGa
  *gigaSvc() >> hitscollections;

  if ( 0 != hitscollections )
  {

    // Locate the MCRichOpticalPhotons
    const MCRichOpticalPhotons * photons = get<MCRichOpticalPhotons>( m_richPhotonsLocation ) ;

    // map of vectors to store Hits associated to each MCParticle
    typedef std::pair< const MCParticle*, Rich::RadiatorType> HitDataListKey;
    typedef std::pair< const RichG4Hit*, const LHCb::MCRichHit* > HitDataType;
    typedef std::vector< HitDataType > G4HitList;
    typedef std::map< const HitDataListKey, G4HitList > HitDataList;
    HitDataList sortedHits;

    ++m_nEvts; // Count events
    for ( int iii = 0; iii < RichG4HitCollectionName()->RichHCSize(); ++iii )
    {
      std::string colName = RichG4HitCollectionName()->RichHCName(iii);

      G4SDManager * fSDM = G4SDManager::GetSDMpointer();
      if ( !fSDM ) return Error( "NULL G4SDManager pointer !!" );
      int collectionID = fSDM->GetCollectionID(colName);
      if ( -1 == collectionID ) return StatusCode::SUCCESS;

      RichG4HitsCollection * myCollection =
        dynamic_cast<RichG4HitsCollection*>(hitscollections->GetHC(collectionID));
      if ( 0 == myCollection )
        return Error( "Null RichG4HitsCollection", StatusCode::SUCCESS );

      // Reference table between G4 tracks/trajectories and MC particles
      const GiGaKineRefTable & table = kineSvc()->table();

      // loop over hits and sort them
      for ( int ihit = 0; ihit < myCollection->entries(); ++ihit )
      {
        // Pointer to G4 hit
        const RichG4Hit * g4hit = (*myCollection)[ihit];
        if ( !g4hit ) { Error( "Null RichG4Hit pointer" ); continue; }

        // Rich detector information
        const Rich::DetectorType rich = g4hit->detectorType();
        if ( !richIsActive(rich) ) { continue; }

        if ( g4hit->GetRadiatorNumber() >= 0 )
        {
          const Rich::RadiatorType rad = g4hit->radiatorType();

          // get MCParticle information
          const int traid = const_cast<RichG4Hit*>(g4hit)->GetTrackID();
          const MCParticle * mcPart = table[traid].particle();
          if ( !mcPart )
          {
            warning() << "No pointer to MCParticle for "
                      << " MCRichHit associated to trackID: "
                      << iii << "  " << ihit << "   " << traid << endreq;
          }
          else
          {
            // get MCRichHit
            const LHCb::MCRichHit * mchit = getMCRichHit(g4hit);
            if ( mchit )
            {
              // add to sorted list
              const HitDataListKey key(mcPart,rad);
              const HitDataType   data(g4hit,mchit);
              sortedHits[key].push_back( data );
            }
          }
        }
        else
        {
          if ( msgLevel(MSG::DEBUG) )
          {
            debug() << "RichG4Hit " << ihit << " has no radiator information !"
                    << endreq;
          }
        }

      } // end loop over this collection

    } // end loop over all collections

      // loop over each set of hits associated to a single MCParticle and radiator
    for ( HitDataList::const_iterator iList = sortedHits.begin();
          iList != sortedHits.end(); ++iList )
    {
      const MCParticle * mcPart    = ((*iList).first).first;
      const Rich::RadiatorType rad = ((*iList).first).second;
      const G4HitList & g4hitList  = (*iList).second;

      // New MCRichSegment
      MCRichSegment * mcSeg = new MCRichSegment();
      segments->insert( mcSeg );

      // set information
      mcSeg->setRadiator( rad );
      mcSeg->setRich( Rich::Rich2Gas == rad ? Rich::Rich2 : Rich::Rich1 );
      mcSeg->setMcParticle( mcPart );

      debug() << "Creating MCRichSegment " << mcSeg->key()
              << " for MCParticle " << mcPart->key()
              << " in " << rad << endreq;

      // Count segments
      ++m_hitTally[rad];

      // loop over hits
      MomentaAtZ momenta;
      for ( G4HitList::const_iterator iHit = g4hitList.begin();
            iHit != g4hitList.end(); ++iHit )
      {
        if ( msgLevel(MSG::VERBOSE) )
        {
          verbose()
            << " Hit : Prod-pnt  " << Gaudi::XYZPoint((*iHit).first->GetPhotEmisPt()) << endreq
            << "     : Pre-step  " << Gaudi::XYZPoint((*iHit).first->ChTrackCkvPreStepPos()) << endreq
            << "     : post-step " << Gaudi::XYZPoint((*iHit).first->ChTrackCkvPostStepPos()) << endreq
            << "     : tk Mon.   " << Gaudi::XYZVector((*iHit).first->ChTrackMomVect()) << endreq;
        }

        // Add momentum to list of momenta
        momenta.insert( MomentumAtZ( Gaudi::XYZVector((*iHit).first->ChTrackMomVect()),
                                     Gaudi::XYZPoint((*iHit).first->ChTrackCkvPreStepPos()) )  );
        momenta.insert( MomentumAtZ( Gaudi::XYZVector((*iHit).first->ChTrackMomVect()),
                                     Gaudi::XYZPoint((*iHit).first->ChTrackCkvPostStepPos()) ) );

        // MCRichHit Pointer
        const MCRichHit * mchit = (*iHit).second;
        if ( !mchit ) { Warning("Null MCRichHit !!").ignore(); continue; }

        // MCRichOpticalPhoton pointer
        const MCRichOpticalPhoton * mcphot = photons->object( mchit->index() );
        // not all hits have photons, so only add if not null
        if ( mcphot ) { mcSeg->addToMCRichOpticalPhotons( mcphot ); }

      }

      // Finally, loop over sorted trajectory points and add to segment in order
      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << " Found " << momenta.size() << " trajectory points" << endreq;
      }
      for ( MomentaAtZ::const_iterator iMom = momenta.begin();
            iMom != momenta.end(); ++iMom )
      {
        mcSeg->addToTrajectoryMomenta ( (*iMom).first  );
        mcSeg->addToTrajectoryPoints  ( (*iMom).second );
        if ( msgLevel(MSG::DEBUG) )
        {
          debug()
            << "  Added trajectory momentum " << (*iMom).first
            << " at " << (*iMom).second << endreq;
        }
      }

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
StatusCode GetMCRichSegmentsAlg::finalize()
{
  const Rich::StatDivFunctor occ;

  for ( RadMap::const_iterator iM = m_hitTally.begin(); iM != m_hitTally.end(); ++iM )
  {
    std::string name = Rich::text((*iM).first);
    name.resize(' ',20);
    info() << "Av. # MCRichSegments       : " << name << " = "
           << occ((*iM).second,m_nEvts) << " / event" << endreq;
  }

  return GetMCRichInfoBase::finalize();
}

//=============================================================================
