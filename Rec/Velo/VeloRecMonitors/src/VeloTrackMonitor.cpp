// $Id: VeloTrackMonitor.cpp,v 1.17 2009-07-02 15:12:56 siborghi Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiAlg/GaudiTupleAlg.h"

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
    //  : GaudiTupleAlg( name , pSvcLocator ),

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
  declareProperty( "AlignNMeasMin", m_alignNMeasMin = 6);
  declareProperty( "ACDCGeometry", m_ACDC = true);
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
    // online
    if ( context() != "Offline" ) {
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
    // offline
    if ( context() == "Offline" ) {
      char hname[100];
      for ( int i = 0; i < 84; i++ ) {
        sprintf( hname, "m_prof_res_%d", i );
        const GaudiAlg::HistoID sensors = hname;
        m_prof_res[ i ] = bookProfile1D( sensors, "residual vs phi" , -210., 210., 42) ;
      }
    }
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
  bool moniTracks = true;
  StatusCode sc = getVeloTracks();
  if( sc.isFailure() ) moniTracks = false;
  sc = getVeloClusters();
  if( sc.isFailure() ) moniTracks = false;
  
  //Monitor the Tracks
  //------------------
  if( moniTracks ) {
    sc = monitorTracks();  
    if( sc.isFailure() ) {
      return StatusCode::SUCCESS;
    }
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

    m_rawClusters = get<LHCb::VeloClusters>( m_clusterCont );

    if ( m_debugLevel ) debug() << "  -> number of clusters found in TES: "
                                << m_clusters->size()<<endmsg;
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
    (sensors, "Sensors vs BiasedResiduals" , -0.5, 139.5, 140) ;
  if ( ( context() == "Online" ) &&  (m_event == m_resetProfile) ) { prof_sensors -> reset() ; } // RESET AFTER CERTAIN EVENTS

      
  //book profile for momentum of +ve tracks vs biasedResiduals
  const GaudiAlg::HistoID pos_momentum = "PosMomentum_vs_BiasedResiduals" ; 
  AIDA::IProfile1D* prof_pos_mom_res = bookProfile1D
    (pos_momentum, "PosTracksMomentum(GeV) vs BiasedResiduals" , -0.5, 100.5, 20) ;
  if ( (context() == "Online") &&  (m_event == m_resetProfile) ) { prof_pos_mom_res -> reset() ; } // RESET AFTER CERTAIN EVENTS

  //book profile for momentum of -ve tracks vs biasedResiduals 
  const GaudiAlg::HistoID neg_momentum = "NegMomentum_vs_BiasedResiduals" ; 
  AIDA::IProfile1D* prof_neg_mom_res = bookProfile1D
    (neg_momentum, "NegTracksMomentum(GeV) vs BiasedResiduals" , -0.5, 100.5, 20) ;
  if ( (context() == "Online") &&  (m_event == m_resetProfile) ) { prof_neg_mom_res -> reset() ; } // RESET AFTER CERTAIN EVENTS  
  
  
  //book profile for polarAngle vs RClusters
  const GaudiAlg::HistoID thetaR = "RClusters_per_Track_vs_Theta" ; 
  AIDA::IProfile1D* prof_thetaR = bookProfile1D
    (thetaR, " Clusters vs Theta(degree) " , -0.5, 30.5, 31) ;
  if ( (context() == "Online") &&  (m_event == m_resetProfile) ) { prof_thetaR -> reset() ; } // RESET AFTER CERTAIN EVENTS
  
  //book profile for polarAngle vs TotClusters
  const GaudiAlg::HistoID thetaTot = "TotClusters_per_Track_vs_Theta" ; 
  AIDA::IProfile1D* prof_thetaTot = bookProfile1D
    (thetaTot, "Number of measurements vs #theta(degree)" , -0.5, 30.5, 31) ;
  if ( (context() == "Online") &&  (m_event == m_resetProfile) ) { prof_thetaTot -> reset() ; } // RESET AFTER CERTAIN EVENTS

 //book profile for polarAngle vs Measurements
  const GaudiAlg::HistoID thetaMeas = "Measurements_per_Track_vs_Theta" ; 
  AIDA::IProfile1D* prof_thetaMeas = bookProfile1D
    (thetaMeas, "Number of measurements vs #theta(degree)" , -0.5, 30.5, 31) ;
  if ( (context() == "Online") &&  (m_event == m_resetProfile) ) { prof_thetaMeas -> reset() ; } // RESET AFTER CERTAIN EVENTS


//book profile for Pseudoefficiency_sensor vs sensor
  const GaudiAlg::HistoID pseudoEffsens = "Pseudoefficiency_per_sensor_vs_sensorID";
  AIDA::IProfile1D* prof_pseudoEffsens = bookProfile1D
    (pseudoEffsens, "Pseudoefficiency vs sensorID",-0.5, 110.5, 111);
  if ( (context() == "Online") &&  (m_event == m_resetProfile) ) { prof_pseudoEffsens -> reset() ; } // RESET AFTER CERTAIN EVENTS


    std::vector<float> N_exp;    
    std::vector<float> N_rec;
    std::vector<double> pseudoEfficiency_sens;

      for(int i=0; i<105; i++){

        N_rec.push_back(0.);
        N_exp.push_back(0.);
        pseudoEfficiency_sens.push_back(0.);
      }
    
//Loop over track container
  LHCb::Tracks::const_iterator itTrk;
  
  for(itTrk=m_tracks->begin();itTrk!=m_tracks->end();itTrk++){

    nTracks++;
    LHCb::Track* track = (*itTrk);
    
    //Call Measurement Provider
    if(!track -> checkPatRecStatus(Track::PatRecMeas)){
      if(m_debugLevel)debug() <<"No measurements found on tracks, calling measurement provider"<<endmsg;
      StatusCode sc1 = m_measurementprovider->load(*track); 
      if ( !( sc1.isSuccess() ) ) {
        debug() << "Measurement provider could not be loaded" << endmsg;
        return StatusCode::SUCCESS;
      }
    }

    //Skip the tracks without VELO(Velo, VeloR, Upstream and Long)hits
    if ( !track->hasVelo() ) {
      if (m_debugLevel) debug() <<"Track has no VELO hits, continuing with next track."<< endmsg;
      continue;
    }
    
    //General track properties
      if (m_debugLevel){
        debug() << "ErrorFlag:" << track->flag() << endmsg;
        debug() << "Track Type:" << track->type() << endmsg;
        debug() << "Track History:"<< track->history() << endmsg;
        if( m_trackCont != LHCb::TrackLocation::Velo)
          debug() << "Momentum(GeV):" << track->p()/GeV << endmsg;
        debug() << "Chi2/DoF per Track:" << track->chi2PerDoF() << endmsg;
        debug() << "Tracks per container:" << m_tracks->size() << endmsg;
      }
      //theta, phi& p  plots
      //---------------------------
      if( m_xPlots){
        theta = track->firstState().slopes().theta();
        plot1D(theta/degree, "theta(degree)", -0.5, 50.5, 51);
        plot1D(track->phi()/degree, "Azimuth:phi (degree)", -200., 200., 100);
      }
      
      //eta
      //---
      plot1D(track->pseudoRapidity(), "eta", 0.95, 6.05, 51);

      //pseudo-efficiency: Using Velo expectation Tool
      //--------------------------------------------
      int nExpectedHits = m_expectTool->nExpected(*track);
      
      //unsigned int nVeloHits = track->nLHCbIDs(); 
      const std::vector<LHCb::LHCbID>& id = track->lhcbIDs();
      const int nVeloHits = std::count_if(id.begin(), id.end(),bind(&LHCbID::isVelo,_1));
      if(nExpectedHits != 0){
        double pseudoEfficiency = double(nVeloHits)/double(nExpectedHits);
        plot1D(pseudoEfficiency, "pseudoEfficiency", "PseudoEfficiency", -0.05, 1.55, 16);
      }

      //Unbiased Residuals
      //------------------
      if(m_unbiasedResidualProfile &&  m_trackCont != LHCb::TrackLocation::Velo){  
        
        StatusCode sc2 = unbiasedResiduals (track);
        if ( !( sc2.isSuccess() ) ) {
          debug() << "Unbiased residuals method" << endmsg;
          return StatusCode::SUCCESS;
        }
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
       
       //Loop over sensors
         for(int j=0; j<=105; j++){
           if(j>=0 && j<=41 || j>=64 && j<=105){
             debug()<<"sensor number "<<j<<endmsg;
             bool nExpectedHits_sensor = m_expectTool -> isInside(*track, j);
             if(nExpectedHits_sensor == true) {
               N_exp[j]+=1;
             }
             debug()<<"==> N_exp"<<N_exp[j]<<endmsg;
          }
       }//end of loop over sensors
      

//Loop over measurements
    const std::vector<LHCb::Measurement*>& measures = track->measurements(); 
    std::vector<LHCb::Measurement*>::const_iterator it = measures.begin();
    unsigned int n_measurements = measures.size();
    int charge = track->charge();  
    
    //if(charge == 0)
      plot1D( n_measurements, "NMeasurements", "Number of measurements associated to a track", 0, 45, 45 );
   
      // else if(charge == 1){
      // plot1D( n_measurements, "NPosMeasurements", "Number of measurements by positive tracks", 0, 100, 100 );
      // }
      // else if(charge == -1){
      // plot1D( n_measurements, "NNegMeasurements", "Number of measurements by negative tracks", 0, 100, 100 );
      // }
      
    
    for ( ; it != measures.end(); ++it ) {
 
      //Get ChannelID belonging to the Velo LHCbID from the measurements
      LHCb::LHCbID id = (*it)->lhcbID();
      if(!id.isVelo()) continue;

      LHCb::VeloChannelID vcID = id.veloID();
      int sensorID = vcID.sensor();
      unsigned int stripID = vcID.strip();
      const DeVeloSensor* sensor = m_veloDet->sensor(sensorID); 

      N_rec[sensorID]+=1;

      debug()<<"N_rec "<<N_rec[sensorID]<<endmsg;


      //UsedSensor plot
       //---------------
      if ( sensor->isR() || sensor->isPhi() ) {
        plot1D(sensorID,"UsedSensors","Number of times a sensor was used in the tracking vs Sensors",-0.5, 110.5, 111);
      }
      
      Gaudi::XYZPoint trackInterceptGlobal= extrapolateToZ(track, sensor->z());
      Gaudi::XYZPoint trackInterceptLocal(0,0,0) ;      
      trackInterceptLocal    = sensor -> globalToLocal( trackInterceptGlobal );
      double interceptRadius = trackInterceptLocal.Rho();
      double interceptAngle  = trackInterceptLocal.Phi();  
      
      LHCb::VeloCluster* cluster = m_clusters->object(vcID);
      
      SiPositionInfo<LHCb::VeloChannelID> toolInfo;
      toolInfo=m_clusterTool->position(cluster);
      double interStripFr = toolInfo.fractionalPosition; 
      double biasedResid;
      double chi2;
      StatusCode sc3 = sensor -> residual( trackInterceptGlobal, vcID,
                                           interStripFr, biasedResid, chi2 );
      if ( !( sc3.isSuccess() ) ) {
        debug() << "Residual calculation failed for " 
                << trackInterceptGlobal.x() << " "
                << trackInterceptGlobal.y() << " "
                << trackInterceptGlobal.z() << " "
                << vcID << " "
                << interStripFr << " "
                << endmsg;
        continue;
      }

      double pitch;
      if ( sensor->isR() ) {
        pitch = sensor->rType()->rPitch( stripID );
      }
      else {
        pitch = sensor->phiType()->phiPitch( interceptRadius );
      }

      if ( ( m_alignMoniBasic ) // offline alignment monitoring
        && ( context() == "Offline" ) 
        && ( n_measurements >= m_alignNMeasMin ) ) {
        unsigned int n_sen = sensor->sensorNumber();
        if ( sensor->isR() ) {
          m_prof_res[ n_sen ]->fill( interceptAngle / degree, biasedResid / um );
        }
        else {
          double phi_state = 0.;
          if ( sensor->phiType()->zoneOfStrip( stripID ) == 0 ) { // inner
            phi_state = sensor->phiType()->phiOfStrip( stripID, interStripFr, sensor->phiType()->rMin( 0 ) ) / degree;
            phi_state += 20.;                                 // stereo angle for inner strips
          }
          else {                                               // outer
            phi_state = sensor->phiType()->phiOfStrip( stripID, interStripFr, sensor->phiType()->rMin( 1 ) ) / degree;
            phi_state += -10.35;                                // stereo angle for outer strips
          }
          m_prof_res[ n_sen - 22 ]->fill( phi_state, biasedResid / um ); // phi sensor numbering is 64-105, filled into 42-83
        }
      }
      
      //Biased Residuals
      //----------------
      if(m_biasedResidualProfile){
        if ( 0 != pitch ) {
          //Sensors vs Biased Residuals profile only for Velo tracks
          prof_sensors -> fill (sensor->sensorNumber(), m_binary * biasedResid / pitch );
          
          //Momentum vs Biased Residuals profiles for +ve and -ve charges from Forward tracks with VeloHits        
          if(m_trackCont == LHCb::TrackLocation::Forward && charge  == 1) 
            prof_pos_mom_res -> fill (track->p() /GeV, m_binary * biasedResid / pitch );   
          
          else if(m_trackCont == LHCb::TrackLocation::Forward && charge == -1)
            prof_neg_mom_res -> fill (track->p() /GeV, m_binary * biasedResid / pitch );
          
          if (m_trackCont != LHCb::TrackLocation::Forward && m_debugLevel)
            debug()<<"Profiles for biased residual vs momentum are not filled: m_trackCont != Rec/Track/Forward"<<endmsg;
        }//end pitch
        else warning()<< "Pitch is" << pitch << " for sensorID " << sensorID << endmsg;
        
      }//if m_biasedResidual
      else{
        if(m_debugLevel)
          debug()<< "Profiles for biased residual are not filled: m_biasedResidual == False" << endmsg;
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
      
    
    }//end of loop over measurements



     //Cluster associated to a track
    StatusCode sc4 = fill_clusteradcsum (m_tracks);
    if ( !( sc4.isSuccess() ) ) {
      debug() << "fill_clusteradcsum method" << endmsg;
      return StatusCode::SUCCESS;
    }     
    
    //Loop over module
    //----------------------------------------------------------
    for (int i=0;i<=41;i++) {
      if((matchedSensors[i] == 1 || matchedSensors[i] == 10) && matchedSensors[i] != 11){
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
      
      
      plot1D( rCoor[i]/mm, "RCoord", "R coordinate(mm)", 0, 50, 500 );
      
      plot1D( correct_phi/degree,
              "LocalPhiCoord", "Local Phi Coordinate(degree)",
              -100 , 100, 100 );
      
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
          // plot2D(z_glob[i], y_glob[i], "ZY_HitMap", "Z(mm) vs Y(mm) hitmap", -200, 800, -75, 75, 100, 100); 
      }
      
    }//for loop on module
    
    nSumClus = nRClus +  nPhiClus;
    
    //No of R and total Clusters per track 
    //------------------------------------
    prof_thetaR -> fill (theta/degree, nRClus);
    prof_thetaTot -> fill (theta/degree, nSumClus);

    prof_thetaMeas -> fill (theta/degree, n_measurements);
    
    
    nRClusEvent += nRClus;
    nPhiClusEvent += nPhiClus;
    
  }//End of loop over track container

    //Pseudoefficiency for each sensor
    //--------------------------------

      for(int i=0; i<=105; i++){
        if(i>=0 && i<=41 || i>=64 && i<=105){

          debug()<<"N_rec "<<N_rec[i]<<endmsg;
          debug()<<"N_exp "<<N_exp[i]<<endmsg;

          if(N_exp[i]>0){
            pseudoEfficiency_sens[i] = N_rec[i] / N_exp[i];
            debug()<<"==> pseudoefficiency "<<pseudoEfficiency_sens[i]<<endmsg;
            plot2D(i, pseudoEfficiency_sens[i], "Pseudoefficiency vs sensorID", "Pseudoefficiency vs sensorID", 
                 -0.5, 110.5, -0.05, 1.55, 111, 16);
            prof_pseudoEffsens -> fill(i, pseudoEfficiency_sens[i]);
          }
        }
      }
    
if(m_EventClusterInfo){
    
    nSumClusEvent =  nRClusEvent + nPhiClusEvent;
    
    //No of R Clusters per Event 
    //--------------------------
    plot1D( nRClusEvent, "# R clusters", "Number of R-Clusters per event",
            0., 2000, 400 );
    
    //No of R+Phi Clusters per Event 
    //------------------------------
    plot1D( nSumClusEvent,
            "# R+Phi clusters", "Number of R and Phi Clusters per event",
            0., 2000, 400 );
    
  }
  
  // online alignment monitoring
  if( ( m_alignMoniBasic )
        && ( context() != "Offline" ) ) {
    error() << "Filling online context" << endmsg;
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
      debug() << "Start Unbiased Residual method" << endmsg;    
    //book profile for sensors vs unbiasedResiduals
    
    const GaudiAlg::HistoID unsensors = "Sensors_vs_UnbiasedResiduals" ; 
    AIDA::IProfile1D* prof_unsensors = bookProfile1D
      (unsensors, "Sensors vs UnbiasedResiduals" , -0.5, 139.5, 140) ;
    if ( m_event ==  m_resetProfile ) { prof_unsensors -> reset() ; } // RESET AFTER CERTAIN EVENTS
    
    //book profile for momentum of +ve tracks vs biasedResiduals
    const GaudiAlg::HistoID pos_momentum = "PosMomentum_vs_UnBiasedResiduals" ; 
    AIDA::IProfile1D* prof_pos_mom_res = bookProfile1D
      (pos_momentum, "PosTracksMomentum(GeV) vs UnBiasedResiduals" , -0.5, 100.5, 20) ;
    if ( (context() == "Online") &&  (m_event == m_resetProfile) ) { prof_pos_mom_res -> reset() ; } // RESET AFTER CERTAIN EVENTS
    
    //book profile for momentum of -ve tracks vs biasedResiduals 
    const GaudiAlg::HistoID neg_momentum = "NegMomentum_vs_UnBiasedResiduals" ; 
    AIDA::IProfile1D* prof_neg_mom_res = bookProfile1D
      (neg_momentum, "NegTracksMomentum(GeV) vs UnBiasedResiduals" , -0.5, 100.5, 20) ;
    if ( (context() == "Online") &&  (m_event == m_resetProfile) ) { prof_neg_mom_res -> reset() ; } // RESET AFTER CERTAIN EVENTS  
    
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
      StatusCode sc3 = sensor->residual(trackInterceptGlobal, vcID, interStripFr, UnbiasedResid, chi2);
      if ( !( sc3.isSuccess() ) ) {
        debug() << "Residual calculation failed for "
                << trackInterceptGlobal.x() << " "
                << trackInterceptGlobal.y() << " "
                << trackInterceptGlobal.z() << " "
                << vcID << " "
                << interStripFr << " "
                << endmsg;
        continue;
      }
      double pitch;
      if ( sensor->isR() ) {
        unsigned int stripID = vcID.strip();
        pitch = sensor->rType()->rPitch( stripID );
      }
      else {
        pitch = sensor->phiType()->phiPitch( interceptRadius );
      }

      if ( 0 != pitch ) {
        //Sensors vs Unbiased Residuals
        prof_unsensors -> fill (sensor->sensorNumber(), m_binary * UnbiasedResid / pitch );
        
        //Momentum vs UnBiased Residuals profiles for +ve and -ve charges for Velo part of Forward tracks        
        if(m_trackCont == LHCb::TrackLocation::Forward && track->charge()  == 1)
          prof_pos_mom_res -> fill (track->p()/GeV,  m_binary * UnbiasedResid / pitch );   
        
        else if(m_trackCont == LHCb::TrackLocation::Forward && track->charge() == -1)
          prof_neg_mom_res -> fill (track->p()/GeV,  m_binary * UnbiasedResid / pitch );   
        
        if (m_trackCont != LHCb::TrackLocation::Forward && m_debugLevel)
          debug()<<"Profiles for unbiased residual vs momentum are not filled: m_trackCont != Rec/Track/Forward"<<endmsg; 
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

//=========================================================================
// ADCsum for cluster associated to a track
//=========================================================================
StatusCode Velo::VeloTrackMonitor::fill_clusteradcsum(LHCb::Tracks* tracks)
{

  debug() << "Ntuple on event : " << endmsg;

  LHCb::Tracks::const_iterator itTrg;
  for(itTrg=tracks->begin();itTrg!=tracks->end();itTrg++){

    LHCb::Track* track = (*itTrg);

   const std::vector< LHCb::LHCbID >& trackIDs = track->lhcbIDs();
    std::vector< LHCb::LHCbID >::const_iterator it;

    for ( it = trackIDs.begin(); it != trackIDs.end(); it++ ) {
      LHCb::VeloCluster *cluster;
      cluster = (LHCb::VeloCluster*)m_rawClusters->containedObject( (it)->channelID() );
      double sumadc = cluster ->totalCharge();
      plot1D(sumadc, "adcsum", "adcsum", 0 ,100 , 20);
        
      if(cluster->isRType()){
        double Radc = cluster ->totalCharge();
        plot1D(Radc, "Radc", "ADC for R clusters associated to a track", 0 ,100 , 100);
      }  
      if(cluster->isPhiType()){
        double Phiadc = cluster ->totalCharge();
        plot1D(Phiadc, "Phiadc", "ADC for Phi clusters associated to a track", 0 ,100 , 100);
      }
    }	
    
  }
  
  return StatusCode::SUCCESS;

}	  
