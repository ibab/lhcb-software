// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "DetDesc/3DTransformationFunctions.h"
#include "DetDesc/GlobalToLocalDelta.h"


// local
#include "Align.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Align
//
// 2005-07-28 : Sebastien Viret
//-----------------------------------------------------------------------------


DECLARE_ALGORITHM_FACTORY( Align );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Align::Align( const std::string& name,
                      ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator )
  // The following variables will be set via a joboptions files
{
  declareProperty("Internal_Alignment"        , m_step1);
  declareProperty("Internal_Mod_Left"         , m_VELOmap_l);
  declareProperty("Internal_Mod_Right"        , m_VELOmap_r);
  declareProperty("Internal_DOF"              , m_align);
  declareProperty("Internal_PTerms"           , m_sigma);
  declareProperty("Internal_EQs"              , m_constrain);
  declareProperty("Internal_Residual_Cut"     , m_residual_cut);
  declareProperty("Box_Alignment"             , m_step2);
  declareProperty("Box_VELOopen"              , m_VELOopen);
  declareProperty("Box_TrackperPV"            , m_trackmin);
  declareProperty("Box_DOF"                   , m_alignb);
  declareProperty("Box_PTerms"                , m_sigmab);
  declareProperty("Box_EQs"                   , m_constrainb);
  declareProperty("Box_Residual_Cut"          , m_residual_cutb);
  declareProperty("Box_Z_window"              , m_z_range);
  declareProperty("Box_MinTracks_perPV"       , m_PV_trackmin);
  declareProperty("Box_PV_Z_window"           , m_z_sigma);
  declareProperty("Box_PV_IPmax"              , m_IPmax);
  declareProperty("Box_PV_Track_IPmax"        , m_TrIPmax);
  declareProperty("General_Startfactor"       , m_starfactr);
  declareProperty("General_Maxtracks"         , m_maxtrack);
  declareProperty("Monitor_Constants"         , m_moni_constants);   
  declareProperty("Monitor_PV"                , m_moni_PV);  
  declareProperty("Monitor_Overlaps"          , m_moni_overlaps);  
  declareProperty("Monitor_Tracks"            , m_moni_tracks);  
  declareProperty("Monitor_Tracks_num"        , m_nTrackSample);  
  declareProperty("TrackStoreTool"            , my_TrackStore = "TrackStore");
  declareProperty("MillepedeTool"             , my_Millepede  = "Millepede");
}
//=============================================================================
// Destructor
//=============================================================================
Align::~Align() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode Align::initialize() {
  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  nEvents = 0;          // Total number of events (event tag is necessary for box alignment)
  nTrackSample = 0;     // Number of track in the blind control sample

  nLeft_tracks  = 0;    // Number of left tracks
  nRight_tracks = 0;    // Number of right tracks

  Align::DefineVeloMap();         // Define the stations to be aligned
  Align::GetAlignmentConstants(); // Get the previous constants from the CondDB  


//  The TrackStore TOOL is performing the track selection before MILLEPEDE feeding
//  it transforms Tracks into AlignTracks objects which are formatted for MILLEPEDE
//
//  Selection cuts are set externally via the Joboption ($VELOALIGNOPTS/TrackStore.opts)  
//  Selection cuts for Pattern Recognition are set in $VELOALIGNOPTS/TRGtune.opts  

  
  my_tracks = tool<ITrackStore>(my_TrackStore);

  selected_tracks.clear(); // Containers of align tracks 
  control_tracks.clear();  // Control sample (tracks not used for alignment)
  overlap_tracks.clear();  // Containers of overlap tracks 

  PV_tracks.clear();                // (needed for STEP2: PV tracks)
  my_PV_finder    = new PVHisto();  // The PV finding tool

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode Align::execute() {

  debug() << "==> Execute" << endmsg;

  ++nEvents;
  int nTracks = 0;

  // Get the Tracks
 
  LHCb::Tracks* tracks = get<LHCb::Tracks>( LHCb::TrackLocation::Velo );
  //  LHCb::Tracks* tracks = get<LHCb::Tracks>( LHCb::TrackLocation::Default );

  debug() << "Retrieved Tracks       : " << tracks->size() << endmsg;
     
  if ( tracks->size() == 0 || tracks->size() >= m_maxtrack ){
    debug() << "Event skipped (busy or no tracks)" << endmsg;
    return StatusCode::SUCCESS;
  }

  LHCb::Tracks::const_iterator iTrack;

  for (iTrack = tracks->begin(); iTrack != tracks->end(); ++iTrack ) 
  {    
    ++nTracks;

    LHCb::Track* my_fitted_track = *iTrack ;          // retrieve track

    //    if (my_fitted_track->type() == LHCb::Track::Velo || 
    //	my_fitted_track->type() == LHCb::Track::Long ||
    //	my_fitted_track->type() == LHCb::Track::Upstream)
    {

      VeloTrack my_align_track  = VeloTrack();  // create AlignTrack 

      my_align_track.setNEvent(nEvents);
      my_align_track.setNTrack(nTracks);
      
      //   Select the track according to the cuts defined in $VELOALIGNOPTS/TrackStore.opts	  
      //   and the geometry defined in $VELOALIGNOPTS/VeloAlign.opts  

      my_tracks->TransformTrack(my_fitted_track,my_align_track,&VELOmap[0]); 

      if (my_align_track.nIsGood())  
      {
	if (my_align_track.nType() == 0 || my_align_track.nType() == 1)
	{
	  if (m_moni_tracks && (nTrackSample < m_nTrackSample)) // Request control sample
	  {
	    nTrackSample++;
	    control_tracks.push_back(my_align_track); 	    
	    Align::fill_params(my_align_track,0);	 
	  }
	  else
	  {
	    if (my_align_track.nType() == 0) nLeft_tracks++; 
	    if (my_align_track.nType() == 1) nRight_tracks++; 

	    selected_tracks.push_back(my_align_track); 
	  }
	}

	if (my_align_track.nType() == 2 && m_step2) // Overlap tracks (monitored by default)
	{
	  if (m_moni_overlaps) Align::fill_overlaps(my_align_track,0);	  
	  overlap_tracks.push_back(my_align_track); 
	}	
      }
    }
  } // End of loop on event tracks

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode Align::finalize() {

  debug() << "==> Finalize" << endmsg;

  misal_left.resize(126);  // This vector will receive the alignment constants
  error_left.resize(126);  // This vector will receive the alignment errors
  pulls_left.resize(126);  // This vector will receive the alignment pulls

  misal_right.resize(126);  
  error_right.resize(126);  
  pulls_right.resize(126);  

  misal_box.resize(18);  
  error_box.resize(18);  
  pulls_box.resize(18); 

  for (int j=0; j<126; j++)
  {
    misal_right[j] = 0.;
    misal_left[j] = 0.;
  }


  
  //  Millepede TOOL configuration is contained in a MilleConfig object 
  //  which is defined externally via the Joboption ($VELOALIGNOPTS/Millepede.opts)

  my_align  = tool<IMillepede>(my_Millepede);
  my_Config = new MilleConfig();  

  if (m_step1)
  {

    // Set the configuration for the left box internal alignment

    my_Config->InitMilleTool(my_align,0, m_align, m_sigma, m_constrain, 
			     4, m_starfactr, 3, m_residual_cut, &VELOmap[0],nLeft_tracks);

    Align::FindAlignment(my_Config);           // Make the global fit 
  
    int n_stat  = my_Config->GetNstations();   // The total number of stations to align
  
    for (int i=0;i<21;i++) 
    {
      if (m_VELOmap_l[i])  // Useless otherwise
      {
	for (int jj=0; jj<6; jj++)
	{
	  misal_left[i+jj*21] = mis_const[my_Config->Get_CorrectStation(i)+jj*n_stat];
	  error_left[i+jj*21] = mis_error[my_Config->Get_CorrectStation(i)+jj*n_stat];
	  pulls_left[i+jj*21] = mis_pull[my_Config->Get_CorrectStation(i)+jj*n_stat];
	}
      }
      else
      {
	for (int jj=0; jj<6; jj++)
	{
	  misal_left[i+jj*21] = -999.;
	  error_left[i+jj*21] = -999.;
	  pulls_left[i+jj*21] = -999.;
	}
      }
    }
    
    if (m_moni_constants) Align::fill_misalignments(misal_left, error_left, pulls_left, 2);
    
    // Now set the configuration for the right box internal alignment
    
    my_Config->InitMilleTool(my_align,1, m_align, m_sigma, m_constrain, 
			     4, m_starfactr, 3, m_residual_cut, &VELOmap[0],nRight_tracks);

    Align::FindAlignment(my_Config);           // Make the global fit

    n_stat  = my_Config->GetNstations();   // The total number of stations to align

    for (int i=0;i<21;i++) 
    {
      if (m_VELOmap_r[i])  // Useless otherwise
      {
	for (int jj=0; jj<6; jj++)
	{
	  misal_right[i+jj*21] = mis_const[my_Config->Get_CorrectStation(i)+jj*n_stat];
	  error_right[i+jj*21] = mis_error[my_Config->Get_CorrectStation(i)+jj*n_stat];
	  pulls_right[i+jj*21] = mis_pull[my_Config->Get_CorrectStation(i)+jj*n_stat];
	}
      }
      else
      {
	for (int jj=0; jj<6; jj++)
	{
	  misal_right[i+jj*21] = -999.;
	  error_right[i+jj*21] = -999.;
	  pulls_right[i+jj*21] = -999.;
	}
      }
    }
    
    if (m_moni_constants) Align::fill_misalignments(misal_right, error_right, pulls_right, 3);
  }  
  
  if (m_moni_tracks)  // Monitor the residuals of the control sample AFTER alignment
  {
    VeloTracks::const_iterator itrack;
    int iteration = 0;

    for (itrack = control_tracks.begin(); itrack != control_tracks.end(); ++itrack ) 
    {
      VeloTrack my_corrected_track  = VeloTrack();  // create AlignTrack 
	
      my_corrected_track.setNEvent(control_tracks[iteration].nEvent());
	
      my_Config->correcTrack(control_tracks[iteration],my_corrected_track,
			     misal_left,misal_right,&VELOmap[0]);

      Align::fill_params(my_corrected_track,1);	 

      iteration++;
    }
  }

      
  // Finally perfom the box alignment

  if (m_step2)
  {
    for (int i=0;i<18;i++) misal_box[i] = 0.;

    if (m_VELOopen) // Just a quick check
    {
      for (int i=0;i<6;i++) m_constrainb[i] = false;
    }    

    my_Config->InitBox(my_align,m_alignb, m_sigmab, m_constrainb, 
    		       4.*m_starfactr, m_residual_cutb, (zmoy_R+zmoy_L)/2,2*nEvents);
  
    Align::FindAlignment(my_Config);  // Make the global fit  

    for (int i=0;i<18;i++) 
    {
      misal_box[i] = mis_const[i];  
      error_box[i] = mis_error[i];  
      pulls_box[i] = mis_pull[i];  
    }

    if (m_moni_constants) Align::fill_misalignments(misal_box, error_box, pulls_box, 6);
  }   

  Align::updateConditions( misal_left, misal_right, misal_box );

  return GaudiTupleAlg::finalize();  // must be called after all other actions
}


//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////


//-----------------------------------------------------------
//  FINDALIGNMENT:  perform global params fit
//-----------------------------------------------------------
//
//  my_config    = the alignment configuration
//
//-----------------------------------------------------------


StatusCode Align::FindAlignment(MilleConfig *my_config) 
{
  
  MsgStream log(msgSvc(), name());

  int n_stat  = my_config->GetNstations();   // The total number of stations to align

  mis_const.resize(6*n_stat);
  mis_error.resize(6*n_stat);
  mis_pull.resize(6*n_stat);

  for (int j=0; j<6*n_stat; j++)
  {
    mis_const[j] = 0.;
    mis_error[j] = 0.;
    mis_pull[j]  = 0.;
  }

  debug() << "Entering FindAlignment" << endmsg;

  // First we perform loop on tracks or vertices, depending which alignment step we are considering

  VeloTracks::const_iterator itrack;

  if (my_config->isInternal())   // Internal Alignment 
  {
    int iteration = 0;

    for (itrack = selected_tracks.begin(); itrack != selected_tracks.end(); ++itrack ) 
      {if (selected_tracks[iteration].nType() == my_config->isLeft())
	my_config->PutTrack(selected_tracks[iteration],my_align);

       iteration++;
      }
  }
  else // Box alignment
  {

    // Start by putting in the overlap tracks 

    if (!m_VELOopen) // Don't use overlap in VELO open mode
    {
      int iteration = 0;

      for (itrack = overlap_tracks.begin(); itrack != overlap_tracks.end(); ++itrack ) 
      {
	VeloTrack my_corrected_track = VeloTrack();  // create AlignTrack 
	
	my_corrected_track.setNEvent(overlap_tracks[iteration].nEvent());
	
	my_config->correcTrack(overlap_tracks[iteration],my_corrected_track,
			       misal_left,misal_right,&VELOmap[0]);
	
	if (my_corrected_track.nIsGood()) 
	  my_config->PutOverlapTrack(my_corrected_track,my_align);

	iteration++;
      }
    }      

    // Then put the primary vertices

    for (int j=1; j<=nEvents; j++)
    { 
      PV_tracks.clear();
      int iteration = 0;

      for (itrack = selected_tracks.begin(); itrack != selected_tracks.end(); ++itrack ) 
      {
	if(selected_tracks[iteration].nEvent() == j 
	   && selected_tracks[iteration].nIsGood() == true 
	   && selected_tracks[iteration].nType() < 2)
	{ 
	  VeloTrack my_corrected_track  = VeloTrack();  // create AlignTrack 
	  
	  my_corrected_track.setNEvent(j);

	  my_config->correcTrack(selected_tracks[iteration],my_corrected_track,
				 misal_left,misal_right,&VELOmap[0]);
	  
	  if (my_corrected_track.nIsGood())
	    PV_tracks.push_back(my_corrected_track);
	}
	
	iteration++;
      }

      int nPV = -1;  // Total number of PV

      my_PV_finder->FindPVTracks(PV_tracks,-fabs(m_z_range),fabs(m_z_range),m_PV_trackmin,
				 m_z_sigma,m_IPmax,m_TrIPmax,nPV);
    
      if (nPV >= 0) // Found at least one
	//if (nPV == 0) // Found only one
      {
	for (int k=0; k<=nPV; k++) 
	  my_config->PutPVTrack(PV_tracks,my_align,k,zmoy_L,zmoy_R,m_VELOopen,m_trackmin);

	if (m_moni_PV) Align::fill_primary(PV_tracks,nPV);
      }
    }    
  }

  // Millepede matrix is now filled

  my_align->MakeGlobalFit(&mis_const[0],&mis_error[0],&mis_pull[0]);   // Perform the global fit
 
  return StatusCode::SUCCESS;
}


//-----------------------------------------------------------
//  DEFINEVELOMAP:  define the geometry to align
//-----------------------------------------------------------
//
//-----------------------------------------------------------


StatusCode Align::DefineVeloMap() 
{

//  First retrieve the velo detector element

  m_velo = getDet<DeVelo>("/dd/Structure/LHCb/BeforeMagnetRegion/Velo" );
   
  if ( 0 == m_velo ) {
    debug() << "Can't get velo!!!." << endmsg;
    return StatusCode::FAILURE;
   }

  // Fill the maps, depending on the chosen geometry
  // with the Z values taken from the detector element

  int nonzer_l = 0;
  int nonzer_r = 0;

  zmoy_R = 0.;
  zmoy_L = 0.;
  szmoy_R = 0.;
  szmoy_L = 0.;

  for (int i=0;i<42;i++) VELOmap[i] = 0.;

  for (int i=0; i<21; i++)
  {
    if (m_VELOmap_l[i]) VELOmap[i]    = (m_velo->sensor(2*i))->z();
    if (m_VELOmap_r[i]) VELOmap[i+21] = (m_velo->sensor(2*i+1))->z();
  }

  // Compute the parameters necessary for the canonical approximation

  for (int i=0; i<21; i++)
  {
    if (VELOmap[i] != 0.)    
    {  
      zmoy_L += VELOmap[i];
      nonzer_l++;
    }
    if (VELOmap[i+21] != 0.) 
    {
      zmoy_R += VELOmap[i+21];
      nonzer_r++;
    }
  }

  zmoy_L /= nonzer_l;
  zmoy_R /= nonzer_r;

  for (int i=0; i<21; i++)
  {
    debug() << VELOmap[i+21] << endmsg;

    if (VELOmap[i] != 0.)    szmoy_L += (VELOmap[i] - zmoy_L)*(VELOmap[i] - zmoy_L);
    if (VELOmap[i+21] != 0.) szmoy_R += (VELOmap[i+21] - zmoy_R)*(VELOmap[i+21] - zmoy_R);
  }

  szmoy_L /= nonzer_l;
  szmoy_R /= nonzer_r;

  // Some Debugging Printouts, in order to check the geometry

  std::string Rig,Lef;

  debug() << "___________________________________________" << endmsg;
  debug() << "|                                         |" << endmsg;
  debug() << "|        VELO GEOMETRY FOR ALIGNMENT      |" << endmsg;
  debug() << "|_________________________________________|" << endmsg;   
  debug() << "|             |             |             |" << endmsg;   
  debug() << "|   Station   |     Left    |    Right    |" << endmsg;   
  debug() << "|_____________|_____________|_____________|" << endmsg;   
  debug() << "|             |             |             |" << endmsg;   

  for (int i=0; i<21; i++)
  {
    (!m_VELOmap_l[i]) ? Lef = "OFF" : Lef = "ON"; 
    (!m_VELOmap_r[i]) ? Rig = "OFF" : Rig = "ON";     

    debug() << "|     " << std::setw(2) << i 
	    << "      |    " << std::setw(3) << Lef 
	    << "      |    " << std::setw(3) << Rig << "      |" << endmsg;   
  }

  debug() << "|_____________|_____________|_____________|" << endmsg;   
  debug() << "" << endmsg;  

  return StatusCode::SUCCESS;
}



//-----------------------------------------------------------
//  GETALIGNMENTCONSTANTS:  get the previous alignment constants
//                          from the CondDB
//-----------------------------------------------------------
//
//  This method uses the AlignmentCondition class
//
//-----------------------------------------------------------


StatusCode Align::GetAlignmentConstants() 
{

  MsgStream log(msgSvc(), name());

  std::string det_velo     = "/dd/Conditions/Alignment/Velo/VeloSystem";
  std::string det_rightbox = "/dd/Conditions/Alignment/Velo/VeloRight";
  std::string det_leftbox  = "/dd/Conditions/Alignment/Velo/VeloLeft";
  std::string det_module   = "/dd/Conditions/Alignment/Velo/Module";

  int module_num;

  misal_init_right.resize(126);
  misal_init_left.resize(126);
  misal_init_box.resize(12);

  std::string det_number_unit,det_number_diza,det_name;
  std::vector<double> translations,rotations;

  AlignmentCondition* myCond;  // Alignment condition

  for (int i=0; i<21; i++)   // First get the alignment conditions from the stations
  {
    for (int side=0; side<2; side++)   // 0 left, 1 right
    {
      module_num = 2*i+side; 

      det_number_unit  = Align::itos(module_num%10);
      det_number_diza  = Align::itos((module_num-module_num%10)/10);
      det_name  = det_module+det_number_diza+det_number_unit;

      myCond = getDet<AlignmentCondition>(det_name);
      translations = myCond->paramAsDoubleVect("dPosXYZ");
      rotations    = myCond->paramAsDoubleVect("dRotXYZ");

      // Now just put the misalignments in their respective containers
      
      if (side == 0)  // Left module
      {
	for (int j=0; j<3; j++) misal_init_left[i+21*j] = translations[j];
	for (int j=3; j<6; j++) misal_init_left[i+21*j] = rotations[j%3];	
      }
      else
      {
	for (int j=0; j<3; j++) misal_init_right[i+21*j] = translations[j];
	for (int j=3; j<6; j++) misal_init_right[i+21*j] = rotations[j%3];	
      }
    }
  }

  // Constants for the left box

  det_name  = det_leftbox;

  myCond = getDet<AlignmentCondition>(det_name);
  translations = myCond->paramAsDoubleVect("dPosXYZ");
  rotations    = myCond->paramAsDoubleVect("dRotXYZ");

  for (int j=0; j<3; j++) misal_init_box[2*j] = translations[j];
  for (int j=3; j<6; j++) misal_init_box[2*j] = rotations[j%3];
 
  // Constants for the right box

  det_name  = det_rightbox;

  myCond = getDet<AlignmentCondition>(det_name);
  translations = myCond->paramAsDoubleVect("dPosXYZ");
  rotations    = myCond->paramAsDoubleVect("dRotXYZ");

  for (int j=0; j<3; j++) misal_init_box[2*j+1] = translations[j];
  for (int j=3; j<6; j++) misal_init_box[2*j+1] = rotations[j%3];

  // Fill the nTuple with corresponding values

  std::vector<double> dummy_box, dummy_internal;

  dummy_box.resize(12);
  dummy_internal.resize(126);

  for (int i=0; i<126; i++) dummy_internal[i] = -999.; 
  for (int i=0; i<12; i++)  dummy_box[i] = -999.; 

  if (m_moni_constants)
  {
    Align::fill_misalignments(misal_init_left, dummy_internal, dummy_internal, 0);
    Align::fill_misalignments(misal_init_right, dummy_internal, dummy_internal, 1);
    Align::fill_misalignments(misal_init_box, dummy_box, dummy_box, 4);
  }

  return StatusCode::SUCCESS;
}


////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//
// Monitoring nTuples : used for development, should be switched off in 
//                      the future (replaced by histograms) 
//
/////////////////////////////////////////////////////

StatusCode Align::fill_params(VeloTrack& my_track, int my_step) 
{

  debug() << "Control track on event : " << my_track.nEvent() << endmsg;

  Tuple tuple=nTuple("Trackinfo", "Test sample");
  
  double slx, sly, x0, y0;
  int n_side = 0;
  
  slx  = my_track.nSlope_x();
  x0   = my_track.nXo_x();
  sly  = my_track.nSlope_y();
  y0   = my_track.nYo_y();

  int Ncoords = my_track.nGoodCoordinates();

  for (int k=0; k<Ncoords; k++)
  {
    if (slx==0.0 && sly==0.0) return StatusCode::SUCCESS;
  
    (my_track.nType() == 0)      
      ? n_side =0
      : n_side =1;

    double n_stationB = ((my_track.Coords()[k]).first).z();
    double n_X = ((my_track.Coords()[k]).first).x();
    double n_Y = ((my_track.Coords()[k]).first).y();
  
    double n_resX = n_X-(slx*n_stationB+x0);
    double n_resY = n_Y-(sly*n_stationB+y0);

    tuple->column( "step",    my_step);
    tuple->column( "side",    n_side);
    tuple->column( "vx",      slx*(-(slx*x0+sly*y0)/(slx*slx+sly*sly))+x0);
    tuple->column( "vy",      sly*(-(slx*x0+sly*y0)/(slx*slx+sly*sly))+y0);
    tuple->column( "vz",      -(slx*x0+sly*y0)/(slx*slx+sly*sly));
    tuple->column( "station", ((my_track.Coords()[k]).first).z());
    tuple->column( "X_clus",  ((my_track.Coords()[k]).first).x());
    tuple->column( "Y_clus",  ((my_track.Coords()[k]).first).y());
    tuple->column( "res_X",      n_resX);
    tuple->column( "res_Y",      n_resY);
    tuple->write();
  }

  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////
StatusCode Align::fill_overlaps(VeloTrack& my_track, int my_step)
{

  debug() << "Overlap track on event : " << my_track.nEvent() << endmsg;

  Tuple tuple=nTuple("Overlaps", "Overlap track info");

  for (unsigned int j=0; j<my_track.Coords().size(); j++)
  {
    tuple->column( "event",   my_track.nEvent());
    tuple->column( "track",   my_track.nTrack());
    tuple->column( "type",    my_track.nType()+my_step);
    tuple->column( "x",       ((my_track.Coords()[j]).first).x());
    tuple->column( "y",       ((my_track.Coords()[j]).first).y());
    tuple->column( "z",       ((my_track.Coords()[j]).first).z());
    tuple->column( "station", ((my_track.Coords()[j]).second).z());
    tuple->write();
  }

  return StatusCode::SUCCESS;
}	  


/////////////////////////////////////////////////////
StatusCode Align::fill_misalignments(std::vector<double> constants, std::vector<double> errors, 
				     std::vector<double> pulls, int my_step)
{
 
  int n_stat = constants.size()/6; // 21 for internal stuff, 2 for the boxes 

  Tuple tuple=nTuple("Constants", "Misalignment values");

  for (int i=0;i<n_stat;i++) 
  {
    for (int jj=0; jj<6; jj++)
    {
  
      tuple->column( "type",       my_step);
      tuple->column( "station",    i);
      tuple->column( "DOF",        jj);
      tuple->column( "mis_value",  constants[i+n_stat*jj]);
      tuple->column( "mis_error",  errors[i+n_stat*jj]);
      tuple->column( "mis_pull",   pulls[i+n_stat*jj]);
  
      tuple->write();
    }
  }

  return StatusCode::SUCCESS;
}


/////////////////////////////////////////////////////
StatusCode Align::fill_primary(VeloTracks& aPV, int PV_numb)
{

  VeloTracks::const_iterator itrack;

  debug() << PV_numb+1 << " primary vertices found "<< endmsg;

  Tuple tuple=nTuple("PV", "Primary vertices");

  for (int k=0; k<=PV_numb; k++) 
  {  
    int compteur = 0;
    int iteration = 0;

    for (itrack = aPV.begin(); itrack != aPV.end(); ++itrack ) 
    {
      if (aPV[iteration].nPVnumber() == k && compteur == 0)  
      {      

	tuple->column( "event",       aPV[iteration].nEvent());
	tuple->column( "PV_number",   k);
	tuple->column( "Track_num",   aPV[iteration].nTrack());
	tuple->column( "PV_x",        aPV[iteration].nPV_x());
	tuple->column( "PV_y",        aPV[iteration].nPV_y());
	tuple->column( "PV_z",        aPV[iteration].nPV_z());
	tuple->write();

	compteur = 1;
      }

      iteration++;
    }
  }

  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////
StatusCode Align::updateConditions( std::vector<double> const_left, std::vector<double> const_right, std::vector<double> const_box )
{
  std::string det_velo     = "/dd/Conditions/Alignment/Velo/VeloSystem";
  std::string cond_veloright = "/dd/Conditions/Alignment/Velo/VeloRight";
  std::string cond_veloleft  = "/dd/Conditions/Alignment/Velo/VeloLeft";
  std::string det_veloright   = "/dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloRight";
  std::string det_veloleft   = "/dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloLeft";
  std::string det_module_right   = "/dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloRight/Module";
  std::string det_module_left   = "/dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloLeft/Module";
  std::string cond_module   = "/dd/Conditions/Alignment/Velo/Module";

  int module_num;

  std::string det_number_unit, det_number_diza, det_name, cond_name;
  std::vector<double> translations, rotations;


  if ( m_step1 ) {
    for (int i=0; i<21; i++)   // First get the alignment conditions from the stations
    {
      for (int side=0; side<2; side++)   // 0 left, 1 right
      {
        if ( ( side == 0 ) && ! ( m_VELOmap_l[ i ] ) ) continue;
        if ( ( side == 1 ) && ! ( m_VELOmap_r[ i ] ) ) continue;
  
        module_num = 2*i+side; 
        det_number_unit  = Align::itos(module_num%10);
        det_number_diza  = Align::itos((module_num-module_num%10)/10);
        if ( side == 0 ) {
          det_name  = det_module_left+det_number_diza+det_number_unit;
        }  
        else {
          det_name  = det_module_right+det_number_diza+det_number_unit;
        }  
        cond_name  = cond_module+det_number_diza+det_number_unit;

        IDetectorElement *det = getDet<IDetectorElement>(det_name);
        IGeometryInfo *geo = det->geometry();
        AlignmentCondition *myCond = getDet<AlignmentCondition>(cond_name);
        // Convert alignment parameters into
        // a vector containing translations
        // a vector containing rotations and
        // a vector with the pivot point. 
        // That vector should actually be obtained from the detector element!
        std::vector<double> translation;
        translation.push_back(0.0);
        translation.push_back(0.0);
        translation.push_back(0.0);
        std::vector<double> rotation;
        rotation.push_back(0.0);
        rotation.push_back(0.0);
        rotation.push_back(0.0);
        std::vector<double> pivot;
        // Get the pivot vector from the geometry object:
        Gaudi::XYZPoint P = geo->toGlobal( Gaudi::XYZPoint() );
        pivot.push_back( P.X() );
        pivot.push_back( P.Y() );
        pivot.push_back( P.Z() );
        // fill the translation & rotation vectors
        // by looping over the number of degrees of freedom
        // the vector m_align is filled according to the following scheme:
        // first loop over detector elements, then loop over the degrees of 
        // freedom.
        // !!!
        int n_stat;
        if ( side == 0 ) {
          n_stat = const_left.size()/6; // 21 for internal stuff, 2 for the boxes 
        }
        else {
          n_stat = const_right.size()/6; // 21 for internal stuff, 2 for the boxes 
        }
        for ( unsigned int d = 0; d < 3; d++ ) {
          unsigned int pos1 = i + n_stat * d; // translations
          unsigned int pos2 = i + n_stat * (d + 3); //rotations
          if ( side == 0 ) {
            translation[d] = const_left[ pos1 ];
            rotation[d] = const_left[ pos2 ];
          }
          else {
            translation[d] = const_right[ pos1 ];
            rotation[d] = const_right[ pos2 ];
          }
        }
    
        debug() << "Translation vector: " << translation << "\n"
      	        << "Rotation vector   : " << rotation << "\n"
    	        << "pivot point       : " << pivot << endreq;
    
        const Gaudi::Transform3D halfDelta = DetDesc::localToGlobalTransformation( translation, rotation, pivot);
        const Gaudi::Transform3D frame = geo->parentIGeometryInfo()->parentIGeometryInfo()->toLocalMatrix() 
					 * geo->toGlobalMatrix();
					 // frame = moduleToHalf = VELO_half->toLocal * module->toGlobal 	
        const Gaudi::Transform3D localDelta = DetDesc::localDeltaFromAnyFrameDelta( frame, geo, halfDelta );
        myCond->offNominalMatrix( localDelta );
        updMgrSvc()->invalidate(myCond);

      }
    }
  }
  if ( m_step2 ) {  // VELO half alignment
    for (int side=0; side<2; side++)   // 0 left, 1 right
    {
      if ( side == 0 ) {
        det_name  = det_veloleft;
        cond_name = cond_veloleft;
      }  
      else {
        det_name  = det_veloright;
        cond_name = cond_veloright;
      }  

      IDetectorElement *det = getDet<IDetectorElement>(det_name);
      IGeometryInfo *geo = det->geometry();
      AlignmentCondition *myCond = getDet<AlignmentCondition>(cond_name);
      // Convert alignment parameters into
      // a vector containing translations
      // a vector containing rotations and
      // a vector with the pivot point. 
      // That vector should actually be obtained from the detector element!
      std::vector<double> translation;
      translation.push_back(0.0);
      translation.push_back(0.0);
      translation.push_back(0.0);
      std::vector<double> rotation;
      rotation.push_back(0.0);
      rotation.push_back(0.0);
      rotation.push_back(0.0);
      std::vector<double> pivot;
      pivot.push_back( 0.0 );
      pivot.push_back( 0.0 );
      pivot.push_back( 0.0 );
      // fill the translation & rotation vectors
      // by looping over the number of degrees of freedom
      // the vector m_align is filled according to the following scheme:
      // first loop over detector elements, then loop over the degrees of 
      // freedom.
      // !!!
      int n_stat = const_box.size()/6; // 21 for internal stuff, 2 for the boxes 
      info() << "n_stat " << n_stat << endmsg;

      for ( unsigned int d = 0; d < 3; d++ ) {
        if ( m_VELOopen ) {
          unsigned int pos1 = side + n_stat * d; // translations
          unsigned int pos2 = side + n_stat * (d + 3); //rotations
          translation[d] = const_box[ pos1 ];
          rotation[d] = const_box[ pos2 ];
        }
        else {
          if ( side == 0 ) {
            translation[d] = 0;
            rotation[d] = 0;
          }
          else {
            unsigned int pos1 = 2 + n_stat * d; // translations
            unsigned int pos2 = 2 + n_stat * (d + 3); //rotations
            translation[d] = const_box[ pos1 ];
            rotation[d] = const_box[ pos2 ];
          }
        }
      }
  
      info() << "Translation vector: " << translation << "\n"
    	        << "Rotation vector   : " << rotation << "\n"
  	        << "pivot point       : " << pivot << endreq;
  
      for ( int jjj = 0; jjj < const_box.size(); jjj++ ) {
        info() << jjj << " " << const_box[ jjj ] << endmsg;
      }
 
      const Gaudi::Transform3D halfDelta = DetDesc::localToGlobalTransformation( translation, rotation, pivot);
      const Gaudi::Transform3D localDelta = geo->ownToOffNominalMatrix() * halfDelta;
      myCond->offNominalMatrix( localDelta );
      updMgrSvc()->invalidate(myCond);
    }
  }
  return StatusCode::SUCCESS;
}


