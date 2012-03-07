// Include files 

// local
#include "MilleConfig.h"

using namespace std;

//-----------------------------------------------------------------------------
// Implementation file for class : MilleConfig
//
// 2005-07-28 : Sebastien Viret
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MilleConfig::MilleConfig(  )
{}

//=============================================================================
// Destructor
//=============================================================================
MilleConfig::~MilleConfig() {}; 

//=============================================================================

void MilleConfig::InitMilleTool(IMillepede *my_millepede, int right,
				std::vector<bool> DOF, std::vector<double> Sigm,
				std::vector<bool> Cons, int nloc, double starfact, 
				int nstd, std::vector<double> res_cut, double VELOmap[],
				int n_objects) 
{

  m_i_align = true;    // It is internal alignment
  m_right = right;     // Do we align left (FALSE) or right box (TRUE) ?

  for (int j=0; j<6; j++) {m_DOF[j] = DOF[j];}   // What are the parameter to align ?
  for (int j=0; j<9; j++) {m_Cons[j] = Cons[j];} // What are the constraints equations to define ?
  for (int j=0; j<6; j++) {m_Sigm[j] = Sigm[j];} // What are the 'annealing' factors transmitted to the global matrix ?
  
//  
// We first get the number of stations to align
//
// The parameters zmoy** and s_zmoy** will be used in the constraints
//

  double zmoy   = 0.0;
  double s_zmoy = 0.0;
  int nonzer    = 0;

  for (int j=0; j<21; j++)
  {
    CorrectVELOmap[j] = -1;
    if (VELOmap[j+21*m_right] != 0.) 
    {
      CorrectVELOmap[j] = nonzer; // CorrectVELOmap gives the rank of the i^th station in Millepede
      zmoy  += VELOmap[j+21*m_right];
      nonzer++;
    }
  }

  zmoy /= nonzer;
  
  for (int j=0; j<21; j++)
  {
    if (VELOmap[j+21*m_right] != 0.) 
      s_zmoy += (VELOmap[j+21*m_right]-zmoy)*(VELOmap[j+21*m_right]-zmoy);
  }

  s_zmoy /= nonzer;

  m_zmoy  = zmoy;
  m_szmoy = s_zmoy;
   
  m_nglo      = nonzer;           // Number of station to align
  m_nloc      = nloc;             // Number of local params
  m_startfact = starfact;         // Start factor for Chi^2 local fit cut
  m_nstd      = nstd;             // Num. of std deviations for local fit 

  // Initialize Millepede

  my_millepede->InitMille(&m_DOF[0], &m_Sigm[0], GetNstations(), m_nloc, 
			  m_startfact, m_nstd, res_cut[1], res_cut[0], n_objects);

  //
  // Parameters for constraint equations
  //

  int Nstations  = GetNstations();   // Number of stations to be aligned (for VELO)
  int Nparams    = 6*GetNstations();   // Number of params to be aligned (for VELO)

  //
  // Here we define the 9 constraints equations
  // according to the requested geometry
  //

  ftx.resize(Nparams);	
  fty.resize(Nparams);
  ftz.resize(Nparams);
  frotx.resize(Nparams);	
  froty.resize(Nparams);	
  frotz.resize(Nparams);	
  fscaz.resize(Nparams);	  
  shearx.resize(Nparams);
  sheary.resize(Nparams);
    
  for (int j=0; j<Nparams; j++)
  {
    ftx[j]    = 0.0;
    fty[j]    = 0.0;
    ftz[j]    = 0.0;
    frotx[j]  = 0.0;
    froty[j]  = 0.0;
    frotz[j]  = 0.0;
    fscaz[j]  = 0.0;
    shearx[j] = 0.0;
    sheary[j] = 0.0;
  }
  
  for (int j=0; j<21; j++) 
  {
    double z_station = VELOmap[j+21*m_right];
    
    if (z_station != 0.) // Only included stations
    {
      ftx[CorrectVELOmap[j]]                 = 1.0;
      fty[CorrectVELOmap[j]+Nstations]       = 1.0;
      ftz[CorrectVELOmap[j]+2*Nstations]     = 1.0;
      frotx[CorrectVELOmap[j]+3*Nstations]   = 1.0;
      froty[CorrectVELOmap[j]+4*Nstations]   = 1.0;
      frotz[CorrectVELOmap[j]+5*Nstations]   = 1.0;
	
      shearx[CorrectVELOmap[j]]              = (z_station-zmoy)/s_zmoy;
      sheary[CorrectVELOmap[j]+Nstations]    = (z_station-zmoy)/s_zmoy;
      fscaz[CorrectVELOmap[j]+2*Nstations]   = (z_station-zmoy)/s_zmoy;	
    }
  }
  
  //  Here we put the constraints information in the basket
  
  if (m_Cons[0] && m_DOF[0])   my_millepede->ConstF(&ftx[0],     0.0);    
  if (m_Cons[1] && m_DOF[0])   my_millepede->ConstF(&shearx[0],  0.0);  
  if (m_Cons[2] && m_DOF[1])   my_millepede->ConstF(&fty[0],     0.0);      
  if (m_Cons[3] && m_DOF[1])   my_millepede->ConstF(&sheary[0],  0.0);     
  if (m_Cons[4] && m_DOF[2])   my_millepede->ConstF(&ftz[0],     0.0);       
  if (m_Cons[5] && m_DOF[2])   my_millepede->ConstF(&fscaz[0],   0.0);           
  if (m_Cons[6] && m_DOF[3])   my_millepede->ConstF(&frotx[0],   0.0);   
  if (m_Cons[7] && m_DOF[4])   my_millepede->ConstF(&froty[0],   0.0);       
  if (m_Cons[8] && m_DOF[5])   my_millepede->ConstF(&frotz[0],   0.0);      

  // That's it!

}


