// $Id: DiagSolvTool.h,v 1.11 2010-03-02 15:55:26 wouter Exp $
#ifndef ALIGNSOLVTOOLS_DIAGSOLVTOOL_H 
#define ALIGNSOLVTOOLS_DIAGSOLVTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "AlignmentInterfaces/IAlignSolvTool.h"            // Interface

class AlSymMat;
class AlMat;
class AlVec;

/** @class DiagSolvTool DiagSolvTool.h AlignSolvTools/DiagSolvTool.h
 *  
 *
 *  @author Adlene Hicheur
 *  @date   2007-06
 */
class DiagSolvTool : public GaudiTool, virtual public IAlignSolvTool {
public: 
  /// Standard constructor
  DiagSolvTool( const std::string& type, 
		const std::string& name,
		const IInterface* parent);
  
  virtual ~DiagSolvTool( ); ///< Destructor

  ///< Call method to compute the solution, get symetric 
  /// matrix and vector in input and replaces them by 
  /// inverse matrix and solution vector in output.
  virtual bool compute(AlSymMat& ,AlVec&) const ; 

  // Solves the system Ax=b for x. Return A=A^{-1} and
  // b=x. Furthermore, returns the total chi2 (b^T*A^{-1}*b) and the
  // largest contribution to the chi2 from a single mode.
  virtual StatusCode compute(AlSymMat& A, AlVec& b, IAlignSolvTool::SolutionInfo&) const ;

  StatusCode initialize() ;
  //enum Solver {SPMINV, 
  //               DIAG, 
  //             MINRES};


protected:

private:

  //parameters
  size_t m_numberOfPrintedEigenvalues ;
  bool m_applyScaling ;
  double m_eigenValueThreshold ;
  double m_minEigenModeChisquare ;
  
  void MonitorDiag(AlMat& z, AlVec& w, AlVec& D, double scale);
  int SolvMinRes(AlSymMat& m_bigmatrix, AlVec& m_bigvector, AlVec& result);
};
#endif // ALIGNSOLVTOOLS_ALIGNSOLVTOOL_H
