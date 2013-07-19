// $Id: GetMCRichOpticalPhotonsAlg.cpp,v 1.16 2009-07-17 13:46:12 jonrob Exp $
// Include files 

// from Gaudi
//#include "GaudiKernel/DeclareFactoryEntries.h" 

// local
#include "GaussRICH/GetMCRichOpticalPhotonsAlg.h"

// namespaces
using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : GetMCRichOpticalPhotonsAlg
//
// 2005-12-06 : Sajan EASO
// 2007-01-11 : Gloria Corti, adapt to Gaudi v19 (also compatible with v18)
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory moved to Factories.cpp
//DECLARE_ALGORITHM_FACTORY( GetMCRichOpticalPhotonsAlg );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GetMCRichOpticalPhotonsAlg::GetMCRichOpticalPhotonsAlg( const std::string& name,
                                                        ISvcLocator* pSvcLocator)
  : GetMCRichInfoBase ( name , pSvcLocator )
  , m_nEvts           ( 0 )
{
  declareProperty( "MCRichOpticalPhotonsLocation",
                   m_dataToFill = MCRichOpticalPhotonLocation::Default );
  declareProperty( "MCRichHits" , 
                   m_mcRichHits = MCRichHitLocation::Default ) ;
}

//=============================================================================
// Destructor
//=============================================================================
GetMCRichOpticalPhotonsAlg::~GetMCRichOpticalPhotonsAlg() { }

//=============================================================================
// Main execution
//=============================================================================
StatusCode GetMCRichOpticalPhotonsAlg::execute()
{
  debug() << "Execute" << endmsg;

  // Create the photons and put them in the TES
  MCRichOpticalPhotons * photons = new MCRichOpticalPhotons();
  put( photons, dataLocationInTES() );

  // Get the G4 necessary hit collections from GiGa
  G4HCofThisEvent* hitscollections = 0;
  *gigaSvc() >> hitscollections;

  if ( 0 != hitscollections )
  {
    // Get MCRichHits
    const MCRichHits * mcHits = get<MCRichHits>( m_mcRichHits );

    // reserve space in photon container
    photons->reserve( mcHits->size() );

    // note this key is need for consistency with MCRichHit converter
    ++m_nEvts; // Count events
    for ( int iii=0; iii < RichG4HitCollectionName()->RichHCSize(); ++iii )
    {
      const std::string & colName = RichG4HitCollectionName()->RichHCName(iii);

      G4SDManager * fSDM = G4SDManager::GetSDMpointer();
      if ( !fSDM ) return Error( "NULL G4SDManager pointer !!" );
      const int collectionID = fSDM->GetCollectionID(colName);
      if ( -1 == collectionID )
      {
        return Warning( "RICH Collection "+colName+" : ID = -1", StatusCode::SUCCESS );
      }

      const RichG4HitsCollection * myCollection =
        dynamic_cast<RichG4HitsCollection*>(hitscollections->GetHC(collectionID));
      if ( 0 == myCollection )
      {
        return Warning( "Null RICH hit collection "+colName, StatusCode::SUCCESS );
      }

      const int numberofhits = myCollection->entries();

      // convert hits
      for ( int ihit = 0; ihit < numberofhits; ++ihit )
      {
        // Pointer to G4 hit
        const RichG4Hit * g4hit = (*myCollection)[ihit];
        if ( !g4hit ) { Error( "Null RichG4Hit pointer" ); continue; }

        // Rich detector information
        const Rich::DetectorType rich = g4hit->detectorType();
        if ( !richIsActive(rich) ) { continue; }

        // Find associated MCRichHit
        const MCRichHit * mchit = getMCRichHit(g4hit);
        if ( !mchit ) continue;

        // Only create a photon for Cherenkov radiator hits
        if ( mchit->radiator() != Rich::InvalidRadiator )
        {

          // New optical photon object
          MCRichOpticalPhoton * mcPhoton = new MCRichOpticalPhoton();
          // insert in container, with same key as MCRichHit position in container
          photons->insert( mcPhoton, mchit->index() );

          // SmartRef to associated MCRichHit
          mcPhoton->setMcRichHit( mchit );

          // Copy required info from RichG4Hit to MCRichOpticalPhoton

          // Incidence point on HPD
          mcPhoton->setPdIncidencePoint( Gaudi::XYZPoint(g4hit->GetGlobalPEOriginPos()) );

          // Photon energy at production
          mcPhoton->setEnergyAtProduction( static_cast<float>(g4hit->PhotEnergyAtProd()) );

          // Emission point on track
          mcPhoton->setEmissionPoint( Gaudi::XYZPoint(g4hit->GetPhotEmisPt()) );

          // The momentum of the parent track at production
          mcPhoton->setParentMomentum( Gaudi::XYZVector(g4hit->ChTrackMomVect()) );

          // Cherenkov theta and phi at production
          mcPhoton->setCherenkovTheta ( g4hit->ThetaCkvAtProd() );
          mcPhoton->setCherenkovPhi   ( g4hit->PhiCkvAtProd()   );

          // Spherical mirror reflection point
          mcPhoton->setSphericalMirrorReflectPoint( Gaudi::XYZPoint(g4hit->Mirror1PhotonReflPosition()) );

          // Flat mirror reflection point
          mcPhoton->setFlatMirrorReflectPoint( Gaudi::XYZPoint(g4hit->Mirror2PhotonReflPosition()) );

          // exit point from aerogel (only meaningful for aerogel photons)
          mcPhoton->setAerogelExitPoint( Gaudi::XYZPoint(g4hit->OptPhotAgelExitPos()) );

          // Hpd Quartz Window External Surface Incidence Point of the photon
	        mcPhoton->setHpdQWIncidencePoint(Gaudi::XYZPoint(g4hit->HpdQuartzWindowExtSurfPhotIncidentPosition()));

          // Count photons
          ++m_hitTally[mchit->radiator()];

        }

      } // loop over g4 hits

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
StatusCode GetMCRichOpticalPhotonsAlg::finalize()
{
  const Rich::StatDivFunctor occ;

  for ( RadMap::const_iterator iM = m_hitTally.begin(); iM != m_hitTally.end(); ++iM )
  {
    std::string name = Rich::text((*iM).first);
    name.resize(' ',20);
    info() << "Av. # MCRichOpticalPhotons : " << name << " = "
           << occ((*iM).second,m_nEvts) << " / event" << endreq;
  }

  return GetMCRichInfoBase::finalize();  // must be called after all other actions
}

//=============================================================================
