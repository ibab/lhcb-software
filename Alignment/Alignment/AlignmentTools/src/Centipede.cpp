// Include files:

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/ToolFactory.h" 
#include "GaudiKernel/MsgStream.h"

#include "Centipede.h"
//#include "Singlepede.h"
#include "math.h"

/** @class Centipede Centipede.h AlignmentTools/Centipede.h
 *  
 *
 *  @author Johan Blouw
 *  @date   2007-08-14
 * Implementation of Centipede.
 *
 * This is a helper class which inherits from the Millepede class.
 * It is used to provide the Heidelberg alignment algorithm with
 * a unified interface to Millepede.
 * 
 */


DECLARE_TOOL_FACTORY( Centipede );

Centipede::Centipede( const std::string& type,
                      const std::string& name,
                      const IInterface* parent ) : Millepede(type,name,parent) {
  declareInterface<ICentipede>(this);
  info() << "Initializing Centipede!" << endreq;
};

Centipede::~Centipede() { 
  //  delete m_Singlepede; 
};

StatusCode Centipede::InitMille( bool DOF[],
				 int nglo,
				 int nloc, 
				 double startfact, 
				 int nstd,
				 double res_cut, 
				 double res_cut_init ) {
  double Sigma[6];
  int n_fits = 1;
  info() << "Into  Centipede::InitMille" << endreq;
  // Need to set Sigma to an absurd value, such as not
  // to direct the global fit to a certain direction.
  Sigma[0] = 10000000.0;
  Sigma[1] = 10000000.0;
  Sigma[2] = 10000000.0;
  Sigma[3] = 10000000.0;
  Sigma[4] = 10000000.0;
  Sigma[5] = 10000000.0;


  debug() << "" << endmsg;
  debug() << "----------------------------------------------------" << endmsg;
  debug() << "" << endmsg;
  debug() << "    Entering InitMille" << endmsg;
  debug() << "" << endmsg;
  debug() << "-----------------------------------------------------" << endmsg;
  debug() << "" << endmsg;

  ncs = 0;
  loctot  = 0;                        // Total number of local fits
  locrej  = 0;                        // Total number of local fits rejected
  cfactref  = 1.0;                    // Reference value for Chi^2/ndof cut

  Millepede::SetTrackNumber(0);       // Number of local fits (starts at 0)

  m_residual_cut = res_cut;
  m_residual_cut_init = res_cut_init; 
 
  nagb	  = nglo;    // Number of global derivatives
  nalc	  = nloc;       // Number of local derivatives
  nstdev  = nstd;     // Number of StDev for local fit chisquare cut

  int ndof = 0;

  for (int i=0; i<6; i++)
  {
    verbose() << "GetDOF(" << i << ")= " << DOF[i] << endmsg;

    if (DOF[i]) ndof++;

  }

  int n_stations = nagb/ndof;

  info() << "You want to take into account " << ndof << " degrees of freedom" << endmsg;
  info() << "You have " << n_stations << " objects to align" << endmsg;

  m_par.clear();       // Vector containing the alignment constants
  m_par.resize(nagb);

  info() << "Number of global parameters   : " << nagb << endmsg;
  info() << "Number of local parameters    : " << nalc << endmsg;
  info() << "Number of standard deviations : " << nstdev << endmsg;

  if (nagb>mglobl || nalc>mlocal)
  {
    debug() << "Too many parameters !!!!!" << endmsg;
    return StatusCode::FAILURE;
  }

  // All parameters initializations

  for (int i=0; i<mglobl; i++)
  {
    corrv[i] = 0.;
    psigm[i] = 0.;
    pparm[i] = 0.;
    dparm[i] = 0.;
    scdiag[i] = 0.;
    indgb[i] = 0; 
    nlnpa[i] = 0; 
    indnz[i] = 0; 
    indbk[i] = 0;
    
    for (int j=0; j<mglobl;j++) corrm[i][j] = 0.;
    for (int j=0; j<mlocal;j++) clcmat[i][j] = 0.;        
    for (int j=0; j<mcs;j++) adercs[j][i] = 0.;   
  }

  for (int i=0; i<mgl; i++)
  {
    diag[i] = 0.; 
    bgvec[i] = 0.;

    for (int j=0; j<mgl;j++) cgmat[i][j] = 0.; 
  }

  for (int i=0; i<mlocal; i++)
  {
    blvec[i] = 0.; 
    indlc[i] = 0;

    for (int j=0; j<mlocal;j++) clmat[i][j] = 0.; 
  }

  for (int j=0; j<mcs;j++) arhs[j] = 0.;   

  // Then we fix all parameters...

  for (int j=0; j<nagb; j++)  ParSig(j,0.0);

  // ...and we allow them to move if requested

  int compteur = 0;

  for (int i=0; i<6; i++) {
    debug() << "GetDOF(" << i << ")= " << DOF[i] << " Sigm " << Sigma[i] << endmsg;
    if (DOF[i]) {
      for (int j=compteur*n_stations; j<(compteur+1)*n_stations; j++) {
	debug() << "Calling ParSig with j = " << j << " Sigm[" << i << "] = " << Sigma[i] << " m_fixed = " << m_fixed << endreq;
	if ((j-compteur*n_stations) != m_fixed) {
	  ParSig(j,Sigma[i]); // Check that module is not fixed
	}
      }
      compteur++;
    }
  }

  for (int j=0; j<nagb; j++) debug() << "Sigm(" << j << ")= " << psigm[j] << endmsg;

  // Activate iterations (if requested)

  itert   = 0;	// By default iterations are turned off
  cfactr  = startfact;
  if (m_iteration) Millepede::InitUn(startfact);          

  arest.clear();  // Number of stored parameters when doing local fit
  arenl.clear(); // Linear or not
  indst.clear(); 

  storeind.clear();
  storeare.clear();
  storenl.clear();
  storeplace.clear();

  // Memory allocation for the stores

  debug() << "Store size is " << n_fits*(nagb+nalc+3) << endmsg;

  storeind.reserve(2*n_fits*(nagb+nalc+3));
  storeare.reserve(2*n_fits*(nagb+nalc+3));
  storenl.reserve(2*n_fits*(nagb+nalc+3));
  storeplace.reserve(2*n_fits);

  debug() << "" << endmsg;
  debug() << "----------------------------------------------------" << endmsg;
  debug() << "" << endmsg;
  debug() << "    InitMille has been successfully called!" << endmsg;
  debug() << "" << endmsg;
  debug() << "-----------------------------------------------------" << endmsg;
  debug() << "" << endmsg;
	
  return StatusCode::SUCCESS;




  // call S.V.'s initmille!
  //  return Millepede::InitMille( DOF, Sigma, nglo, 
  //			       nloc, startfact, nstd, 
  //			       res_cut, res_cut_init, n_fits );
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

StatusCode Centipede::EquLoc( std::vector<double> &dergb, 
			      std::vector<double> &derlc, 
			      const double &rmeas, 
			      double sigma ) {	
  // instantiate 4 arrays which are needed by Millepede,
  // but not used by us.
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

    
  
  debug() << "Out Equloc --  NST = " << arest.size() << endreq;
  //   for(unsigned int u = 0; u<arest.size();u++)
  //   {
  //     if(indst[u] += -1) info() << " ----------1. X hit/2. global/3. Y hit/4. global -------------------" << endreq;
  //     info() << "entry "<< u << " indst: " << indst[u] << " arest: " << arest[u] << endreq;
  //   }
  

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
 
StatusCode Centipede::ZerLoc( std::vector<double> &dergb, 
			      std::vector<double> &derlc ) {
  m_derGB = new double [dergb.size()];
  VectortoArray( dergb, m_derGB );
  VectortoArray( derlc, m_derLC );
  
  double dernl[dergb.size()];
  double dernl_i[dergb.size()];
  StatusCode sc = Millepede::ZerLoc( m_derGB, m_derLC, dernl, dernl_i );
  ArraytoVector( m_derGB, dergb);
  ArraytoVector( m_derLC, derlc);
  return sc;
}


StatusCode Centipede::FitLoc( int n,
			      std::vector<double> &track_params,
			      int single_fit,
			      std::vector<double> &estimated_para,
			      double &chi2,
			      double &res ) {
  // Few initializations
	
  int i, j, k, ik, ij, ist, nderlc, ndergl, ndf;
  int ja      = -1;
  int jb      = 0;
  int nagbn   = 0;
  double rmeas, wght, rms, cutval;
  double summ  = 0.0;
  double estimation_sum = 0.;
  int    nsum  = 0;
  nst   = 0; 
  nst   = arest.size(); // number of stored derivatives(loc&glob) & wght(for each glob entry)
  debug() << "Into FitLoc..." << endreq;

  for (int ii=0; ii<nagb; ii++)  // fill estimated parameters
  {
    pparm[ii] = estimated_para[ii];
  }

  // Fill the track store at first pass
  
  if (itert < 2 && single_fit != 1)  // Do it only once 
  {
    for (i=0; i<nst; i++)    // Store the track parameters
    {
      storeind.push_back(indst[i]);
      storeare.push_back(arest[i]);
    }
    storeplace.push_back(storeind.size());
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
        
        for (i=(jb+1); i<ist; i++)   // Now suppress the global part   
          // (only relevant with iterations)
        {
          j = indst[i];              // Global param indice
          
          rmeas -= arest[i]*(pparm[j]+dparm[j]);

        }

        for (i=(ja+1); i<jb; i++)    // Finally fill local matrix and vector
        {
          j = indst[i];   // Local param indice (the matrix line) 
          blvec[j] += wght*rmeas*arest[i];  // See note for precisions

          for (k=(ja+1); k<=i ; k++) // Symmetric matrix, don't bother k>j coeffs
          {
            ik = indst[k];						
            clmat[j][ik] += wght*arest[i]*arest[k];
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
  nrank = Millepede::SpmInv(clmat, &blvec[0], nalc, scdiag, scflag); //MD 15-11
  //  nrank = Millepede::SpmInv(clmat, &blvec[0], nalc);
  
    
  // Store the track params and errors

  for (i=0; i<nalc; i++)
  {
    //MD fill m_blvec[]
    track_params[2*i] = blvec[i]; // reference !
    track_params[2*i+1] = sqrt(fabs(clmat[i][i])); // reference !!

    
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

      if (ja == -1)     {ja = ist;}  // First  0 : rmeas
      else if (jb == 0) {jb = ist;}  // Second 0 : weight 
      else                           // Third  0 : end of equation  
      {	


        rmeas	= arest[ja];

        wght 	= arest[jb];
        
        nderlc = jb-ja-1;    // Number of local derivatives involved
        ndergl = ist-jb-1;   // Number of global derivatives involved
        
        // Print all (for debugging purposes)
        

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
        // reject the track if rmeas is too important (outlier)
        if (fabs(rmeas) >= m_residual_cut_init && itert <= 1)  
        {
          if(single_fit == 0){
            locrej++;      
            loctot++; //although rej. it was fittet before !
          }
          indst.clear(); // reset stores and go to the next track 
          arest.clear();	  
          return false;
        }
        
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

  if (ndf > 0) rms = summ/float(ndf);  // Chi^2/dof
	
  if (single_fit == 0) {
    loctot++;
  }
  

  if (nstdev != 0 && ndf > 0 && single_fit != 1) // Chisquare cut
  {
    cutval = Millepede::chindl(nstdev, ndf)*cfactr;
    if (rms > cutval) // Reject the track if too much...
    {
      
      locrej++;      
      
      indst.clear(); // reset stores and go to the next track 
      arest.clear();
      return false;
    }
  }

  if (single_fit == 1) //Stop here if just updating the track parameters
  {
    
    if(n==0) verbose() << "FitLoc called with argument '1' -> no global der. update!!" << endmsg ;
    
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
		
    for (k=0; k<nagbn; k++)
    {
      ik = indbk[k];
      cgmat[j][ik] -= corrm[i][k];

    }
  }

  indst.clear(); // Reset store for the next track 
  arest.clear();
  // calculate reduced chi2:
  res = summ/double(ndf);
  return true;
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