void MilleConfig::InitBox(IMillepede *my_millepede,
			  std::vector<bool> DOF, std::vector<double> Sigm,
			  std::vector<bool> Cons, double starfact, 
			  std::vector<double> res_cut, double zmoy,
			  int n_objects)  
{
  m_zmoy  = zmoy;
  m_i_align = false;    // It is box alignment 

  for (int j=0; j<6; j++) {m_DOF[j] = DOF[j];}   // What are the parameter to align ?
  for (int j=0; j<6; j++) {m_Cons[j] = Cons[j];} // What are the constraints equations to define ?
  for (int j=0; j<6; j++) {m_Sigm[j] = Sigm[j];} // What are the 'annealing' factors transmitted to the global matrix ?
   
  m_nglo      = 3;            // Number of stations to align (Left box PV, Right box PV, Overlaps)
  m_nloc      = 7;            // Number of local params
  m_startfact = starfact;     // Start factor for Chi^2 local fit cut
  m_nstd      = 0;            // Num. of std deviations for local fit 

  // Initialize Millepede

  my_millepede->InitMille(&m_DOF[0], &m_Sigm[0], m_nglo, m_nloc, 
			  m_startfact, m_nstd, res_cut[1], res_cut[0], n_objects);

  //
  // constraint equations for boxes (see LHCb 2007-067 for details)
  //

  double Const_X[6*m_nglo];
  double Const_Y[6*m_nglo];
  double Const_Z[6*m_nglo];
  double Const_SX[6*m_nglo];
  double Const_SY[6*m_nglo];
  //double Const_SZ[6*m_nglo];

  for (int i = 0; i< 6*m_nglo; ++i)
  {
    Const_X[i] = 0.0;
    Const_Y[i] = 0.0;
    Const_Z[i] = 0.0;
    Const_SX[i] = 0.0;
    Const_SY[i] = 0.0;
    //Const_SZ[i] = 0.0;
  }

  Const_X[0]    = 1.;
  Const_X[1]    = -1.;
  Const_X[3]    = 1.;

  Const_Y[4]    = 1.;
  Const_Y[5]    = -1.;
  Const_Y[7]    = 1.;

  Const_Z[8]    = 1.;
  Const_Z[9]    = -1.;
  Const_Z[11]   = 1.;

  Const_SX[12]  = 1.;
  Const_SX[13]  = -1.;
  Const_SX[15]  = 1.;

  Const_SY[16]  = 1.;
  Const_SY[17]  = -1.;
  Const_SY[19]  =  1.;

  if (m_DOF[0] && m_Cons[0]) my_millepede->ConstF(&Const_X[0],  0.0);   
  if (m_DOF[1] && m_Cons[1]) my_millepede->ConstF(&Const_Y[0],  0.0); 
  if (m_DOF[2] && m_Cons[2]) my_millepede->ConstF(&Const_Z[0],  0.0); 
  if (m_DOF[3] && m_Cons[3]) my_millepede->ConstF(&Const_SX[0],  0.0);
  if (m_DOF[4] && m_Cons[4]) my_millepede->ConstF(&Const_SY[0],  0.0);

  // this is it! Millepede is now ready to align boxes...


}

/*
-----------------------------------------------------------
  PUTTRACK : Feed the Millepede object with track info
           and perform the local fit
-----------------------------------------------------------

  atrack    = object containing all the necessary
              info for putting a track into Millepede
	      (see Aligntrack class for more info)

  my_config = configuration of the Millepede job
              (see MilleConfig class for more info)

-----------------------------------------------------------
*/

