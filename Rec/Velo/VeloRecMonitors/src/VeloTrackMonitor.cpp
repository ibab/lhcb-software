// Include files 

// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"

/*
 *  Implementation file for class : Velo::VeloTrackMonitor
 *
 *  @author Zhou Xing
 *  @date   2012-02-03
 *  Low ADC Monitoring histogram added
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
#include "Event/TrackFitResult.h"
#include "Event/VeloPhiMeasurement.h"
#include "Event/VeloRMeasurement.h"
#include "Event/STMeasurement.h"
#include "Event/OTMeasurement.h"
#include "Event/STLiteMeasurement.h"
#include "Event/VeloLitePhiMeasurement.h"
#include "Event/VeloLiteRMeasurement.h"
#include "Event/Particle.h"


// AIDA
#include "AIDA/IProfile1D.h"
#include "AIDA/IHistogram1D.h"

// local
#include "VeloTrackMonitor.h"

// Event
#include "Event/VeloCluster.h"
#include "Event/TrackTypes.h"
#include <iostream>

using namespace Gaudi::Units;
using namespace boost::lambda;
using namespace LHCb;
using namespace Gaudi;

// Declaration of the Algorithm Factory
DECLARE_NAMESPACE_ALGORITHM_FACTORY(Velo, VeloTrackMonitor )

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
  declareProperty( "TrackLocation", m_trackCont = LHCb::TrackLocation::Default);
  declareProperty( "ClusterLocation", m_clusterCont = LHCb::VeloClusterLocation::Default );
  declareProperty( "HitMapHistos", m_hitmapHistos = false);
  declareProperty( "BiasedResiduals", m_biasedResidualProfile = true);
  declareProperty( "UnbiasedResiduals", m_unbiasedResidualProfile = false);
  declareProperty( "EventNumber", m_event);
  declareProperty( "ProfileResetEvent", m_resetProfile);
  declareProperty( "ExtraPlots", m_xPlots = true);
  declareProperty( "EventClusterInfo", m_EventClusterInfo = true);
  declareProperty( "AlignMoniBasic", m_alignMoniBasic = true);
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

  //book profile for sensors vs unbiasedResiduals
  const GaudiAlg::HistoID unsensors = "Sensors_vs_UnbiasedResiduals" ; 
  m_prof_unsensors = bookProfile1D (unsensors, "Sensors vs UnbiasedResiduals" , -0.5, 139.5, 140) ;

  //book profile for momentum of +ve tracks vs biasedResiduals
  const GaudiAlg::HistoID pos_unmomentum = "PosMomentum_vs_UnBiasedResiduals" ; 
  m_prof_pos_mom_unres = bookProfile1D (pos_unmomentum, "PosTracksMomentum(GeV) vs UnBiasedResiduals" , -0.5, 100.5, 20) ;

  //book profile for momentum of -ve tracks vs biasedResiduals 
  const GaudiAlg::HistoID neg_unmomentum = "NegMomentum_vs_UnBiasedResiduals" ; 
  m_prof_neg_mom_unres = bookProfile1D (neg_unmomentum, "NegTracksMomentum(GeV) vs UnBiasedResiduals" , -0.5, 100.5, 20) ;
  
  //book profile for sensors vs biasedResiduals
  const GaudiAlg::HistoID sensors = "Sensors_vs_BiasedResiduals" ; 
  m_prof_sensors = bookProfile1D (sensors, "Sensors vs BiasedResiduals" , -0.5, 139.5, 140) ;
      
  //book profile for momentum of +ve tracks vs biasedResiduals
  const GaudiAlg::HistoID pos_momentum = "PosMomentum_vs_BiasedResiduals" ; 
  m_prof_pos_mom_res = bookProfile1D (pos_momentum, "PosTracksMomentum(GeV) vs BiasedResiduals" , -0.5, 100.5, 20) ;

  //book profile for momentum of -ve tracks vs biasedResiduals 
  const GaudiAlg::HistoID neg_momentum = "NegMomentum_vs_BiasedResiduals" ; 
  m_prof_neg_mom_res = bookProfile1D (neg_momentum, "NegTracksMomentum(GeV) vs BiasedResiduals" , -0.5, 100.5, 20) ;
  
  //book profile for polarAngle vs RClusters
  const GaudiAlg::HistoID thetaR = "RClusters_per_Track_vs_Theta" ; 
  m_prof_thetaR = bookProfile1D (thetaR, "r clusters vs #theta (degree) " , -0.5, 30.5, 31) ;
  
  //book profile for polarAngle vs TotClusters
  const GaudiAlg::HistoID thetaTot = "TotClusters_per_Track_vs_Theta" ; 
  m_prof_thetaTot = bookProfile1D (thetaTot, "Number of Velo measurements vs #theta (degree)" , -0.5, 30.5, 31) ;

  //book profile for Pseudoefficiency_sensor vs sensor
  const GaudiAlg::HistoID pseudoEffsens = "Pseudoefficiency_per_sensor_vs_sensorID";
  m_prof_pseudoEffsens = bookProfile1D (pseudoEffsens, "Pseudoefficiency vs sensorID",-0.5, 110.5, 111);

  if ( m_xPlots) {
    m_track_theta = book1D("Track_Theta", "#theta (degree)", -0.5, 50.5, 51);
    m_track_phi = book1D("Track_Phi", "Azimuth #phi (degree)", -200., 200., 100);
  }
  m_track_eta = book1D("Track_Eta", "PseudoRapidity", 0.95, 6.05, 51);
  m_track_adcsum = book1D("Track_adcsum", "adcsum", 0 ,100 , 100);
  m_track_radc = book1D("Track_radc", "ADC for r clusters associated to a track", 0 ,100 , 100);
  m_track_phiadc = book1D("Track_phiadc", "ADC for #phi clusters associated to a track", 0 ,100 , 100);
  m_track_pseudoeff = book1D("Track_pseudoEfficiency", "PseudoEfficiency", -0.05, 1.55, 16);
  m_track_pseudoeff_int = book1D("Track_pseudoEfficiencyByInterp", "PseudoEfficiency by Interpolation", -0.05, 1.55, 16);
  m_used_sensors = book1D("UsedSensors","Number of times a sensor was used in the tracking vs Sensors",-0.5, 110.5, 111);
  m_phi_diff = book1D("PhiDiff", "PhiDiff", -30., 30., 100);
  m_track_rcoord = book1D("RCoord", "R coordinate(mm)", 0, 50, 100 );
  m_half_phi_a = book1D( "HalfPhiCoordA", "Cluster #phi coordinate (degree) in half frame A", -95 , 95, 100 );
  m_half_phi_c = book1D( "HalfPhiCoordC", "Cluster #phi coordinate (degree) in half frame C", -95 , 95, 100 );
  m_nmeasure = book1D( "NMeasurements", "Number of clusters associated to a track", 0, 45, 45 );
  m_nposmeas = book1D("NPosMeasurements", "Number of clusters associated to a pos. track", 0, 100, 100 );
  m_nnegmeas = book1D("NNegMeasurements", "Number of clusters associated to a neg. track", 0, 100, 100 );
  m_mod_mismatch = book1D("ModuleMismatch","Tracks with one used sensor from a module vs Module number" ,-0.5, 110.5, 111);
  m_nrclus = book1D( "# R clusters", "Number of r clusters per event", 0., 2000, 400 );
  m_nrphiclus = book1D( "# R+Phi clusters", "Number of r and #phi clusters per event", 0., 2000, 400 );
  m_Rate_DistToM2 = bookProfile1D("Rate_DistToM2","Rate vs DistToM2[um]",0,50,50);
  m_Rate_DistToOutStrip = bookProfile1D("Rate_DistToOutStrip","Rate vs DistToOutStrip[um]",0,50,50);
  
  


  if ( m_hitmapHistos ) {
    m_xy_hitmap = book2D("XY_HitMap", "X(mm) vs Y(mm) hitmap", -75, 75, 100, -45, 45, 100);
    m_zx_hitmap = book2D("ZX_HitMap", "Z(mm) vs X(mm) hitmap", -200, 800, 500, -75, 75, 100);
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

  return VeloMonitorBase::finalize();  // must be called after all other actions
}
//=========================================================================
// Retrieve the VeloClusters
//=========================================================================
StatusCode Velo::VeloTrackMonitor::getVeloClusters ( ) {
  if ( m_debugLevel )
    debug() << "Retrieving VeloClusters from " << m_clusterCont << endmsg;
  
  m_clusters = getIfExists<LHCb::VeloClusters>( m_clusterCont );
  if ( NULL == m_clusters ) {
    if ( m_debugLevel )
      debug() << "No VeloClusters container found for this event !" << endmsg;
    return StatusCode::FAILURE;
    
  }
  else {
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
  
  m_tracks = getIfExists<LHCb::Tracks>( m_trackCont );
  if ( NULL == m_tracks ) {
    if ( m_debugLevel )
      debug() << "No VeloTracks container found for this event !" << endmsg;
    return StatusCode::FAILURE;
  }
  else {
    if ( m_debugLevel ) debug() << "  -> number of tracks found in TES: "
                                << m_tracks->size() <<endmsg;    
  }

  return StatusCode::SUCCESS;  
}

//=========================================================================
// Monitor the VeloTracks 
//=========================================================================
StatusCode Velo::VeloTrackMonitor::monitorTracks ( )
{ 
  if ( m_debugLevel ) debug() <<"--> Excecuting trackMonitor"<< endmsg;
  
  if ( ! m_tracks ) return StatusCode::SUCCESS;
  if ( m_tracks -> size() == 0 ) {
    if ( m_debugLevel ) debug() <<"---Track Container retrieved but EMPTY---"<< endmsg;
    return StatusCode::SUCCESS;
  }

  if( m_clusters == NULL )
    return Warning("No VELO clusters in default location",StatusCode::FAILURE);
  
  //Number of Tracks per Event
  unsigned int nTracks = m_tracks->size();
  //PolarAngle of Tracks
  double theta(0.);
  //Number of R, Phi and total clusters in tracks per Event
  unsigned int nRClusEvent (0), nPhiClusEvent (0), nSumClusEvent (0);
 // RESET AFTER CERTAIN EVENTS
  if ( (context() != "Offline" ) && (m_event == m_resetProfile) ) {
    m_prof_sensors       -> reset();
    m_prof_pos_mom_res   -> reset();
    m_prof_neg_mom_res   -> reset();
    m_prof_thetaR        -> reset();
    m_prof_thetaTot      -> reset();
    m_prof_pseudoEffsens -> reset();
  }

  //Loop over track container
  LHCb::Tracks::const_iterator itTrk;
  
  for ( itTrk=m_tracks->begin(); itTrk!=m_tracks->end(); ++itTrk ) {
    LHCb::Track* track = (*itTrk);
    
    //Skip the tracks without VELO(Velo, VeloR, Upstream and Long)hits
    if ( !track->hasVelo() ) {
      if (m_debugLevel) debug() <<"Track has no VELO hits, continuing with next track."<< endmsg;
      continue;
    }


    //start to save the low adc monitoring histogram    
    const LHCb::TrackFitResult *fitResult = track->fitResult();
    if(!fitResult) return Warning("No fitResult on this track",StatusCode::FAILURE);
    const std::vector<LHCb::Node*> nodes = fitResult->nodes();
    //Loop over all nodes on this track    
    for( std::vector<LHCb::Node*>::const_iterator iNode = nodes.begin();
         iNode != nodes.end(); ++iNode ){
      if(LHCb::Node::HitOnTrack!=(*iNode)->type()) continue; 
      if(!(*iNode)->hasMeasurement()) continue; 
      const LHCb::Measurement *meas = &((*iNode)->measurement());
      const LHCb::VeloRMeasurement *rMeas= dynamic_cast<const LHCb::VeloRMeasurement*>(meas);
      if( ! rMeas ) continue; 
      const LHCb::FitNode * rNode = dynamic_cast<const LHCb::FitNode*>(*iNode);
      if(!rNode){
        return Warning("None fit node found", StatusCode::FAILURE);
      }
      
      
      LHCb::VeloChannelID clusID = rMeas->lhcbID().veloID();// Outter strip        
      const LHCb::VeloCluster *outerClus = rMeas->cluster();
      int outerSize= outerClus->size();
      if(outerSize!=1) continue;// Select only one strip outter cluster
      
      double ADCInner(-99.);      
      double innerSize(-1.);
      double distToStrip(-1.);
      double distToM2(-1.);
      
      LHCb::VeloChannelID innerID;
      LHCb::VeloCluster *innerClus;
      
      //load the map for M2 routing line
      StatusCode sc =
        rMeas->sensor().distToM2Line(rNode->position(),innerID,distToM2,distToStrip);
      if(!sc)continue;
      
      innerClus = m_clusters->object(innerID);// Inner cluster
      if(innerClus){
        ADCInner = innerClus->totalCharge();
        innerSize = (double)innerClus->size(); 
        if(innerSize!=1) continue;// Select only one strip inner cluster
      }
      else {
        continue;
      }
      
      // fill the monitoring histograms
      m_Rate_DistToM2->fill(distToM2*1000., ADCInner<24); // in um
      m_Rate_DistToOutStrip->fill(distToStrip*1000. , ADCInner<24);// in um
      
    }// End of loop over all nodes on this track
    
    
    //General track properties
    if ( m_debugLevel ) {
      debug() << "ErrorFlag:" << track->flag() << std::endl
              << "Track Type:" << track->type() << std::endl
              << "Track History:"<< track->history() << endmsg;
      if( m_trackCont != LHCb::TrackLocation::Velo )
        debug() << "Momentum(GeV):" << track->p()/GeV << std::endl
                << "Chi2/DoF per Track:" << track->chi2PerDoF() << std::endl
                << "Tracks per container:" << m_tracks->size() << endmsg;
    }
    //theta, phi& p  plots
    //---------------------------
    if( m_xPlots){
      theta = track->firstState().slopes().theta();
      m_track_theta->fill(theta/degree);
      m_track_phi->fill(track->phi()/degree);
    }
      
    //eta
    //---
    m_track_eta->fill(track->pseudoRapidity());

    //pseudo-efficiency: Using Velo expectation Tool
    // evaluation using only in the region between 
    // the first and last hit along z
    //-----------------------------------------------
    std::vector<double> N_exp;    
    std::vector<double> N_rec;
    for (int j=0; j<106; j++){
      N_rec.push_back(0.);
      N_exp.push_back(0.);
    }
    double zmin= +800.;
    double zmax= -350.;
    unsigned int sensnrmin=105;
    unsigned int sensnrmax=0;
    unsigned int sensnpmin=105;
    unsigned int sensnpmax=0;
    const std::vector<LHCbID>& vids = track->lhcbIDs();
    for (std::vector<LHCbID>::const_iterator iter = vids.begin() ; iter != vids.end() ; ++iter){
      if (iter->isVelo()){
        //evaluation of the z of first (zmin) and last (zmax) hit
        VeloChannelID chan = iter->veloID(); 
        const DeVeloSensor* sensor = m_veloDet->sensor(chan);
        if (sensor->z() <zmin)
          zmin = sensor->z();
        if ((chan.sensor()<sensnrmin) && (chan.sensor()<64))
          sensnrmin=chan.sensor();
        if ( (chan.sensor()<sensnpmin) && (chan.sensor()>=64))
          sensnpmin = chan.sensor();
        if (sensor->z() > zmax)
          zmax = sensor->z();
        if ((chan.sensor()>sensnrmax) && (chan.sensor()<64))
          sensnrmax=chan.sensor();
        if ( (chan.sensor()>sensnpmax) && (chan.sensor()>=64))
          sensnpmax = chan.sensor();
        
        //evaluation of adcsum for clusters associated to a track
        LHCb::VeloCluster *cluster;
        cluster = (LHCb::VeloCluster*) m_clusters->containedObject( (iter)->channelID() );
        if ( cluster ) {
          double sumadc = cluster ->totalCharge();
          m_track_adcsum->fill(sumadc);
          if(cluster->isRType()){
            double Radc = cluster ->totalCharge();
            m_track_radc->fill(Radc);
          }
          if(cluster->isPhiType()){
            double Phiadc = cluster ->totalCharge();
            m_track_phiadc->fill(Phiadc);
          }
        }
        else
          Error( "Cannot find cluster for VeloChannelID => skipped!" ).ignore();
      }
      
    }  // loop ids  
    unsigned int sensnmin=105, sensnmax=0;
    sensnmax= ((sensnpmax-64)>sensnrmax) ? (sensnpmax-64) : sensnrmax ;
    sensnmin= ((sensnpmin-64)<sensnrmin) ? (sensnpmin-64) : sensnrmin ;
    if ( m_debugLevel )
      debug()<<" zmin=" <<zmin <<" zmax="<<zmax<<" sensnmin="<<sensnmin<<" sensnmax="<<sensnmax<<endmsg;      
    
    //Evaluation of Number of reconstructed hits per track
    const int nVeloHits = std::count_if(vids.begin(), vids.end(),bind(&LHCbID::isVelo,_1));
    //Evaluation of Number of expected hits per track
    int nExpectedHits = m_expectTool->nExpected(*track);
    //Evaluation of Number of expected hits per track by interpolation
    int nExpectedHitsInterp = m_expectTool->nExpected(*track,zmin,zmax);
    if(nExpectedHits != 0){
      m_track_pseudoeff->fill( nExpectedHits>0 ? double(nVeloHits)/double(nExpectedHits) : 0 );
    }
    if(nExpectedHitsInterp != 0){
      m_track_pseudoeff_int->fill( nExpectedHitsInterp>0 ? 
                                   double(nVeloHits)/double(nExpectedHitsInterp) : 0 );
    }

    //Evaluation of expected hit per sensor
    //Loop over sensors
    for(unsigned int j=(sensnmin); j<=sensnmax; j++){
      if ( m_debugLevel ) debug() <<"sensor number " << j << endmsg;
      bool nExpectedHits_sensorR = m_expectTool -> isInside(*track, j);
      if(nExpectedHits_sensorR == true) 
        N_exp[j]+=1;
      if ( m_debugLevel ) debug() << j << " ==> N_exp " << N_exp[j] << endmsg;
      bool nExpectedHits_sensorP = m_expectTool -> isInside(*track, j+64);
      if(nExpectedHits_sensorP == true) 
        N_exp[j+64]+=1;
      if ( m_debugLevel ) debug() << j+64 << " ==> N_exp " << N_exp[j] << endmsg;
    }//end of loop over sensors

    //Unbiased Residuals
    //------------------
    if(m_unbiasedResidualProfile){ 
      
      StatusCode sc2 = unbiasedResiduals (track);
      if ( !( sc2.isSuccess() ) ) {
        if ( m_debugLevel ) debug() << "Unbiased residuals method" << endmsg;
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
       
    //Loop over measurements
    const LHCb::Track::LHCbIDContainer& lhcbids = track->lhcbIDs() ;
    unsigned int n_measurements = lhcbids.size();
    int charge = track->charge();    

    for ( LHCb::Track::LHCbIDContainer::const_iterator it = lhcbids.begin() ; 
	  it != lhcbids.end(); ++it ) {
 
      //Get ChannelID belonging to the Velo LHCbID from the measurements
      LHCb::LHCbID id = (*it) ;
      if(!id.isVelo()) continue;

      LHCb::VeloChannelID vcID = id.veloID();
      int sensorID = vcID.sensor();
      unsigned int stripID = vcID.strip();
      const DeVeloSensor* sensor = m_veloDet->sensor(sensorID); 

      N_rec[sensorID]+=1;
      if ( m_debugLevel ) debug()<<"N_rec "<<N_rec[sensorID]<<endmsg;
      
      //UsedSensor plot
      //---------------
      if ( sensor->isR() || sensor->isPhi() ) {
        m_used_sensors->fill(sensorID);
      }
      
      Gaudi::XYZPoint trackInterceptGlobal= extrapolateToZ(track, sensor->z());
      Gaudi::XYZPoint trackInterceptHalf = sensor->globalToVeloHalfBox ( trackInterceptGlobal );
      Gaudi::XYZPoint trackInterceptLocal(0,0,0) ;      
      trackInterceptLocal    = sensor -> globalToLocal( trackInterceptGlobal );
      double interceptRadius = trackInterceptLocal.Rho();
      double interceptAngle  = trackInterceptLocal.Phi();  
      double interceptAngle_half  = trackInterceptHalf.Phi();  
      
      LHCb::VeloCluster* cluster = m_clusters->object(vcID);
      
      if ( ! cluster ) {
        Error( "Cannot find cluster for VeloChannelID => skipped!" ).ignore();
        continue;
      }
     
      SiPositionInfo<LHCb::VeloChannelID> toolInfo;
      toolInfo=m_clusterTool->position(cluster);
      double interStripFr = toolInfo.fractionalPosition; 
      double biasedResid;
      double chi2;
 
      StatusCode sc3 = sensor -> residual( trackInterceptGlobal,track->slopes(), vcID,
                                           interStripFr, biasedResid, chi2 );
      if ( !( sc3.isSuccess() ) ) {
        if ( m_debugLevel ) debug() << "Residual calculation failed for " 
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
      //State Parameters
      //----------------
      const DeVeloRType* rSensor = m_veloDet->rSensor(sensorID);
      const DeVeloPhiType* phiSensor = m_veloDet->phiSensor(sensorID);

      if ( ( m_alignMoniBasic ) // offline alignment monitoring
           && ( context() == "Offline" ) 
           && ( n_measurements >= m_alignNMeasMin ) ) {
        unsigned int n_sen = sensor->sensorNumber();
        if ( sensor->isR() ) {
          m_prof_res[ n_sen ]->fill( interceptAngle_half / degree, biasedResid / um );
        }
        else {
          double stereo_sign;
          if ( m_ACDC ) {
            stereo_sign = -1.;
          }
          else {
            stereo_sign = 1.;
          }
          double phi_state = 0.;
          if ( sensor->phiType()->zoneOfStrip( stripID ) == 0 ) { // inner
            phi_state = sensor->phiType()->phiOfStrip( stripID, interStripFr, sensor->phiType()->rMin( 0 ) ) / degree;
            phi_state += stereo_sign * -20.04251; // stereo angle for inner strips
          }
          else {                                               // outer
            phi_state = sensor->phiType()->phiOfStrip( stripID, interStripFr, sensor->phiType()->rMin( 1 ) ) / degree;
            phi_state += stereo_sign * 10.35288; // stereo angle for outer strips
          }

          // transform to VELO half phi state
          if( UNLIKELY( msgLevel(MSG::VERBOSE) ) )
            verbose() << "Phi local is " << phi_state * degree << endmsg;
          Gaudi::RhoZPhiPoint phiStateLoc;
          phiStateLoc.SetRho( 1. );
          phiStateLoc.SetPhi( phi_state * degree );
          phiStateLoc.SetZ( 0. );
          Gaudi::XYZPoint phiStateLocal;
          phiStateLocal.SetX( phiStateLoc.X() );
          phiStateLocal.SetY( phiStateLoc.Y() );
          phiStateLocal.SetZ( phiStateLoc.Z() );
          Gaudi::XYZPoint phiStateGlobal( 0., 0., 0. );
          phiStateGlobal = phiSensor->localToGlobal( phiStateLocal );
          Gaudi::XYZPoint phiStateHalf = phiSensor->localToVeloHalfBox( phiStateLocal );
          double phi_state_half = phiStateHalf.Phi() / degree;
          if( UNLIKELY( msgLevel(MSG::VERBOSE) ) )
            verbose() << "Phi half is " << phi_state_half  << endmsg;
          // phi sensor numbering is 64-105, filled into 42-83
          m_prof_res[ n_sen - 22 ]->fill( phi_state_half, biasedResid / um ); 
        }
      }
      
      //Biased Residuals
      //----------------
      if(m_biasedResidualProfile){
        if ( 0 != pitch ) {
          //Sensors vs Biased Residuals profile only for Velo tracks
          m_prof_sensors -> fill (sensor->sensorNumber(), m_binary * biasedResid / pitch );
          //Momentum vs Biased Residuals profiles for +ve and -ve charges from Long tracks with VeloHits
          if( (track->checkType(Track::Long) == true) && charge  == 1) 
            m_prof_pos_mom_res -> fill (track->p() /GeV, m_binary * biasedResid / pitch );   
          
          else if( (track->checkType(Track::Long) == true) && charge == -1)
            m_prof_neg_mom_res -> fill (track->p() /GeV, m_binary * biasedResid / pitch );
          
          if ((track->checkType(Track::Long) != true) && m_debugLevel)
            debug()<<"Profiles for biased residual vs momentum are not filled: track is not Long Track"<<endmsg;
        }//end pitch
        else {
          Warning("Pitch error",StatusCode::SUCCESS).ignore();
          if ( m_debugLevel ) debug()<< "Pitch is" << pitch << " for sensorID " << sensorID << endmsg;
        }
        
        
      }//if m_biasedResidual
      else{
        if( m_debugLevel )
          debug()<< "Profiles for biased residual are not filled: m_biasedResidual == False" << endmsg;
      }  
      
            
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
    } // end of loop over measurements


    //Loop over module
    //----------------------------------------------------------
    for (int i=0;i<=41;i++) {
      
      //Hit Map plots at R sensor position from Marco's Alignment code
      //-----------------------------------------------------------
      if(matchedSensors[i] != 11) continue;
      
      double correct_phi = 0;

      if( m_ACDC ) {
        correct_phi = -phiCoor[i] + phiCoor_s[2*i] + phiCoor_s[2*i+1];//ACDC Geometry
        m_phi_diff->fill((phiCoor_s[2*i] + phiCoor_s[2*i+1])/degree);
      }
      else {
        correct_phi = phiCoor[i] + phiCoor_s[2*i] - phiCoor_s[2*i+1];//DC06 Geometry
        m_phi_diff->fill((phiCoor_s[2*i] - phiCoor_s[2*i+1])/degree);
      }
      
      
      m_track_rcoord->fill( rCoor[i]/mm);
      
      const DeVeloSensor* sensor = m_veloDet->sensor(i);
      
      double x_loc = rCoor[i] * cos(correct_phi);
      double y_loc = rCoor[i] * sin(correct_phi);    
      Gaudi::XYZPoint pointLocal(x_loc, y_loc, 0);
      Gaudi::XYZPoint phiStateHalf = sensor->localToVeloHalfBox( pointLocal );

      // Plot of phi coordinate of the point in half frame: for C side translate the phi coordinate
      // from (-180,-90)+(90,180) into (-90,90). Phi for top part for both halves is within (0,90)
      // the bottom part within (-90,0)
      //A side
      if (i%2==0)
        m_half_phi_a->fill( phiStateHalf.Phi()/degree);
      else
        //C side
        m_half_phi_c->fill( -atan(tan(phiStateHalf.Phi()))/degree);

      if(m_hitmapHistos){ 
        
        Gaudi::XYZPoint pointGlobal(0, 0, 0);
        pointGlobal = sensor->localToGlobal (pointLocal);
          
        x_glob[i] = pointGlobal.x();
        y_glob[i] = pointGlobal.y();
        z_glob[i] = pointGlobal.z();
          
        m_xy_hitmap->fill(x_glob[i], y_glob[i]);
        m_zx_hitmap->fill(z_glob[i], x_glob[i]);
      }
      
    } // for loop on module
    
    nSumClus = nRClus +  nPhiClus;
    
    //No of R and total Clusters per track 
    //------------------------------------
    m_prof_thetaR -> fill (theta/degree, nRClus);
    m_prof_thetaTot -> fill (theta/degree, nSumClus);
    m_nmeasure->fill( nSumClus);
    if( (track->checkType(Track::Long) == true) && (charge == 1))
      m_nposmeas->fill( nSumClus);
    else if ((track->checkType(Track::Long) == true) && (charge == -1))
      m_nnegmeas->fill( nSumClus);

    
    nRClusEvent += nRClus;
    nPhiClusEvent += nPhiClus;
    
    //Pseudoefficiency for each sensor
    //--------------------------------
    double pseudoEfficiency_sens=0;
    double sensnumber;
    double N_expTot=0,N_recTot=0;
    
    for(unsigned int i=(sensnmin); i<=sensnmax; i++){
      N_expTot+=N_exp[i];
      N_recTot+=N_rec[i];
      if(N_exp[i]>=1){
        pseudoEfficiency_sens = N_rec[i] / N_exp[i];
        sensnumber=(double) i;
        m_prof_pseudoEffsens -> fill(sensnumber, pseudoEfficiency_sens);          
      }
      if ( m_debugLevel )
        debug()<<i+64<<" N_rec "<<N_rec[i+64]<<" N_exp "<<N_exp[i+64]<<endmsg;
      if(N_exp[i+64]>=1){
        pseudoEfficiency_sens = N_rec[i+64] / N_exp[i+64];
        sensnumber=(double) i+64.;
        m_prof_pseudoEffsens -> fill(sensnumber, pseudoEfficiency_sens);          
      }
      //if expect hits on both sensor and only one sensor has hit -> mismatch
      if (((N_exp[i]>=1) && (N_exp[i+64]>=1)) && 
          ((N_rec[i]==0) || (N_rec[i+64]==0)) &&
          ((N_rec[i]==1) || (N_rec[i+64]==1)) ){
        if (N_rec[i+64]==0)
           m_mod_mismatch->fill(i+64);
        else if (N_rec[i]==0)
           m_mod_mismatch->fill(i);
      } 
    }
  }//End of loop over track container

    
  if(m_EventClusterInfo){
    
    nSumClusEvent =  nRClusEvent + nPhiClusEvent;
    
    //No of R Clusters per Event 
    //--------------------------
    m_nrclus->fill( nRClusEvent);
    
    //No of R+Phi Clusters per Event 
    //------------------------------
    m_nrphiclus->fill( nSumClusEvent);
    
  }
  
  // online alignment monitoring
  if( ( m_alignMoniBasic )  && ( context() != "Offline" ) ) {
    if (m_debugLevel) debug() << "Filling online context" << endmsg;
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
        m_h_aliMon_Mean_R_A->fill( m_prof_sensors->binHeight( bin ) );
        m_h_aliMon_Sigma_R_A->fill( m_prof_sensors->binError( bin ) );
        m_h_aliMon_Mean_P_A->fill( m_prof_sensors->binHeight( bin + 64 ) );
        m_h_aliMon_Sigma_P_A->fill( m_prof_sensors->binError( bin + 64 ) );
      }
      else {
        m_h_aliMon_Mean_R_C->fill( m_prof_sensors->binHeight( bin ) );
        m_h_aliMon_Sigma_R_C->fill( m_prof_sensors->binError( bin ) );
        m_h_aliMon_Mean_P_C->fill( m_prof_sensors->binHeight( bin + 64 ) );
        m_h_aliMon_Sigma_P_C->fill( m_prof_sensors->binError( bin + 64 ) );
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
    if ( m_debugLevel )
      debug() << "Start Unbiased Residual method" << endmsg;    
    // RESET AFTER CERTAIN EVENTS
    if ( m_event ==  m_resetProfile ) { m_prof_unsensors -> reset() ; } 
    // RESET AFTER CERTAIN EVENTS
    if ( (context() != "Offline") &&  (m_event == m_resetProfile) ) { m_prof_pos_mom_unres -> reset() ; } 
    // RESET AFTER CERTAIN EVENTS
    if ( (context() != "Offline") &&  (m_event == m_resetProfile) ) { m_prof_neg_mom_unres -> reset() ; } 
    
    //Loop over nodes
    LHCb::Track::ConstNodeRange nodes = track->nodes();
    LHCb::Track::ConstNodeRange::const_iterator it = nodes.begin();
    
    for ( ; it != nodes.end(); ++it ) {
      
      const FitNode& fitnode =  dynamic_cast<const FitNode&>(**it);
      if(!fitnode.hasMeasurement()) continue;
      
      //Get ChannelID belonging to the LHCbID from the measured fitnode
      const LHCb::LHCbID& lhcbID = fitnode.measurement().lhcbID();
      if ( lhcbID.detectorType() != LHCbID::Velo ) continue;
      
      LHCb::VeloChannelID vcID = lhcbID.veloID();
      LHCb::VeloCluster* cluster = m_clusters->object(vcID);

      if ( ! cluster ) {
        Error( "Cannot find cluster for VeloChannelID => skipped!" ).ignore();
        return StatusCode::FAILURE;
      }
      
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
      StatusCode sc3 = sensor->residual(trackInterceptGlobal,track->slopes(),
                                        vcID, interStripFr, UnbiasedResid, chi2);
      if ( !( sc3.isSuccess() ) ) {
        if ( m_debugLevel ) debug() << "Residual calculation failed for "
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
        m_prof_unsensors -> fill (sensor->sensorNumber(), m_binary * UnbiasedResid / pitch );
        
        //Momentum vs UnBiased Residuals profiles for +ve and -ve charges for Velo part of Long tracks        
        if((track->checkType(Track::Long) == true) && track->charge()  == 1)
          m_prof_pos_mom_res -> fill (track->p()/GeV,  m_binary * UnbiasedResid / pitch );   
        
        else if((track->checkType(Track::Long) == true) && track->charge() == -1)
          m_prof_neg_mom_res -> fill (track->p()/GeV,  m_binary * UnbiasedResid / pitch );   
        
        if ((track->checkType(Track::Long) == true) && m_debugLevel)
          debug()<<"Profiles for unbiased residual vs momentum are not filled: track is not Long Track"<<endmsg; 
      }
      
      
      else {
        Warning("Pitch error",StatusCode::SUCCESS).ignore();
        if ( m_debugLevel ) debug()<< "Pitch is" << pitch << " for sensorID " << sensorID << endmsg;
      }
      
    }//end of fit node
  }//if fit is BiKalman
  
  else{ 
    if(m_debugLevel){
      debug() << "No FitNode. Terminating Unbiased Residual method" << endmsg;
    }
  }
  
  return StatusCode::SUCCESS;
}
