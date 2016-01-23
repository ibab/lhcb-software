
//---------------------------------------------------------------------------------
/** @file IRichRadiatorTool.h
 *
 *  Header file for RICH tool interface : Rich::IRadiatorTool
 *
 *  @author Antonis Papanestis
 *  @date   2006-03-01
 */
//---------------------------------------------------------------------------------

#ifndef RICHKERNEL_IRICHRADIATORTOOL_H
#define RICHKERNEL_IRICHRADIATORTOOL_H 1

// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// from MathCore
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"

// from LHCbKernel
#include "Kernel/RichRadiatorType.h"
#include "Kernel/RichRadIntersection.h"

static const InterfaceID IID_IRichRadiatorTool ( "Rich::IRadiatorTool", 1, 0 );

namespace Rich
{

  /** @class IRadiatorTool IRichRadiatorTool.h RichKernel/IRichRadiatorTool.h
   *
   *  Interface to RICH radiator tool
   *
   *  @author Antonis Papanestis
   *  @date   2006-03-01
   */

  class IRadiatorTool : virtual public IAlgTool
  {

  public:

    /// Return the interface ID
    static const InterfaceID& interfaceID() { return IID_IRichRadiatorTool; }

    /** @brief Finds the intersections (entry/exit) with radiator.
     *
     * For multiple radiators (e.g. the aerogel tiles) there can be more than one intersections
     *
     * The intersections are sorted into order of increasing z
     *
     * @param globalPoint   The start point for the intersection extraplotion
     * @param globalVector  The direction vector for the intersection extraplotion
     * @param radiator      The radiator to find the intersections in
     * @param intersections The found intersections
     *
     * @return The number of intersections
     */
    virtual unsigned int intersections( const Gaudi::XYZPoint& globalPoint,
                                        const Gaudi::XYZVector& globalVector,
                                        const Rich::RadiatorType radiator,
                                        Rich::RadIntersection::Vector& intersections ) const = 0;

  };

}

#endif // RICHKERNEL_IRICHRADIATORTOOL_H
