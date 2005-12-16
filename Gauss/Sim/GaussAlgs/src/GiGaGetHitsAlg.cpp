// $Id: GiGaGetHitsAlg.cpp,v 1.7 2005-12-16 11:24:11 gcorti Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
// #include "GaudiKernel/ISvcLocator.h"
// #include "GaudiKernel/IDataProviderSvc.h"
// #include "GaudiKernel/SmartDataPtr.h"
// #include "GaudiKernel/DataObject.h"
#include "GaudiKernel/Stat.h"
// #include "GaudiKernel/MsgStream.h"

// from GiGa
//#include "GiGa/IGiGaSvc.h"

/// from Event
#include "Event/MCHit.h"
#include "Event/MCVeloHit.h"
#include "Event/MCMuonHit.h"
#include "Event/MCCaloHit.h"
#include "Event/MCMuonHitPath.h"
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
// 2004-11-22 : G.Corti
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<GiGaGetHitsAlg>          s_factory ;
const        IAlgFactory& GiGaGetHitsAlgFactory = s_factory ;


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GiGaGetHitsAlg::GiGaGetHitsAlg( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
    , m_caloHits     ()
{
  m_caloHits.push_back ( MCCaloHitLocation::Spd  ) ;
  m_caloHits.push_back ( MCCaloHitLocation::Prs  ) ;
  m_caloHits.push_back ( MCCaloHitLocation::Ecal ) ;
  m_caloHits.push_back ( MCCaloHitLocation::Hcal ) ;
  m_caloDet.push_back ( "Spd" );
  m_caloDet.push_back ( "Prs" );
  m_caloDet.push_back ( "Ecal" );
  m_caloDet.push_back ( "Hcal" );

  declareProperty( "OTHits"    , m_othits = MCHitLocation::OTHits );
  declareProperty( "STHits"    , m_sthits = MCHitLocation::ITHits );
  declareProperty( "VeloHits"  , m_velohits = MCVeloHitLocation::Default );
  declareProperty( "PuVeloHits", m_puvelohits = MCVeloHitLocation::PuVeto );
  declareProperty( "MuonHits"  , m_muonhits = MCMuonHitLocation::MCMuonHits );
  declareProperty( "RichHits"  , m_richhits = MCRichHitLocation::Default );
  declareProperty( "RichOpticalPhotons", m_richop = "" );
  declareProperty( "RichTracks", m_richtracks = "" );
  declareProperty( "RichSegments", m_richsegments = "" );
  declareProperty( "CaloHits",           m_caloHits      );
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
  hitsVelo( "Velo", m_velohits );

  // Velo PileUp hits
  hitsVelo( "VeloPU", m_puvelohits );

  // Silicon Tracker hits
  hitsTracker( "ST", m_sthits );
  
  // Outer Tracker hits
  hitsTracker( "OT", m_othits );  
  
  // RICH info: hits, optical photons, segments and 
  infoRICH( );

  // Calorimeter hits
  unsigned int idet = 0;
  for(Addresses::const_iterator address = m_caloHits.begin() ;
      m_caloHits.end() != address ; ++address ) {
    if( (*address).empty() ) continue;
    MCCaloHits* hits = get<MCCaloHits>( *address );
    info() << "Number of extracted MCCaloHits  '" << *address << "' \t"
           << hits->size() << endmsg ;
    Stat stat( chronoSvc(), "#"+m_caloDet[idet]+" MCHits", hits->size() );
    ++idet;
  }

  // Muon hits
  infoMuon();

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
// Get hits for Velo: standard and PileUp
// If verbose print some details
//=============================================================================
void GiGaGetHitsAlg::hitsVelo(const std::string det, 
                              const std::string location)
{
  if( !location.empty() ) {
    MCVeloHits* obj = get<MCVeloHits>( location );
    std::string dethits = det+" MCHits";
    info() << "Number of extracted MCVeloHits  '"
           << location << "'  \t"
           << obj->size()
           << endmsg ;
    Stat stat( chronoSvc(), "#"+dethits, obj->size() ) ;
    
    if( msgLevel( MSG::VERBOSE ) ) {
      for( MCVeloHits::const_iterator vhiter=obj->begin(); vhiter!=obj->end();
           ++vhiter ) {
        verbose() <<  "Energy of MCVeloHit: " 
                  << (*vhiter)->energy()
                  << "   Sensor: " 
                  << (*vhiter)-> sensor()
                  << "   MCParticle: " 
                  << (*vhiter)->mcParticle()->particleID().pid()
                  << "     Energy: " 
                  << (*vhiter)->mcParticle()-> momentum().e()
                  <<  endmsg;
      }
    }
  } 
  return;
}

//=============================================================================
// Get hits for Trackers: OT and ST
// If verbose print some details
//=============================================================================
void GiGaGetHitsAlg::hitsTracker(const std::string det, 
                                 const std::string location)
{
  if( !location.empty() ) {
    MCHits* obj = get<MCHits>( location );
    std::string dethits = det+" MCHits";
    info() << "Number of extracted MCHits      '"
           << location << "'  \t"
           << obj->size()
           << endmsg;
    Stat stat( chronoSvc(), "#"+dethits, obj->size() );
    if( msgLevel( MSG::VERBOSE ) ) {
      for( MCHits::const_iterator hiter=obj->begin(); hiter!=obj->end();
           ++hiter ) {
        verbose() << "Energy of MCHit: "
                  << (*hiter)->energy()
                  << "     MCParticle: " 
                  << (*hiter)->mcParticle()->particleID().pid()
                  << "     Energy: "
                  << (*hiter)->mcParticle()-> momentum().e()
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
    MCRichHits* obj = get<MCRichHits>( m_richhits );
    info() << "Number of extracted MCRichHits  '"
           << m_richhits << "' \t"
           << obj->size()
           << endmsg ;
    unsigned int nHitsRich1 = 0, nHitsRich2 = 0;
    for( MCRichHits::const_iterator hiter=obj->begin(); hiter!=obj->end();
         ++hiter ) {
      if( (*hiter)->rich() == Rich::Rich1 ) nHitsRich1++;
      if( (*hiter)->rich() == Rich::Rich2 ) nHitsRich2++;
      verbose() << "Rich: " << (*hiter)->rich() 
                << " Radiator: " << (*hiter)->radiator()
                << "  Entry point: " << (*hiter)->entry();
      if( (*hiter)->mcParticle() != NULL ) {
        verbose() << "   MCParticle: "
                  << (*hiter)->mcParticle()->particleID().pid()
                  << "     Energy: " 
                  << (*hiter)->mcParticle()-> momentum().e()
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
    MCRichOpticalPhotons* obj = get<MCRichOpticalPhotons>( m_richop );
    info() << "Number of extracted MCRichOpticalPhotons '"
            << m_richop << "' \t"
            << obj->size()
            << endmsg;
    Stat stat( chronoSvc() , "#MCRichOpPhotons" , obj->size() );
  }


  // Rich segments
  if( !m_richsegments.empty() ) {
    MCRichSegments* obj = get<MCRichSegments>( m_richsegments );
    info() << "Number of extracted MCRichSegments '"
           << m_richsegments << "' \t"
           << obj->size()
           << endmsg;
    Stat stat( chronoSvc(), "#MCRichSegments", obj->size() );
  }
  

  // Rich tracks
  if( !m_richtracks.empty() ) {
    MCRichTracks* obj = get<MCRichTracks>( m_richtracks );
    info() << MSG::INFO
           << "Number of extracted MCRichTracks '"
           << m_richtracks << "' \t"
           << obj->size()
           << endmsg;
    Stat stat( chronoSvc(), "#MCRichTracks", obj->size() );
  }

  
  return;
}

//=============================================================================
// Get hits for MUON system:
// If verbose print some details
//=============================================================================
void GiGaGetHitsAlg::infoMuon() {

  // MCMuonHits
  if( m_muonhits.empty() ) {
    return;
  }
  
  unsigned int nTotMuonHits = 0;
  unsigned int nM1Hits = 0, nM2Hits = 0, nM3Hits = 0, nM4Hits = 0, nM5Hits = 0;
  for ( int i=0; i<20; ++i) {
    MCMuonHits* obj = get<MCMuonHits>( MCMuonHitPath::MCMuonHitPath[i] );
    nTotMuonHits += obj->size();
    if( msgLevel( MSG::DEBUG ) ) {
      if( (MCMuonHitPath::MCMuonHitPath[i]).find("M1") != std::string::npos ) {
        nM1Hits += obj->size();
      }
      if( (MCMuonHitPath::MCMuonHitPath[i]).find("M2") != std::string::npos ) {
        nM2Hits += obj->size();
      }
      if( (MCMuonHitPath::MCMuonHitPath[i]).find("M3") != std::string::npos ) {
        nM3Hits += obj->size();
      }
      if( (MCMuonHitPath::MCMuonHitPath[i]).find("M4") != std::string::npos ) {
        nM4Hits += obj->size();
      }
      if( (MCMuonHitPath::MCMuonHitPath[i]).find("M5") != std::string::npos ) {
        nM5Hits += obj->size();
      }
      debug() << "Number of extracted MCMuonHits  '"
              << MCMuonHitPath::MCMuonHitPath[i] << "'  \t"
              << obj->size() 
              << endmsg;
    }
    unsigned int chamLimit = 0;
    if( (MCMuonHitPath::MCMuonHitPath[i]).find("R1") != std::string::npos ) {
      chamLimit = 12;
    }
    if( (MCMuonHitPath::MCMuonHitPath[i]).find("R2") != std::string::npos ) {
      chamLimit = 24;
    }
    if( (MCMuonHitPath::MCMuonHitPath[i]).find("R3") != std::string::npos ) {
      chamLimit = 48;
    }
    if( (MCMuonHitPath::MCMuonHitPath[i]).find("R4") != std::string::npos ) {
      chamLimit = 192;  
    }
    //    if( msgLevel( MSG::VERBOSE ) ) {
    for( MCMuonHits::const_iterator hiter=obj->begin(); hiter!=obj->end();
         ++hiter ) {
      if( (*hiter)->chamberID() > chamLimit ) {
        warning() << "Chamber out of limits in " 
                  << MCMuonHitPath::MCMuonHitPath[i]
                  << endmsg;
        
        warning() << "Chamber: " << (*hiter)->chamberID() 
                  << " Gap: " << (*hiter)->gapID()
                  << " Entry point: " << (*hiter)->entry()
                  << " Exit point: "  << (*hiter)->exit()
                  << endmsg;  
      }
    }
  }
  info() << "Number of extracted MCMuonHits  (total)       \t"
         << nTotMuonHits
         << endmsg;

  Stat stat( chronoSvc(), "#Muon MCHits", nTotMuonHits );
  if( msgLevel( MSG::DEBUG ) ) {
    Stat statM1( chronoSvc(), "#M1 MCHits", nM1Hits );
    Stat statM2( chronoSvc(), "#M2 MCHits", nM2Hits );
    Stat statM3( chronoSvc(), "#M3 MCHits", nM3Hits );
    Stat statM4( chronoSvc(), "#M4 MCHits", nM4Hits );
    Stat statM5( chronoSvc(), "#M5 MCHits", nM5Hits );
  }
  
  return;
  
}


//=============================================================================
    
