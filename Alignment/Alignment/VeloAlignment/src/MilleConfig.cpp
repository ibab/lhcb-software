// Include files 
//#include "GaudiKernel/MsgStream.h"

// local
#include "MilleConfig.h"
#include "PVHisto.h"

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

void MilleConfig::InitMilleTool(IMillepede *my_millepede, bool i_align, int right,
				std::vector<bool> DOF, std::vector<double> Sigm,
				std::vector<bool> Cons, int nloc, double starfact, 
				int nstd, std::vector<double> res_cut, double VELOmap[]) 
{

  m_i_align = i_align;    // Is it internal (TRUE) or box alignment (FALSE) ?
  m_right = right;        // Do we align left (FALSE) or right box (TRUE) ?

  for (int j=0; j<6; j++) {m_DOF[j] = DOF[j];}   // What are the parameter to align ?

  for (int j=0; j<9; j++) {m_Cons[j] = Cons[j];} // What are the constraints equations to define ?
  for (int j=0; j<6; j++) {m_Sigm[j] = Sigm[j];} // What are the 'annealing' factors transmitted to the global matrix ?
  
//  
// We first get the number of stations to align
//
// The parameters zmoy** and s_zmoy** will be used in the constraints
// We also get the first and last sensors here
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
   
  m_i_align ? m_nglo = nonzer : m_nglo = 2;    // Number of station to align (2 if box alignment)
  m_nloc      = nloc;                          // Number of local params
  m_startfact = starfact;                      // Start factor for Chi^2 local fit cut
  m_nstd      = nstd;                          // Num. of std deviations for local fit 

  // Initialize Millepede

  my_millepede->InitMille(&m_DOF[0], &m_Sigm[0], GetNstations(), m_nloc, 
			  m_startfact, m_nstd, res_cut[1], res_cut[0]);


  if (m_i_align) // Parameters for constraint equation (internal alignment only)
  {

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

    double zmoy   = Get_zmoy();
    double s_zmoy = Get_szmoy();
  
    for (int j=0; j<21; j++) 
    {
      double z_station = VELOmap[j+21*m_right];
      
      if (z_station != 0.) // Only included stations
      {
	//	std::cout << "CorrectVELOmap[" << j << "] = " << CorrectVELOmap[j] << std::endl;

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
  }
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

  after     = PUTTRACK could be used in 2 ways: if after=0
              one do the local fit and update the Millepede
	      object. after=1 means that you just fit the track
	      and don't update Millepede.

-----------------------------------------------------------
*/

StatusCode MilleConfig::PutTrack(LHCb::AlignTrack* atrack, IMillepede *my_millepede)
{

  int Nstations  = GetNstations();   // Number of stations to be aligned (for VELO)
  int Nlocal     = GetNlocal();      // Number of params to be aligned (for VELO)
  int Nparams    = 6*Nstations;      // Number of params to be aligned (for VELO)

  StatusCode sc;

 // Global parameters initializations

  std::vector<double>   derLC; 
  std::vector<double>   derGB;
  std::vector<double>   derNonLin;

  derGB.resize(Nparams);          // Vector containing the global derivatives 
  derNonLin.resize(Nparams);      // Global derivatives non linearly related to residual
  derLC.resize(Nlocal);           // Vector containing the local derivatives
  m_aliConstants.resize(Nparams); // Vector containing the alignment constants 

  for (int i=0; i<Nparams; i++) {derGB[i]=0.; m_aliConstants[i]=0.;}
  for (int i=0; i<Nlocal; i++)  {derLC[i]=0.;}


  double track_params[10];     // Vector containing the track parameters 

  for (int i=0; i<10; i++) {track_params[i]=0.;}

  int n_station = 0;

  double x_cor  = 0.;
  double y_cor  = 0.;
  double z_cor  = 0.;
  double err_x  = 0.;
  double err_y  = 0.;

  for (int k=0; k<(atrack->nGoodCoordinates()); k++)  // We loop an all the track hits
  {
    // Retrieve coordinates from the track object

    x_cor = ((atrack->Coords()[k]).first).x();
    y_cor = ((atrack->Coords()[k]).first).y();
    z_cor = ((atrack->Coords()[k]).first).z();

    n_station = int(((atrack->Coords()[k]).second).z());
    n_station = CorrectVELOmap[n_station];

    err_x = ((atrack->Coords()[k]).second).x();
    err_y = ((atrack->Coords()[k]).second).y();

    my_millepede->ZerLoc(&derGB[0],&derLC[0],&derNonLin[0]); // reset derLC and derGB arrays

    // LOCAL 1st derivatives for the X equation
	    
    derLC[0] = 1.0;
    derLC[1] = z_cor;
    derLC[2] = 0.0;
    derLC[3] = 0.0;

    // GLOBAL 1st derivatives (see LHCbnote-2005-101 for definition)

    if (m_DOF[0]) derGB[n_station]             = -1.0;             // dX	    
    if (m_DOF[1]) derGB[Nstations+n_station]   =  0.0;             // dY
    if (m_DOF[2]) derGB[2*Nstations+n_station] =  0.0;          // dZ
    if (m_DOF[3]) derGB[3*Nstations+n_station] =  0.0;    // d_alpha
    if (m_DOF[4]) derGB[4*Nstations+n_station] =  0.0;    // d_beta
    if (m_DOF[5]) derGB[5*Nstations+n_station] =  y_cor;           // d_gamma
    
    if (m_DOF[0]) derNonLin[n_station]             =  0.0;             // dX	    
    if (m_DOF[1]) derNonLin[Nstations+n_station]   =  0.0;             // dY
    if (m_DOF[2]) derNonLin[2*Nstations+n_station] =  2000.0 + 1.0;          // dZ
    if (m_DOF[3]) derNonLin[3*Nstations+n_station] =  2000.0 + y_cor;    // d_alpha
    if (m_DOF[4]) derNonLin[4*Nstations+n_station] =  2000.0 + x_cor;    // d_beta
    if (m_DOF[5]) derNonLin[5*Nstations+n_station] =  0.0;           // d_gamma
    
    sc = my_millepede->EquLoc(&derGB[0], &derLC[0], &derNonLin[0], x_cor, err_x); // Store hits parameters
    if (! sc) {break;} 	

    my_millepede->ZerLoc(&derGB[0],&derLC[0],&derNonLin[0]); // reset derLC and derGB arrays

    // LOCAL 1st derivatives for the Y equation

    derLC[0] = 0.0;
    derLC[1] = 0.0;
    derLC[2] = 1.0;
    derLC[3] = z_cor;

    // GLOBAL 1st derivatives

    if (m_DOF[0]) derGB[n_station]             =  0.0;             // dX	    
    if (m_DOF[1]) derGB[Nstations+n_station]   = -1.0;             // dY
    if (m_DOF[2]) derGB[2*Nstations+n_station] = 0.0;           // dZ
    if (m_DOF[3]) derGB[3*Nstations+n_station] = 0.0;     // d_alpha
    if (m_DOF[4]) derGB[4*Nstations+n_station] = 0.0;     // d_beta
    if (m_DOF[5]) derGB[5*Nstations+n_station] = -x_cor;           // d_gamma
    
    if (m_DOF[0]) derNonLin[n_station]             =  0.0;             // dX	    
    if (m_DOF[1]) derNonLin[Nstations+n_station]   =  0.0;             // dY
    if (m_DOF[2]) derNonLin[2*Nstations+n_station] =  -2000.0 + 1.0;          // dZ
    if (m_DOF[3]) derNonLin[3*Nstations+n_station] =  -2000.0 + y_cor;    // d_alpha
    if (m_DOF[4]) derNonLin[4*Nstations+n_station] =  -2000.0 + x_cor;    // d_beta
    if (m_DOF[5]) derNonLin[5*Nstations+n_station] =  0.0;           // d_gamma

    sc = my_millepede->EquLoc(&derGB[0], &derLC[0], &derNonLin[0], y_cor, err_y); // Store hits parameters
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

StatusCode MilleConfig::PutOverlapTrack(LHCb::AlignTrack* atrack, IMillepede *my_millepede)
{

  int isdebug = 0;

  if (isdebug == 1)
  {
    std::cout << "" << std::endl;
    std::cout << "Num. of good coords : " << (atrack->nGoodCoordinates()) << std::endl;
  }

  int Nstations  = GetNstations();   // Number of stations to be aligned (for VELO)
  int Nlocal     = GetNlocal();      // Number of params to be aligned (for VELO)
  int Nparams    = 6*Nstations;      // Number of params to be aligned (for VELO)
  double zmoyr = Get_zmoy();

  int after      = 0;

  StatusCode sc;

 // Global parameters initializations

  std::vector<double>   derLC; 
  std::vector<double>   derGB;
  std::vector<double>   derNonLin;

  derNonLin.resize(Nparams);   
  derGB.resize(Nparams);     // Vector containing the global derivatives 
  derLC.resize(Nlocal);      // Vector containing the local derivatives

  for (int i=0; i<Nparams; i++) {derGB[i]=0.;}
  for (int i=0; i<Nlocal; i++) {derLC[i]=0.;}

  double track_params[10];     // Vector containing the track parameters 

  for (int i=0; i<10; i++) {track_params[i]=0.;}

  for (int k=0; k<(atrack->nGoodCoordinates()); k++)  // We loop an all the track hits
  {
    // Retrieve coordinates from the track object

    double x_cor     = ((atrack->Coords()[k]).first).x();
    double y_cor     = ((atrack->Coords()[k]).first).y();
    double z_cor     = ((atrack->Coords()[k]).first).z();
    double err_x     = ((atrack->Coords()[k]).second).x();
    double err_y     = ((atrack->Coords()[k]).second).y();
    int n_station    = int(((atrack->Coords()[k]).second).z());

    int n_ove     = int(n_station)%100;
    n_station = int(n_station-n_ove)/100;;

    n_station = n_station%2;

    my_millepede->ZerLoc(&derGB[0],&derLC[0],&derNonLin[0]); // reset derLC and derGB arrays

    if (isdebug == 1)
    {
      cout << "Overlap Type   : " << n_ove << endl;
      cout << "Station number : " << n_station << endl;
      cout << "x = " << x_cor << endl; 
      cout << "y = " << y_cor << endl; 
      cout << "z = " << z_cor << endl; 
      cout << "side = " << n_station << endl; 
    }

    // LOCAL 1st derivatives for the X equation
	    
    derLC[0] = 1.0;
    derLC[1] = z_cor;
    derLC[2] = 0.0;
    derLC[3] = 0.0;

    // GLOBAL 1st derivatives (see LHCbnote-2005-101 for definition)
    
    if (n_station == 1)
    {
      if (m_DOF[0]) derGB[n_station]    = -1.0;             // dX	    
      if (m_DOF[1]) derGB[2+n_station]  =  0.0;             // dY
      if (m_DOF[2]) derGB[4+n_station]  =  0.0;          // dZ
      if (m_DOF[3]) derGB[6+n_station]  =  0.0;    // d_alpha
      if (m_DOF[4]) derGB[8+n_station]  =  z_cor - zmoyr;    // d_beta
      if (m_DOF[5]) derGB[10+n_station] =  y_cor;           // d_gamma
      
      if (m_DOF[0]) derNonLin[n_station]    =  0.0;             // dX	    
      if (m_DOF[1]) derNonLin[2+n_station]  =  0.0;             // dY
      if (m_DOF[2]) derNonLin[4+n_station]  =  2000.0 + 1.;          // dZ
      if (m_DOF[3]) derNonLin[6+n_station]  =  2000.0 + y_cor;    // d_alpha
      if (m_DOF[4]) derNonLin[8+n_station]  =  2000.0 + x_cor;    // d_beta
      if (m_DOF[5]) derNonLin[10+n_station] =  0.0;           // d_gamma
    }

    sc = my_millepede->EquLoc(&derGB[0], &derLC[0],&derNonLin[0], x_cor, err_x); // Store hits parameters
    if (! sc) {break;} 	

    my_millepede->ZerLoc(&derGB[0],&derLC[0],&derNonLin[0]); // reset derLC and derGB arrays

    // LOCAL 1st derivatives for the Y equation

    derLC[0] = 0.0;
    derLC[1] = 0.0;
    derLC[2] = 1.0;
    derLC[3] = z_cor;

    // GLOBAL 1st derivatives

    if (n_station == 1)
    {   
      if (m_DOF[0]) derGB[n_station]    =  0.0;             // dX	    
      if (m_DOF[1]) derGB[2+n_station]  = -1.0;             // dY
      if (m_DOF[2]) derGB[4+n_station]  =  0.0;           // dZ
      if (m_DOF[3]) derGB[6+n_station]  =  z_cor - zmoyr;     // d_alpha
      if (m_DOF[4]) derGB[8+n_station]  =  0.0;     // d_beta
      if (m_DOF[5]) derGB[10+n_station] = -x_cor;           // d_gamma
      
      if (m_DOF[0]) derNonLin[n_station]    =  0.0;             // dX	    
      if (m_DOF[1]) derNonLin[2+n_station]  =  0.0;             // dY
      if (m_DOF[2]) derNonLin[4+n_station]  = -2000.0 + 1.;          // dZ
      if (m_DOF[3]) derNonLin[6+n_station]  = -2000.0 + y_cor;    // d_alpha
      if (m_DOF[4]) derNonLin[8+n_station]  = -2000.0 + x_cor;    // d_beta
      if (m_DOF[5]) derNonLin[10+n_station] =  0.0;           // d_gamma
    }

    sc = my_millepede->EquLoc(&derGB[0], &derLC[0],&derNonLin[0], y_cor, err_y); // Store hits parameters
    if (! sc) {break;} 	
  }

  sc = my_millepede->FitLoc(my_millepede->GetTrackNumber(),track_params,after); // Fit the track
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

StatusCode MilleConfig::PutPVTrack(LHCb::AlignTracks* aPV, IMillepede* my_millepede, int nPV)
{

  int Nstations  = GetNstations();   // Number of stations to be aligned (for VELO)
  int Nlocal     = GetNlocal();      // Number of params to be aligned (for VELO)
  int Nparams    = 6*Nstations;      // Number of params to be aligned (for VELO)
  
  LHCb::AlignTracks::const_iterator itrack;

  double slx, sly, x0, y0, errx, erry, zclos;

 // Global parameters initializations

  std::vector<double>   derLC; 
  std::vector<double>   derGB;
  std::vector<double>   derNonLin;

  derGB.resize(Nparams);     // Vector containing the global derivatives 
  derNonLin.resize(Nparams);     
  derLC.resize(Nlocal);      // Vector containing the local derivatives

  for (int i=0; i<Nparams; i++) {derGB[i]=0.;}
  for (int i=0; i<Nlocal; i++) {derLC[i]=0.;}

  double vertex_params[8];     // Vector containing the vertex parameters 

  for (int i=0; i<8; i++) {vertex_params[i]=0.;}

  StatusCode sc;

  for (itrack = aPV->begin(); itrack != aPV->end(); ++itrack ) 
  {
    if ((*itrack)->nPVnumber() == nPV) // A good track
    {
      slx  = (*itrack)->nSlope_x();
      x0   = (*itrack)->nXo_x();
      errx = (*itrack)->nErrX_x();
      sly  = (*itrack)->nSlope_y();
      y0   = (*itrack)->nYo_y();
      erry = (*itrack)->nErrY_y();;

      zclos = (*itrack)->nPV_z();      
      y0    = sly*zclos+y0;
      x0    = slx*zclos+x0;
      
      my_millepede->ZerLoc(&derGB[0],&derLC[0],&derNonLin[0]); // reset derLC and derGB arrays
      
      // LOCAL 1st derivatives for v_x 
      
      derLC[0] = 0.;
      derLC[1] = 0.;
      derLC[2] = -slx;
      
      // GLOBAL 1st derivatives
      
      if (m_DOF[0]) derGB[(*itrack)->nType()]     =  -1.0;             // dX	    
      if (m_DOF[1]) derGB[2+(*itrack)->nType()]   =  0.0;            // dY
      if (m_DOF[2]) derGB[4+(*itrack)->nType()]   = 0.0;            // dZ
      if (m_DOF[3]) derGB[6+(*itrack)->nType()]   = -zclos;            // dtX    
      if (m_DOF[4]) derGB[8+(*itrack)->nType()]   = 0.0;            // dtY    
      if (m_DOF[5]) derGB[10+(*itrack)->nType()]  = y0;            // dtZ   

      if (m_DOF[0]) derNonLin[(*itrack)->nType()]     =  0.0;             // dX	    
      if (m_DOF[1]) derNonLin[2+(*itrack)->nType()]   =  0.0;            // dY
      if (m_DOF[2]) derNonLin[4+(*itrack)->nType()]   =  0.0;            // dZ
      if (m_DOF[3]) derNonLin[6+(*itrack)->nType()]   =  0.0;            // dtX    
      if (m_DOF[4]) derNonLin[8+(*itrack)->nType()]   =  0.0;            // dtY    
      if (m_DOF[5]) derNonLin[10+(*itrack)->nType()]  =  0.0;            // dtZ   

      sc = my_millepede->EquLoc(&derGB[0], &derLC[0],&derNonLin[0], x0, errx); // Store hits parameters
      if (! sc) {break;}
      
      my_millepede->ZerLoc(&derGB[0],&derLC[0],&derNonLin[0]); // reset derLC and derGB arrays
      
      // LOCAL 1st derivatives for v_y
      
      derLC[0] = 0.;
      derLC[1] = 0.;
      derLC[2] = -sly;	    
      
      // GLOBAL 1st derivatives
      
      if (m_DOF[0]) derGB[(*itrack)->nType()]     =  0.0;             // dX	    
      if (m_DOF[1]) derGB[2+(*itrack)->nType()]   =  -1.0;            // dY
      if (m_DOF[2]) derGB[4+(*itrack)->nType()]   = 0.0;            // dZ
      if (m_DOF[3]) derGB[6+(*itrack)->nType()]   = 0.0;            // dtX    
      if (m_DOF[4]) derGB[8+(*itrack)->nType()]   = -zclos;            // dtY   
      if (m_DOF[5]) derGB[10+(*itrack)->nType()]  = -x0;            // dtZ   

      if (m_DOF[0]) derNonLin[(*itrack)->nType()]     =  0.0;             // dX	    
      if (m_DOF[1]) derNonLin[2+(*itrack)->nType()]   =  0.0;            // dY
      if (m_DOF[2]) derNonLin[4+(*itrack)->nType()]   =  0.0;            // dZ
      if (m_DOF[3]) derNonLin[6+(*itrack)->nType()]   =  0.0;            // dtX    
      if (m_DOF[4]) derNonLin[8+(*itrack)->nType()]   =  0.0;            // dtY    
      if (m_DOF[5]) derNonLin[10+(*itrack)->nType()]  =  0.0;            // dtZ 

      sc = my_millepede->EquLoc(&derGB[0], &derLC[0],&derNonLin[0], y0, erry); // Store hits parameters
      if (! sc) {break;}
    }
  }

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

StatusCode MilleConfig::correcTrack(LHCb::AlignTrack* mistrack, 
				    LHCb::AlignTrack* correctrack, 
				    std::vector<double> left_constants, 
				    std::vector<double> right_constants,
				    std::vector<double> box_constants,  
				    double VELOmap[])
{

  LHCb::AlignTrack* first_track = new LHCb::AlignTrack();

  double z_moy_R =0;
  double z_moy_L =0;

  double x_cor, y_cor, z_cor;
  double x_o, y_o, z_o;

  int n_left  = left_constants.size()/6;
  int n_right = right_constants.size()/6;

  if (n_left != n_right) return StatusCode::SUCCESS; // Stop here if we don't have the same number of stations on each side

  correctrack->setNIsGood(false);

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

  for (int k=0; k<(mistrack->nGoodCoordinates()); k++)  // We loop an all the track hits
  {
    // Retrieve coordinates from the track object

    x_cor = ((mistrack->Coords()[k]).first).x();
    y_cor = ((mistrack->Coords()[k]).first).y();
    z_cor = ((mistrack->Coords()[k]).first).z();
    x_o = x_cor;
    y_o = y_cor;
    z_o = z_cor;

    //    std::cout << "" << std::endl;
    //    std::cout << "Track coordinate before correction : " << x_cor << " / " << y_cor << " / " << z_cor << std::endl;

    int iStation = int(((mistrack->Coords()[k]).second).z());
    int side     = mistrack->nType();

    if (side >= 2) // Overlap track
    {
      int n_ove     = int(iStation)%100;
      iStation      = int(iStation-n_ove)/100;

      (iStation%2 == 0)
	? side = 0
	: side = 1;

      iStation/=2;
    }
    
    // Correct the coordinates

    if (side == 0)  // Left hit
    {
      // Correct the X coordinate

      x_cor = x_o + left_constants[iStation]
	          + box_constants[0] + box_constants[6]*(VELOmap[iStation]-z_moy_L) 
	          - y_o*left_constants[iStation+5*n_left];
      
      // Correct the Y coordinate

      y_cor = y_o + left_constants[iStation+n_left]
	          + box_constants[2] + box_constants[8]*(VELOmap[iStation]-z_moy_L) 
	          + x_o*left_constants[iStation+5*n_left];
    }
    else // Right hit
    {
      // Correct the X coordinate

      x_cor = x_o + right_constants[iStation]
	          + box_constants[1] + box_constants[7]*(VELOmap[iStation+21]-z_moy_R) 
	          - y_o*right_constants[iStation+5*n_right];
      
      // Correct the Y coordinate

      y_cor = y_o + right_constants[iStation+n_right]
	          + box_constants[3] + box_constants[9]*(VELOmap[iStation+21]-z_moy_R) 
  	          + x_o*right_constants[iStation+5*n_right];
    }

    //    std::cout << "" << std::endl;
    //    std::cout << "Track coordinate after correction : " << x_cor << " / " << y_cor << " / " << z_cor << std::endl;

    // Now put the coordinate in the new track

    Gaudi::XYZPoint trackcoord(0.,0.,0.);
    Gaudi::XYZPoint trackerrors(0.,0.,0.);

    double r   = sqrt(x_cor*x_cor+y_cor*y_cor);

    // Errors are calculated as (pitch/sqrt(12)) for the moment (SV 08/07/05)
    
    double error_r = (40+(101.6-40.0)*(r-8.19)/(41.95-8.19))/3464.;
    double error_p = 0.;

    if (r <= 17.2) 
      error_p = (35.5+(78.3-35.5)*(r-8.19)/(17.2-8.19))/3464.;
    if (r > 17.2)  
      error_p = (39.3+(96.6-39.3)*(r-17.2)/(41.95-17.2))/3464.;
    
    double error_x = sqrt(pow(x_cor*error_r,2)+pow(y_cor*error_p,2))/r;
    double error_y = sqrt(pow(y_cor*error_r,2)+pow(x_cor*error_p,2))/r;

    // Finally fill the AlignTrack

    trackcoord  = Gaudi::XYZPoint(x_cor,y_cor,z_cor);
    trackerrors = Gaudi::XYZPoint(error_x,error_y,((mistrack->Coords()[k]).second).z()); 

    n_valid_coord++;
    first_track->addCoord(trackcoord,trackerrors);
  }

    
  first_track->setNIsGood(true);
  first_track->setNType(mistrack->nType());
  first_track->setNGoodCoordinates(n_valid_coord);

  MilleConfig::GetTrackSlope(first_track);

  if (!first_track->nIsGood()) return StatusCode::SUCCESS;

  double slopex = first_track->nSlope_x();
  double slopey = first_track->nSlope_y();

  n_valid_coord = 0;

  for (int k=0; k<(first_track->nGoodCoordinates()); k++)  // We loop an all the track hits
  {
    // Retrieve coordinates from the track object

    x_cor = ((first_track->Coords()[k]).first).x();
    y_cor = ((first_track->Coords()[k]).first).y();
    z_cor = ((first_track->Coords()[k]).first).z();
    x_o = x_cor;
    y_o = y_cor;
    z_o = z_cor;

    //    std::cout << "" << std::endl;
    //    std::cout << "Track coordinate before correction : " << x_cor << " / " << y_cor << " / " << z_cor << std::endl;

    int iStation = int(((first_track->Coords()[k]).second).z());
    int side     = first_track->nType();

    if (side >= 2) // Overlap track
    {
      int n_ove     = int(iStation)%100;
      iStation      = int(iStation-n_ove)/100;       
      iStation/=2;
    }
    
    // Correct the coordinates with non-lin term, only for non-overlap tracks however 

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

    //    std::cout << "" << std::endl;
    //    std::cout << "Track coordinate after correction : " << x_cor << " / " << y_cor << " / " << z_cor << std::endl;

    // Now put the coordinate in the new track

    Gaudi::XYZPoint trackcoord(0.,0.,0.);
    Gaudi::XYZPoint trackerrors(0.,0.,0.);

    double r   = sqrt(x_cor*x_cor+y_cor*y_cor);

    // Errors are calculated as (pitch/sqrt(12)) for the moment (SV 08/07/05)
    
    double error_r = (40+(101.6-40.0)*(r-8.19)/(41.95-8.19))/3464.;
    double error_p = 0.;

    if (r <= 17.2) 
      error_p = (35.5+(78.3-35.5)*(r-8.19)/(17.2-8.19))/3464.;
    if (r > 17.2)  
      error_p = (39.3+(96.6-39.3)*(r-17.2)/(41.95-17.2))/3464.;
    
    double error_x = sqrt(pow(x_cor*error_r,2)+pow(y_cor*error_p,2))/r;
    double error_y = sqrt(pow(y_cor*error_r,2)+pow(x_cor*error_p,2))/r;

    // Finally fill the AlignTrack

    trackcoord  = Gaudi::XYZPoint(x_cor,y_cor,z_cor);
    trackerrors = Gaudi::XYZPoint(error_x,error_y,((first_track->Coords()[k]).second).z()); 

    n_valid_coord++;
    correctrack->addCoord(trackcoord,trackerrors);
  }

  correctrack->setNType(mistrack->nType());
  correctrack->setNGoodCoordinates(n_valid_coord);

  delete first_track;
  
  MilleConfig::GetTrackSlope(correctrack);

  return StatusCode::SUCCESS;
}



//------------------------------------------------------------------
//  GETTRACKSLOPE: get straight line parameters
//------------------------------------------------------------------
//
//  atrack      = the track to be processed and updated
//
//------------------------------------------------------------------


StatusCode MilleConfig::GetTrackSlope(LHCb::AlignTrack* atrack)
{
  int Ncoords      = atrack->nGoodCoordinates();

  double sx, sy, cx, cy, err_cx, err_cy;

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

  if (detx == 0. || dety == 0.) 
  {
    atrack->setNIsGood(false);
    return StatusCode::SUCCESS;
  }

  sx = (local_vecx[1]*local_matx[0][0]-local_vecx[0]*local_matx[1][0])/detx;
  sy = (local_vecy[1]*local_maty[0][0]-local_vecy[0]*local_maty[1][0])/dety;
  cx = (local_vecx[0]*local_matx[1][1]-local_vecx[1]*local_matx[1][0])/detx;
  cy = (local_vecy[0]*local_maty[1][1]-local_vecy[1]*local_maty[1][0])/dety;

  atrack->setNSlope_x((local_vecx[1]*local_matx[0][0]-local_vecx[0]*local_matx[1][0])/detx);
  atrack->setNSlope_y((local_vecy[1]*local_maty[0][0]-local_vecy[0]*local_maty[1][0])/dety);

  atrack->setNXo_x((local_vecx[0]*local_matx[1][1]-local_vecx[1]*local_matx[1][0])/detx);
  atrack->setNYo_y((local_vecy[0]*local_maty[1][1]-local_vecy[1]*local_maty[1][0])/dety);

  double zclos = 0.;
	  
  if (sx!=0.0 || sy!=0.0) zclos  = -(sx*cx+sy*cy)/(sx*sx+sy*sy);

  //  cout << "MILLECONFIG : " << zclos << endl;

  err_cx = sqrt(fabs(local_matx[1][1]/detx));
  err_cy = sqrt(fabs(local_maty[1][1]/dety));

  atrack->setNErrX_x(err_cx);
  atrack->setNErrY_y(err_cy);

  atrack->setNZclos(zclos);
  atrack->setNIsGood(true);

  return StatusCode::SUCCESS; 
}

