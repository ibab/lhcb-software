// $Id: MA27Solver.cpp,v 1.1 2008-04-10 20:21:22 wouter Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
#include "GaudiAlg/GaudiTool.h"

// Interface
#include "AlignmentInterfaces/IAlignSolvTool.h"

/** @class MA27Solver MA27Solver.h
 *  
 *
 *  @author Jan Amoraal
 *  @date   2007-07-24
 */

class MA27Solver : public GaudiTool, virtual public IAlignSolvTool {

public: 
  /// Standard constructor
  MA27Solver( const std::string& type, 
                const std::string& name,
                const IInterface* parent);

  virtual ~MA27Solver( ); ///< Destructor

  /// Solves Ax = b using gsl_linalg_SV_decomp (gsl_matrix * A, gsl_matrix * V, gsl_vector * S, gsl_vector * work)
  bool compute(AlSymMat& symMatrix, AlVec& vector) const;
  
private:
};

//-----------------------------------------------------------------------------
// Implementation file for class : MA27Solver
//
// 2007-07-24 : Jan Amoraal
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( MA27Solver );

#include "SolvKernel/AlSparseSymMat.h"

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MA27Solver::MA27Solver( const std::string& type,
			const std::string& name,
			const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IAlignSolvTool>(this);
}
//=============================================================================
// Destructor
//=============================================================================
MA27Solver::~MA27Solver() {} 

bool MA27Solver::compute(AlSymMat& symMatrix, AlVec& vector) const 
{
  // create a sparce matrix
  AlSparseSymMat m(symMatrix) ;
  info() << "Created sparse matrix. Number of non-zero elements is "
	 << m.nele() << " out of " << m.size()*(m.size()+1)/2 << endreq ;
  
  int ierr = m.MA27Solve(vector) ;
  // need to do something with the covariance matrix. Because we don't
  // have a clue what it is, we simply take the diagonal and invert each element ;
  info() << "MA27 returns: " << ierr << endreq ;

  AlSymMat tmp(symMatrix.size()) ;
  for(size_t irow = 0; irow <tmp.size(); ++irow) {
    double c = symMatrix.fast(irow,irow) ;
    tmp.fast(irow,irow) = c != 0 ? 1/c : 0 ;
  }
  symMatrix = tmp ;
  return ierr ? false : true ;
}
