// $Id: CLHEPSolver.cpp,v 1.4 2009-08-16 14:16:23 wouter Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
#include "GaudiAlg/GaudiTool.h"

// Interface
#include "AlignmentInterfaces/IAlignSolvTool.h"

/** @class CLHEPSolver CLHEPSolver.h
 *  
 *
 *  @author Jan Amoraal
 *  @date   2007-07-24
 */

class CLHEPSolver : public GaudiTool, virtual public IAlignSolvTool {

public: 
  /// Standard constructor
  CLHEPSolver( const std::string& type, 
                const std::string& name,
                const IInterface* parent);

  virtual ~CLHEPSolver( ); ///< Destructor

  /// Solves Ax = b using gsl_linalg_SV_decomp (gsl_matrix * A, gsl_matrix * V, gsl_vector * S, gsl_vector * work)
  bool compute(AlSymMat& symMatrix, AlVec& vector) const;
  
protected:

private:
  bool   m_useSVD; 
  size_t m_numberOfPrintedEigenvalues ;
};

//-----------------------------------------------------------------------------
// Implementation file for class : CLHEPSolver
//
// 2007-07-24 : Jan Amoraal
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( CLHEPSolver )

#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/Matrix.h"

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CLHEPSolver::CLHEPSolver( const std::string& type,
                            const std::string& name,
                            const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IAlignSolvTool>(this);
  declareProperty("NumberOfPrintedEigenvalues", m_numberOfPrintedEigenvalues = 20 ) ;
  declareProperty("UseSVD", m_useSVD = true ) ;
}
//=============================================================================
// Destructor
//=============================================================================
CLHEPSolver::~CLHEPSolver() {} 

// fix CLHEP bug exposed in gcc 4.3
namespace CLHEP {
  double dot(const HepVector &v1,const HepVector &v2) ;
}

bool CLHEPSolver::compute(AlSymMat& symMatrix, AlVec& vector) const {
  
  // copy system into CLHEP matrices
  size_t size = symMatrix.size();
  CLHEP::HepSymMatrix A(size) ;
  CLHEP::HepVector b(size) ;
  for(size_t irow=0; irow<size; ++irow) {
    b(irow+1) = vector[irow] ;
    for(size_t icol=0; icol<=irow; ++icol)
      A.fast(irow+1,icol+1) = symMatrix[irow][icol] ;
  }

  clock_t starttime = clock();
  
  CLHEP::HepSymMatrix cov(size,0);
  CLHEP::HepVector delta(size,0) ;
  if(!m_useSVD) {
    int ierr ;
    cov = A ;
    cov.invert(ierr) ;
    delta = - (cov * b ) ;
  } else {
    info() << "Before CLHEP diagonalize." << endreq ;
    CLHEP::HepSymMatrix D = A ;
    CLHEP::HepMatrix U = CLHEP::diagonalize( &D ) ;
    info() << "After CLHEP diagonalize." << endreq ;
    //sold = U*sdiag*U.T.

    // how do I now get the eigenvalues? this cannot be the most
    // efficienct way ...  HepSymMatrix D =
    // //halfd2ChisqdAlpha2.similarityT( U ) ;

    CLHEP::HepVector eigenvector(size) ;
    std::vector<double> eigenvalues ;
    for(size_t imode=1; imode<=size; ++imode) {
      // get the relevant eigenvector
      for(size_t irow=1; irow<=size; ++irow) eigenvector(irow) = U(irow,imode) ;
      // calculate the eigenvalue
      double eigenvalue = A.similarity( eigenvector ) ;
      eigenvalues.push_back(eigenvalue) ;
      double evdotb = CLHEP::dot(b,eigenvector) ;
      delta += evdotb/eigenvalue * eigenvector ;
      // this is the time consuming part
      for(size_t irow=1; irow<=size; ++irow) 
	for(size_t icol=1; icol<=irow; ++icol) 
	  cov.fast(irow,icol) += eigenvector(irow) * eigenvector(icol) /eigenvalue ;
    }
    // dump the eigenvalues
    std::sort( eigenvalues.begin(), eigenvalues.end() ) ;
    std::ostringstream logmessage ;
    logmessage << "Smallest eigen values: [ " << std::setprecision(4) ;
    for( size_t ipar=0; ipar <m_numberOfPrintedEigenvalues; ++ipar)
      logmessage << eigenvalues[ipar] << ", " ;
    logmessage << "]" ;
    info() << logmessage.str() << endmsg ;
  }

  double dist = ( A * delta + b ).norm() ;
  info() << "distance to solution in solveCLHEP: " << dist << endreq ;
  clock_t stoptime = clock();
  info() << "time spent in solveCLHEP: " << (stoptime-starttime)/double(CLOCKS_PER_SEC) << endreq ;
  
  // now copy everything back
  for(size_t irow=0; irow<size; ++irow) {
    vector[irow] = delta(irow+1) ;
    for(size_t icol=0; icol<=irow; ++icol)
      symMatrix[irow][icol] = cov.fast(irow+1,icol+1) ;
  }

  return true ;
}