StatusCode MilleConfig::PutTrack(VeloTrack& atrack, IMillepede *my_millepede)
{

  int Nstations  = GetNstations();   // Number of stations to be aligned (for VELO)
  int Nlocal     = GetNlocal();      // Number of params to be aligned (for VELO)
  int Nparams    = 6*Nstations;      // Number of params to be aligned (for VELO)

  StatusCode sc;

 // Global parameters initializations

  std::vector<double>   derLC; 
  std::vector<double>   derGB;
  std::vector<double>   derNonLin;
  std::vector<double>   derNonLin_i;

  derGB.resize(Nparams);          // Vector containing the global derivatives 
  derNonLin.resize(Nparams);      // Global derivatives non linearly related to residual
  derNonLin_i.resize(Nparams);    // Global derivatives non linearly related to residual
  derLC.resize(Nlocal);           // Vector containing the local derivatives

  for (int i=0; i<Nparams; i++) {derGB[i]=0.; derNonLin[i]=0.; derNonLin_i[i]=0.;}
  for (int i=0; i<Nlocal; i++)  {derLC[i]=0.;}


  double track_params[2*Nlocal+2];     // Vector containing the track parameters 

  for (int i=0; i<2*Nlocal+2; i++) {track_params[i]=0.;}

  int n_station = 0;

  double x_cor  = 0.;
  double y_cor  = 0.;
  double z_cor  = 0.;
  double err_x  = 0.;
  double err_y  = 0.;

  for (int k=0; k<(atrack.nGoodCoordinates()); k++)  // We loop an all the track hits
  {
    // Retrieve coordinates from the track object

    x_cor = ((atrack.Coords()[k]).first).x();
    y_cor = ((atrack.Coords()[k]).first).y();
    z_cor = ((atrack.Coords()[k]).first).z();

    n_station = int(((atrack.Coords()[k]).second).z());
    n_station = CorrectVELOmap[n_station];

    err_x = ((atrack.Coords()[k]).second).x();
    err_y = ((atrack.Coords()[k]).second).y();

    my_millepede->ZerLoc(&derGB[0],&derLC[0],&derNonLin[0],&derNonLin_i[0]); 

    // LOCAL 1st derivatives for the X equation
	    
    derLC[0] = 1.0;
    derLC[1] = z_cor;
    derLC[2] = 0.0;
    derLC[3] = 0.0;

    // GLOBAL 1st derivatives (see LHCbnote-2005-101 for definition)

    if (m_DOF[0]) derGB[n_station]             = -1.0;    // dX	    
    if (m_DOF[1]) derGB[Nstations+n_station]   =  0.0;    // dY
    if (m_DOF[2]) derGB[2*Nstations+n_station] =  0.0;    // dZ
    if (m_DOF[3]) derGB[3*Nstations+n_station] =  0.0;    // d_alpha
    if (m_DOF[4]) derGB[4*Nstations+n_station] =  0.0;    // d_beta
    if (m_DOF[5]) derGB[5*Nstations+n_station] =  y_cor;  // d_gamma
    
    if (m_DOF[0]) derNonLin[n_station]             =  0.0;      // dX	    
    if (m_DOF[1]) derNonLin[Nstations+n_station]   =  0.0;      // dY
    if (m_DOF[2]) derNonLin[2*Nstations+n_station] =  1.0;      // dZ
    if (m_DOF[3]) derNonLin[3*Nstations+n_station] =  y_cor;    // d_alpha
    if (m_DOF[4]) derNonLin[4*Nstations+n_station] =  x_cor;    // d_beta
    if (m_DOF[5]) derNonLin[5*Nstations+n_station] =  0.0;      // d_gamma
    
    if (m_DOF[0]) derNonLin_i[n_station]             =  0.0;      // dX	    
    if (m_DOF[1]) derNonLin_i[Nstations+n_station]   =  0.0;      // dY
    if (m_DOF[2]) derNonLin_i[2*Nstations+n_station] =  1.0;      // dZ
    if (m_DOF[3]) derNonLin_i[3*Nstations+n_station] =  1.0;      // d_alpha
    if (m_DOF[4]) derNonLin_i[4*Nstations+n_station] =  1.0;      // d_beta
    if (m_DOF[5]) derNonLin_i[5*Nstations+n_station] =  0.0;      // d_gamma

    
    sc = my_millepede->EquLoc(&derGB[0], &derLC[0], &derNonLin[0], &derNonLin_i[0], 
			      x_cor, err_x); // Store hits parameters
    if (! sc) {break;} 	

    my_millepede->ZerLoc(&derGB[0],&derLC[0],&derNonLin[0],&derNonLin_i[0]); 

    // LOCAL 1st derivatives for the Y equation

    derLC[0] = 0.0;
    derLC[1] = 0.0;
    derLC[2] = 1.0;
    derLC[3] = z_cor;

    // GLOBAL 1st derivatives

    if (m_DOF[0]) derGB[n_station]             =  0.0;        // dX	    
    if (m_DOF[1]) derGB[Nstations+n_station]   = -1.0;        // dY
    if (m_DOF[2]) derGB[2*Nstations+n_station] = 0.0;         // dZ
    if (m_DOF[3]) derGB[3*Nstations+n_station] = 0.0;         // d_alpha
    if (m_DOF[4]) derGB[4*Nstations+n_station] = 0.0;         // d_beta
    if (m_DOF[5]) derGB[5*Nstations+n_station] = -x_cor;      // d_gamma
    
    if (m_DOF[0]) derNonLin[n_station]             =  0.0;     // dX	    
    if (m_DOF[1]) derNonLin[Nstations+n_station]   =  0.0;     // dY
    if (m_DOF[2]) derNonLin[2*Nstations+n_station] =  1.0;     // dZ
    if (m_DOF[3]) derNonLin[3*Nstations+n_station] =  y_cor;   // d_alpha
    if (m_DOF[4]) derNonLin[4*Nstations+n_station] =  x_cor;   // d_beta
    if (m_DOF[5]) derNonLin[5*Nstations+n_station] =  0.0;     // d_gamma
    
    if (m_DOF[0]) derNonLin_i[n_station]             =  0.0;      // dX	    
    if (m_DOF[1]) derNonLin_i[Nstations+n_station]   =  0.0;      // dY
    if (m_DOF[2]) derNonLin_i[2*Nstations+n_station] =  3.0;      // dZ
    if (m_DOF[3]) derNonLin_i[3*Nstations+n_station] =  3.0;      // d_alpha
    if (m_DOF[4]) derNonLin_i[4*Nstations+n_station] =  3.0;      // d_beta
    if (m_DOF[5]) derNonLin_i[5*Nstations+n_station] =  0.0;      // d_gamma

    sc = my_millepede->EquLoc(&derGB[0], &derLC[0], &derNonLin[0], &derNonLin_i[0],
			      y_cor, err_y); // Store hits parameters
    if (! sc) {break;} 	
  }

  sc = my_millepede->FitLoc(my_millepede->GetTrackNumber(),track_params,0); // Fit the track

  if (sc.isSuccess())
  {
    my_millepede->SetTrackNumber(my_millepede->GetTrackNumber()+1); // We are in the first loop  
                                                                    // update needed for the store
    return StatusCode::SUCCESS;
  }

  return StatusCode::FAILURE;
}


