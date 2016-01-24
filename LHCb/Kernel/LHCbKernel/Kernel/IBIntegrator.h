#ifndef KERNEL_IBINTEGRATOR_H
#define KERNEL_IBINTEGRATOR_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"


static const InterfaceID IID_IBIntegrator ( "IBIntegrator", 2, 0 );

/** @class IBIntegrator IBIntegrator.h Kernel/IBIntegrator.h
 *  Interface class for field integral tool.
 *
 *  @author Rutger Hierck
 *  @date   2002-05-28
 */
class IBIntegrator : virtual public IAlgTool {
public:

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IBIntegrator; }

  /// Get the z of center and the total Bdl
  virtual StatusCode calculateBdlAndCenter( const Gaudi::XYZPoint& beginPoint,
                                            const Gaudi::XYZPoint& endPoint,
                                            const double tX,
                                            const double tY,
                                            double& zCenter,
                                            Gaudi::XYZVector& Bdl ) const = 0;

};
#endif // KERNEL_IBINTEGRATOR_H
