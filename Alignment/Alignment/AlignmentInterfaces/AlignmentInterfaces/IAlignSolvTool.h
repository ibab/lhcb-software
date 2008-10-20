// $Id: IAlignSolvTool.h,v 1.4 2008-10-20 10:18:59 wouter Exp $
#ifndef ALIGNSOLVTOOLS_IALIGNSOLVTOOL_H 
#define ALIGNSOLVTOOLS_IALIGNSOLVTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// from SolvKernel
#include "SolvKernel/AlVec.h"
#include "SolvKernel/AlSymMat.h"
#include "SolvKernel/AlMat.h"

static const InterfaceID IID_IAlignSolvTool ( "IAlignSolvTool", 1, 0 );

/** @class IAlignSolvTool IAlignSolvTool.h Kernel/IAlignSolvTool.h
 *  
 *
 *  @author Adlene Hicheur
 *  @date   2006-09-20
 */

class IAlignSolvTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IAlignSolvTool; }
  
  // Solves the system Ax=b for x. Returns A=A^{-1} and b=x.
  virtual bool compute(AlSymMat& A, AlVec&b) const = 0;
  
  // Solves the system Ax=b for x. Returns A=A^{-1}, b=x and S =
  // eigenvalues of A.
  virtual bool compute(AlSymMat& A, AlVec& b, AlVec& S) const {
    return compute(A,b) ;
  }
  
protected:

private:

};
#endif // ALIGNSOLVTOOLS_IALIGNSOLVTOOL_H
