
//---------------------------------------------------------------------------------
/** @file IRichRadiatorTool.h
 *
 *  Header file for RICH tool interface : IRichRadiatorTool
 *
 *  $Id: IRichRadiatorTool.h,v 1.4 2006-03-24 13:44:05 jonrob Exp $
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
#include "Kernel/Point3DTypes.h"
#include "Kernel/Vector3DTypes.h"

// from LHCbKernel
#include "Kernel/RichRadiatorType.h"

//class RichRadIntersection;
#include "RichDet/RichRadIntersection.h"

static const InterfaceID IID_IRichRadiatorTool ( "IRichRadiatorTool", 1, 0 );

/** @class IRichRadiatorTool IRichRadiatorTool.h RichKernel/IRichRadiatorTool.h
 *
 *  Interface to RICH radiator tool
 *  
 *  @author Antonis Papanestis
 *  @date   2006-03-01
 */

class IRichRadiatorTool : virtual public IAlgTool 
{

public:

  /// Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IRichRadiatorTool; }

  /** @brief Finds the intersections (entry/exit) with radiator. 
   * 
   * For multiple solids there can be more than one intersections
   *
   * @return The number of intersections.
   * @retval Zero if there are no intersction.
   */
  virtual unsigned int intersections( const Gaudi::XYZPoint& globalPoint,
                                      const Gaudi::XYZVector& globalVector,
                                      const Rich::RadiatorType radiator,
                                      std::vector<RichRadIntersection>&
                                      intersections ) const = 0;

};

#endif // RICHKERNEL_IRICHRADIATORTOOL_H
