// $Id: AlignSolvTool.h,v 1.5 2007-05-11 15:41:31 ahicheur Exp $
#ifndef ALIGNSOLVTOOLS_ALIGNSOLVTOOL_H 
#define ALIGNSOLVTOOLS_ALIGNSOLVTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"


#include "AlignmentInterfaces/IAlignSolvTool.h"            // Interface

class AlSymMat;
class AlMat;
class AlVec;

/** @class AlignSolvTool AlignSolvTool.h AlignSolvTools/AlignSolvTool.h
 *  
 *
 *  @author Adlene Hicheur
 *  @date   2006-09-27
 */
class AlignSolvTool : public GaudiTool, virtual public IAlignSolvTool {
public: 
  /// Standard constructor
  AlignSolvTool( const std::string& type, 
                 const std::string& name,
                 const IInterface* parent);

  virtual ~AlignSolvTool( ); ///< Destructor

  virtual bool compute(AlSymMat& ,AlVec&); ///< Call method to compute the solution, get symetric matrix and vector in input and replaces them by inverse matrix and solution vector in output.

  //enum Solver {SPMINV, 
  //               DIAG, 
  //             MINRES};


protected:

private:

  //parameters
  int m_method;
  double m_scale;
  
  int SolvSpmInv(AlSymMat& M, AlVec& B);
  
  int SolvDiag(AlSymMat& m_bigmatrix, AlVec& m_bigvector);
  void MonitorDiag(AlMat& z, AlVec& w, AlVec& D);
  int SolvMinRes(AlSymMat& m_bigmatrix, AlVec& m_bigvector, AlVec& result);
  void Precond(AlSymMat& M, AlVec& r, AlVec& c, bool equIn);
  double findMax(AlSymMat& M);
  


};
#endif // ALIGNSOLVTOOLS_ALIGNSOLVTOOL_H
