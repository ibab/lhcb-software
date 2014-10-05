// $Id: IAlignSolvTool.h,v 1.7 2009-07-06 14:06:44 wouter Exp $
#ifndef ALIGNSOLVTOOLS_IALIGNSOLVTOOL_H 
#define ALIGNSOLVTOOLS_IALIGNSOLVTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// from AlignKernel
#include "AlignKernel/AlVec.h"
#include "AlignKernel/AlSymMat.h"
#include "AlignKernel/AlMat.h"

static const InterfaceID IID_IAlignSolvTool ( "IAlignSolvTool", 1, 0 );

/** @class IAlignAlignTool IAlignAlignTool.h Kernel/IAlignAlignTool.h
 *  
 *
 *  @author Adlene Hicheur
 *  @date   2006-09-20
 */

class IAlignSolvTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IAlignSolvTool; }

  // simple struct that stores some info on solution
  struct SolutionInfo {
    double totalChisq ;
    double maxChisqEigenMode ;
    double minEigenValue ;
  } ;

  // Solves the system Ax=b for x. Returns A=A^{-1} and b=x.
  virtual bool compute(AlSymMat& A, AlVec&b) const = 0;
  
  // Solves the system Ax=b for x. Returns A=A^{-1}, b=x and S =
  // eigenvalues of A.
  virtual bool compute(AlSymMat& A, AlVec& b, AlVec& /*S*/) const {
    return compute(A,b) ;
  }

  // Solves the system Ax=b for x. Return A=A^{-1} and
  // b=x. Furthermore, returns an object with some characteristics of
  // system/solution.
  virtual StatusCode compute(AlSymMat& A, AlVec& b, SolutionInfo& /*info*/ ) const {
    return compute(A,b) ? StatusCode::SUCCESS : StatusCode::FAILURE ;
  }
  
protected:

private:

};
#endif // ALIGNSOLVTOOLS_IALIGNSOLVTOOL_H
