// $Id: VPClusterMonitor.cpp, v1.0 2012-07-06 dhynds

// Include files
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "Event/Track.h"
#include "GaudiAlg/GaudiHistoAlg.h"

// local
#include "VPClusterMonitor.h"

// VPelDet
#include "VPDet/DeVP.h"
// Event
#include "Event/VPDigit.h"
#include "Event/VPCluster.h"
#include "Event/VPLiteCluster.h"
#include "Linker/LinkedTo.h"

#include "Event/MCHit.h"
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"

using namespace Gaudi::Units;
using namespace LHCb;
using namespace Gaudi;

//-----------------------------------------------------------------------------
// Implementation file for class : VPClusterMonitor
//
// 2012-07-06 : Daniel Hynds
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( VPClusterMonitor )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VPClusterMonitor::VPClusterMonitor( const std::string& name,
                        ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator )
{
  declareProperty( "ClusterLocation", m_clusterCont = LHCb::VPClusterLocation::VPClusterLocation );
  declareProperty( "MCHitLocation", m_hitCont = LHCb::MCHitLocation::VP );
  declareProperty( "DataTaken", m_dataTaken = 0. );
  declareProperty( "Irradiated", m_irradiated = false );

}

//=============================================================================
// Destructor
//=============================================================================
VPClusterMonitor::~VPClusterMonitor() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode VPClusterMonitor::initialize() {
  StatusCode sc = GaudiHistoAlg::initialize(); 
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiHistoAlg
  debug() << "==> Initialize" << endmsg;
  // Set histo dir
  setHistoTopDir( "VPDigiMon/" );
  // Get detector element
  m_vpDet = getDet<DeVP>( DeVPLocation::Default );
  // Get radiation damage tool
  if(m_irradiated) m_radDamageTool = tool<VPRadiationDamageTool>("VPRadiationDamageTool");
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode VPClusterMonitor::execute() {

  debug() << "==> Execute" << endmsg;
  // Get VP Clusters
  m_clusters = getIfExists<LHCb::VPClusters>( m_clusterCont );
  if(!m_clusters){
    error() << "No VPClusters founds in this event" << endmsg;
    return StatusCode::FAILURE;
  }
  // Make plots
  debug() << "  -> number of clusters found in TES: "<< m_clusters->size() <<endmsg;
  loopClusters ();
  return StatusCode::SUCCESS;
}
//=============================================================================
// Loop over cluster container
//=============================================================================
StatusCode VPClusterMonitor::loopClusters (){
  debug() << "==> Looping over clusters " << endmsg;
  // Get cluster iterator and link to MC hits
  LHCb::VPClusters::const_iterator itVPClusters;
  LinkedTo<LHCb::MCHit, LHCb::VPCluster> links = LinkedTo<LHCb::MCHit, LHCb::VPCluster>(evtSvc(), msgSvc(), LHCb::VPClusterLocation::VPClusterLocation + "2MCHits" );
  // Loop over clusters  
  for (itVPClusters = m_clusters->begin();itVPClusters != m_clusters->end(); itVPClusters++){
    LHCb::VPCluster* cluster = *itVPClusters;
    LHCb::VPLiteCluster lCluster = cluster->lCluster();
    // Set up some objects for later use
    std::vector< std::pair<LHCb::VPChannelID,int> > totVec = cluster->pixelHitVec();
    int clustToT = 0;
    double cluster_x(0),cluster_y(0),cluster_z(0),pix1ToT(0),pix2ToT(0),pixel_x(0); 
    std::vector<double> xvalues;
    // Loop over pixel hits
    for(unsigned int it=0;it<totVec.size();it++){
      clustToT+=totVec[it].second;
      // Enter pixel values for eta distribution plots
      if(it == 0 && totVec.size() == 2) pix1ToT = totVec[it].second;
      if(it == 1 && totVec.size() == 2) pix2ToT = totVec[it].second;
      // Get sensor
      const DeVPSensor* sensor = m_vpDet->sensorOfChannel(totVec[it].first);
      // Get XYZ of pixel
      Gaudi::XYZPoint pointGlobal = sensor->channelToPoint(totVec[it].first);
      // Get pixel radius
      double radius = sqrt(pow(pointGlobal.x(),2)+pow(pointGlobal.y(),2));
      // Plot pixel information
      plot(radius, "pixel_radius", 0, 100, 200);
      plot(totVec[it].second, "single_pixel_tot", 0, 50, 50);
      plot2D(radius, totVec[it].second, "single_pixel_tot_versus_r", 0, 100, 0, 50, 200, 50);
      plot2D(pointGlobal.x(), pointGlobal.y(), "xy_map_pixels", -100, 100, -100, 100, 400, 400);
      plot2D(pointGlobal.z(), pointGlobal.x(), "xz_map_pixels", -500, 1000, -100, 100, 1000, 400);
      plot2D(pointGlobal.z(), pointGlobal.y(), "yz_map_pixels", -500, 1000, -100, 100, 1000, 400);
      // Get info for eta distribution
      pixel_x=pointGlobal.x();
      // Make cluster information
      cluster_x+=totVec[it].second*pointGlobal.x();
      cluster_y+=totVec[it].second*pointGlobal.y();
      cluster_z+=totVec[it].second*pointGlobal.z();
      // Information about cluster width
      bool newX=true;
      for(unsigned int j=0;j<xvalues.size();j++){
        if(xvalues[j] == pointGlobal.x()) newX=false;
      }
      if(newX) xvalues.push_back(pointGlobal.x());
    }
    // Get XYZ of cluster 
    cluster_x/=clustToT;
    cluster_y/=clustToT;
    cluster_z/=clustToT;
    Gaudi::XYZPoint cluster_point(cluster_x, cluster_y, cluster_z);
    // Plot where the cluster is
    double cluster_radius = sqrt(pow(cluster_x,2)+pow(cluster_y,2));
    plot(totVec.size(), "global_cluster_size", 0, 100, 100);
    plot(cluster_radius, "cluster_radius", 0, 100, 200);
    plot(clustToT, "cluster_tot", 0, 50, 50);
    plot2D(cluster_radius, clustToT, "cluster_tot_versus_r", 0, 100, 0, 50, 200, 50);
    plot2D(cluster_radius, totVec.size(), "cluster_size_versus_r", 0, 100, 0, 100, 200, 100);
    plot2D(cluster_x, cluster_y, "xy_map_clusters", -100, 100, -100, 100, 400, 400);
    plot2D(cluster_z, cluster_x, "xz_map_clusters", -500, 1000, -100, 100, 1000, 400);
    plot2D(cluster_z, cluster_y, "yz_map_clusters", -500, 1000, -100, 100, 1000, 400);
    plot(lCluster.interPixelFractionX(), "interpixel_fraction_x",-0.1,1.1,120);
    plot(lCluster.interPixelFractionY(), "interpixel_fraction_y",-0.1,1.1,120);
    // Check if 2 pixels wide in x
    if(totVec.size() == 2 && cluster_x != pixel_x){
      // Plot eta distributions
      plot(pix1ToT/(pix1ToT+pix2ToT), "eta_distribution_pix1", 0, 1, 100);
      plot(pix2ToT/(pix1ToT+pix2ToT), "eta_distribution_pix2", 0, 1, 100);
      plot2D(cluster_radius, pix1ToT/(pix1ToT+pix2ToT), "eta_distribution_pix1_versus_radius", 0, 100, 0, 1, 200, 100);
      plot2D(cluster_radius, pix2ToT/(pix1ToT+pix2ToT), "eta_distribution_pix2_versus_radius", 0, 100, 0, 1, 200, 100);
    }
    // Plot fluence information
    if(m_irradiated){
      double F = m_radDamageTool->fluence(cluster_point, m_dataTaken);
      plot(F/1e15, "global_fluence_e15", 0, 20, 200);
      plot2D(cluster_radius, F/1e15, "fluence_e15_versus_r", 0, 100, 0, 20, 200, 200);
      plot2D(cluster_point.z(), F/1e15, "fluence_e15_versus_z", -500, 1000, 0, 20, 1000, 200);
    }
    // Get MC hit for this cluster and plot residuals
    LHCb::MCHit* hit = links.first(cluster->channelID());
    if(hit){
      // Get true track direction for this hit
      double yangle = atan(hit->dydz())/degree;
      double xangle = atan(hit->dxdz())/degree;
      double theta=sqrt(pow(xangle,2)+pow(yangle,2));
      plot(theta, "track_theta", 0., 50., 100);
      plot(xangle, "track_x_angle", -30., 30., 120);
      plot(yangle, "track_y_angle", -30., 30., 120);
      // Get XYZ of hit
      Gaudi::XYZPoint mchitPoint=(hit->entry() + hit->displacement()/2);  
      // Calculate 3D hit residuals
      double resid3d = sqrt(pow(cluster_point.x()-mchitPoint.x(),2)
                            +pow(cluster_point.y()-mchitPoint.y(),2)
                            +pow(cluster_point.z()-mchitPoint.z(),2));
      plot(resid3d, "3d_residuals",-0.2,0.2,4000);
      plot2D(theta, resid3d, "3d_residuals_versus_track_theta", 0., 50., -0.2, 0.2, 100, 400);
      plot2D(cluster_radius, resid3d, "3d_residuals_versus_radius", 0., 100, -0.2, 0.2, 200, 400);
      // Plot XY residuals
      plot(cluster_x - mchitPoint.x(), "x_residuals",-0.2,0.2,4000);
      plot(cluster_y - mchitPoint.y(), "y_residuals",-0.2,0.2,4000);
      plot2D(xangle, cluster_x - mchitPoint.x(), "x_residuals_versus_track_angle_x", -30., 30., -0.2, 0.2, 120, 400);
      plot2D(xangle, cluster_y - mchitPoint.y(), "y_residuals_versus_track_angle_x", -30., 30., -0.2, 0.2, 120, 400);
      plot2D(xangle, totVec.size(), "cluster_size_versus_track_angle_x", -30., 30., 0, 20, 120, 20);
      plot2D(xangle, xvalues.size(), "cluster_widthx_versus_track_angle_x", -30., 30., 0, 20, 120, 20);
      // Plot the same for small y angles
      if(yangle < 2 && yangle > -2){
        plot2D(xangle, cluster_x - mchitPoint.x(), "x_residuals_versus_track_angle_x_small_y", -30., 30., -0.2, 0.2, 120, 400);
        plot2D(xangle, cluster_y - mchitPoint.y(), "y_residuals_versus_track_angle_x_small_y", -30., 30., -0.2, 0.2, 120, 400);
        plot2D(xangle, totVec.size(), "cluster_size_versus_track_angle_x_small_y", -30., 30., 0, 20, 120, 20);
        plot2D(xangle, xvalues.size(), "cluster_widthx_versus_track_angle_x_small_y", -30., 30., 0, 20, 120, 20);
      }      
    } // End of hit information
  } // End of loop over clusters
  return StatusCode::SUCCESS;
}


