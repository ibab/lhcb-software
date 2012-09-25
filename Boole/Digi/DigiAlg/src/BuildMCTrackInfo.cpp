// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

#include "Event/MCParticle.h"
#include "Event/MCHit.h"
#include "Event/VeloCluster.h"
#include "Event/STCluster.h"
#include "Event/OTTime.h" /// Needed for path of table
#include "Event/MCOTTime.h"

#include "Linker/LinkedTo.h"
#include "Event/MCProperty.h"

// Det
#include "VeloDet/DeVelo.h"
#include "STDet/DeSTDetector.h"
#include "OTDet/DeOTStation.h"
#include "OTDet/DeOTDetector.h"
#include "OTDet/DeOTLayer.h"

// local
#include "BuildMCTrackInfo.h"

//-----------------------------------------------------------------------------
// Implementation file for class : BuildMCTrackInfo
//
// 2004-01-08 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( BuildMCTrackInfo )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BuildMCTrackInfo::BuildMCTrackInfo( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_velo(0)
  , m_ttDet(0)
  , m_itDet(0)
  , m_otDet(0)
{
  declareProperty( "UTForTT",     m_utForTT = false );
}
//=============================================================================
// Destructor
//=============================================================================
BuildMCTrackInfo::~BuildMCTrackInfo() {}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode BuildMCTrackInfo::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if(msgLevel(MSG::DEBUG)) debug() << "==> Initialize" << endmsg;

  m_velo  = getDet<DeVelo>( DeVeloLocation::Default );

  if ( m_utForTT ) m_ttLocation = DeSTDetLocation::UT;
  else             m_ttLocation = DeSTDetLocation::TT;
  m_ttDet = getDet<DeSTDetector>(m_ttLocation.c_str());

  m_itDet = getDet<DeSTDetector>(DeSTDetLocation::IT );
  m_otDet = getDet<DeOTDetector>(DeOTDetectorLocation::Default );

  if(msgLevel(MSG::DEBUG)) {
    if ( m_utForTT )
      debug() << "Number of UT layers " << m_ttDet->layers().size() << endmsg;
    else
      debug() << "Number of TT layers " << m_ttDet->layers().size() << endmsg;
    debug() << "Number of IT layers " << m_itDet->layers().size() << endmsg;
    debug() << "Number of OT layers " << m_otDet->layers().size() << endmsg;
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode BuildMCTrackInfo::execute() {
  const bool isDebug   = msgLevel(MSG::DEBUG);
  const bool isVerbose = msgLevel(MSG::VERBOSE);

  if(isDebug) debug() << "==> Execute" << endmsg;
  
  LinkedTo<LHCb::MCParticle, LHCb::VeloCluster> veloLink( eventSvc(), msgSvc(), 
                                       LHCb::VeloClusterLocation::Default );
  if( veloLink.notFound() ) return StatusCode::FAILURE;  
  
  if ( m_utForTT ) m_ttLocation = LHCb::STClusterLocation::UTClusters;
  else             m_ttLocation = LHCb::STClusterLocation::TTClusters;
  LinkedTo<LHCb::MCParticle, LHCb::STCluster> ttLink( eventSvc(), msgSvc(), m_ttLocation.c_str());

  if( ttLink.notFound() ) return StatusCode::FAILURE;
  
  LinkedTo<LHCb::MCParticle, LHCb::STCluster> itLink( eventSvc(), msgSvc(), 
                                     LHCb::STClusterLocation::ITClusters );
  if( itLink.notFound() ) return StatusCode::FAILURE;
  
  LinkedTo<LHCb::MCParticle> otLink( eventSvc(), msgSvc(), LHCb::OTTimeLocation::Default );
  if( otLink.notFound() ) return StatusCode::FAILURE;

  LHCb::MCParticles* mcParts = get<LHCb::MCParticles>(LHCb::MCParticleLocation::Default);

  LHCb::MCProperty* trackInfo = new LHCb::MCProperty();
  put( trackInfo, LHCb::MCPropertyLocation::TrackInfo );  

  //== The array size is bigger than MCParticle.size() as the MCParticles are
  //== compressed, uninteresting particles were removed at the end of Brunel.
  int highestKey = mcParts->size();
  for ( LHCb::MCParticles::const_iterator itP = mcParts->begin();
        mcParts->end() != itP; itP++ ) {
    if ( highestKey < (*itP)->key() ) highestKey = (*itP)->key();
  }
  unsigned int nbMcPart = highestKey;

  if(isDebug) debug() << "Highest MCParticle number " << nbMcPart << endmsg;

  std::vector<int> veloR   ( nbMcPart+1, 0 );
  std::vector<int> lastVelo ( nbMcPart+1, -1 );
  std::vector<int> veloPhi ( nbMcPart+1, 0 );
  std::vector<int> station ( nbMcPart+1, 0 );
  
  LHCb::MCParticle* part;
  unsigned int MCNum;
  
  //== particle-> VeloDigit links
  LHCb::VeloClusters* veloClus = get<LHCb::VeloClusters>( LHCb::VeloClusterLocation::Default);
  
  for ( LHCb::VeloClusters::const_iterator vIt = veloClus->begin() ;
        veloClus->end() != vIt ; vIt++ ) {
    int sensor = (*vIt)->channelID().sensor();
    const DeVeloSensor* sens=m_velo->sensor(sensor);
    part = veloLink.first( *vIt );
    while ( NULL != part ) {
      if ( mcParts == part->parent() ) {
        MCNum = part->key();
        if ( veloR.size() > MCNum ) {
          if ( sensor != lastVelo[MCNum] ) {  // Count only once per sensor a given MCParticle
            lastVelo[MCNum] = sensor;
            if ( sens->isR() ) {
              veloR[MCNum]++;
              if(isVerbose)
                verbose() << "MC " << MCNum << " Velo R sensor " << sensor << " nbR " << veloR[MCNum] << endmsg;
            } else if ( sens->isPhi() ) {
              veloPhi[MCNum]++;
              if(isVerbose)
                verbose() << "MC " << MCNum << " Velo Phi sensor " << sensor << " nbPhi " << veloPhi[MCNum] << endmsg;
            }
          }
        }
      }
      part = veloLink.next() ;
    }
  }

  //== TT cluster -> particle associaton
  if ( m_utForTT ) m_ttLocation = LHCb::STClusterLocation::UTClusters;
  else             m_ttLocation = LHCb::STClusterLocation::TTClusters;
  LHCb::STClusters* TTDig = get<LHCb::STClusters>( m_ttLocation.c_str() );
  for ( LHCb::STClusters::const_iterator tIt = TTDig->begin() ;
        TTDig->end() != tIt ; tIt++ ) {
    int sta   = (*tIt)->channelID().station() - 1;  // 0-1 from 1-2
    int lay   = ((*tIt)->channelID().layer() - 1)%4;
    bool isX  = (0==lay) || (3==lay);
    part = ttLink.first( *tIt );
    while ( NULL != part ) {
      if ( mcParts == part->parent() ) {
        MCNum = part->key();
        if ( station.size() > MCNum ) {
          updateBit( station[MCNum], sta, isX );
          if(isVerbose) verbose() << "MC " << MCNum << " TT Sta " << sta << " lay " << lay << endmsg;
        }
      }
      part = ttLink.next() ;
    }
  }
  
  //== IT cluster -> particle associaton
  LHCb::STClusters* ITDig = get<LHCb::STClusters>( LHCb::STClusterLocation::ITClusters);
  for ( LHCb::STClusters::const_iterator iIt = ITDig->begin() ;
        ITDig->end() != iIt ; iIt++ ) {
    int sta   = (*iIt)->channelID().station() + 1; // 2-4 from 1-3
    int lay   = ((*iIt)->channelID().layer() - 1)%4;
    bool isX  = (0==lay) || (3==lay);
    part = itLink.first( *iIt );
    while ( NULL != part ) {
      if ( mcParts == part->parent() ) {
        MCNum = part->key();
        if ( station.size() > MCNum ) {
          updateBit( station[MCNum], sta, isX );
          if(isVerbose) verbose() << "MC " << MCNum << " IT Sta " << sta << " lay " << lay << endmsg;
        }
      }
      part = itLink.next() ;
    }
  }
  
  const LHCb::MCOTTimes* OTTim = get<LHCb::MCOTTimes>(LHCb::MCOTTimeLocation::Default);

  for ( LHCb::MCOTTimes::const_iterator timIt = OTTim->begin() ;
        OTTim->end() != timIt ; timIt++ ) {
    // OT stations should be 2-4 in this numbering, 0,1 are TT stations
    int sta   = (*timIt)->channel().station() + 1;
    int lay   = (*timIt)->channel().layer() ;
    
    bool isX  = (0==lay) || (3==lay);
    part = otLink.first( (*timIt)->channel() );
    while ( NULL != part ) {
      if ( mcParts == part->parent() ) {
        MCNum = part->key();
        if ( station.size() > MCNum ) {
          updateBit( station[MCNum], sta, isX );
          if(isVerbose) verbose() << "MC " << MCNum << " OT Sta " << sta << " lay " << lay << endmsg;
        }
      }
      part = otLink.next() ;
    }
  }
  
  //=====================================================================
  // Now compute the acceptance, with MCHits
  //=====================================================================
  computeAcceptance( station );

  //== Build now the trackInfo tracks

  for ( LHCb::MCParticles::const_iterator itP = mcParts->begin();
        mcParts->end() != itP; itP++ ) {
    MCNum = (*itP)->key();    
    int mask = station[MCNum];    
    if ( 2 < veloR[MCNum] )    mask |= MCTrackInfo::maskVeloR;
    if ( 2 < veloPhi[MCNum] )  mask |= MCTrackInfo::maskVeloPhi;
    if ( 15 < veloR[MCNum]   ) veloR[MCNum]   = 15;
    if ( 15 < veloPhi[MCNum] ) veloPhi[MCNum] = 15;
    
    mask |= (veloR[MCNum]  <<MCTrackInfo::multVeloR );
    mask |= (veloPhi[MCNum]<<MCTrackInfo::multVeloPhi );
    

    if ( 0 != mask ) {
      trackInfo->setProperty( *itP, mask );
      if(isDebug) debug() << format( "Track %4d mask %8x nR %2d nPhi %2d ", 
                                     MCNum, mask, veloR[MCNum], veloPhi[MCNum] );
      if ( MCTrackInfo::maskHasVelo == (mask & MCTrackInfo::maskHasVelo ) )
        if(isDebug) debug() << " hasVelo ";
      if ( MCTrackInfo::maskHasTT   == (mask & MCTrackInfo::maskHasTT ) )
        if(isDebug) debug() << " hasTT ";
      if ( MCTrackInfo::maskHasT    == (mask & MCTrackInfo::maskHasT   ) )
        if(isDebug) debug() << " hasT ";
      if(isDebug) debug() << endmsg;
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode BuildMCTrackInfo::finalize() {

    if(msgLevel(MSG::DEBUG)) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}


//=========================================================================
//  Process the MC(Velo)Hits to get the 'acceptance'
//=========================================================================
void BuildMCTrackInfo::computeAcceptance ( std::vector<int>& station ) {

  const bool isDebug = msgLevel(MSG::DEBUG);
  std::vector<int> nVeloR( station.size(), 0 );
  std::vector<int> nVeloP( station.size(), 0 );
  
  LHCb::MCHits* veloHits = get<LHCb::MCHits>( LHCb::MCHitLocation::Velo);
  for ( LHCb::MCHits::const_iterator vHit = veloHits->begin();
        veloHits->end() != vHit; vHit++ ) {
    unsigned int MCNum = (*vHit)->mcParticle()->key();
    if ( station.size() <= MCNum ) continue;
    
    double deltaZ = fabs( (*vHit)->entry().z() - (*vHit)->exit().z() );
    if( 100. < deltaZ ) {
      warning() << "Large deltaZ for VeloHit entry-exit points: "
                << deltaZ << endmsg;
      continue;
    }
    
    int staNr = (*vHit)->sensDetID();
    const DeVeloSensor* sens=m_velo->sensor(staNr);   
    if ( sens->isR() ) {
      nVeloR[MCNum]++;
    } else {
      nVeloP[MCNum]++;
    }
  }
  for ( unsigned int MCNum = 0; station.size() > MCNum; MCNum++ ){
    if ( 2 < nVeloR[MCNum] ) station[MCNum] |= MCTrackInfo::maskAccVeloR;
    if ( 2 < nVeloP[MCNum] ) station[MCNum] |= MCTrackInfo::maskAccVeloPhi;
  }
  //== TT
  
  if ( m_utForTT ) m_ttLocation = LHCb::MCHitLocation::UT;
  else             m_ttLocation = LHCb::MCHitLocation::TT;
  LHCb::MCHits* ttHits = get<LHCb::MCHits>( m_ttLocation.c_str() );
  for ( LHCb::MCHits::const_iterator tHit = ttHits->begin();
        ttHits->end() != tHit; tHit++ ) {
    unsigned int MCNum = (*tHit)->mcParticle()->key();
    if ( station.size() <= MCNum ) continue;
    
    double deltaZ = fabs( (*tHit)->entry().z() - (*tHit)->exit().z() );
    if( 100. < deltaZ ) {
      warning() << "Large deltaZ for ITHit entry-exit points: "
                << deltaZ << endmsg;
      continue;
    }

    
    Gaudi::XYZPoint midPoint = (*tHit)->midPoint();
    DeSTLayer* ttLay = m_ttDet->findLayer( midPoint );
    if ( 0 == ttLay ) {
      if(isDebug) debug() << format( "TT Hit not in any LAYER ! x %8.2f y%8.2f z%9.2f",
                                     midPoint.x(), midPoint.y(), midPoint.z() ) << endmsg;
      continue;
    }
    int  sta = ttLay->elementID().station() -1;
    bool isX = ttLay->angle() == 0;
    updateAccBit( station[MCNum], sta, isX );
  }

  //== IT
  
  LHCb::MCHits* itHits = get<LHCb::MCHits>( LHCb::MCHitLocation::IT );
  for ( LHCb::MCHits::const_iterator iHit = itHits->begin();
        itHits->end() != iHit; iHit++ ) {
    unsigned int MCNum = (*iHit)->mcParticle()->key();
    if ( station.size() <= MCNum ) continue;
    
    double deltaZ = fabs( (*iHit)->entry().z() - (*iHit)->exit().z() );
    if( 100. < deltaZ ) {
      warning() << "Large deltaZ for ITHit entry-exit points: "
                << deltaZ << endmsg;
      continue;
    }

    
    Gaudi::XYZPoint midPoint = (*iHit)->midPoint();
    DeSTLayer* itLay = m_itDet->findLayer( midPoint );
    if ( 0 == itLay ) {
      if(isDebug) debug() << format( "IT Hit not in any LAYER ! x %8.2f y%8.2f z%9.2f",
                                     midPoint.x(), midPoint.y(), midPoint.z() ) << endmsg;
      continue;
    }
    int  sta = itLay->elementID().station() +1;  // want 2,3,4
    bool isX = itLay->angle() == 0;
    updateAccBit( station[MCNum], sta, isX );    
  }

  //== OT

  LHCb::MCHits* otHits = get<LHCb::MCHits>( LHCb::MCHitLocation::OT );
  for ( LHCb::MCHits::const_iterator oHit = otHits->begin();
        otHits->end() != oHit; oHit++ ) {
    unsigned int MCNum = (*oHit)->mcParticle()->key();
    if ( station.size() <= MCNum ) continue;
    
    double deltaZ = fabs( (*oHit)->entry().z() - (*oHit)->exit().z() );
    if( 100. < deltaZ ) {
      warning() << "Large deltaZ for OTHit entry-exit points: "
                << deltaZ << endmsg;
      continue;
    }

    Gaudi::XYZPoint midPoint = (*oHit)->midPoint();
    DeOTLayer* otLay = m_otDet->findLayer( midPoint );
    if ( 0 == otLay ) {
      if(isDebug) debug() << format( "OT Hit not in any LAYER ! x %8.2f y%8.2f z%9.2f",
                                     midPoint.x(), midPoint.y(), midPoint.z() ) << endmsg;
      continue;
    }
    int sta = otLay->elementID().station() +1;
    bool isX = otLay->angle() == 0;
    updateAccBit( station[MCNum], sta, isX );
  }


}
//=============================================================================
