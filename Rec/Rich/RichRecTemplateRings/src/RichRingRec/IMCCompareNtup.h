// $Id: IMCCompareNtup.h,v 1.1.1.1 2009-03-04 12:01:45 jonrob Exp $
#ifndef RICHRINGREC_IMCCOMPARENTUP_H 
#define RICHRINGREC_IMCCOMPARENTUP_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IMCCompareNtup ( "IMCCompareNtup", 1, 0 );

/** @class IMCCompareNtup IMCCompareNtup.h RichRingRec/IMCCompareNtup.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2007-06-28
 */
class IMCCompareNtup : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IMCCompareNtup; }


protected:

private:

};
#endif // RICHRINGREC_IMCCOMPARENTUP_H
