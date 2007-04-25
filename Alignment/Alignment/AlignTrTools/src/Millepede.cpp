// Include files 

// global
#include <iostream>
#include <iomanip>
#include <cmath>

//from GAUDI
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/MsgStream.h"

// local
#include "Millepede.h"


//-----------------------------------------------------------------------------
// Implementation file for class : Millepede
//
// 2007-02-09 : Marc 
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( Millepede );

using namespace std;

#if ! defined ( RDSTDMACROS )
#    define RDSTDMACROS 1
#    define INFO( S )     { std::cout<<__FILE__<<":"<<__LINE__<< ":INFO: "  << S << std::endl; }
#    define TRACE( S )    { std::cout<<__FILE__<<":"<<__LINE__<< ":TRACE: " << S << std::endl; }
#    define WARN( S )     { std::cout<<__FILE__<<":"<<__LINE__<< ":WARN: "  << S << std::endl; }
#    define PRINTV(C) " "<<#C<<"="<<C
#  endif

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Millepede::Millepede( const std::string& type,
                      const std::string& name,
                      const IInterface* parent )
  : GaudiTool ( type, name , parent )
  //Default cuts for the Alignment
{
  //MD  declareProperty("Iteration"  , m_iteration);
  //MD declareProperty("ModuleFixed" , m_fixed);

  declareInterface<IMillepede>(this);
}
//=============================================================================
// Destructor
//=============================================================================
Millepede::~Millepede() {}; 

//=============================================================================



StatusCode Millepede::initialize()
{
  
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure())
  {
    return Error("Failed to initialize", sc);
  }

  info() << "                                           " << endmsg;
  info() << "            * o o                   o      " << endmsg;
  info() << "              o o                   o      " << endmsg;
  info() << "   o ooooo  o o o  oo  ooo   oo   ooo  oo  " << endmsg;
  info() << "    o  o  o o o o o  o o  o o  o o  o o  o " << endmsg;
  info() << "    o  o  o o o o oooo o  o oooo o  o oooo " << endmsg;
  info() << "    o  o  o o o o o    ooo  o    o  o o    " << endmsg;
  info() << "    o  o  o o o o  oo  o     oo   ooo  oo  ++ starts" << endmsg;	   
  info() << "                                           " << endmsg;

  return StatusCode::SUCCESS;
}


/*
  ------------------------------------------------------
  INITMILLE: first initialization of Millepede
  this part is sub-detector independant
  ------------------------------------------------------




  ------------------------------------------------------
*/

StatusCode Millepede::InitMille(bool DOF[], int nglo
                          , int nloc, double startfact, int nstd 
                          , double res_cut, double res_cut_init)
