// Include files:

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/ToolFactory.h" 
#include "GaudiKernel/MsgStream.h"

#include "Centipede.h"
#include "math.h"
// global
#include <iostream>
#include <iomanip>
#include <fstream>

/** @class Centipede Centipede.h AlignmentTools/Centipede.h
 *  
 *
 *  @author Marc Deissenroth
 *  @date   2007-11-14
 * Implementation of Centipede.
 *
 * This is a helper class which inherits from the Millepede class.
 * It is used to provide the Heidelberg alignment algorithm with
 * a unified interface to Millepede.
 * Some Functions in Millepede have been modified such, that they are 
 * only used in Centipede. 
 * 
 */


DECLARE_TOOL_FACTORY( Centipede );

Centipede::Centipede( const std::string& type,
                      const std::string& name,
                      const IInterface* parent ) 
  :Millepede(type,name,parent), 
   DEBUGGLOBAL(false),
   DEBUGLOCAL(false)
{
  declareInterface<ICentipede>(this);
  info() << "Initializing Centipede!" << endreq;
};

Centipede::~Centipede() { 
 
};

// StatusCode Centipede::initialize()
// {
//   StatusCode sc = GaudiTupleTool::initialize();
//   if ( sc.isFailure() ) {
//     error() << "Error initialising Centipede" << endreq;
//     return sc;
//   }
//   return sc;
// }


