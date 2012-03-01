// $Id: DiagSolvTool.cpp,v 1.21 2010-05-03 14:30:37 wouter Exp $
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
#include "AlignKernel/AlSymMat.h"
#include "AlignKernel/AlMat.h"
#include "AlignKernel/AlVec.h"
 
// local
#include "DiagSolvTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DiagSolvTool
//
// 2007-06 : Adlene Hicheur
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( DiagSolvTool );



//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DiagSolvTool::DiagSolvTool( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : GaudiTupleTool ( type, name , parent )
{
  declareInterface<IAlignSolvTool>(this);
  declareProperty( "NumberOfPrintedEigenvalues", m_numberOfPrintedEigenvalues = 20 ) ;
  declareProperty( "LowerModCut",par_modcut=0);
  declareProperty( "EigenValueThreshold", m_eigenValueThreshold = -1 ) ;
  declareProperty( "WriteMonNTuple",par_writentp=false);
  declareProperty( "ApplyScaling", m_applyScaling=false) ;
  declareProperty( "MinEigenModeChisquare", m_minEigenModeChisquare = -1 ) ;
}

//=============================================================================
// Destructor
//=============================================================================
DiagSolvTool::~DiagSolvTool() {} 

//=============================================================================
// Initialize
//=============================================================================
StatusCode DiagSolvTool::initialize() 
{
  StatusCode sc = GaudiTupleTool::initialize() ;
  info() << "EigenValueThreshold = " << m_eigenValueThreshold << endreq ;
  info() << "MinEigenModeChisquare = " << m_minEigenModeChisquare << endreq ;
  info() << "WriteMonNTuple = " << par_writentp << endreq ;
  return sc ;
}




//=============================================================================

bool DiagSolvTool::compute(AlSymMat& m,AlVec& b) const
{


 if(m.size() > 0) {
   
   const_cast<DiagSolvTool*>(this)->SolvDiag(m,b);

   return true;
 } else {
   error() << "Error, null matrix size, don't call the solving!"<<endmsg;
   return false;
 }
  

  
}

template<class T>
struct SortByAbs
{
  bool operator()(T lhs, T rhs) const {
    return std::abs( lhs ) < std::abs(rhs ) ;
  }
} ;

