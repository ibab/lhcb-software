#ifndef KERNEL_ICHECKTOOL_H
#define KERNEL_ICHECKTOOL_H 1

// Include files

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

/** @class ICheckTool ICheckTool.h
 *  Interface class for generic checking tools
 *
 *  @author Marco Cattaneo
 *  @date   2003-02-24
 */

// Declaration of  the interface ID
static const InterfaceID IID_ICheckTool( "ICheckTool", 0, 0 );


class ICheckTool : virtual public IAlgTool {
public:
  /// Static access to interface id
  static const InterfaceID& interfaceID() { return IID_ICheckTool; }

  /// Check the data
  virtual StatusCode check()=0;

};
#endif // KERNEL_ICHECKTOOL_H