StatusCode Centipede::MakeGlobalFit( std::vector<double> &para, std::vector<double> &err ) {
  double *par = new double[para.size()];
  double *error = new double[err.size()];
  VectortoArray(para,par);
  VectortoArray(err,error);

  int i, j,nvar;
  int nfixed; //no of fixed parameters

  //int nstillgood;

  double sum;

  double step[150];

  //  double trackpars[2*mlocal];

  int ntotal = Millepede::GetTrackNumber();  
  if(true){//DEBUGGLOBAL){
    info() <<"*******************************************************"<<endreq
           <<"**** preparing global fit                     *********"<<endreq
           <<"****                                          *********"<<endreq
           <<"****                                          *********"<<endreq
           <<"****...using " << ntotal << " tracks          *********"<<endreq
           <<"*******************************************************"<<endreq;
  }
  
  for (i=0; i<nagb; i++) {
    diag[i] = cgmat[i][i];
  }


  ////////////////////////////////////////////////////////////////////////////////////
  //  Then we retrieve the different constraints: fixed parameter or global equation//
  ////////////////////////////////////////////////////////////////////////////////////

  nfixed = 0; // First look at the fixed global params MD nof fixed params
    
  for (i=0; i<nagb; i++)
  {
    if(psigm[i] == (-1.) ) continue; //MD  default value->do nothing
      
    if (psigm[i] == 0.0)   //MD  fixed global param 
    {
      nfixed++;

      for (j=0; j<nagb; j++)
      {
        cgmat[i][j] = 0.0;  // Reset row and column
        cgmat[j][i] = 0.0;
      }			
    }
    else if (psigm[i] > 0.0) // constrain movement of plane
    {
      cgmat[i][i] += 1.0/(psigm[i]*psigm[i]);
    }
  }
  nvar = nagb;  // Current number of equations	
    
  if (false) std::cout << "Number of constraint equations : " << ncs << std::endl;
  info() << "--> Centi : ntotal " << ntotal << endreq;
  
  
  if (ncs > 0) // Then the constraint equation
  {
    for (i=0; i<ncs; i++)
    {
      sum = arhs[i];
      for (j=0; j<nagb; j++)
      {
        cgmat[nvar][j] = float(ntotal)*adercs[i][j];
        cgmat[j][nvar] = float(ntotal)*adercs[i][j];          
        if(psigm[j] == 0.){
          info() <<" --> Centi: set par to zero...."<<endreq;
          
          cgmat[nvar][j] = 0.0;
          cgmat[j][nvar] = 0.0;
        }
        
        sum -= adercs[i][j]*(pparm[j]+dparm[j]);
      }
	
      cgmat[nvar][nvar] = 0.0; // and the non-diagonal elements...?
      bgvec[nvar] = float(ntotal)*sum;
      nvar++;
    }
  }


  ///////////////////////////////////////////////////////////
  /////   Intended to compute the final global chisquare  ///
  ////     during iterations                             ////
  ///////////////////////////////////////////////////////////
  double final_cor = 0.0;
  if (itert > 1){
    for ( j = 0; j < nagb; ++j){
      for ( i = 0; i < nagb; ++i){
        if (psigm[i] != 0.0){ //MD forget about zero entries in cgmat
          final_cor += step[j]*cgmat[j][i]*step[i]; 
          if (i == j) final_cor -= step[i]*step[i]/(psigm[i]*psigm[i]);
        }
      }
    }
  }
  if(false){
    std::cout << " Final coeff is " << final_cor << std::endl;		
    std::cout << " Final NDOFs =  " << nagb << std::endl;
  }
    
  info() << " row& column of cgmat " << nvar << endreq;
  
  //---------------------------------------------------------------
  //-------  The final matrix inversion --
  //--
  //-- cgmat and bgvec already corrected !(loop 3\3 fitloc() )
  //---------------------------------------------------------------
  int nrank = 0;
  nrank = SpmInvGlobal( cgmat, &bgvec[0], nvar);
  if(DEBUGGLOBAL){
    std::cout <<"*******************************************************"<<std::endl
              <<"**** GLOBAL MATRIX INVERSION DONE  ********************"<<std::endl
              <<"*******************************************************"<<std::endl;
  }


  std::ofstream outputFile("Inverse_Cmatrix.txt",std::ios::app);
  if (outputFile.fail() ){
    return Warning("Failed to open matrix output file",StatusCode::FAILURE);
  }
  outputFile << nagb <<std::endl;//first line = number of rows and columns
  for(int r=0;r<nagb;r++){
    for(int c=0;c<nagb;c++){
      //      plot3D(c,r,cgmat[r][c],"C_after_Inversion",0,nagb,0,nagb,0,1e6,nagb,nagb,1000);
      outputFile <<  cgmat[r][c] << std::endl; //fill file row wise
    }
  }
  
  

    
  for (i = 0; i < nagb; ++i){
    /* Update global parameters values (for iterations)*/
    dparm[i] += bgvec[i];
    if(DEBUGGLOBAL){ std::cout << "Update global parameters values (for iterations)" 
                               << i << " " << dparm[i] << std::endl;
    }
  }
    
  std::cout << "The rank defect of the symmetric " << nvar << " by " << nvar 
            << " matrix is " << nvar-nfixed-nrank << " (bad if non 0)" << std::endl;
  std::cout << "Total : " << loctot << " local fits, " 
            << locrej << " rejected." << "  test number = "  << test << std::endl;
  //MD
  test =0.;
  
  // Reinitialize parameters for iteration
  loctot = 0;
  locrej = 0;
    
  if (cfactr != cfactref && sqrt(cfactr) > 1.2*cfactref)
  {
    if(DEBUGGLOBAL) std::cout << " the cutfactor was in this iteration step..." << cfactr << std::endl;
    cfactr = sqrt(cfactr);
    if(DEBUGGLOBAL) std::cout << " the cutfactor in the next iteration step will be..." << cfactr << std::endl;
  }
  else
  {
    if(DEBUGGLOBAL) std::cout <<" the cutfactor was in this iteration step..." << cfactr << std::endl;
    cfactr = cfactref;
    if(DEBUGGLOBAL) std::cout <<" the cutfactor in the next iteration step will be..." << cfactr << std::endl;     
    //      itert = itelim;
  }
  //MD15-04
  //   if( m_residual_cut_init > 2.5)
  //     m_residual_cut_init = 0.5*(m_residual_cut_init);
  //   if( m_residual_cut_init < 2.5)
  //     m_residual_cut_init = 2.5;
  //   info() << "---> outlier rejection in next iteration = " <<   m_residual_cut_init << endreq;
  
  Millepede::PrtGlo(); // Print the final results

  for (j=0; j<nagb; j++)
  {
    par[j]   = dparm[j];
    debug() << " CENTIPEDE par["<<j<<"] = " << par[j] << endmsg;
    //dparm[j] = 0.;
    error[j] = sqrt(fabs(cgmat[j][j]));
  }

  std::cout << std::setw(10) << " " << std::endl;
  std::cout << std::setw(10) << "            * o o                   o      " << std::endl;
  std::cout << std::setw(10) << "              o o                   o      " << std::endl;
  std::cout << std::setw(10) << "   o ooooo  o o o  oo  ooo   oo   ooo  oo  " << std::endl;
  std::cout << std::setw(10) << "    o  o  o o o o o  o o  o o  o o  o o  o " << std::endl;
  std::cout << std::setw(10) << "    o  o  o o o o oooo o  o oooo o  o oooo " << std::endl;
  std::cout << std::setw(10) << "    o  o  o o o o o    ooo  o    o  o o    " << std::endl;
  std::cout << std::setw(10) << "    o  o  o o o o  oo  o     oo   ooo  oo ++ ends." << std::endl;
  std::cout << std::setw(10) << "                       o                   " << std::endl;	  


  //////////////////////////////////
  //   Reset global variables    //
  ////////////////////////////////


  info() << " MILLEPEDE : nvar = " << nvar << endreq;
  
  
  for (i=0; i<nvar; i++)
  {
    bgvec[i] = 0.0;
    dparm[i] = 0.0;
    pparm[i] = 0.0;
    
    for (j=0; j<nvar; j++)
    {
      cgmat[i][j] = 0.0;
    }
  }


  ArraytoVector( par, para );
  ArraytoVector( error, err );
  delete [] par;
  delete [] error;

  return StatusCode::SUCCESS;
};
StatusCode Centipede::InitMille( int nglo,
                                 int nloc, 
                                 double startfact, 
                                 int nstd,
                                 double res_cut, 
                                 double res_cut_init ) 
{
  
  //Md 
  test = 0.;
  

  std::cout << "" << std::endl;
  std::cout << "----------------------------------------------------" << std::endl;
  std::cout << "" << std::endl;
  std::cout << "    Entering InitMille" << std::endl;
  std::cout << "" << std::endl;
  std::cout << "-----------------------------------------------------" << std::endl;
  std::cout << "" << std::endl;

  ncs = 0;
  loctot  = 0;                        // Total number of local fits
  locrej  = 0;                        // Total number of local fits rejected
  cfactref  = 1.0;                    // Reference value for Chi^2/ndof cut

  Millepede::SetTrackNumber(0);       // Number of local fits (starts at 0)
  
  m_NofDOFs = 0;
  m_residual_cut = res_cut;
  m_residual_cut_init = res_cut_init; 
  //  m_NofPlanes = NofPlanes;
  
  nagb	  = nglo;    // Number of global derivatives
  nalc	  = nloc;       // Number of local derivatives
  nstdev  = nstd;     // Number of StDev for local fit chisquare cut

  //   m_res_value.clear(); //clear res vectors for local fit iterations
  //   m_res_index.clear(); //clear resindex vectors for local fit iterations
  //   m_res_counter = 0; // starting value for counter of iteration
  //   m_res_trackcounter = 0; //starting valur for track counter
  //   m_rmeas_guess.clear();  
  
  
  std::cout << "Number of global parameters   : " << nagb << std::endl;
  std::cout << "Number of local parameters    : " << nalc << std::endl;
  std::cout << "Number of standard deviations : " << nstdev << std::endl;
  
  
  if (nagb>mglobl || nalc>mlocal)
  {
    std::cout << "Too many parameters !!!!!" << std::endl;
    return false;
  }

  // Global parameters initializations

  for (int i=0; i<nagb; i++)
  {
    bgvec[i]=0.; // global measurement vector
    pparm[i]=0.; // starting misalignment values
    dparm[i]=0.; // misalignment values after iterations
    psigm[i]=-1.; //default value for ParSig()
    indnz[i]=-1;
    indbk[i]=-1;
    //nlnpa[i]=0;
    
    for (int j=0; j<nagb;j++)
    {
      cgmat[i][j]=0.;
    }
  }

  // Local parameters initializations
  
  for (int i=0; i<nalc; i++)
  {
    blvec[i]=0.;
    
    for (int j=0; j<nalc;j++)
    {
      clmat[i][j]=0.;
    }
  }

  // Activate iterations (if requested)

  itert   = 0;	// By default iterations are turned off
  cfactr = startfact;
  //if (m_iteration) 
  Millepede::InitUn(startfact);          
  
  arest.clear();  // Number of stored parameters when doing local fit
  //arenl.clear();  // Is it linear or not?
  indst.clear(); 

  // storeind.clear();
  //storeare.clear();
  //  storenl.clear();
  storeplace.clear();
  
  info() << "----> the 'cfactr' = "  << cfactr <<  endmsg;  

  std::cout << "" << std::endl;
  std::cout << "----------------------------------------------------" << std::endl;
  std::cout << "" << std::endl;
  std::cout << "    InitMille has been successfully called!" << std::endl;
  std::cout << "" << std::endl;
  std::cout << "-----------------------------------------------------" << std::endl;
  std::cout << "" << std::endl;
	
  return true;



}
/*
  -----------------------------------------------------------
  CONSTF: define a constraint equation in Millepede
  -----------------------------------------------------------

  dercs    = the row containing constraint equation 
  derivatives (put into the final matrix)

  rhs      = the lagrange multiplier value (sum of equation)	     

  -----------------------------------------------------------
*/