//			  ,int NofPlanes)
{

  cout << "" << endl;
  cout << "----------------------------------------------------" << endl;
  cout << "" << endl;
  cout << "    Entering InitMille" << endl;
  cout << "" << endl;
  cout << "-----------------------------------------------------" << endl;
  cout << "" << endl;

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

  m_res_value.clear(); //clear res vectors for local fit iterations
  m_res_index.clear(); //clear resindex vectors for local fit iterations
  m_res_counter = 0; // starting value for counter of iteration
  m_res_trackcounter = 0; //starting valur for track counter
  m_rmeas_guess.clear();  
  
  
  //if (debug_mode) 
  cout << "Number of global parameters   : " << nagb << endl;
  //if (debug_mode)
  cout << "Number of local parameters    : " << nalc << endl;
  //if (debug_mode) 
  cout << "Number of standard deviations : " << nstdev << endl;
  
  //MDDEBUG
//  TRACE(PRINTV(NofPlanes));
  
  if (nagb>mglobl || nalc>mlocal)
  {
    if (debug_mode) cout << "Too many parameters !!!!!" << endl;
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

  //this is pushed to parsig()  
//   // Then we fix all parameters...

//   for (int j=0; j<nagb; j++)  {Millepede::ParSig(j,0.0);}

//   //  ...and we allow them to move if requested
     
//   for (int i=0; i<6; i++)//MDDEBUG hardcoded :(
//   {
//     if (verbose_mode) cout << "GetDOF(" << i << ")= " << DOF[i] << endl;
//     TRACE( "GetDOF" << PRINTV(i) << " " << PRINTV( DOF[i]));

//     if (DOF[i]) 
//     {
//       for (int j=i*nglo; j<(i+1)*nglo; j++) 
//        {
         
//          //MD enter by hand !!!!BAD!!!
//          //if(j==3) Millepede::ParSig(j,0.); //fix the 4th plane
//          //else   Millepede::ParSig(j,Sigm[i]);
//          Millepede::ParSig(j,Sigm[i]);
//        }
//       m_NofDOFs++;
//     }
//   }
  
  


  // Activate iterations (if requested)

  itert   = 0;	// By default iterations are turned off
  cfactr = 500.0;
  //if (m_iteration) 
  Millepede::InitUn(startfact);          

  arest.clear();  // Number of stored parameters when doing local fit
  //arenl.clear();  // Is it linear or not?
  indst.clear(); 

  storeind.clear();
  storeare.clear();
  //  storenl.clear();
  storeplace.clear();

  cout << "" << endl;
  cout << "----------------------------------------------------" << endl;
  cout << "" << endl;
  cout << "    InitMille has been successfully called!" << endl;
  cout << "" << endl;
  cout << "-----------------------------------------------------" << endl;
  cout << "" << endl;
	
  return true;
}


/*
  -----------------------------------------------------------
  PARGLO: initialization of global parameters
  -----------------------------------------------------------

  index    = the index of the global parameter in the 
  result array (equivalent to dparm[]).

  param    = the starting value

  -----------------------------------------------------------
*/

StatusCode Millepede::ParGlo(int index, double param)
{
  if (index<0 || index>=nagb)
  {return false;}
  else
  {pparm[index] = param;}

  return true;
}


/*
  -----------------------------------------------------------
  PARSIG: define a constraint for a single global param
  param is 'encouraged' to vary within [-sigma;sigma] 
  range
  -----------------------------------------------------------

  index    = the index of the global parameter in the 
  result array (equivalent to dparm[]).

  sigma	   = value of the constraint (sigma <= 0. will 
  mean that parameter is FIXED !!!) 
 
  -----------------------------------------------------------
*/
 
StatusCode Millepede::ParSig(int index, double sigma)
{

//   for (int i=0; i<6; i++)//MDDEBUG hardcoded :(
//   {
//     if (verbose_mode) cout << "GetDOF(" << i << ")= " << DOF[i] << endl;
//     TRACE( "GetDOF" << PRINTV(i) << " " << PRINTV( DOF[i]));

//     if (DOF[i]) 
//     {
//       for (int j=i*nglo; j<(i+1)*nglo; j++) 
//        {
         
//          //MD enter by hand !!!!BAD!!!
//          //if(j==3) Millepede::ParSig(j,0.); //fix the 4th plane
//          //else   Millepede::ParSig(j,Sigm[i]);
//          Millepede::ParSig(j,Sigm[i]);
//        }
//       m_NofDOFs++;
//     }
//   }
  
//   //MDDEBUG  
//   TRACE(PRINTV(m_NofDOFs));



  if (index>=nagb) {return false;}

  if (sigma == 0) {
    psigm[index] = 0.;// fix the parameter
  } else {
    psigm[index] = sigma;
  } // assign sigma to chosen parameter 

  if(DEBUGPARSIG){TRACE(endl << "+++++++++ PARSIG() called : Index " << 
                        PRINTV(index) << " value " << PRINTV(psigm[index]) << endl );
  }
  
  
  
  return true;
}


/*
  -----------------------------------------------------------
  INITUN: unit for iteration
  -----------------------------------------------------------
  
  cutfac is used by Fitloc to define the Chi^2/ndof cut value

  A large cutfac value enables to take a wider range of tracks 
  for first iterations, which might be useful if misalignments
  are large.

  As soon as cutfac differs from 0 iteration are requested.
  cutfac is then reduced, from one iteration to the other,
  and iterations are stopped when it reaches the value 1.

  At least one more iteration is often needed in order to remove
  tracks containing outliers.
  
  -----------------------------------------------------------
*/

bool Millepede::InitUn(double cutfac)
{
  cfactr = std::max(1.0, cutfac);
  itert = 1; // Initializes the iteration process
  TRACE("INITUN called...Initial cut factor is  " << cfactr 
        <<"and itert set to "<< PRINTV(itert) << endl);
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

StatusCode Millepede::ConstF(double dercs[], double rhs)
{  
  if (ncs>=mcs) // mcs is defined in Millepede.h
  {
    cout << "Too many constraints !!!" << endl;
    return false;
  }
 	//MD why loop over all nagb ??
  for (int i=0; i<nagb; i++) {adercs[ncs][i] = dercs[i];} 
 	
  arhs[ncs] = rhs;
  ncs++ ;
  TRACE("Number of constraints increased to " << PRINTV(ncs) << endl
       << "----------------------------------------------- " << endl) ;
  
  // for (int i=0; i<ncs; i++) {
//     for (int ii=0; ii<nagb; ii++) {
//       INFO("i = ncs ; ii = nagb " << PRINTV(i) << PRINTV(ii)
//            << PRINTV(adercs[i][ii]) << PRINTV(arhs[i]) );
//     } 
//   }
    

  return true;
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

//bool Millepede::EquLoc(double dergb[], double derlc[], double rmeas, double sigma, double dernl[])
StatusCode Millepede::EquLoc(double dergb[], double derlc[], double rmeas, double sigma)
{	

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

  
  //  TRACE("EQULOC..." << PRINTV(nagb) << PRINTV(nalc) );
  
  
  // Serious equation, initialize parameters
  	
  double wght =  1.0/(sigma*sigma);
  int nonzer  =  0;
  int ialc    = -1;
  int iblc    = -1;
  int iagb    = -1;
  int ibgb    = -1;

  for (int i=0; i<nalc; i++) // Retrieve local param interesting indices
  {
    if (derlc[i]!=0.0)
    {
      nonzer++;
      if (ialc == -1) ialc=i;	// first index
      iblc = i;       	     	// last index
    }
  }
	
  if (verbose_mode)  cout << ialc << " / " << iblc << endl;
  //  TRACE(PRINTV(wght) << PRINTV(sigma) );
  

	
  for (int i=0; i<nagb; i++)  // Idem for global parameters
  {
    if (dergb[i]!=0.0)
    {
      nonzer++;
      if (iagb == -1) iagb=i;	// first index
      ibgb = i; 	     	// last index
    }
  }

  if (verbose_mode) cout <<" global index ranges " << iagb << " / " << ibgb << endl;


  indst.push_back(-1);
  arest.push_back(rmeas);
  //  arenl.push_back(0.);

  //TRACE(PRINTV(arest.back() ) << PRINTV(arest.size()) );
  
  for (int i=ialc; i<=iblc; i++) 
  {
    if (derlc[i]!=0.0)
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
    if (dergb[i]!=0.0)
    {
      indst.push_back(i);
      arest.push_back(dergb[i]);
      //      arenl.push_back(dernl[i]);
      dergb[i]   = 0.0;  // reference: also affects MilleTool::derGB[]!!
    }
  }	

    
  
  if (verbose_mode) cout << "Out Equloc --  NST = " << arest.size() << endl;

  //TRACE( PRINTV( "Out Equloc --  NST = " << arest.size()) << " indexsize" << PRINTV(indst.size()));
  
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
 
StatusCode Millepede::ZerLoc(double dergb[], double derlc[])
{
  for(int i=0; i<nalc; i++) {derlc[i] = 0.0;}
  for(int i=0; i<nagb; i++) {dergb[i] = 0.0;}
  // for(int i=0; i<nagb; i++) {dernl[i] = 0.0;}

  return true;
}

/*
  -----------------------------------------------------------
  FITLOC:  perform local params fit, once all the equations
  have been written by EquLoc
  -----------------------------------------------------------

  n            = number of the fit, it is used to store 
  fit parameters and then retrieve them 
  for iterations (via STOREIND and STOREARE)

  track_params = contains the fitted track parameters and
  related errors

  single_fit   = is an option, if it is set to 1, we don't 
  perform the last loop. It is used to update 
  the track parameters without modifying global
  matrices

  -----------------------------------------------------------
*/

StatusCode Millepede::FitLoc(int n, double track_params[], int single_fit, 
			     //			     double estimated_para[] )
			     double estimated_para[], double &summ, double &res )
{
  // Few initializations
	
  int i, j, k, ik, ij, ist, nderlc, ndergl, ndf;
  int ja      = -1;
  int jb      = 0;
  int nagbn   = 0;
  double rmeas, wght, rms, cutval;
  summ  = 0.0;
  double estimation_sum = 0.;
  int    nsum  = 0;
  nst   = 0; 
  nst   = arest.size(); // number of stored derivatives(loc&glob) & wght(for each glob entry)
  //MDDEBUG  
  //    for ( unsigned int mi=0 ; mi < arest.size()  ;mi++  ) {
  //      TRACE("check..." << PRINTV(mi) << PRINTV(indst[mi]) <<  PRINTV(arest[mi])); 
  //    }

  for (int ii=0; ii<nagb; ii++)  // fill estimated parameters
  {
    pparm[ii] = estimated_para[ii];
    //    debug() << "Estimated params: " << estimated_para[ii] << endreq;
    //if(pparm[ii] != 0 && n==1) INFO(PRINTV(ii) << PRINTV(pparm[ii]));
    
  }

  cout << std::setprecision(10) << std::fixed;
  // Fill the track store at first pass
  
  if (itert < 2 && single_fit != 1)  // Do it only once 
  {
    //TRACE("Store equation no: " << PRINTV(n) ); 
    
    for (i=0; i<nst; i++)    // Store the track parameters
    {
      storeind.push_back(indst[i]);
      storeare.push_back(arest[i]);
      //storenl.push_back(arenl[i]);

      // if (arenl[i] != 0.) arest[i] = 0.0; // Reset global derivatives if non linear and first iteration
    }

    // arenl.clear();

    storeplace.push_back(storeind.size());

    //if (verbose_mode)     cout << "StorePlace size = " << storeplace[n] << endl; 
    //if (verbose_mode)     cout << "StoreInd size   = " << storeind.size() << endl; 
  }	


  for (i=0; i<nalc; i++) // reset local params
  {
    blvec[i] = 0.0;

    for (j=0; j<nalc; j++) {clmat[i][j] = 0.0;}
  }
	
  for (i=0; i<nagb; i++) {indnz[i] = -1;} // reset mixed params


  /*

  LOOPS : HOW DOES IT WORKS ?	

  Now start by reading the informations stored with EquLoc.
  Those informations are in vector INDST and AREST.
  Each -1 in INDST delimits the equation parameters:
  
  First -1  ---> rmeas in AREST 
  Then we have indices of local eq in INDST, and derivatives in AREST
  Second -1 ---> weight in AREST
  Then follows indices and derivatives of global eq.
  ....
  
  We took them and store them into matrices.
  
  As we want ONLY local params, we substract the part of the estimated value
  due to global params. Indeed we could have already an idea of these params,
  with previous alignment constants for example (set with PARGLO). Also if there
  are more than one iteration (FITLOC could be called by FITGLO)

  */

    
  //************************************************
  // FIRST LOOP : local track fit
  //************************************************
	
  ist = 0;
  indst.push_back(-1);
  
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
        
        //TRACE( "rmeas = " << PRINTV(rmeas) << "ja : " << PRINTV(ja));
        if (verbose_mode) cout << "wght = " << wght << "jb  : " << jb << endl ;
        
        for (i=(jb+1); i<ist; i++)   // Now suppress the global part   
          // (only relevant with iterations)
        {
          j = indst[i];              // Global param indice
          
          // TRACE("misalign iteration before : " <<PRINTV(j) <<PRINTV(dparm[j]) );
          //TRACE("Starting misalignment = " << PRINTV( pparm[j]) );
          //MDDEBUG dparm is changed by iterations
          //           if( pparm[j] !=0. && n == 1){
          //             TRACE( " FIRST LOOP FitLoc() " << PRINTV(i) <<  PRINTV(arest[i]) 
          //                    <<  PRINTV(j) << PRINTV(dparm[j]) << PRINTV(pparm[j]) );
          //           }
          
//	  info() << "1: rmeas = " << rmeas << endreq;

          rmeas -= arest[i]*(pparm[j]+dparm[j]);

//	  info() << "1: Weight = " << wght << endreq;
//	  info() << "2: rmeas = " << rmeas << endreq;
//	  info() << "arest[i] = " << arest[i] << endreq;
//	  info() << "pparm[j] = " << pparm[j] << endreq;
//	  info() << "dparm[j] = " << dparm[j] << endreq;
          //TRACE("Resulting ...." << PRINTV( rmeas) <<" with " <<PRINTV(i) 
          //      << PRINTV(arest[i]) << " and pparm" << PRINTV(j)<<PRINTV(pparm[j])<< endl);
          
        }
        m_rmeas_guess.push_back(rmeas); //MD 3/1/07
        //if (verbose_mode) 
        //cout << "rmeas after global stuff removal = " << rmeas 
        //     << "  " << m_rmeas_guess.back() << endl ;

        for (i=(ja+1); i<jb; i++)    // Finally fill local matrix and vector
        {
          j = indst[i];   // Local param indice (the matrix line) 
          blvec[j] += wght*rmeas*arest[i];  // See note for precisions

          if (verbose_mode) cout << "blvec[" << j << "] = " << blvec[j] 
                                 <<" mit arest [" << i << "]" << arest[i] <<  endl ;
          for (k=(ja+1); k<=i ; k++) // Symmetric matrix, don't bother k>j coeffs
          {
            ik = indst[k];						
            clmat[j][ik] += wght*arest[i]*arest[k];
            //cout << "clmat[" << j << "][" << ik << "] = " << clmat[j][ik] << endl;
          } 
        }  
        ja = -1;
        jb = 0;
        ist--;
      } // End of "end of equation" operations //end of last else statement
    } // End of loop on equation "first if"
    ist++;
  } // End of loop on all equations used in the fit "while"


  //*********************************************************
  // Local params matrix is completed, now invert to solve...
  //***********************************************************
	
  int nrank = 0;  //MD Rank is the number of nonzero diagonal elements 
  //  nrank = Millepede::SpmInv(clmat, &blvec[0], nalc, scdiag, scflag); MD 15-11
  nrank = Millepede::SpmInv(clmat, &blvec[0], nalc);
  
  
  if (debug_mode) cout << "" << endl;
  if (debug_mode) cout << " __________________________________________________" << endl;
  //MDDEBUG
  //INFO( " Printout of local fit  (FITLOC)  with rank= " << PRINTV(nrank) << endl
  //        << " Result of local fit :      (index/parameter/error)" << endl);
  
    
  for (i=0; i<nalc; i++)
  {
    if(DEBUGLOCFIT){
      cout << std::setw(5) << i << "   /   " << std::setw(5) 
           << blvec[i] << "   /   " << sqrt(clmat[i][i]) << endl;
      
//         for(int count=0; count<nalc; count++){
//           cout << "offdiag  :" << i <<" " << count << "  " << sqrt(fabs(clmat[i][count]))  
//                << " sign of correlation: "<< clmat[i][count]  
// 	       << " corr. coefficient : " <<  clmat[i][count] / (sqrt(clmat[i][i])* sqrt(clmat[i+1][i+1]) ) << endl;
//         }
    }
  }
  

  // Store the track params and errors

  for (i=0; i<nalc; i++)
  {
    //MD fill m_blvec[]
    m_blvec[i] = blvec[i];
    track_params[2*i] = blvec[i]; // reference !
    track_params[2*i+1] = sqrt(fabs(clmat[i][i])); // reference !!

    //TRACE("trackparams in fitloc..." << PRINTV(i) << PRINTV(track_params[2*i]) 
    //<< PRINTV( track_params[2*i+1]) );
    
    m_storeslopes.push_back(blvec[i]);
    m_storeslopes.push_back( sqrt(fabs(clmat[i][i]))   );
    
  }

    
  //
  // SECOND LOOP : residual calculation
  //
  
  ist = 0;
  ja = -1;
  jb = 0;

  while (ist <= nst)
  {
    if (indst[ist] == -1)
    {

      //MD TRACE("___Inside the if statement with value___ " << PRINTV(ist) );

      if (ja == -1)     {ja = ist;}  // First  0 : rmeas
      else if (jb == 0) {jb = ist;}  // Second 0 : weight 
      else                           // Third  0 : end of equation  
      {	


        //MD TRACE("___Inside the third else statement with value___ " << PRINTV(ist)
        // << PRINTV(ja) << PRINTV(jb) );

        rmeas	= arest[ja];

        //TRACE(PRINTV(rmeas) << PRINTV(ja) << PRINTV(arest[ja]) );
        
        wght 	= arest[jb];
        
        nderlc = jb-ja-1;    // Number of local derivatives involved
        ndergl = ist-jb-1;   // Number of global derivatives involved
        
        // Print all (for debugging purposes)
        
        if (verbose_mode) cout << "" << endl;
        if (verbose_mode) cout << std::setprecision(4) << std::fixed;
        if (verbose_mode) cout << ". equation:  measured value " << std::setw(13) 
                               << rmeas << " +/- " << std::setw(13) << 1.0/sqrt(wght) << endl;
        if (verbose_mode) cout << "Number of derivatives (global, local): " 
                               << ndergl << ", " << nderlc << endl;
        if (verbose_mode) cout << "Global derivatives are: (index/derivative/parvalue) " << endl;
	
        for (i=(jb+1); i<ist; i++)
        {if (verbose_mode) cout << indst[i] << " / " << arest[i] 
                                << " / " << pparm[indst[i]] << endl;} 

        if (verbose_mode) cout << "Local derivatives are: (index/derivative) " << endl;
	
        for (i=(ja+1); i<jb; i++) {if (verbose_mode) cout << indst[i] << " / " << arest[i] << endl;}	  

        // Now suppress local and global parts to RMEAS;

        for (i=(ja+1); i<jb; i++) // First the local part 
        {
          j = indst[i];
          rmeas -= arest[i]*blvec[j];
        }
        
        for (i=(jb+1); i<ist; i++) // Then the global part
        {
          j = indst[i];
          rmeas -= arest[i]*(pparm[j]+dparm[j]);
        }


        ////////////////////////////////////////////
        ///     fill res vectors for Graphs     ////
        ///      plotted in myAlg               ////
        ///    default init values :            ////
        /// m_res_counter = 0                   ///
        /// itert = 0 or 1 (for iterations)     ///
        ///////////////////////////////////////////
        // count the iterations
        if(m_res_counter < itert){
          m_res_index.push_back(-2);  // iteration begin
          m_res_value.push_back(itert);
          m_res_counter++;
          m_res_trackcounter = 0; // set to 0 after each iteration
        }
        //MD TRACE("____track counter___" << PRINTV(n) << PRINTV(m_res_trackcounter) );
       // count the tracks
        if(m_res_trackcounter == n){
          m_res_index.push_back(-1);    //track begin
          m_res_value.push_back(n); 
          m_res_trackcounter++;
	}
	//fill the arrays with data
        m_res_index.push_back(n);
        m_res_value.push_back(rmeas);
        
        // reject the track if rmeas is too important (outlier)
        if (fabs(rmeas) >= m_residual_cut_init && itert <= 1)  
        {
          if(single_fit == 0){
            locrej++;      
            loctot++; //although rej. it was fittet before !
            INFO(PRINTV(loctot));
          }
          TRACE("Rejected track !!!!! using starting cut factor" << PRINTV(locrej)<<endl);
          indst.clear(); // reset stores and go to the next track 
          arest.clear();	  
          return false;
        }
        
//         if (fabs(rmeas) >= m_residual_cut && itert > 1)   
//         {
//           TRACE( "             !!!!!!!!      Rejected track         !!!!!" << endl
//                  <<"since" << PRINTV(rmeas) <<" >= " << PRINTV(m_residual_cut)  );
//           locrej++;      
//           indst.clear(); // reset stores and go to the next track 
//           arest.clear();	  
//           return false;
//         }
	
//	INFO("              " << PRINTV(rmeas));

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

  ndf = nsum-nrank; //nsum = number of measurements, nrank=nof local param.	
  rms = 0.0;

  if (debug_mode) cout << "Final chi square / degrees of freedom "<< summ << " / " << ndf << endl;

   if(DEBUGLOCFIT){
    WARN( "Final chi square / degrees of freedom "<< PRINTV(summ) << PRINTV(ndf)
          << " " << PRINTV(nrank) << PRINTV(estimation_sum));
  }
  
  if (ndf > 0) rms = summ/float(ndf);  // Chi^2/dof
	
  if (single_fit == 0) {
    if(DEBUGLOCFIT){
      if(n==0) cout << endl << "FitLoc called with argument '0' -> global der. update as well!!" << endl ;
    }
    loctot++;
    //    INFO(PRINTV(loctot));
    
  }
  

  if (nstdev != 0 && ndf > 0 && single_fit != 1) // Chisquare cut
  {
    cutval = Millepede::chindl(nstdev, ndf)*cfactr;
    if (rms > cutval) // Reject the track if too much...
    {
      
      locrej++;      
      //       TRACE( "Rejected track !!!!! --> " << PRINTV(rms) << " > "<<PRINTV(cutval) << endl
      //              << PRINTV(locrej) << PRINTV(cfactr) << endl);
      
      indst.clear(); // reset stores and go to the next track 
      arest.clear();
      return false;
    }
  }

  if (single_fit == 1) //Stop here if just updating the track parameters
  {
    
    if(n==0) cout << endl << "FitLoc called with argument '1' -> no global der. update!!" << endl ;
    //locrej = 0; //just count rej. when global fit afterwards
    
    indst.clear(); // Reset store for the next track 
    arest.clear();
    return true;
  }


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
    
          //   TRACE("vector " << PRINTV(i)<<PRINTV(indst[i]) << PRINTV(arest[i]) << " mit indst[i] = j ");
          
          bgvec[j] += wght*rmeas*arest[i];  // See note for precisions
          
          for (k=(jb+1); k<ist ; k++)
          {
            ik = indst[k];						
            cgmat[j][ik] += wght*arest[i]*arest[k];
            if(DEBUGcgmat){
              if(j ==ik){
                INFO( PRINTV(j) << PRINTV(ik) << PRINTV(cgmat[j][ik]) << PRINTV(arest[i])
                      << PRINTV(arest[k])<< PRINTV(wght) );
              }
            }
            
            
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


//            TRACE("clcmat " << PRINTV(ik) << PRINTV(ij) << endl 
//                  <<PRINTV(i) << PRINTV(indst[i]) << PRINTV(arest[i]) << endl
//                  << PRINTV(k) << PRINTV(indst[k]) << PRINTV(arest[k]));      


            clcmat[ik][ij] += wght*arest[i]*arest[k];
//if (verbose_mode) cout << "clcmat[" << ik << "][" << ij << "] = " << clcmat[ik][ij] << endl;
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
		
    for (k=0; k<nagbn; k++)
    {
      ik = indbk[k];
      cgmat[j][ik] -= corrm[i][k];

      //TRACE( "LOOP 3/3...cgmat corrected" <<PRINTV( j) << PRINTV(k) 
      //     << " ---> " PRINTV( cgmat[i][k] ) );

    }
  }

  //MD  TRACE(PRINTV(indst.at(0)) << PRINTV(indst.size()) );
  //   int Eintraege=0;
  //   Eintraege = ( (sizeof(bgvec))/(sizeof(bgvec[1])) );
  //   TRACE("in bgvec " << PRINTV(Eintraege));    

	
  indst.clear(); // Reset store for the next track 
  arest.clear();
  res = summ/double(ndf);
//  info() << "Wght = "<< wght << endreq;
 // info() << "rmeas = " << rmeas/ndf << endreq;
  //info() << "summ = " << summ << endreq;
  return true;
}
 

/*
  -----------------------------------------------------------
  MAKEGLOBALFIT:  perform global params fit, once all the 'tracks'
  have been fitted by FitLoc
  -----------------------------------------------------------

  par[]        = array containing the computed global 
  parameters (the misalignment constants)

  error[]      = array containing the error on global 
  parameters (estimated by Millepede)

  pull[]        = array containing the corresponding pulls 

  -----------------------------------------------------------
*/

StatusCode Millepede::MakeGlobalFit(double par[], double error[], double pull[])
{

 
  int i, j,nvar;
  int nfixed; //no of fixed parameters
  int itelim = 0;

  //int nstillgood;

  double sum;

  double step[150];

  //  double trackpars[2*mlocal];

  int ntotal_start, ntotal;

  //   Double_t Xiter[m_NofPlanes]; // arrays to fill in tree
  //   Double_t Yiter[m_NofPlanes];
  //   Double_t Ziter[m_NofPlanes];

  ntotal = Millepede::GetTrackNumber();  
  if(DEBUGGLOBAL){
    cout <<"*******************************************************"<<endl
         <<"**** preparing global fit                     *********"<<endl
         <<"****                                          *********"<<endl
         <<"****                                          *********"<<endl
         <<"****...using " << ntotal << " tracks          *********"<<endl
         <<"*******************************************************"<<endl;
  
 
  ntotal_start = Millepede::GetTrackNumber();
//  TRACE("________" <<PRINTV(ntotal_start) <<PRINTV(RunNumber) << PRINTV(m_NofPlanes));
  }
  

  if (itert <= 1) itelim = 1;    // Max number of iterations

  //--------------  MD vectors for iteration  ----------------------

  //   for ( int iteration = 0 ; iteration < itelim  ; iteration++ ) {
  //     std::vector<double> IterationNo_vec;
  //     m_iteration_vector.push_back(IterationNo_vec);
  //     m_iterationError_vector.push_back(IterationNo_vec);
  //   }
  //MDDEBUg
  //TRACE(PRINTV(m_iteration_vector.size()));
  //ShowIterationVector();
  
  //std::vector<double> track_slopes; // slope-vector for iterations
  
  //  while (itert < itelim)  // Iteration for the final loop
  //{
  //  track_slopes.resize(m_storeslopes.size(),0.);
  // track_slopes = m_storeslopes; //  vector aus fitloc an loc variable
  // m_storeslopes.clear(); // loesche, weil bei naechsten FitLoc() neu;
  
  // Start by saving the diagonal elements for
    // gcor calculation at end
    for (i=0; i<nagb; i++) {
      m_diag[i] = cgmat[i][i];
      
      if(DEBUGGLOBAL) TRACE("***** fitglo m_diag : "  << PRINTV(i) << PRINTV(m_diag[i]));
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
          
          if(DEBUGGLOBAL){
            TRACE(endl<<"+++ for Param. " << PRINTV(i) << "row &col. of cgmat fixed : " << endl
                  << PRINTV(i) << PRINTV(j) << PRINTV(cgmat[i][j]) << PRINTV(cgmat[j][i] ) << endl);
          }
          
        }			
      }
      else if (psigm[i] > 0.0) // constrain movement of plane
      {
        cgmat[i][i] += 1.0/(psigm[i]*psigm[i]);

        if(DEBUGGLOBAL) TRACE("+++ for Param. " << PRINTV(i) << "cgmat[i][i] changed" );
      }
    }

    nvar = nagb;  // Current number of equations	
    
    if (debug_mode) cout << "Number of constraint equations : " << ncs << endl;

  
  if (ncs > 0) // Then the constraint equation
    {
      for (i=0; i<ncs; i++)
      {
        sum = arhs[i];
        for (j=0; j<nagb; j++)
        {
          cgmat[nvar][j] = float(ntotal)*adercs[i][j];
          cgmat[j][nvar] = float(ntotal)*adercs[i][j];          
          
          //TRACE(PRINTV(i) << PRINTV(j) << PRINTV(adercs[i][j]) << endl);
          
          //MD 30/10 set all in above resetted rows & cols to zero
          if(psigm[j] == 0.){
            cgmat[nvar][j] = 0.0;
            cgmat[j][nvar] = 0.0;
          }
          
          sum -= adercs[i][j]*(pparm[j]+dparm[j]);



          //TRACE( PRINTV(nvar) << PRINTV(j) << PRINTV(cgmat[nvar][j])
          //     <<PRINTV(cgmat[j][nvar]) << PRINTV(adercs[i][j]) );
          //TRACE(PRINTV(float(ntotal)) << PRINTV(sum) );
          
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
            
            //MDDEBUG  TRACE(" final global chi2" << PRINTV(psigm[i]));
            
            
            final_cor += step[j]*cgmat[j][i]*step[i]; 

//MDDEBUG TRACE(PRINTV(final_cor) << PRINTV(j) << PRINTV(step[j]) << PRINTV(cgmat[j][i]));
            
            if (i == j) final_cor -= step[i]*step[i]/(psigm[i]*psigm[i]);

            //MDDEBUG TRACE(PRINTV(final_cor));
          }
        }
      }
    }
    if(DEBUGGLOBAL){
      cout << " Final coeff is " << final_cor << endl;		
      cout << " Final NDOFs =  " << nagb << endl;
    }
    

    //---------------------------------------------------------------
    //-------  The final matrix inversion --
    //--
    //-- cgmat and bgvec already corrected !(loop 3\3 fitloc() )
    //---------------------------------------------------------------
    int nrank = 0;
    
    
    //nrank = Millepede::SpmInvGlobal( cgmat, &bgvec[0], nvar, scdiag, scflag); //MD 15-11
    nrank = Millepede::SpmInvGlobal( cgmat, &bgvec[0], nvar);
    if(DEBUGGLOBAL){
      cout <<"*******************************************************"<<endl
           <<"**** GLOBAL MATRIX INVERSION DONE  ********************"<<endl
           <<"*******************************************************"<<endl;
    }
    
    for (i = 0; i < nagb; ++i){
      /* Update global parameters values (for iterations)*/
      dparm[i] += bgvec[i];
      
      if(DEBUGGLOBAL){ TRACE("Update global parameters values (for iterations)" << PRINTV(i)
                             <<PRINTV(dparm[i]) );
      }
      
      
      
      //-------------------------------------------------------------
      //MD global params and errors after each iteration
      //-------------------------------------------------------------
      // m_iteration_vector[itert].push_back(dparm[i]);
      //m_iterationError_vector[itert].push_back(sqrt(fabs(cgmat[i][i])));

      //***************************************************************
      //*** MD 19/09 new approach of iteration
      //*** number of DOF is hardcoded !!
      //***************************************************************

    }
    
    TRACE("INFORMATION >>>> " << PRINTV(nvar) <<PRINTV(nfixed)<<PRINTV(nrank) << endl);
    cout << "The rank defect of the symmetric " << nvar << " by " << nvar 
         << " matrix is " << nvar-nfixed-nrank << " (bad if non 0)" << endl;
    cout << "Total : " << loctot << " local fits, " 
         << locrej << " rejected." << endl;
    
    
    //if (itert == 0)  break;       
    //    itert++;
		
    
    // Reinitialize parameters for iteration
    loctot = 0;
    locrej = 0;
    
    if (cfactr != cfactref && sqrt(cfactr) > 1.2*cfactref)
    {
      if(DEBUGGLOBAL) TRACE(" the cutfactor was in this iteration step..." <<PRINTV(cfactr));
      cfactr = sqrt(cfactr);
      if(DEBUGGLOBAL) TRACE(" the cutfactor in the next iteration step will be..." <<PRINTV(cfactr));
    }
    else
    {
      if(DEBUGGLOBAL) TRACE(" the cutfactor was in this iteration step..." <<PRINTV(cfactr));
      cfactr = cfactref;
      if(DEBUGGLOBAL) TRACE(" the cutfactor in the next iteration step will be..." <<PRINTV(cfactr));     
      //      itert = itelim;
    }

    //if (itert == itelim)  break;  // End of story         

    //cout << endl <<  "Next Iteration " << itert << " with cut factor " << cfactr << endl << endl;
    
    // Reset global variables
    
//     for (i=0; i<nvar; i++)
//     {
//       bgvec[i] = 0.0;
//       for (j=0; j<nvar; j++)
//       {
//         cgmat[i][j] = 0.0;
//       }
//     }

 /////////////////////////////////////////////////////////////////////////////////////////////////    

    /////////////////////////////////////////////////////////
    /////       We start a new iteration    ////////////////
    ////////////////////////////////////////////////////////

    // First we read the stores for retrieving the local params

    //    nstillgood = 0;	

//     for (i=0; i<ntotal_start; i++)
//     {
//       int rank_i = 0;
//       int rank_f = 0;

//       (i>0) ? rank_i = abs(storeplace[i-1]) : rank_i = 0; //MD necessary for first track
//       rank_f = storeplace[i];

//       //TRACE( "Track " <<PRINTV(i)<< " starts at " << PRINTV(rank_i)<< " ends at " << PRINTV(rank_f));

//       if (rank_f >= 0) // Fit is still OK
//       {
//         //MDDEBUG
//          //        if (itert > 3)
//         //                 {
        
//         //                   indst.clear();
//         //                   arest.clear();
        
//         //                   for (j=rank_i; j<rank_f; j++)
//         //                   {
//         //                     indst.push_back(storeind[j]);
        
//         //                     if (storenl[j] == 0) arest.push_back(storeare[j]);
//         //                     if (storenl[j] == 1) arest.push_back(track_slopes[2*i]*storeare[j]);
//         //                     if (storenl[j] == 2) arest.push_back(track_slopes[2*i+1]*storeare[j]);
//         //                   }
        
//         //                   for (j=0; j<2*nalc; j++) {trackpars[j] = 0.;}	
        
//         //                   Millepede::FitLoc(i,trackpars,1);
        
//         //                   track_slopes[2*i] = trackpars[2];
//         //                   track_slopes[2*i+1] = trackpars[6];
        
//         //                 }

        
//         indst.clear(); //MD already deleted in fitloc()...
//         arest.clear();
//         //cout <<"****************************************" <<endl
//         //      <<"***  get information from stores *****" <<endl
//         //      <<"**************************************"<<endl;
        

//         for (j=rank_i; j<rank_f; j++)
//         {
//           indst.push_back(storeind[j]);
//           //arest.push_back(storeare[j]);
          
//           if (storenl[j] == 0){
          
//             arest.push_back(storeare[j]); //z position 
            
//             //   TRACE("storenl[j] == 0 at " << PRINTV(j) << PRINTV(storeare[j]) );
            
//           }
//           if (storenl[j] == 1){
          
//             arest.push_back(track_slopes[ (2+i*4 )]); //derNL[iStation+geom.]
//             // arest.push_back(track_slopes[ (2+i*4 )]*(-1)*storeare[j-3]); //derNL[iStation+geom.]
//             //TRACE("storenl[j] == 1 at " << PRINTV(j) << PRINTV(track_slopes[2+i*4]));  
          
//           }
          
//           // if (storenl[j] == 2) arest.push_back(track_slopes[2*i+1]*storeare[j]);

//           //           TRACE( "__ITERATION II eintrag__" << PRINTV(j) << PRINTV( indst[j] ) << PRINTV( arest[j] )
//           //                  << PRINTV(m_storeslopes[i+1]) <<endl );  

//         }
//         for (j=0; j<2*nalc; j++) {trackpars[j] = 0.;}	 // reset track_params
//         track_slopes.clear();
        
        
//         //         for ( unsigned int qq =0 ; qq<indst.size() ;qq++ ) {
        
//TRACE( " kurz vor FITLOC eintrag ITERATIONEN..." << PRINTV(qq) << PRINTV( indst[qq] ) << PRINTV( arest[qq] )
//          << PRINTV( arenl[qq]));  
        
//         //         }
//         // cout << "@@@@@ fitloc in iteration procedure @@@@@@@" << endl;
//         bool sc = Millepede::FitLoc(i,&trackpars[0],0);
        
//         (sc) ? nstillgood++: storeplace[i] = -rank_f;     
//         //cout<< "$$$$$$$$$$$$$$$ GOOD FIT -> NEXT ONE $$$$$$$$$$$$$$" << endl;
        

//       }//if statement n_rank..


//     } // End of loop on fits
    
//     Millepede::SetTrackNumber(nstillgood);
    
    //  } // End of iteration loop


  //MDDEBUG
	
  // ShowIterationVector();
    //////////////////////////////////////////////////////////////////////////////////////////////////////////

  Millepede::PrtGlo(); // Print the final results

  for (j=0; j<nagb; j++)
  {
    par[j]   = dparm[j];
    dparm[j] = 0.;
    //    pull[j]  = par[j]/sqrt(psigm[j]*psigm[j]-cgmat[j][j]);
    
    //MDDEBUG       TRACE(PRINTV(psigm[j]));
    error[j] = sqrt(fabs(cgmat[j][j]));
  }

  cout << std::setw(10) << " " << endl;
  cout << std::setw(10) << "            * o o                   o      " << endl;
  cout << std::setw(10) << "              o o                   o      " << endl;
  cout << std::setw(10) << "   o ooooo  o o o  oo  ooo   oo   ooo  oo  " << endl;
  cout << std::setw(10) << "    o  o  o o o o o  o o  o o  o o  o o  o " << endl;
  cout << std::setw(10) << "    o  o  o o o o oooo o  o oooo o  o oooo " << endl;
  cout << std::setw(10) << "    o  o  o o o o o    ooo  o    o  o o    " << endl;
  cout << std::setw(10) << "    o  o  o o o o  oo  o     oo   ooo  oo ++ ends." << endl;
  cout << std::setw(10) << "                       o                   " << endl;	  


  //////////////////////////////////
  //   Reset global variables    //
  ////////////////////////////////
  
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

  storeare.clear();
  storeind.clear();
  storenl.clear();
  
  
  
  return true;
}


/*
  -----------------------------------------------------------
  SPMINV:  obtain solution of a system of linear equations with symmetric matrix 
  and the inverse (using 'singular-value friendly' GAUSS pivot)
  -----------------------------------------------------------

	Solve the equation :  V * X = B
	
	V is replaced by inverse matrix and B by X, the solution vector
  -----------------------------------------------------------
*/

//int Millepede::SpmInvGlobal(double v[][mgl], double b[], const int n, double diag[], bool flag[]) 
//MD 15-11
int Millepede::SpmInvGlobal(double v[][mgl], double b[], const int n)
{

  cout << std::setprecision(20) << std::fixed;
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

    // TRACE("%%%%%%% BEFORE equilibration in global inverting fct " << PRINTV(i)<<PRINTV(diag[i]) << PRINTV(v[i][i])  ) ;

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
    
    //TRACE("%%%%%%% after equilibration in global inverting fct " 
    //     << PRINTV(i)<<PRINTV(diag[i]) << PRINTV(v[i][i])  ) ;
  } 

  for (i=0; i<n; i++) // n=nvar = ngbl+Nofconstraints
  {
    vkk = 0.0;
    k = -1;
    j=i; //look at diagonal elements
    

    //    TRACE("%%%%%%% in global inverting fct : " <<endl);

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
      //TRACE( "FOUND Pivot value :" << vkk << PRINTV(k) << endl);
            
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


//int Millepede::SpmInv(double v[][mlocal], double b[], int n, double diag[], bool flag[]) //MD 15-11
int Millepede::SpmInv(double v[][mlocal], double b[], int n)
{
  cout << std::setprecision(10) << std::fixed;
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

      //TRACE( "FOUND Pivot value :" << vkk << PRINTV(k) << endl);
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
      temp[j] += v[j][jj]*b[jj];

     
      
      //TRACE("In SPMINV()...calculating params..."<< PRINTV(j) << PRINTV(jj) 
      //<< PRINTV(v[j][jj]) <<" / old...: " <<PRINTV(b[jj]) << " / new...: "
      //<<PRINTV(temp[j]) );     
            
      
    }					
  }

  for (j=0; j<n; j++)
  {	
    b[j] = temp[j];
  }					

  delete[] temp;
  
  return nrank;
}


