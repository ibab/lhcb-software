// $Id: IAlignSolvTool.h,v 1.3 2007-11-21 13:07:46 janos Exp $
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
  
  virtual bool compute(AlSymMat& ,AlVec&) const = 0;


protected:

private:

};
#endif // ALIGNSOLVTOOLS_IALIGNSOLVTOOL_H
