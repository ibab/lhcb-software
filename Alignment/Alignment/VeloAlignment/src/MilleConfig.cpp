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
				bool DOF[], double Sigm[], bool Cons[],
				int nloc, double starfact, int nstd, double res_cut, 
				double res_cut_init, double VELOmap[]) 
{

  m_i_align = i_align;    // Is it internal or box alignment ?
  m_right = right;        // Do we align left or right box ?

  for (int j=0; j<6; j++) {m_DOF[j] = DOF[j];} // What are the parameter to align ?

  for (int j=0; j<9; j++) {m_Cons[j] = Cons[j];} // What are the constraints to define ?
  for (int j=0; j<6; j++) {m_Sigm[j] = Sigm[j];}
  
//  
// We first get the number of stations to align
//
// The parameters zmoy** and s_zmoy** will be used in the constraints
// We also get the first and last sensors here
//

  int ialc      = -1;
  int iblc      = 0;
  double zmoy   = 0.0;
  double s_zmoy = 0.0;
  int nonzer    = 0;

  for (int j=0; j<21; j++)
  {
    CorrectVELOmap[j] = -1;
    if (VELOmap[j+21*m_right] != 0.) 
    {
      CorrectVELOmap[j] = nonzer; 
      if (ialc == -1) ialc=j;	 // first station
      iblc = j;       	     	 // last  station
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

  m_nlast  = iblc;
  m_nfirst = ialc;
   
  m_i_align ? m_nglo = nonzer : m_nglo = 2;    // Number of station to align (2 if box alignment)
  m_nloc = nloc;                               // Number of local params
  m_startfact = starfact;                       // Start factor for Chi^2 local fit cut
  m_nstd = nstd;

  // Initialize Millepede

  my_millepede->InitMille(&m_DOF[0], &m_Sigm[0], GetNstations(), m_nloc, 
			  m_startfact, m_nstd, res_cut, res_cut_init);


  if (m_i_align) // Parameters for constraint equation (internal alignment only)
  {

    int First_Plan = GetFirst();     // First station to be aligned (for VELO)
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
  
    for (int j=First_Plan; j<21; j++) 
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
      else // Stations not included
      {
	my_millepede->ParSig(CorrectVELOmap[j]          ,0.);
	my_millepede->ParSig(Nstations+CorrectVELOmap[j]  ,0.);
	my_millepede->ParSig(2*Nstations+CorrectVELOmap[j],0.);
	my_millepede->ParSig(3*Nstations+CorrectVELOmap[j],0.);
	my_millepede->ParSig(4*Nstations+CorrectVELOmap[j],0.);
	my_millepede->ParSig(5*Nstations+CorrectVELOmap[j],0.);
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

StatusCode MilleConfig::PutTrack(AlignTrack* atrack, IMillepede *my_millepede, int after)
{

  int First_Plan = GetFirst();       // First station to be aligned (for VELO)
  int Nstations  = GetNstations();   // Number of stations to be aligned (for VELO)
  int Nlocal     = GetNlocal();      // Number of params to be aligned (for VELO)
  int Nparams    = 6*Nstations;      // Number of params to be aligned (for VELO)

  StatusCode sc;

 // Global parameters initializations

  std::vector<double>   derLC; 
  std::vector<double>   derGB;

  derGB.resize(Nparams);     // Vector containing the global derivatives 
  derLC.resize(Nlocal);      // Vector containing the local derivatives

  for (int i=0; i<Nparams; i++) {derGB[i]=0.;}
  for (int i=0; i<Nlocal; i++) {derLC[i]=0.;}

  double track_params[8];     // Vector containing the track parameters 

  for (int i=0; i<8; i++) {track_params[i]=0.;}

  int n_station = 0;

  double x_cor  = 0.;
  double y_cor  = 0.;
  double z_cor  = 0.;
  double err_x  = 0.;
  double err_y  = 0.;
  double slopex = atrack->nSlope_x();
  double slopey = atrack->nSlope_y();

  for (int k=0; k<(atrack->nGoodCoordinates()); k++)  // We loop an all the track hits
  {
    // Retrieve coordinates from the track object

    x_cor = ((atrack->Coords()[k]).first).x();
    y_cor = ((atrack->Coords()[k]).first).y();
    z_cor = ((atrack->Coords()[k]).first).z();
    err_x = ((atrack->Coords()[k]).second).x();
    err_y = ((atrack->Coords()[k]).second).y();
    n_station = int(((atrack->Coords()[k]).second).z());
    n_station = CorrectVELOmap[n_station];

    my_millepede->ZerLoc(&derGB[0],&derLC[0]); // reset derLC and derGB arrays

    // LOCAL 1st derivatives for the X equation
	    
    derLC[0] = 1.0;
    derLC[1] = z_cor;
    derLC[2] = 0.0;
    derLC[3] = 0.0;

    // GLOBAL 1st derivatives (see LHCbnote-2005-101 for definition)
    
    if (m_DOF[0]) derGB[n_station]             = -1.0;             // dX	    
    if (m_DOF[1]) derGB[Nstations+n_station]   =  0.0;             // dY
    if (m_DOF[2]) derGB[2*Nstations+n_station] =  slopex;          // dZ
    if (m_DOF[3]) derGB[3*Nstations+n_station] =  y_cor*slopex;    // d_alpha
    if (m_DOF[4]) derGB[4*Nstations+n_station] =  x_cor*slopex;    // d_beta
    if (m_DOF[5]) derGB[5*Nstations+n_station] =  y_cor;           // d_gamma
   
    sc = my_millepede->EquLoc(&derGB[0], &derLC[0], x_cor, err_x); // Store hits parameters
    if (! sc) {break;} 	

    my_millepede->ZerLoc(&derGB[0],&derLC[0]); // reset derLC and derGB arrays

    // LOCAL 1st derivatives for the Y equation

    derLC[0] = 0.0;
    derLC[1] = 0.0;
    derLC[2] = 1.0;
    derLC[3] = z_cor;

    // GLOBAL 1st derivatives
    
    if (m_DOF[0]) derGB[n_station]             =  0.0;             // dX	    
    if (m_DOF[1]) derGB[Nstations+n_station]   = -1.0;             // dY
    if (m_DOF[2]) derGB[2*Nstations+n_station] = slopey;           // dZ
    if (m_DOF[3]) derGB[3*Nstations+n_station] = y_cor*slopey;     // d_alpha
    if (m_DOF[4]) derGB[4*Nstations+n_station] = x_cor*slopey;     // d_beta
    if (m_DOF[5]) derGB[5*Nstations+n_station] = -x_cor;           // d_gamma
    
    sc = my_millepede->EquLoc(&derGB[0], &derLC[0], y_cor, err_y); // Store hits parameters
    if (! sc) {break;} 	
  }

  sc = my_millepede->FitLoc(my_millepede->GetTrackNumber(),track_params,after); // Fit the track
  if (sc.isSuccess())
  {
    if (after == 1) // Used to update the track parameters after internal alignment
    {
      atrack->setNSlope_x(track_params[2]);
      atrack->setNXo_x(track_params[0]);
      atrack->setNErrX_x(track_params[1]);
      atrack->setNSlope_y(track_params[6]);
      atrack->setNYo_y(track_params[4]);
      atrack->setNErrY_y(track_params[5]);
      
      // Compute also the Z of closest approach
    
      double zclos = 0.0;
      double sx = (atrack)->nSlope_x();
      double sy = (atrack)->nSlope_y();
      double x0 = (atrack)->nXo_x();
      double y0 = (atrack)->nYo_y();
      
      if (sx!=0.0 || sy !=0.0) zclos = -(sx*x0+sy*y0)/(sx*sx+sy*sy);
      atrack->setNZclos(zclos);
    }

    if (after == 0) my_millepede->SetTrackNumber(my_millepede->GetTrackNumber()+1); // We are in the first loop  
                                                                                  // update needed for the store

    return StatusCode::SUCCESS;
  }

  return StatusCode::FAILURE;
}


/*
-----------------------------------------------------------
  PUTPVTRACK : Here we perform the local fit
-----------------------------------------------------------  

  Here is the real primary vertex fit method
  
-----------------------------------------------------------
*/

StatusCode MilleConfig::PutPVTrack(AlignTracks* aPV, IMillepede* my_millepede, int nPV)
{

  int Nstations  = GetNstations();   // Number of stations to be aligned (for VELO)
  int Nlocal     = GetNlocal();      // Number of params to be aligned (for VELO)
  int Nparams    = 6*Nstations;      // Number of params to be aligned (for VELO)
  
  AlignTracks::const_iterator itrack;

  double slx, sly, x0, y0, errx, erry, zclos;

 // Global parameters initializations

  std::vector<double>   derLC; 
  std::vector<double>   derGB;

  derGB.resize(Nparams);     // Vector containing the global derivatives 
  derLC.resize(Nlocal);      // Vector containing the local derivatives

  for (int i=0; i<Nparams; i++) {derGB[i]=0.;}
  for (int i=0; i<Nlocal; i++) {derLC[i]=0.;}

  double track_params[6];     // Vector containing the vertex parameters 

  for (int i=0; i<6; i++) {track_params[i]=0.;}

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
      
      //    zclos = -(slx*x0+sly*y0)/(slx*slx+sly*sly);
      zclos = (*itrack)->nZclos(); // This choice is under testing
      y0 = sly*zclos+y0;
      x0 = slx*zclos+x0;
      
      my_millepede->ZerLoc(&derGB[0],&derLC[0]); // reset derLC and derGB arrays
      
      // LOCAL 1st derivatives for v_x 
      
      derLC[0] = 0.;
      derLC[1] = 0.;
      derLC[2] = slx;
      
      // GLOBAL 1st derivatives
      
      if (m_DOF[0]) derGB[(*itrack)->nType()]     =  1.0;             // dX	    
      if (m_DOF[1]) derGB[2+(*itrack)->nType()]   =  0.0;            // dY
      if (m_DOF[2]) derGB[4+(*itrack)->nType()]   = -slx;            // dZ
      if (m_DOF[3]) derGB[6+(*itrack)->nType()]   = 0.0;            // dtX    
      if (m_DOF[4]) derGB[8+(*itrack)->nType()]   = zclos;            // dtY    
      if (m_DOF[5]) derGB[10+(*itrack)->nType()]  = -y0;            // dtZ   

      sc = my_millepede->EquLoc(&derGB[0], &derLC[0], -x0, errx); // Store hits parameters
      if (! sc) {break;}
      
      my_millepede->ZerLoc(&derGB[0],&derLC[0]); // reset derLC and derGB arrays
      
      // LOCAL 1st derivatives for v_y
      
      derLC[0] = 0.;
      derLC[1] = 0.;
      derLC[2] = sly;	    
      
      // GLOBAL 1st derivatives
      
      if (m_DOF[0]) derGB[(*itrack)->nType()]     =  0.0;             // dX	    
      if (m_DOF[1]) derGB[2+(*itrack)->nType()]   =  1.0;            // dY
      if (m_DOF[2]) derGB[4+(*itrack)->nType()]   = -sly;            // dZ
      if (m_DOF[3]) derGB[6+(*itrack)->nType()]   = zclos;            // dtX    
      if (m_DOF[4]) derGB[8+(*itrack)->nType()]   = 0.0;            // dtY   
      if (m_DOF[5]) derGB[10+(*itrack)->nType()]  =  x0;            // dtZ   

      sc = my_millepede->EquLoc(&derGB[0], &derLC[0], -y0, erry); // Store hits parameters
      if (! sc) {break;}
    }
  }

  // Fit the vertex

  sc = my_millepede->FitLoc(my_millepede->GetTrackNumber(),track_params,0); // Fit the track

  if (sc.isSuccess()) 
  {
    my_millepede->SetTrackNumber(my_millepede->GetTrackNumber()+1);
    return StatusCode::SUCCESS;
  }  

  return StatusCode::FAILURE;
}



/*
-----------------------------------------------------------
  FINDPVTRACKS:  looking for primary vetices candidates
-----------------------------------------------------------

  my_config    = the alignment configuration
  event        = the event number

  ....This method is under development....

-----------------------------------------------------------
*/

StatusCode MilleConfig::FindPVTracks(AlignTracks* aPV, IMillepede* my_millepede, double m_z_min,
				     double m_z_max, int m_PV_trackmin, double m_z_sigma, 
				     double m_IPmax, double m_TrIPmax, int& nPV, int nEvent,
				     std::vector<double> PVvals[]) 
{
   StatusCode sc;

  AlignTracks::const_iterator itrack;

  double zclos;

  double m_Sigma = m_z_sigma;
  double m_Gran = 1.;

  int n_sel_tracks;
  int n_v_tracks;
  double sum_zclos;
  int max = 0; 

  int sigmaBins = 1 + int(m_Sigma/m_Gran);

  bool keepgoing = true;


  double sX, sY, x0, y0;

  double sig_x = 0.01;
  double sig_y = 0.01;

  double a_x = 0.0;
  double b_x = 0.0;
  double c_x = 0.0;
  double d_x = 0.0;
  double e_x = 0.0;

  double a_y = 0.0;
  double b_y = 0.0;
  double c_y = 0.0;
  double d_y = 0.0;
  double e_y = 0.0;

  double vp_x, vp_y, vp_z, chisq;

  PVHisto::iterator start, end, maxBin;
 
  for (itrack = aPV->begin(); itrack != aPV->end(); ++itrack ) 
  {
    (*itrack)->setNPVtag(false);      
    (*itrack)->setNPVnumber(0);  
  }

  // Histogram for peak finding and parallel vector to pvTracks for caching track info
  PVHisto zCloseHisto( m_z_min, m_z_max, m_Gran);

  while (keepgoing == true)
  {
    n_sel_tracks = 0;
    zCloseHisto.reset();

    for (itrack = aPV->begin(); itrack != aPV->end(); ++itrack ) 
    {
      if((*itrack)->nPVtag() == false && (*itrack)->nPVnumber() != -1 
	 && (*itrack)->nZclos() > m_z_min && (*itrack)->nZclos() < m_z_max)  
      {
        n_sel_tracks++;
	zCloseHisto.fill((*itrack)->nZclos());
      }
    }

    // Don't need to find a maximum if there is not enough tracks 

    if (n_sel_tracks < m_PV_trackmin)
    {
      max = -1; 
      keepgoing = false; // end of the quest
    }
    else
    {
      // Find the maximum of the histogram
      max = 0;
      for( PVHisto::iterator bin = zCloseHisto.begin(); zCloseHisto.end() != bin; bin++) 
      {
        if( *bin >= max ) 
	{
	  max = *bin;
	  maxBin = bin;
        }
      }

      a_x = 0.0;
      b_x = 0.0;
      c_x = 0.0;
      d_x = 0.0;
      e_x = 0.0;
      
      a_y = 0.0;
      b_y = 0.0;
      c_y = 0.0;
      d_y = 0.0;
      e_y = 0.0;
      
      // Try and make a preliminary vertex out of this bump
      start = maxBin - sigmaBins;
      if( start < zCloseHisto.begin() ) start = zCloseHisto.begin();
      end = maxBin + sigmaBins;
      if( end > zCloseHisto.end() ) end = zCloseHisto.end();
      
      // Now fill in the track vector
      // Count the number of tracks

      nPV++;
      n_v_tracks = 0;
      sum_zclos = 0.;
      PVHisto::iterator zclos_bin;

      // First loop just to count the number of tracks in the window

      for (itrack = aPV->begin(); itrack != aPV->end(); ++itrack ) 
      {
       	if((*itrack)->nPVtag() == false)  
	{
	  zclos_bin = zCloseHisto.getbin((*itrack)->nZclos());      
	  
	  if (zclos_bin <= end && zclos_bin >= start)
	  {
	    n_v_tracks++;
	    (*itrack)->setNPVtag(true);      
	    (*itrack)->setNPVnumber(nPV);      
	  }
	}
      }

      if (n_v_tracks < m_PV_trackmin) // Not enough tracks
      {
	for (itrack = aPV->begin(); itrack != aPV->end(); ++itrack ) 
	{
	  if ((*itrack)->nPVnumber() == nPV) // Make the track non re-usable (!!perfectible!!)  
	  {
	    (*itrack)->setNPVtag(false);      
	    (*itrack)->setNPVnumber(-1);      
	  }
	}

 	nPV--;
      }
      else // Good candidate
      {
	for (itrack = aPV->begin(); itrack != aPV->end(); ++itrack ) 
	{
	  if ((*itrack)->nPVtag() == true && (*itrack)->nPVnumber() == nPV) 
	  {
	    zclos_bin = zCloseHisto.getbin((*itrack)->nZclos());      
	    zclos = (*itrack)->nZclos();      
	    
	    if (zclos_bin <= end && zclos_bin >= start)
	    {
	      sum_zclos+=zclos;
	      
	      sX = (*itrack)->nSlope_x();
	      sY = (*itrack)->nSlope_y();
	      x0 = (*itrack)->nXo_x();
	      y0 = (*itrack)->nYo_y();
	      sig_x = (*itrack)->nErrX_x();
	      sig_y = (*itrack)->nErrX_x();

	      a_x += x0/(sig_x*sig_x);
	      b_x += 1/(sig_x*sig_x);
	      c_x += sX/(sig_x*sig_x);
	      d_x += (sX*sX)/(sig_x*sig_x);
	      e_x += (sX*x0)/(sig_x*sig_x);
	      
	      a_y += y0/(sig_y*sig_y);
	      b_y += 1/(sig_y*sig_y);
	      c_y += sY/(sig_y*sig_y);
	      d_y += (sY*sY)/(sig_y*sig_y);
	      e_y += (sY*y0)/(sig_y*sig_y); 	    
	    }
	  }
	}
	
	vp_z = ((c_x*a_x)/b_x+(c_y*a_y)/b_y-e_x-e_y)/(d_x+d_y-(c_x*c_x)/b_x-(c_y*c_y)/b_y); 
	vp_y = c_y*vp_z/b_y+a_y/b_y;
	vp_x = c_x*vp_z/b_x+a_x/b_x;
	
	//	double zPosition = sum_zclos / n_v_tracks;
	
	// Now try to compute chisquare

	chisq = 0.;

       	for (itrack = aPV->begin(); itrack != aPV->end(); ++itrack ) 
	{
	  if ((*itrack)->nPVtag() == true && (*itrack)->nPVnumber() == nPV) // Track into the vertex
	  {
	      x0 = (*itrack)->nXo_x()+(*itrack)->nSlope_x()*(*itrack)->nZclos();
	      y0 = (*itrack)->nYo_y()+(*itrack)->nSlope_y()*(*itrack)->nZclos();
	      sig_x = (*itrack)->nErrX_x();
	      sig_y = (*itrack)->nErrX_x();

	      chisq += ((x0-vp_x)/sig_x)*((x0-vp_x)/sig_x);
	      chisq += ((y0-vp_y)/sig_y)*((y0-vp_y)/sig_y);
	  }
	}

	// New loop to suppress outliers

	double IParam = 0.;
	double chiprop = 0.;
	double IP_PV = 0.;

	for (itrack = aPV->begin(); itrack != aPV->end(); ++itrack ) 
	{
	  if ((*itrack)->nPVtag() == true && (*itrack)->nPVnumber() == nPV) 
	  {
	    //	    x0 = (*itrack)->nXo_x()+(*itrack)->nSlope_x()*(*itrack)->nZclos();
	    //	    y0 = (*itrack)->nYo_y()+(*itrack)->nSlope_y()*(*itrack)->nZclos();
	    x0 = (*itrack)->nXo_x()+(*itrack)->nSlope_x()*vp_z;
	    y0 = (*itrack)->nYo_y()+(*itrack)->nSlope_y()*vp_z;
	    sig_x = (*itrack)->nErrX_x();
	    sig_y = (*itrack)->nErrX_x();

	    IParam = sqrt((x0-vp_x)*(x0-vp_x)+(y0-vp_y)*(y0-vp_y));
	    chiprop = (((x0-vp_x)*(x0-vp_x))/(sig_x*sig_x)+((y0-vp_y)*(y0-vp_y))/(sig_y*sig_y))/chisq;
	    IP_PV  = sqrt(vp_x*vp_x+vp_y*vp_y);
	    
	    if (chiprop >= 3*(chisq/n_v_tracks) || IParam >= m_TrIPmax || IP_PV >= m_IPmax)
	    {
	      (*itrack)->setNPVtag(false);      
	      (*itrack)->setNPVnumber(-1);      
	      n_v_tracks--;
	    }
	  }
	}
      
	if (n_v_tracks < m_PV_trackmin) // Not enough tracks
	{
	  for (itrack = aPV->begin(); itrack != aPV->end(); ++itrack ) 
	  {
	    if ((*itrack)->nPVnumber() == nPV) // Make the track non re-usable (!!perfectible!!)  
	    {
	      (*itrack)->setNPVtag(false);      
	      (*itrack)->setNPVnumber(-1);      
	    }
	  }

	  nPV--;
	}
	else
	{
	  sc = PutPVTrack(aPV,my_millepede,nPV);
 
	  if (sc.isSuccess())
	  {
	    PVvals->push_back(float(nEvent));    
	    PVvals->push_back(float(nPV));    
	    PVvals->push_back(float(n_v_tracks));    
	    PVvals->push_back(vp_x);    
	    PVvals->push_back(vp_y);    
	    PVvals->push_back(vp_z);    
	  }	 
	}
      }
    }
  }
 
  return StatusCode::SUCCESS;
}
