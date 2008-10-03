// $Id: VeloTrackMonitor.cpp,v 1.1 2008-10-03 12:25:12 erodrigu Exp $
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
#include "Event/VeloCluster.h"

// from AIDA
#include "AIDA/IProfile1D.h"

// local
#include "VeloTrackMonitor.h"

using ROOT::Math::SMatrix;
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
  declareProperty( "ClusterLocation",   m_clusterCont = LHCb::VeloClusterLocation::Default );
  declareProperty( "TrackLocation",     m_trackCont = LHCb::TrackLocation::Velo );
  declareProperty( "HitMapHistos",      m_hitmapHistos = false );
  declareProperty( "UnbiasedResiduals", m_unbiasedResidualProfile = false );
  declareProperty( "BiasedResiduals",   m_biasedResidualProfile = true );
  declareProperty( "EventNumber",       m_event );
  declareProperty( "ProfileResetEvent", m_resetProfile );
  
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
  if( !( sc1.isSuccess() && sc2.isSuccess() ) ) {
    return StatusCode::SUCCESS;
  }
  
  //Monitor the Tracks
  //------------------
  monitorTracks();

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
  unsigned int nTracks = m_tracks->size(); //Number of Tracks per Event
  unsigned int nRClusEvent (0), nPhiClusEvent (0), nSumClusEvent (0); //Number of R, Phi and total clusters in tracks per Event
  
  //Loop over track container
  LHCb::Tracks::const_iterator itTrk;
  
  for(itTrk=m_tracks->begin();itTrk!=m_tracks->end();itTrk++){

    nTracks++;
    LHCb::Track* track = (*itTrk);
    
    //Call Measurement Provider if tracks have no measurements
    if(track->nMeasurements() == 0){
      if(m_debugLevel)debug() <<"No measuremnets found on tracks, calling measurement provider"<<endmsg;
      m_measurementprovider->load(*track); 
    }

    //Skip the tracks without VELO hits
    if ( !track->hasVelo() ) {
      if (m_debugLevel)
        debug() <<"Track has no VELO hits, continuing with next track."<< endmsg;
      continue;
    }

    //General track properties
    if (m_debugLevel){
      debug()<< "ErrorFlags:"<< track->flag()<<endmsg;
      debug()<< "Chi2/DoF per Track:" << track->chi2PerDoF()<<endmsg;
      debug()<< "Track type:"<<m_tracks->size()<<endmsg;
    }
    
    //eta plot
    //-------
    plot1D(track->pseudoRapidity(), "eta", 0.95, 6.05, 51);

    //psudo-effeciency: Using Velo expectation Tool
    //--------------------------------------------
    int nExpectedHits = m_expectTool->nExpected(*track);

    //unsigned int nVeloHits = track->nLHCbIDs(); 
    const std::vector<LHCb::LHCbID>& id = track->lhcbIDs();
    const int nVeloHits = std::count_if(id.begin(), id.end(),bind(&LHCbID::isVelo,_1));
    double pseudoEfficiency = double(nVeloHits)/double(nExpectedHits);

    plot1D( pseudoEfficiency, "pseudoEfficiency", "pseudoEfficiency",
            -0.05, 1.55, 16 );

    if(m_unbiasedResidualProfile){
      unbiasedResiduals(track);
    }
    
    std::vector<int> matchedSensors;
    matchedSensors.assign(140,1);

    unsigned int nRClus(0), nPhiClus(0);
     
    //book profile for sensors vs biasedResiduals
    const GaudiAlg::HistoID sensors = "BiasedResiduals_vs_Sensors" ; 
    AIDA::IProfile1D* prof_sensors = bookProfile1D
      (sensors, "BiasedResiduals(mm) vs Sensors" , -0.5, 139.5, 140) ;
    if ( m_event == m_resetProfile ) { prof_sensors -> reset() ; } // RESET AFTER CERTAIN EVENTS

    //Loop over measurements
    const std::vector<LHCb::Measurement*>& measures = track->measurements(); 
    std::vector<LHCb::Measurement*>::const_iterator it = measures.begin();
 
    for ( ; it != measures.end(); ++it ) {
 
      //Get ChannelID belonging to the Velo LHCbID from the measurements
      LHCb::LHCbID id = (*it)->lhcbID();
      if(!id.isVelo()) continue;

      LHCb::VeloChannelID vcID = id.veloID();
      int sensorID = vcID.sensor();
      int stripID = vcID.sensor();

      //UsedSensor plot
      //---------------
      plot1D(sensorID,"UsedSensors","Number of times a sensor was used in the tracking vs Sensors",-0.5,110.5,110);
      
      matchedSensors[sensorID] = 0;

      const DeVeloSensor* sensor = m_veloDet->sensor(sensorID);
    
      Gaudi::XYZPoint trackInterceptGlobal= extrapolateToZ(track, sensor->z());
      Gaudi::XYZPoint trackInterceptLocal(0,0,0) ;      
      trackInterceptLocal = sensor->globalToLocal(trackInterceptGlobal);
      double interceptRadius = trackInterceptLocal.Rho();
      
      LHCb::VeloCluster* cluster = m_clusters->object(vcID);

      SiPositionInfo<LHCb::VeloChannelID> toolInfo;
      toolInfo=m_clusterTool->position(cluster);
      double interStripFr = toolInfo.fractionalPosition; 
      double biasedResid;
      double chi2;

      //Sensors vs Biased Residuals profile
      //-----------------------------------
      sensor->residual(trackInterceptGlobal, vcID, interStripFr, biasedResid, chi2);

      if(m_biasedResidualProfile == true){
        prof_sensors -> fill (sensor->sensorNumber(), biasedResid);  
      }  
        else{
           if(m_debugLevel) debug()<<"Biased Residual option is off. Profile booked but not available"<<endmsg;
        }  
      
      
      double rCoor(0.),phiCoor(0.); //Global r and phi Coordinates
      
      if (sensor->isR()) {
        nRClus++;    
        const DeVeloRType* rSensor = m_veloDet->rSensor(sensorID);        
        rCoor = rSensor->rOfStrip(stripID,interStripFr);
      }

      else if(sensor->isPhi()){
        nPhiClus++;
        const DeVeloPhiType* phiSensor = m_veloDet->phiSensor(sensorID);
       
        double phiLocal = phiSensor->phiOfStrip(stripID,interStripFr,interceptRadius);
       
        phiCoor = phiSensor->idealPhi(stripID, interStripFr, interceptRadius);

        //Plot of localPhi Coord of strip.
        //----------------------
        plot1D(phiLocal/degree,"ClusterInTrackLocalPhi", "Local Phi of clusters in tracks", -100,100,200);
        
      }
      
      //Hit Map plots
      //--------------------
      if(m_hitmapHistos){
        
        double x_pos = rCoor * cos(phiCoor);
        double y_pos = rCoor * sin(phiCoor);
        double z_pos = sensor->z()/cm;
        
        plot2D(x_pos, y_pos, "XY_HitMap", "X(mm) vs Y(mm) hitmap", -5,15,-2,2,20,4);
        plot2D(x_pos, z_pos, "XZ_HitMap", "X(mm) vs Z(cm) hitmap", -10.,20.,20.,80.,30,60);
      }

    }//end of fit node
    
    //Check if one of the sensor from a module is used in tracks
    //----------------------------------------------------------
    for (int i=0;i<=41;i++) {
      if(matchedSensors[i] != matchedSensors[i+64]){
        plot1D(i,"ModuleMismatch","Tracks with one used sensor from a module vs Module number"
               ,-0.5,43.5,44); 
      }    
    }
    nRClusEvent += nRClus;
    nPhiClusEvent += nPhiClus;  
    
  }//End of loop over track container
    nSumClusEvent =  nRClusEvent + nPhiClusEvent;
    
    //No of R Clusters plot
    //---------------------
    plot1D( nRClusEvent, "# R clusters", "Number of R-Clusters per event", -0.5, 1000.5, 1001 );
    
    //No of R+Phi Clusters plot
    //-------------------------
    plot1D( nSumClusEvent, "# R+Phi clusters", "Number of R and Phi Clusters per event", -0.5, 1000.5, 1001 );
    
    
    //PrintInfo:InEvent
    //-----------------

    if(m_debugLevel){
      
      debug() << " #TotalEventTracks ="<< nTracks << endmsg;  
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
  
  if (m_debugLevel) {             
    
    debug() << "Track# : " 	<< track->key()+1
            << " x : " 	<< coord.x()
            << " y : " 	<< coord.y()
            << " z : " 	<< coord.z()
            << " dx/dz : "	<< slope.x()
            << " dy/dz : "    << slope.y()
            << " dz/dz : "    << slope.z()
            << endmsg;
    
    debug() << "Extrapolation: "
            << " x : " << result.x()
            << " y : " << result.y() 
            << " z : " << result.z() 
            << endmsg;    
  }
  return result;
}

