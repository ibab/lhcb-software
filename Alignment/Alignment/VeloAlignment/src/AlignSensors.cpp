
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IRegistry.h"

// velo

#include "VeloDet/DeVelo.h"
#include "VeloDet/DeVeloSensor.h"
#include "Event/VeloCluster.h"
//#include "Event/VeloRMeasurement.h"
//#include "Event/VeloPhiMeasurement.h"
#include "DetDesc/AlignmentCondition.h"
#include "DetDesc/GlobalToLocalDelta.h"

//tracking

//#include "TrackInterfaces/IMeasurementProvider.h"
#include "TrackInterfaces/ITrackProjector.h"

//boost
#include <boost/lexical_cast.hpp>

// local
#include "AlignSensors.h"

//using ROOT::Math::SMatrix;

using namespace Gaudi::Units;
using namespace LHCb;
using namespace Gaudi;

//-----------------------------------------------------------------------------
// Implementation file for class : AlignSensors
//
// 2008-01-23 : Marco Gersabeck
//
// A fitting algorithm for the relative VELO sensor alignment. It uses tracks 
// from the Track bank in TES to produce unbiased residual plots and fit the
// misalignments to them.
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( AlignSensors )


// Standard constructor, initializes variables
AlignSensors::AlignSensors( const std::string& name,ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator ),
    m_veloDet ( NULL ),
    m_clusterCont ( LHCb::VeloClusterLocation::Default ),
    m_trackCont ("Rec/Track/Velo" ),
    m_iteration ( 0 ),
    m_numberOfEvents ( 0 )
{
  declareProperty( "PrintInfo"		, m_printInfo = false);
  declareProperty( "SelectedEvent"		, m_trackValue = 1 );
  declareProperty( "TrackLocation", m_trackCont);
  declareProperty( "OutputNtuple", m_ntuple=false );
  declareProperty( "TestSensors", m_testSensors) ;
  declareProperty( "Fitter", m_fitterName = "TrackMasterFitter" );
  declareProperty( "MinSenR", m_minSenR = 4 );
  declareProperty( "MinSenPhi", m_minSenPhi = 4 );
  declareProperty( "MinModule", m_minModule = 4 );
  declareProperty( "KalmanResidual", m_kalmanResidual = false );
  declareProperty( "MinNResiduals", m_minNResiduals = 0 );
  declareProperty( "ACDC", m_ACDC = true );
  declareProperty( "MinDeltaSig", m_minDeltaSig = 1 );
}
//=============================================================================
// Destructor
//=============================================================================