/*
  -----------------------------------------------------------
  SPAVAT
  -----------------------------------------------------------

  multiply symmetric N-by-N matrix from the left with general M-by-N
  matrix and from the right with the transposed of the same  general
  matrix  to  form  symmetric  M-by-M   matrix.
  
  T
  CALL SPAVAT(V,A,W,N,M)      W   =   A   *   V   *   A
  M*M     M*N     N*N     N*M
  
  where V = symmetric N-by-N matrix
  A = general N-by-M matrix
  W = symmetric M-by-M matrix
  -----------------------------------------------------------
*/


bool Millepede::SpAVAt(double v[][mlocal], double a[][mlocal], double w[][mglobl], int n, int m)
{
  int i,j, k, l; 
	
  for (i=0; i<m; i++)
  {
    for (j=0; j<m; j++)   // Could be improved as matrix symmetric
    {
      w[i][j] = 0.0;      // Reset final matrix
			
      for (k=0; k<n; k++)
      {
        for (l=0; l<n; l++)
        {
          w[i][j] += a[i][k]*v[k][l]*a[j][l];  // fill the matrix
        }
      }
    }
  }
	
  return true;
}


/*
  -----------------------------------------------------------
  SPAX
  -----------------------------------------------------------

  multiply general M-by-N matrix A and N-vector X
 
  CALL  SPAX(A,X,Y,M,N)          Y =  A * X
  M   M*N  N
 
  where A = general M-by-N matrix (A11 A12 ... A1N  A21 A22 ...)
  X = N vector
  Y = M vector
  -----------------------------------------------------------
*/

