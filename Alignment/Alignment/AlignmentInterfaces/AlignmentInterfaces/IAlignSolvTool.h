// $Id: IAlignSolvTool.h,v 1.2 2007-05-10 13:27:17 jblouw Exp $
#ifndef ALIGNSOLVTOOLS_IALIGNSOLVTOOL_H 
#define ALIGNSOLVTOOLS_IALIGNSOLVTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"


class AlSymMat;
class AlMat;
class AlVec;


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
  static const InterfaceID& interfaceID() { 

    return IID_IAlignSolvTool;    
    
  }
virtual bool compute(AlSymMat& ,AlVec&) = 0;


protected:

private:

};
#endif // ALIGNSOLVTOOLS_IALIGNSOLVTOOL_H