int DiagSolvTool::SolvDiag(AlSymMat& m_bigmatrix, AlVec& m_bigvector) {

 
  info() << "Solving with diagonalization method "<<endmsg;
  
  int infjob=999;
  char jobz = 'V';
  size_t N = m_bigmatrix.size();
  
  //declare transition matrix + vector to store eigenvalues
  AlMat z(N,N);
  AlVec w(N);
  AlVec rweight(N), cweight(N);

  info() << "After z/w allocation" << endmsg;
  
  //normalize bigmatrix and bigvector before solving, alternatively use preconditionning (but complicates eigenvalue analysis)
  
  double scale=1 ;
  if( m_applyScaling ) {
    scale = findMax(m_bigmatrix);
    if (scale>1e-19) {    
      info() << "Scaling system with scale: " << scale << endreq ;
      m_bigvector*=(1./scale);
      m_bigmatrix*=(1./scale);
    } else {scale=1.;}
  }
  //   Precond(m_bigmatrix,rweight,cweight,true);

  //  for(int i=0;i<N;i++) m_bigvector[i]*=rweight[i];

  //do solving, default diagonalization via LAPACK

  infjob = m_bigmatrix.diagonalize(jobz,w,z);

  //diagonalize via GSL
  //  m_bigmatrix.diagonalize_GSL(w,z);

  info() << "After diagonalization" << endmsg;

  // Dump the 10 smallest eigenvalues. We'll sort these really in abs,
  // ignoring the sign.
  {
    std::ostringstream logmessage ;
    logmessage << "Smallest eigen values: [ " << std::setprecision(4) ;
    std::vector<double> sortedev(N) ;
    for(size_t ipar = 0; ipar<N; ++ipar) sortedev[ipar] = w[ipar] ;
    std::sort(sortedev.begin(),sortedev.end(),SortByAbs<double>()) ;
    size_t maxpar = std::min(N,m_numberOfPrintedEigenvalues) ;
    for(size_t ipar = 0; ipar<maxpar; ++ipar) 
      logmessage << sortedev[ipar]*scale << (( ipar<maxpar-1) ? ", " : " ]") ;

    // find the smallest eigenvalue and dump the corresponding eigenvector
    if( N<=m_numberOfPrintedEigenvalues ) {
      logmessage << std::endl ;
      size_t imin(0) ;
      for(size_t ipar = 1; ipar<N; ++ipar) 
	if( std::abs(w[ipar]) < std::abs(w[imin]) ) imin = ipar ;
      logmessage << "Eigenvector for smallest eigenvalue: [ " ;
      for(size_t ipar = 0; ipar<N; ++ipar) logmessage << z[imin][ipar] << (( ipar<N-1) ? ", " : " ]") ;
    }
    info() << logmessage.str() << endmsg ;
  }
  
  if (infjob==0) {

    //   Compute bigvector in diagonal basis
    AlVec D(N);
    
    D = z*m_bigvector;
    //Warning: with GSL, the definition of z is transposed:
    //D = z.T()*m_bigvector; 

  // Issue a warning for each negative eigenvalue
    for(size_t ipar = 0; ipar<N; ++ipar) 
      if( w[ipar] < - DBL_MIN ) 
	warning() << "Negative eigenvalue (i,val,chi2)= " << ipar << " " 
		  << w[ipar] << " "
		  << D[ipar]*D[ipar]/w[ipar] << endmsg ;
    
    //compute alignment corrections and their variance. first flag modes that we cut away by eigenvalue
    std::vector<bool> keepEigenValue(N,true) ;
    // cut away a fixed number of modes (still need to pass the constraints!)
    for (size_t i=0;i<par_modcut;i++) keepEigenValue[i] = false ;
    // or cut by value (constraints have large negative value, so the 'abs' should do)
    if( m_eigenValueThreshold > 0 )
      for( size_t i=0; i<N; i++) 
	keepEigenValue[i] = std::abs(w[i]) > m_eigenValueThreshold ||
	  ( m_minEigenModeChisquare >0 && D[i]*D[i]/w[i] > m_minEigenModeChisquare ) ;

    double sumchisqrejected(0), sumchisqaccepted(0) ;
    size_t numrejected(0) ;
    for( size_t i=0; i<N; i++) 
      if( !keepEigenValue[i] ) {
	info() << "Rejecting eigenvalue: val = " << w[i]
	       << " chisq = " << D[i]*D[i]/w[i] << endreq ;
	++numrejected ;
	sumchisqrejected += D[i]*D[i]/w[i] ;
      } else {
	sumchisqaccepted += D[i]*D[i]/w[i] ;
	if( std::abs(w[i]) < m_eigenValueThreshold )
	  info() << "Accepting eigenvalue: val = " << w[i]
		 << " chisq = " << D[i]*D[i]/w[i] << endreq ;
      }
    
    info() << "Number / total chi2 of rejected eigenvalues: "
	   << numrejected << " " << sumchisqrejected << endreq ;
    info() << "Total chi2 of accepted eigenvalues: "
	   << sumchisqaccepted << endreq ;
    
    // reset the input
    for( size_t i=0; i<N; i++) {
      m_bigvector[i] = 0 ;
      for (size_t j=0;j<=i;j++)
        m_bigmatrix[i][j]=0.;
    }
    
    // now fill it
    for( size_t k=0;k<N;++k)
      if( keepEigenValue[k] )
	for( size_t i=0; i<N; ++i) {
	  m_bigvector[i] += z[k][i]*D[k]/w[k] ;
	  for (size_t j=0;j<=i; ++j) 
	    m_bigmatrix[i][j] += (z[k][i]*z[k][j]/(w[k]*scale));
	}
    
    // fill the ntuple, but only the first time the solver tool is called
    if( par_writentp) {
      static bool first=true ;
      if (first) MonitorDiag(z,w,D,scale);
      first = false ;
    }
  }
  else {
    
    error() << "inversion (diagonalization) of big matrix failed" <<endmsg;
  }
  
  return infjob;
}

