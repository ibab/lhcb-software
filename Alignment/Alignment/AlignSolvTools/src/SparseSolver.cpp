// $Id: SparseSolver.cpp,v 1.3 2009-08-16 14:16:24 wouter Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
#include "GaudiAlg/GaudiTool.h"

// Interface
#include "AlignmentInterfaces/IAlignSolvTool.h"

/** @class SparseSolver SparseSolver.h
 *  
 *
 *  @author Wouter Hulsbergen
 *  @date   2007-07-24
 */

class SparseSolver : public GaudiTool, virtual public IAlignSolvTool {

public: 
  /// Standard constructor
  SparseSolver( const std::string& type, 
                const std::string& name,
                const IInterface* parent);

  virtual ~SparseSolver( ); ///< Destructor

  /// Solves Ax = b using gsl_linalg_SV_decomp (gsl_matrix * A, gsl_matrix * V, gsl_vector * S, gsl_vector * work)
  bool compute(AlSymMat& symMatrix, AlVec& vector) const;
  
private:
};

//-----------------------------------------------------------------------------
// Implementation file for class : SparseSolver
//
// 2007-07-24 : Jan Amoraal
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( SparseSolver )

#include "TMatrixDSym.h"
#include "TVectorD.h"
#include "TMatrixDSparse.h"
#include "TDecompSparse.h"

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SparseSolver::SparseSolver( const std::string& type,
			const std::string& name,
			const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IAlignSolvTool>(this);
}
//=============================================================================
// Destructor
//=============================================================================
SparseSolver::~SparseSolver() {} 

bool SparseSolver::compute(AlSymMat& symMatrix, AlVec& vector) const 
{
  // create a sparce matrix. I think that this is most efficient via
  // TMatrix, because the structure of the TSparseMatrix isn't
  // entirely self evident.

  size_t dim = symMatrix.size() ;
  TMatrixDSym tsymMatrix( dim ) ; 
  TVectorD tvector( dim ) ;
  for( size_t irow=0; irow<dim; ++irow ) {
    tvector(irow) = vector(irow) ;
    for( size_t icol=0; icol<=irow; ++icol )
      tsymMatrix(irow,icol) = symMatrix.fast(irow,icol) ;
  }
  
  TMatrixDSparse sparsematrix( tsymMatrix ) ;
  info() << "Created sparse matrix. Number of non-zero elements is "
	 << sparsematrix.NonZeros() << " out of "
	 << dim*dim << endreq ;
  
  // solve
  int verbose(0) ;
  TDecompSparse decompsparse( sparsematrix, verbose ) ;
  bool ierr = decompsparse.Solve( tvector ) ;

  info() << "TDecompSparse::Solve returns: " << ierr << endreq ;

  // now copy the result back
  for( size_t irow=0; irow<dim; ++irow )
    vector(irow) = tvector(irow) ;

  // fill something arbitrary for the covariance matrix
  AlSymMat tmp(symMatrix.size()) ;
  for(size_t irow = 0; irow <tmp.size(); ++irow) {
    double c = symMatrix.fast(irow,irow) ;
    tmp.fast(irow,irow) = c != 0 ? 1/c : 0 ;
  }
  symMatrix = tmp ;
  return ierr ? false : true ;
}