//=========================================================================
//Unbiased Residuals  
//=========================================================================
StatusCode Velo::VeloTrackMonitor::unbiasedResiduals (LHCb::Track* track ) {

  //book profile for sensors vs unbiasedResiduals
  const GaudiAlg::HistoID sensors = "UnbiasedResiduals_vs_Sensors" ; 
  AIDA::IProfile1D* prof_sensors = bookProfile1D
    (sensors, "UnbiasedResiduals(mm) vs Sensors" , -0.5, 139.5, 140) ;
  if ( m_event ==  m_resetProfile ) { prof_sensors -> reset() ; } // RESET AFTER CERTAIN EVENTS
  
  //Loop over nodes
  const std::vector<LHCb::Node*>& nodes = track->nodes(); 
  std::vector<LHCb::Node*>::const_iterator it = nodes.begin();
  
  for ( ; it != nodes.end(); ++it ) {
    
    FitNode& fitnode =  dynamic_cast<FitNode&>(**it);
    if(!fitnode.hasMeasurement()) continue;
    
    //Get ChannelID belonging to the LHCbID from the measured fitnode
    LHCb::VeloChannelID vcID = fitnode.measurement().lhcbID().veloID();
    LHCb::VeloCluster* cluster = m_clusters->object(vcID);
    int sensorID = vcID.sensor();
    const DeVeloSensor* sensor = m_veloDet->sensor(sensorID);

    // get Unbiased State
    const State state = fitnode.unbiasedState();
    Gaudi::XYZPoint trackInterceptGlobal(state.x(),state.y(),state.z());
   
    SiPositionInfo<LHCb::VeloChannelID> toolInfo;
    toolInfo=m_clusterTool->position(cluster);
    double interStripFr = toolInfo.fractionalPosition; 
    double unbiasedResid;
    double chi2;
    
    //Sensors vs Unbiased Residuals profile
    sensor->residual(trackInterceptGlobal, vcID, interStripFr, unbiasedResid, chi2);

    prof_sensors -> fill (sensor->sensorNumber(), unbiasedResid);
    
  }//end of fit node

 return StatusCode::SUCCESS;
}
