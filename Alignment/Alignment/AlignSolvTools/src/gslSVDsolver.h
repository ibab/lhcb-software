// $Id: gslSVDsolver.h,v 1.4 2008-08-29 13:09:11 wouter Exp $
#ifndef GSLSVDSOLVER_H 
#define GSLSVDSOLVER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from GSL
#include "gsl/gsl_linalg.h"

// Interface
#include "AlignmentInterfaces/IAlignSolvTool.h"

/** @class gslSVDsolver gslSVDsolver.h
 *  
 *
 *  @author Jan Amoraal
 *  @date   2007-07-24
 */

class gslSVDsolver : public GaudiTool, virtual public IAlignSolvTool {

public: 
  /// Standard constructor
  gslSVDsolver( const std::string& type, 
                const std::string& name,
                const IInterface* parent);

  virtual ~gslSVDsolver( ); ///< Destructor

  /// Solves Ax = b using gsl_linalg_SV_decomp (gsl_matrix * A, gsl_matrix * V, gsl_vector * S, gsl_vector * work)
  bool compute(AlSymMat& symMatrix, AlVec& vector) const;
  
protected:

private:
  bool         m_svdJacobi;    ///< Use Jacobi SVD?
  double       m_svdEpsilon;   ///< SVD threshold. The singular values are stored in gsl_vector S.
  unsigned int m_nZero;        ///< remove the n smallest eigenvalues
  double m_eigenValueThreshold ;
  size_t m_numberOfPrintedEigenvalues ;
};

inline std::ostream& operator<<(std::ostream& lhs, const gsl_vector& rhs) {
  lhs << std::endl;
  lhs << "[ " << std::endl;
  for (unsigned(i) = 0u; i < rhs.size; ++i) { 
    lhs << " " << (*gsl_vector_const_ptr(&rhs, i)) << " ";
    lhs << std::endl;     
  }
  lhs << " ]" << std::endl;
  return lhs;
}

inline std::ostream& operator<<(std::ostream& lhs, const gsl_matrix& rhs) {
  lhs << std::endl;
  lhs << "[ " <<std::endl;
  for (unsigned(i) = 0u; i < rhs.size1; ++i) { 
    for (unsigned(j) = 0u; j < rhs.size2; ++j) {
      lhs << " " << (*gsl_matrix_const_ptr (&rhs, i, j)) << " ";
    }
  lhs << std::endl;     
  }
  lhs << " ]" << std::endl;
  return lhs;
}

#endif // GSLSVDSOLVER_H
