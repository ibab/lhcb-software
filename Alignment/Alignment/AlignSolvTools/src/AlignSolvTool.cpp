// $Id: AlignSolvTool.cpp,v 1.8 2007-05-15 13:26:24 jblouw Exp $
// Include files 

#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <math.h>


// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/NTuple.h"
//
#include "SolvKernel/AlSymMat.h"
#include "SolvKernel/AlMat.h"
#include "SolvKernel/AlVec.h"
 
// local
#include "AlignSolvTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : AlignSolvTool
//
// 2006-09-27 : Adlene Hicheur
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( AlignSolvTool );

extern "C" {
#ifdef WIN32
 void MINRES (int*,double*,double*,double*,double*,double*,double*,double*,double*,double*,double*,int*,int*,double*,int*,int*,double*,int*,int*,double*,double*,double*,double*);
#define minres_ MINRES
#else
 void minres_ (int*,double*,double*,double*,double*,double*,double*,double*,double*,double*,double*,int*,int*,double*,int*,int*,double*,int*,int*,double*,double*,double*,double*);
#endif
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
AlignSolvTool::AlignSolvTool( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IAlignSolvTool>(this);
  declareProperty( "SolvMethod",m_method=1);
}
//=============================================================================
// Destructor
//=============================================================================
AlignSolvTool::~AlignSolvTool() {} 

//=============================================================================

bool AlignSolvTool::compute(AlSymMat& m,AlVec& b) 
{
  if (m_method==1) SolvSpmInv(m,b);
  if (m_method==2) SolvDiag(m,b);
  

  return true;
}


int AlignSolvTool::SolvSpmInv(AlSymMat& M, AlVec& B)
{

 AlMat V(M.size(),M.size()); 
 V.copyS(M);
  
  int N = M.size();
  
  AlVec diag(N);
  bool *flag = new bool[N];
  
  int i, j, jj, k,nrank;
  double vkk;
  double eps = 0.00000000000001;

  AlVec r(N);
  AlVec c(N);
  

  AlVec temp(N);

  for (i=0; i<N; i++)
  {
    flag[i] = true;

   //   for (j=0; j<=i; j++) {if (V[j][i] == 0) {V[j][i] = V[i][j];}}
  }
  
  // Small loop for matrix equilibration (gives a better conditioning) 

  for (i=0; i<N; i++)
  {
    for (j=0; j<N; j++)
    { 
      if (fabs(V[i][j]) >= r[i]) r[i] = fabs(V[i][j]); // Max elemt of row i
      if (fabs(V[j][i]) >= c[i]) c[i] = fabs(V[j][i]); // Max elemt of column i
    }
  }
  for (i=0; i<N; i++)
  {
    if (eps< r[i]) r[i] = 1./r[i]; // Max elemt of row i
    if (eps< c[i]) c[i] = 1./c[i]; // Max elemt of column i

  }

  for (i=0; i<N; i++) // Equilibrate the V matrix
  {
    for (j=0; j<N; j++) {V[i][j] = sqrt(r[i])*V[i][j]*sqrt(c[j]);}
    }

  nrank = 0;

  // save diagonal elem absolute values 
  for (i=0; i<N; i++) {diag[i] = fabs(V[i][i]);} 

  for (i=0; i<N; i++)
  {
    vkk = 0.0;
    k = -1;
    
    for (j=0; j<N; j++) // First look for the pivot, ie max unused diagonal element 
    {
      if (flag[j] && (fabs(V[j][j])>std::max(fabs(vkk),eps*diag[j])))
      {
        vkk = V[j][j];
        k = j;
      }
    }
             
    if (k >= 0)    // pivot found
    {      
      debug() << "Pivot value :" << vkk << endmsg;
      nrank++;
      flag[k] = false; // This value is used
      vkk = 1.0/vkk;
      V[k][k] = -vkk; // Replace pivot by its inverse
 
      for (j=0; j<N; j++)
      {
        for (jj=0; jj<N; jj++)  
        {
          if (j != k && jj != k) // Other elements (!!! do them first as you use old Vk][j]'s !!!)
          {
            V[j][jj] = V[j][jj] - vkk*V[j][k]*V[k][jj];
            //    V[j][jj] = V[j][jj] + vkk*V[j][k]*V[k][jj];

          }
        }
      }

      for (j=0; j<N; j++)
      {
        if (j != k) // Pivot row or column elements 
        {
          V[j][k] = (V[j][k])*vkk;      // Column
          V[k][j] = (V[k][j])*vkk;      // Line
        }
      }
    }
    else   // No more pivot value (clear those elements)
    {
      for (j=0; j<N; j++)
      {
        if (flag[j])
        {
          B[j] = 0.0;

          for (k=0; k<=j; k++)
          {
            V[j][k] = 0.0;
            //           V[k][j] = 0.0;
          }
        }
      }

      break;  // No more pivots anyway, stop here
    }
  }
  for (i=0; i<N; i++) // Correct matrix V
  {
    for (j=0; j<N; j++) {V[i][j] = sqrt(c[i])*V[i][j]*sqrt(r[j]);}
  }

  for (j=0; j<N; j++)
  {
    temp[j] = 0.0;
    
    for (jj=0; jj<N; jj++)  // Reverse matrix elements
    {
      V[j][jj] = -V[j][jj];
      M[j][jj]=V[j][jj];
      temp[j] += V[j][jj]*B[jj];
    }
  }

  for (j=0; j<N; j++) {B[j] = temp[j];} // The final result                         
  delete [] flag;
  return nrank;
}
   

int AlignSolvTool::SolvDiag(AlSymMat& m_bigmatrix, AlVec& m_bigvector) {

 
  info() << "Solving with diagonalization method "<<endmsg;
  
  int infjob=999;
  char jobz = 'V';
  int N = m_bigmatrix.size();
  m_scale = 1.;
  
  //declare transition matrix + vector to store eigenvalues
  AlMat z(N,N);
  AlVec w(N);
  AlVec rweight(N), cweight(N);

  info() << "After z/w allocation" << endmsg;
  
  //normalize bigmatrix and bigvector before solving, alternatively use preconditionning (but complicates eigenvalue analysis)
  
  m_scale=findMax(m_bigmatrix);

  if (m_scale>1e-19) {    
    m_bigvector*=(1./m_scale);
    m_bigmatrix*=(1./m_scale);
  } else {m_scale=1.;}

  //   Precond(m_bigmatrix,rweight,cweight,true);

  //  for(int i=0;i<N;i++) m_bigvector[i]*=rweight[i];

  //do solving, default diagonalization via LAPACK

  infjob = m_bigmatrix.diagonalize(jobz,w,z);

  //diagonalize via GSL
  //  m_bigmatrix.diagonalize_GSL(w,z);

  info() << "After diagonalization" << endmsg;
  
  if (infjob==0) {

    //   Compute bigvector in diagonal basis
    AlVec D(N);
 
   
    D = z*m_bigvector;
    //Warning: with GSL, the definition of z is transposed:
    //D = z.T()*m_bigvector; 


    //compute alignment corrections and their variance
    double threshold = 1e-19;
    int par_modcut =0;
    AlVec m_AlignPar(N);
    AlVec m_AlignSqErr(N);
    
    for( int i=0; i<N; i++) {

      
      // for( int i=(24); i<N; i++) {
      for (int j=0;j<par_modcut;j++) {
        m_bigmatrix[i][j]=0.;
        for (int k=0;k<N;k++) {
          if (w[k]>threshold) m_bigmatrix[i][j]=m_bigmatrix[i][j]+(z[k][i]*z[k][j]/w[k]);
          //with GSL convention:
          //   if (w[k]>threshold) m_bigmatrix[i][j]=m_bigmatrix[i][j]+(z[i][k]*z[j][k]/w[k]); 
        }
        
      }
      

      for (int j=par_modcut;j<N;j++) {

        m_bigmatrix[i][j]=0.;
        if (w[j]>threshold){
	  m_AlignPar[i] += z[j][i]*(D[j]/w[j]);
    //m_AlignPar[i] += z[j][i]*(D[j]/w[j])*cweight[i];
	  m_AlignSqErr[i] += (z[j][i])*(z[j][i])/(w[j]*m_scale);
    //m_AlignSqErr[i] += ((z[j][i])*(z[j][i])/w[j])*rweight[i]*cweight[i];
	  //Warning, in GSL, definition of z is transposed:
	  //	  m_AlignPar[i] += z[i][j]*(D[j]/w[j]);
	  //	  m_AlignSqErr[i] += (z[i][j])*(z[i][j])/(w[j]*m_scale);	  

    

        }

        for (int k=0;k<N;k++) {
          if (w[k]>threshold) m_bigmatrix[i][j]=m_bigmatrix[i][j]+(z[k][i]*z[k][j]/w[k]);
          //with GSL convention:
          //   if (w[k]>threshold) m_bigmatrix[i][j]=m_bigmatrix[i][j]+(z[i][k]*z[j][k]/w[k]); 
        }

      }

      m_bigvector[i]=m_AlignPar[i];
      
 

    }




    
    
  }
  else {
    
    info() << "inversion (diagonalization) of big matrix failed" <<endmsg;
  }
  
  return infjob;
}

int AlignSolvTool::SolvMinRes(AlSymMat& m_bigmatrix, AlVec& m_bigvector, AlVec& result) {


  info() << "Solving with MINRES method "<<endmsg;

double m_scale = 1000000.;
  m_bigvector*=(1./m_scale);
  m_bigmatrix*=(1./m_scale);


  int nmin = m_bigmatrix.size();
  //  AlVec bmres(nminres);
  //  double* bmres =  new double[nmin];
  double* r1 =  new double[nmin];
  double* r2 =  new double[nmin];
  double* v =  new double[nmin];
  double* w0 =  new double[nmin];
  double* w1 =  new double[nmin];
  double* w2 =  new double[nmin];
  //  double* x =  new double[nmin];
  double* y =  new double[nmin];
  int checkA = 0;
  int precon = 0;
  double shift = 0.;
  int nout(3), itnlim(500),istop(999),itn;
  double rtol(0.00000000001), Anorm,Acond,rnorm,ynorm;
  
  
  

  minres_(&nmin, m_bigmatrix.ptr_data,m_bigvector.ptr_data, r1, r2, v, w0, w1, w2, result.ptr_data,y,&checkA, &precon, &shift,&nout ,&itnlim, &rtol,&istop,&itn,&Anorm,&Acond,&rnorm,&ynorm );

  std::fstream align_min("align_minres.txt", std::ios::out);

      int nmod=21;
    
      for (int i=0;i<nmod;i++) {
        for (int m=0;m<6;m++) {
          
      align_min<<std::setprecision(6)<<result[i+m*nmod]<<std::endl;

        }
        
      }
    align_min.close();

    //delete [] bmres;
delete [] r1;
delete [] r2;
delete [] v;
delete [] w0;
delete [] w1;
delete [] w2;
//delete [] x;
delete [] y;


 return istop;
 

}


void AlignSolvTool::MonitorDiag(AlMat& z, AlVec& w, AlVec& D) 
{

  NTuple::Item<float> nteig_tx,nteig_ty,nteig_tz,nteig_rx,nteig_ry,nteig_rz;
  NTuple::Item<float> nteig_mode,nteig_eigval,nteig_eigmod,nteig_erreigmod;
  
  //  NTuple::Array<float> nt_vec;
  //  NTuple::Matrix<float> nt_mat;
  
  
  std::string m_NtpName = "/NTUPLES/MONITOR";
  
  info() <<"Booking Monitoring Ntuple"<< endmsg;
  NTuplePtr MonNtp(ntupleSvc(),m_NtpName);
  if (MonNtp) debug() <<"Ntuple already booked"<<endmsg;
  else {
    MonNtp = ntupleSvc()->book(m_NtpName,CLID_ColumnWiseTuple,"SolvMonitor");

    if (MonNtp) {
      StatusCode stat;

      stat = MonNtp->addItem("mode",nteig_mode);
      stat = MonNtp->addItem("eigenval",nteig_eigval);
      stat = MonNtp->addItem("eparam",nteig_eigmod);
      stat = MonNtp->addItem("erreparam",nteig_erreigmod);


      /*      stat = MonNtp->addItem("tx",nteig_tx);
      stat = MonNtp->addItem("ty",nteig_ty);
      stat = MonNtp->addItem("tz",nteig_tz);
      stat = MonNtp->addItem("rx",nteig_rx);
      stat = MonNtp->addItem("ry",nteig_ry);
      stat = MonNtp->addItem("rz",nteig_rz);*/
      
      debug() << "Ntuple "<<m_NtpName<< " is now booked..."<<endmsg;
      
      
    } else error()<<"Error in Ntuple booking"<<endmsg;
    
    
  }

  for (int i=0;i<w.size();i++) {
    nteig_mode = i;
    nteig_eigval = w[i];
    if (w[i]>1e-16) nteig_eigmod = D[i]/w[i]; else nteig_eigmod = 0.0;
    if (w[i]>1e-16) nteig_erreigmod = sqrt(1.0/(w[i]*m_scale)); else nteig_erreigmod = 0.0;
    StatusCode sc = MonNtp->writeRecord();
    
    if (!sc.isSuccess()) error()<<"Problem filling Monitor ntuple" << endmsg;
    
  }
  

  /*  for(int k=0;k<nweak;k++) {
  nteig_tx = z[j][i]*(-D[j]/w[j]);
  nteig_ty = z[j][i+1]*(-D[j]/w[j]);
  nteig_tz = z[j][i+2]*(-D[j]/w[j]);
  nteig_rx = z[j][i+3]*(-D[j]/w[j]);
  nteig_ry = z[j][i+4]*(-D[j]/w[j]);
  nteig_rz = z[j][i+5]*(-D[j]/w[j]);

  StatusCode sc = MonNtp->writeRecord();
  }*/
  
      
    int nweak = 6;
    
    int nmod = 21;
    
    for(int k=0;k<nweak;k++) {
      std::stringstream temp;
      std::string istr;
      
      bool conv = (temp << k);
      if (conv) {
        istr = temp.str();      
      }
      
      std::string eigpath = "eigenvec"+istr+".txt";
      
      std::fstream eigenvec(eigpath.c_str(), std::ios::out);
      
       for (int i=0;i<nmod;i++) {
        for (int m=0;m<6;m++) {  

 //Print the eigenmodes
          eigenvec << std::setw(12) << z[i+m*nmod][k]<< std::endl;
          //with GSL, convention for z is transposed:
          //  eigenvec << std::setw(12) << z[k][i+m*nmod]<< std::endl;
    
           }
        
      }
       eigenvec.close();
       
       
    }  


  
}


// Routine to equilibrate the matrix for better conditioning

void AlignSolvTool::Precond(AlSymMat& M, AlVec& r, AlVec& c, bool equIn = true) 
{
  
 AlMat V(M.size(),M.size()); 
 V.copyS(M);
  
 int N = M.size();
  
 
  
 int i, j;

 double eps = 1e-17;


 if (equIn) {
   

  
  // Matrix equilibration 

  for (i=0; i<N; i++)
  {
    for (j=0; j<N; j++)
    { 
      if (fabs(V[i][j]) >= r[i]) r[i] = fabs(V[i][j]); // Max elemt of row i
      if (fabs(V[j][i]) >= c[i]) c[i] = fabs(V[j][i]); // Max elemt of column i
    }
  }

  for (i=0; i<N; i++)
  {
    if (eps< r[i]) r[i] = sqrt(1./r[i]); // Max elemt of row i
    if (eps< c[i]) c[i] = sqrt(1./c[i]); // Max elemt of column i

  }

  for (i=0; i<N; i++) // Equilibrate the V matrix
  {
    for (j=0; j<N; j++) {

      V[i][j] = r[i]*V[i][j]*c[j];
      M[i][j] = V[i][j];
      
    }
  }
  
 } else { // Recover the matrix only
   
 
  for (i=0; i<N; i++) // Correct matrix V
  {
    for (j=0; j<N; j++) {

      V[i][j] = c[i]*V[i][j]*r[j];
      M[i][j] = V[i][j];

    }
  }
  
 }
 
  
}


//Routine to find the maximum element of a matrix

double AlignSolvTool::findMax(AlSymMat& M) 
{
  
  double max(0.);
  
 for (int i=0;i<M.size();i++) {

   for (int j=0;j<=i;j++) {
     if (fabs(M[i][j])>max) max = fabs(M[i][j]);      
    }

  } 

 
   return max; 
  
}
