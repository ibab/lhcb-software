// $Id: DiagSolvTool.cpp,v 1.21 2010-05-03 14:30:37 wouter Exp $
// Include files 

#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <math.h>
#include <set>


// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
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
DECLARE_TOOL_FACTORY( DiagSolvTool )



//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DiagSolvTool::DiagSolvTool( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IAlignSolvTool>(this);
  declareProperty( "NumberOfPrintedEigenvalues", m_numberOfPrintedEigenvalues = 20 ) ;
  declareProperty( "EigenValueThreshold", m_eigenValueThreshold = -1 ) ;
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
  StatusCode sc = GaudiTool::initialize() ;
  info() << "EigenValueThreshold = " << m_eigenValueThreshold << endreq ;
  info() << "MinEigenModeChisquare = " << m_minEigenModeChisquare << endreq ;
  return sc ;
}

//=============================================================================

bool DiagSolvTool::compute(AlSymMat& m,AlVec& b) const
{
  IAlignSolvTool::SolutionInfo info ;
  StatusCode sc =  compute(m,b,info) ;
  return sc.isSuccess() ;
}

namespace {
  struct SortByAbsSecond
  {
    template<class T>
    bool operator()(const T& lhs, 
		    const T& rhs) const {
      return std::abs( lhs.second ) < std::abs(rhs.second ) ;
    }
  } ;
}

