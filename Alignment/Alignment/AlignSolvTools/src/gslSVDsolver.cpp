// $Id: gslSVDsolver.cpp,v 1.11 2010-05-03 14:31:03 wouter Exp $
// Include files 

#include <cmath>

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "gslSVDsolver.h"

//-----------------------------------------------------------------------------
// Implementation file for class : gslSVDsolver
//
// 2007-07-24 : Jan Amoraal
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( gslSVDsolver )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
gslSVDsolver::gslSVDsolver( const std::string& type,
                            const std::string& name,
                            const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IAlignSolvTool>(this);
  declareProperty("SVDJacobi" ,           m_svdJacobi  = false);
  declareProperty("SVDEpsilon",           m_svdEpsilon = 0.0  );
  declareProperty("SVDSetSmallestToZero", m_nZero      = 0    );
  declareProperty("EigenValueThreshold", m_eigenValueThreshold = -1 ) ;
  declareProperty( "NumberOfPrintedEigenvalues", m_numberOfPrintedEigenvalues = 20 ) ;
}
//=============================================================================
// Destructor
//=============================================================================
gslSVDsolver::~gslSVDsolver() {} 


bool gslSVDsolver::compute(AlSymMat& symMatrix, AlVec& vector,
			   AlVec& evvector) const {
  
  size_t size = symMatrix.size();

  debug() << "Size of AlSymMat A = " << size << endmsg;
  /// Allocate matrix A
  gsl_matrix* matrixA = gsl_matrix_alloc(size, size);
  debug() << "Size of gsl_matrix A = " << matrixA->size1 << endmsg;
  /// Fill matrix A
  for (unsigned i = 0u; i < size; ++i) {
    for (unsigned j = 0u; j < size; ++j) {
      debug() << "Element (i,j) of AlSymMat A = " << symMatrix[i][j] << endmsg;
      gsl_matrix_set(matrixA, i, j, symMatrix[i][j]);
      debug() << "Element (i,j) of gsl_matrix A = " << gsl_matrix_get(matrixA, i, j) << endmsg;
    }
  }

  /// Allocate required matrix, vector and workspace
  gsl_matrix* matrixV = gsl_matrix_alloc(size, size);
  gsl_vector* vectorS = gsl_vector_alloc(size);
  gsl_vector* vectorW = gsl_vector_alloc(size);
  gsl_matrix* matrixU = matrixA ;

  debug() << "==> Matrix A  = " << (*matrixA) << endmsg;
 
  debug() << "Factorising matrix A" << endmsg;
  /// Factorise A into the SVD A = USV^T. Note matrix A is replaced with matrix U.
  /// GSL returns 0 if successful
  int factorised = 1;
  if (!m_svdJacobi) {
    info() << "==> SVD standard" << endmsg;
    factorised = gsl_linalg_SV_decomp(matrixA, matrixV, vectorS, vectorW);
  } else {
    info() << "==> SVD Jacobi" << endmsg;
    factorised = gsl_linalg_SV_decomp_jacobi(matrixA, matrixV, vectorS);
  }
  if (factorised != 0) {
    error() << "==> Couldn't factorise  matrix" << endmsg;
    return false;
  }
  debug() << "Done factorising matrix A" << endmsg;

  debug() << "==> Matrix U  = " << (*matrixA) << endmsg;
  debug() << "==> Vector S  = " << (*vectorS) << endmsg;
  debug() << "==> Matrix V  = " << (*matrixV) << endmsg;

  {
    // print the eigenvalues, but make sure to insert minus signs where needed
    std::vector<double> eigenvalues ;
    eigenvalues.reserve(size) ;
    for(size_t ipar = 0; ipar<size; ++ipar) {
      // A = U S V^T
      // take inner product of column of U with row of V^T
      double prod(0) ;
      for(size_t k=0; k<size; ++k)
	prod += gsl_matrix_get(matrixA,k,ipar) * gsl_matrix_get(matrixV,k,ipar) ;
      eigenvalues.push_back( (prod > 0 ? 1 : -1) * gsl_vector_get(vectorS,ipar)) ;
    }
    std::sort( eigenvalues.begin(), eigenvalues.end() ) ;
    std::ostringstream logmessage ;
    logmessage << "Smallest eigen values: [ " << std::setprecision(4) ;
    for( size_t ipar=0; ipar <m_numberOfPrintedEigenvalues && ipar<size; ++ipar)
      logmessage << eigenvalues[ipar] << ", " ;
    logmessage << "]" ;
    info() << logmessage.str() << endmsg ;
    for(size_t ipar = 0; ipar<size; ++ipar) 
      evvector(ipar) = eigenvalues[ipar] ;
  }
  
  /// Regularise by zeroing singular values below threshold
  if (m_svdEpsilon > 0) {
    /// Threshold is epsilon times max singular value
    double threshold = m_svdEpsilon * (*gsl_vector_const_ptr(vectorS, 0));
    for (unsigned i = 0u; i < size; ++i) {
      double* s = gsl_vector_ptr(vectorS, i);
      if ((*s) < threshold) (*s) = 0; 
    }
  }
  if (m_nZero > 0) { /// regularise by removing the m_nzero smallest eigenvalues
    for (unsigned(i) = 0; i < m_nZero; ++i) {
       (*gsl_vector_ptr(vectorS, size-1-i)) = 0;
    }
  }
  size_t numremoved(0) ;
  if(m_eigenValueThreshold>0) {
    info() << "Removing eigenvalues smaller than " << m_eigenValueThreshold << endmsg ;
    for (unsigned i = 0u; i < size; ++i) {
      double* s = gsl_vector_ptr(vectorS, i);
      if (std::abs(*s) < m_eigenValueThreshold) {
	(*s) = 0; 
	++numremoved ;
      }
    }
  }
  
  for (unsigned i = 0u; i < size; ++i) {
    double* s = gsl_vector_ptr(vectorS, i);
    if( *s < 0) {
      warning() << "Negative eigenvalue: " << i << " " << *s << endreq ;
    }
  }


  info() << "Number of removed eigenvalues = " << numremoved << endreq ;
  debug() << "==> Regularised Vector S  = " << (*vectorS) << endmsg;
  
  // Replace symMatrix with its inverse (the covariance matrix)
  for(unsigned irow=0; irow<size; ++irow)
    for(unsigned int icol=0; icol<=irow; ++icol) 
      symMatrix[irow][icol] = 0 ;
  for(unsigned int k=0; k<size; ++k) {
    double s = gsl_vector_get(vectorS,k) ;
    if( s>0 ) {
      for(unsigned irow=0; irow<size; ++irow)
	for(unsigned int icol=0; icol<=irow; ++icol) {
	  // honestly: don't have a clue about the order of U and V
	  // here, but they should be identical for our symmetric
	  // systems
	  symMatrix[irow][icol] +=
	    gsl_matrix_get(matrixV, irow, k) * gsl_matrix_get(matrixU, icol, k) / s ;
	}
    }
  }

  // Now calculate the solution
  gsl_vector* vectorB = gsl_vector_alloc(size);
  for (unsigned(i) = 0; i < size; ++i) gsl_vector_set(vectorB, i, vector[i]);
  gsl_vector* vectorX = gsl_vector_alloc(size);
  /// Solve Ax = b
  int solved = gsl_linalg_SV_solve(matrixA, matrixV, vectorS, vectorB, vectorX);
  if (solved != 0) {
    error() << "==> Couldn't solve system Ax=b" << endmsg;
    return false;
  }

  /// Fill AlVec
  for (unsigned(i) = 0; i < size; ++i) vector[i] = (*gsl_vector_const_ptr(vectorX, i));

  debug() << "==> Vector x = " << (*vectorX) << endmsg;
  
  
  /// free gsl vector and matrices
  gsl_matrix_free(matrixA);
  gsl_matrix_free(matrixV);
  gsl_vector_free(vectorS);
  gsl_vector_free(vectorW);
  gsl_vector_free(vectorB);
  gsl_vector_free(vectorX);

  return true;
}

  
//=============================================================================