StatusCode Centipede::ConstF(std::vector<double> dercs, double rhs)
{  
  if (ncs>=mcs) // mcs is defined in Millepede.h
  {
    info() << "Too many constraints !!!" << endmsg;
    return StatusCode::FAILURE;
  }
 	
  for (int i=0; i<nagb; i++) {
    adercs[ncs][i] = dercs[i];
    info() << " --> " << i << " dercs = " << dercs[i] << " adercs["<<ncs<<"]["<<i<<"]="<< adercs[ncs][i]<< endreq;
    
  }
 	
  arhs[ncs] = rhs;
  ncs++ ;
  info() << "Number of constraints increased to " << ncs << endmsg;
  return StatusCode::SUCCESS;
}


/*
  -----------------------------------------------------------
  EQULOC: write ONE equation in the matrices
  -----------------------------------------------------------

  dergb[1..nagb]	= global parameters derivatives
  derlc[1..nalc] 	= local parameters derivatives
  rmeas  		= measured value
  sigma 		= error on measured value (nothin to do with ParSig!!!)

  -----------------------------------------------------------
*/

StatusCode Centipede::EquLoc( std::vector<double> &gb, 
                              std::vector<double> &lc, 
                              const double &rmeas, 
                              double sigma ) {	

  double dergb[gb.size()];
  double derlc[lc.size()];
  VectortoArray(gb, &dergb[0]);
  VectortoArray(lc, &derlc[0]);


  if (sigma<=0.0) // If parameter is fixed, then no equation
  {
    for (int i=0; i<nalc; i++)
    {
      derlc[i] = 0.0;
    }
    for (int i=0; i<nagb; i++)
    {
      dergb[i] = 0.0;
    }
    return true;
  }

  // Serious equation, initialize parameters
  	
  double wght =  1.0/(sigma*sigma);
  int nonzer  =  0;
  int ialc    = -1;
  int iblc    = -1;
  int iagb    = -1;
  int ibgb    = -1;

  for (int i=0; i<nalc; i++) // Retrieve local param interesting indices
  {
    double fabsder = fabs(derlc[i]);
    if (derlc[i]!=0.0 && fabsder >= 1e-10 )
    {
      nonzer++;
      if (ialc == -1) ialc=i;	// first index
      iblc = i;       	     	// last index
    }
  }
	
  for (int i=0; i<nagb; i++)  // Idem for global parameters
  {
    
    if (dergb[i]!=0.0 )
    {
      nonzer++;
      if (iagb == -1) iagb=i;	// first index
      ibgb = i; 	     	// last index
    }
  }


  indst.push_back(-1);
  arest.push_back(rmeas);
  
  for (int i=ialc; i<=iblc; i++) 
  {
    if (derlc[i]!=0.0 && i!=(-1) )
    {
      indst.push_back(i);
      arest.push_back(derlc[i]);
      // arenl.push_back(0.0);
      derlc[i]   = 0.0;
    }
  }

  //   for (unsigned int i=0 ; i< arest.size()  ; i++ ) {
  //     TRACE( "EQULOC after local " << PRINTV(i) << PRINTV(arest.at(i))
  //            << PRINTV(indst.at(i)));    
  //   }
  //  TRACE( "EQULOC after local" << PRINTV(arest.back()) << PRINTV(arest.size()) );  
  

  indst.push_back(-1);
  arest.push_back(wght);
  // arenl.push_back(0.0);


  for (int i=iagb; i<=ibgb; i++)
  {
    if (dergb[i]!=0.0 && i!=(-1) ) //MD12-12-07 added extra i!=...
    {
      indst.push_back(i);
      arest.push_back(dergb[i]);
      //      arenl.push_back(dernl[i]);
      dergb[i]   = 0.0;  // reference: also affects MilleTool::derGB[]!!
    }
  }	

  ArraytoVector(&dergb[0],gb);
  ArraytoVector(&derlc[0],lc);
    
  
  //    info() << "Out Equloc --  NST = " << arest.size() << endreq;
  for(unsigned int u = 0; u<arest.size();u++)
    debug() <<  " indst["<<u<<"] =  "<< indst[u] << "  arest["<<u<<"] = " <<  arest[u] << endreq;
    
  
  return true; 	
}


