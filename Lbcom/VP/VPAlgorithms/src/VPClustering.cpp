// $Id: VPClustering.cpp, v1.0 2012-07-06 dhynds

// Include files
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiAlg/GaudiHistoAlg.h"

// local
#include "VPClustering.h"

// VPelDet
#include "VPDet/DeVP.h"
// Event
#include "Event/VPDigit.h"
#include "Event/VPCluster.h"
#include "Event/VPLiteCluster.h"

//using namespace Gaudi::Units;
using namespace LHCb;
using namespace Gaudi;


//-----------------------------------------------------------------------------
// Implementation file for class : VPClustering
//
// 2013-08-13 : Daniel Hynds
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( VPClustering )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VPClustering::VPClustering( const std::string& name,
                        ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator )
{
  declareProperty( "Debug",                   m_debugLevel           = true );
  declareProperty( "DigitContainer",          m_digitLocation        = LHCb::VPDigitLocation::VPDigitLocation );
  declareProperty( "LiteClusterContainer",    m_liteClusterLocation  = LHCb::VPLiteClusterLocation::Default );
  declareProperty( "VPClusterContainer",      m_VPClusterLocation    = LHCb::VPClusterLocation::VPClusterLocation );
  

}

//=============================================================================
// Destructor
//=============================================================================
VPClustering::~VPClustering() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode VPClustering::initialize() {
  StatusCode sc = GaudiHistoAlg::initialize(); 
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiHistoAlg
  if ( m_debugLevel ) debug() << "==> Initialize" << endmsg;
  
  // Set histo dir if debugging
  if ( m_debugLevel ) setHistoTopDir( "VPRecMon/" );

  // Get detector element
  m_vpDet = getDet<DeVP>( DeVPLocation::Default );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode VPClustering::execute() {

  if ( m_debugLevel ) debug() << "==> Execute" << endmsg;

  // Check if LiteClusters already decoded. If not, run clustering on decoded digits
  if ( !exist<LHCb::VPLiteCluster::VPLiteClusters>( LHCb::VPLiteClusterLocation::Default ) ) m_clusterHits = true;
  else m_clusterHits = false;

  // If lite clusters are decoded there is no need to build clusters
  if ( !m_clusterHits ){
    if ( m_debugLevel ) debug() << "Lite clusters already exist. No clustering taking place" << endmsg;
    return StatusCode::SUCCESS;
  }
  return clusterDigits();

}
//=============================================================================
// Loop over track container
//=============================================================================
StatusCode VPClustering::clusterDigits (){

  if ( m_debugLevel ) debug() << "Clustering digits" << endmsg;

  // Pick up pixel hits (digits) to cluster
  if ( !exist<LHCb::VPDigits>( m_digitLocation ) ){
    if ( m_debugLevel ) error() << "No digits founds" << endmsg;
    return StatusCode::FAILURE;
  }  
  LHCb::VPDigits* raw_hits = get<LHCb::VPDigits>(m_digitLocation);
  LHCb::VPDigits::const_iterator iHit;

  // Make space on the TES for the lite clusters and VP clusters
  VPLiteCluster::VPLiteClusters* liteclusterCont = new VPLiteCluster::VPLiteClusters();
  put( liteclusterCont, m_liteClusterLocation );
  
  VPClusters* clusterCont = new VPClusters();
  put( clusterCont, m_VPClusterLocation );

  // Make a temporary storage space for the pixels, and delete when used
  std::vector<LHCb::VPDigit*> digitCont;
  for(iHit=raw_hits->begin();iHit!=raw_hits->end();iHit++){
    digitCont.push_back(*iHit);
  }
  
  // Sort pixels by channelID
  std::sort(digitCont.begin(), digitCont.end(),less_than_channelID());

  
  int newsize=0;
  int oldsize=0;
  
  // Cluster while there are still pixels in the temporary storage
  while(digitCont.size() > 0){
    

    std::vector<LHCb::VPDigit*> cluster;                          //create new cluster 
    std::vector< std::pair<LHCb::VPChannelID,int> > totVec;      
    cluster.push_back(digitCont[0]);                              //push back first hit in container 
    totVec.push_back(std::make_pair(digitCont[0]->channelID(),digitCont[0]->ToTValue()));
    digitCont.erase(digitCont.begin());                           //remove hit from container              
    

    // Add hits to cluster until it no longer changes size
    do {
      oldsize=cluster.size();
      for(unsigned int i=0;i<cluster.size();i++){
        if(digitCont.size() == 0) break; // No more hits in temp. storage
        
        if(   cluster[i]->channelID().station() != digitCont[0]->channelID().station()     
           || cluster[i]->channelID().module()  != digitCont[0]->channelID().module()  ) continue;  // Next hit not on same station (move this)
        
        if(    cluster[i]->channelID().col() != 255 
            && cluster[i]->channelID().col() != 0
            && cluster[i]->channelID().chip()     != digitCont[0]->channelID().chip() ) continue;  // Not edge and next hit not on same chip
        
        
        for(unsigned int hit=0; hit<digitCont.size(); hit++){ // Loop down the stored hits until new pixels cannot be part of cluster
        
          if(   !isEdge(cluster[i])
             && abs(cluster[i]->channelID().col() - digitCont[hit]->channelID().col()) > 1
             && abs(cluster[i]->channelID().row() - digitCont[hit]->channelID().row()) > 1 ) break; // Too far away to be added

          if( abs(cluster[i]->channelID().row() - digitCont[hit]->channelID().row()) > 1 ) continue; // Not neighbouring in y
          
          if( !isEdge(cluster[i]) ){ // Deal with x direction for non-edge pixels
            if( abs(cluster[i]->channelID().col() - digitCont[hit]->channelID().col()) > 1 ) continue;
            else{
//              totVec.push_back(std::make_pair(digitCont[hit]->channelID(),digitCont[hit]->ToTValue()));
//              cluster.push_back(digitCont[hit]);                 //add hit to cluster
//              digitCont.erase(digitCont.begin()+hit);            //remove hit from container

              addToCluster(cluster,totVec,digitCont,hit);
            }
//            std::cout<<"Added pixel to cluster, now has size "<<cluster.size()<<std::endl;
            continue;
          } else{
            
            // Deal with x direction for edge pixels
            
            unsigned int ladder_cluster = floor(cluster[i]->channelID().chip() / 3);
            unsigned int ladder_hit = floor(digitCont[hit]->channelID().chip() / 3);
            unsigned int ladder_position = cluster[i]->channelID().chip() % 3;
            
            if(  (ladder_position == 0 && cluster[i]->channelID().col() == 0)
              || (ladder_position == 2 && cluster[i]->channelID().col() == 255) ){
              if( abs(cluster[i]->channelID().col() - digitCont[hit]->channelID().col()) > 1 ) continue;
              
              else{

//                totVec.push_back(std::make_pair(digitCont[hit]->channelID(),digitCont[hit]->ToTValue()));
//                cluster.push_back(digitCont[hit]);                 //add hit to cluster
//                digitCont.erase(digitCont.begin()+hit);            //remove hit from container

                addToCluster(cluster,totVec,digitCont,hit);
              }
              continue;
            }
               
            else{

              if( ladder_cluster != ladder_hit ) continue; // Not on same ladder
              
              if(   (cluster[i]->channelID().col() == 0 && digitCont[hit]->channelID().col() == 255)
                 || (cluster[i]->channelID().col() == 255 && digitCont[hit]->channelID().col() == 0) ){
                
//                totVec.push_back(std::make_pair(digitCont[hit]->channelID(),digitCont[hit]->ToTValue()));
//                cluster.push_back(digitCont[hit]);                 //add hit to cluster
//                digitCont.erase(digitCont.begin()+hit);            //remove hit from container
                addToCluster(cluster,totVec,digitCont,hit);
                continue;
                
              }
          
            }
          } // End of edge pixel check
        }
      }
      newsize=cluster.size();
    } while (newsize!=oldsize);//no more hits to be added to cluster
        

    unsigned int sensor = cluster[0]->channelID().module();      // sensor number in Gaudi
    
    double x = 0; //temp
    double y = 0; //temp
    double z = 0; //temp
    StatusCode mycode; //temp
    const DeVPSensor* vp_sensor = m_vpDet->sensorOfChannel(cluster[0]->channelID()); //temp
    
    int clustToT=0;
    double clustRow=0.;
    double clustCol=0.;    
    for(unsigned int it=0;it<cluster.size();it++){ 
      clustToT+=cluster[it]->ToTValue();
      clustRow+=cluster[it]->ToTValue() * (cluster[it]->channelID().col()+0.5);
      clustCol+=cluster[it]->ToTValue() * (cluster[it]->channelID().row()+0.5);
      Gaudi::XYZPoint pixel = vp_sensor->channelToPoint(cluster[it]->channelID()); //temp
      x+=(pixel.x() * cluster[it]->ToTValue()); //temp
      y+=(pixel.y() * cluster[it]->ToTValue()); //temp
      z+=(pixel.z() * cluster[it]->ToTValue()); //temp
    }
    clustRow/=clustToT;
    clustCol/=clustToT;
    
    x/=clustToT; //temp
    y/=clustToT; //temp
    z/=clustToT; //temp
    Gaudi::XYZPoint cluster_point(x,y,z); //temp
    LHCb::VPChannelID temp_id; //temp
    std::pair< double, double > temp_frac; //temp
    mycode = vp_sensor->pointToChannel(cluster_point, temp_id, temp_frac); //temp
    
    if(mycode == StatusCode::FAILURE) continue;
    //Make the barycentre channel
    LHCb::VPChannelID baryCenterChID(sensor,cluster[0]->channelID().chip(),floor(clustRow),floor(clustCol));
        
    //Get the interpixel fraction
    std::pair<unsigned int,unsigned int> xyFrac;
    xyFrac.first = int(ceil((clustRow-floor(clustRow)) * 7.));
    xyFrac.second = int(ceil((clustCol-floor(clustCol)) * 7.));
    if(xyFrac.first > 7.) xyFrac.first = int(7.);
    if(xyFrac.second > 7.) xyFrac.second = int(7.);
    
    //Push back lite clusters
//    bool isLong = false;
//    const VPLiteCluster newLiteCluster(baryCenterChID,1,xyFrac,isLong);
//    LHCb::VPCluster* newCluster = new LHCb::VPCluster(newLiteCluster,totVec);
//    clusterCont->insert(newCluster, baryCenterChID);
//    liteclusterCont->push_back(newLiteCluster); 
//    std::cout<<"Pushing back new cluster at "<<cluster_point<<" with channel id "<<temp_id<<std::endl;
    bool isLong = false; //temp
//    std::cout<<"New cluster with temp frac: "<<temp_frac.first<<","<<temp_frac.second<<std::endl<<" and cluster id: "<<temp_id<<std::endl;
    const VPLiteCluster newLiteCluster(temp_id,1,temp_frac,isLong); //temp
    LHCb::VPCluster* newCluster = new LHCb::VPCluster(newLiteCluster,totVec); //temp
    clusterCont->insert(newCluster, temp_id); //temp
    liteclusterCont->push_back(newLiteCluster); //temp
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Check if pixel is on the edge of a chip
//=============================================================================
bool VPClustering::isEdge(LHCb::VPDigit* digit) {
  
  if(digit->channelID().col() == 0 || digit->channelID().col() == 255) return true;
  else return false;
  
}

//=============================================================================

//=============================================================================
//  Add pixel to cluster
//=============================================================================
void VPClustering::addToCluster(std::vector<LHCb::VPDigit*>& cluster, std::vector< std::pair<LHCb::VPChannelID,int> >& totVec,
                                std::vector<LHCb::VPDigit*>& digitCont, unsigned int hit) {
  totVec.push_back(std::make_pair(digitCont[hit]->channelID(),digitCont[hit]->ToTValue()));
  cluster.push_back(digitCont[hit]);                 //add hit to cluster
  digitCont.erase(digitCont.begin()+hit);            //remove hit from container
 
}

//=============================================================================


//=============================================================================
//  Finalize
//=============================================================================
StatusCode VPClustering::finalize() {
  
  if ( m_debugLevel ) debug() << "==> Finalize" << endmsg;
  return GaudiHistoAlg::finalize(); // must be called after all other actions
}

//=============================================================================
