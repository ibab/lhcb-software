// $Id: ILumiTool.h,v 1.1 2003-10-06 16:16:39 cattanem Exp $
#ifndef DIGIALG_ILUMITOOL_H 
#define DIGIALG_ILUMITOOL_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

/** @class ILumiTool ILumiTool.h DigiAlg/ILumiTool.h
 *  Interface class for Luminosity tool. 
 *
 *  @author Marco Cattaneo
 *  @date   2003-09-29
 */

// Declaration of the interface ID. 
static const InterfaceID IID_ILumiTool("ILumiTool", 1, 0); 

class ILumiTool : virtual public IAlgTool {
public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_ILumiTool; }

  /// Return number of interactions 
  virtual StatusCode numInteractions( int& number ) = 0;
};
#endif // DIGIALG_ILUMITOOL_H