void DiagSolvTool::MonitorDiag(AlMat& z, AlVec& w, AlVec& D, double scale) 
{
// Creating the tuple in memory
  Tuples::Tuple MonTuple = nTuple("SolvMonitor","Spectral Monitoring ntuple" );
  
// Fill the ntuple

  for (unsigned i = 0u; i < w.size(); ++i) {
    nteig_mode = i;
    nteig_eigval = w[i];
    if (w[i]>1e-16) nteig_eigmod = D[i]/w[i]; else nteig_eigmod = 0.0;
    if (w[i]>1e-16) nteig_erreigmod = sqrt(1.0/(w[i]*scale)); else nteig_erreigmod = 0.0;

    MonTuple->column("mode",nteig_mode);
    MonTuple->column("eigenval",nteig_eigval);
    MonTuple->column("eparam",nteig_eigmod);
    MonTuple->column("erreparam",nteig_erreigmod);  
    std::vector<double> eigenvector;
    for (unsigned j = 0u; j < w.size(); ++j) {
      eigenvector.push_back(z[i][j]);
    }
    
    MonTuple->farray("eigenvec",eigenvector.begin(),eigenvector.end(),"nvec",w.size());
    
    StatusCode sc = MonTuple->write();  
    if (!sc.isSuccess()) error()<<"Problem filling Monitor ntuple" << endmsg;
  }
  
}


/*void DiagSolvTool::MonitorDiag(AlMat& z, AlVec& w, AlVec& D) 
{


  
  std::string m_NtpName = "/NTUPLES/MONITOR";
  
  info() <<"Booking Monitoring Ntuple"<< endmsg;
  NTuplePtr MonNtp(ntupleSvc(),m_NtpName);
  if (MonNtp)  {m_tuple=MonNtp; debug() <<"Ntuple already booked"<<endmsg;
  }
  
  else {
    m_tuple = ntupleSvc()->book(m_NtpName,CLID_ColumnWiseTuple,"SolvMonitor");
    
    if (m_tuple) {
      StatusCode stat;

      stat = m_tuple->addItem("mode",nteig_mode);
      stat = m_tuple->addItem("eigenval",nteig_eigval);
      stat = m_tuple->addItem("eparam",nteig_eigmod);
      stat = m_tuple->addItem("erreparam",nteig_erreigmod);      
      
      
      info() << "Ntuple "<<m_NtpName<< " is now booked..."<<endmsg;
      
      
    } else error()<<"Error in Ntuple booking"<<endmsg;
    
    
  }
  

  
  for (size_t i=0;i<w.size();i++) {
    nteig_mode = i;
    nteig_eigval = w[i];
    if (w[i]>1e-16) nteig_eigmod = D[i]/w[i]; else nteig_eigmod = 0.0;
    if (w[i]>1e-16) nteig_erreigmod = sqrt(1.0/(w[i]*scale)); else nteig_erreigmod = 0.0;
    StatusCode sc = m_tuple->write(); 
    if (!sc.isSuccess()) error()<<"Problem filling Monitor ntuple" << endmsg;   
  }

  
}*/


// Routine to equilibrate the matrix for better conditioning

void DiagSolvTool::Precond(AlSymMat& M, AlVec& r, AlVec& c, bool equIn = true) 
{
  
 AlMat V(M.size(),M.size()); 
 V.copyS(M);
  
 size_t N = M.size();
  
 
  
 size_t i, j;

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

double DiagSolvTool::findMax(AlSymMat& M) 
{
  
  double max(0.);
  
 for (unsigned i = 0u; i < M.size(); ++i) {

   for (unsigned j = 0u; j <= i; ++j) {
     if (fabs(M[i][j])>max) max = fabs(M[i][j]);      
    }

  } 

 
   return max; 
  
}
