// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"


//from DetDesc
#include "VeloDet/DeVelo.h"
#include "DetDesc/AlignmentCondition.h"
#include "DetDesc/ParamValidDataObject.h"

// Event
#include "Event/Track.h"

// local
#include "Align.h"
#include "ITrackStore.h"
#include "AlignmentTools/IMillepede.h"
#include "MilleConfig.h"
#include "PVHisto.h"
#include "VeloTrack.h"
#include "math.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Align
//
// 2005-07-28 : Sebastien Viret
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<Align>          s_factory ;
const        IAlgFactory& AlignFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Align::Align( const std::string& name,
                      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
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
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  nEvents = 0;          // Total number of events (event tag is necessary for box alignment)
  nTrackSample = 0;     // Number of track in the blind control sample

  nLeft_tracks  = 0;    // Number of left tracks
  nRight_tracks = 0;    // Number of right tracks

  sc = bookNTuple();
  if(sc.isFailure()){
    error() << "Could not book ntuples" << endmsg;
    return sc;
  }
  
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

  PV_tracks.clear();                   // (needed for STEP2: PV tracks)
  my_PV_finder    = new PVHisto();     // The PV tool

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

    verbose() << "Event " << nEvents << endmsg;
    verbose() << "Track " << nTracks << endmsg;

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

  misal_box.resize(24);  
  error_box.resize(24);  
  pulls_box.resize(24); 

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

    my_Config->InitMilleTool(my_align,true,0, m_align, m_sigma, m_constrain, 
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
    
    my_Config->InitMilleTool(my_align,true,1, m_align, m_sigma, m_constrain, 
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
			     misal_left,misal_right,misal_box,
			     &VELOmap[0]);

      Align::fill_params(my_corrected_track,1);	 

      iteration++;
    }
  }

      
  // Finally perfom the box alignment

  if (m_step2)
  {
    for (int i=0;i<24;i++) misal_box[i] = 0.;

    if (m_VELOopen) // Just a quick check
    {
      for (int i=0;i<6;i++) m_constrainb[i] = false;
    }    

    my_Config->InitBox(my_align,m_alignb, m_sigmab, m_constrainb, 
    		       4.*m_starfactr, m_residual_cutb, (zmoy_R+zmoy_L)/2,2*nEvents);
  
    Align::FindAlignment(my_Config);  // Make the global fit  

    for (int i=0;i<24;i++) 
    {
      misal_box[i] = mis_const[i];  
      error_box[i] = mis_error[i];  
      pulls_box[i] = mis_pull[i];  
    }

    if (m_moni_constants) Align::fill_misalignments(misal_box, error_box, pulls_box, 6);
  }   

  return GaudiAlgorithm::finalize();  // must be called after all other actions
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
			       misal_left,misal_right,misal_box,
			       &VELOmap[0]);
	
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
				 misal_left,misal_right,misal_box,
				 &VELOmap[0]);
	  
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


StatusCode Align::bookNTuple() {

  MsgStream log(msgSvc(), name());

  if ( 0 == ntupleSvc() ) {
    log << MSG::FATAL << "NtupleSvc is not available !" << endreq ;
    return StatusCode::FAILURE ;
  }

  //align info
    
  if (m_moni_constants)
  {
    std::string ntupleName1 = "ALIGN/constants"; 
    NTuplePtr nt1(ntupleSvc(), ntupleName1 );
    if ( ! nt1 ) {
      nt1 = ntupleSvc()->book(ntupleName1, CLID_ColumnWiseTuple, "constants info" );
    }
    if ( ! nt1 ) {
      log << MSG::FATAL << "Could not create ntuple constants info" << endreq;
      return StatusCode::FAILURE;
    }
  
    nt1->addItem( "type",       n_typeA);
    nt1->addItem( "station",    n_stationA);
    nt1->addItem( "DOF",        n_dof);
    nt1->addItem( "mis_value",  n_const);
    nt1->addItem( "mis_error",  n_error);
    nt1->addItem( "mis_pull",   n_pull);
  }  

  //track closest info
 
  if (m_moni_tracks)
  {
    std::string ntupleName2 = "ALIGN/trackinfo"; 
    NTuplePtr nt2(ntupleSvc(), ntupleName2 );
    if ( ! nt2 ) {
      nt2 = ntupleSvc()->book(ntupleName2, CLID_ColumnWiseTuple, "track closest info" );
    }
    if ( ! nt2 ) {
      log << MSG::FATAL << "Could not create ntuple track closest info" << endreq;
      return StatusCode::FAILURE;
    }

    nt2->addItem( "step",       n_step);
    nt2->addItem( "side",       n_side);
    nt2->addItem( "vx",         n_vx);
    nt2->addItem( "vy",         n_vy);
    nt2->addItem( "vz",         n_vz);
    nt2->addItem( "station",    n_stationB);
    nt2->addItem( "X_clus",     n_X);
    nt2->addItem( "Y_clus",     n_Y);
    nt2->addItem( "res_X",      n_resX);
    nt2->addItem( "res_Y",      n_resY);
  }

  //primary vertex info
 
  if (m_moni_PV)
  {
    std::string ntupleName3 = "ALIGN/PV"; 
    NTuplePtr nt3(ntupleSvc(), ntupleName3 );
    if ( ! nt3 ) {
      nt3 = ntupleSvc()->book(ntupleName3, CLID_ColumnWiseTuple, "primary vertex info" );
    }
    if ( ! nt3 ) {
      log << MSG::FATAL << "Could not create ntuple PV info" << endreq;
      return StatusCode::FAILURE;
    }
  
    nt3->addItem( "event",       n_eventV);
    nt3->addItem( "PV_number",   n_vertex);
    nt3->addItem( "Track_num",   n_PVtracks);
    nt3->addItem( "PV_x",        n_PVx);
    nt3->addItem( "PV_y",        n_PVy);
    nt3->addItem( "PV_z",        n_PVz);
  }
 
  //overlap info

  if (m_moni_overlaps)
  {
    std::string ntupleName4 = "ALIGN/overlaps"; 
    NTuplePtr nt4(ntupleSvc(), ntupleName4 );
    if ( ! nt4 ) {
      nt4 = ntupleSvc()->book(ntupleName4, CLID_ColumnWiseTuple, "overlaps info" );
    }
    if ( ! nt4 ) {
      log << MSG::FATAL << "Could not create ntuple overlaps info" << endreq;
      return StatusCode::FAILURE;
    }
  
    nt4->addItem( "event",       n_event);
    nt4->addItem( "track",       n_track);
    nt4->addItem( "type",        n_type);
    nt4->addItem( "x",           n_x);
    nt4->addItem( "y",           n_y);
    nt4->addItem( "z",           n_z);
    nt4->addItem( "station",     n_station);
  }
  
  return StatusCode::SUCCESS;
  
}


