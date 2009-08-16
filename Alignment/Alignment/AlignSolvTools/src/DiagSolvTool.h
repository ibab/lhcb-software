// $Id: DiagSolvTool.h,v 1.10 2009-08-16 14:16:23 wouter Exp $
#ifndef ALIGNSOLVTOOLS_DIAGSOLVTOOL_H 
#define ALIGNSOLVTOOLS_DIAGSOLVTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiAlg/GaudiTupleTool.h"
#include "GaudiKernel/NTuple.h"

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
class DiagSolvTool : public GaudiTupleTool, virtual public IAlignSolvTool {
public: 
  /// Standard constructor
  DiagSolvTool( const std::string& type, 
		const std::string& name,
		const IInterface* parent);
  
  virtual ~DiagSolvTool( ); ///< Destructor

  ///< Call method to compute the solution, get symetric 
  /// matrix and vector in input and replaces them by 
  /// inverse matrix and solution vector in output.
  virtual bool compute(AlSymMat& ,AlVec&) const; 

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
  size_t par_modcut;
  bool par_writentp;
  
  //Monitoring Ntuple variables
  NTuple::Tuple* m_tuple;
  //  NTuple::Item<float> nteig_tx,nteig_ty,nteig_tz,nteig_rx,nteig_ry,nteig_rz;
  //  NTuple::Item<float> nteig_mode,nteig_eigval,nteig_eigmod,nteig_erreigmod;
  float nteig_tx,nteig_ty,nteig_tz,nteig_rx,nteig_ry,nteig_rz;
  float nteig_mode,nteig_eigval,nteig_eigmod,nteig_erreigmod;

  int SolvDiag(AlSymMat& m_bigmatrix, AlVec& m_bigvector);
  void MonitorDiag(AlMat& z, AlVec& w, AlVec& D, double scale);
  int SolvMinRes(AlSymMat& m_bigmatrix, AlVec& m_bigvector, AlVec& result);
  void Precond(AlSymMat& M, AlVec& r, AlVec& c, bool equIn);
  double findMax(AlSymMat& M);
  


};
#endif // ALIGNSOLVTOOLS_ALIGNSOLVTOOL_H
