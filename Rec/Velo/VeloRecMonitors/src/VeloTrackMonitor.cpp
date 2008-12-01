// $Id: VeloTrackMonitor.cpp,v 1.6 2008-12-01 12:57:57 gersabec Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
/*
 *  Implementation file for class : Velo::VeloTrackMonitor
 *
 *  @author Sadia Khalil
 *  @date   2008-09-15
 *  @email: skhalil@phy.syr.edu
 *  Based on original version by
 *  @author Aras Papadelis, Thijs Versloot
 *  @date   2006-04-30
*/

// from Boost
#include <boost/lambda/bind.hpp>

// from VeloDet
#include "VeloDet/DeVeloSensor.h"
#include "VeloDet/DeVelo.h"
#include "Event/FitNode.h"

// AIDA
#include "AIDA/IProfile1D.h"
#include "AIDA/IHistogram1D.h"

// local
#include "VeloTrackMonitor.h"

// Event
#include "Event/VeloCluster.h"
#include "Event/TrackTypes.h"

using namespace Gaudi::Units;
using namespace boost::lambda;
using namespace LHCb;
using namespace Gaudi;

// Declaration of the Algorithm Factory
DECLARE_NAMESPACE_ALGORITHM_FACTORY(Velo, VeloTrackMonitor );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Velo::VeloTrackMonitor::VeloTrackMonitor( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : Velo::VeloMonitorBase ( name , pSvcLocator ),
    
    m_veloDet(0),
    m_event(0),
    m_resetProfile(1000)

{
  declareProperty( "TrackLocation", m_trackCont = LHCb::TrackLocation::Velo);
  declareProperty( "ClusterLocation", m_clusterCont = LHCb::VeloClusterLocation::Default );
  declareProperty( "HitMapHistos", m_hitmapHistos = false);
  declareProperty( "BiasedResiduals", m_biasedResidualProfile = true);
  declareProperty( "UnbiasedResiduals", m_unbiasedResidualProfile = false);
  declareProperty( "EventNumber", m_event);
  declareProperty( "ProfileResetEvent", m_resetProfile);
  declareProperty( "ExtraPlots", m_xPlots = true);
  declareProperty( "EventClusterInfo", m_EventClusterInfo = true);
  declareProperty( "AlignMoniBasic", m_alignMoniBasic = false);
  declareProperty( "ACDCGeometry", m_ACDC = false);
}
//=============================================================================
// Destructor
//=============================================================================
Velo::VeloTrackMonitor::~VeloTrackMonitor() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode Velo::VeloTrackMonitor::initialize() {
  StatusCode sc = VeloMonitorBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  m_veloDet = getDet<DeVelo>( DeVeloLocation::Default );
  m_measurementprovider  = tool<IMeasurementProvider>( "MeasurementProvider",
                                                       "MeasProvider", this );
  m_clusterTool = tool<IVeloClusterPosition>( "VeloClusterPosition" );
  m_expectTool = tool<IVeloExpectation>( "VeloExpectation");
 
  if ( m_alignMoniBasic ) {
    const GaudiAlg::HistoID aliMonM_R_A_ID = "ResidualMean_R_A" ; 
    const GaudiAlg::HistoID aliMonM_R_C_ID = "ResidualMean_R_C" ; 
    const GaudiAlg::HistoID aliMonM_P_A_ID = "ResidualMean_P_A" ; 
    const GaudiAlg::HistoID aliMonM_P_C_ID = "ResidualMean_P_C" ; 
    m_h_aliMon_Mean_R_A = book1D( aliMonM_R_A_ID, "Residual mean R sensors A side", -5, 5, 100 ); 
    m_h_aliMon_Mean_R_C = book1D( aliMonM_R_C_ID, "Residual mean R sensors C side", -5, 5, 100 ); 
    m_h_aliMon_Mean_P_A = book1D( aliMonM_P_A_ID, "Residual mean Phi sensors A side", -5, 5, 100 ); 
    m_h_aliMon_Mean_P_C = book1D( aliMonM_P_C_ID, "Residual mean Phi sensors C side", -5, 5, 100 ); 
    const GaudiAlg::HistoID aliMonS_R_A_ID = "ResidualSigma_R_A" ; 
    const GaudiAlg::HistoID aliMonS_R_C_ID = "ResidualSigma_R_C" ; 
    const GaudiAlg::HistoID aliMonS_P_A_ID = "ResidualSigma_P_A" ; 
    const GaudiAlg::HistoID aliMonS_P_C_ID = "ResidualSigma_P_C" ; 
    m_h_aliMon_Sigma_R_A = book1D( aliMonS_R_A_ID, "Residual sigma R sensors A side", 0, 5, 50 ); 
    m_h_aliMon_Sigma_R_C = book1D( aliMonS_R_C_ID, "Residual sigma R sensors C side", 0, 5, 50 ); 
    m_h_aliMon_Sigma_P_A = book1D( aliMonS_P_A_ID, "Residual sigma Phi sensors A side", 0, 5, 50 ); 
    m_h_aliMon_Sigma_P_C = book1D( aliMonS_P_C_ID, "Residual sigma Phi sensors C side", 0, 5, 50 ); 
  }

  m_binary = sqrt( 12. );

  return StatusCode::SUCCESS;
}
//=============================================================================
// Main execution
//=============================================================================
StatusCode Velo::VeloTrackMonitor::execute() {

  // Count the number of events monitored
  // ------------------------------------
  m_event++;
  
  // Retrieve the VeloClusters and VeloTracks
  // ----------------------------------------
  StatusCode sc1 = getVeloTracks();
  StatusCode sc2 = getVeloClusters();
  if(!(sc1.isSuccess() && sc2.isSuccess())){
    return StatusCode::SUCCESS;
  }

  //Monitor the Tracks
  //------------------
  StatusCode sc3 = monitorTracks();  
  if ( !( sc3.isSuccess() ) ) {
    return StatusCode::SUCCESS;
  }
  
  return StatusCode::SUCCESS;
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode Velo::VeloTrackMonitor::finalize() {
  
  if ( m_debugLevel ) debug() << "==> Finalize" << endmsg;
  m_event = 0;
  return VeloMonitorBase::finalize();  // must be called after all other actions
}
//=========================================================================
// Retrieve the VeloClusters 
//=========================================================================
StatusCode Velo::VeloTrackMonitor::getVeloClusters ( ) {
  if ( m_debugLevel )
    debug() << "Retrieving VeloClusters from " << m_clusterCont << endmsg;
  
  if ( !exist<LHCb::VeloClusters>( m_clusterCont ) ) {
    debug() << "No VeloClusters container found for this event !" << endmsg;
    return StatusCode::FAILURE;
    
  }
  else {
    m_clusters = get<LHCb::VeloClusters>( m_clusterCont );
    if ( m_debugLevel ) debug() << "  -> number of clusters found in TES: "
                                << m_clusters->size() <<endmsg;
  }
 
  return StatusCode::SUCCESS; 
}
//=========================================================================
// Retrieve the VeloTracks
//=========================================================================
StatusCode Velo::VeloTrackMonitor::getVeloTracks ( ) {
  if ( m_debugLevel )
    debug() << "Retrieving VeloTracks from " << m_trackCont << endmsg;
  
  if ( !exist<LHCb::Tracks>( m_trackCont ) ) {
    debug() << "No VeloTracks container found for this event !" << endmsg;
    return StatusCode::FAILURE;
  }
  else {
    m_tracks = get<LHCb::Tracks>( m_trackCont );
    if ( m_debugLevel ) debug() << "  -> number of tracks found in TES: "
                                << m_tracks->size() <<endmsg;
  }

  return StatusCode::SUCCESS;  
}

//=========================================================================
// Monitor the VeloTracks 
//=========================================================================
StatusCode Velo::VeloTrackMonitor::monitorTracks ( ) {
  
  
  if (m_debugLevel ) debug()<<"--> Excecuting trackMonitor"<< endmsg;
  if (!m_tracks) return (StatusCode::SUCCESS);
  
  if (m_tracks->size() == 0) {
    if (m_debugLevel ) debug()<<"---Track Container retrieved but EMPTY---"<< endmsg;
    return StatusCode::SUCCESS;
  }

  //Number of Tracks per Event
  unsigned int nTracks = m_tracks->size();
  //PolarAngle of Tracks
  double theta(0.);
  //Number of R, Phi and total clusters in tracks per Event
  unsigned int nRClusEvent (0), nPhiClusEvent (0), nSumClusEvent (0);
 
  //book profile for sensors vs biasedResiduals
  const GaudiAlg::HistoID sensors = "Sensors_vs_BiasedResiduals" ; 
  AIDA::IProfile1D* prof_sensors = bookProfile1D
    (sensors, "Sensors vs BiasedResiduals(um)" , -0.5, 139.5, 140) ;
  if ( m_event == m_resetProfile ) { prof_sensors -> reset() ; } // RESET AFTER CERTAIN EVENTS

  //book profile for polarAngle vs RClusters
  const GaudiAlg::HistoID thetaR = "RClusters_per_Track_vs_Theta" ; 
  AIDA::IProfile1D* prof_thetaR = bookProfile1D
    (thetaR, " Clusters vs Theta(degree) " , -0.5, 30.5, 31) ;
  if ( m_event == m_resetProfile ) { prof_thetaR -> reset() ; } // RESET AFTER CERTAIN EVENTS
  
  //book profile for polarAngle vs TotClusters
  const GaudiAlg::HistoID thetaTot = "TotClusters_per_Track_vs_Theta" ; 
  AIDA::IProfile1D* prof_thetaTot = bookProfile1D
    (thetaTot, "TotClusters vs Theta(degree)" , -0.5, 30.5, 31) ;
  if ( m_event == m_resetProfile ) { prof_thetaTot -> reset() ; } // RESET AFTER CERTAIN EVENTS
  
  //Loop over track container
  LHCb::Tracks::const_iterator itTrk;
  
  for(itTrk=m_tracks->begin();itTrk!=m_tracks->end();itTrk++){

    nTracks++;
    LHCb::Track* track = (*itTrk);
    
    //Call Measurement Provider
    if(!track -> checkPatRecStatus(Track::PatRecMeas)){
      if(m_debugLevel)debug() <<"No measurements found on tracks, calling measurement provider"<<endmsg;
      m_measurementprovider->load(*track); 
    }

    //Skip the tracks without VELO(Velo, VeloR, Upstream and Long)hits
    if ( !track->hasVelo() ) {
      if (m_debugLevel) debug() <<"Track has no VELO hits, continuing with next track."<< endmsg;
      continue;
    }
    
    //General track properties
      if (m_debugLevel){
        debug()<< "ErrorFlag:"<< track->flag()<<endmsg;
        debug()<< "Track Type:"<<track->type()<<endmsg;
        debug()<< "Track History:"<<track->history()<<endmsg;
        debug()<< "Chi2/DoF per Track:" << track->chi2PerDoF()<<endmsg;
        debug()<< "Tracks per container:"<<m_tracks->size()<<endmsg;
      }
      //theta, phi& p  plots
      //---------------------------
      if( m_xPlots){
        theta = track->firstState().slopes().theta();
        plot1D(theta/degree, "theta(degree)", -0.5, 50.5, 51);
        plot1D(track->phi()/degree, "Azimuth:phi (degree)", -200., 200., 100);
        
        if( m_trackCont != LHCb::TrackLocation::Velo){
          plot1D(track->p()/GeV, "Momentum(GeV)", -0.5, 100.5, 101);
        }
      }
      
      //eta
      //---
      plot1D(track->pseudoRapidity(), "eta", 0.95, 6.05, 51);

      //psudo-efficiency: Using Velo expectation Tool
      //--------------------------------------------
      int nExpectedHits = m_expectTool->nExpected(*track);
      
      //unsigned int nVeloHits = track->nLHCbIDs(); 
      const std::vector<LHCb::LHCbID>& id = track->lhcbIDs();
      const int nVeloHits = std::count_if(id.begin(), id.end(),bind(&LHCbID::isVelo,_1));
      if(nExpectedHits != 0){
        double pseudoEfficiency = double(nVeloHits)/double(nExpectedHits);
        plot1D(pseudoEfficiency, "pseudoEfficiency", "pseudoEfficiency", -0.05, 1.55, 16);
      }

      //Unbiased Residuals
      //------------------
      if(m_unbiasedResidualProfile && m_trackCont != LHCb::TrackLocation::Velo){  
        
        unbiasedResiduals (track);
      }
      
      unsigned int nRClus(0), nPhiClus(0), nSumClus(0);
    
      //Store info in matrices
      std::vector<int> matchedSensors;
      std::vector<double> rCoor;
      std::vector<double> phiCoor;
      std::vector<double> rCoor_s;
      std::vector<double> phiCoor_s;
      std::vector<double> x_glob;
      std::vector<double> y_glob;
      std::vector<double> z_glob;


      matchedSensors.assign(42,0);
      rCoor.assign(42,0);
      phiCoor.assign(42,0);
      rCoor_s.assign(84,0);
      phiCoor_s.assign(84,0);
      x_glob.assign(42,0);
      y_glob.assign(42,0);
      z_glob.assign(42,0);
      

    //Loop over measurements
    const std::vector<LHCb::Measurement*>& measures = track->measurements(); 
    std::vector<LHCb::Measurement*>::const_iterator it = measures.begin();
 
    for ( ; it != measures.end(); ++it ) {
 
      //Get ChannelID belonging to the Velo LHCbID from the measurements
      LHCb::LHCbID id = (*it)->lhcbID();
      if(!id.isVelo()) continue;

      LHCb::VeloChannelID vcID = id.veloID();
      int sensorID = vcID.sensor();
      unsigned int stripID = vcID.strip();

      //UsedSensor plot
      //---------------
      plot1D(sensorID,"UsedSensors","Number of times a sensor was used in the tracking vs Sensors",-0.5, 110.5, 111);
      
      const DeVeloSensor* sensor = m_veloDet->sensor(sensorID);
    
      Gaudi::XYZPoint trackInterceptGlobal= extrapolateToZ(track, sensor->z());
      Gaudi::XYZPoint trackInterceptLocal(0,0,0) ;      
      trackInterceptLocal = sensor->globalToLocal(trackInterceptGlobal);
      double interceptRadius = trackInterceptLocal.Rho();
      double interceptAngle = trackInterceptLocal.Phi();  
      
      LHCb::VeloCluster* cluster = m_clusters->object(vcID);
      
      SiPositionInfo<LHCb::VeloChannelID> toolInfo;
      toolInfo=m_clusterTool->position(cluster);
      double interStripFr = toolInfo.fractionalPosition; 
      double biasedResid;
      double chi2;
      sensor->residual(trackInterceptGlobal, vcID, interStripFr, biasedResid, chi2);
      double pitch;
      if ( sensor->isR() ) {
        pitch = sensor->rType()->rPitch( stripID );
      }
      else {
        pitch = sensor->phiType()->phiPitch( interceptRadius );
      }
      
      //Sensors vs Biased Residuals profile only for Velo
      //------------------------------------------------- 
      if(m_biasedResidualProfile){
        if ( 0 != pitch ) {
          prof_sensors -> fill (sensor->sensorNumber(), m_binary * biasedResid / pitch ); 
        }
        else {
          warning()<< "Pitch is " << pitch << " for sensorID " << sensorID << endmsg;
        }
      }
      else{
        if (m_debugLevel){
          debug()<< "Profile for biased residual is booked but not filled"<<endmsg;
        }
      }  
      //State Parameters
      //----------------
      const DeVeloRType* rSensor = m_veloDet->rSensor(sensorID);
      const DeVeloPhiType* phiSensor = m_veloDet->phiSensor(sensorID);
        
      if (sensor->isR()) {
        nRClus++;        
        matchedSensors[sensorID] += 1;
        rCoor[sensorID] = rSensor->rOfStrip(stripID,interStripFr);
        phiCoor_s[2*sensorID] = interceptAngle;
        rCoor_s[2*sensorID] = interceptRadius;
      }
      else if (sensor->isPhi()){
        nPhiClus++;
        sensorID = sensorID-64;//Phi sensors: 64->105
        matchedSensors[sensorID] += 10;
        phiCoor[sensorID] = phiSensor -> phiOfStrip(stripID,interStripFr,interceptRadius);
        phiCoor_s[2*sensorID+1] = interceptAngle;
        rCoor_s[2*sensorID+1] = interceptRadius;
      }
      
        
    }//end of for loop over measurements
    
    //Loop over module
    //----------------------------------------------------------
    for (int i=0;i<=41;i++) {
      if((matchedSensors[i] == 1 || matchedSensors[i] == 10)&& matchedSensors[i] != 11){
        plot1D(i,"ModuleMismatch","Tracks with one used sensor from a module vs Module number"
               ,-0.5, 43.5, 44);
      }
      
      //Hit Map plots at R sensor position from Marco's Alignment code
        //-----------------------------------------------------------
      if(matchedSensors[i] != 11) continue;
      
      double correct_phi = 0;

      if( m_ACDC ) {
        correct_phi = -phiCoor[i] + phiCoor_s[2*i] + phiCoor_s[2*i+1];//ACDC Geometry
        plot1D((phiCoor_s[2*i] + phiCoor_s[2*i+1])/degree, "PhiDiff", "PhiDiff", -2., 2., 100);
      }
      else {
        correct_phi = phiCoor[i] + phiCoor_s[2*i] - phiCoor_s[2*i+1];//DC06 Geometry
        plot1D((phiCoor_s[2*i] - phiCoor_s[2*i+1])/degree, "PhiDiff", "PhiDiff", -2., 2., 100);
      }

      plot1D(rCoor[i]/mm, "RCoord", "R coordinate(mm,log)", 0, 50, 500);
      
      plot1D(correct_phi/degree, "LocalPhiCoord", "Local Phi Coordinate(degree)", -200 , 200, 200);

      if(m_hitmapHistos){ 
        
        const DeVeloSensor* sensor = m_veloDet->sensor(i);
        
          double x_loc = rCoor[i] * cos(correct_phi);
          double y_loc = rCoor[i] * sin(correct_phi);   
          
          Gaudi::XYZPoint pointLocal(x_loc, y_loc, 0);
          Gaudi::XYZPoint pointGlobal(0, 0, 0);
          pointGlobal = sensor->localToGlobal (pointLocal);
          
          x_glob[i] = pointGlobal.x();
          y_glob[i] = pointGlobal.y();
          z_glob[i] = pointGlobal.z();
          
          plot2D(x_glob[i], y_glob[i], "XY_HitMap", "X(mm) vs Y(mm) hitmap", -75, 75, -45, 45, 100, 100);
          plot2D(z_glob[i], x_glob[i], "ZX_HitMap", "Z(mm) vs X(mm) hitmap", -200, 800, -75, 75, 100, 100); 
      }
    }//for loop on module
    
    nSumClus = nRClus +  nPhiClus;
    
    //No of R and total Clusters per track 
    //------------------------------------
    prof_thetaR -> fill (theta/degree, nRClus);
    prof_thetaTot -> fill (theta/degree, nSumClus);
    
    nRClusEvent += nRClus;
    nPhiClusEvent += nPhiClus;
    
  }//End of loop over track container
  
  if(m_EventClusterInfo){
    
    nSumClusEvent =  nRClusEvent + nPhiClusEvent;
    
    //No of R Clusters per Event 
    //--------------------------
    plot1D(nRClusEvent, "# R clusters", "Number of R-Clusters per event", -0.5, 2000.5, 2001 );
    
    //No of R+Phi Clusters per Event 
    //------------------------------
    plot1D(nSumClusEvent, "# R+Phi clusters", "Number of R and Phi Clusters per event", -0.5, 2000.5, 2001 );
    
  }
  
  if( m_alignMoniBasic ){
    // reset histograms
    m_h_aliMon_Mean_R_A->reset();
    m_h_aliMon_Mean_R_C->reset();
    m_h_aliMon_Mean_P_A->reset();
    m_h_aliMon_Mean_P_C->reset();
    m_h_aliMon_Sigma_R_A->reset();
    m_h_aliMon_Sigma_R_C->reset();
    m_h_aliMon_Sigma_P_A->reset();
    m_h_aliMon_Sigma_P_C->reset();
    for ( int bin = 1; bin <= 42; bin++ ) {
      // fill histograms
      if ( 0 == (bin % 2) ) {
        m_h_aliMon_Mean_R_A->fill( prof_sensors->binHeight( bin ) );
        m_h_aliMon_Sigma_R_A->fill( prof_sensors->binError( bin ) );
        m_h_aliMon_Mean_P_A->fill( prof_sensors->binHeight( bin + 64 ) );
        m_h_aliMon_Sigma_P_A->fill( prof_sensors->binError( bin + 64 ) );
      }
      else {
        m_h_aliMon_Mean_R_C->fill( prof_sensors->binHeight( bin ) );
        m_h_aliMon_Sigma_R_C->fill( prof_sensors->binError( bin ) );
        m_h_aliMon_Mean_P_C->fill( prof_sensors->binHeight( bin + 64 ) );
        m_h_aliMon_Sigma_P_C->fill( prof_sensors->binError( bin + 64 ) );
      }
    }
  }
  //PrintInfo:InEvent
  //-----------------
  
  if(m_debugLevel){
      debug() << " total tracks per event:"<< nTracks << endmsg;  
  }
  
  return StatusCode::SUCCESS;
}

//========================================================
// Linear extrapolator from a track to a given z position.
//========================================================
Gaudi::XYZPoint Velo::VeloTrackMonitor::extrapolateToZ(LHCb::Track *track, double toZ)
{
  // get state parameters
  Gaudi::XYZPoint coord = track->position();
  Gaudi::XYZVector slope = track->slopes();
  Gaudi::XYZPoint result;
  double deltaZ=toZ-coord.z();
  result.SetXYZ(coord.x()+slope.x()*deltaZ, coord.y()+slope.y()*deltaZ, toZ);
  
  return result;
}

//=========================================================================
//Unbiased Residuals
//=========================================================================
StatusCode Velo::VeloTrackMonitor::unbiasedResiduals (LHCb::Track *track ) 
{
  if(track->checkFitHistory(Track::BiKalman) == true){
    
    //book profile for sensors vs unbiasedResiduals
    
    const GaudiAlg::HistoID sensors = "Sensors_vs_UnbiasedResiduals" ; 
    AIDA::IProfile1D* prof_sensors = bookProfile1D
      (sensors, "Sensors vs UnbiasedResiduals(um)" , -0.5, 139.5, 140) ;
    if ( m_event ==  m_resetProfile ) { prof_sensors -> reset() ; } // RESET AFTER CERTAIN EVENTS
    
    //Loop over nodes
    const std::vector<LHCb::Node*>& nodes = track->nodes(); 
    std::vector<LHCb::Node*>::const_iterator it = nodes.begin();
    
    for ( ; it != nodes.end(); ++it ) {
      
      FitNode& fitnode =  dynamic_cast<FitNode&>(**it);
      if(!fitnode.hasMeasurement()) continue;
      
      //Get ChannelID belonging to the LHCbID from the measured fitnode
      const LHCb::LHCbID& lhcbID = fitnode.measurement().lhcbID();
      if ( lhcbID.detectorType() != LHCbID::Velo ) continue;
      
      LHCb::VeloChannelID vcID = lhcbID.veloID();
      LHCb::VeloCluster* cluster = m_clusters->object(vcID);
      int sensorID = vcID.sensor();
      const DeVeloSensor* sensor = m_veloDet->sensor(sensorID);
      
      // get Unbiased State
      const State state = fitnode.unbiasedState();
      Gaudi::XYZPoint trackInterceptGlobal(state.x(),state.y(),state.z());
      Gaudi::XYZPoint trackInterceptLocal(0,0,0) ;      
      trackInterceptLocal = sensor->globalToLocal(trackInterceptGlobal);
      double interceptRadius = trackInterceptLocal.Rho();
      
      SiPositionInfo<LHCb::VeloChannelID> toolInfo;
      toolInfo=m_clusterTool->position(cluster);
      double interStripFr = toolInfo.fractionalPosition; 
      double UnbiasedResid;
      double chi2;
      
      //Sensors vs Unbiased Residuals profile
      sensor->residual(trackInterceptGlobal, vcID, interStripFr, UnbiasedResid, chi2);
      double pitch;
      if ( sensor->isR() ) {
        unsigned int stripID = vcID.strip();
        pitch = sensor->rType()->rPitch( stripID );
      }
      else {
        pitch = sensor->phiType()->phiPitch( interceptRadius );
      }
      
      if ( 0 != pitch ) {
        prof_sensors -> fill (sensor->sensorNumber(), m_binary * UnbiasedResid / pitch );   
      }
      else {
        warning()<< "Pitch is " << pitch << " for sensorID " << sensorID << endmsg;
      }
        
    }//end of fit node
  }//if fit is BiKalman
  
  else{ 
    if(m_debugLevel){
      debug() << "No BiKalman fit history could be retrieved. Terminating Unbiased Residual method" << endmsg;
      }
  }
  return StatusCode::SUCCESS;
}




