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

    ,m_momentum_cut (0.)
    ,m_longCut(true)
    ,m_RDiffCut(100.)
    ,m_RMin(10.)
    ,m_NonzerCut(3)
    ,m_OverlapCut(false)
    ,m_MissedHits(false)
    ,m_SelectOption(0)
    ,m_extrapolator(0)
{
  declareProperty("MomentumCut", m_momentum_cut);
  declareProperty("LongCut", m_longCut);
  declareProperty("RDiffMax", m_RDiffCut);
  declareProperty("RMin", m_RMin);
  declareProperty("NonzerMin", m_NonzerCut);
  declareProperty("TakeOverlap", m_OverlapCut);
  declareProperty("MissedHits", m_MissedHits);
  declareProperty("Option", m_SelectOption);
  declareProperty("Extrapolator", m_extrapolatorName = "TrackMasterExtrapolator");

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

  // Extrapolator for retrieving track states
  
  m_extrapolator = tool<ITrackExtrapolator>(m_extrapolatorName);
  if (sc.isFailure())
  {
    return Error("Failed to initialize extrapolator", sc);
  }

  // Show the cuts (debug printout)

  debug() << "" << endmsg;
  debug() << "--------------------------------------------" << endmsg;
  debug() << "Alignment tracks will be selected with the following criteria :" << endmsg;
  debug() << "" << endmsg;
  debug() << "-> Track Momentum              >= " << m_momentum_cut << " GeV/c" << endmsg;
  debug() << "-> Number of valid coordinates >= " << m_NonzerCut << endmsg;
  debug() << "-> Overlap Track Selection     == " << m_OverlapCut << endmsg;
  debug() << "-> Only Long track             == " << m_longCut << endmsg;
  debug() << "-> Track w. missed hits        == " << m_MissedHits << endmsg;
  debug() << "-> Radius difference w/station <= " << m_RDiffCut << " mm" << endmsg;
  debug() << "" << endmsg;
  debug() << "-> Track Selection Method is set to " << m_SelectOption << endmsg;
  debug() << "--------------------------------------------" << endmsg;
  debug() << "" << endmsg;

  return StatusCode::SUCCESS;
}