bool Millepede::SpAX(double a[][mlocal], double x[], double y[], int n, int m)
{
  int i,j; 
	
  for (i=0; i<m; i++)
  {
    y[i] = 0.0;	    // Reset final vector
			
    for (j=0; j<n; j++)
    {
      y[i] += a[i][j]*x[j];  // fill the vector
    }
  }
	
  return true;
}


/*
  -----------------------------------------------------------
  PRTGLO
  -----------------------------------------------------------

  Print the final results into the logfile

  -----------------------------------------------------------
*/


bool Millepede::PrtGlo()
{
  double err, gcor;

 //MDDEBUG
//   std::vector<double>::const_iterator itervec = m_res_value.begin();
//   int test =0;
  
//   while( itervec != m_res_value.end()) {
//     TRACE(PRINTV(test) << PRINTV( (*itervec) ) << PRINTV( (m_res_index[test]) ));
//     itervec++;
//     test++;
//   }



  cout << "" << endl;
  cout << "   Result of fit for global parameters" << endl;
  cout << "   ===================================" << endl;
  cout << "    I       initial       final       differ   " 
       << "     lastcor        Error       gcor" << endl;
  cout << "-----------------------------------------" 
       << "------------------------------------------" << endl;
	
  for (int i=0; i<nagb; i++)
  {
    err = sqrt(fabs(cgmat[i][i]));
    if (cgmat[i][i] < 0.0) err = -err;
    gcor = 0.0;


    //MDDEBUG
    //TRACE(endl<<" GLOBAL PAR.CORREL " << PRINTV(i) << PRINTV(gcor) << PRINTV(cgmat[i][i]) << PRINTV(m_diag[i]) << endl);
            


    if (i%(nagb/6) == 0)
    {
      cout << "-----------------PRTGLO()------------------------" 
           << "------------------------------------------" << endl;
    }

    if(DEBUGOUTPUT) TRACE(PRINTV(m_diag[i]) << PRINTV(cgmat[i][i]) );
    
		
    if (fabs(cgmat[i][i]*m_diag[i]) > 0)
    {
      cout << std::setprecision(4) << std::fixed;
      gcor = sqrt(fabs(1.0-1.0/(cgmat[i][i]*m_diag[i]) ));

      cout << std::setw(4) << i << "  / " << std::setw(10) << pparm[i] 
           << "  / " << std::setw(10) << pparm[i]+ dparm[i] 
           << "  / " << std::setw(13) << dparm[i] 
           << "  / " << std::setw(13) << bgvec[i] << "  / " << std::setw(10) 
           << std::setprecision(5) << err << "  / " << std::setw(10) << gcor << endl;
    }
    else
    {
      cout << std::setw(4) << i << "  / " << std::setw(4) << "OFF" 
           << "  / " << std::setw(4) << "OFF" 
           << "  / " << std::setw(4) << "OFF" 
           << "  / " << std::setw(4) << "OFF" 
           << "  / " << std::setw(4) << "OFF" 
           << "  / " << std::setw(4) << "OFF" << endl;
    }
  }
  return true;
}