/*
  -----------------------------------------------------------
  ZERLOC: reset the derivative vectors
  -----------------------------------------------------------

  dergb[1..nagb]		= global parameters derivatives
  dergb[1..nalc]		= local parameters derivatives
 
  -----------------------------------------------------------
*/
 
StatusCode Centipede::ZerLoc( std::vector<double> &dergb, 
                              std::vector<double> &derlc ) {

  
  m_derGB = new double [dergb.size()];
  m_derLC = new double [derlc.size()];
  m_dernl = new double [dergb.size()];
  m_dernl_i = new double [dergb.size()];
  VectortoArray( dergb, m_derGB );
  VectortoArray( derlc, m_derLC );

  StatusCode sc = Millepede::ZerLoc( m_derGB, m_derLC, m_dernl, m_dernl_i );
  ArraytoVector( m_derGB, dergb);
  ArraytoVector( m_derLC, derlc);
  delete[] m_derGB;
  delete[] m_derLC;
  delete[] m_dernl;
  delete[] m_dernl_i;

  return sc;
}


StatusCode Centipede::FitLoc( int n,
                              int locrank,
                              std::vector< std::vector< double> >  locVec,
                              std::vector<double> &track,
                              int single_fit,
                              std::vector<double> &estimated,
                              double &chi2,
                              double &res ) 
{
  // Few initializations
  int i, j, k, ik, ij, ist, nderlc, ndergl, ndf;
  int ja      = -1;
  int jb      = 0;
  int nagbn   = 0;
  double rmeas, wght, rms;//MD 15-04 cutval;
  double summ = 0.;
  double locChi2 = 0;
  chi2  = 0.0;
  double estimation_sum = 0.;
  int    nsum  = 0;
  nst   = 0; 
  nst   = arest.size(); // number of stored derivatives(loc&glob) & wght(for each glob entry)


  Millepede::SetTrackNumber(n);
  
  //MD new appproach  
  //   double track_params[track.size()];
  //   double estimated_para[estimated.size()];
  //   VectortoArray(track, &track_params[0]);
  //   VectortoArray(estimated, &estimated_para[0]);

  //MDDEBUG  
  //   for ( unsigned int mi=0 ; mi < arest.size()  ;mi++  ) 
  //     info() <<" check..." << mi << " : indst= " << indst[mi] <<" arest= " << arest[mi] << endreq; 
  //  info() << " ---> begin of MP::fitloc() " << endreq;
  

  for (int ii=0; ii<nagb; ii++)  // fill estimated parameters
  {
    pparm[ii] = estimated.at(ii);
    //info() << "MP::FitLoc--> pparm[ii] : "<<pparm[ii] << endreq;
    
  }

  std::cout << std::setprecision(10) << std::fixed;
  
  for (i=0; i<nalc; i++) // reset local params
  {
    blvec[i] = 0.0;
    for (j=0; j<nalc; j++) {clmat[i][j] = 0.0;}
  }
  for (i=0; i<nagb; i++) {indnz[i] = -1;} // reset mixed params

  //************************************************
  // FIRST LOOP : local track fit
  //************************************************
  	
  ist = 0;
  indst.push_back(-1);
  

  //*********************************************************
  // Local params matrix is completed, now invert to solve...
  //***********************************************************
  
  //MD int nrank = 0;  //MD Rank is the number of nonzero diagonal elements 
  //  nrank = Millepede::SpmInv(clmat, &blvec[0], nalc, scdiag, scflag); MD 15-11
  //MD   nrank = SpmInv(clmat, &blvec[0], nalc);

  //MD new approach where I already fitted the track
  for (i=0; i<nalc; i++)
  {
    blvec[i] = track.at(i);
    debug() << "---> track at "<< i << " "<< blvec[i]<<endreq;
    for (k=0; k<nalc; k++){
      clmat[i][k] = locVec[i][k];
    }
  }
  
  
  if (DEBUGLOCAL) std::cout << "" << std::endl;
  if (DEBUGLOCAL) std::cout << " __________________________________________________" << std::endl;
  //MDDEBUG
  debug() << " Result of local fit :      (index/parameter/error)" << endreq;
  for (i=0; i<nalc; i++)
  {
    debug()  << std::setw(5) << i << "   /   " << std::setw(5) 
            << blvec[i] << "   /   " << sqrt(clmat[i][i]) << endreq;
    // //             for(int count=0; count<nalc; count++){
    // //               std::cout << "offdiag  :" << i <<" " << count << "  " << sqrt(fabs(clmat[i][count]))  
    // //                         << " sign of correlation: "<< clmat[i][count]  
    // //                         << " corr. coefficient : " <<  clmat[i][count] / (sqrt(clmat[i][i])* sqrt(clmat[i+1][i+1]) ) 
    // //       << std::endl;
    // //             }
    
  }

    
  //
  // SECOND LOOP : residual calculation
  //
  
  //MD-----------------------------
  //-- fill a tuple for monitoring 
  //-- residuals etc.
  //----------------------------
  //   Tuple t_trackMoni = nTuple("track_monitoring","track_monitoring");
  //   t_trackMoni->column("iteration",itera);
  test++;
  
  //  std::vector<double> tmeas;
  //  std::vector<double> tresid;
  //std::vector<double> tglresid;
  bool rej=0;// rejected
  
  
  ist = 0;
  ja = -1;
  jb = 0;
  //iteration over all hits on the track
  while (ist <= nst)
  {
    if (indst[ist] == -1)
    {

      if (ja == -1)     {ja = ist;}  // First  0 : rmeas
      else if (jb == 0) {jb = ist;}  // Second 0 : weight 
      else                           // Third  0 : end of equation  
      {	
        rmeas	= arest[ja];
        wght 	= arest[jb];
        nderlc = jb-ja-1;    // Number of local derivatives involved
        ndergl = ist-jb-1;   // Number of global derivatives involved
        
        //if (DEBUGLOCAL) std::cout << std::setprecision(4) << std::fixed;
        debug() << "--->> localfit 2nd loop: "<< endreq
                << "measured value = "  << rmeas << " +/- " << std::setw(13) << 1.0/sqrt(wght) << endreq;
        debug() << "Number of derivatives (global, local): " 
                << ndergl << ", " << nderlc << endreq;
        debug() << "Global derivatives are: (index/derivative/parvalue) " << endreq;
        for (i=(jb+1); i<ist; i++)
        {
          debug() << indst[i] << " / " << arest[i] << " / " << pparm[indst[i]] << endreq;
        } 
        debug() << "Local derivatives are: (index/derivative) " << endreq;
        for (i=(ja+1); i<jb; i++) {
          debug() << indst[i] << " / " << arest[i] << endreq;
        }	  
	
        //tmeas.push_back(rmeas);
        // Now suppress local and global parts to RMEAS;
        for (i=(ja+1); i<jb; i++) // First the local part 
        {
          j = indst[i];
          rmeas -= arest[i]*blvec[j];
          
        }
        //plot(rmeas,"CentiMeas","CentiMeas",-4.,4,60); 
        //for each hit on the track
        //tresid.push_back(rmeas);
        locChi2 += wght*rmeas*rmeas;
        
        for (i=(jb+1); i<ist; i++) // Then the global part
        {
          j = indst[i];
          rmeas -= arest[i]*(pparm[j]+dparm[j]);
          if(pparm[j] != 0.) info() << " pparm !=0 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<endreq;
          if(dparm[j] != 0.) info() << " dparm !=0 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<endreq;
          
        }


        // reject the track if rmeas is too important (outlier)
        if (fabs(rmeas) >= m_residual_cut_init && itert <= 1)  
        {
          if(single_fit == 0){
            locrej++;      
            loctot++; //although rej. it was fittet before !
            rej =1;
            info() << "# of outlier removals : " << locrej
                   << " m_residual_cut_init = " <<  m_residual_cut_init   
                   << " <   rmeas =  " << rmeas << endmsg;
          }
          indst.clear(); // reset stores and go to the next track 
          arest.clear();	  
          //MD 8-4-08 return false;
        }
        //tglresid.push_back(rmeas);
        if(rej) break;//MD 8-4-08
        
        //plot(rmeas,"rmeas"," MP: residual " , -15,15,300);
        summ += wght*rmeas*rmeas ; // total chi^2
        nsum++;                    // number of equations			
        estimation_sum += rmeas*rmeas; //MD
        ja = -1;
        jb = 0;
        ist--;
      } // End of "end of equation" operations
    }   // End of loop on equation
    ist++;
  } // End of loop on all equations used in the fit

  //  info() << " ---> Centipede :locrank = " << locrank << endreq;
  

  ndf = nsum-locrank; //nsum = number of measurements, nrank=nof local param.	
  rms = 0.0;
  //info() << "Final chi square / degrees of freedom "<< summ << " / " << ndf << endreq;
  if (ndf > 0) rms = summ/float(ndf);  // Chi^2/dof
	
  chi2 = summ;
  res = rms;
  //MD
  //double trchi2ndf=locChi2/ndf;
  //tmeas.resize(50);
  //tresid.resize(50);
  //tglresid.resize(50);
  //int n_tmeas  = tmeas.size();
  //int n_tresid = tresid.size();
  //int n_tglresid = tglresid.size();
  //   t_trackMoni->farray("measurement",tmeas,"n_tmeas",n_tmeas);
  //   t_trackMoni->farray("ith_track_residuum",tresid,"n_tresid", n_tresid);
  //   t_trackMoni->farray("ith_param_residuum",tglresid,"n_tglresid",n_tglresid);
  //   t_trackMoni->column("track_chi2",locChi2);
  //   t_trackMoni->column("track_chi2ndf",trchi2ndf);
  //   t_trackMoni->column("global_chi2",summ);
  //   t_trackMoni->column("global_chi2perNDF",rms);
  //   t_trackMoni->column("global_ndf",ndf);
  //  t_trackMoni->column("rejected",rej);
  //  t_trackMoni->write();  

  //tmeas.clear();
  //tresid.clear();
  //tglresid.clear();
  
  if(rej) return false;//MD 8-4-08
  
  
  //  info() << "Wght = "<< wght << endreq;
  // info() << "rmeas = " << rmeas/ndf << endreq;
  //info() << "summ = " << summ << endreq;
  
  if (single_fit == 0) {
    if(n==0) debug() << std::endl << "FitLoc called with argument '0' -> global der. update as well!!" << endreq;
    loctot++;
    //    INFO(PRINTV(loctot));
  }
  
  //   if (nstdev != 0 && ndf > 0 && single_fit != 1) // Chisquare cut
  //   {
  //     //MD
  //     cutval = Millepede::chindl(nstdev, ndf)*cfactr;
  //     //MD!!!!!
  //     if (rms > cutval) // Reject the track if too much...
  //     {
      
  //       locrej++;      
  //       info() <<  "---> Rejected track due to     chi2/dof = " << rms << " > "<< cutval << endmsg
  //              << "( the 'cfactr' = "  << cfactr << " )" << endmsg;
      
      
  //       indst.clear(); // reset stores and go to the next track 
  //       arest.clear();
  //       return false;
  //     }
  //   }

  if (single_fit == 1) //Stop here if just updating the track parameters
  {
    
    if(n==0) std::cout << std::endl << "FitLoc called with argument '1' -> no global der. update!!" << std::endl ;
    //locrej = 0; //just count rej. when global fit afterwards
    
    indst.clear(); // Reset store for the next track 
    arest.clear();
    return true;
  }

  //  info() << " ---> @ third loop " << endreq;
  
  //**************************************************************  
  // THIRD LOOP: local operations are finished, track is accepted 
  // We now update the global parameters (other matrices)
  //**************************************************************

  ist = 0;
  ja = -1;
  jb = 0;

  //  TRACE(PRINTV(indst.size()));

  while (ist <= nst)
  {

    if (indst[ist] == -1)
    {
      if (ja == -1)     {ja = ist;}  // First  0 : rmeas
      else if (jb == 0) {jb = ist;}  // Second 0 : weight 
      else                           // Third  0 : end of equation  
      {	
        rmeas	= arest[ja];
        wght 	= arest[jb];
        
        for (i=(jb+1); i<ist; i++) // Now suppress the global part
        {
          j = indst[i];   // Global param indice
          rmeas -= arest[i]*(pparm[j]+dparm[j]);
        }
        
        // First of all, the global/global terms (exactly like local matrix)

        for (i=(jb+1); i<ist; i++)  
        {
          j = indst[i];   // Global param indice (the matrix line)          
          bgvec[j] += wght*rmeas*arest[i];  // See note for precisions

          for (k=(jb+1); k<ist ; k++)
          {
            ik = indst[k];						
            cgmat[j][ik] += wght*arest[i]*arest[k];
          } 
        }

        // Now we have also rectangular matrices containing global/local terms.
        
        for (i=(jb+1); i<ist; i++) 
        {
          j  = indst[i];  // Global param indice (the matrix line)
          ik = indnz[j];  // Index of index
          
          if (ik == -1)	  // New global variable
          {
            for (k=0; k<nalc; k++) {clcmat[nagbn][k] = 0.0;} // Initialize the row

            indnz[j] = nagbn;
            indbk[nagbn] = j;
            ik = nagbn;
            nagbn++;
          }

          for (k=(ja+1); k<jb ; k++) // Now fill the rectangular matrix
          {
            ij = indst[k];						
            clcmat[ik][ij] += wght*arest[i]*arest[k];
          } 
        }
        ja = -1;
        jb = 0;
        ist--;
      } // End of "end of equation" operations
    }   // End of loop on equation
    ist++;
  } // End of loop on all equations used in the fit
	
  // Third loop is finished, now we update the correction matrices (see notes)
  
  Millepede::SpAVAt(clmat, clcmat, corrm, nalc, nagbn);
  Millepede::SpAX(clcmat, blvec, corrv, nalc, nagbn);
	
  for (i=0; i<nagbn; i++)
  {
    j = indbk[i];
    bgvec[j] -= corrv[i];
    //info()<< "---> bgvec["<<j<<"] = " << bgvec[j] << endreq;
    for (k=0; k<nagbn; k++)
    {
      ik = indbk[k];
      cgmat[j][ik] -= corrm[i][k];
      //      info() << "---> cgmat["<<j<<"]["<<ik<<"]="<<cgmat[j][ik]<<endreq;
      

    }
  }
	
  //ArraytoVector(&track_params[0],track);
  //ArraytoVector(&estimated_para[0],estimated);


  indst.clear(); // Reset store for the next track 
  arest.clear();
  return true;

}



