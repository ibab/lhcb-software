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
#include "Event/AlignTrack.h"
#include "Align.h"
#include "ITrackStore.h"
#include "AlignmentTools/IMillepede.h"
#include "MilleConfig.h"
#include "PVHisto.h"

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
  declareProperty("L_0"                  , VELOmap[0] = 0);
  declareProperty("L_1"                  , VELOmap[1] = 0);
  declareProperty("L_2"                  , VELOmap[2] = 0);
  declareProperty("L_3"                  , VELOmap[3] = 0);
  declareProperty("L_4"                  , VELOmap[4] = 0);
  declareProperty("L_5"                  , VELOmap[5] = 0);
  declareProperty("L_6"                  , VELOmap[6] = 0);
  declareProperty("L_7"                  , VELOmap[7] = 0);
  declareProperty("L_8"                  , VELOmap[8] = 0);
  declareProperty("L_9"                  , VELOmap[9] = 0);
  declareProperty("L_10"                 , VELOmap[10] = 0);
  declareProperty("L_11"                 , VELOmap[11] = 0);
  declareProperty("L_12"                 , VELOmap[12] = 0);
  declareProperty("L_13"                 , VELOmap[13] = 0);
  declareProperty("L_14"                 , VELOmap[14] = 0);
  declareProperty("L_15"                 , VELOmap[15] = 0);
  declareProperty("L_16"                 , VELOmap[16] = 0);
  declareProperty("L_17"                 , VELOmap[17] = 0);
  declareProperty("L_18"                 , VELOmap[18] = 0);
  declareProperty("L_19"                 , VELOmap[19] = 0);
  declareProperty("L_20"                 , VELOmap[20] = 0);
  declareProperty("R_0"                  , VELOmap[21] = 0);
  declareProperty("R_1"                  , VELOmap[22] = 0);
  declareProperty("R_2"                  , VELOmap[23] = 0);
  declareProperty("R_3"                  , VELOmap[24] = 0);
  declareProperty("R_4"                  , VELOmap[25] = 0);
  declareProperty("R_5"                  , VELOmap[26] = 0);
  declareProperty("R_6"                  , VELOmap[27] = 0);
  declareProperty("R_7"                  , VELOmap[28] = 0);
  declareProperty("R_8"                  , VELOmap[29] = 0);
  declareProperty("R_9"                  , VELOmap[30] = 0);
  declareProperty("R_10"                 , VELOmap[31] = 0);
  declareProperty("R_11"                 , VELOmap[32] = 0);
  declareProperty("R_12"                 , VELOmap[33] = 0);
  declareProperty("R_13"                 , VELOmap[34] = 0);
  declareProperty("R_14"                 , VELOmap[35] = 0);
  declareProperty("R_15"                 , VELOmap[36] = 0);
  declareProperty("R_16"                 , VELOmap[37] = 0);
  declareProperty("R_17"                 , VELOmap[38] = 0);
  declareProperty("R_18"                 , VELOmap[39] = 0);
  declareProperty("R_19"                 , VELOmap[40] = 0);
  declareProperty("R_20"                 , VELOmap[41] = 0);
  declareProperty("Add_Halo"             , m_halo = false);
  declareProperty("Primary_Z_min"        , m_z_min);
  declareProperty("Primary_Z_max"        , m_z_max);
  declareProperty("minTracks_perPV"      , m_PV_trackmin);
  declareProperty("PrimZ_window"         , m_z_sigma);
  declareProperty("Prim_IPmax"           , m_IPmax);
  declareProperty("Track_IPmax"          , m_TrIPmax);
  declareProperty("Ntracks"              , m_nTracks);
  declareProperty("Iteration"            , m_iteration);
  declareProperty("Iteration_Startfactor", m_starfactr= 10.);
  declareProperty("X_alignment"          , m_align[0] = false);
  declareProperty("Y_alignment"          , m_align[1] = false);
  declareProperty("Z_alignment"          , m_align[2] = false);
  declareProperty("Xrot_alignment"       , m_align[3] = false);
  declareProperty("Yrot_alignment"       , m_align[4] = false);
  declareProperty("Zrot_alignment"       , m_align[5] = false);
  declareProperty("BX_alignment"         , m_alignb[0] = false);
  declareProperty("BY_alignment"         , m_alignb[1] = false);
  declareProperty("BZ_alignment"         , m_alignb[2] = false);
  declareProperty("BXrot_alignment"      , m_alignb[3] = false);
  declareProperty("BYrot_alignment"      , m_alignb[4] = false);
  declareProperty("BZrot_alignment"      , m_alignb[5] = false);
  declareProperty("X_trans_constrained"  , m_constrain[0] = false);
  declareProperty("X_shear_constrained"  , m_constrain[1] = false);
  declareProperty("Y_trans_constrained"  , m_constrain[2] = false);
  declareProperty("Y_shear_constrained"  , m_constrain[3] = false);
  declareProperty("Z_trans_constrained"  , m_constrain[4] = false);
  declareProperty("Z_scale_constrained"  , m_constrain[5] = false);
  declareProperty("X_rotat_constrained"  , m_constrain[6] = false);
  declareProperty("Y_rotat_constrained"  , m_constrain[7] = false);
  declareProperty("Z_rotat_constrained"  , m_constrain[8] = false);
  declareProperty("X_sigma"              , m_sigma[0] = 0.1);
  declareProperty("Y_sigma"              , m_sigma[1] = 0.1);
  declareProperty("Z_sigma"              , m_sigma[2] = 0.1);
  declareProperty("Xrot_sigma"           , m_sigma[3] = 0.01);
  declareProperty("Yrot_sigma"           , m_sigma[4] = 0.01);
  declareProperty("Zrot_sigma"           , m_sigma[5] = 0.01);
  declareProperty("BX_sigma"             , m_sigmab[0] = 0.5);
  declareProperty("BY_sigma"             , m_sigmab[1] = 0.5);
  declareProperty("BZ_sigma"             , m_sigmab[2] = 0.5);
  declareProperty("BXrot_sigma"          , m_sigmab[3] = 0.005);
  declareProperty("BYrot_sigma"          , m_sigmab[4] = 0.005);
  declareProperty("BZrot_sigma"          , m_sigmab[5] = 0.005);
  declareProperty("StartingCutOnResidual", m_residual_cut_init);
  declareProperty("FurtherCutOnResidual" , m_residual_cut);

  declareProperty("TrackStoreTool", my_TrackStore = "TrackStore");
  declareProperty("MillepedeTool" , my_Millepede  = "Millepede");
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
    
  sc = bookNTuple();
  if(sc.isFailure()){
    error() << "Could not book ntuples" << endmsg;
    return sc;
  }

