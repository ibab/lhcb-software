// Include files 

// from Gaudi
#include "Event/FTLiteCluster.h"
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
: PrHitManager ( type, name , parent ),
  m_geometryBuilt(false)
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
  
  // -- only build geometry once.
  if(m_geometryBuilt) return;
  

  IRndmGenSvc* randSvc = svc<IRndmGenSvc>( "RndmGenSvc", true );
  StatusCode sc = m_gauss.initialize( randSvc, Rndm::Gauss( 0., 1. ) );
  if ( sc.isFailure() ){
    error() << "Could not initialize Rndm::Gauss generator" << endmsg;
    return;
  }
  
    
  m_ftDet = getDet<DeFTDetector>( DeFTDetectorLocation::Default );
  //version 20 is the detector with monolayer and fibremat structure, including the dead regions
  if ( msgLevel( MSG::DEBUG) )  debug() << "DETECTOR VERSION: " << m_ftDet->version() << endmsg;  
  //Different Detector Segments has to be used to set the geometry of zone Up and Down to be consistent with new DeFTFibreMat
  DetectorSegment segUp;
  DetectorSegment segDown;
  for ( std::vector<DeFTLayer*>::const_iterator itL = m_ftDet->layers().begin();  //loop over layers
	m_ftDet->layers().end() != itL; ++itL ) {
    int id = (*itL)->params()->param<int>("layerID");  //ask layerID
    
    //FTChannelID (layer,module,mat,SiPMID,netCellID)
    LHCb::FTChannelID ChUp( id, 0u, 0u, 0u, 0u ); //"0u" means "unsigned 0" 
    const DeFTFibreMat* ftMatUp = m_ftDet->findFibreMat(ChUp);
    segUp = ftMatUp->createDetSegment( ChUp, 0. );
    
    LHCb::FTChannelID ChDown( id, 0u, 1u, 0u, 0u ); //"0u" means "unsigned 0"
    const DeFTFibreMat* ftMatDown = m_ftDet->findFibreMat(ChDown);
    segDown = ftMatDown->createDetSegment( ChDown, 0. );

    if ( msgLevel( MSG::DEBUG) ) debug() << "STEREO ANGLE Up: " <<  ftMatUp->angle() << endmsg;
    if ( msgLevel( MSG::DEBUG) ) debug() << "STEREO ANGLE Down: " <<  ftMatDown->angle() << endmsg;
    
    //The setGeometry defines the z at y=0, the dxDy and the dzDy, as well as the isX properties of the zone. 
    //This is important, since these are used in the following. 
    //They are set once for each zone in this method.
    zone( 2*id   )->setGeometry( segUp ); // ex:layers 0  down (up with new FTChannelID)
    zone( 2*id+1 )->setGeometry( segDown ); // ex:layers 0  up (down with new FTChannelID)
    //The boundaries are needed in case you will use the m_xMin, m_xMax, m_yMin, m_yMax of the zone, 
    //or methods that are indirectly using them, like dxOnAFibre() or isInside(x,y) 
    //(see https://svnweb.cern.ch/trac/lhcb/browser/Rec/trunk/Pr/PrKernel/PrKernel/PrHitZone.h?rev=164716). 
    //These are currently not used anywhere in the seeding algorithm.
    //The isInside(x,y) method is used in the forward algorithm. 
    zone( 2*id+1 )->setBoundaries( -4090., 4090., -3030., 50. ); //check these boudaries for zone down (with new FTChannelID)
    zone( 2*id   )->setBoundaries( -4090., 4090., -50., 3030. ); //check these boudaries for zone up (with new FTChannelID) 
    if ( msgLevel( MSG::DEBUG) ) debug() << "Layer " << id << " z " << zone(2*id)->z() 
                                         << " angle " << zone(2*id)->dxDy() << endmsg;
  }
  if ( msgLevel( MSG::DEBUG) ) debug() << "XSmearing " << m_xSmearing << " ZSmearing " << m_zSmearing << endmsg;
  
  m_geometryBuilt = true;
  

}


//=========================================================================
//  Decode an event.
//=========================================================================
void PrFTHitManager::decodeData ( ) {
  
  if ( msgLevel( MSG::DEBUG) ) debug() << "I AM IN DECODEDATA " << endmsg;
  
  typedef FastClusterContainer<LHCb::FTLiteCluster,int> FTLiteClusters;
  FTLiteClusters* clus = get<FTLiteClusters>( LHCb::FTLiteClusterLocation::Default );
  if ( msgLevel( MSG::DEBUG) ) debug() << "Retrieved " << clus->size() << " clusters" << endmsg;
  const DeFTFibreMat* ftMat = nullptr;
  const DeFTFibreMat* anaFtMat = nullptr;
  unsigned int oldFibreMatID = 99999999;
    
  float zDisplacement = 0.;
  
  DetectorSegment seg ; 
  
  for ( FTLiteClusters::iterator itC = clus->begin(); clus->end() != itC; ++itC ) {
    /// find fibremat to which the cluster belongs 
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
    //    int zone = ( (*itC).channelID().quarter() > 1 ) ? 1 : 0;  //if yes, it is top (before bottom was 0)
    int zone = ( (*itC).channelID().mat() > 0 ) ? 1 : 0;  //if yes, it is bottom (now bottom is 1)

    int code = 2*lay + zone;
    PrHit* aHit = newHitInZone( code );
    float errX = 0.05 + .03 * (*itC).size();
    aHit->setHit( LHCb::LHCbID( (*itC).channelID() ), (*itC).size(), (*itC).charge(), seg, errX , zone, lay );
    if ( msgLevel( MSG::DEBUG) ) debug() << " .. hit " << (*itC).channelID() 
                                         << " zone " << zone << " x " << seg.x(0.) << endmsg;
  }


  for ( unsigned int lay = 0; nbZones() > lay ; ++lay ) {
    std::sort( zone(lay)->hits().begin(), zone(lay)->hits().end(), PrHit::LowerByCoord() );
  }
  
}
//=============================================================================
