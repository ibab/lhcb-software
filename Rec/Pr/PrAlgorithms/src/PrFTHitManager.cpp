// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
#include "Event/FTRawCluster.h"

// local
#include "PrFTHitManager.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PrFTHitManager
//
// 2012-03-13 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( PrFTHitManager )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrFTHitManager::PrFTHitManager( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : PrHitManager ( type, name , parent )
{
  declareInterface<PrFTHitManager>(this);

}
//=============================================================================
// Destructor
//=============================================================================
PrFTHitManager::~PrFTHitManager() {}

//=========================================================================
//  Create the zones in the hit manager, 'zone' is a method of the base class
//=========================================================================
void PrFTHitManager::buildGeometry ( ) {
  m_ftDet = getDet<DeFTDetector>( DeFTDetectorLocation::Default );
  for ( std::vector<DeFTLayer*>::const_iterator itL = m_ftDet->layers().begin();
        m_ftDet->layers().end() != itL; ++itL ) {
    int id = (*itL)->layerID();
    LHCb::FTChannelID tmp( id, 0, 0, 0 );
    DetectorSegment seg = (*itL)->createDetSegment( tmp, 0. );
    zone( 2*id   )->setGeometry( seg );
    zone( 2*id+1 )->setGeometry( seg );
    zone( 2*id   )->setBoundaries( -4090., 4090., -3030., 50. );
    zone( 2*id+1 )->setBoundaries( -4090., 4090., -50., 3030. );    
    debug() << "Zone " << id << " z " << zone(2*id)->z() << " angle " << zone(2*id)->dxDy() << endmsg;
  }
}

//=========================================================================
//  Decode an event.
//=========================================================================
void PrFTHitManager::decodeData ( ) {
  
  typedef FastClusterContainer<LHCb::FTRawCluster,int> FTRawClusters;
  FTRawClusters* clus = get<FTRawClusters>( LHCb::FTRawClusterLocation::Default );
  DeFTLayer* myLayer = NULL;
  unsigned int oldLayer = 1000;
  debug() << "Retrieved " << clus->size() << " clusters" << endmsg;
  
  for ( FTRawClusters::iterator itC = clus->begin(); clus->end() != itC; ++itC ) {
    if( (*itC).channelID().layer() != oldLayer ) {
      oldLayer =(*itC).channelID().layer();
      myLayer = m_ftDet->layers()[oldLayer];
      if ( NULL == myLayer ) {
        info() << "Layer not found for FT channelID " << (*itC).channelID() << endmsg;    
        oldLayer = 1000;
      }
    }
    float fraction = (*itC).fraction() + 0.125;   // Truncated to 4 bits = 0.25. Add half of it
    LHCb::FTChannelID id = (*itC).channelID();
    DetectorSegment seg = myLayer->createDetSegment( id, fraction );

    int lay  = (*itC).channelID().layer();
    int zone = ( (*itC).channelID().quarter() > 1 ) ? 1 : 0;
    int code = 2*lay + zone;
    PrHit* aHit = newHitInZone( code );
    float errX = 0.05 + .03 * (*itC).size();
    aHit->setHit( LHCb::LHCbID( (*itC).channelID() ), (*itC).size(), (*itC).charge(), seg, errX , zone, lay );
    debug() << " .. hit " << (*itC).channelID() << " zone " << zone << " x " << seg.x(0.) << endmsg;
  }
  /*
  //== Remove a bilayer.
  zone(0 ) ->hits().clear();
  zone(1  )->hits().clear();
  zone(2  )->hits().clear();
  zone(3  )->hits().clear();
  */
  for ( unsigned int lay = 0; nbZones() > lay ; ++lay ) {
    std::sort( zone(lay)->hits().begin(), zone(lay)->hits().end(), PrHit::LowerByCoord() );
  }
}
//=============================================================================
