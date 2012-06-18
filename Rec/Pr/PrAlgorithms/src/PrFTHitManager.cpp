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
//  Create the layers in the hit manager, 'layer' is a method of the base class
//=========================================================================
void PrFTHitManager::buildGeometry ( ) {
  m_ftDet = getDet<DeFTDetector>( DeFTDetectorLocation::Default );
  for ( std::vector<DeFTLayer*>::const_iterator itL = m_ftDet->layers().begin();
        m_ftDet->layers().end() != itL; ++itL ) {
    int id = (*itL)->layerID();
    LHCb::FTChannelID tmp( id, 0, 0, 0 );
    DetectorSegment seg = (*itL)->createDetSegment( tmp, 0. );
    layer( 2*id   )->setGeometry( seg );
    layer( 2*id+1 )->setGeometry( seg );
    layer( 2*id   )->setBoundaries( -4090., 4090., -3030., 50. );
    layer( 2*id+1 )->setBoundaries( -4090., 4090., -50., 3030. );    
    debug() << "Layer " << id << " z " << layer(2*id)->z() << " angle " << layer(2*id)->dxDy() << endmsg;
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
    float fraction = (*itC).fraction();
    LHCb::FTChannelID id = (*itC).channelID();
    DetectorSegment seg = myLayer->createDetSegment( id, fraction );

    int lay  = (*itC).channelID().layer();
    int zone = ( (*itC).channelID().quarter() > 1 ) ? 1 : 0;
    int code = 2*lay + zone;
    PrHit* aHit = newHitInLayer( code );
    float errX = 0.1;
    if ( 4 < (*itC).size() ) errX = 0.2;
    aHit->setHit( LHCb::LHCbID( (*itC).channelID() ), (*itC).size(), (*itC).charge(), seg, errX , zone, lay );
    debug() << " .. hit " << (*itC).channelID() << " zone " << zone << " x " << seg.x(0.) << endmsg;
  }
  for ( unsigned int lay = 0; nbLayers() > lay ; ++lay ) {
    std::sort( layer(lay)->hits().begin(), layer(lay)->hits().end(), PrHit::LowerByCoord() );
  }
}
//=============================================================================