StatusCode DiagSolvTool::compute(AlSymMat& m_bigmatrix, AlVec& m_bigvector,
				 IAlignSolvTool::SolutionInfo& solinfo) const
{
  StatusCode sc = StatusCode::SUCCESS ;
 
  info() << "Solving with diagonalization method "<<endmsg;
  
  char jobz = 'V';
  size_t N = m_bigmatrix.size();
  
  //declare transition matrix + vector to store eigenvalues
  AlMat z(N,N);
  AlVec w(N);
  AlVec rweight(N), cweight(N);

  info() << "After z/w allocation" << endmsg;
  
  //do solving, default diagonalization via LAPACK

  int infjob = m_bigmatrix.diagonalize(jobz,w,z);

  //diagonalize via GSL
  //  m_bigmatrix.diagonalize_GSL(w,z);

  info() << "After diagonalization" << endmsg;

  // Dump the 10 smallest eigenvalues. We'll sort these really in abs,
  // ignoring the sign.
  {
    // count the number of negative eigenvalues and number smaller than one
    solinfo.numNegativeEigenvalues = solinfo.numSmallEigenvalues = 0 ;
    std::vector<std::pair<size_t,double> > sortedev(N) ;
    for(size_t ipar = 0; ipar<N; ++ipar) {
      sortedev[ipar] = std::make_pair(ipar,w[ipar]) ;
      if( std::abs( w[ipar] ) < 1.0 ) ++solinfo.numSmallEigenvalues ;
      if( w[ipar] < 0.0 ) ++solinfo.numNegativeEigenvalues ;
    }

    std::sort(sortedev.begin(),sortedev.end(),SortByAbsSecond()) ;
    std::ostringstream logmessage ;
    logmessage << "Smallest eigen values: [ " << std::setprecision(4) ;
    size_t maxpar = std::min(N,m_numberOfPrintedEigenvalues) ;
    for(size_t ipar = 0; ipar<maxpar; ++ipar) 
      logmessage << sortedev[ipar].second << (( ipar<maxpar-1) ? ", " : " ]") ;
    
    // consider the smallest eigenvalue
    if(N>0) {
      // dump the corresponding eigenvector
      if( N<=m_numberOfPrintedEigenvalues ) {
	logmessage << std::endl ;
	logmessage << "Eigenvector for smallest eigenvalue: [ " ;
	for(size_t ipar = 0; ipar<N; ++ipar) {
	  logmessage << z[sortedev[0].first][ipar] << (( ipar<N-1) ? ", " : " ]") ;
	}
      }
      // figure out which parameter contributes most to this eigenvector
      size_t thepar(0) ;
      for(size_t ipar = 0; ipar<N; ++ipar) {
	if( std::abs( z[sortedev[0].first][thepar] ) < std::abs( z[sortedev[0].first][ipar] ) )
	  thepar = ipar ;
      }
      solinfo.minEigenValue = sortedev[0].second ;
      solinfo.weakestModeMaxPar = thepar ;
      solinfo.weakestModeMaxParCoef = z[sortedev[0].first][thepar] ;
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
      if( !( w[ipar] > 0 ) ) 
	warning() << "Negative eigenvalue (i,val,chi2)= " << ipar << " " 
		  << w[ipar] << " "
		  << D[ipar]*D[ipar]/w[ipar] << endmsg ;
    
    //compute alignment corrections and their variance. first flag
    //modes that we cut away by eigenvalue. (constraints have large
    //negative value, so the 'abs' should do)
    std::vector<bool> keepEigenValue(N,true) ;
    if( m_eigenValueThreshold > 0 )
      for( size_t i=0; i<N; i++) 
	keepEigenValue[i] = std::abs(w[i]) > m_eigenValueThreshold ||
	  ( m_minEigenModeChisquare >0 && D[i]*D[i]/w[i] > m_minEigenModeChisquare ) ;

    double sumchisqaccepted(0), sumchisqrejected(0), maxchisq(0) ;
    size_t numrejected(0) ;
    std::multiset<double> chisqvalues ;
    for( size_t i=0; i<N; i++) {
      double thischisq = D[i]*D[i]/w[i] ;
      if( !keepEigenValue[i] ) {
	info() << "Rejecting eigenvalue: val = " << w[i]
	       << " chisq = " << thischisq << endreq ;
	++numrejected ;
	sumchisqrejected += thischisq ;
      } else {
	chisqvalues.insert(thischisq) ;
	if( maxchisq < thischisq ) maxchisq = thischisq ;
	sumchisqaccepted += thischisq ;
	if( std::abs(w[i]) < m_eigenValueThreshold )
	  info() << "Accepting eigenvalue: val = " << w[i]
		 << " chisq = " << thischisq << endreq ;
      }
    }

    solinfo.totalChisq = sumchisqaccepted ;

    info() << "Number / total chi2 of rejected eigenvalues: "
	     << numrejected << " " << sumchisqrejected << endreq ;
    info() << "Total chi2 of accepted eigenvalues: " <<  sumchisqaccepted << endreq ;
    info() << "Maximum chi2 of individual mode: " << maxchisq << endreq ;

    // because the lagrange constraints lead to negative ev, they also
    // give negative chi2 contributions. that's all fine for the total
    // chi2, but it leads to problems if you want to identify the
    // 'largest' chi2 contribution: such contributions may be
    // compensated by large negative contributions. Let's solve this
    // pragmatically:
    // - make a multiset of all chisq contributions
    // - while the smallest one is negative, remove the smallest and
    //   largest from the set, and reinsert the sum
    // - the largest remaining eigenvalue is the one we need;
    while( !chisqvalues.empty() && *(chisqvalues.begin())<0 && *(chisqvalues.rbegin())>0  ) {
      double a = *(chisqvalues.begin()) ; chisqvalues.erase(chisqvalues.begin()) ;
      double b = *(chisqvalues.rbegin()) ; chisqvalues.erase(--(chisqvalues.rbegin().base())) ;
      chisqvalues.insert( a + b ) ;
    }
    if( !chisqvalues.empty() ) solinfo.maxChisqEigenMode = *(chisqvalues.rbegin()) ;

    info() << "Maximum chi2 of individual mode after correcting for lagrange constraints: " 
	   << solinfo.maxChisqEigenMode << endreq ;
    
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
	    m_bigmatrix[i][j] += (z[k][i]*z[k][j]/(w[k]));
	}
    
  }
  else {
    error() << "inversion (diagonalization) of big matrix failed" <<endmsg;
    sc = StatusCode::FAILURE ;
  }
  
  return sc ;
}