int Centipede::SpmInvGlobal(double v[][mgl], double b[], const int n)
{

  std::cout << std::setprecision(20) << std::fixed;
  int i, j, jj, k;
  double vkk, *temp;
  double  *r, *c;
  double eps = 0.00000000000001;
  bool flag[n]; //MD 15-11
  double diag[n];//MD 15-11
  
  r = new double[n]; //r=row
  c = new double[n]; //c=column

  temp = new double[n];

  for (i=0; i<n; i++)
  {
    r[i] = 0.0;
    c[i] = 0.0;
    flag[i] = true; // we want to use all elements as possible pivot value

    for (j=0; j<=i; j++) { if (v[j][i] == 0) {v[j][i] = v[i][j];} }// MD what for ??????????



  }
  
  // Small loop for matrix equilibration (gives a better conditioning) 

  for (i=0; i<n; i++)
  {
    for (j=0; j<n; j++)
    { 
      m_glmat[i][j]=v[i][j];

      if (fabs(v[i][j]) >= r[i]) r[i] = fabs(v[i][j]); // Max elemt of row i
      if (fabs(v[j][i]) >= c[i]) c[i] = fabs(v[j][i]); // Max elemt of column i
    }
  }
  
  for (i=0; i<n; i++)
  {
    if (0.0 != r[i]) r[i] = 1./r[i]; // Max elemt of row i
    if (0.0 != c[i]) c[i] = 1./c[i]; // Max elemt of column i

    //    if (eps >= r[i]) r[i] = 0.0; // Max elemt of row i
    //    if (eps >= c[i]) c[i] = 0.0; // Max elemt of column i
  }

  for (i=0; i<n; i++) // Equilibrate the V matrix
  {
    for (j=0; j<n; j++) {v[i][j] = sqrt(r[i])*v[i][j]*sqrt(c[j]);}
  }

  int nrank = 0;

  // // save diagonal elem absolute values 	
  for (i=0; i<n; i++) {
    diag[i] = fabs(v[i][i]);
    
  } 

  for (i=0; i<n; i++) // n=nvar = ngbl+Nofconstraints
  {
    vkk = 0.0;
    k = -1;
    j=i; //look at diagonal elements
    

    //    TRACE("%%%%%%% in global inverting fct : " <<std::endl);

    for (j=0; j<n; j++) // First look for the pivot, ie max unused diagonal element 
    {
      // TRACE("search pivot ... " PRINTV(j) <<" greater than " << PRINTV(std::max(fabs(vkk),eps*diag[j]) )
      //    << "..is flag = 1 ? --> " << PRINTV(flag[j])       );
      
      if (flag[j] && ( fabs(v[j][j]) > std::max(fabs(vkk),eps*diag[j])) )
      {
        //TRACE("possible pivot " << PRINTV(i) << PRINTV(j) << PRINTV((v[j][j])) <<"flag : " << PRINTV(flag[j]) );
        vkk = v[j][j]; //pivot value
        k = j;         //index of pivot value
      }
    }
	     
    if (k >= 0)    // pivot found
    {      
      //MD 
      //TRACE( "FOUND Pivot value :" << vkk << PRINTV(k) << std::endl);
            
      nrank++;
      flag[k] = false; // Reset flag -> this value is used
      vkk = 1.0/vkk;
      v[k][k] = -vkk; // Replace pivot by its inverse
     
      for (j=0; j<n; j++)
      {
        for (jj=0; jj<n; jj++)  
        {
          if (j != k && jj != k) // Other elements (!!! do them first as you use old v[k][j]'s !!!)
          {
            v[j][jj] = v[j][jj] - vkk*v[j][k]*v[k][jj];
          }					
        }					
      }

      for (j=0; j<n; j++)
      {
        if (j != k) // Pivot row or column elements 
        {
          v[j][k] = (v[j][k])*vkk;	// Column
          v[k][j] = (v[k][j])*vkk;	// Line
        }
      }	
      ///////////////////////////////////
      /// Print out the matrix elements for debug///
      /////////////////////////////////
      //MD
      // for (int ja=0; ja < n; ja++){
      //         TRACE("Pivot Changes Made..." << PRINTV(ja)<< PRINTV(v[ja][ja]));           
      //       }
    }
    else   // No more pivot value (clear those elements)
    {
      for (j=0; j<n; j++)
      {
        if (flag[j])
        {
          b[j] = 0.0;

          for (k=0; k<n; k++)
          {
            v[j][k] = 0.0;
            v[k][j] = 0.0;
          }
        }				
      }

      break;  // No more pivots anyway, stop here
    }
  }

  for (i=0; i<n; i++) // Correct matrix V
  {
    for (j=0; j<n; j++) {v[i][j] = sqrt(c[i])*v[i][j]*sqrt(r[j]);}
  }
	
  for (j=0; j<n; j++)
  {
    temp[j] = 0.0;
    
    for (jj=0; jj<n; jj++)  // Reverse sign of matrix elements
    {
      v[j][jj] = -v[j][jj];
      temp[j] += v[j][jj]*b[jj];

      m_glmatinv[j][jj]=v[j][jj];
    }					
  }
  //final result = change also bgvec into solution
  for (j=0; j<n; j++) {b[j] = temp[j];}	// The final result				

  delete[] temp;
  delete[] r;
  delete[] c;
    
  return nrank;
}