/*
-----------------------------------------------------------
  PUTOVERLAPTRACK : Feed the Millepede object with overlap 
                    track info and perform the fit
-----------------------------------------------------------

  atrack    = object containing all the necessary
              info for putting a track into Millepede
	      (see Aligntrack class for more info)

  my_config = configuration of the Millepede job
              (see MilleConfig class for more info)

-----------------------------------------------------------
*/

StatusCode MilleConfig::PutOverlapTrack(VeloTrack& atrack, IMillepede *my_millepede)
{
  int Nstations  = GetNstations();   // Number of stations to be aligned (for VELO)
  int Nlocal     = GetNlocal();      // Number of params to be aligned (for VELO)
  int Nparams    = 6*Nstations;      // Number of params to be aligned (for VELO)
  double zmoy    = Get_zmoy();

  StatusCode sc;

 // Global parameters initializations

  std::vector<double>   derLC; 
  std::vector<double>   derGB;
  std::vector<double>   derNonLin;
  std::vector<double>   derNonLin_i;

  derNonLin.resize(Nparams);   
  derNonLin_i.resize(Nparams);   
  derGB.resize(Nparams);     // Vector containing the global derivatives 
  derLC.resize(Nlocal);      // Vector containing the local derivatives

  for (int i=0; i<Nparams; i++) {derGB[i]=0.;}
  for (int i=0; i<Nparams; i++) {derNonLin[i]=0.;}
  for (int i=0; i<Nparams; i++) {derNonLin_i[i]=0.;}
  for (int i=0; i<Nlocal; i++) {derLC[i]=0.;}

  double track_params[2*Nlocal+2];     // Vector containing the track parameters 

  for (int i=0; i<2*Nlocal+2; i++) {track_params[i]=0.;}

  for (int k=0; k<(atrack.nGoodCoordinates()); k++)  // We loop an all the track hits
  {
    // Retrieve coordinates from the track object

    double x_cor     = ((atrack.Coords()[k]).first).x();
    double y_cor     = ((atrack.Coords()[k]).first).y();
    double z_cor     = ((atrack.Coords()[k]).first).z();
    double err_x     = ((atrack.Coords()[k]).second).x();
    double err_y     = ((atrack.Coords()[k]).second).y();
    int n_station    = int(((atrack.Coords()[k]).second).z());

    int n_ove     = int(n_station)%100;
    n_station = int(n_station-n_ove)/100;;

    n_station = n_station%2;

    my_millepede->ZerLoc(&derGB[0],&derLC[0],&derNonLin[0],&derNonLin_i[0]); // reset derLC and derGB arrays

    // LOCAL 1st derivatives for the X equation
	    
    derLC[0] = 1.0;
    derLC[1] = z_cor;
    derLC[2] = 0.0;
    derLC[3] = 0.0;

    // GLOBAL 1st derivatives (see LHCbnote-2007-067 for definition)
    
    if (n_station == 1) // Left box is fixed, just consider right box
    {
      if (m_DOF[0]) derGB[2]  = -1.0;             // dX	    
      if (m_DOF[1]) derGB[5]  =  0.0;             // dY
      if (m_DOF[2]) derGB[8]  =  0.0;             // dZ
      if (m_DOF[3]) derGB[11] =  0.0;             // d_alpha
      if (m_DOF[4]) derGB[14] =  z_cor - zmoy;    // d_beta
      if (m_DOF[5]) derGB[17] =  y_cor;           // d_gamma
      
      if (m_DOF[0]) derNonLin[2]  =  0.0;      // dX	    
      if (m_DOF[1]) derNonLin[5]  =  0.0;      // dY
      if (m_DOF[2]) derNonLin[8]  =  1.;       // dZ
      if (m_DOF[3]) derNonLin[11] =  y_cor;    // d_alpha
      if (m_DOF[4]) derNonLin[14] =  x_cor;    // d_beta
      if (m_DOF[5]) derNonLin[17] =  0.0;      // d_gamma

      if (m_DOF[0]) derNonLin_i[2]  =  0.0;    // dX	    
      if (m_DOF[1]) derNonLin_i[5]  =  0.0;    // dY
      if (m_DOF[2]) derNonLin_i[8] =  1.0;    // dZ
      if (m_DOF[3]) derNonLin_i[11] =  1.0;    // d_alpha
      if (m_DOF[4]) derNonLin_i[14] =  1.0;    // d_beta
      if (m_DOF[5]) derNonLin_i[17] =  0.0;    // d_gamma
    }

    sc = my_millepede->EquLoc(&derGB[0], &derLC[0], &derNonLin[0], &derNonLin_i[0],
			      x_cor, err_x); // Store hits parameters
    if (! sc) {break;} 	

    my_millepede->ZerLoc(&derGB[0],&derLC[0],&derNonLin[0],&derNonLin_i[0]); // reset derLC and derGB arrays

    // LOCAL 1st derivatives for the Y equation

    derLC[0] = 0.0;
    derLC[1] = 0.0;
    derLC[2] = 1.0;
    derLC[3] = z_cor;

    // GLOBAL 1st derivatives

    if (n_station == 1)
    {   
      if (m_DOF[0]) derGB[2]  =  0.0;            // dX	    
      if (m_DOF[1]) derGB[5]  = -1.0;            // dY
      if (m_DOF[2]) derGB[8]  =  0.0;            // dZ
      if (m_DOF[3]) derGB[11] =  z_cor - zmoy;   // d_alpha
      if (m_DOF[4]) derGB[14] =  0.0;            // d_beta
      if (m_DOF[5]) derGB[17] = -x_cor;          // d_gamma
      
      if (m_DOF[0]) derNonLin[2]  =  0.0;      // dX	    
      if (m_DOF[1]) derNonLin[5]  =  0.0;      // dY
      if (m_DOF[2]) derNonLin[8]  =  1.;       // dZ
      if (m_DOF[3]) derNonLin[11] =  y_cor;    // d_alpha
      if (m_DOF[4]) derNonLin[14] =  x_cor;    // d_beta
      if (m_DOF[5]) derNonLin[17] =  0.0;      // d_gamma

      if (m_DOF[0]) derNonLin_i[2]  =  0.0;    // dX	    
      if (m_DOF[1]) derNonLin_i[5]  =  0.0;    // dY
      if (m_DOF[2]) derNonLin_i[8]  =  3.0;    // dZ
      if (m_DOF[3]) derNonLin_i[11] =  3.0;    // d_alpha
      if (m_DOF[4]) derNonLin_i[14] =  3.0;    // d_beta
      if (m_DOF[5]) derNonLin_i[17] =  0.0;    // d_gamma
    }

    sc = my_millepede->EquLoc(&derGB[0], &derLC[0],&derNonLin[0], &derNonLin_i[0],
			      y_cor, err_y); // Store hits parameters
    if (! sc) {break;} 	
  }

  sc = my_millepede->FitLoc(my_millepede->GetTrackNumber(),track_params,0); // Fit the track
  if (sc.isSuccess())
  {
    my_millepede->SetTrackNumber(my_millepede->GetTrackNumber()+1); // We are in the first loop  
                                                                    // update needed for the store
    return StatusCode::SUCCESS;
  }
  
  return StatusCode::SUCCESS;
}



