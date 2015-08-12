// Include files 

// local
#include "Event/Track.h"
#include "Event/StateParameters.h"
#include "PrCheatedSciFiTracking.h"
#include "Event/FTCluster.h"
#include "Linker/LinkedTo.h"
#include "Event/MCTrackInfo.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PrCheatedSciFiTracking
//
// 2015-03-23 : Michel De Cian
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PrCheatedSciFiTracking )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrCheatedSciFiTracking::PrCheatedSciFiTracking( const std::string& name,
                                        ISvcLocator* pSvcLocator)
: 
  GaudiHistoAlg ( name, pSvcLocator),
  m_hitManager(nullptr)
{
  declareProperty( "HitManagerName",      m_hitManagerName       = "PrFTHitManager"             );
  declareProperty( "DecodeData",          m_decodeData           = false                        );
  declareProperty( "OutputName",          m_outputName           = LHCb::TrackLocation::Seed    );
  declareProperty( "NumZones",            m_numZones             = 24                           );
  declareProperty( "MinXHits",            m_minXHits             = 5                            );
  declareProperty( "MinStereoHits",       m_minStereoHits        = 5                            );
  declareProperty( "MinTotHits",          m_minTotHits           = 10                           );
  
   
}
//=============================================================================
// Destructor
//=============================================================================
PrCheatedSciFiTracking::~PrCheatedSciFiTracking() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode PrCheatedSciFiTracking::initialize() {
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_hitManager = tool<PrHitManager>( m_hitManagerName );
  m_hitManager->buildGeometry();
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PrCheatedSciFiTracking::execute() {
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  
  
  LHCb::Tracks* result = new LHCb::Tracks();
  put( result, m_outputName );

  if( m_decodeData ) m_hitManager->decodeData();
  
  makeLHCbTracks( result );

  

  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PrCheatedSciFiTracking::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiHistoAlg::finalize();  // must be called after all other actions
}


//=========================================================================
//  Make the cheated tracks
//=========================================================================
void PrCheatedSciFiTracking::makeLHCbTracks ( LHCb::Tracks* result ) {

  LinkedTo<LHCb::MCParticle, LHCb::FTCluster> myClusterLink ( evtSvc(), msgSvc(), LHCb::FTClusterLocation::Default );
  LHCb::MCParticles* mcParts = getIfExists<LHCb::MCParticles> ( LHCb::MCParticleLocation::Default );

  MCTrackInfo trackInfo( evtSvc(), msgSvc() );

  for( LHCb::MCParticles::const_iterator iPart = mcParts->begin(); iPart != mcParts->end(); ++iPart){
    
    const LHCb::MCParticle* mcPart = *iPart;

    //const bool isLong = trackInfo.hasVeloAndT( mcPart );
    //const bool isDown = trackInfo.hasT( mcPart ) && trackInfo.hasTT( mcPart );
    const bool isSeed = trackInfo.hasT( mcPart );
    

    //if( !isLong && !isDown ) continue;
    if( !isSeed ) continue;
    
    LHCb::Track* tmp = new LHCb::Track;
    tmp->setType( LHCb::Track::Ttrack );
    tmp->setHistory( LHCb::Track::PrSeeding );
    tmp->setPatRecStatus( LHCb::Track::PatRecIDs );
    
    std::vector<int> firedXLayers(m_numZones,0);
    std::vector<int> firedStereoLayers(m_numZones,0);
    
    int totHits = 0;
    
    // -- loop over all zones
    for(int iZone = 0; iZone < m_numZones; ++iZone){
      
      PrHitZone* zone = m_hitManager->zone( iZone );
      
      // -- loop over all hits in a zone
      for ( PrHits::iterator itH = zone->hits().begin(); zone->hits().end() != itH; ++itH){
        
        PrHit* hit = *itH;
      
        LHCb::MCParticle* mcPart1 = myClusterLink.first( hit->id().ftID() ); 
        bool found = false;
        
        while( mcPart1 != nullptr){
          if( mcPart1 == mcPart){
            found = true;
          }
          mcPart1 = myClusterLink.next();
        }
      
  
        if( hit->isX() && found ){
          if( firedXLayers[iZone] == 0){
            firedXLayers[iZone]++;
          }
        }

        if( !(hit->isX()) && found ){
          if( firedStereoLayers[iZone] == 0){
            firedStereoLayers[iZone]++;
          }
        }

        if( found ){
          totHits++;
          tmp->addToLhcbIDs( hit->id() );
        }
        
      }
    }
    
        
    int sumLowerX = 0;
    int sumUpperX = 0;
    int sumStereo = 0;
    

    for(int i = 0; i < m_numZones; i = i+2){
      sumLowerX += firedXLayers[i];
    }
    
    for(int i = 1; i < m_numZones; i = i+2){
      sumUpperX += firedXLayers[i];
    }

    for(int i = 0; i < m_numZones; i++){
      sumStereo += firedStereoLayers[i];
    }
    

    debug() << "sumLowerX: " << sumLowerX 
            << " sumUpperX " << sumUpperX 
            << " sumStereo " << sumStereo 
            << " totHits "   << totHits << endmsg;
    
    if( (sumLowerX < m_minXHits && sumUpperX < m_minXHits) || sumStereo < m_minStereoHits || totHits < m_minTotHits){
      delete tmp;
      continue;
    }
    
    



    // -- these are obviously useless numbers, but it should just make the checkers happy
    double qOverP = 0.01;

    LHCb::State tState;
    double z = StateParameters::ZEndT;
    tState.setLocation( LHCb::State::AtT );
    tState.setState( 100, 50, z, 0.1, 0.1, qOverP );

    //tState.setCovariance( m_geoTool->covariance( qOverP ) );
    tmp->addToStates( tState );
    
    result->insert( tmp );
    
    
  }
  
  
  

 

}



