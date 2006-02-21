// $Id: BuildMCTrackInfo.cpp,v 1.1 2006-02-21 17:17:18 ocallot Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

#include "Event/MCParticle.h"
#include "Event/MCHit.h"
#include "Event/VeloCluster.h"
#include "Event/STCluster.h"
#include "Event/OTTime.h"

#include "Linker/LinkedTo.h"
#include "Event/MCProperty.h"
// Det
#include "OTDet/DeOTDetector.h"

// local
#include "BuildMCTrackInfo.h"

//-----------------------------------------------------------------------------
// Implementation file for class : BuildMCTrackInfo
//
// 2004-01-08 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<BuildMCTrackInfo>          s_factory ;
const        IAlgFactory& BuildMCTrackInfoFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BuildMCTrackInfo::BuildMCTrackInfo( const std::string& name,
                                            ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{

}
//=============================================================================
// Destructor
//=============================================================================
BuildMCTrackInfo::~BuildMCTrackInfo() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode BuildMCTrackInfo::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  m_velo = getDet<DeVelo>( "/dd/Structure/LHCb/BeforeMagnetRegion/Velo" );
  

  m_ttDet = getDet<DeSTDetector>(DeSTDetLocation::TT );
  info() << "Number TT layer " << m_ttDet->layers().size() << endreq;
  
  m_itDet = getDet<DeSTDetector>(DeSTDetLocation::IT );
  info() << "Number IT layer " << m_itDet->layers().size() << endreq;  

  DeOTDetector* otDet = getDet<DeOTDetector>(DeOTDetectorLocation::Default );


  m_OT1Station = otDet->station( 1 );
  m_OT2Station = otDet->station( 2 );
  m_OT3Station = otDet->station( 3 );

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode BuildMCTrackInfo::execute() {
  debug() << "==> Execute" << endreq;
  
  //LinkedTo<LHCb::MCParticle> veloLink( eventSvc(), msgSvc(), 
  //                                     LHCb::VeloClusterLocation::VeloClusters );
  
  LinkedTo<LHCb::MCParticle> ttLink( eventSvc(), msgSvc(), 
                                     LHCb::STClusterLocation::TTClusters );
  
  LinkedTo<LHCb::MCParticle> itLink( eventSvc(), msgSvc(), 
                                     LHCb::STClusterLocation::ITClusters );
  
  LinkedTo<LHCb::MCParticle> otLink( eventSvc(), msgSvc(), 
                                     LHCb::OTTimeLocation::Default );
  
  LHCb::MCProperty* trackInfo = new LHCb::MCProperty();
  put( trackInfo, LHCb::MCPropertyLocation::TrackInfo );  
  
  LHCb::MCParticles* mcParts = get<LHCb::MCParticles>(LHCb::MCParticleLocation::Default);

  //== The array size is bigger than MCParticle.size() as the MCParticles are
  //== compressed, uninteresting particles were removed at the end of Brunel.
  int highestKey = mcParts->size();
  for ( LHCb::MCParticles::const_iterator itP = mcParts->begin();
        mcParts->end() != itP; itP++ ) {
    if ( highestKey < (*itP)->key() ) highestKey = (*itP)->key();
  }
  unsigned int nbMcPart = highestKey;

  debug() << "Highest MCParticle number " << nbMcPart << endreq;

  std::vector<int> veloR   ( nbMcPart, 0 );
  std::vector<int> veloPhi ( nbMcPart, 0 );
  std::vector<int> station ( nbMcPart, 0 );
  
  LHCb::MCParticle* part;
  unsigned int MCNum;
  /*
  //== particle->cluster Velo links
  LHCb::VeloClusters* veloClus = get<LHCb::VeloClusters>( LHCb::VeloClusterLocation::VeloClusters);
  for ( LHCb::VeloClusters::const_iterator cluIt = veloClus->begin() ;
        veloClus->end() != cluIt ; cluIt++ ) {
    int myKey = (*cluIt)->channelID();
    part = veloLink.first( myKey );
    while ( NULL != part ) {
      if ( mcParts == part->parent() ) {
        MCNum = part->key();
        if ( veloR.size() > MCNum ) {
          if ( m_velo->isRSensor( (*cluIt)->channelID().sensor() ) ) {
            veloR[MCNum]++;
          } else {
            veloPhi[MCNum]++;
          }
        }
      }
      part = veloLink.next() ;
    }
  }
  */

  //== IT cluster -> particle associaton
  LHCb::STClusters* ITDig = get<LHCb::STClusters>( LHCb::STClusterLocation::TTClusters);
  for ( LHCb::STClusters::const_iterator digIt = ITDig->begin() ;
        ITDig->end() != digIt ; digIt++ ) {
    int sta   = (*digIt)->channelID().station() - 1;
    int lay   = ((*digIt)->channelID().layer() - 1)%4;
    bool isX  = (0==lay) || (3==lay);
    part = itLink.first( (*digIt)->channelID() );
    while ( NULL != part ) {
      if ( mcParts == part->parent() ) {
        MCNum = part->key();
        if ( station.size() > MCNum ) {
          updateBit( station[MCNum], sta, isX );
        }
      }
      part = itLink.next() ;
    }
  }
  
  //== OT Times -> particle associaton
  LHCb::OTTimes* OTTim = get<LHCb::OTTimes>( LHCb::OTTimeLocation::Default);
  for ( LHCb::OTTimes::const_iterator timIt = OTTim->begin() ;
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
    if ( 2 < veloR[MCNum] )    mask |= MCTrackInfoBits::maskVeloR;
    if ( 2 < veloPhi[MCNum] )  mask |= MCTrackInfoBits::maskVeloPhi;
    mask |= (veloR[MCNum]  <<MCTrackInfoBits::multVeloR );
    mask |= (veloPhi[MCNum]<<MCTrackInfoBits::multVeloPhi );
    

    if ( 0 != mask ) {
      trackInfo->setProperty( *itP, mask );
      debug() << format( "Track %4d mask %8x", MCNum, mask ) << endreq;
    }
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode BuildMCTrackInfo::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}


//=========================================================================
//  Process the MC(Velo)Hits to get the 'acceptance'
//=========================================================================
void BuildMCTrackInfo::computeAcceptance ( std::vector<int>& station ) {

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
    
    Gaudi::XYZPoint midPoint = (*vHit)->midPoint();
    int staNr = m_velo->sensorNumber( midPoint );   
    if ( m_velo->isRSensor( staNr ) ) {
      nVeloR[MCNum]++;
    } else {
      nVeloP[MCNum]++;
    }
  }
  for ( unsigned int MCNum = 0; station.size() > MCNum; MCNum++ ){
    if ( 2 < nVeloR[MCNum] ) station[MCNum] |= MCTrackInfoBits::maskAccVeloR;
    if ( 2 < nVeloP[MCNum] ) station[MCNum] |= MCTrackInfoBits::maskAccVeloPhi;
  }
  //== TT
  
  LHCb::MCHits* ttHits = get<LHCb::MCHits>( LHCb::MCHitLocation::TT );
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
      debug() << format( "TT Hit not in any LAYER ! x %8.2f y%8.2f z%9.2f",
                         midPoint.x(), midPoint.y(), midPoint.z() ) << endreq;
      continue;
    }
    int  sta = ttLay->elementID().station() -1;
    bool isX = ttLay->angle() == 0;
    updateAccBit( station[MCNum], sta, isX );

    debug() << format( "TT Hit x %8.2f y%8.2f z%9.2f Sta %1d Lay %1d ang %5.1f",
                       midPoint.x(), midPoint.y(), midPoint.z(), 
                       ttLay->elementID().station(),
                       ttLay->elementID().layer(),
                       ttLay->angle()/degree ) << endreq;
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
      debug() << format( "IT Hit not in any LAYER ! x %8.2f y%8.2f z%9.2f",
                         midPoint.x(), midPoint.y(), midPoint.z() ) << endreq;
      continue;
    }
    
    debug() << format( "IT Hit x %8.2f y%8.2f z%9.2f Sta %1d Lay %1d ang %5.1f",
                       midPoint.x(), midPoint.y(), midPoint.z(), 
                       itLay->elementID().station(),
                       itLay->elementID().layer(),
                       itLay->angle()/degree ) << endreq;

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
    
    if (        m_OT1Station->layer( midPoint ) ) {  // OT1 MCHit?
      bool isX = m_OT1Station->layer( midPoint )->stereoAngle() == 0;
      updateAccBit( station[MCNum], 2, isX );

    } else if ( m_OT2Station->layer( midPoint ) ) {  // OT2 MCHit?
      bool isX = m_OT2Station->layer( midPoint )->stereoAngle() == 0;
      updateAccBit( station[MCNum], 3, isX );

    } else if ( m_OT3Station->layer( midPoint ) ) {  // OT3 MCHit?
      bool isX = m_OT3Station->layer( midPoint )->stereoAngle() == 0;
      updateAccBit( station[MCNum], 4, isX );

    }
  }


}
//=============================================================================