/*
-----------------------------------------------------------
  PUTPVTRACK : Here we perform the local fit
-----------------------------------------------------------  

  Here is the real primary vertex fit method
  
-----------------------------------------------------------
*/

StatusCode MilleConfig::PutPVTrack(VeloTracks& aPV, IMillepede* my_millepede, int nPV,
				   double zmoyl, double zmoyr, double VELO_open, int n_min)
{

  int Nstations  = GetNstations();   // Number of stations to be aligned (for VELO)
  int Nlocal     = GetNlocal();      // Number of params to be aligned (for VELO)
  int Nparams    = 6*Nstations;      // Number of params to be aligned (for VELO)
  
  VeloTracks::const_iterator itrack;

  double slx, sly, x0, y0, errx, erry, zclos;
  double zmoy=0;

  int iteration = 0;


 // Global parameters initializations

  std::vector<double>   derLC; 
  std::vector<double>   derGB;
  std::vector<double>   derNonLin;
  std::vector<double>   derNonLin_i;

  derGB.resize(Nparams);     // Vector containing the global derivatives 
  derNonLin.resize(Nparams);     
  derNonLin_i.resize(Nparams);     
  derLC.resize(Nlocal);      // Vector containing the local derivatives

  for (int i=0; i<Nparams; i++) {derGB[i]=0.;}
  for (int i=0; i<Nparams; i++) {derNonLin[i]=0.;}
  for (int i=0; i<Nparams; i++) {derNonLin_i[i]=0.;}
  for (int i=0; i<Nlocal; i++) {derLC[i]=0.;}

  double vertex_params[2*Nlocal+2];     // Vector containing the vertex parameters 
  for (int i=0; i<2*Nlocal+2; i++) {vertex_params[i]=0.;}

  StatusCode sc;

  // If VELO is open, one box is fixed and we fit vertex in the 2 boxes
  // thus we need at sufficient number of tracks per box
  if (VELO_open)
  {
    int n_left  = 0;
    int n_right = 0;

    for (itrack = aPV.begin(); itrack != aPV.end(); ++itrack ) 
    {
      if (aPV[iteration].nPVnumber() == nPV && aPV[iteration].nType() == 0) n_left++;
      if (aPV[iteration].nPVnumber() == nPV && aPV[iteration].nType() == 1) n_right++;

      iteration++;
    }

    iteration = 0;
    if (n_right < n_min || n_left < n_min) return StatusCode::SUCCESS;
  }

  // 
  // This is a good vertex, put it into Millepede
  //

  for (itrack = aPV.begin(); itrack != aPV.end(); ++itrack ) 
  {
    if (aPV[iteration].nPVnumber() == nPV) // A good track
    {
      slx  = aPV[iteration].nSlope_x();
      x0   = aPV[iteration].nXo_x();
      errx = aPV[iteration].nErrX_x();
      sly  = aPV[iteration].nSlope_y();
      y0   = aPV[iteration].nYo_y();
      erry = aPV[iteration].nErrY_y();;

      zclos = aPV[iteration].nPV_z();      

      if (aPV[iteration].nType() == 0) zmoy = zmoyl;
      if (aPV[iteration].nType() == 1) zmoy = zmoyr;
      if (VELO_open) zmoy = (zmoyl+zmoyr)/2;
      
      my_millepede->ZerLoc(&derGB[0],&derLC[0],&derNonLin[0],&derNonLin_i[0]); // reset derLC and derGB arrays
      
      if (VELO_open)
      {
        // LOCAL 1st derivatives for v_x 
        
        derLC[4] = 1.;
        derLC[5] = 0.;
        derLC[6] = -slx;
        
        // GLOBAL 1st derivatives
        if (aPV[iteration].nType() == 1) 
        {
          if (m_DOF[0]) derGB[2]   =  -1.0;          // dX	    
          if (m_DOF[1]) derGB[5]   =  0.0;           // dY
          if (m_DOF[2]) derGB[8]   = slx;            // dZ
          if (m_DOF[3]) derGB[11]  = zclos-zmoy;     // dtX    
          if (m_DOF[4]) derGB[14]  = 0.0;            // dtY    
          if (m_DOF[5]) derGB[17]  = 0.0;            // dtZ   
        }
      }
      else{
        
        // LOCAL 1st derivatives for v_x 
        
        derLC[4] = 1.;
        derLC[5] = 0.;
        derLC[6] = -slx;
        
        // GLOBAL 1st derivatives
        
        if (m_DOF[0]) derGB[aPV[iteration].nType()]     =  -1.0;             // dX	    
        if (m_DOF[1]) derGB[3+aPV[iteration].nType()]   =  0.0;            // dY
        if (m_DOF[2]) derGB[6+aPV[iteration].nType()]   = slx;            // dZ
        //if (m_DOF[3]) derGB[12+aPV[iteration].nType()]  = zclos-zmoy;            // dtX    
        if (m_DOF[3]) derGB[9+aPV[iteration].nType()]   = -zmoy;            // dtX    
        if (m_DOF[4]) derGB[12+aPV[iteration].nType()]  = 0.0;            // dtY    
        if (m_DOF[5]) derGB[15+aPV[iteration].nType()]  = 0.0;            // dtZ 
        
        if (m_DOF[0]) derNonLin[aPV[iteration].nType()]    =  0.0;      // dX	    
        if (m_DOF[1]) derNonLin[3+aPV[iteration].nType()]  =  0.0;      // dY
        if (m_DOF[2]) derNonLin[6+aPV[iteration].nType()]  =  0.0;       // dZ
        if (m_DOF[3]) derNonLin[9+aPV[iteration].nType()]  =  0.0;    // d_alpha
        if (m_DOF[4]) derNonLin[12+aPV[iteration].nType()] =  1.0;    // d_beta
        if (m_DOF[5]) derNonLin[15+aPV[iteration].nType()] =  0.0;      // d_gamma
        
        if (m_DOF[0]) derNonLin_i[aPV[iteration].nType()]    =  0.0;    // dX	    
        if (m_DOF[1]) derNonLin_i[3+aPV[iteration].nType()]  =  0.0;    // dY
        if (m_DOF[2]) derNonLin_i[6+aPV[iteration].nType()]  =  0.0;    // dZ
        if (m_DOF[3]) derNonLin_i[9+aPV[iteration].nType()]  =  0.0;    // d_alpha
        if (m_DOF[4]) derNonLin_i[12+aPV[iteration].nType()] =  6.0;    // d_beta
        if (m_DOF[5]) derNonLin_i[15+aPV[iteration].nType()] =  0.0;    // d_gamma
      }

      sc = my_millepede->EquLoc(&derGB[0], &derLC[0],&derNonLin[0],&derNonLin_i[0], 
                                x0, errx); // Store hits parameters
      if (! sc) {break;}
      
      my_millepede->ZerLoc(&derGB[0],&derLC[0],&derNonLin[0],&derNonLin_i[0]); // reset derLC and derGB arrays
      
      if (VELO_open)
      {
        // LOCAL 1st derivatives for v_y 
        
        derLC[4] = 0.;
        derLC[5] = 1.;
        derLC[6] = -sly;
        
        // GLOBAL 1st derivatives
        if (aPV[iteration].nType() == 1) 
        {
          if (m_DOF[0]) derGB[2]   =  0.0;          // dX	    
          if (m_DOF[1]) derGB[5]   = -1.0;           // dY
          if (m_DOF[2]) derGB[8]   = sly;            // dZ
          if (m_DOF[3]) derGB[11]  = 0.0;     // dtX    
          if (m_DOF[4]) derGB[14]  = zclos-zmoy;            // dtY    
          if (m_DOF[5]) derGB[17]  = 0.0;            // dtZ   
        }
      }
      else{ 
        
        // LOCAL 1st derivatives for v_y
        
        derLC[4] = 0.;
        derLC[5] = 1.;
        derLC[6] = -sly;	    
        
        // GLOBAL 1st derivatives
        
        if (m_DOF[0]) derGB[aPV[iteration].nType()]     =  0.0;             // dX	    
        if (m_DOF[1]) derGB[3+aPV[iteration].nType()]   =  -1.0;            // dY
        if (m_DOF[2]) derGB[6+aPV[iteration].nType()]   = sly;            // dZ
        if (m_DOF[3]) derGB[9+aPV[iteration].nType()]   = 0.0;            // dtX    
        //if (m_DOF[4]) derGB[16+aPV[iteration].nType()]  = zclos-zmoy;            // dtY   
        if (m_DOF[4]) derGB[12+aPV[iteration].nType()]  = -zmoy;            // dtY   
        if (m_DOF[5]) derGB[15+aPV[iteration].nType()]  = 0.0;            // dtZ   
        
        if (m_DOF[0]) derNonLin[aPV[iteration].nType()]    =  0.0;      // dX	    
        if (m_DOF[1]) derNonLin[3+aPV[iteration].nType()]  =  0.0;      // dY
        if (m_DOF[2]) derNonLin[6+aPV[iteration].nType()]  =  0.0;       // dZ
        if (m_DOF[3]) derNonLin[9+aPV[iteration].nType()]  =  1.0;    // d_alpha
        if (m_DOF[4]) derNonLin[12+aPV[iteration].nType()] =  0.0;    // d_beta
        if (m_DOF[5]) derNonLin[15+aPV[iteration].nType()] =  0.0;      // d_gamma
        
        if (m_DOF[0]) derNonLin_i[aPV[iteration].nType()]    =  0.0;    // dX	    
        if (m_DOF[1]) derNonLin_i[3+aPV[iteration].nType()]  =  0.0;    // dY
        if (m_DOF[2]) derNonLin_i[6+aPV[iteration].nType()]  =  0.0;    // dZ
        if (m_DOF[3]) derNonLin_i[9+aPV[iteration].nType()]  =  6.0;    // d_alpha
        if (m_DOF[4]) derNonLin_i[12+aPV[iteration].nType()] =  0.0;    // d_beta
        if (m_DOF[5]) derNonLin_i[15+aPV[iteration].nType()] =  0.0;    // d_gamma
      }

      sc = my_millepede->EquLoc(&derGB[0], &derLC[0],&derNonLin[0],&derNonLin_i[0],
				y0, erry); // Store hits parameters
      if (! sc) {break;}
    }

    iteration++;
  }

  iteration = 0;

  // Fit the vertex

  sc = my_millepede->FitLoc(my_millepede->GetTrackNumber(),&vertex_params[0],0); // Fit the track

  if (sc.isSuccess()) 
  {
    my_millepede->SetTrackNumber(my_millepede->GetTrackNumber()+1);
    return StatusCode::SUCCESS;
  }  

  return StatusCode::FAILURE;
}