AlignSensors::~AlignSensors() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode AlignSensors::initialize() {
  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiTupleAlg

  debug() << "==> Initialize" << endmsg;
  
  m_clusterTool=tool<IVeloClusterPosition>("VeloClusterPosition");
  m_veloDet = getDet<DeVelo>( DeVeloLocation::Default );
  
  m_tracksFitter = tool<ITrackFitter>( m_fitterName, "Fitter", this );
  
  m_trackInterpolator = tool<ITrackInterpolator>( "TrackInterpolator" );

  m_projectorSelector = tool<ITrackProjectorSelector>( "TrackProjectorSelector",
                                                       "Projector", this );
  
  // Get UpdateManagerSvc for updating alignment conditions
  m_updMgrSvc = svc<IUpdateManagerSvc>( "UpdateManagerSvc" );
  if ( !m_updMgrSvc ) return Error( "==> Failed to retrieve UpdateManagerSvc!", StatusCode::FAILURE );

  /// Get pointer to incident service and add listener
  m_incSvc = svc<IIncidentSvc>( "IncidentSvc" );
  if ( !m_incSvc ) return Error( "==> Failed to retrieve IncidentSvc!", StatusCode::FAILURE );
  m_incSvc->addListener( this, "FitSensorResiduals" );
  m_incSvc->addListener( this, "ResetHistos" );


  char hname[100];
  for ( int i = 0; i < 84; i++ ) {
    sprintf( hname, "m_h_res_2d_%d", i );
    m_h_res_2d[ i ] = new TH2D( hname, "residual vs phi", 42, -210., 210., 400, -200., 200. );
    sprintf( hname, "m_h_resr_2d_%d", i );
    m_h_resr_2d[ i ] = new TH2D( hname, "residual vs r", 25, 0., 50., 400, -200., 200. );
  }
  m_fit_func = new TF1( "m_fit_func",
                        "[0]*sin(x*TMath::Pi()/180.)+[1]*cos(x*TMath::Pi()/180.)+[2]",
                        -210., 210. );
  m_r_fit_func = new TF1( "m_r_fit_func",
                          "[0]*x+[1]",
                          0., 50. );


  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode AlignSensors::execute() {
  
  debug() << "==> Execute" << endmsg;
  
  m_numberOfEvents++;
  StatusCode sc=getTrackContainer(); 
  verbose() << "getTrackContainer() done" << endmsg;
  StatusCode sc2=getClusterContainer();
  if(!(sc && sc2)){
    return StatusCode::SUCCESS;
  }

  verbose() << "call trackMonitor()" << endmsg;
  trackMonitor();
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode AlignSensors::finalize() {

  debug() << "==> Finalize" << endmsg;

  for ( int i = 0; i < 84; i++ ) {
    ( m_h_res_2d[ i ] )->Write();
    ( m_h_resr_2d[ i ] )->Write();
    delete m_h_res_2d[ i ];
    delete m_h_resr_2d[ i ];
  }
  delete m_fit_func;
  delete m_r_fit_func;

  return GaudiTupleAlg::finalize();  // must be called after all other actions
}


//=============================================================================
// Get the track container
//=============================================================================
StatusCode AlignSensors::getTrackContainer()
{ 

  debug()<< "==> getTrackContainer() " <<endmsg;

  verbose() << "Retrieving " << m_trackCont << endmsg; 
  if ( !exist<LHCb::Tracks>(m_trackCont ) ) { 
    debug() << " ----  No VeloTracks container retrieved --- "
            << endmsg;
  }else{
    m_trackContainer=get<LHCb::Tracks>(m_trackCont);
    verbose() << "---- VeloTrack Container retrieved from " << m_trackCont << endmsg;
  }
  
  return (StatusCode::SUCCESS);
}

//=============================================================================
// Get the cluster container
//=============================================================================

StatusCode AlignSensors::getClusterContainer()
{


  debug()<< "==> getClusterContainer() " <<endmsg;

  verbose() << "Retrieving " << m_clusterCont << endmsg;
  if ( !exist<LHCb::VeloClusters>(m_clusterCont ) ) {
    debug() << " ----  No VeloClusters container retrieved --- "
            << endmsg;
    return (StatusCode::FAILURE);
  }else{
    m_clusters=get<LHCb::VeloClusters>(m_clusterCont );
    verbose() << "---- VeloCluster Container retrieved from default location ----"
              << endmsg;
    if(m_printInfo){
      verbose()<< "Number of clusters with connected sensors found in TES: "
               << m_clusters->size() <<endmsg;
    }
  }
  return (StatusCode::SUCCESS);
}




//============================================================================
//Track monitoring
//===========================================================================

StatusCode AlignSensors::trackMonitor() 
{
  
  debug() << "--> Excecuting trackMonitor" << endmsg;
  if (!m_trackContainer) return (StatusCode::SUCCESS);
 
  if (m_trackContainer->size() == 0) {
    debug() << " ----  Track Container retrieved but EMPTY --- "
            << endmsg;
    return StatusCode::FAILURE;
  }

  LHCb::Tracks::const_iterator itTrg;

  // loop over tracks
  for(itTrg=m_trackContainer->begin();itTrg!=m_trackContainer->end();itTrg++){

    LHCb::Track* track = (*itTrg);

    // Tracks without VELO hits are of no use
    if ( !track->hasVelo() ) {
      debug() << "Track has no VELO hits. Continuing with next track." << endmsg;
      continue;
    }

    if ( m_kalmanResidual ) {
      makeKalmanPlots( track );
    } 
    else {
      makeStraightLinePlots( track );
    }

  }

  return StatusCode::SUCCESS;
}

//============================================================================
// Produce unbiased residual plots with Kalman track fitting
//===========================================================================
StatusCode AlignSensors::makeKalmanPlots( LHCb::Track* track ) 
{
    
  // count R and Phi hits
  // produce list of modules
  const std::vector< LHCb::LHCbID >& trackIDs = track->lhcbIDs();
  int n_r = 0;
  int n_phi = 0;
  std::vector< LHCb::LHCbID >::const_iterator it;
  bool modules[ 42 ];
  for ( int i = 0; i < 42; i++ ) {
    modules[ i ] = false;
  }
  for ( it = trackIDs.begin(); it != trackIDs.end(); it++ ) {
    if ( it->isVeloR() ) {
      n_r++;
      modules[ it->veloID().sensor() ] = true;
    }
    if ( it->isVeloPhi() ) {
      n_phi++;
      modules[ it->veloID().sensor() - 64 ] = true;
    }
  }
  plot1D( n_r, "RSenPerTrack", "R sensors per track", -0.5, 50.5, 51 );
  plot1D( n_phi, "PhiSenPerTrack", "Phi sensors per track", -0.5, 50.5, 51 );
  if ( ( n_r < m_minSenR ) || ( n_phi < m_minSenPhi ) ) {
    debug() << "Track does not have enough R and/or Phi hits. Continuing with next track." << endmsg;
    return StatusCode::FAILURE;
  }

  // fill general track histograms
  plot1D(track->flag(),"ErrorFlags","Error flags",-0.5,1.5,2);
  plot1D(track->type(),"TrackType","Track type",-0.5,7.5,8);
  
  plot1D(track->chi2PerDoF(),"Chi2_nDoF","Chi2/DoF per Track",-0.1,4.1,75);      

  // get state parameters
  if (m_printInfo) {             
    Gaudi::XYZPoint m_coord = track->position();
    Gaudi::XYZVector m_slopes = track->slopes();
    verbose()  << "Track# : " 	<< track->key()+1
               << " Chi2 : " 	<< track->chi2()
               << " DoF : "      << track->nDoF()
               << " Charge : " 	<< track->charge() 
               << " p : " 	<< track->p()      
               << " pt : " 	<< track->pt()     
               << " x : " 	<< m_coord.x()
               << " y : " 	<< m_coord.y()
               << " z : " 	<< m_coord.z()
               << " dx/dz : "	<< m_slopes.x()
               << " dy/dz : "    << m_slopes.y()
               << " dz/dz : "    << m_slopes.z()
               << endmsg;
  }

  //   // set q/p and error in all of the existing states
  //   const std::vector< LHCb::State * > states = track->states();
  //   std::vector< LHCb::State * >::const_iterator iState;
  //   for ( iState = states.begin() ; iState != states.end() ; ++iState ) {
  //     (*iState)->setQOverP( 1. / 1e6 );  // 1 TeV tracks
  //   }
 
  // loop over modules with hits
  std::vector< LHCb::LHCbID > moduleIDs;
  for ( unsigned int i = 0; i < 42; i++ ) {
    if ( !modules[ i ] ) {
      verbose() << "Module " << i << " has no hits." << endmsg;
      continue;
    }
    // clone track and remove LHCbID corresponding to current module
    // while filling vector of LHCbIDs for current module
    LHCb::Track* tr_clone = track->cloneWithKey();
    moduleIDs.clear();
    for ( it = trackIDs.begin(); it != trackIDs.end(); it++ ) {
      if ( it->isVeloR() ) {
        if ( it->veloID().sensor() == i ) {
          tr_clone->removeFromLhcbIDs( *it );
          moduleIDs.push_back( *it );
        }
      }
      if ( it->isVeloPhi() ) {
        if ( ( it->veloID().sensor() - 64 ) == i ) {
          tr_clone->removeFromLhcbIDs( *it );
          moduleIDs.push_back( *it );
        }
      }
    }

    // fit the remaining track
    StatusCode sc = m_tracksFitter -> fit( *tr_clone );

    if ( !sc.isSuccess() ) {
      debug() << "Track fit failed. Continuing with next track." << endmsg;
      continue;
    }
    
    plot1D( tr_clone->chi2PerDoF(), "TrackChi2pDOF", "Track Chi2 per DOF", 0, 50, 500 );
      
    // loop over LHCbIDs of current module and plot residuals
    double residual, residual_err;
    for ( it = moduleIDs.begin(); it != moduleIDs.end(); it++ ) {
      const int sensorNumber = it->veloID().sensor();
      const DeVeloSensor* sensor = m_veloDet->sensor( sensorNumber );
      const double sensor_z = sensor->z();
      LHCb::State tr_state = LHCb::State();
      m_trackInterpolator->interpolate( *tr_clone, sensor_z, tr_state );
      const LHCb::VeloChannelID vcID = it->veloID();
      LHCb::VeloCluster *cluster = m_clusters->object( vcID );
      double interStripFr = cluster->interStripFraction();
      const Gaudi::XYZPoint trackInterceptGlobal = tr_state.position();
      sensor->residual( trackInterceptGlobal,tr_clone->slopes(), vcID, interStripFr, residual, residual_err );
      // plot residual vs phi state
      double phi_state = 0;
      if ( it->isVeloR() ) {
        const DeVeloRType* rSensor = m_veloDet->rSensor( sensorNumber );
        Gaudi::XYZPoint trackInterceptHalf = rSensor->globalToVeloHalfBox ( trackInterceptGlobal );
        phi_state = trackInterceptHalf.Phi() / degree;
        verbose() << "Phi state is " << phi_state << endmsg;
        verbose() << "X,Y state is " << trackInterceptGlobal.X() << ", " << trackInterceptGlobal.Y() << endmsg;
        ( m_h_res_2d[ sensorNumber ] )->Fill( phi_state, residual/um );
      }
      else { // is Phi sensor
        const DeVeloPhiType* phiSensor = m_veloDet->phiSensor( sensorNumber );
        int stereo_sign;
        if ( m_ACDC ) {
          stereo_sign = -1;
        }
        else {
          stereo_sign = 1;
        }
        if ( phiSensor->zoneOfStrip( vcID.strip() ) == 0 ) { // inner
          phi_state = phiSensor->phiOfStrip( vcID.strip(), interStripFr, phiSensor->rMin( 0 ) ) / degree;
          phi_state += stereo_sign * -20.04251;                                 // stereo angle for inner strips
        }
        else {                                      // outer
          phi_state = phiSensor->phiOfStrip( vcID.strip(), interStripFr, phiSensor->rMin( 1 ) ) / degree;
          phi_state += stereo_sign * 10.35288;                       // stereo angle for outer strips
        }
        // transform to global phi state
        verbose() << "Phi phi local is " << phi_state * degree << endmsg;
        Gaudi::RhoZPhiPoint phiStateLoc;
        phiStateLoc.SetRho( 1. );
        phiStateLoc.SetPhi( phi_state * degree );
        phiStateLoc.SetZ( 0. );
        Gaudi::XYZPoint phiStateLocal;
        phiStateLocal.SetX( phiStateLoc.X() );
        phiStateLocal.SetY( phiStateLoc.Y() );
        phiStateLocal.SetZ( phiStateLoc.Z() );
        Gaudi::XYZPoint phiStateGlobal( 0., 0., 0. );
        //phi state is in half frame
        Gaudi::XYZPoint phiStateHalf = phiSensor->localToVeloHalfBox( phiStateLocal );
        phi_state = phiStateHalf.Phi() / degree;
        verbose() << "Phi phi global is " << phi_state << endmsg;
        ( m_h_res_2d[ sensorNumber - 64 + 42 ] )->Fill( phi_state, residual/um );
      }

    } // end loop over module hits
    delete tr_clone;
  } // end loop over modules
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Produced unbiased residual histograms using straight line fit.
//=============================================================================
StatusCode AlignSensors::makeStraightLinePlots( LHCb::Track* track ) {

  const std::vector<LHCb::LHCbID>& m_idContainer = track->lhcbIDs();
  std::vector<LHCb::LHCbID>::const_iterator itID;
  std::vector<int> unusedSensors;
  unusedSensors.assign(140,1);
  
  std::vector<LHCb::VeloCluster*> cluster_container;
  std::vector<LHCb::VeloCluster*>::iterator itClus;

  //Move clusters in track to a general cluster container
  
  for (itID=m_idContainer.begin();itID!=m_idContainer.end();itID++) {
    
    
    const LHCb::LHCbID& id = *itID;
    
    //Get ChannelID and cluster belonging to the LHCbID:
    LHCb::VeloChannelID vcID = id.veloID();
    
    LHCb::VeloCluster *cluster=m_clusters->object(vcID );

    cluster_container.push_back(cluster);
  } //end of loop over LHCbIDs.

  int station;
  int hits[42]{};
  //int nStrips[84];
  double r[42]{};
  double r_e[42]{};
  //double r_s[84];
  double phi[42]{};
  double phi_e[42]{};
  double phi_s[84]{};
  double x_meas[42]{};
  double y_meas[42]{};
  double z_meas[42]{};
  double x_err[42]{};
  double y_err[42]{};
  //double z_err[42];
  int    active[42]{};  // array to point to active modules
  int    last_active = -1; // counter to point at index of last active module

  // set everything to 0
  for (int j=0; j<42; j++) active[j] = -1;

  // check which modules have hits on both sensors and store track states at every sensor as well as cluster positions
  for(itClus=cluster_container.begin(); itClus!=cluster_container.end();++itClus) {
    LHCb::VeloCluster *cluster=(*itClus);
    LHCb::VeloChannelID vcID = cluster->channelID();
    station = vcID.sensor();
    const DeVeloSensor* sns = m_veloDet->sensor(station);
    Gaudi::XYZPoint trackInterceptGlobal= extrapolateToZ(track, sns->z());
    Gaudi::XYZPoint trackInterceptLocal(0,0,0) ;
    trackInterceptLocal = sns->globalToLocal( trackInterceptGlobal );
    double rstate   = trackInterceptLocal.Rho();
    double phistate = trackInterceptLocal.Phi();

    SiPositionInfo<LHCb::VeloChannelID> toolInfo;
    toolInfo=m_clusterTool->position(cluster);
    double interStripFr = toolInfo.fractionalPosition;
    vcID =toolInfo.strip;
    // Here we need to compute the correct cluster position
    if(rstate != 0.) {
     
      verbose() << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-" << endmsg;
      verbose() << "Track state (in local frame) at z  = " << sns->z() << " :" << endmsg;
      verbose() << "-->  Extrapolated phi              = " << phistate << endmsg;
      verbose() << "-->  Extrapolated r                = " << rstate << endmsg;
      // Compute the cluster position

      unsigned int cl_str = vcID.strip();
      if (vcID.type() == LHCb::VeloChannelID::RType) {
        const DeVeloRType *rDet = m_veloDet->rSensor(cluster->channelID().sensor());

        hits[station]      += 1;
        r[station]          = rDet->rOfStrip( cl_str ) + interStripFr * ( rDet->rPitch( cl_str ) );	  
        r_e[station]        =  rDet -> rPitch(  cl_str  ) * toolInfo.fractionalError ;
        phi_s[2*station]    = phistate;
        //r_s[2*station]      = rstate;
        //nStrips[2*station]  = cluster->size();
      } 
      else if (vcID.type() == LHCb::VeloChannelID::PhiType) {
        const DeVeloPhiType *phiDet = m_veloDet->phiSensor( vcID.sensor() );

        station = station-64;  // Phi stations are numbered from 64 to 95
        hits[station]      += 10;
        phi[station]          = phiDet->phiOfStrip(cl_str,interStripFr,rstate);	  
        phi_e[station+1]      =  phiDet -> phiPitch( rstate ) * toolInfo.fractionalError;
        phi_s[2*station+1]    = phistate;
        //r_s[2*station+1]      = rstate;
        //nStrips[2*station+1]  = cluster->size();
      }
    } // finished computation of corrected cluster position

  } // end loop on clusters

  //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

  // compute spacepoints at R sensor position

  double error_r, error_p;
  
  for (int n_stat = 0; n_stat < 42; n_stat++ ) {
     
    error_r = 0.;
    error_p = 0.;
                      
    // Left sensor have even number, right have odd ones
                      
    if ( hits[ n_stat ] != 11 ) continue;  // Only carry on if both sensors have hits
                      
    // The space-point (local frame so be careful with the phi's)
    // In fact it appears that local Phi of R sensor is inverted (ACDC geom)
    // They are in the same frame in DC06
          
    // correct phi measurement for difference in phi states between R and Phi sensor          
    double correct_phi = 0;
    if ( m_ACDC ) {  // ACDC geometry
      correct_phi = -phi[n_stat]+phi_s[2*n_stat]+phi_s[2*n_stat+1];
      plot1D( phi_s[2*n_stat]+phi_s[2*n_stat+1], "phi_state_corr",
              "phi state correction",-0.5,0.5,100);
    }
    else {           // DC06 geometry
      correct_phi = phi[n_stat]+phi_s[2*n_stat]-phi_s[2*n_stat+1];
      plot1D( phi_s[2*n_stat]-phi_s[2*n_stat+1], "phi_state_corr",
              "phi state correction",-0.5,0.5,100);
    }

    const DeVeloSensor* sns = m_veloDet->sensor(n_stat); // this is the R sensor
              
    // get space point in local frame      
    double x = r[n_stat]*cos(correct_phi);
    double y = r[n_stat]*sin(correct_phi);
                    
    // Space-point is now expressed in the global frame
                    
    Gaudi::XYZPoint pointLocal(x, y,0);
    Gaudi::XYZPoint pointGlobal(0,0,0) ;
                    
    pointGlobal = sns->localToGlobal( pointLocal );
                    
    //     // Errors are calculated as (pitch/sqrt(12)) for the moment (SV 08/07/05) 
    //     error_r = (40+(101.6-40.0)*(r[n_stat]-8.19)/(41.95-8.19))/3464.;
    //     if (r[n_stat] <= 17.2)
    //       error_p = (35.5+(78.3-35.5)*(r[n_stat]-8.19)/(17.2-8.19))/3464.;
    //     if (r[n_stat] > 17.2)
    //       error_p = (39.3+(96.6-39.3)*(r[n_stat]-17.2)/(41.95-17.2))/3464.;
         
    //Error defined in cluster tool
    error_r = r_e[n_stat];
    error_p = phi_e[n_stat];

    // store measurements and errors as well as module number
    x_meas[n_stat] = pointGlobal.x();
    y_meas[n_stat] = pointGlobal.y();
    z_meas[n_stat] = pointGlobal.z();
    x_err[n_stat]  = sqrt(pow(x*error_r,2)+pow(y*error_p,2))/r[n_stat];
    y_err[n_stat]  = sqrt(pow(y*error_r,2)+pow(x*error_p,2))/r[n_stat];
    //z_err[n_stat]  = 11;
    active[++last_active] = n_stat;
                    
    verbose() << "" << endmsg;
    verbose() << "-> Global coordinates for sensor " << n_stat << endmsg;
    verbose() << "x_spacepoint  = " << x_meas[n_stat] << endmsg;
    verbose() << "y_spacepoint  = " << y_meas[n_stat] << endmsg;
    verbose() << "z_spacepoint  = " << z_meas[n_stat] << endmsg;

  } // end loop over modules

  //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

  // now get unbiased track fit, compute and plot residuals

  double local_vecx[2];
  double local_matx[2][2];
  double local_vecy[2];
  double local_maty[2][2];
          
  double wghtx, rmeasx, detx;
  double wghty, rmeasy, dety;
  double z_cor;
  double slope_x, slope_y, x0_x, y0_y, z0_z;
          
  plot1D( last_active + 1, "fewSpacepoints", "Number of spacepoints per track", -0.5, 49.5, 50 );

  // at least m_minModule active modules for unbiased residual plots
  if ( ( last_active + 1 ) < m_minModule ) {
    debug() << "Not enough spacepoints on track" << endmsg;
    return StatusCode::FAILURE;
  }

  // loop over all active modules
  for ( int res_mod = 0; res_mod <= last_active; res_mod++ ) {

    if ( ( ( res_mod >= 16 ) && ( ( last_active + 1 ) < 6 ) ) 
         || ( ( res_mod >= 32 ) && ( ( last_active + 1 ) < 8 ) ) ) {
      debug() << "Not enough spacepoints on track" << endmsg;
      return StatusCode::FAILURE;
    }
    for (int i=0; i<2; i++) { // reset local params
    
      local_vecx[i] = 0.0;
      local_vecy[i] = 0.0;
                      
      for (int j=0; j<2; j++) {
        local_matx[i][j] = 0.0;
        local_maty[i][j] = 0.0;
      }

    }

    // now loop over all modules but the one for the residuals to populate matrix for unbiased track fit
    for ( int act_mod = 0; act_mod <= last_active; act_mod++ ) {
      if ( res_mod == act_mod ) continue; // leave out module to plot residuals for 

      z_cor = z_meas[active[act_mod]];
      wghtx = x_err[active[act_mod]];
      rmeasx = x_meas[active[act_mod]];
      wghty = y_err[active[act_mod]];
      rmeasy = y_meas[active[act_mod]];
          
      local_vecx[0] += wghtx*rmeasx;
      local_vecx[1] += wghtx*rmeasx*z_cor;
      local_vecy[0] += wghty*rmeasy;
      local_vecy[1] += wghty*rmeasy*z_cor;
                    
      local_matx[0][0] += wghtx;
      local_matx[1][0] += wghtx*z_cor;
      local_matx[1][1] += wghtx*z_cor*z_cor;
      local_maty[0][0] += wghty;
      local_maty[1][0] += wghty*z_cor;
      local_maty[1][1] += wghty*z_cor*z_cor;
    }

    // invert the matrix and compute track parameters
    detx = 0.;
    dety = 0.;
          
    detx = local_matx[0][0] *local_matx[1][1] - local_matx[1][0] * local_matx[1][0];
    dety = local_maty[0][0] * local_maty[1][1] - local_maty[1][0] * local_maty[1][0];
          
    if (detx == 0. || dety == 0.) continue; // track fit failed
          
    slope_x = ( local_vecx[1] * local_matx[0][0] - local_vecx[0] * local_matx[1][0] ) / detx;
    slope_y = ( local_vecy[1] * local_maty[0][0] - local_vecy[0] * local_maty[1][0] ) / dety;
          
    x0_x = ( local_vecx[0] * local_matx[1][1] - local_vecx[1] * local_matx[1][0] ) / detx;
    y0_y = ( local_vecy[0] * local_maty[1][1] - local_vecy[1] * local_maty[1][0] ) / dety;
    z0_z = 0.;

    // check and monitor track slopes/angle
    double theta = atan( sqrt( slope_x * slope_x + slope_y * slope_y ) );
    plot1D( theta, "track_angle", "Track angle", 0, 1.57, 1570 );
    plot1D( slope_x, "track_slope_x", "Track slope x", -0.5, 0.5, 1000 );
    plot1D( slope_y, "track_slope_y", "Track slope y", -0.5, 0.5, 1000 );
    plot2D( slope_x, slope_y, "track_slope_xy", "Track slope xy", -0.1, 0.1, -0.1, 0.1, 200, 200 );

    //if ( theta > 0.005 ) continue; // only low angle track to ensure small change in pitch

    plot2D( x0_x, y0_y, "track_position_xy", "Track position xy", -50, 50, -50, 50, 100, 100 );
    int module = active[res_mod];
    double residual, chi2;

    // loop over cluster to plot residuals
    for(itClus = cluster_container.begin();
        itClus != cluster_container.end();
        ++itClus ) {

      // check whether cluster belongs to correct sensor
      LHCb::VeloChannelID vcID = (*itClus)->channelID();
      int cl_sns = vcID.sensor();
      // cluster doesn't belong to module under study
      if ( ( cl_sns != module ) && ( cl_sns != ( module + 64 ) ) ) continue; 

      debug() << "Sensor " << cl_sns << endmsg;

      // extrapolate unbiased track to sensor of cluster
      double phi_state = 0;
      double r_state = 0;
      double interStripFr = (*itClus)->interStripFraction();
      const DeVeloSensor* sns = m_veloDet->sensor( cl_sns );
      double z_sen = sns->z();
      Gaudi::XYZPoint trackInterceptGlobal( x0_x + slope_x * ( z_sen - z0_z ), 
                y0_y + slope_y * ( z_sen - z0_z ), z_sen) ;
      Gaudi::XYZPoint trackInterceptHalf = sns->globalToVeloHalfBox ( trackInterceptGlobal );
      plot2D( trackInterceptGlobal.Rho() / mm, trackInterceptGlobal.Phi() / degree, 
              "track_position_rphi", "Track position r/phi", -50, 50, -200, 200, 100, 200 );
      plot2D( trackInterceptHalf.Rho() / mm, trackInterceptHalf.Phi() / degree, 
              "track_position_rphi_half", "Track position r/phi in VELO half", -50, 50, -200, 200, 100, 200 );
      r_state = trackInterceptHalf.Rho() / mm;
      debug() << "R state is " << r_state / um << endmsg;

      // calculate the residual
      Gaudi::XYZVector track_dir( slope_x , slope_y ,1.);
      sns->residual( trackInterceptGlobal,track_dir, vcID, interStripFr, residual, chi2 );

      if ( cl_sns == module ) { // R sensor cluster
        if ( sns->isR() ) {     // make really sure

          const DeVeloRType* rSensor = m_veloDet->rSensor( cl_sns );
          // retrieve phi state
          phi_state = trackInterceptHalf.Phi() / degree;
          verbose() << "Phi state is " << phi_state << endmsg;
          verbose() << "X,Y state is " << trackInterceptHalf.X() << ", " << trackInterceptHalf.Y() << endmsg;

          // fill histos for fit
          ( m_h_res_2d[ module ] )->Fill( phi_state, residual/um );
          ( m_h_resr_2d[ module ] )->Fill( r_state, residual/um );

          // fill scatter plots for later admiration at given iteration
          if ( 1 == m_iteration ) {
            char hname[100], htit[100];
            sprintf( hname, "unbiasedResR_%d", module );
            sprintf( htit, "Unbiased R residual (um) for module %d vs Phi", module );
            plot2D( phi_state, residual/um, hname, htit, -210, 210, -100, 100, 100, 420 );

            sprintf( hname, "unbiasedResRvsR_%d", module );
            sprintf( htit, "Unbiased R residual (um) for module %d vs R", module );
            plot2D( r_state, residual/um, hname, htit, 0, 50, -100, 100, 100, 25 );

            // distinguish between different readout directions, border is roughly at r=14mm
            if ( r_state < 14 ) {
              sprintf( hname, "unbiasedResRBeetle1_%d", module );
              sprintf( htit, "Unbiased R residual (um) for Beetle 1 of module %d vs Phi", module );
              plot2D( phi_state, residual/um, hname, htit, -210, 210, -100, 100, 42, 100 );
            }
            else {
              sprintf( hname, "unbiasedResRBeetle234_%d", module );
              sprintf( htit, "Unbiased R residual (um) for Beetles 234 of module %d vs Phi", module );
              plot2D( phi_state, residual/um, hname, htit, -210, 210, -100, 100, 42, 100 );
            }

            double position = rSensor->rPitch(vcID.strip())/um;
            sprintf( hname, "unbiasedPitchR_%d", module );
            sprintf( htit, "Unbiased R residual (um) for module %d vs pitch", module );
            plot2D( position, residual/um, hname, htit, 30, 105, -100, 100, 15, 200 );
          }
        }
      } // end if R sensor
      else if ( cl_sns == ( module + 64 ) ) { // Phi sensor cluster

        // calculate phi co-ordinate at minimal strip radius minus stereo angle;
        const DeVeloPhiType* phiSensor = m_veloDet->phiSensor( cl_sns );
        int stereo_sign;
        if ( m_ACDC ) {
          stereo_sign = -1;
        }
        else {
          stereo_sign = 1;
        }
        if ( phiSensor->zoneOfStrip( vcID.strip() ) == 0 ) { // inner
          phi_state = phiSensor->phiOfStrip( vcID.strip(), interStripFr, phiSensor->rMin( 0 ) ) / degree;
          phi_state += stereo_sign * -20.04251;     // stereo angle for inner strips
        }
        else {     // outer
          phi_state = phiSensor->phiOfStrip( vcID.strip(), interStripFr, phiSensor->rMin( 1 ) ) / degree;
          phi_state += stereo_sign * 10.35288;    // stereo angle for outer strips
        }

        // transform to VELO half phi state
        verbose() << "Phi phi local is " << phi_state * degree << endmsg;
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
        phi_state = phiStateHalf.Phi() / degree;
        verbose() << "Phi phi half is " << phi_state << endmsg;

        // fill histo for fit
        ( m_h_res_2d[ module + 42 ] )->Fill( phi_state, residual/um );
        z_sen = phiSensor->z();

        // calculate r state of track and plot residual vs r for gamma fit
        trackInterceptGlobal.SetXYZ( x0_x + slope_x * ( z_sen - z0_z ), y0_y + slope_y * ( z_sen - z0_z ), z_sen) ;
        trackInterceptHalf = phiSensor->globalToVeloHalfBox( trackInterceptGlobal );
        r_state = trackInterceptHalf.Rho() / mm;
        debug() << "R state at Phi is " << r_state / um << endmsg;
        ( m_h_resr_2d[ module + 42 ] )->Fill( r_state, residual/um );

        // fill scatter plots for later admiration at given iteration
        if ( 4 == m_iteration ) {
          char hname[100], htit[100];
          if ( phiSensor->zoneOfStrip( vcID.strip() ) == 0 ) { // inner
            sprintf( hname, "unbiasedResPhiInner_%d", module );
            sprintf( htit, "Unbiased Phi residual (um) for module %d vs Phi", module );
            plot2D( phi_state, residual/um, hname, htit, -210, 210, -100, 100, 42, 200 );
          }
          else {          // outer
            sprintf( hname, "unbiasedResPhiOuter_%d", module );
            sprintf( htit, "Unbiased Phi residual (um) for module %d vs Phi", module );
            plot2D( phi_state, residual/um, hname, htit, -210, 210, -100, 100, 42, 200 );
          }
          sprintf( hname, "unbiasedPhiResVsR_%d", module );
          sprintf( htit, "Unbiased Phi residual (um) for module %d vs R", module );
          plot2D( r_state, residual/um, hname, htit, 0, 50, -100, 100, 25, 200 );

          sprintf( hname, "unbiasedPhiResVsRPhi_%d", module );
          sprintf( htit, "Unbiased Phi residual (um) for module %d vs R and Phi", module );
          plot3D( r_state, phi_state, residual/um, hname, htit, 7, 42, -180, 180, -100, 100, 5, 18, 200 );

          double position = phiSensor->phiPitch( r_state ) / um;
          sprintf( hname, "unbiasedPitchPhi_%d", module );
          sprintf( htit, "Unbiased Phi residual (um) for module %d vs pitch", module );
          plot2D( position, residual/um, hname, htit, 30, 105, -100, 100, 15, 200 );
        }
      } // end if Phi sensor
    } // end cluster loop
  } // end residual modules loop

  return StatusCode::SUCCESS;
}

//=============================================================================
// Handle for python calls.
//=============================================================================
void AlignSensors::handle( const Incident& incident ) {
  if ( "FitSensorResiduals" == incident.type() ) {
    debug() << "Calling fitSensorResiduals()" << endmsg;
    fitSensorResiduals();
  }
  else if ( "ResetHistos" == incident.type() ) {
    debug() << "Calling resetHistos()" << endmsg;
    resetHistos();
  }
  else {
    debug() << "Could not handle incident of type " << incident.type() << endmsg;
  }
}

//=============================================================================
// More interface fuzz for python.
//=============================================================================
StatusCode AlignSensors::queryInterface( const InterfaceID& id, void** ppI) {
  // check the placeholder:
  if (0 == ppI) return StatusCode::FAILURE;
  // check ID of the interface
  if ( IIncidentListener::interfaceID() != id ) {
    return Algorithm::queryInterface( id, ppI );
  }
  *ppI = static_cast<IIncidentListener*>( this );
  addRef();
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Reset histograms after fit iteration
//=============================================================================
StatusCode AlignSensors::resetHistos() {

  for ( int i = 0; i < 84; i++ ) {
    ( m_h_res_2d[ i ] )->Reset();
    ( m_h_resr_2d[ i ] )->Reset();
    verbose() << "Histo " << i << " has " << ( m_h_res_2d[ i ] )->GetEntries() << " entries." << endmsg;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Clean histograms to remove low statistics bins
//=============================================================================
StatusCode AlignSensors::cleanHisto( TH2 *hist ) {

  if ( 0 == hist->GetEntries() ) return StatusCode::SUCCESS;

  // histos are residuals (y) vs r or phi (x)
  // loop over bins in r/phi (x)
  for ( int ix = 1; ix <= hist->GetNbinsX(); ix++ ) {
    double colEntries = 0;
    // integrate over residual range (y) for given bin in r/phi (x)
    for ( int iy = 1; iy <= hist->GetNbinsY(); iy++ ) {
      colEntries += hist->GetBinContent( ix, iy );
    }
    // if r/phi bin is not populated enough delete entries
    if ( colEntries < ( m_minNResiduals / 10. ) ) {
      if ( colEntries > 0 ) {
        debug() << "Removing entries in histogram " << hist->GetName() << ", column " << ix << endmsg;
        for ( int iy = 1; iy <= hist->GetNbinsY(); iy++ ) {
          hist->SetBinContent( ix, iy, 0 );
        }
      }
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Fits sensor residuals.
//=============================================================================
StatusCode AlignSensors::fitSensorResiduals() {
  
  info() << "Starting fit iteration " << m_iteration << endmsg;

  // iteration counter used for plotting and triggering some stuff at first iteration
  m_iteration++;

  double dx_R[42], dx_Phi[42];    // x fit results
  double dy_R[42], dy_Phi[42];    // y fit results
  double dx[42], dy[42], dg[42];  // x/y deltas and gamma fit results
  double dxe[42], dye[42];        // x/y fit errors on deltas
  double par[3];    // parameters of fit function
  double *parerr;   // errors of fit function
  bool   rFitted[42];             // mark sations with R sensor fit

  // cleaning of histos
  for ( int i = 0; i < 42; i++ ) {
    cleanHisto( m_h_res_2d[ i ] );
    cleanHisto( m_h_res_2d[ i + 42 ] );
    cleanHisto( m_h_resr_2d[ i + 42 ] );
  }

  // loop over R sensors first
  for ( int i = 0; i < 42; i++ ) {
    rFitted[ i ] = false;
    // check whether residual distribution has enough entries
    if ( ( m_h_res_2d[ i ] )->GetEntries() < m_minNResiduals ) {
      debug() << "Not enough entries to fit R sensor of module " << i << endmsg;
      dx_R[ i ] = 0;
      dy_R[ i ] = 0;
      continue;
    }

    // now fit the R sensor distribution vs phi to get x/y translation
    verbose() << "Generating profile for R sensor of module " << i << endmsg;
    m_h_profile = ( m_h_res_2d[ i ] )->ProfileX();
    verbose() << "Fitting profile." << endmsg;
    m_h_profile->Fit( m_fit_func , "Q" );
    verbose() << "Retrieving fit results." << endmsg;
    m_fit_func->GetParameters( &par[0] );
    parerr = (double*)m_fit_func->GetParErrors();
    debug() << "Fit result for R sensor of module " << i << ": " << par[0] 
            << ", " << par[1] << ", " << par[2] << endmsg;
    debug() << "Fit errors for R sensor of module " << i << ": " << parerr[0] 
            << ", " << parerr[1] << ", " << parerr[2] << endmsg;
    plot1D( fabs(par[0])/parerr[0] + fabs(par[1])/parerr[1], "parsigR", "parsigR", 0, 100, 200);
    dx_R[ i ] = - par[ 1 ] / 1000.; // um to mm
    dy_R[ i ] = - par[ 0 ] / 1000.;
    dxe[ i ] = parerr[ 1 ];
    dye[ i ] = parerr[ 0 ];

    rFitted[ i ] = true;
  }

  int nModulesChanged = 0;

  // loop over Phi sensors, compute difference and determine alignment constant
  for ( int i = 0; i < 42; i++ ) {

    // only a complete set of fits makes sense
    if ( !rFitted[ i ] ) continue;

    // check the initial alignment constants (i.e. current constants at first iteration)
    if ( 1 == m_iteration ) {
      const DeVeloPhiType* phiSensor = m_veloDet->phiSensor( i + 64 ); // Phi sensor numbering offset
      const AlignmentCondition* oldCond = phiSensor->geometry()->alignmentCondition();
      Gaudi::Transform3D oldTrans = oldCond->toNominalMatrix(); // was matrix()
      oldTrans = oldTrans.Inverse();
      Gaudi::Rotation3D rot;
      Gaudi::Transform3D::Vector trans;
      oldTrans.GetDecomposition( rot, trans );
      double rotation[3][3];
      rot.GetComponents( rotation[0][0], rotation[0][1], rotation[0][2],  
           rotation[1][0], rotation[1][1], rotation[1][2],   
           rotation[2][0], rotation[2][1], rotation[2][2] );
      debug() << "Old localDelta, station " << i << ", x = " << trans.X() << ", y = " << trans.Y() 
              << ", gamma = " << rotation[0][1] << endmsg;
      plot1D( trans.X()/um, "Misal_x_before", "Initial x misalignment", -100, 100, 200 );
      plot1D( trans.Y()/um, "Misal_y_before", "Initial y misalignment", -100, 100, 200 );

      Tuple tuple = nTuple( "SensorAlignment", "SensorAlignment" );
      tuple->column( "iteration", 0 );
      tuple->column( "station", i );
      tuple->column( "dx", trans.X() );
      tuple->column( "dy", trans.Y() );
      tuple->column( "dg", rotation[0][1] );
      tuple->write();
    }

    // check whether residual distribution has enough entries
    if ( ( m_h_res_2d[ i + 42 ] )->GetEntries() < m_minNResiduals ) {
      debug() << "Not enough entries to fit Phi sensor of module " << i << endmsg;
      dx_Phi[ i ] = 0;
      dy_Phi[ i ] = 0;
      continue;
    }

    // now fit the Phi sensor distribution vs phi to get x/y translation
    verbose() << "Generating profile for Phi sensor of module " << i << endmsg;
    m_h_profile = ( m_h_res_2d[ i + 42 ] )->ProfileX();
    verbose() << "Fitting profile." << endmsg;
    m_h_profile->Fit( m_fit_func , "Q" );
    verbose() << "Retrieving fit results." << endmsg;
    m_fit_func->GetParameters( &par[0] );
    parerr = (double*)m_fit_func->GetParErrors();
    debug() << "Fit result for Phi sensor of module " << i << ": " << par[0] << ", " 
            << par[1] << ", " << par[2] << endmsg;
    debug() << "Fit errors for Phi sensor of module " << i << ": " << parerr[0] << ", " 
            << parerr[1] << ", " << parerr[2] << endmsg;
    plot1D( fabs(par[0])/parerr[0] + fabs(par[1])/parerr[1], "parsigPhi", "parsigPhi", 0, 100, 200);
    plot2D( m_iteration, m_fit_func->GetChisquare(), "SensorFitChi2", "Sensor fit chi2", 0.5, 20.5, 100, 0, 20, 1000);
    dx_Phi[ i ] = - par[ 0 ] / 1000.; // sign changed
    dy_Phi[ i ] = par[ 1 ] / 1000.; // sign changed
    dxe[ i ] = sqrt( dxe[ i ] * dxe[ i ] + parerr[ 0 ] * parerr[ 0 ] ) / 1000.;
    dye[ i ] = sqrt( dye[ i ] * dye[ i ] + parerr[ 1 ] * parerr[ 1 ] ) / 1000.;

    // check whether residual distribution has enough entries
    if ( ( m_h_resr_2d[ i + 42 ] )->GetEntries() < m_minNResiduals ) {
      debug() << "Not enough entries to fit Phi sensor vs r of module " << i << endmsg;
      dg[ i ] = 0;
      continue;
    }

    // now fit the Phi sensor distribution vs r to get z rotation 
    verbose() << "Generating profile for Phi sensor vs r of module " << i << endmsg;
    m_h_profile = ( m_h_resr_2d[ i + 42 ] )->ProfileX();
    verbose() << "Fitting profile." << endmsg;
    m_h_profile->Fit( m_r_fit_func , "Q" );
    verbose() << "Retrieving fit results." << endmsg;
    m_r_fit_func->GetParameters( &par[0] );
    debug() << "Fit result for Phi sensor vs r of module " << i << ": " << par[0] << ", " << par[1] << endmsg;
    plot2D( m_iteration, m_r_fit_func->GetChisquare(), "SensorRFitChi2", "Sensor r fit chi2", 0.5, 20.5, 100, 0, 20, 1000);
    dg[ i ] = par[ 0 ] / 1000.;

    // subtract R from Phi
    dx[ i ] = dx_Phi[ i ] - dx_R[ i ];
    dy[ i ] = dy_Phi[ i ] - dy_R[ i ];

    // check delta significance
    plot1D( fabs( dx[ i ] ) / dxe[ i ], "deltaSig", "deltaSig", 0, 50, 1000);
    plot1D( fabs( dy[ i ] ) / dye[ i ], "deltaSig", "deltaSig", 0, 50, 1000);
    if ( ( ( fabs( dx[ i ] ) / dxe[ i ] ) < m_minDeltaSig )
         && ( ( fabs( dy[ i ] ) / dye[ i ] ) < m_minDeltaSig ) ) {
      debug() << "No significant delta measured for module " << i << endmsg;
      debug() << "x: " << dx[ i ] << "+/-" << dxe[ i ] << ", y: " << dy[ i ] << "+/-" << dye[ i ] << endmsg;
      continue;
    }

    // 20 micron warning
    if ( ( fabs( dx[ i ] ) > 0.02 ) || ( fabs( dy[ i ] ) > 0.02 ) ) {
      warning() << "Large new globalDelta x = " << dx[ i ] << ", y = " << dy[ i ] << endmsg;
    }

    // monitor deltas. they should decrease with every iteration
    debug() << "New globalDelta x = " << dx[ i ] << ", y = " << dy[ i ] << endmsg;
    plot2D( m_iteration, dx[ i ] / um, "Delta_x", "Delta x misalignment", 0.5, 20.5, -100, 100, 20, 2000 );
    plot2D( m_iteration, dy[ i ] / um, "Delta_y", "Delta y misalignment", 0.5, 20.5, -100, 100, 20, 2000 );
    plot2D( m_iteration, dg[ i ] * 1000., "Misal_gamma", "Gamma misalignment", 0.5, 20.5, -10, 10, 20, 200 );

    // compute the new alignment constants
    // as the alignment is done in the global frame, compute the global transformation...
    Gaudi::XYZVector displ( dx[ i ], dy[ i ], 0. );
    RotationZ rotZ( dg[ i ] );
    Gaudi::Transform3D globalDelta( rotZ, rotZ( displ ) );
    Gaudi::Transform3D halfDelta( rotZ, rotZ( displ ) );

    // ...and transform the global into a local transformation 
    const DeVeloPhiType* phiSensor = m_veloDet->phiSensor( i + 64 ); // Phi sensor numbering offset
    const Gaudi::Transform3D frame = phiSensor->geometry()->parentIGeometryInfo()
      ->parentIGeometryInfo()
      ->parentIGeometryInfo()->toLocalMatrix() 
      * phiSensor->geometry()->toGlobalMatrix();
    // frame = sensorToHalf = VELO_half->toLocal * sensor->toGlobal 	
    const Gaudi::Transform3D localDelta = DetDesc::localDeltaFromAnyFrameDelta( frame, phiSensor->geometry(), halfDelta );

    // now store the new alignment constants
    verbose() << "Retrieving AlignmentCondition object" << endmsg;
    std::string path = phiSensor->geometry()->alignmentCondition()->registry()->identifier();
    AlignmentCondition *myCond = getDet<AlignmentCondition>( path );
    verbose() << "Applying new alignment condition" << endmsg;
    myCond->offNominalMatrix( localDelta );  // was matrix(...)
    updMgrSvc()->invalidate(myCond);
    nModulesChanged++;

    //     // make sure that R sensor constants are set to 0
    //     if ( 1 == m_iteration ) {
    //       const DeVeloRType* rSensor = m_veloDet->rSensor( i );
    //       std::string path = rSensor->geometry()->alignmentCondition()->registry()->identifier();
    //       AlignmentCondition *myCond = getDet<AlignmentCondition>( path );
    //       debug() << "Setting R sensor transformation to identity" << endmsg;
    //       myCond->offNominalMatrix( Gaudi::Transform3D() );  // was matrix(...)
    //       updMgrSvc()->invalidate(myCond);
    //     } 

    // get the decomposition of the transformation for monitoring
    Gaudi::Rotation3D rot;
    Gaudi::Transform3D::Vector trans;
    localDelta.GetDecomposition( rot, trans );
    double rotation[3][3];
    rot.GetComponents( rotation[0][0], rotation[0][1], rotation[0][2],
         rotation[1][0], rotation[1][1], rotation[1][2],
         rotation[2][0], rotation[2][1], rotation[2][2] );
    info() << "New localDelta, station " << i << ", x = " << trans.X() 
           << ", y = " << trans.Y() << ", gamma = " << rotation[0][1] << endmsg;

    // safe constants in ntuple
    Tuple tuple = nTuple( "SensorAlignment", "SensorAlignment" );
    tuple->column( "iteration", m_iteration );
    tuple->column( "station", i );
    tuple->column( "dx", trans.X() );
    tuple->column( "dy", trans.Y() );
    tuple->column( "dg", rotation[0][1] );
    tuple->write();

    // and finally produce some nice plots
    char hname[100], htit[100];

    sprintf( hname, "Misal_x_mod_%d", i );
    sprintf( htit, "X misalignment for module %d", i );
    plot2D( m_iteration, trans.X()/um, hname, htit, 0.5, 20.5, -100, 100, 20, 200 );
    plot2D( m_iteration, trans.X()/um, "Misal_x", "X misalignment", 0.5, 20.5, -100, 100, 20, 200 );

    sprintf( hname, "Misal_y_mod_%d", i );
    sprintf( htit, "Y misalignment for module %d", i );
    plot2D( m_iteration, trans.Y()/um, hname, htit, 0.5, 20.5, -100, 100, 20, 200 );
    plot2D( m_iteration, trans.Y()/um, "Misal_y", "Y misalignment", 0.5, 20.5, -100, 100, 20, 200 );
    if ( i % 2 ) {
      plot2D( m_iteration, trans.X()/um, "Misal_x_odd", "X misalignment odd module number", 0.5, 20.5, -100, 100, 20, 200 );
      plot2D( m_iteration, trans.Y()/um, "Misal_y_odd", "Y misalignment odd module number", 0.5, 20.5, -100, 100, 20, 200 );
      sprintf( hname, "Misal_x_vs_z_odd_it_%d", m_iteration );
      sprintf( htit, "X misalignment vs z odd for iteration %d", m_iteration );
      plot2D( phiSensor->z(), trans.X() / um, hname, htit, -250, 750, -100, 100, 100, 200 );
      sprintf( hname, "Misal_y_vs_z_odd_it_%d", m_iteration );
      sprintf( htit, "Y misalignment vs z odd for iteration %d", m_iteration );
      plot2D( phiSensor->z(), trans.Y() / um, hname, htit, -250, 750, -100, 100, 100, 200 );
    }
    else {
      plot2D( m_iteration, trans.X()/um, "Misal_x_even", "X misalignment even module number", 0.5, 20.5, -100, 100, 20, 200 );
      plot2D( m_iteration, trans.Y()/um, "Misal_y_even", "Y misalignment even module number", 0.5, 20.5, -100, 100, 20, 200 );
      sprintf( hname, "Misal_x_vs_z_even_it_%d", m_iteration );
      sprintf( htit, "X misalignment vs z even for iteration %d", m_iteration );
      plot2D( phiSensor->z(), trans.X() / um, hname, htit, -250, 750, -100, 100, 100, 200 );
      sprintf( hname, "Misal_y_vs_z_even_it_%d", m_iteration );
      sprintf( htit, "Y misalignment vs z even for iteration %d", m_iteration );
      plot2D( phiSensor->z(), trans.Y() / um, hname, htit, -250, 750, -100, 100, 100, 200 );
    }

    sprintf( hname, "Misal_x_vs_z_it_%d", m_iteration );
    sprintf( htit, "X misalignment vs z for iteration %d", m_iteration );
    plot2D( phiSensor->z(), trans.X() / um, hname, htit, -250, 750, -100, 100, 100, 200 );

    sprintf( hname, "Misal_y_vs_z_it_%d", m_iteration );
    sprintf( htit, "Y misalignment vs z for iteration %d", m_iteration );
    plot2D( phiSensor->z(), trans.Y() / um, hname, htit, -250, 750, -100, 100, 100, 200 );

  } // end of Phi sensor loop

  info() << nModulesChanged << " modules' constants updated." << endmsg;
  plot2D( m_iteration, nModulesChanged, "nModulesChanged", 
          "Number of modules with changed constants vs iteration", 0, 20, 0, 50, 20, 50 );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Quick and dirty linear extrapolator from a track to a given z position.
//=============================================================================
Gaudi::XYZPoint AlignSensors::extrapolateToZ(LHCb::Track *track, double toZ)
{
  
  // get state parameters
  Gaudi::XYZPoint coord = track->position();
  Gaudi::XYZVector slope = track->slopes();
  
  Gaudi::XYZPoint result;
  
  // extrapolation distance
  double deltaZ=toZ-coord.z();
  
  //extrapolate
  result.SetXYZ(coord.x()+slope.x()*deltaZ,
  coord.y()+slope.y()*deltaZ,
  toZ);
  
  if (m_printInfo) {             
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


