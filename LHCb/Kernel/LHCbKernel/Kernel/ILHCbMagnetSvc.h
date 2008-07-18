// $Id: ILHCbMagnetSvc.h,v 1.1 2008-07-18 16:12:10 cattanem Exp $
#ifndef KERNEL_ILHCBMAGNETSVC_H 
#define KERNEL_ILHCBMAGNETSVC_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/IMagneticFieldSvc.h"

static const InterfaceID IID_ILHCbMagnetSvc ( "ILHCbMagnetSvc", 1, 0 );

/** @class ILHCbMagnetSvc ILHCbMagnetSvc.h Kernel/ILHCbMagnetSvc.h
 *  LHCb extensions to the Gaudi IMagneticFieldSvc interface
 *
 *  @author Marco Cattaneo
 *  @date   2008-07-18
 */
class ILHCbMagnetSvc : virtual public IMagneticFieldSvc {

public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ILHCbMagnetSvc; }

  virtual double scaleFactor() const = 0; ///< Return the field scaling factor
  virtual bool   useRealMap()  const = 0; ///< True if using measured map

protected:

private:

};
#endif // KERNEL_ILHCBMAGNETSVC_H
