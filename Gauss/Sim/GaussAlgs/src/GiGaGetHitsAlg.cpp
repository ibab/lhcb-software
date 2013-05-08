// $Id: GiGaGetHitsAlg.cpp,v 1.13 2009-01-26 12:02:48 jonrob Exp $
// Include files

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/Stat.h"

/// from Event
#include "Event/MCHit.h"
#include "Event/MCCaloHit.h"
#include "Event/MCRichHit.h"
#include "Event/MCRichOpticalPhoton.h"
#include "Event/MCRichTrack.h"
#include "Event/MCRichSegment.h"

// local
#include "GiGaGetHitsAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GiGaGetHitsAlg
//
// 2002-08-13 : Witold Pokorski
// 2004-11-22 : Gloria Corti
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( GiGaGetHitsAlg );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GiGaGetHitsAlg::GiGaGetHitsAlg( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_caloHits     ()
{
  m_caloHits.push_back ( LHCb::MCCaloHitLocation::Spd  ) ;
  m_caloHits.push_back ( LHCb::MCCaloHitLocation::Prs  ) ;
  m_caloHits.push_back ( LHCb::MCCaloHitLocation::Ecal ) ;
  m_caloHits.push_back ( LHCb::MCCaloHitLocation::Hcal ) ;
  m_caloDet.push_back ( "Spd" );
  m_caloDet.push_back ( "Prs" );
  m_caloDet.push_back ( "Ecal" );
  m_caloDet.push_back ( "Hcal" );

  declareProperty( "VeloHits",   m_velohits   = LHCb::MCHitLocation::Velo );
  declareProperty( "PuVetoHits", m_puvelohits = LHCb::MCHitLocation::PuVeto );
  declareProperty( "TTHits",     m_tthits = LHCb::MCHitLocation::TT );
  declareProperty( "SLHits",     m_slhits = LHCb::MCHitLocation::SL );
  declareProperty( "ITHits",     m_ithits = LHCb::MCHitLocation::IT );
  declareProperty( "OTHits",     m_othits = LHCb::MCHitLocation::OT );
  declareProperty( "FTHits",     m_fthits = LHCb::MCHitLocation::FT );
  declareProperty( "UTHits",     m_uthits = LHCb::MCHitLocation::UT );
  declareProperty( "CaloHits",   m_caloHits );
  declareProperty( "MuonHits",   m_muonhits = LHCb::MCHitLocation::Muon );
  declareProperty( "RichHits",   m_richhits = LHCb::MCRichHitLocation::Default );
  declareProperty( "RichOpticalPhotons", m_richop = "" );
  declareProperty( "RichTracks",         m_richtracks = "" );
  declareProperty( "RichSegments",       m_richsegments = "" );
  declareProperty( "ExtraHits",          m_extraHits = "" );

}

//=============================================================================
// Destructor
//=============================================================================
GiGaGetHitsAlg::~GiGaGetHitsAlg() {};

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode GiGaGetHitsAlg::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode GiGaGetHitsAlg::execute() {

  debug() << "==> Execute" << endmsg;

  // Velo hits
  hitsTracker( "Velo", m_velohits );

  // Velo PileUp hits
  hitsTracker( "VeloPU", m_puvelohits );

  // Trigger Tracker hits
  hitsTracker( "TT", m_tthits );

  // Inner Tracker hits
  hitsTracker( "IT", m_ithits );
  
  // SL  hits
  //hitsTracker( "SL", m_slhits );

  // Outer Tracker hits
  hitsTracker( "OT", m_othits );

  // RICH info: hits, optical photons, segments and
  infoRICH( );

  // Calorimeter hits
  unsigned int idet = 0;
  for(Addresses::const_iterator address = m_caloHits.begin() ;
      m_caloHits.end() != address ; ++address ) {
    if( (*address).empty() ) continue;
    LHCb::MCCaloHits* hits = get<LHCb::MCCaloHits>( *address );
    debug() << "Number of extracted MCCaloHits  '" << *address << "' \t"
            << hits->size() << endmsg ;
    Stat stat( chronoSvc(), "#"+m_caloDet[idet]+" MCHits", hits->size() );
    ++idet;
  }

  // Muon hits
  hitsTracker( "Muon", m_muonhits );

  // Extra detector hits
  hitsTracker( "Extra" , m_extraHits );

  return StatusCode::SUCCESS;

};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode GiGaGetHitsAlg::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
// Get hits for Trackers: Velo, PuVeto, TT, IT and OT
// If verbose print some details
//=============================================================================
void GiGaGetHitsAlg::hitsTracker(const std::string det,
                                 const std::string location)
{
  if( !location.empty() ) {
    LHCb::MCHits* obj = get<LHCb::MCHits>( location );
    std::string dethits = det+" MCHits";
    debug() << "Number of extracted MCHits      '"
            << location << "'  \t"
            << obj->size()
            << endmsg;
    Stat stat( chronoSvc(), "#"+dethits, obj->size() );
    if( msgLevel( MSG::VERBOSE ) ) {
      int icount = 0;
      for( LHCb::MCHits::const_iterator hiter=obj->begin(); hiter!=obj->end();
           ++hiter ) {
        verbose() << dethits << " " << icount++ << std::endl
                  << *(*hiter) << std::endl
                  << " from MCParticle " << (*hiter)->mcParticle()->key()
                  << "  pdg " << (*hiter)->mcParticle()->particleID().pid()
                  << "  E " << (*hiter)->mcParticle()-> momentum().e()
                  << endmsg;
      }
    }
  }
  return;
}

//=============================================================================
// Get hits for RICH: standard,
// If verbose print some details
//=============================================================================
void GiGaGetHitsAlg::infoRICH()
{
  // MCRichHits
  if( !m_richhits.empty() ) {
    LHCb::MCRichHits* obj = get<LHCb::MCRichHits>( m_richhits );
    debug() << "Number of extracted MCRichHits  '"
            << m_richhits << "' \t"
            << obj->size()
            << endmsg ;
    unsigned int nHitsRich1 = 0, nHitsRich2 = 0;
    int icount = 0;
    for( LHCb::MCRichHits::const_iterator hiter=obj->begin();
         hiter!=obj->end(); ++hiter ) {
      if( (*hiter)->rich() == Rich::Rich1 ) nHitsRich1++;
      if( (*hiter)->rich() == Rich::Rich2 ) nHitsRich2++;
      verbose() << "MCRichHit " << icount++ << std::endl
                << *(*hiter) << std::endl;
      //       verbose() << "Rich: " << (*hiter)->rich()
      //                 << " Radiator: " << (*hiter)->radiator()
      //                 << "  Entry point: " << (*hiter)->entry();
      if( (*hiter)->mcParticle() != NULL ) {
        verbose() << " from MCParticle: " << (*hiter)->mcParticle()->key()
                  << "  pdg " << (*hiter)->mcParticle()->particleID().pid()
                  << "  E " << (*hiter)->mcParticle()-> momentum().e()
                  <<  endmsg;
      } else {
        verbose() << "   No MCParticle " << endmsg;
      }
    }
    Stat stat  ( chronoSvc(), "#Rich MCHits", obj->size() );
    Stat statR1( chronoSvc(), "#Rich1 MCHits", nHitsRich1 );
    Stat statR2( chronoSvc(), "#Rich2 MCHits", nHitsRich2 );
  }


  // Optical photons
  if( !m_richop.empty() ) {
    LHCb::MCRichOpticalPhotons* obj =
      get<LHCb::MCRichOpticalPhotons>( m_richop );
    debug() << "Number of extracted MCRichOpticalPhotons '"
            << m_richop << "' \t"
            << obj->size()
            << endmsg;
    for( LHCb::MCRichOpticalPhotons::const_iterator hiter=obj->begin();
         hiter!=obj->end();  ++hiter ) {
      verbose() << "MCRichOpPhotons " << std::endl << *(*hiter) << endmsg;
    }
    Stat stat( chronoSvc() , "#MCRichOpPhotons" , obj->size() );
  }


  // Rich segments
  if( !m_richsegments.empty() ) {
    LHCb::MCRichSegments* obj = get<LHCb::MCRichSegments>( m_richsegments );
    debug() << "Number of extracted MCRichSegments '"
            << m_richsegments << "' \t"
            << obj->size()
            << endmsg;
    for( LHCb::MCRichSegments::const_iterator hiter=obj->begin();
         hiter!=obj->end();  ++hiter ) {
      verbose() << "MCRichSegments " << std::endl << *(*hiter) << endmsg;
    }
    Stat stat( chronoSvc(), "#MCRichSegments", obj->size() );
  }


  // Rich tracks
  if( !m_richtracks.empty() ) {
    LHCb::MCRichTracks* obj = get<LHCb::MCRichTracks>( m_richtracks );
    debug() << "Number of extracted MCRichTracks '"
            << m_richtracks << "' \t"
            << obj->size()
            << endmsg;
    Stat stat( chronoSvc(), "#MCRichTracks", obj->size() );
  }

  return;
}



//=============================================================================