//  First retrieve the velo detector element

  SmartDataPtr<DeVelo> velo( detDataService(), "/dd/Structure/LHCb/Velo" );
   
  if ( 0 == velo ) {
    debug() << "Can't get velo!!!." << endmsg;
    return StatusCode::FAILURE;
   }
  debug() << "got velo" << endmsg;

  m_velo = velo;

  DefineVeloMap(); // Define the stations to be aligned

//  The TrackStore TOOL is performing the track selection before MILLEPEDE feeding
//  it transforms Tracks into AlignTracks objects which are formatted for MILLEPEDE
//
//  Selection cuts are contained TrackStoreConfig object and  are set externally via 
//  the Joboption ($VELOALIGNOPTS/TrackStore.opts)  

  
  my_tracks = tool<ITrackStore>(my_TrackStore);
  m_measProvider = tool<IMeasurementProvider>("MeasurementProvider");

  selected_tracks = new AlignTracks(); // Containers of align tracks 
  PV_tracks       = new AlignTracks(); // (needed for STEP2 alignment)

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode Align::execute() {

  debug() << "==> Execute" << endmsg;

  ++nEvents;
  int nTracks = 0;

  double res_r[42];
  double res_phi[42];

  // Get the Tracks

  SmartDataPtr<Tracks> tracks( eventSvc(), "Rec/Track/Best" );
  //SmartDataPtr<Tracks> tracks( eventSvc(), "Rec/Track/Ideal" ); // For testing
 
  if ( tracks == 0 ){
    debug() << "Could not get reconstructed tracks" << endmsg;
    return StatusCode::FAILURE;
  }

  debug() << "Retrieved Tracks : " << tracks->size() << endmsg;

  // Loop over tracks

  Tracks::const_iterator iTrack;
  for (iTrack = tracks->begin(); iTrack != tracks->end(); ++iTrack ) 
  {    
    ++nTracks;

    verbose() << "Event " << nEvents << endmsg;
    verbose() << "Track " << nTracks << endmsg;

    Track* my_fitted_track = *iTrack ;          // retrieve TrFitTrack

    //    if (my_fitted_track->type() == Track::Velo)
    {
      AlignTrack* my_align_track  = new AlignTrack();  // create AlignTrack 

      my_align_track->setNEvent(nEvents);
      my_align_track->setNTrack(nTracks);
      my_align_track->setNPVnumber(-1);      
      my_align_track->setNPVtag(false);
      
      //      Select the track according to the cuts defined in
      //      $VELOALIGNOPTS/TrackStore.opts	  
      //      and the geometry defined in
      //      $VELOALIGNOPTS/VeloAlign.opts    

      my_tracks->TransformTrack(my_fitted_track,my_align_track,VELOmap,m_velo,&res_r[0],&res_phi[0]); 

      if (my_align_track->nIsGood())  
      {
	selected_tracks->insert(my_align_track); 
	
	for (int j=0; j<42; j++)
	{
	  if (res_r[j] != -999. && res_phi[j] != -999.)
	  {
	    n_res_r   = res_r[j];
	    n_res_phi = res_phi[j];
	    writeNtuple("FILE1/51");
	  }
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

  
  //  Millepede TOOL configuration is contained in a MilleConfig object 
  //  which is defined externally via the Joboption ($VELOALIGNOPTS/Millepede.opts)
  
  my_align  = tool<IMillepede>(my_Millepede);
  my_Config = new MilleConfig();  

  // Set the configuration for the left box internal alignment

  my_Config->InitMilleTool(my_align,true,0, &m_align[0], &m_sigma[0], &m_constrain[0], 
			   4, m_starfactr, 3, m_residual_cut, m_residual_cut_init,
			   &VELOmap[0]);

  FindAlignment(my_Config);           // Make the global fit 

  
  // Now set the configuration for the right box internal alignment

  my_Config->InitMilleTool(my_align,true,1, &m_align[0], &m_sigma[0], &m_constrain[0], 
			   4, m_starfactr, 3, m_residual_cut, m_residual_cut_init,
			   &VELOmap[0]);

  FindAlignment(my_Config);           // Make the global fit
      
  // Finally configure the box alignment, based on primary vertex fit 

  my_Config->InitMilleTool(my_align,false,1, &m_alignb[0], &m_sigmab[0], &m_constrain[0], 
			   3, 100.*m_starfactr, 0, 2.*m_residual_cut, 4.*m_residual_cut_init,
			   &VELOmap[0]);

  FindAlignment(my_Config);  // Make the global fit  
   
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

  StatusCode sc;

  int n_stat  = my_config->GetNstations();   // The total number of stations to align

  std::vector<double> misalignments;
  std::vector<double> PVcandidates;

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

  AlignTracks::const_iterator itrack;

  // First of all we write the alignment constants values BEFORE the alignment

  misalignments.resize(150);
  for (int j=0; j<150.; j++) {misalignments[j] = 0.;}

  GetAlignmentConstants(my_config, &misalignments[0]);   // Get the previous constants from the Geometry framework  
    
  if (my_config->isInternal()) 
  {
    for (int i=0;i<21;i++) 
    {
      for (int jj=0; jj<6; jj++)
      {
	n_typeA = my_config->isLeft();    
	n_stationA = i;
	n_dof    = jj;
	n_const  = misalignments[i+21*jj];
	n_error  = -999.;
	n_pull   = -999.;
	writeNtuple("FILE1/50");
      }
    }
  }
  else // Box alignment
  {
    n_stat = 2;  // Only two boxes

    for (int i=0; i<6*n_stat; i++)
    {
      n_typeA = i%2+4;    
      n_stationA = -1.;
      n_dof    = i/2;
      n_const  = misalignments[i+21*6];
      n_error  = mis_error[i];
      n_pull   = -999.;
      writeNtuple("FILE1/50");
    }
  }
   
  // Then we perform loop on tracks or vertices, depending which alignment step we are considering

  if (my_config->isInternal())   // Put the tracks into Millepede 
  {
    for (itrack = selected_tracks->begin(); itrack != selected_tracks->end(); ++itrack ) 
      {if ((*itrack)->nType() == my_config->isLeft()) my_config->PutTrack(*itrack,my_align,0);}
  }
  else
  {
    nPV = 0;  // Total number of PV

    for (int j=1; j<=nEvents; j++) // Search for the PV in each event
    {  
      PV_tracks->clear();
      PVcandidates.clear();

      for (itrack = selected_tracks->begin(); itrack != selected_tracks->end(); ++itrack ) 
      {
	if((*itrack)->nEvent() == j && (*itrack)->nIsGood() == true) PV_tracks->insert(*itrack);
      }

      my_config->FindPVTracks(PV_tracks,my_align,m_z_min,m_z_max,m_PV_trackmin,
			      m_z_sigma,m_IPmax,m_TrIPmax,nPV,j,&PVcandidates);

      if (PVcandidates.size() != 0)
      {
	for (int k=0; k<(PVcandidates.size()/6); k++) 
	{
	  n_eventV = PVcandidates[6*k];
	  n_vertex = PVcandidates[6*k+1];
	  n_PVtracks = PVcandidates[6*k+2];
	  n_PVx = PVcandidates[6*k+3];
	  n_PVy = PVcandidates[6*k+4];
	  n_PVz = PVcandidates[6*k+5];
	  writeNtuple("FILE1/52");

	  info() << PVcandidates[6*k+1] << endmsg;
	}
      }
    }
  }

  // Millepede matrix is now filled

  my_align->MakeGlobalFit(&mis_const[0],&mis_error[0],&mis_pull[0]);   // Perform the global fit

  // If step 1, update Millepede with the new constants in order to have clean tracks for step 2

  if (my_config->isInternal())  
  {
    for (int j=0; j<6*n_stat; j++) {my_align->ParGlo(j, mis_const[j]);}
    
    for (itrack = selected_tracks->begin(); itrack != selected_tracks->end(); ++itrack ) 
    {
      if ((*itrack)->nType() == my_config->isLeft()) 
      {
	sc = my_config->PutTrack(*itrack,my_align,1);  // Fit and update the track with Millepede 
	if (!sc) (*itrack)->setNIsGood(false); // Reject the track if non correct
      }
    }
  }

  // Finally fill the Ntuple with the computed alignment constants
  
  if (my_config->isInternal()) 
  {
    for (int i=0;i<21;i++) 
    {
      if (VELOmap[i+21*my_config->isLeft()] != 0.)  // Useless otherwise
      {
	for (int jj=0; jj<6; jj++)
	{
	  n_typeA = my_config->isLeft()+2;    
	  n_stationA = i;
	  n_dof    = jj;
	  n_const  = mis_const[my_config->Get_CorrectStation(i)+jj*n_stat];
	  n_error  = mis_error[my_config->Get_CorrectStation(i)+jj*n_stat];
	  n_pull   = mis_pull[my_config->Get_CorrectStation(i)+jj*n_stat];
	  writeNtuple("FILE1/50");
	}
      }
    }
  }
  else // Box alignment
  {
    for (int i=0; i<6*n_stat; i++)
    {
      n_typeA = i%2+6;    
      n_stationA = -1.;
      n_dof    = i/2;
      n_const  = mis_const[i];
      n_error  = mis_error[i];
      n_pull   = -999.;
      writeNtuple("FILE1/50");
    }
  }
 
  return StatusCode::SUCCESS;
}


//-----------------------------------------------------------
//  DEFINEVELOMAP:  define the geometry to align
//-----------------------------------------------------------
//
//-----------------------------------------------------------


StatusCode Align::DefineVeloMap() 
{
  // Fill the maps, depending on the chosen geometry
  // with the Z values taken from the detector element

  for (int i=0; i<21; i++)
  {
    if (VELOmap[i] == 1)    VELOmap[i] = m_velo->zSensor(2*i);
    if (VELOmap[i+21] == 1) VELOmap[i+21] = m_velo->zSensor(2*i+1);
  }

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
    (VELOmap[i]    == 0.) ? Lef = "OFF" : Lef = "ON"; 
    (VELOmap[i+21] == 0.) ? Rig = "OFF" : Rig = "ON";     

    debug() << "|     " << std::setw(2) << i 
	    << "      |    " << std::setw(3) << Lef 
	    << "      |    " << std::setw(3) << Rig << "      |" << endmsg;   
  }

  debug() << "|_____________|_____________|_____________|" << endmsg;   
  debug() << "" << endmsg;  

  return StatusCode::SUCCESS;
}


/*
-----------------------------------------------------------
  GETALIGNMENTCONSTANTS:  get the previous alignment constants
-----------------------------------------------------------

  This method uses the AlignmentCondition class

-----------------------------------------------------------
*/

StatusCode Align::GetAlignmentConstants(MilleConfig* my_config, double misalignments[]) 
{

  MsgStream log(msgSvc(), name());

  std::string det_velo     = "/dd/Conditions/Alignment/Velo/VeloSystem";
  std::string det_rightbox = "/dd/Conditions/Alignment/Velo/VeloRight";
  std::string det_leftbox  = "/dd/Conditions/Alignment/Velo/VeloLeft";
  std::string det_module   = "/dd/Conditions/Alignment/Velo/Module";

  int module_num;

  std::string det_number_unit,det_number_diza,det_name;
  std::vector<double> translations,rotations;

  AlignmentCondition* myCond;  // Alignment condition

  for (int i=0; i<21; i++)   // First get the alignment conditions from the stations
  {
    (my_config->isLeft()) ? module_num = 2*i+1 : module_num = 2*i; 

    det_number_unit  = Align::itos(module_num%10);
    det_number_diza  = Align::itos((module_num-module_num%10)/10);
    det_name  = det_module+det_number_diza+det_number_unit;

    myCond = getDet<AlignmentCondition>(det_name);
    translations = myCond->paramAsDoubleVect("dPosXYZ");
    rotations    = myCond->paramAsDoubleVect("dRotXYZ");

    // Some manipulations due to frame differences

    for (int j=0; j<3; j++) (i%2 == 1 && j == 1) ? misalignments[i+21*j] = -translations[j]: misalignments[i+21*j] = translations[j];
    for (int j=3; j<6; j++) (i%2 == 1 && j == 5) ? misalignments[i+21*j] = -rotations[j%3]: misalignments[i+21*j] = rotations[j%3];
  }

  // Constants for the left box

  det_name  = det_leftbox;

  myCond = getDet<AlignmentCondition>(det_name);
  translations = myCond->paramAsDoubleVect("dPosXYZ");
  rotations    = myCond->paramAsDoubleVect("dRotXYZ");

  for (int j=0; j<3; j++) misalignments[j+21*6] = translations[j];
  for (int j=3; j<6; j++) misalignments[j+21*6] = rotations[j%3];
 
  // Constants for the right box

  det_name  = det_rightbox;

  myCond = getDet<AlignmentCondition>(det_name);
  translations = myCond->paramAsDoubleVect("dPosXYZ");
  rotations    = myCond->paramAsDoubleVect("dRotXYZ");

  for (int j=0; j<3; j++) misalignments[j+22*6] = translations[j];
  for (int j=3; j<6; j++) misalignments[j+22*6] = rotations[j%3];

  return StatusCode::SUCCESS;
}


/////////////////////////////////////////////////////
//
// Update the constants after alignment, uses Alignment Framework

StatusCode Align::UpdateAlignmentConstants() 
{

  MsgStream log(msgSvc(), name());

  // Not yet implemented

  return StatusCode::SUCCESS;
}


////////////////////////////////////////////////////////////////////////

StatusCode Align::bookNTuple() {

  MsgStream log(msgSvc(), name());

  if ( 0 == ntupleSvc() ) {
    log << MSG::FATAL << "NtupleSvc is not available !" << endreq ;
    return StatusCode::FAILURE ;
  }
  
   
  std::string ntupleName1 = "FILE1/50"; 
  NTuplePtr nt1(ntupleSvc(), ntupleName1 );
  if ( ! nt1 ) {
      nt1 = ntupleSvc()->book(ntupleName1, CLID_ColumnWiseTuple, "align info" );
  }
  if ( ! nt1 ) {
    log << MSG::FATAL << "Could not create ntuple align info" << endreq;
    return StatusCode::FAILURE;
  }

  std::string ntupleName2 = "FILE1/51"; 
  NTuplePtr nt2(ntupleSvc(), ntupleName2 );
  if ( ! nt2 ) {
      nt2 = ntupleSvc()->book(ntupleName2, CLID_ColumnWiseTuple, "track residual info" );
  }
  if ( ! nt2 ) {
    log << MSG::FATAL << "Could not create ntuple residual info" << endreq;
    return StatusCode::FAILURE;
  }
   
  std::string ntupleName3 = "FILE1/52"; 
  NTuplePtr nt3(ntupleSvc(), ntupleName3 );
  if ( ! nt3 ) {
      nt3 = ntupleSvc()->book(ntupleName3, CLID_ColumnWiseTuple, "primary vertex info" );
  }
  if ( ! nt3 ) {
    log << MSG::FATAL << "Could not create ntuple PV info" << endreq;
    return StatusCode::FAILURE;
  }

  //align info
  
  nt1->addItem( "type",       n_typeA);
  nt1->addItem( "station",    n_stationA);
  nt1->addItem( "DOF",        n_dof);
  nt1->addItem( "mis_value",  n_const);
  nt1->addItem( "mis_error",  n_error);
  nt1->addItem( "mis_pull",   n_pull);

  //residual info
  
  nt2->addItem( "R_sensor",      n_res_r);
  nt2->addItem( "Phi_sensor",    n_res_phi);
  
  //primary vertex info
  
  nt3->addItem( "event",       n_eventV);
  nt3->addItem( "PV_number",   n_vertex);
  nt3->addItem( "Track_num",   n_PVtracks);
  nt3->addItem( "PV_x",        n_PVx);
  nt3->addItem( "PV_y",        n_PVy);
  nt3->addItem( "PV_z",        n_PVz);
  
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
