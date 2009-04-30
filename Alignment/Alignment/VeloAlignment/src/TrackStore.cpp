// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "TrackStore.h"
// velo
#include "VeloDet/DeVelo.h"
#include "TrackInterfaces/IVeloClusterPosition.h"


//-----------------------------------------------------------------------------
// Implementation file for class : TrackStore
//
// 2005-07-29 : Sebastien Viret
//-----------------------------------------------------------------------------


// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TrackStore );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackStore::TrackStore( const std::string& type,
                      const std::string& name,
                      const IInterface* parent )
  : GaudiTool ( type, name , parent )
  //Default cuts for the TrackStorePreparation
{
  declareProperty("NonzerMin"     , m_NonzerCut);
  declareProperty("TakeOverlap"   , m_OverlapCut);
  declareProperty("NonzerOverlap" , m_NonzerOver);
  declareProperty("XOverlapCut"   , m_xOverlapCut);
  declareProperty("MissedHits"    , m_MissedHits);
  declareProperty("ACDC"          , m_ACDC);

  declareInterface<ITrackStore>(this);
}
//=============================================================================
// Destructor
//=============================================================================
TrackStore::~TrackStore() {}; 

//=============================================================================

StatusCode TrackStore::initialize()
{
  
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure())
  {
    return Error("Failed to initialize", sc);
  }

  debug() << "TrackStore Tool launched !!!!" << endmsg;

  //  First retrieve the velo detector element

  my_velo      = getDet<DeVelo>("/dd/Structure/LHCb/BeforeMagnetRegion/Velo");
  m_clusterTool=tool<IVeloClusterPosition>("VeloClusterPosition");
  
  // Printout cuts (debug printout)

  debug() << "" << endmsg;
  debug() << "--------------------------------------------" << endmsg;
  debug() << "Alignment tracks will be selected with the following criteria :" << endmsg;
  debug() << "" << endmsg;
  debug() << "-> Number of valid coordinates >= " << m_NonzerCut << endmsg;
  debug() << "-> Overlap Track Selection     == " << m_OverlapCut << endmsg;
  debug() << "-> Track w. missed hits        == " << m_MissedHits << endmsg;
  debug() << "--------------------------------------------" << endmsg;
  debug() << "" << endmsg;
  

  return StatusCode::SUCCESS;
}


//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////


//------------------------------------------------------------------
//  TRANSFORMTRACK: put the track into a Millepede-friendly format
//------------------------------------------------------------------
//
//  ftrack      = the input track
//  atrack      = the output track (set of spacepoints + fitted track parameters)
//  Map_VELO[]  = array giving the modules to be aligned 
//
//------------------------------------------------------------------

