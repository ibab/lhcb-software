// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
#include "Event/FTRawCluster.h"
#include "GaudiKernel/IRndmGenSvc.h"
// local
#include "PrFTHitManager.h"

//#include  "Kernel/FTChannelID.h"

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
  declareProperty( "XSmearing",   m_xSmearing = -1. );
  declareProperty( "ZSmearing",   m_zSmearing = -1. );
}
//=============================================================================
// Destructor
//=============================================================================
PrFTHitManager::~PrFTHitManager() {}

//=========================================================================
//  Create the zones in the hit manager, 'zone' is a method of the base class
//=========================================================================
void PrFTHitManager::buildGeometry ( ) {  
  IRndmGenSvc* randSvc;
  service( "RndmGenSvc", randSvc );
  m_gauss.initialize( randSvc, Rndm::Gauss( 0., 1. ) );
  m_ftDet = getDet<DeFTDetector>( DeFTDetectorLocation::Default );
  debug() << "DETECTOR VERSION: " << m_ftDet->version() << endmsg;  //version 20 is the detector with monolayer and fibremat structure, including the dead regions
  DetectorSegment seg;

  for ( std::vector<DeFTLayer*>::const_iterator itL = m_ftDet->layers().begin();  //loop over layers
	m_ftDet->layers().end() != itL; ++itL ) {
    int id = (*itL)->params()->param<int>("layerID");  //ask layerID
    LHCb::FTChannelID tmp( id, 0, 0, 0 ); 
    const DeFTFibreMat* ftMat = m_ftDet->findFibreMat(tmp);
    seg = ftMat->createDetSegment( tmp, 0. ); 
    debug() << "STEREO ANGLE: " <<  ftMat->angle() << endmsg;
    
    zone( 2*id   )->setGeometry( seg ); // ex:layers 0  down
    zone( 2*id+1 )->setGeometry( seg ); // ex:layers 0  up 
    zone( 2*id   )->setBoundaries( -4090., 4090., -3030., 50. ); //check this boudaries values for zone down 
    zone( 2*id+1 )->setBoundaries( -4090., 4090., -50., 3030. ); //check this boudaries values for zone up   
    debug() << "Layer " << id << " z " << zone(2*id)->z() << " angle " << zone(2*id)->dxDy() << endmsg;
  }
  debug() << "XSmearing " << m_xSmearing << " ZSmearing " << m_zSmearing << endmsg;
}


//=========================================================================
//  Decode an event.
//=========================================================================
void PrFTHitManager::decodeData ( ) {
  
  debug() << "I AM IN DECODEDATA " << endmsg;
  
  typedef FastClusterContainer<LHCb::FTRawCluster,int> FTRawClusters;
  FTRawClusters* clus = get<FTRawClusters>( LHCb::FTRawClusterLocation::Default );
  debug() << "Retrieved " << clus->size() << " clusters" << endmsg;
  const DeFTFibreMat* ftMat = nullptr;
  const DeFTFibreMat* anaFtMat = nullptr;
  unsigned int oldFibreMatID = 99999999;
    
  float zDisplacement = 0.;
  
  DetectorSegment seg ; 
  
  for ( FTRawClusters::iterator itC = clus->begin(); clus->end() != itC; ++itC ) {
    //find fibremat to which the cluster belongs 
    anaFtMat = m_ftDet->findFibreMat( (*itC).channelID() );
    if(anaFtMat->FibreMatID() != oldFibreMatID)  { 
      oldFibreMatID =  anaFtMat->FibreMatID();
      ftMat =  anaFtMat; 
      if ( nullptr == ftMat ) {
	info() << "FiberMat not found for FT channelID " << (*itC).channelID() << endmsg; 
	oldFibreMatID = 99999999;   
      } 
    }
    
    float fraction = (*itC).fraction() + 0.125;   // Truncated to 4 bits = 0.25. Add half of it
    LHCb::FTChannelID id = (*itC).channelID();
    seg = ftMat->createDetSegment( id, fraction );   //create detector segment  
	
    
    //== Add some smearing if needed
    if ( m_xSmearing > 0. || m_zSmearing > 0. ) {
      float x0 = seg.x(0.);
      float z0 = seg.z(0.);
      float dxDy = 0.001 * ( seg.x(1000.)-seg.x(0.) );
      float dzDy = 0.001 * ( seg.z(1000.)-seg.z(0.) );
      float yMin = seg.yMin();
      float yMax = seg.yMax();
      
      if ( m_xSmearing > 0. ) x0 = x0 + m_xSmearing * m_gauss();
      if ( m_zSmearing > 0. ) z0 = z0 + zDisplacement;
      DetectorSegment tmp( x0, z0, dxDy, dzDy, yMin, yMax );
      seg = tmp;
    }


    int lay  = (*itC).channelID().layer();  
    int zone = ( (*itC).channelID().quarter() > 1 ) ? 1 : 0; 
    int code = 2*lay + zone;
    PrHit* aHit = newHitInZone( code );
    float errX = 0.05 + .03 * (*itC).size();
    aHit->setHit( LHCb::LHCbID( (*itC).channelID() ), (*itC).size(), (*itC).charge(), seg, errX , zone, lay );
    debug() << " .. hit " << (*itC).channelID() << " zone " << zone << " x " << seg.x(0.) << endmsg;
  }


  for ( unsigned int lay = 0; nbZones() > lay ; ++lay ) {
    std::sort( zone(lay)->hits().begin(), zone(lay)->hits().end(), PrHit::LowerByCoord() );
  }
  
}
//=============================================================================
