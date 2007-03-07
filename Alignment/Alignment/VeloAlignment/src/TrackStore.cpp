// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "TrackStore.h"


//-----------------------------------------------------------------------------
// Implementation file for class : TrackStore
//
// 2005-07-29 : Sebastien Viret
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<TrackStore>          s_factory ;
const        IToolFactory& TrackStoreFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackStore::TrackStore( const std::string& type,
                      const std::string& name,
                      const IInterface* parent )
  : GaudiTool ( type, name , parent )
  //Default cuts for the TrackStorePreparation
{
  declareProperty("MomentumCut"   , m_momentum_cut);
  declareProperty("RDiffMax"      , m_RDiffCut);
  declareProperty("NonzerMin"     , m_NonzerCut);
  declareProperty("TakeOverlap"   , m_OverlapCut);
  declareProperty("NonzerOverlap" , m_NonzerOver);
  declareProperty("XOverlapCut"   , m_xOverlapCut);
  declareProperty("MissedHits"    , m_MissedHits);

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

  my_velo = getDet<DeVelo>("/dd/Structure/LHCb/BeforeMagnetRegion/Velo");

  // Printout cuts (debug printout)

  debug() << "" << endmsg;
  debug() << "--------------------------------------------" << endmsg;
  debug() << "Alignment tracks will be selected with the following criteria :" << endmsg;
  debug() << "" << endmsg;
  debug() << "-> Number of valid coordinates >= " << m_NonzerCut << endmsg;
  debug() << "-> Overlap Track Selection     == " << m_OverlapCut << endmsg;
  debug() << "-> Track w. missed hits        == " << m_MissedHits << endmsg;
  debug() << "-> Radius difference w/station <= " << m_RDiffCut << " mm" << endmsg;
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

StatusCode TrackStore::TransformTrack(LHCb::Track* ftrack, LHCb::AlignTrack* atrack, double Map_VELO[])
{

  debug() << "Entering TransformTrack" << endmsg;


  // Start with an a priori good track of undefined type
    
  atrack->setNIsGood(true);
  atrack->setNType(-1);

  m_veloClusters = get<LHCb::VeloClusters>( LHCb::VeloClusterLocation::Default );

  int station;
  int hits[42];
  double r[42];
  double r_s[84];

  double phi[42];
  double phi_s[84];

  for (int j=0; j<42; j++)
  {
    hits[j]         = 0;
    r[j]            = 0.0;
    phi[j]          = 0.0;
    phi[j]          = 0.0;
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
 
      station  = VeloChannel.sensor();

      const DeVeloSensor* m_sensor = my_velo->sensor(station); 
      
      LHCb::State state = ftrack->closestState(m_sensor->z());      

      if ( !(state.x()==state.x()) ) // Because of PatVeloGeneric bug.
      {
	verbose() << "Wow, that's bad NAN stuff!!!!" << endmsg;
	atrack->setNIsGood(false);
	return StatusCode::SUCCESS;
      }

      // Perform a manual extrapolation (quicker)

      double xstate = state.x() + state.tx()*(m_sensor->z() - state.z());
      double ystate = state.y() + state.ty()*(m_sensor->z() - state.z());

      Gaudi::XYZPoint trackInterceptGlobal(xstate, ystate, m_sensor->z());
      Gaudi::XYZPoint trackInterceptLocal(0,0,0) ;      
      m_sensor->globalToLocal(trackInterceptGlobal,trackInterceptLocal);

      double rstate   = trackInterceptLocal.Rho();
      double phistate = trackInterceptLocal.Phi();

      if(rstate != 0.) // Here we need to compute the correct cluster position
      {
	verbose() << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-" << endmsg;
	verbose() << "Track state (in local frame) at z  = " << m_sensor->z() << " :" << endmsg;	  
	verbose() << "-->  Extrapolated phi              = " << phistate << endmsg;
	verbose() << "-->  Extrapolated r                = " << rstate << endmsg;
	  
	// Compute the cluster position

	double sum   = 0.;
	double sums  = 0.;
	double clust_meas = 0.;
	double adc = 0.;
	//	double adcmax = -99.;
	
	std::vector<LHCb::VeloChannelID> channels = VeloClus->channels();
	std::vector<LHCb::VeloChannelID>::const_iterator iChan;
	
	if (VeloChannel.type() == LHCb::VeloChannelID::RType) 
	  rDet=my_velo->rSensor(VeloClus->channelID().sensor());
	
	if (VeloChannel.type() == LHCb::VeloChannelID::PhiType)
	  phiDet=my_velo->phiSensor(VeloClus->channelID().sensor());
	  
	for( iChan = channels.begin() ; iChan !=  channels.end() ; ++iChan ) 
	{
	  if (VeloChannel.type() == LHCb::VeloChannelID::RType) 
	    clust_meas = rDet->rOfStrip( (*iChan).strip() );

	  if (VeloChannel.type() == LHCb::VeloChannelID::PhiType) 
	    clust_meas = phiDet->phiOfStrip( (*iChan).strip(),0.,rstate);
	  
	  adc   = VeloClus->adcValue(iChan-channels.begin());

	  sum  += adc;
	  sums += adc * clust_meas ;
	}
	  	  	    
	if ( 0 < sum ) clust_meas = sums / sum; 
	  
	verbose() << " Cluster on station " << station << endmsg;
	verbose() << " Measure   = " << clust_meas << endmsg;
	verbose() << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-" << endmsg;
	
	// Now we store the cluster and state information in temporary arrays
	// if the values are satisfying the TrackStore cuts.
	
	if (VeloChannel.type() == LHCb::VeloChannelID::RType)  // R cluster
	{	    
	  if (r_s[2*station+1] != 0. && fabs(r_s[2*station+1]-rstate) >= m_RDiffCut)
	  {
	    atrack->setNIsGood(false);
	    return StatusCode::SUCCESS;
	  }	    
	    
	  hits[station]      += 1;
	  r[station]          = clust_meas;	    
	  
	  phi_s[2*station]    = phistate;
	  r_s[2*station]      = rstate;
	}

	if (VeloChannel.type() == LHCb::VeloChannelID::PhiType)  // Phi cluster 
	{		  
	  station = station-64;  // Phi stations are numbered from 64 to 95
	  
	  if (r_s[2*station] != 0. && fabs(r_s[2*station]-rstate) >= m_RDiffCut)
	  {
	    atrack->setNIsGood(false);
	    return StatusCode::SUCCESS;
	  }
	  else if (r[station] == 0.)
	  {
	    r_s[2*station+1] = rstate;
	  }	   
	    
	  hits[station]        += 10;
	  phi[station]          = clust_meas;
	  
	  phi_s[2*station+1]    = phistate;
	  r_s[2*station+1]      = rstate;
	}	  	  
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
    atrack->setNIsGood(false); // Missed hits
    return StatusCode::SUCCESS; // Reject overlap tracks if requested
  }

  for (int j=ialc_right; j<=iblc_right; j++) {if (Map_VELO[j+21] != 0.)  nonzer_right_max++;}
  for (int j=ialc_left; j<=iblc_left; j++)   {if (Map_VELO[j] != 0.)     nonzer_left_max++;}
 
  
  if (nonzer_right != 0 && nonzer_left == 0) // Right track
  {
    if (nonzer_right < m_NonzerCut || (nonzer_right != nonzer_right_max && !m_MissedHits))
    {
      atrack->setNIsGood(false); // Reject track if don't want missed hits or if not enough coord
      return StatusCode::SUCCESS; 
    }

    atrack->setNGoodCoordinates(nonzer_right);
    atrack->setNFirstStation(ialc_right);
    atrack->setNType(1); 
  }
  else if (nonzer_right == 0 && nonzer_left != 0) // Left track
  {
    if (nonzer_left < m_NonzerCut || (nonzer_left != nonzer_left_max && !m_MissedHits))
    {
      atrack->setNIsGood(false); // Reject track if don't want missed hits or if not enough coord
      return StatusCode::SUCCESS; 
    }

    atrack->setNGoodCoordinates(nonzer_left);
    atrack->setNFirstStation(ialc_left);
    atrack->setNType(0);  // Left track
  }
  else if (nonzer_right >= m_NonzerOver && nonzer_left >= m_NonzerOver
	   && (nonzer_right >= m_NonzerCut || nonzer_left >= m_NonzerCut)) // Overlap track
  {
    debug() << "<<<<<<<<<<<<< OVERLAP TRACK >>>>>>>>>>>>>>>" << endmsg;
    TrackStore::AnalyzeOverlap(&hits[0]);
    atrack->setNType(2);  // Overlap track
  }

  if (atrack->nIsGood() == false) return StatusCode::SUCCESS; // Stop here if the track is rejected

  verbose() << "Loop 2 is finished" << endmsg;


  //======================================================
  //
  // Third Loop :
  //
  // This track seems to be definitely interesting
  // We store now its coordinates as spacepoints
  // (see note LHCb 2005-101 for details)
  //
  //======================================================


  Gaudi::XYZPoint trackcoord(0.,0.,0.);
  Gaudi::XYZPoint trackerrors(0.,0.,0.);

  double error_x, error_y, error_r, error_p;

  if (atrack->nType() == 0 || atrack->nType() == 1) // Storage for Non-Overlap track
  {

    verbose() << "First Station  = " << atrack->nFirstStation() << endmsg;

    for (int jj=0; jj<21; jj++) 
    {  
      verbose() << "Into Loop 3: jj = " << jj << endmsg;

      error_r = 0.;
      error_p = 0.;
      error_x = 0.;
      error_y = 0.;

      // Left sensor have even number, right have odd ones

      if (hits[2*jj+atrack->nType()] == 11 && Map_VELO[jj+21*atrack->nType()] != 0.)   // Hit in the detector plane
      {

	int n_stat = 2*jj+atrack->nType();	

	// The space-point (local frame so be careful with the phi's)
	// In fact it appears that local Phi of R sensor is inverted (ACDC geom)
        // They are in the same frame in DC06

	//	double correct_phi = -phi[n_stat]+phi_s[2*n_stat]+phi_s[2*n_stat+1]; // ACDC
	double correct_phi = phi[n_stat]+phi_s[2*n_stat]-phi_s[2*n_stat+1]; // DC06

	const DeVeloSensor* sns = my_velo->sensor(n_stat); 

	double x = r[n_stat]*cos(correct_phi);
	double y = r[n_stat]*sin(correct_phi);
      
	// Space-point is now expressed in the global frame
      
	Gaudi::XYZPoint pointLocal(x, y,0);
	Gaudi::XYZPoint pointGlobal(0,0,0) ;
      
	sns->localToGlobal(pointLocal,pointGlobal);
      
	double x_glo = pointGlobal.x();
	double y_glo = pointGlobal.y();
	double z_glo = pointGlobal.z();

	verbose() << "" << endmsg;
	verbose() << "-> Global coordinates for sensor " << n_stat << endmsg;
	verbose() << "x_spacepoint  = " << x_glo << endmsg;
	verbose() << "y_spacepoint  = " << y_glo << endmsg;
	verbose() << "z_spacepoint  = " << z_glo << endmsg;

	// Errors are calculated as (pitch/sqrt(12)) for the moment (SV 08/07/05)
	
	error_r = (40+(101.6-40.0)*(r[n_stat]-8.19)/(41.95-8.19))/3464.;

	if (r[n_stat] <= 17.2) 
	  error_p = (35.5+(78.3-35.5)*(r[n_stat]-8.19)/(17.2-8.19))/3464.;
	if (r[n_stat] > 17.2)  
	  error_p = (39.3+(96.6-39.3)*(r[n_stat]-17.2)/(41.95-17.2))/3464.;
	
	error_x = sqrt(pow(x*error_r,2)+pow(y*error_p,2))/r[n_stat];
	error_y = sqrt(pow(y*error_r,2)+pow(x*error_p,2))/r[n_stat];

	// Finally fill the AlignTrack

	trackcoord  = Gaudi::XYZPoint(x_glo,y_glo,z_glo);
	trackerrors = Gaudi::XYZPoint(error_x,error_y,float(jj));  

	atrack->addCoord(trackcoord,trackerrors);

	verbose() << "After addcoord" << endmsg;
      }
    }

    // Get the track parameters at the end of this loop

    TrackStore::GetTrackSlope(atrack);

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
	//	phi_o = -phi[jj]+phi_s[2*jj]+phi_s[2*jj+1];  // Corresponding phi state (ACDC)
	phi_o = phi[jj]+phi_s[2*jj]-phi_s[2*jj+1];  // Corresponding phi state (DC06)

	x = r_o*cos(phi_o);
	y = r_o*sin(phi_o);
      }
      else if (hits[jj] == 11 && Map_VELO[jj+21] != 0. && jj%2 == 1)   // Right side hit (rotated along Z)
      {
	r_o   = r[jj];  // Corresponding r state
	//	phi_o = -phi[jj]+phi_s[2*jj]+phi_s[2*jj+1];  // Corresponding phi state (ACDC)
	phi_o = phi[jj]+phi_s[2*jj]-phi_s[2*jj+1];  // Corresponding phi state (DC06)

	x = r_o*cos(phi_o);
	y = r_o*sin(phi_o);
      }
      else  // Useless othewise
      {
	continue;
      }
      

      // Good hit, store it...
      // First space-point is expressed in the global frame

      const DeVeloSensor* sns = my_velo->sensor(jj); 
      
      Gaudi::XYZPoint pointLocal(x, y,0);
      Gaudi::XYZPoint pointGlobal(0,0,0) ;
      
      sns->localToGlobal(pointLocal,pointGlobal);
      
      double x_glo = pointGlobal.x();
      double y_glo = pointGlobal.y();
      double z_glo = pointGlobal.z();

      verbose() << "" << endmsg;
      verbose() << "-> Global coordinates for sensor " << jj << endmsg;
      verbose() << "x_spacepoint  = " << x_glo << endmsg;
      verbose() << "y_spacepoint  = " << y_glo << endmsg;
      verbose() << "z_spacepoint  = " << z_glo << endmsg;
      
      if (fabs(x) >= m_xOverlapCut)  // Not in the overlap region!!! 
      {
	atrack->setNIsGood(false); 
	return StatusCode::SUCCESS; // Bad overlap
      }
      

      // Errors are calculated as (pitch/sqrt(12)) for the moment (SV 08/07/05)

      error_r = (40+(101.6-40.0)*(r_o-8.19)/(41.95-8.19))/3464.;
      
      if (r_o <= 17.2) 
	error_p = (35.5+(78.3-35.5)*(r_o-8.19)/(17.2-8.19))/3464.;
      if (r_o > 17.2)  
	error_p = (39.3+(96.6-39.3)*(r_o-17.2)/(41.95-17.2))/3464.;
      
      error_x = sqrt(pow(x*error_r,2)+pow(y*error_p,2))/r_o;
      error_y = sqrt(pow(y*error_r,2)+pow(x*error_p,2))/r_o;
      
      // Finally fill the AlignTrack
      
      trackcoord  = Gaudi::XYZPoint(x_glo,y_glo,z_glo);
      trackerrors = Gaudi::XYZPoint(error_x,error_y,100.*float(jj)+float(hits[jj]));  //

      n_valid_coord++;
      atrack->addCoord(trackcoord,trackerrors);
      
      verbose() << "After addcoord" << endmsg;
    }

    if (n_valid_coord < m_NonzerCut)
    {
      atrack->setNIsGood(false); // Reject track if not enough coord
      return StatusCode::SUCCESS; 
    }

    TrackStore::GetTrackSlope(atrack);
    debug() << "n_valid_coord : " << n_valid_coord << endmsg;
    atrack->setNGoodCoordinates(n_valid_coord);
  }

  debug() << "Leaving TransformTrack without any trouble" << endmsg;

  return StatusCode::SUCCESS;
}