StatusCode TrackStore::TransformTrack(LHCb::Track* ftrack, VeloTrack& atrack, double Map_VELO[])
{

  debug() << "Entering TransformTrack" << endmsg;


  // Start with an a priori good track of undefined type
    
  atrack.setNIsGood(true);
  atrack.setNType(-1);

  m_veloClusters = get<LHCb::VeloClusters>( LHCb::VeloClusterLocation::Default );
  //m_veloClusters = get<LHCb::VeloClusters>("Raw/Velo/Clusters" );
  verbose() << "Retrieved clusters "<< m_veloClusters->size()<< endmsg;

  int station;
  int hits[42];
  double r[42];
  double r_e[42];
  double r_s[84];

  double phi[42];
  double phi_e[42];
  double phi_s[84];

  for (int j=0; j<42; j++)
  {
    hits[j]         = 0;
    r[j]            = 0.0;
    r_e[j]          = 0.0;
    phi[j]          = 0.0;
    phi_e[j]        = 0.0;
    r_s[2*j]        = 0.0;
    phi_s[2*j]      = 0.0;
    r_s[2*j+1]      = 0.0;
    phi_s[2*j+1]    = 0.0;
  }

  //======================================================
  //
  // First Loop: we loop over all track LHCbIDs, looking  
  //             for VELO states. Once a VELO state is found
  //             we get the corresponding cluster and compute its position
  //             (in Millepede we are not using the measurement used for the 
  //              kalman fit)
  //
  //======================================================
  
  for (unsigned int itMeas = 0; itMeas != (ftrack->lhcbIDs()).size(); ++itMeas)
  {

    LHCb::LHCbID channel = (ftrack->lhcbIDs())[itMeas];

    if (channel.isVelo())
    {
      LHCb::VeloChannelID VeloChannel = channel.veloID();
      LHCb::VeloCluster* VeloClus = m_veloClusters->object(VeloChannel);
      verbose() << VeloClus <<" Cluster channel " << VeloChannel << endmsg;
      ///if (!VeloClus) continue;
      
      station  = VeloChannel.sensor();

      const DeVeloSensor* m_sensor = my_velo->sensor(station); 
      
      LHCb::State state = ftrack->closestState(m_sensor->z());      

      if ( !(state.x()==state.x()) ) // Because of PatVeloGeneric bug.
	                             // Should be suppressed asap
      {
        verbose() << "Wow, that's bad NAN stuff!!!!" << endmsg;
        atrack.setNIsGood(false);
        return StatusCode::SUCCESS;
      }

      // Perform a manual extrapolation (quickest)

      double xstate = state.x() + state.tx()*(m_sensor->z() - state.z());
      double ystate = state.y() + state.ty()*(m_sensor->z() - state.z());

      Gaudi::XYZPoint trackInterceptGlobal(xstate, ystate, m_sensor->z());
      Gaudi::XYZPoint trackInterceptLocal(0,0,0) ;      

      trackInterceptLocal = (m_sensor->globalToLocal(trackInterceptGlobal));
      //trackInterceptLocal = (m_sensor->veloHalfBoxToLocal(trackInterceptGlobal));
      

      double rstate   = trackInterceptLocal.Rho();
      double phistate = trackInterceptLocal.Phi();

      LHCb::SiPositionInfo<LHCb::VeloChannelID> toolInfo;
      toolInfo=m_clusterTool->position(VeloClus);

      verbose() << "R sensor? " << rstate  << endmsg;

      if(rstate != 0.) // Here we need to compute the correct cluster position
      {
        unsigned int cl_str = 0;
        double interStripFr = 0.;
        double clust_meas   = 0.;
        verbose() << "VeloChannle strip " << VeloChannel.strip()  << endmsg;
	
        cl_str = VeloChannel.strip();
        verbose() << "VeloClus fraction" << VeloClus  << endmsg;
        interStripFr = VeloClus->interStripFraction();

        verbose() << "VeloChannle Type " << VeloChannel.type()  << endmsg;
		
        if (VeloChannel.type() == LHCb::VeloChannelID::RType)
        {
          rDet=my_velo->rSensor(VeloClus->channelID().sensor());
          
          clust_meas = rDet->rOfStrip(cl_str) + interStripFr*(rDet->rPitch(cl_str));        
          
          hits[station]      += 1;
          r[station]          = clust_meas;	  
          r_e[station]      =  rDet -> rPitch( toolInfo.strip.strip() )
            * toolInfo.fractionalError;
          phi_s[2*station]    = phistate;
          r_s[2*station]      = rstate;
        }
        
        if (VeloChannel.type() == LHCb::VeloChannelID::PhiType)
        {
          phiDet=my_velo->phiSensor(VeloClus->channelID().sensor());
          
          clust_meas = phiDet->phiOfStrip(cl_str,interStripFr,rstate);        
          
          station = station-64;  // Phi stations are numbered from 64 to 95
          hits[station]      += 10;
          phi[station]          = clust_meas;	  
          phi_e[station]      =  phiDet -> phiPitch( toolInfo.strip.strip() )
            * toolInfo.fractionalError;
          phi_s[2*station+1]    = phistate;
          r_s[2*station+1]      = rstate;
        }
        
        verbose() << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-" << endmsg;
        verbose() << "Track state (in local frame) at z  = " << m_sensor->z() << " :" << endmsg;	  
        verbose() << "-->  Extrapolated phi              = " << phistate << endmsg;
        verbose() << "-->  Extrapolated r                = " << rstate << endmsg;
        verbose() << " Cluster on station " << station << endmsg;
        verbose() << " Measure   = " << clust_meas << endmsg;
        verbose() << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-" << endmsg;
        
      }
    }             
  }   // End of loop on LHCbIDs
      
  verbose() << "Loop 1 is finished" << endmsg;


  //======================================================
  //
  // Second Loop: 
  //
  // The first loop on clusters is finished
  // Here we distinguish the 2 sides, in order to unfold overlap tracks
  //
  // EVEN indices == LEFT  side
  // ODD  indices == RIGHT side
  //
  //======================================================  


  int ialc_left        = -1;   // First good hit on left side
  int iblc_left        = 0;    // Last good hit on left side
  int nonzer_left      = 0;    // Number of left good hits
  int nonzer_left_max  = 0;    // Max. possible number of left hits

  int ialc_right       = -1;   // First good hit on right side
  int iblc_right       = 0;    // Last good hit on right side
  int nonzer_right     = 0;    // Number of right good hits
  int nonzer_right_max = 0;    // Max. possible number of right hits

  for (int j=0; j<21; j++)
  {
    if (hits[2*j] == 11 && Map_VELO[j] != 0.)    // Left hit
    {
      verbose() << "Loop 2 left: " << j << endmsg;
      
      nonzer_left++;
      if (ialc_left == -1) ialc_left=j;	        // first station
      iblc_left = j;       	     	        // last  station
    }

    if (hits[2*j+1] == 11 && Map_VELO[j+21] != 0.)    // Right hit
    {
      verbose() << "Loop 2 right: " << j << endmsg;

      nonzer_right++;
      if (ialc_right == -1) ialc_right=j;       // first station
      iblc_right = j;       	     	        // last  station
    }
  }  // End of loop on stations


  if (!m_OverlapCut && nonzer_right != 0 && nonzer_left != 0) // Overlap track
  {
    atrack.setNIsGood(false); // Missed hits
    return StatusCode::SUCCESS; // Reject overlap tracks if requested
  }

  for (int j=ialc_right; j<=iblc_right; j++) {if (Map_VELO[j+21] != 0.)  nonzer_right_max++;}
  for (int j=ialc_left; j<=iblc_left; j++)   {if (Map_VELO[j] != 0.)     nonzer_left_max++;}
 
  
  if (nonzer_right != 0 && nonzer_left == 0) // Right track
  {
    if (nonzer_right < m_NonzerCut || (nonzer_right != nonzer_right_max && !m_MissedHits))
    {
      atrack.setNIsGood(false); // Reject track if don't want missed hits or if not enough coord
      return StatusCode::SUCCESS; 
    }

    atrack.setNGoodCoordinates(nonzer_right);
    atrack.setNFirstStation(ialc_right);
    atrack.setNType(1); 
  }
  else if (nonzer_right == 0 && nonzer_left != 0) // Left track
  {
    if (nonzer_left < m_NonzerCut || (nonzer_left != nonzer_left_max && !m_MissedHits))
    {
      atrack.setNIsGood(false); // Reject track if don't want missed hits or if not enough coord
      return StatusCode::SUCCESS; 
    }

    atrack.setNGoodCoordinates(nonzer_left);
    atrack.setNFirstStation(ialc_left);
    atrack.setNType(0);  // Left track
  }
  else if (nonzer_right >= m_NonzerOver && nonzer_left >= m_NonzerOver
           && (nonzer_right >= m_NonzerCut || nonzer_left >= m_NonzerCut)) // Overlap track
  {
    debug() << "<<<<<<<<<<<<< OVERLAP TRACK >>>>>>>>>>>>>>>" << endmsg;
    //    TrackStore::AnalyzeOverlap(&hits[0]); // Only for debugging purposes
    atrack.setNType(2);  // Overlap track
  }
  
  if (atrack.nIsGood() == false) return StatusCode::SUCCESS; // Stop here if the track is rejected
  
  verbose() << "Loop 2 is finished" << endmsg;
  

  //======================================================
  //
  // Third Loop :
  //
  // This track seems to be definitely interesting
  // We store now its coordinates as spacepoints
  // (see notes LHCb 2005-101 and 2007-067 for details)
  //
  //======================================================


  Gaudi::XYZPoint trackcoord(0.,0.,0.);
  Gaudi::XYZPoint trackerrors(0.,0.,0.);

  double error_x, error_y, error_r, error_p;

  if (atrack.nType() == 0 || atrack.nType() == 1) // Storage for Non-Overlap track
  {

    verbose() << "First Station  = " << atrack.nFirstStation() << endmsg;

    for (int jj=0; jj<21; jj++) 
    {  
      verbose() << "Into Loop 3: jj = " << jj << endmsg;

      error_r = 0.;
      error_p = 0.;
      error_x = 0.;
      error_y = 0.;

      // Left sensor have even number, right have odd ones

      if (hits[2*jj+atrack.nType()] == 11 && Map_VELO[jj+21*atrack.nType()] != 0.)   // Hit in the detector plane
      {

        int n_stat = 2*jj+atrack.nType();	
        
        // The space-point (local frame so be careful with the phi's)
        // In fact it appears that local Phi of R sensor is inverted (ACDC geom)
        // They are in the same frame in DC06
        
        double correct_phi = 0;
        
        (m_ACDC)
          ? correct_phi = -phi[n_stat]+phi_s[2*n_stat]+phi_s[2*n_stat+1] // ACDC
          : correct_phi = phi[n_stat]+phi_s[2*n_stat]-phi_s[2*n_stat+1]; // DC06
        
        const DeVeloSensor* sns = my_velo->sensor(n_stat); 
        
        double x = r[n_stat]*cos(correct_phi);
        double y = r[n_stat]*sin(correct_phi);
        
        // Space-point is now expressed in the global frame
        
        Gaudi::XYZPoint pointLocal(x, y,0);
        Gaudi::XYZPoint pointGlobal(0,0,0) ;
        
        pointGlobal = sns->localToGlobal(pointLocal);
        //pointGlobal = sns->localToVeloHalfBox(pointLocal);
        
        double x_glo = pointGlobal.x();
        double y_glo = pointGlobal.y();
        double z_glo = pointGlobal.z();
        
        verbose() << "" << endmsg;
        verbose() << "-> Global coordinates for sensor " << n_stat << endmsg;
        verbose() << "x_spacepoint  = " << x_glo << endmsg;
        verbose() << "y_spacepoint  = " << y_glo << endmsg;
        verbose() << "z_spacepoint  = " << z_glo << endmsg;
        verbose() << "-> Local coordinates for sensor " << n_stat << endmsg;
        verbose() << "x_spacepoint  = " << pointLocal.x() << endmsg;
        verbose() << "y_spacepoint  = " << pointLocal.y() << endmsg;
  
        
        // Errors are calculated as (pitch/sqrt(12)) for the moment (SV 08/07/05)        
        error_r = (40+(101.6-40.0)*(r[n_stat]-8.19)/(41.95-8.19))/3464.;
        
        if (r[n_stat] <= 17.2) 
          error_p = (35.5+(78.3-35.5)*(r[n_stat]-8.19)/(17.2-8.19))/3464.;
        if (r[n_stat] > 17.2)  
          error_p = (39.3+(96.6-39.3)*(r[n_stat]-17.2)/(41.95-17.2))/3464.;
        
        // error_r=r_e[n_stat];
        //error_p=phi_e[n_stat];
        error_x = sqrt(pow(x*error_r,2)+pow(y*error_p,2))/r[n_stat];
        error_y = sqrt(pow(y*error_r,2)+pow(x*error_p,2))/r[n_stat];
        verbose()<< "ERR "<<error_r<< " " << error_p<<endmsg;
        
        // Finally fill the AlignTrack
        
        trackcoord  = Gaudi::XYZPoint(x_glo,y_glo,z_glo);
        trackerrors = Gaudi::XYZPoint(error_x,error_y,float(jj));  
        
        atrack.addCoord(trackcoord,trackerrors);
        
        verbose() << "After addcoord" << endmsg;
      }
    }
    
    // Finally get the track parameters at the end of this loop
    
    atrack.GetTrackSlope();
  } 
  else   // Overlap treatment (Here we express the coordinates in the VELO frame (not the box))
  {
    int n_valid_coord = 0;
    
    for (int jj=0; jj<42; jj++) 
    {  
      error_r = 0.;
      error_p = 0.;
      error_x = 0.;
      error_y = 0.;
      double x = 0.;
      double y = 0.;

      double r_o   = 0.;
      double phi_o = 0.;


      // The space-point (local frame so be careful with the phi's)
      // In fact it appears that local Phi of R sensor is inverted (ACDC geom)
      // They are in the same frame in DC06

      
      if (hits[jj] == 11 && Map_VELO[jj] != 0. && jj%2 == 0) // Left side hit
      {
        r_o   = r[jj];  // Corresponding r state
        
        (m_ACDC)
          ? phi_o = -phi[jj]+phi_s[2*jj]+phi_s[2*jj+1]  // Corresponding phi state (ACDC)
          : phi_o = phi[jj]+phi_s[2*jj]-phi_s[2*jj+1];  // Corresponding phi state (DC06)
        
        x = r_o*cos(phi_o);
        y = r_o*sin(phi_o);
      }
      else if (hits[jj] == 11 && Map_VELO[jj+21] != 0. && jj%2 == 1)   // Right side hit (rotated along Z)
      {
        r_o   = r[jj];  // Corresponding r state
        
        (m_ACDC)
          ? phi_o = -phi[jj]+phi_s[2*jj]+phi_s[2*jj+1]  // Corresponding phi state (ACDC)
          : phi_o = phi[jj]+phi_s[2*jj]-phi_s[2*jj+1];  // Corresponding phi state (DC06)
        
        x = r_o*cos(phi_o);
        y = r_o*sin(phi_o);
      }
      else  // Useless otherwise
      {
        continue;
      }
      

      // Good hit, store it...
      // First space-point is expressed in the half-box frame

      const DeVeloSensor* sns = my_velo->sensor(jj); 
      
      Gaudi::XYZPoint pointLocal(x, y,0);
      Gaudi::XYZPoint pointGlobal(0,0,0) ;
      
      pointGlobal = sns->localToGlobal(pointLocal);
      
      double x_glo = pointGlobal.x();
      double y_glo = pointGlobal.y();
      double z_glo = pointGlobal.z();

      verbose() << "" << endmsg;
      verbose() << "-> Global coordinates for sensor " << jj << endmsg;
      verbose() << "x_spacepoint  = " << x_glo << endmsg;
      verbose() << "y_spacepoint  = " << y_glo << endmsg;
      verbose() << "z_spacepoint  = " << z_glo << endmsg;
      verbose() << "-> Local coordinates for sensor " << jj << endmsg;
      verbose() << "x_spacepoint  = " << pointLocal.x() << endmsg;
      verbose() << "y_spacepoint  = " << pointLocal.y() << endmsg;
      
      if (fabs(x) >= m_xOverlapCut)  // Not in the overlap region!!! 
      {
        atrack.setNIsGood(false); 
        return StatusCode::SUCCESS; // Bad overlap
      }
      

      // Errors are calculated as (pitch/sqrt(12)) for the moment (SV 08/07/05)
      error_r = (40+(101.6-40.0)*(r_o-8.19)/(41.95-8.19))/3464.;
      if (r_o <= 17.2) 
        error_p = (35.5+(78.3-35.5)*(r_o-8.19)/(17.2-8.19))/3464.;
      if (r_o > 17.2)  
        error_p = (39.3+(96.6-39.3)*(r_o-17.2)/(41.95-17.2))/3464.;
      
      //error_r=r_e[jj];
      //error_p=phi_e[jj];

      error_x = sqrt(pow(x*error_r,2)+pow(y*error_p,2))/r_o;
      error_y = sqrt(pow(y*error_r,2)+pow(x*error_p,2))/r_o;
      verbose()<< "ERR "<<error_r<< " " << error_p<<endmsg;
       
      // Finally fill the AlignTrack
      
      trackcoord  = Gaudi::XYZPoint(x_glo,y_glo,z_glo);
      trackerrors = Gaudi::XYZPoint(error_x,error_y,100.*float(jj)+float(hits[jj]));  //

      n_valid_coord++;
      atrack.addCoord(trackcoord,trackerrors);
      
      verbose() << "After addcoord" << endmsg;
    }
    

    if (n_valid_coord < m_NonzerCut)
    {
      atrack.setNIsGood(false); // Reject track if not enough coord
      return StatusCode::SUCCESS; 
    }

    debug() << "n_valid_coord : " << n_valid_coord << endmsg;
    atrack.setNGoodCoordinates(n_valid_coord);
    atrack.GetTrackSlope();
  }

  debug() << "Leaving TransformTrack without any trouble" << endmsg;

  return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------
//  AnalyzeOverlap: get info on overlap track
//---------------------------------------------------------------------
//
//  hits[]      = array containing the nature of hits in all 42 modules
//
//---------------------------------------------------------------------

StatusCode TrackStore::AnalyzeOverlap(int hits[])
{

  debug() << "--------------------------" << endmsg;
  debug() << "|    OVERLAP ANALYZER    |" << endmsg;
  debug() << "--------------------------" << endmsg;
  debug() << "| Station | Left | Right |" << endmsg;
  debug() << "--------------------------" << endmsg;

  std::string Rig,Lef;

  for (int i=0; i<21; i++)
  {
    switch (hits[2*i])
    {
    case 0  :  Lef = "0";break;
    case 1  :  Lef = "R";break;
    case 10 :  Lef = "Phi";break;
    case 11 :  Lef = "GOOD";break;
    default :  Lef = "???";break;
    }
    switch (hits[2*i+1])
    {
    case 0  :  Rig = "0";break;
    case 1  :  Rig = "R";break;
    case 10 :  Rig = "Phi";break;
    case 11 :  Rig = "GOOD";break;
    default :  Rig = "???";break;
    }

    debug() << "|   " << std::setw(2) << i 
            << "    | " << std::setw(4) << Lef 
            << " | " << std::setw(4) << Rig << "|" << endmsg;   
  }

  debug() << "|_________|______|_______|" << endmsg;

  return StatusCode::SUCCESS; 
}

