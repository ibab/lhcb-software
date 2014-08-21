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
 
// local
#include "SpmInvTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : SpmInvTool
//
// 2007-06 : Adlene Hicheur
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( SpmInvTool )



//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SpmInvTool::SpmInvTool( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IAlignSolvTool>(this);
 }
//=============================================================================
// Destructor
//=============================================================================
SpmInvTool::~SpmInvTool() {} 

//=============================================================================

bool SpmInvTool::compute(AlSymMat& m,AlVec& b) const 
{
if(m.size() > 0) {
  const_cast<SpmInvTool*>(this)->SolvSpmInv(m,b);
  return true;
} else {
  error() << "Error, null matrix size, don't call the solving!"<<endmsg;
  return false;
}

}


int SpmInvTool::SolvSpmInv(AlSymMat& M, AlVec& B)
{

 AlMat V(M.size(),M.size()); 
 V.copyS(M);
  
  int N = M.size();
  
  AlVec diag(N);
  bool *flag = new bool[N];
  
  int i, j, jj, k,nrank;
  double vkk;
  double eps = 1e-16;

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
   


// Routine to equilibrate the matrix for better conditioning

void SpmInvTool::Precond(AlSymMat& M, AlVec& r, AlVec& c, bool equIn = true) 
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



