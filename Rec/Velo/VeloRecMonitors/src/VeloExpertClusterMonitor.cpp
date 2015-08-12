// Include files// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"
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
  DECLARE_ALGORITHM_FACTORY( VeloExpertClusterMonitor )
}

//using namespace LHCb;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Velo::VeloExpertClusterMonitor::VeloExpertClusterMonitor( const std::string& name,
                        ISvcLocator* pSvcLocator)
  //  : GaudiAlgorithm ( name , pSvcLocator )
  : Velo::VeloMonitorBase ( name , pSvcLocator )//GaudiHistoAlg
  , m_tracks(0)
  , m_clusters(0)
{
  declareProperty( "ExpertMode",   m_Expert = false );
  declareProperty( "RZones",   m_RZones = false );
  declareProperty( "PhiZones",   m_PhiZones = false );
  declareProperty( "BothRange",   m_BothRange = false );
  declareProperty( "RRange",   m_RRange = false );
  declareProperty( "PhiRange",   m_PhiRange = false );
  declareProperty( "RapidRange",   m_RapidRange = false );
  declareProperty( "Matching",   m_Matching = false );
  declareProperty( "ThetaRange",   m_ThetaRange = false );
  declareProperty( "NoSplits",   m_i_max =2 );
  declareProperty( "ShiftMode",    m_Shift = true );
  declareProperty( "TrackLocation",   m_trackCont = LHCb::TrackLocation::Velo );
  declareProperty( "ClusterLocation", m_clusterCont = LHCb::VeloClusterLocation::Default );
  declareProperty( "ClusterSize", m_plotClSize = false)  ;
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
  //=======================================================
  // Initilise histograms
  //=======================================================
  for( int i=0;i<84;i++){
    int no(0);
    if(i>41){
      no = i+22;
    }else{
      no = i;
    }
    char hname[100];
    char htitle[100];
    
    sprintf( hname, "raw/non_corr/adc/adc_cluster_sen_%03i", no );
    const GaudiAlg::HistoID name = hname;
    sprintf( htitle, "adc_cluster_sen_%03i", no );
    const GaudiAlg::HistoID title = htitle;
    
    m_adc_hist[i][0] = book1D( name, title, 0, 100, 100 );
  
    
    sprintf( hname, "ontrack/non_corr/adc/adc_cluster_sen_%03i", no );
    const GaudiAlg::HistoID on_name = hname;
    sprintf( htitle, "adc_cluster_sen_%03i", no );
    const GaudiAlg::HistoID on_title = htitle;
    
    m_adc_hist[i][1] = book1D( on_name, on_title, 0, 100, 100 );
    
    sprintf( hname, "ontrack/corr/adc/adc_cluster_sen_%03i", no );
    const GaudiAlg::HistoID corr_name = hname;
    sprintf( htitle, "adc_cluster_sen_%03i", no );
    const GaudiAlg::HistoID corr_title = htitle;
    m_adc_hist[i][2] = book1D( corr_name, corr_title, 0, 100, 100 );
    
  }
  

  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode Velo::VeloExpertClusterMonitor::execute() {

  if ( m_debugLevel ) debug() << "==> Execute" << endmsg;

  m_clusters = getIfExists<LHCb::VeloClusters>( m_clusterCont );
  if ( NULL == m_clusters ) {
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug() << "No VeloClusters container found for this event !" << endmsg;
    return StatusCode::FAILURE;
    
  }
  else {
    loopClusters ();
    if ( m_debugLevel ) debug() << "  -> number of clusters found in TES: "
                                << m_clusters->size() <<endmsg;
  }


  m_tracks = getIfExists<LHCb::Tracks>( m_trackCont );
  if ( NULL == m_tracks ) {
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug() << "No VeloTracks container found for this event !" << endmsg;
    //    return StatusCode::FAILURE;
  }
  else {
    loopTracks();
    if ( m_debugLevel ) debug() << "  -> number of tracks found in TES: "
                                << m_tracks->size() <<endmsg;
  }

  //  loopTracks ();


  return StatusCode::SUCCESS;
}
//=============================================================================
// Loop over cluster container
//=============================================================================
StatusCode Velo::VeloExpertClusterMonitor::loopClusters (){

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) {
    debug() << " ==> plotClusters " << endmsg;
    debug() << "Before making itereators" << endmsg;
  }
  
  LHCb::VeloClusters::const_iterator itClusters ;

  std::string dirName;
  //   //=========================
  //   //loop for the raw clusters
  //   //=========================
  
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
    debug() << "Before Loop over raw clusters" << endmsg;
  for (itClusters = m_clusters->begin();itClusters != m_clusters->end(); ++itClusters ) {
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug() << "Loop over raw clusters" << endmsg;
    LHCb::VeloCluster* cluster = *itClusters;

    LHCb::VeloChannelID vcID = cluster->channelID();

    if (m_RZones||m_PhiZones||m_Expert){
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
        debug() << "Loop over raw clusters 3" << endmsg;
      LHCb::VeloChannelID idfilter = cluster->channelID();
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
        debug() << "Loop over raw clusters 4" << endmsg;
      unsigned int zone = (m_veloDet->sensor(cluster->channelID().sensor())->zoneOfStrip(cluster->channelID().strip()));  
      if(idfilter.isRType() && (m_RZones||m_Expert)){
        if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
          debug() << "\t-> is r type" << endmsg;
        boost::format fmtEvt ( "raw/non_corr/r_zone_%03d" ) ;
        fmtEvt % zone ;
        dirName = fmtEvt.str() ;
        if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
          debug() << "----------Expert Mode------------" << endmsg;
        plotCluster(cluster,dirName);
      }
      else if (idfilter.isPhiType() && (m_PhiZones||m_Expert)){
        if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
          debug() << "\t-> is phi type" << endmsg;
        boost::format fmtEvt ( "raw/non_corr/phi_zone_%03d" ) ;
        fmtEvt % zone ;
        dirName = fmtEvt.str() ;
        if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
          debug() << "----------Expert Mode------------" << endmsg;
        plotCluster(cluster,dirName);
      }
    }
    
    
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug() << "Plot RAW clusters" << endmsg;
    plotCluster(cluster,"raw");
  }
  return StatusCode::SUCCESS;
}
//=============================================================================
// Loop over track container
//=============================================================================
StatusCode Velo::VeloExpertClusterMonitor::loopTracks (){
  
 
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
    debug() << "Before making itereators" << endmsg;
  LHCb::Tracks::const_iterator itT ; 
  
  // //    loop for the rec track clusters
  std::string dirName;
  for (itT = m_tracks->begin();itT != m_tracks->end(); ++itT ) {
    LHCb::Track* track = *itT;
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug() << "Loop over Tracks" << endmsg;
    const std::vector< LHCb::LHCbID > & trackID = track->lhcbIDs() ;
    Gaudi::XYZVector slope= track->slopes();
    double theta = slope.Theta();

    std::vector<LHCb::LHCbID>::const_iterator iID;
    std::vector<LHCb::LHCbID>::const_iterator imatchID;
    for (iID=trackID.begin(); iID!=trackID.end(); ++iID) {
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
        debug()<<"==> Loop over track IDs"<<endmsg;
      if((*iID).isVelo()) {
        const LHCb::VeloChannelID vcID = (*iID).veloID();
        LHCb::VeloCluster *cluster = m_clusters->object( vcID );
	if ( 0 == cluster ) {
	  error() << "Missing cluster with ID " << vcID << endmsg;
	  continue;
	}
        LHCb::VeloChannelID idfilter = cluster->channelID();
        int zone = (m_veloDet->sensor(cluster->channelID().sensor())->zoneOfStrip(cluster->channelID().strip()));  
        if(idfilter.isRType()&& (m_Matching||m_Expert)){
          // add loop over track to get the relevent phi sensor cluster
          for (imatchID=iID; imatchID!=trackID.end(); ++imatchID) {
            if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
              debug() << "\t==> Second Loop over tracks for matching points" << endmsg;
            const LHCb::VeloChannelID vcmatchID = (*imatchID).veloID();
            LHCb::VeloCluster *cluster_match = m_clusters->object( vcmatchID );
            LHCb::VeloChannelID idfilter_match = cluster_match->channelID();
            unsigned int init_sensor = idfilter_match.sensor();
            const DeVeloRType* r_to_match = m_veloDet->rSensor(idfilter);

            const DeVeloPhiType* associated_sensor =  r_to_match->associatedPhiSensor();
            unsigned int phisenNo = associated_sensor->sensorNumber();
            unsigned int rsenNo = r_to_match->sensorNumber();
            if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
              debug() << "\t==> R sensor found to then match to: "<< rsenNo << endmsg;            
            if (phisenNo== init_sensor ){
            }
          }
        }
        double prap = track->pseudoRapidity();
        plot1D(prap,"ontrack/pseudoRap_plot", "pseudorapidity",0,6.2,100);
        
        if(idfilter.isRType() && (m_RZones||m_Expert)){
          boost::format fmtEvt ( "ontrack/r_zone_%03d" ) ;
          fmtEvt % zone ;
          dirName = fmtEvt.str() ;
          plotCluster(cluster,dirName,theta,prap);
        }
        else if (idfilter.isPhiType()&& (m_PhiZones||m_Expert)){
          boost::format fmtEvt ( "ontrack/phi_zone_%03d" ) ;
          fmtEvt % zone ;
          dirName = fmtEvt.str() ;
          plotCluster(cluster,dirName,theta,prap);
        }
        if (m_Expert){
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

StatusCode Velo::VeloExpertClusterMonitor::plotCluster(LHCb::VeloCluster* cluster,std::string ClusterType,
                                                       double theta,double prap){
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
    debug() << " ==> plotClusters " << endmsg;

  LHCb::VeloChannelID idtemp = cluster->channelID();
  int clsens = idtemp.sensor();
  int clstr = idtemp.strip();
  double clsize = cluster->size();
  double adc = cluster->totalCharge();
  double corr_adc(0.);
  corr_adc =  adc*cos(theta);
  double corr_clsize(0.);
  if(idtemp.isRType()){
    const DeVeloRType *rsens = m_veloDet->rSensor(clsens);
    plot1D(adc, ClusterType+"/adc_cluster_R", "Cluster adc R", 0, 100, 100);
    if(m_plotClSize)plot1D(clsize, ClusterType+"/cluster_size_R", "Cluster size R", -0.5, 9.5, 10);
    double r_strfr = cluster->interStripFraction();
    double radius = rsens->rOfStrip(clstr,r_strfr);
    plot1D(radius, ClusterType+"/radius", "Radius of Clusters", 0, 43, 50);
  }else if(idtemp.isPhiType()){
    const DeVeloPhiType *psens = m_veloDet->phiSensor(clsens);
    plot1D(adc, ClusterType+"/adc_cluster_phi", "Cluster adc phi", 0, 100, 100);
    if(m_plotClSize) plot1D(clsize, ClusterType+"/cluster_size_phi", "Cluster size phi", -0.5, 9.5, 10);
    double phi_strfr = cluster->interStripFraction();
    double phi = psens->idealPhi(clstr,phi_strfr);
    plot1D(phi, ClusterType+"/phi", "Phi of Clusters", -4, 4, 50);
  }
  corr_clsize =  clsize*cos(theta);
  if (theta != -400.){
    plot1D(theta, ClusterType+"/theta_plot", "Theta(r)", 0, 0.52, 100);
  }
  
    if(prap!=-400.){
      // rapidity goes from ~1.3 to ~6
      //--------------------------------------------------------------------------------
      // loop over rapidity range
      //--------------------------------------------------------------------------------
      if (m_Expert||m_RapidRange){
        double rap_min = 1.3;//lower rapidity range
        double rap_max = 6;
               double step = (rap_max-rap_min)/m_i_max;
        for ( int i = 0 ;i<m_i_max;i++){
          double max_rap = 1.1+(step*(i+1));
          double min_rap = 1.1+(step*(i));
          std::string rapName;
          std::string corr_rapName;
          std::string rapname;
          std::string corr_rapname;
          if(prap<max_rap && prap>min_rap){
            boost::format fmtEvt ( "/non_corr/rapid/adc_cluster_rap_max_%03d" ) ;
            fmtEvt % max_rap ;
            rapname = fmtEvt.str() ;
            plotSensorsADC(adc,rapname,ClusterType,clsens);
    if(m_plotClSize)            plotSensorsSize(clsize,rapname,ClusterType,clsens);
            boost::format fmtCorr ( "/corr/rapid/adc_cluster_rap_max_%03d" ) ;
            fmtCorr % max_rap ;
            corr_rapname = fmtCorr.str() ;
            plotSensorsADC(corr_adc,corr_rapname,ClusterType,clsens);
    if(m_plotClSize)            plotSensorsSize(corr_clsize,corr_rapname,ClusterType,clsens);
          }
        }
      }
      
    }
      //--------------------------------------------------------------------------------
      // loop over Theta range
      //--------------------------------------------------------------------------------
    if (m_Expert || m_RRange || m_PhiRange || m_BothRange){
      plotRPhiRange(cluster,idtemp,adc,"/non_corr",clsize,ClusterType,theta);
      if(theta!=-400.){
        plotRPhiRange(cluster,idtemp,corr_adc,"/corr",corr_clsize,ClusterType,theta);
      }
    }
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug() << "\t ->plot adc and size for all" << endmsg;
    plotSensorsADC(adc,"/non_corr",ClusterType,clsens);
    if(m_plotClSize)    plotSensorsSize(clsize,"/non_corr",ClusterType,clsens);
    if(theta!=-400.){
      plotSensorsADC(corr_adc,"/corr",ClusterType,clsens);
    if(m_plotClSize)      plotSensorsSize(corr_clsize,"/corr",ClusterType,clsens);
    }
    if (m_Expert||m_ThetaRange){
      double theta_min = 0.;//lower rapidity range
      double theta_max = 0.52;
      double step = (theta_max-theta_min)/m_i_max;
      for(int i = 0;i<m_i_max;i++){
        double max_theta_range = (step*(i+1));
        double min_theta_range = (step*(i));
        if(theta<max_theta_range && theta>min_theta_range){
          boost::format fmtEvt ( "/non_corr/theta/adc_theta_max_%03d" ) ;
          fmtEvt % max_theta_range ;
          const std::string theta_rangeName = fmtEvt.str() ;
          //-----------------------------------------------------------------------
          //plots the noncorrected adc and size for different theta range
          //-----------------------------------------------------------------------
          plotSensorsADC(adc,theta_rangeName,ClusterType,clsens);
    if(m_plotClSize)          plotSensorsSize(clsize,theta_rangeName,ClusterType,clsens);
          boost::format fmtCorr ( "/corr/theta/adc_theta_max_%03d" ) ;
          fmtCorr % max_theta_range ;
          const std::string theta_corr_rangeName = fmtCorr.str() ;
          //-----------------------------------------------------------------------
          //plots the corrected adc and size for different theta range
          //-----------------------------------------------------------------------
          plotSensorsADC(corr_adc,theta_corr_rangeName,ClusterType,clsens);
    if(m_plotClSize)          plotSensorsSize(corr_clsize,theta_corr_rangeName,ClusterType,clsens);
          //-----------------------------------------------------------------------
          //plots the corrected and noncorrected adc and size for different theta range
          // as a function of the r and phi ranges too
          //-----------------------------------------------------------------------
          if (m_Expert|| m_RRange || m_PhiRange){
            plotRPhiRange(cluster,idtemp,adc,theta_rangeName,clsize,ClusterType,theta);
            plotRPhiRange(cluster,idtemp,corr_adc,theta_corr_rangeName,corr_clsize,ClusterType,theta);
          }
          
        }
      }
      
    }

    return StatusCode::SUCCESS;
}

//============================================================================
// plot r & phi ranges.
//============================================================================
StatusCode Velo::VeloExpertClusterMonitor::plotRPhiRange( LHCb::VeloCluster* cluster,
                                                          LHCb::VeloChannelID idtemp,
                                                          double adc, std::string path,
                                                          double clsize,std::string ClusterType,
                                                          double theta){
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
    debug() << "\t ->plotRPhiRang function" << endmsg;
  int clsens = idtemp.sensor();
  int clstr = idtemp.strip();
  double corr_adc(0);
  corr_adc =  adc*cos(theta);
  double corr_clsize(0);
  corr_clsize =  clsize*cos(theta);
  if(idtemp.isRType()&&(m_RRange||m_BothRange || m_Expert)) {
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug() << "\t\t ->rtype sensor" << endmsg;
    const DeVeloRType *rsens = m_veloDet->rSensor(clsens);
    boost::format fmtEvt ( "adc_cluster_sen_%03d" ) ;
    fmtEvt % clsens ;
    const std::string senName = fmtEvt.str() ;
    double rad_step = 43/m_i_max;
    
    for(int i = 0;i<m_i_max;i++){
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
        debug() << "\t\t\t ->for loop of rad" << endmsg;
      double max_radius = 7+((i+1)*rad_step);
      double min_radius = 7+(i*rad_step);
      std::string radiusName;
      double r_strfr = cluster->interStripFraction();
      double radius = rsens->rOfStrip(clstr,r_strfr);
      
      if(radius<max_radius && radius>min_radius){
        if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
          debug() << "\t\t\t\t ->if in rad range" << endmsg;
        boost::format fmtEvt ( "/r_range/adc_cluster_r_max_%03d" ) ;
        fmtEvt % max_radius ;
        const std::string radiusname = fmtEvt.str() ;        
        radiusName = path + (radiusname);
        plotSensorsADC(adc,radiusName,ClusterType,clsens);
	if(m_plotClSize)        plotSensorsSize(clsize,radiusName,ClusterType,clsens);
        
      }
    }


    
        plotSensorsADC(adc,"/non_corr",ClusterType,clsens);
    if(m_plotClSize)        plotSensorsSize(clsize,"/non_corr",ClusterType,clsens);
        if (theta != -400.){
          plotSensorsADC(corr_adc,"/corr",ClusterType,clsens);
    if(m_plotClSize)          plotSensorsSize(corr_clsize,"/corr",ClusterType,clsens);
        }
  } else if(idtemp.isPhiType()&&(m_PhiRange || m_BothRange||m_Expert)) {
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug() << "\t ->Phi Type sensor" << endmsg;
    const DeVeloPhiType *psens = m_veloDet->phiSensor(clsens);

    double phi_step = Gaudi::Units::pi/m_i_max;
    for(int i = 1;i<(m_i_max+1);i++){
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
        debug() << "\t\t ->for loop of Phi" << endmsg;
      double max_phi = -Gaudi::Units::pi+phi_step*(i+1);
      double min_phi = -Gaudi::Units::pi+phi_step*(i);
      std::string phiName;
      std::string phiname;
      double phi_strfr = cluster->interStripFraction();
      double phi = psens->idealPhi(clstr,phi_strfr);
      if(phi<max_phi && phi>min_phi){
        if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
          debug() << "\t\t\t ->in phi range" << endmsg;
        boost::format fmtEvt ( "/phi_range/adc_cluster_phi_max_%03d" ) ;
        fmtEvt % max_phi ;
        phiname = fmtEvt.str() ;
        phiName = path +(phiname) ;
        plotSensorsADC(adc,phiName,ClusterType,clsens);
    if(m_plotClSize)        plotSensorsSize(clsize,phiName,ClusterType,clsens);
      }
    }
    plotSensorsADC(adc,"/non_corr",ClusterType,clsens);
    if(m_plotClSize)    plotSensorsSize(clsize,"/non_corr",ClusterType,clsens);
    if (theta != -400.){
      plotSensorsADC(corr_adc,"/corr",ClusterType,clsens);
    if(m_plotClSize)      plotSensorsSize(corr_clsize,"/corr",ClusterType,clsens);
    }
  }
  
  return StatusCode::SUCCESS;
}
//============================================================================
// plot all sensors ADCs
//============================================================================
StatusCode Velo::VeloExpertClusterMonitor::plotSensorsADC(double& adc, std::string corr,std::string& ClusterType,int sensor_num){
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
    debug() << "\t ->plotSensorsADC" << endmsg;
  int  sensor(0);
  if (sensor_num<106){
    if (sensor_num>41){
      sensor = sensor_num-22;
    }else {
      sensor = sensor_num;
    }
    if (ClusterType == "raw"){
      m_adc_hist[sensor][0]->fill (adc);
    } else if (ClusterType == "ontrack"){
      if (corr == "/non_corr"){
	m_adc_hist[sensor][1]->fill (adc);
      }else if ( corr == "/corr"){
	m_adc_hist[sensor][2]->fill (adc);
      }
    }
    plot1D(adc, ClusterType+corr+"/cluster_adc", "Cluster ADC ", 0, 100, 100);
  }
  return StatusCode::SUCCESS;
}
//============================================================================
// plot all sensors size
//============================================================================
StatusCode Velo::VeloExpertClusterMonitor::plotSensorsSize( double& clsize,
                                                            std::string corr,
                                                            std::string& ClusterType,
                                                            int sensor_num ) {
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
    debug() << "\t ->plotSensorSize" << endmsg;
  plot1D(clsize, ClusterType+corr+"/cluster_size", "Cluster Size ", -0.5, 9.5, 10);
  if(sensor_num!= -400){
    std::string senName;
    boost::format fmtEvt ( "cluster_size%03d" ) ;
    fmtEvt % sensor_num ;
    senName = fmtEvt.str() ; 
    plot1D(clsize, ClusterType+corr+"/clsize/"+senName, "Cluster Size ", -0.5, 9.5, 10);
  }
  return StatusCode::SUCCESS;
}
//============================================================================
// seperate angular plots
//============================================================================
StatusCode Velo::VeloExpertClusterMonitor::plotAngles( double& adc,
                                                       std::string& range,
                                                       std::string& ClusterType,
                                                       int sensor_num ) {
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
    debug() << "\t -> plotAngles" << endmsg;
  plot1D(adc, ClusterType+range, "Cluster ADC ", 0, 100, 100);
  if(sensor_num!= -400){
    std::string senName;
    boost::format fmtEvt ( "/adc_cluster_sen_%03d" ) ;
    fmtEvt % sensor_num ;
    senName = fmtEvt.str() ; 
    plot1D(adc, ClusterType+range+senName, "Cluster ADC ", 0, 100, 100);
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