/*
  ----------------------------------------------------------------
  CHINDL:  return the limit in chi^2/nd for n sigmas stdev authorized
  ----------------------------------------------------------------

  Only n=1, 2, and 3 are expected in input
  ----------------------------------------------------------------
*/

double Millepede::chindl(int n, int nd)
{
  int m;
  double sn[3]        =	{0.47523, 1.690140, 2.782170};
  double table[3][30] = {{1.0000, 1.1479, 1.1753, 1.1798, 1.1775, 1.1730, 1.1680, 1.1630,
                          1.1581, 1.1536, 1.1493, 1.1454, 1.1417, 1.1383, 1.1351, 1.1321,
                          1.1293, 1.1266, 1.1242, 1.1218, 1.1196, 1.1175, 1.1155, 1.1136,
                          1.1119, 1.1101, 1.1085, 1.1070, 1.1055, 1.1040},
                         {4.0000, 3.0900, 2.6750, 2.4290, 2.2628, 2.1415, 2.0481, 1.9736,
                          1.9124, 1.8610, 1.8171, 1.7791, 1.7457, 1.7161, 1.6897, 1.6658,
                          1.6442, 1.6246, 1.6065, 1.5899, 1.5745, 1.5603, 1.5470, 1.5346,
                          1.5230, 1.5120, 1.5017, 1.4920, 1.4829, 1.4742},
                         {9.0000, 5.9146, 4.7184, 4.0628, 3.6410, 3.3436, 3.1209, 2.9468,
                          2.8063, 2.6902, 2.5922, 2.5082, 2.4352, 2.3711, 2.3143, 2.2635,
                          2.2178, 2.1764, 2.1386, 2.1040, 2.0722, 2.0428, 2.0155, 1.9901,
                          1.9665, 1.9443, 1.9235, 1.9040, 1.8855, 1.8681}};

  if (nd < 1)
  {
    return 0.0;
  }
  else
  {
    m = std::max(1,std::min(n,3));

    if (nd <= 30)
    {
      return table[m-1][nd-1];
    }
    else // approximation
    {
      return ((sn[m-1]+sqrt(float(2*nd-3)))*(sn[m-1]+sqrt(float(2*nd-3))))/float(2*nd-2);
    }
  }
}


int    Millepede::GetTrackNumber()                      {return m_track_number;}
void   Millepede::SetTrackNumber(int value)             {m_track_number = value;}


const std::vector<double> Millepede::GetLocalResiduals() const
{
  return m_res_value;
}
const std::vector<double> Millepede::GetLocalResidualsIndex() const
{
  return m_res_index;
}
const std::vector<double> Millepede::GetTrackMeasGuess() const
{
  return m_rmeas_guess;
}
const void Millepede::ClearTrackMeasGuess() 
{
  m_rmeas_guess.clear();
}
const double Millepede::GetGlobalMatrix(int row, int column) 
{
  return m_glmat[row][column];
}
const double Millepede::GetGlobalMatrixInv(int row, int column) 
{
  return m_glmatinv[row][column];
}
