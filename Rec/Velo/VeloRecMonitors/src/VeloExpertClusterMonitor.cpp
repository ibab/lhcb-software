// $Id: VeloExpertClusterMonitor.cpp,v 1.4 2009-04-14 10:05:51 erodrigu Exp $
// Include files// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/AlgFactory.h" 
//#include "GaudiAlg/GaudiHisto.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
// from Event model
#include "VeloEvent/VeloTELL1Data.h"
#include "VeloEvent/EvtInfo.h"

//velo
#include "VeloDet/DeVeloSensor.h"
#include "Kernel/VeloChannelID.h"
#include "Event/VeloCluster.h"
#include "Event/Track.h"
#include "TrackInterfaces/IMeasurementProvider.h"

// local
#include "VeloExpertClusterMonitor.h"
//#include "CommonFunctions.h"

#include "boost/format.hpp"
#include <fstream>
#include <cmath>

//-----------------------------------------------------------------------------
// Implementation file for class : VeloExpertClusterMonitor
//
// nother VELO clusters monitoring algorithm should probably be combined with 
// veloclustermonitor and velo trackmonitor.
// Produces a set of histograms from clusters and tracks
// seperate plots are made for each sensor 
// expert mode makes plots for sensors as a function or r,phi and theta
//
// 2008-12-04 : Abdirizack NOOR, James Mylroie-Smith
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
namespace Velo 
{
  DECLARE_ALGORITHM_FACTORY( VeloExpertClusterMonitor );
}