//
// Same method but for local fit, so heavily simplified
//


int Centipede::SpmInv(double v[][mlocal], double b[], int n)
{
  std::cout << std::setprecision(10) << std::fixed;
  int i, j, jj, k;
  double vkk, *temp;
  double eps = 0.0000000000001;
  bool flag[n]; //MD 15-11
  double diag[n];//MD 15-11
  
  temp = new double[n];
  
  for (i=0; i<n; i++)
  {
    flag[i] = true;
    diag[i] = fabs(v[i][i]);     // save diagonal elem absolute values 	

    //TRACE("%%%%%%% in local inverting fct " << PRINTV(i)<<PRINTV(diag[i]) << PRINTV(v[i][i])  ) ;
    for (j=0; j<=i; j++)
    {
      v[j][i] = v[i][j] ;
    }
  }
	
  int nrank = 0;

  for (i=0; i<n; i++)
  {
    vkk = 0.0;
    k = -1;

    //TRACE("&&& in local inverting fct &&& " << PRINTV(i) << PRINTV(diag[i]) << PRINTV(v[i][i]) );
		
    for (j=0; j<n; j++) // First look for the pivot, ie max unused diagonal element 
    {
      if (flag[j] && ( (fabs(v[j][j]) > std::max(fabs(vkk),eps*diag[j])) ) ){
        
        // TRACE("possible pivot local" << PRINTV(i) << PRINTV(j) << PRINTV((v[j][j])) 
        //<<"flag : " << PRINTV(flag[j]) );
        vkk = v[j][j];
        k = j;
      }
    }
		
    if (k >= 0)    // pivot found
    {

      //TRACE( "FOUND Pivot value :" << vkk << PRINTV(k) << std::end);
      nrank++;
      flag[k] = false;
      vkk = 1.0/vkk;
      v[k][k] = -vkk; // Replace pivot by its inverse
      
      for (j=0; j<n; j++)
      {
        for (jj=0; jj<n; jj++)  
        {
          if (j != k && jj != k) // Other elements 
                                 //(!!! do them first as you use old v[k][j]'s !!!)
          {
            v[j][jj] = v[j][jj] - vkk*v[j][k]*v[k][jj];
          }					
        }					
      }

      for (j=0; j<n; j++)
      {
        if (j != k) // Pivot row or column elements 
        {
          v[j][k] = (v[j][k])*vkk; // Column
          v[k][j] = (v[k][j])*vkk; // Line
        }
      }					
    }
    else  // No more pivot value (clear those elements)
    {
      for (j=0; j<n; j++)
      {
        if (flag[j])
        {
          b[j] = 0.0;

          for (k=0; k<n; k++)
          {
            v[j][k] = 0.0;
          }
        }				
      }

      break;  // No more pivots anyway, stop here
    }
  }

  for (j=0; j<n; j++)
  {
    temp[j] = 0.0;
    
    for (jj=0; jj<n; jj++)  // Reverse matrix elements
    {
      v[j][jj] = -v[j][jj];
      //MD  new approach    temp[j] += v[j][jj]*b[jj];

      //TRACE("In SPMINV()...calculating params..."<< PRINTV(j) << PRINTV(jj) 
      //<< PRINTV(v[j][jj]) <<" / old...: " <<PRINTV(b[jj]) << " / new...: "
      //<<PRINTV(temp[j]) );     
            
      
    }					
  }
  //MD new approach
  //   for (j=0; j<n; j++)
  //   {	
  //     b[j] = temp[j];
  //   }					

  delete[] temp;
  
  return nrank;
}







