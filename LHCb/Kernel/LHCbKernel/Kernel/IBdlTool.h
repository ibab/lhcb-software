#ifndef KERNEL_IBDLTOOL_H
#define KERNEL_IBDLTOOL_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IBdlTool("IBdlTool", 1 , 0);

/** @class IBdlTool IBdlTool.h
 *  Interface class for field integral tool
 *
 *  @author Marco Cattaneo
 *  @author Mariusz Witek
 *  @date   2005-09-06
 *
*/
class IBdlTool : virtual public IAlgTool 
{

public:

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IBdlTool; }

  /// Actual operator function
  virtual double bdlIntegral( double ySlopeVelo,
                              double zOrigin,
                              double zVelo ) = 0;
  virtual double zBdlMiddle(  double ySlopeVelo,
                              double zOrigin,
                              double zVelo ) = 0;

};

#endif // KERNEL_IBDLTOOL_H