StatusCode TrackStore::TransformTrack(Track* ftrack, AlignTrack* atrack, double Map_VELO[], DeVelo* my_velo, 
				      double residual_r[], double residual_phi[])
{

  debug() << "Entering TransformTrack" << endmsg;
    
  double momentum = -1.;
      
  atrack->setNIsGood(true);

  momentum = 1.;

  m_veloClusters = get<VeloClusters>( VeloClusterLocation::Default );

  // First look if the track pass the momentum cut and th long cut
  // Don't go further otherwise

  if (momentum <= m_momentum_cut)
  {
      atrack->setNIsGood(false);
      return StatusCode::SUCCESS;
  }

  // Then loop over track hits

  int station, j;
  int hits[42];
  double r[42];
  double z[42];
  double phi[42];

  double sum, sums;
  double phistate, rstate;
  double clust_meas, fphi, rcorrect;

  for (j=0; j<42; j++)
  {
    hits[j] = 0;
    r[j] = 0.0;
    phi[j] = 0.0;
    z[j] = 0.0;
    residual_r[j] = -999.0;
    residual_phi[j] = -999.0;
  }

  for (unsigned int itMeas = 0; itMeas != (ftrack->lhcbIDs()).size(); ++itMeas)
  {

    LHCbID channel = (ftrack->lhcbIDs())[itMeas];

    if (channel.isVelo())
    {
      VeloChannelID VeloChannel = channel.veloID();
      VeloCluster* VeloClus = m_veloClusters->object(VeloChannel);

      station  = VeloChannel.sensor();
      phistate = 0.;
      rstate   = 0.;

      ParticleID partID = 211;
      
      State state = ftrack->closestState(my_velo->zSensor(station));      
      StatusCode sc = m_extrapolator->propagate(state,my_velo->zSensor(station),partID);

      if (sc)
      {
	rstate   = sqrt(state.x()*state.x()+state.y()*state.y());
	phistate = atan2(state.y(),state.x());

	if(rstate != 0.) // Here we need to compute the correct cluster position
	{
	  verbose() << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-" << endmsg;
	  verbose() << "Track state at z = " << my_velo->zSensor(station) << " :" << endmsg;	  
	  verbose() << "-->  Extrap. phi  = " << phistate << endmsg;
	  verbose() << "-->  Extrap. r    = " << rstate << endmsg;

	  // Compute the cluster position

	  sum   = 0.;
	  sums  = 0.;
	  clust_meas = 0.;
	  int sensor = VeloClus->sensor();	  
	  	  	 	  	  	   	    
	  std::vector< std::pair<long,double> > sign = VeloClus->stripSignals();
	  std::vector< std::pair<long,double> >::const_iterator strIt;
	  int strip    = (*sign.begin()).first;
	  VeloChannelID channelstrip(sensor,strip);

	  for ( strIt = sign.begin() ; sign.end() != strIt ; strIt++ )
	  {
	    strip      = (*strIt).first;
	    channelstrip.setStrip(strip);

	    if (VeloChannel.type() == VeloChannelID::RType)   
	      clust_meas = my_velo->rOfStrip( channelstrip ); 
	    if (VeloChannel.type() == VeloChannelID::PhiType) 
	      clust_meas = my_velo->trgPhiOfStrip( channelstrip,rstate ); 
	    /*
	    if( strIt == sign.begin() && VeloChannel.type() == VeloChannelID::PhiType){

	      // if phi cluster get the global positions of the ends of the first strip
	      HepPoint3D end1,end2;
	      StatusCode sc = my_velo->stripLimitsPhi(sensor,strip,end1,end2);
	      if(!sc) return sc;

	      // get global phi correction due to misalignments (for phi cluster)

	      fphi = atan2(end2.y()-end1.y(),end2.x()-end1.x())
		-my_velo->trgPhiDirectionOfStrip(channel);  

	      if (fabs(fphi) > 4.*atan(1.)) fphi = fphi-fabs(fphi)/fphi*(8.*atan(1.));
	    }
	    */	    
	    sum       += (*strIt).second;
	    sums      += (*strIt).second * clust_meas ;
	  }
	    
	  if ( 0 < sum ) clust_meas = sums / sum; 


	  // Here we compute the misaligned position for a R cluster
	  /*
	  if (VeloChannel.type() == VeloChannelID::RType) 
	  {
	    HepPoint3D local1,global1;
	    HepPoint3D local2,global2;
	    global1.set(state.x(),state.y(),state.z());
	    my_velo->globalToLocal(sensor,global1,local1);

	    double lPhi = local1.phi();
	    double lR   = clust_meas;
   	     	  
	    // now make a local point using local R and phi.
	    local1.set(lR*cos(lPhi), lR*sin(lPhi), 0.);
	    // and point at local center of circle
	    local2.set(0., 0., 0.);

	    my_velo->localToGlobal(sensor,local1,global1);
	    my_velo->localToGlobal(sensor,local2,global2);

	    double lx = state.x();
	    double ly = state.y();
	    double gx = global1.x()-global2.x();
	    double gy = global1.y()-global2.y();

	    verbose() << " Origin of station " << station << endmsg;
	    verbose() << " x  = " << global2.x() << endmsg;
	    verbose() << " y  = " << global2.y() << endmsg;
	    verbose() << " z  = " << global2.z() << endmsg;
	  
	    //	  if (lx*gx < 0.) gx = fabs(gx/lx)*lx; // Give to gx the sign of lx
	    //	  if (ly*gy < 0.) gy = fabs(gy/ly)*ly; // Give to gy the sign of ly
	    //	  (gx < 0.) ? philine = atan2(-gy,-gx) : philine = atan2(gy,gx);
	  	  	
	    double philine = atan2(gy,gx);

	    // position = d0 of line at 90deg to phi that goes through global1
	    // if y = m.x + c ; m = tan(phi+pi/2), c = tan(phi+pi/2) x_1 - y_1  
	    // d0 = c / sqrt(1 + m^2)  ; and 1/sqrt(1+tan2(phi)) = cos(phi)

	    //	    clust_meas = fabs(sin(philine)*global1.y()+cos(philine)*global1.x());

	    rcorrect = sqrt(global1.y()*global1.y()+global1.x()*global1.x());
	  }
	  */	    
	  verbose() << " Cluster on station " << station << endmsg;
	  verbose() << " Measure   = " << clust_meas << endmsg;
	  /*
	  if (VeloChannel.type() == VeloChannelID::RType) {verbose() << " Corrected Measure  = " << rcorrect << endmsg;}
	  if (VeloChannel.type() == VeloChannelID::PhiType) {verbose() << " Corrected Measure  = " << clust_meas+fphi << endmsg;}

	  if (VeloChannel.type() == VeloChannelID::RType) {verbose() << " Residual Value = " << rcorrect-rstate << endmsg;}
	  if (VeloChannel.type() == VeloChannelID::PhiType) {verbose() << " Residual Value = " << clust_meas+fphi-phistate << endmsg;}
	  */
	  verbose() << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-" << endmsg;

          // Misalignments not taken into account for the moment (25/01/2006)

	  //	  fphi = 0.;
	  //          rcorrect = clust_meas;

	  if (VeloChannel.type() == VeloChannelID::RType) 
	  {	    
	    if (r[station] != 0. && fabs(r[station]-rstate) >= m_RDiffCut)
	    {
	      atrack->setNIsGood(false);
	      return StatusCode::SUCCESS;
	    }	    
	    
	    if (clust_meas >= m_RMin) hits[station] += 1;
	    residual_r[station] = clust_meas-rstate;
	    r[station]     = clust_meas;
	    z[station]     = my_velo->zSensor(station);
	    phi[station]  += phistate;
	  }

	  if (VeloChannel.type() == VeloChannelID::PhiType) 
	  {	    
	    station = station-64;

	    if (r[station] != 0. && fabs(r[station]-rstate) >= m_RDiffCut)
	    {
	      atrack->setNIsGood(false);
	      return StatusCode::SUCCESS;
	    }
	    else if (r[station] == 0.)
	    {
	      r[station] = rstate;
	    }	   

	    //	    clust_meas += fphi;
	    
	    residual_phi[station] = clust_meas-phistate;
	    hits[station] += 10;
	    phi[station]  += clust_meas-phistate;

	  }	  	  
	}
      }
    }
  }
      
  verbose() << "Loop 1 is finished" << endmsg;

//
// The first loop on clusters is finished
// We distinguish the 2 sides, in order to unfold overlap tracks
//

  int ialc_left        = -1;
  int iblc_left        = 0;
  int nonzer_left      = 0;
  int nonzer_left_max  = 0;

  int ialc_right       = -1;
  int iblc_right       = 0;
  int nonzer_right     = 0;
  int nonzer_right_max = 0;

  for (j=0; j<21; j++)
  {
    if (hits[2*j] == 11 && hits[2*j+1] == 0 && Map_VELO[j] != 0.)    // Left hit with no overlap
    {
      verbose() << "Loop 2 ln: " << j << endmsg;

      nonzer_left++;
      if (ialc_left == -1) ialc_left=j;	        // first station
      iblc_left = j;       	     	        // last  station
    }
    if (hits[2*j] == 11 && hits[2*j+1] != 0 && Map_VELO[j] != 0.)    // Left hit with overlap
    {
      verbose() << "Loop 2 lo: " << j << endmsg;

      if (m_OverlapCut) nonzer_left++; // If we consider overlaps this coordinate
                                                   // is valid
      if (ialc_left == -1) ialc_left=j;	
      iblc_left = j;       	     	
      atrack->setNType(2);
    }

    if (hits[2*j] == 0 && hits[2*j+1] == 11 && Map_VELO[j+21] != 0.)    // Right hit with no overlap
    {
      verbose() << "Loop 2 rn: " << j << endmsg;

      nonzer_right++;
      if (ialc_right == -1) ialc_right=j;       // first station
      iblc_right = j;       	     	        // last  station
    }
    if (hits[2*j] != 0 && hits[2*j+1] == 11 && Map_VELO[j+21] != 0.)    // Right hit with overlap
    {
      verbose() << "Loop 2 ro: " << j << endmsg;

      if (m_OverlapCut) nonzer_right++; // If we consider overlaps this coordinate
                                                    // is valid
      if (ialc_right == -1) ialc_right=j;	
      iblc_right = j;     
      atrack->setNType(2);  	     	
    }
  }

  for (j=ialc_right; j<=iblc_right; j++) {if (Map_VELO[j+21] != 0.)  nonzer_right_max++;}
  for (j=ialc_left; j<=iblc_left; j++)   {if (Map_VELO[j] != 0.)     nonzer_left_max++;}

  verbose() << "Loop 2 : " << nonzer_left << " / " << nonzer_left_max << endmsg;
  verbose() << "Loop 2 : " << nonzer_right << " / " << nonzer_right_max << endmsg;
  
  if (atrack->nType() == 2 && !m_OverlapCut) // Reject overlap if requested
  {
      atrack->setNIsGood(false);
      return StatusCode::SUCCESS;
  }
  
  if (nonzer_right >= nonzer_left)
  {
    if (nonzer_right < m_NonzerCut) atrack->setNIsGood(false);       // Not enough coordinates
    if (nonzer_right != nonzer_right_max && !m_MissedHits) 
      atrack->setNIsGood(false); // Missed hits

    atrack->setNGoodCoordinates(nonzer_right);
    atrack->setNFirstStation(ialc_right);
    atrack->setNType(1);  // Right track
  }
  else
  {
    if (nonzer_left < m_NonzerCut) atrack->setNIsGood(false);       // Not enough coordinates
    if (nonzer_left != nonzer_left_max && !m_MissedHits) 
      atrack->setNIsGood(false); // Missed hits

    atrack->setNGoodCoordinates(nonzer_left);
    atrack->setNFirstStation(ialc_left);
    atrack->setNType(0);  // Left track
  }

  if (atrack->nIsGood() == false) return StatusCode::SUCCESS; // Stop here if the track is rejected

  verbose() << "Loop 2 is finished" << endmsg;

//
// This track seems to be definitely interesting
// We store now its coordinates
//

  HepPoint3D trackcoord(0.,0.,0.);
  HepPoint3D trackerrors(0.,0.,0.);
  double x, y;
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
      x = 0.;
      y = 0.;

      // Left sensor have even number, right have odd ones

      if (hits[2*jj+atrack->nType()] == 11)   // Hit in the detector plane
      {

	verbose() << "Here " << 2*jj+atrack->nType() << endmsg;
	x = r[2*jj+atrack->nType()]*cos(phi[2*jj+atrack->nType()]);
	y = r[2*jj+atrack->nType()]*sin(phi[2*jj+atrack->nType()]);

	verbose() << "Herex " << x << endmsg;
	verbose() << "Herey " << y << endmsg;
	// Errors are calculated as (pitch/sqrt(12)) for the moment (SV 08/07/05)
	
	error_r = (40+(101.6-40.0)*(r[2*jj+atrack->nType()]-8.19)/(41.95-8.19))/3464.;

	if (r[2*jj+atrack->nType()] <= 17.2) 
	  error_p = (35.5+(78.3-35.5)*(r[2*jj+atrack->nType()]-8.19)/(17.2-8.19))/3464.;
	if (r[2*jj+atrack->nType()] > 17.2)  
	  error_p = (39.3+(96.6-39.3)*(r[2*jj+atrack->nType()]-17.2)/(41.95-17.2))/3464.;
	
	error_x = sqrt(pow(x*error_r,2)+pow(y*error_p,2))/r[2*jj+atrack->nType()];
	error_y = sqrt(pow(y*error_r,2)+pow(x*error_p,2))/r[2*jj+atrack->nType()];

	// Finally fill the AlignTrack

	trackcoord  = HepPoint3D(x,y,z[2*jj+atrack->nType()]);
	trackerrors = HepPoint3D(error_x,error_y,float(jj));  // Not a nice implementation, but at least works

	if (atrack->nType()==1 && Map_VELO[jj+21] != 0.) atrack->addCoord(trackcoord,trackerrors);
	if (atrack->nType()==0 && Map_VELO[jj] != 0.) atrack->addCoord(trackcoord,trackerrors);
	verbose() << "After addcoord" << endmsg;
      }
    }

    GetTrackSlope(atrack);

  }
  else
  {
    // Overlap treatment (Not yet implemented)
  }

  debug() << "Leaving TransformTrack without any trouble" << endmsg;

  return StatusCode::SUCCESS;
}

StatusCode TrackStore::GetTrackSlope(AlignTrack* atrack)
{
  int Ncoords      = atrack->nGoodCoordinates();

  int i,j,k;

  double local_vecx[2];
  double local_matx[2][2];
  double local_vecy[2];
  double local_maty[2][2];
	
  double wghtx, rmeasx, detx;
  double wghty, rmeasy, dety;
  double z_cor;

  for (i=0; i<2; i++) // reset local params
  {
    local_vecx[i] = 0.0;
    local_vecy[i] = 0.0;

    for (j=0; j<2; j++)
    {
      local_matx[i][j] = 0.0;
      local_maty[i][j] = 0.0;
    }
  }

  for (k=0; k<Ncoords; k++)
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

  return StatusCode::SUCCESS; 
}