/*********************************************
 *                                           *
 *   VectortoArray                           *
 * converting vectors to arrays              *
 *                                           *
 *********************************************/
void Centipede::VectortoArray(const std::vector<double> &the_vector, double the_array[] ){
  std::vector<double>::const_iterator the_iterator = the_vector.begin();
  unsigned int counter = 0;
  while( the_iterator < the_vector.end() ){
    the_array[counter] = *the_iterator;
    //    the_array[counter] = the_vector[counter];
    the_iterator++;
    counter++;
  }
}

/*****************************************
 * Convert array to vector...
 ****************************************/

void Centipede::ArraytoVector( const double the_array[], std::vector<double> &the_vector ) {
  std::vector<double>::iterator it = the_vector.begin();
  unsigned int cnt = 0;
  for ( ;it != the_vector.end(); it++, cnt++ ) {
    *it = the_array[cnt];
  }
}

// StatusCode Centipede::finalize()
// {
//   StatusCode sc =  GaudiTupleTool::finalize();
//   return sc;
// }
void Centipede::CheckLChi2(double chi2, int tsize, int nm, double cfactrb, double nsdtdevb , bool & flag ) {

  int nndf=nm-tsize;
  int nsdtdev=(int) nsdtdevb;
  //info()<<"nsdtdev=" << nsdtdev << " ndf=" << nndf << " cfactr=" << cfactrb <<endreq;
  double cutval = Millepede::chindl(nsdtdev,nndf)*cfactrb;
  
  if(cutval>(chi2/nndf)){flag=true;}else{flag=false;}
  if(!flag){
    locrej++;      
    info() <<  "---> Rejected track due to   chi2/dof = " << chi2/nndf  << " > "<< cutval << endmsg;
  }
  //  info()<<"Centipede chi2 cut passed?  " <<flag << " cutval=" << cutval << " || reduced chi2 is: " <<chi2/nndf << endreq;
}
