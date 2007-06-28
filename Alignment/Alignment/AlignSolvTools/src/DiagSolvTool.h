// $Id: DiagSolvTool.h,v 1.1 2007-06-28 16:33:56 ahicheur Exp $
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

  virtual bool compute(AlSymMat& ,AlVec&); ///< Call method to compute the solution, get symetric matrix and vector in input and replaces them by inverse matrix and solution vector in output.

  //enum Solver {SPMINV, 
  //               DIAG, 
  //             MINRES};


protected:

private:

  //parameters

  double m_scale;
  int par_modcut;

  int SolvDiag(AlSymMat& m_bigmatrix, AlVec& m_bigvector);
  void MonitorDiag(AlMat& z, AlVec& w, AlVec& D);
  int SolvMinRes(AlSymMat& m_bigmatrix, AlVec& m_bigvector, AlVec& result);
  void Precond(AlSymMat& M, AlVec& r, AlVec& c, bool equIn);
  double findMax(AlSymMat& M);
  


};
#endif // ALIGNSOLVTOOLS_ALIGNSOLVTOOL_H
