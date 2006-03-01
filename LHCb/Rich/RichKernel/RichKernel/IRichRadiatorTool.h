// $Id: IRichRadiatorTool.h,v 1.1 2006-03-01 17:11:06 papanest Exp $
#ifndef RICHKERNEL_IRICHRADIATORTOOL_H
#define RICHKERNEL_IRICHRADIATORTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// from MathCore
#include "Kernel/Point3DTypes.h"
#include "Kernel/Vector3DTypes.h"

// from LHCbKernel
#include "Kernel/RichRadiatorType.h"

class RichRadIntersection;

static const InterfaceID IID_IRichRadiatorTool ( "IRichRadiatorTool", 1, 0 );

/** @class IRichRadiatorTool IRichRadiatorTool.h RichKernel/IRichRadiatorTool.h
 *
 *
 *  @author Antonis Papanestis
 *  @date   2006-03-01
 */
class IRichRadiatorTool : virtual public IAlgTool {
public:

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IRichRadiatorTool; }

  /**
   * Finds the intersections (entry/exit) with radiator. For multiple solids
   * there can be more than one intersections
   *
   * @return The number of intersections.
   * @retval Zero if there is no intersction.
   */
  virtual unsigned int intersections( const Gaudi::XYZPoint& globalPoint,
                                      const Gaudi::XYZVector& globalVector,
                                      Rich::RadiatorType radiator,
                                      std::vector<RichRadIntersection>&
                                      intersections ) const = 0;

protected:

private:

};
#endif // RICHKERNEL_IRICHRADIATORTOOL_H