//===========================================================================
// CORRECTRACK : correct the track from misalignments
//===========================================================================
// 
//  --> mistrack    : the misaligned AlignTrack
//  --> correctrack : the corrected AlignTrack
//
//===========================================================================

StatusCode MilleConfig::correcTrack(VeloTrack& mistrack, 
				    VeloTrack& correctrack, 
				    std::vector<double> left_constants, 
				    std::vector<double> right_constants,
				    double VELOmap[])
{

  VeloTrack first_track = VeloTrack();

  double z_moy_R =0;
  double z_moy_L =0;

  double x_cor, y_cor, z_cor;
  double x_o, y_o; //, z_o;

  int n_left  = left_constants.size()/6;
  int n_right = right_constants.size()/6;

  correctrack.setNIsGood(false);

  // Stop here if we don't have the same number of stations on each side

  if (n_left != n_right) return StatusCode::SUCCESS; 

  // z_average  
  for (int j=0; j<21; j++) 
  {  
    if (VELOmap[j] != 0.)
    {
      z_moy_L  += VELOmap[j];
      z_moy_R  += VELOmap[21+j];
    }
  }

  z_moy_L /= n_left;
  z_moy_R /= n_right;

  int n_valid_coord = 0;

  for (int k=0; k<(mistrack.nGoodCoordinates()); k++)  // We loop an all the track hits
  {
    // Retrieve coordinates from the track object

    x_cor = ((mistrack.Coords()[k]).first).x();
    y_cor = ((mistrack.Coords()[k]).first).y();
    z_cor = ((mistrack.Coords()[k]).first).z();
    x_o = x_cor;
    y_o = y_cor;
    //z_o = z_cor;

    int iStation = int(((mistrack.Coords()[k]).second).z());
    int side     = mistrack.nType();

    if (side >= 2) // Overlap track
    {
      int n_ove     = int(iStation)%100;
      iStation      = int(iStation-n_ove)/100;

      (iStation%2 == 0)
	? side = 0
	: side = 1;

      iStation/=2;
    }

    
    // Correct the coordinates (not taking box into account)

    if (side == 0)  // Left hit
    {
      // Correct the X coordinate

      x_cor = x_o + left_constants[iStation]
	          - y_o*left_constants[iStation+5*n_left];

      // Correct the Y coordinate

      y_cor = y_o + left_constants[iStation+n_left]
	          + x_o*left_constants[iStation+5*n_left];
    }
    else // Right hit
    {
      // Correct the X coordinate

      x_cor = x_o + right_constants[iStation]
	          - y_o*right_constants[iStation+5*n_right];

      
      // Correct the Y coordinate

      y_cor = y_o + right_constants[iStation+n_right]
  	          + x_o*right_constants[iStation+5*n_right];
    }

    // Now put the coordinate in the new track

    Gaudi::XYZPoint trackcoord(0.,0.,0.);
    Gaudi::XYZPoint trackerrors(0.,0.,0.);

    //double r   = sqrt(x_cor*x_cor+y_cor*y_cor);

    // Errors are calculated as (pitch/sqrt(12)) for the moment (SV 08/07/05)
    
    //double error_r = (40+(101.6-40.0)*(r-8.19)/(41.95-8.19))/3464.;
    //double error_p = 0.;

    //if (r <= 17.2) 
    //  error_p = (35.5+(78.3-35.5)*(r-8.19)/(17.2-8.19))/3464.;
    //if (r > 17.2)  
    //  error_p = (39.3+(96.6-39.3)*(r-17.2)/(41.95-17.2))/3464.;
    //Silvia
    double error_x =((mistrack.Coords()[k]).second).x();
    double error_y =((mistrack.Coords()[k]).second).y();
    //old error
    //double error_x = sqrt(pow(x_cor*error_r,2)+pow(y_cor*error_p,2))/r;
    //double error_y = sqrt(pow(y_cor*error_r,2)+pow(x_cor*error_p,2))/r;

    // Finally fill the AlignTrack

    trackcoord  = Gaudi::XYZPoint(x_cor,y_cor,z_cor);
    trackerrors = Gaudi::XYZPoint(error_x,error_y,((mistrack.Coords()[k]).second).z()); 

    n_valid_coord++;
    first_track.addCoord(trackcoord,trackerrors);
  }

    
  first_track.setNIsGood(true);
  first_track.setNType(mistrack.nType());
  first_track.setNGoodCoordinates(n_valid_coord);

  first_track.GetTrackSlope();

  if (!first_track.nIsGood()) return StatusCode::SUCCESS;

  double slopex = first_track.nSlope_x();
  double slopey = first_track.nSlope_y();

  n_valid_coord = 0;

  for (int k=0; k<(first_track.nGoodCoordinates()); k++)  // We loop an all the track hits
  {
    // Retrieve coordinates from the track object

    x_cor = ((first_track.Coords()[k]).first).x();
    y_cor = ((first_track.Coords()[k]).first).y();
    z_cor = ((first_track.Coords()[k]).first).z();
    x_o = x_cor;
    y_o = y_cor;
    //z_o = z_cor;

    int iStation = int(((first_track.Coords()[k]).second).z());
    int side     = first_track.nType();


    if (side >= 2) // Overlap track
    {
      int n_ove     = int(iStation)%100;
      iStation      = int(iStation-n_ove)/100;

      iStation/=2;
    }
    
    // Correct the coordinates with non-lin term

    if (side == 0)  // Left hit
    {
      // Correct the X coordinate

      x_cor = x_o - slopex*(left_constants[iStation+2*n_left] 
			    + y_o*left_constants[iStation+3*n_left]
			    + x_o*left_constants[iStation+4*n_left]);
      
      // Correct the Y coordinate

      y_cor = y_o - slopey*(left_constants[iStation+2*n_left] 
			    + y_o*left_constants[iStation+3*n_left]
			    + x_o*left_constants[iStation+4*n_left]);
    }
    else // Right hit
    {
      // Correct the X coordinate

      x_cor = x_o - slopex*(right_constants[iStation+2*n_right] 
			    + y_o*right_constants[iStation+3*n_right]
			    + x_o*right_constants[iStation+4*n_right]);
      // Correct the Y coordinate

      y_cor = y_o - slopey*(right_constants[iStation+2*n_right] 
			    + y_o*right_constants[iStation+3*n_right]
			    + x_o*right_constants[iStation+4*n_right]);
    }

    // Now put the coordinate in the new track

    Gaudi::XYZPoint trackcoord(0.,0.,0.);
    Gaudi::XYZPoint trackerrors(0.,0.,0.);

    //double r   = sqrt(x_cor*x_cor+y_cor*y_cor);

    // Errors are calculated as (pitch/sqrt(12)) for the moment (SV 08/07/05)
    
    //double error_r = (40+(101.6-40.0)*(r-8.19)/(41.95-8.19))/3464.;
    //double error_p = 0.;

    //if (r <= 17.2) 
    //  error_p = (35.5+(78.3-35.5)*(r-8.19)/(17.2-8.19))/3464.;
    //if (r > 17.2)  
    //  error_p = (39.3+(96.6-39.3)*(r-17.2)/(41.95-17.2))/3464.;
    
    //Silvia
    double error_x =((mistrack.Coords()[k]).second).x();
    double error_y =((mistrack.Coords()[k]).second).y();
    //old error
    //double error_x = sqrt(pow(x_cor*error_r,2)+pow(y_cor*error_p,2))/r;
    //double error_y = sqrt(pow(y_cor*error_r,2)+pow(x_cor*error_p,2))/r;

    // Finally fill the AlignTrack

    trackcoord  = Gaudi::XYZPoint(x_cor,y_cor,z_cor);
    trackerrors = Gaudi::XYZPoint(error_x,error_y,((first_track.Coords()[k]).second).z()); 

    n_valid_coord++;
    correctrack.addCoord(trackcoord,trackerrors);
  }

  correctrack.setNType(mistrack.nType());
  correctrack.setNGoodCoordinates(n_valid_coord);
  correctrack.setNIsGood(true);
  
  correctrack.GetTrackSlope();

  return StatusCode::SUCCESS;
}