//------------------------------------------------------------------
//  GETTRACKSLOPE: get straight line parameters
//------------------------------------------------------------------
//
//  atrack      = the track to be processed and updated
//
//------------------------------------------------------------------


StatusCode TrackStore::GetTrackSlope(LHCb::AlignTrack* atrack)
{
  int Ncoords      = atrack->nGoodCoordinates();

  double local_vecx[2];
  double local_matx[2][2];
  double local_vecy[2];
  double local_maty[2][2];
	
  double wghtx, rmeasx, detx;
  double wghty, rmeasy, dety;
  double z_cor;

  for (int i=0; i<2; i++) // reset local params
  {
    local_vecx[i] = 0.0;
    local_vecy[i] = 0.0;

    for (int j=0; j<2; j++)
    {
      local_matx[i][j] = 0.0;
      local_maty[i][j] = 0.0;
    }
  }

  for (int k=0; k<Ncoords; k++)
  {
    z_cor = ((atrack->Coords()[k]).first).z();
    wghtx = ((atrack->Coords()[k]).second).x();
    rmeasx = ((atrack->Coords()[k]).first).x();
    wghty = ((atrack->Coords()[k]).second).y();
    rmeasy = ((atrack->Coords()[k]).first).y();

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

  detx = 0.;
  dety = 0.;

  detx = local_matx[0][0]*local_matx[1][1]-local_matx[1][0]*local_matx[1][0];
  dety = local_maty[0][0]*local_maty[1][1]-local_maty[1][0]*local_maty[1][0];

  if (detx == 0. || dety == 0.) return StatusCode::FAILURE;

  atrack->setNSlope_x((local_vecx[1]*local_matx[0][0]-local_vecx[0]*local_matx[1][0])/detx);
  atrack->setNSlope_y((local_vecy[1]*local_maty[0][0]-local_vecy[0]*local_maty[1][0])/dety);

  atrack->setNXo_x((local_vecx[0]*local_matx[1][1]-local_vecx[1]*local_matx[1][0])/detx);
  atrack->setNYo_y((local_vecy[0]*local_maty[1][1]-local_vecy[1]*local_maty[1][0])/dety);

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