/////////////////////////////////////////////////////
StatusCode Align::fill_params(VeloTrack& my_track, int my_step) 
{

  double slx, sly, x0, y0;
  n_step = my_step;
  
  slx  = my_track.nSlope_x();
  x0   = my_track.nXo_x();
  sly  = my_track.nSlope_y();
  y0   = my_track.nYo_y();

  int Ncoords = my_track.nGoodCoordinates();

  for (int k=0; k<Ncoords; k++)
  {
    n_step = my_step;
  
    if (slx!=0.0 || sly!=0.0) n_vz = -(slx*x0+sly*y0)/(slx*slx+sly*sly);
    n_vy = sly*n_vz+y0;
    n_vx = slx*n_vz+x0;
  
    (my_track.nType() == 0)      
      ? n_side =0
      : n_side =1;

    n_stationB = ((my_track.Coords()[k]).first).z();
    //    wghtx = ((my_track->Coords()[k]).second).x();
    n_X = ((my_track.Coords()[k]).first).x();
    //    wghty = ((my_track->Coords()[k]).second).y();
    n_Y = ((my_track.Coords()[k]).first).y();
  
    n_resX = n_X-(slx*n_stationB+x0);
    n_resY = n_Y-(sly*n_stationB+y0);

    Align::writeNtuple("ALIGN/trackinfo");

  }

  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////
StatusCode Align::fill_overlaps(VeloTrack& my_track, int my_step)
{

  debug() << "Overlap track on event : " << my_track.nEvent() << endmsg;

  for (unsigned int j=0; j<my_track.Coords().size(); j++)
  {
    n_event = my_track.nEvent();
    n_track = my_track.nTrack();
    n_type  = my_track.nType()+my_step;
    n_x = ((my_track.Coords()[j]).first).x();
    n_y = ((my_track.Coords()[j]).first).y();
    n_z = ((my_track.Coords()[j]).first).z();
    n_station = ((my_track.Coords()[j]).second).z();
    Align::writeNtuple("ALIGN/overlaps");
  }

  return StatusCode::SUCCESS;
}	  


/////////////////////////////////////////////////////
StatusCode Align::fill_misalignments(std::vector<double> constants, std::vector<double> errors, 
				     std::vector<double> pulls, int my_step)
{
 
  int n_stat = constants.size()/6; // 21 for internal stuff, 2 for the boxes 

  for (int i=0;i<n_stat;i++) 
  {
    for (int jj=0; jj<6; jj++)
    {
      n_typeA = my_step;    
      n_stationA = i;
      n_dof    = jj;
      n_const  = constants[i+n_stat*jj];
      n_error  = errors[i+n_stat*jj];
      n_pull   = pulls[i+n_stat*jj];
      Align::writeNtuple("ALIGN/constants");
    }
  }

  return StatusCode::SUCCESS;
}


/////////////////////////////////////////////////////
StatusCode Align::fill_primary(VeloTracks& aPV, int PV_numb)
{

  VeloTracks::const_iterator itrack;

  debug() << PV_numb+1 << " primary vertices found "<< endmsg;

  for (int k=0; k<=PV_numb; k++) 
  {  
    int compteur = 0;
    int iteration = 0;

    for (itrack = aPV.begin(); itrack != aPV.end(); ++itrack ) 
    {
      if (aPV[iteration].nPVnumber() == k && compteur == 0)  
      {      
	n_eventV = aPV[iteration].nEvent();
	n_vertex = k;
	n_PVtracks = aPV[iteration].nTrack();
	n_PVx = aPV[iteration].nPV_x();
	n_PVy = aPV[iteration].nPV_y();
	n_PVz = aPV[iteration].nPV_z();
	compteur = 1;
	Align::writeNtuple("ALIGN/PV");
      }

      iteration++;
    }
  }

  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////
StatusCode Align::writeNtuple(std::string ntupleName) 
{
  MsgStream log(msgSvc(), name());

  NTuplePtr my_ntuple( ntupleSvc(), ntupleName);

  if (my_ntuple) 
  {
    StatusCode sc = ntupleSvc()->writeRecord( my_ntuple.ptr() );
    if (sc.isFailure()) 
    {
      info() << "Failure to write event in NTuple " << ntupleName << endmsg;
      return sc;
    }
  }
  else 
  {
    info() << "Can't find the NTuple " << ntupleName << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