//using namespace LHCb;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Velo::VeloExpertClusterMonitor::VeloExpertClusterMonitor( const std::string& name,
                        ISvcLocator* pSvcLocator)
  //  : GaudiAlgorithm ( name , pSvcLocator )
  : Velo::VeloMonitorBase ( name , pSvcLocator )//GaudiHistoAlg
{
  declareProperty( "UseExpertMode",   m_useExpert = false );
  declareProperty( "UseShiftMode",    m_useShift = true );
  declareProperty( "TrackLocation",   m_trackCont = LHCb::TrackLocation::Velo );
  declareProperty( "ClusterLocation", m_clusterCont = LHCb::VeloClusterLocation::Default );
  
}
//=============================================================================
// Destructor
//=============================================================================
Velo::VeloExpertClusterMonitor::~VeloExpertClusterMonitor() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode Velo::VeloExpertClusterMonitor::initialize() {
  StatusCode sc = VeloMonitorBase::initialize(); 
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( m_debugLevel ) debug() << "==> Initialize" << endmsg;
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode Velo::VeloExpertClusterMonitor::execute() {

  if ( m_debugLevel ) debug() << "==> Execute" << endmsg;
  //  plotClusters();
  loopTracks ();
  return StatusCode::SUCCESS;
}

//=============================================================================
// 
//=============================================================================
StatusCode Velo::VeloExpertClusterMonitor::loopTracks (){
 
  debug() << " ==> plotClusters " << endmsg;
  //  LHCb::Tracks* tracks=get<LHCb::Tracks>("Rec/Track/Velo");
  if ( !exist<LHCb::VeloClusters>( m_clusterCont ) ) {
    debug() << "No VeloClusters container found for this event !" << endmsg;
    return StatusCode::FAILURE;
    
  }
  else {
    m_clusters = get<LHCb::VeloClusters>( m_clusterCont );
    if ( m_debugLevel ) debug() << "  -> number of clusters found in TES: "
                                << m_clusters->size() <<endmsg;
  }
 
  if ( !exist<LHCb::Tracks>( m_trackCont ) ) {
    debug() << "No VeloTracks container found for this event !" << endmsg;
    return StatusCode::FAILURE;
  }
  else {
    tracks = get<LHCb::Tracks>( m_trackCont );
    if ( m_debugLevel ) debug() << "  -> number of tracks found in TES: "
                                << tracks->size() <<endmsg;
  }
  debug() << "Before making itereators" << endmsg;
  LHCb::Tracks::const_iterator itT ; 
  LHCb::VeloClusters::const_iterator itClusters ;

  
  char dirName[100];
  //=========================
  //loop for the raw clusters
  //=========================
  
  debug() << "Before Loop over raw clusters" << endmsg;
  for (itClusters = m_clusters->begin();itClusters != m_clusters->end(); ++itClusters ) {
    debug() << "Loop over raw clusters" << endmsg;
    LHCb::VeloCluster* cluster = *itClusters;
//     IVeloClusterPosition::toolInfo clusInfo = m_veloPositionTool->position( cluster );
    LHCb::VeloChannelID vcID = cluster->channelID();
//     int strip  = clusInfo.strip.strip();
    //    const DeVeloPhiType* thesensor = m_veloDet->sensor(vcID.sensor());
    // unsigned int zone=thesensor->zoneOfStrip(strip); 
    if (m_useExpert){
//       debug() << "Loop over raw clusters 2" << endmsg;
//       unsigned int check1 = cluster->channelID().sensor();
//       debug() << "Loop over raw clusters 2 check 1" << endmsg;
//       unsigned int check2 = cluster->channelID().strip();
//       debug() << "Loop over raw clusters 2 check2" << endmsg;
//      unsigned int zone = (cluster->channelID().sensor()).zopneOfStrip(cluster->channelID().strip());
//       debug() << "Loop over raw clusters 2 check3" << endmsg;
//       unsigned int check4 = DeVeloSensor::zoneOfStrip()      
//       debug() << "Loop over raw clusters 2 chec4" << endmsg;
//       //

      debug() << "Loop over raw clusters 3" << endmsg;
      LHCb::VeloChannelID idfilter = cluster->channelID();
      debug() << "Loop over raw clusters 4" << endmsg;
      unsigned int zone = (m_veloDet->sensor(cluster->channelID().sensor())->zoneOfStrip(cluster->channelID().strip()));  
      if(idfilter.isRType()){
        debug() << "\t-> is r type" << endmsg;
        snprintf(dirName, 99, "raw/non_corr/r_zone_%03d",zone );   
      }
      else if (idfilter.isPhiType()){
        debug() << "\t-> is phi type" << endmsg;
        snprintf(dirName, 99, "raw/non_corr/phi_zone_%03d",zone );   
      }
      
      
      debug() << "----------Expert Mode------------" << endmsg;
      plotCluster(cluster,dirName);
    }
    
    
    debug() << "Plot RAW clusters" << endmsg;
    plotCluster(cluster,"raw");
  }
    
  //===============================
  //loop for the rec track clusters
  //===============================

  for (itT = tracks->begin();itT != tracks->end(); ++itT ) {
    LHCb::Track* track = *itT;
    debug() << "Loop over Tracks" << endmsg;
    const std::vector< LHCb::LHCbID > & trackID = track->lhcbIDs() ;
    Gaudi::XYZVector slope= track->slopes();
    double theta = slope.Theta();
    //    double angleCorr = 1./sqrt(1+(slope.x()*slope.x())+(slope.y()*slope.y()));
    std::vector<LHCb::LHCbID>::const_iterator iID;
    std::vector<LHCb::LHCbID>::const_iterator imatchID;
    for (iID=trackID.begin(); iID!=trackID.end(); ++iID) {
      debug()<<"==> Loop over track IDs"<<endmsg;
      if((*iID).isVelo()) {
        const LHCb::VeloChannelID vcID = (*iID).veloID();
        LHCb::VeloCluster *cluster = m_clusters->object( vcID );
        LHCb::VeloChannelID idfilter = cluster->channelID();
        //        std::cout<<"\n\t the new lhcb channel id is: "<< idfilter.sensor()<<" ";
        int zone = (m_veloDet->sensor(cluster->channelID().sensor())->zoneOfStrip(cluster->channelID().strip()));  
        if(idfilter.isRType()){
          
          // add loop over track to get the relevent phi sensor cluster
          for (imatchID=iID; imatchID!=trackID.end(); ++imatchID) {
            debug() << "\t==> Second Loop over tracks for matching points" << endmsg;
            const LHCb::VeloChannelID vcmatchID = (*imatchID).veloID();
            LHCb::VeloCluster *cluster_match = m_clusters->object( vcmatchID );
            LHCb::VeloChannelID idfilter_match = cluster_match->channelID();
            unsigned int init_sensor = idfilter_match.sensor();
            
            const DeVeloRType* r_to_match = m_veloDet->rSensor(idfilter);
            
            //DeVelo::rSensor(idfilter.sensor());
            
            //          unsigned int)->sensorNumber()
            const DeVeloPhiType* associated_sensor =  r_to_match->associatedPhiSensor();
            unsigned int phisenNo = associated_sensor->sensorNumber();
            unsigned int rsenNo = r_to_match->sensorNumber();
            debug() << "\t==> R sensor found to then match to: "<< rsenNo << endmsg;            
            if (phisenNo== init_sensor ){
              //  std::cout<< "\tThis is a matched track\n";
              
            }
          }
        }
        
        

        if(idfilter.isRType()){
          snprintf(dirName, 99, "ontrack/r_zone_%03d",zone );   
          //std::cout<<" is R Type: \n ";
        }
        else if (idfilter.isPhiType()){
          snprintf(dirName, 99, "ontrack/phi_zone_%03d",zone );   
          //          std::cout<<" is Phi Type: \n ";
          //          snprintf(dirName, 99, "ontrack/zone_%03d",zone );
        }

        double prap = track->pseudoRapidity();
        plot1D(prap,"ontrack/pseudoRap_plot", "pseudorapidity",0,6.2,100);
        LHCb::Track::Types tType = track->type();
        if (m_useExpert){
          
          plotCluster(cluster,dirName,theta,prap);
        }
        plotCluster(cluster,"ontrack",theta,prap);
      }else{
        const LHCb::VeloChannelID no_vcID = (*iID).veloID();
        LHCb::VeloCluster *no_cluster = m_clusters->object( no_vcID );
        plotCluster(no_cluster,"non_velo",theta);
      }
    }  
  }
  
  return StatusCode::SUCCESS;
}


//============================================================================
// plot clusters
//============================================================================

StatusCode Velo::VeloExpertClusterMonitor::plotCluster(LHCb::VeloCluster* cluster, std::string ClusterType,double theta,double prap){
  debug() << " ==> plotClusters " << endmsg;
  // char senName[100];
  //   int rsensnum = 0;
  //   int phisensnum = 0;
  LHCb::VeloChannelID idtemp = cluster->channelID();
  int clsens = idtemp.sensor();
  //  int clstr = idtemp.strip();
  double clsize = cluster->size();
  double adc = cluster->totalCharge();
  double corr_adc(0);
  corr_adc =  adc*cos(theta);
  double corr_clsize(0);
  int i_max = 12;
  corr_clsize =  clsize*cos(theta);
  //if (theta != -400.){
    plot1D(theta, ClusterType+"/theta_plot", "Theta(r)", 0, 0.52, 100);

    if(prap!=-400.){
      
      //  double momentum = track->p();
      //       Gaudi::XYZVector mom = track->momentum();
      //        double prap = track->pseudoRapidity();
      // rapidity goes from 1.3 to 6

      //        int clsens = idfilter.sensor();

      //ClusterType = "ontrack";
      if (m_useExpert){
        double rap_min = 1.3;//lower rapidity range
        double rap_max = 6;
        double step = (rap_max-rap_min)/i_max;
        for ( int i = 0 ;i<i_max;i++){
          double max_rap = 1.1+(step*(i+1));
          double min_rap = 1.1+(step*(i));
          std::string rapName;
          std::string corr_rapName;
          //          double adc = cluster->totalCharge();
          //          double clsize = cluster->size();
          //          double corr_adc(0);
          //          corr_adc =  adc*cos(theta);
          //          double corr_clsize(0);
          //          corr_clsize =  clsize*cos(theta);
          char rapname[100];
          char corr_rapname[100];
          if(prap<max_rap && prap>min_rap){
            snprintf(rapname,99,"/non_corr/rapid/adc_cluster_rap_max_%03f", max_rap);    
            rapName = (rapname);
            plotSensorsADC(adc,rapName,ClusterType,clsens);
            plotSensorsSize(clsize,rapName,ClusterType,clsens);
            snprintf(corr_rapname,99,"/corr/rapid/adc_cluster_rap_max_%03f", max_rap);    
            corr_rapName = (corr_rapname);
            plotSensorsADC(corr_adc,corr_rapName,ClusterType,clsens);
            plotSensorsSize(corr_clsize,corr_rapName,ClusterType,clsens);
          }
        }
      }
    }
    

    if (m_useExpert){
      for(int i = 0;i<i_max;i++){
        double max_range = (i+1)*.05;
        double min_range = (i)*.05;
        std::string rangeName;
        char range[100];
        std::string corr_rangeName;
        char corr_range[100];
        if (m_useExpert){
          plotRPhiRange(cluster,idtemp,adc,"/non_corr",clsize,ClusterType,theta);
          if(theta!=-400.){
            plotRPhiRange(cluster,idtemp,corr_adc,"/corr",corr_clsize,ClusterType,theta);
          }
          
        }
        
        if(theta<max_range && theta>min_range){
          snprintf(range,99,"/non_corr/theta/adc_theta_max_%03f", max_range);    
          rangeName = (range);
          plotSensorsADC(adc,rangeName,ClusterType,clsens);
          plotSensorsSize(clsize,rangeName,ClusterType,clsens);
          snprintf(corr_range,99,"/corr/theta/adc_theta_max_%03f", max_range);    
          corr_rangeName = (corr_range);
          plotSensorsADC(corr_adc,corr_rangeName,ClusterType,clsens);
          plotSensorsSize(corr_clsize,corr_rangeName,ClusterType,clsens);
          if (m_useExpert){
            plotRPhiRange(cluster,idtemp,adc,rangeName,clsize,ClusterType,theta);
            plotRPhiRange(cluster,idtemp,corr_adc,corr_rangeName,corr_clsize,ClusterType,theta);
          }
          
        }
      }
    }
    debug() << "\t ->plot adc and size for all" << endmsg;
    plotSensorsADC(adc,"/non_corr",ClusterType,clsens);
    plotSensorsSize(clsize,"/non_corr",ClusterType,clsens);
    if(theta!=-400.){
      plotSensorsADC(corr_adc,"/corr",ClusterType,clsens);
      plotSensorsSize(corr_clsize,"/corr",ClusterType,clsens);
    }
    
    //}
  return StatusCode::SUCCESS;
}
//============================================================================
// plot r & phi ranges.
//============================================================================
StatusCode Velo::VeloExpertClusterMonitor::plotRPhiRange( LHCb::VeloCluster* cluster,LHCb::VeloChannelID idtemp,
                                      double adc, std::string path,double clsize,std::string ClusterType, double theta){

  char senName[100];
  int clsens = idtemp.sensor();
  int clstr = idtemp.strip();
  int rsensnum = 0;
  int phisensnum = 0;
  double corr_adc(0);
  corr_adc =  adc*cos(theta);
  double corr_clsize(0);
  corr_clsize =  clsize*cos(theta);
  if(idtemp.isRType()) {
    const DeVeloRType *rsens = m_veloDet->rSensor(clsens);
    rsensnum = clsens;
    snprintf(senName, 99, "adc_cluster_sen_%03d", clsens);

    for(int i = 0;i<12;i++){
      double max_radius = (i+10);
      double min_radius = (i+7);
      std::string radiusName;
      //std::string corr_radiusName;
      char radiusname[100];
      char corr_radiusname[100];
      double r_strfr = cluster->interStripFraction();
      double radius = rsens->rOfStrip(clstr,r_strfr);
      plot1D(radius, ClusterType+"/radius", "Cluster adc R", 0, 100, 100);

      if(radius<max_radius && radius>min_radius){
        snprintf(radiusname,99,"/r_range/adc_cluster_r_max_%03f", max_radius);
        radiusName = path + (radiusname);
        plotSensorsADC(adc,radiusName,ClusterType,clsens);
        plotSensorsSize(clsize,radiusName,ClusterType,clsens);
        //if (theta != -400.){    V    
        //snprintf(corr_radiusname,99,"/corr/r_range/adc_cluster_r_max_%03f", max_radius); 
        //corr_radiusName =   path + (corr_radiusname);
        //plotSensorsADC(corr_adc,corr_radiusName,ClusterType,clsens);
        //plotSensorsSize(corr_clsize,corr_radiusName,ClusterType,clsens);
        //}
      }
    }
    plotSensorsADC(adc,"/non_corr",ClusterType,clsens);
    plotSensorsSize(clsize,"/non_corr",ClusterType,clsens);
    if (theta != -400.){
      plotSensorsADC(corr_adc,"/corr",ClusterType,clsens);
      plotSensorsSize(corr_clsize,"/corr",ClusterType,clsens);
    }
    plot1D(adc, ClusterType+"/adc_cluster_R", "Cluster adc R", 0, 100, 100);
    plot1D(clsize, ClusterType+"/cluster_size_R", "Cluster size R", -0.5, 9.5, 10);
  } else if(idtemp.isPhiType()) {
    const DeVeloPhiType *psens = m_veloDet->phiSensor(clsens);
    phisensnum = clsens;
    int i_max = 12;
    double step = Gaudi::Units::pi/i_max;
    for(int i = 1;i<i_max+1;i++){
      double max_phi = -Gaudi::Units::pi+step*(i+1);
      double min_phi = -Gaudi::Units::pi+step*(i);
      std::string phiName;
      char phiname[100];
      //std::string corr_phiName;
      char corr_phiname[100];
      double phi_strfr = cluster->interStripFraction();
      double phi = psens->idealPhi(clstr,phi_strfr);
      if(phi<max_phi && phi>min_phi){
        snprintf(phiname,99,"/phi_range/adc_cluster_phi_max_%03f", max_phi);    
        phiName = path +(phiname) ;
        plotSensorsADC(adc,phiName,ClusterType,clsens);
        plotSensorsSize(clsize,phiName,ClusterType,clsens);
        //if (theta != -400.){
          //snprintf(corr_phiname,99,"/corr/phi_range/adc_cluster_phi_max_%03f", max_phi);    
          //corr_phiName = path +(corr_phiname);
          //plotSensorsADC(corr_adc,corr_phiName,ClusterType,clsens);
          //plotSensorsSize(corr_clsize,corr_phiName,ClusterType,clsens);
        //}
      }
    }


    plotSensorsADC(adc,"/non_corr",ClusterType,clsens);
    plotSensorsSize(clsize,"/non_corr",ClusterType,clsens);
    if (theta != -400.){
      plotSensorsADC(corr_adc,"/corr",ClusterType,clsens);
      plotSensorsSize(corr_clsize,"/corr",ClusterType,clsens);
    }
    plot1D(adc, ClusterType+"/adc_cluster_phi", "Cluster adc phi", 0, 100, 100);
    plot1D(clsize, ClusterType+"/cluster_size_phi", "Cluster size phi", -0.5, 9.5, 10);
  }
  
return StatusCode::SUCCESS;
}
  

//============================================================================
// plot all sensors ADCs
//============================================================================
StatusCode Velo::VeloExpertClusterMonitor::plotSensorsADC(double& adc, std::string corr,std::string& ClusterType,int sensor_num){
  plot1D(adc, ClusterType+corr+"/cluster_adc", "Cluster ADC ", 0, 100, 100);
  if(sensor_num!= -400){
    char senName[100];
    snprintf(senName, 99, "adc_cluster_sen_%03d", sensor_num);
    plot1D(adc, ClusterType+corr+"/adc/"+senName, "Cluster ADC ", 0, 100, 100);
  }
  
  return StatusCode::SUCCESS;
}
//============================================================================
// plot all sensors size
//============================================================================
StatusCode
Velo::VeloExpertClusterMonitor::plotSensorsSize( double& clsize,
                                                 std::string corr,
                                                 std::string& ClusterType,
                                                 int sensor_num ) {
  
  plot1D(clsize, ClusterType+corr+"/cluster_size", "Cluster Size ", -0.5, 9.5, 10);
  if(sensor_num!= -400){
    char senName[100];
    snprintf(senName, 99, "cluster_size%03d", sensor_num);
    plot1D(clsize, ClusterType+corr+"/clsize/"+senName, "Cluster Size ", -0.5, 9.5, 10);
  }
  
  return StatusCode::SUCCESS;
}

//============================================================================
// seperate angular plots
//============================================================================
StatusCode
Velo::VeloExpertClusterMonitor::plotAngles( double& adc,
                                            std::string& range,
                                            std::string& ClusterType,
                                            double theta,int sensor_num ) {
  
  plot1D(adc, ClusterType+range, "Cluster ADC ", 0, 100, 100);
  //+"/theta/adc/"
  if(sensor_num!= -400){
    char senName[100];
    snprintf(senName, 99, "/adc_cluster_sen_%03d", sensor_num);
    plot1D(adc, ClusterType+range+senName, "Cluster ADC ", 0, 100, 100);
    //+"/theta/adc/"
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode Velo::VeloExpertClusterMonitor::finalize() {
  
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return VeloMonitorBase::finalize(); // must be called after all other actions
}

//=============================================================================
