
//-----------------------------------------------------------------------------
/** @file IRichRayTracing.h
 *
 *  Header file for tool interface : Rich::IRayTracing
 *
 *  $Id: IRichRayTracing.h,v 1.28 2007-03-09 17:58:13 jonrob Exp $
 *
 *  @author Antonis Papanestis
 *  @date   2003-10-28
 */
//-----------------------------------------------------------------------------

#ifndef RICHKERNEL_IRICHRAYTRACING_H
#define RICHKERNEL_IRICHRAYTRACING_H 1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// from LHCbKernel
#include "Kernel/RichDetectorType.h"
#include "Kernel/RichSide.h"
#include "Kernel/RichTraceMode.h"

// from MathCore
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Plane3DTypes.h"

// forward decs
namespace LHCb
{
  class RichGeomPhoton;
}

/// Static Interface Identification
static const InterfaceID IID_IRichRayTracing( "Rich::IRayTracing", 1, 0 );

namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @class IRayTracing IRichRayTracing.h RichKernel/IRichRayTracing.h
   *
   *  A tool to trace photons (or similar) from a point all the way to
   *  the photo detectors.
   *
   *  @author Antonis Papanestis
   *  @date   2003-10-28
   */
  //-----------------------------------------------------------------------------

  class IRayTracing : public virtual IAlgTool
  {

  public:

    /** static interface identification
     *  @return unique interface identifier
     */
    static const InterfaceID& interfaceID() { return IID_IRichRayTracing; }

    /** For a given detector, ray-traces a given direction from a given point to
     *  the photo detectors. Returns the result in the form of a RichGeomPhoton
     *  which contains the full ray tracing information.
     *
     *  @param[in]  rich       The RICH detector
     *  @param[in]  startPoint The start point to use for the ray tracing
     *  @param[in]  startDir   The direction to ray trace from the start point
     *  @param[out] photon     The result of the raytracing, encapsulated as a RichGeomPhoton
     *  @param[in]  mode       The ray tracing mode configuration
     *  @param[in]  forcedSide If configured to do so, the forced side to use
     *
     *  @return Status of the ray tracing
     *  @retval StatusCode::SUCCESS Ray tracing was successful
     *  @retval StatusCode::FAILURE Ray tracing fell outside acceptance, as defined by mode
     */
    virtual StatusCode
    traceToDetector ( const Rich::DetectorType rich,
                      const Gaudi::XYZPoint& startPoint,
                      const Gaudi::XYZVector& startDir,
                      LHCb::RichGeomPhoton& photon,
                      const LHCb::RichTraceMode mode = LHCb::RichTraceMode(),
                      const Rich::Side forcedSide = Rich::top ) const = 0;

    /** For a given detector, raytraces a given direction from a given point to
     *  the photo detectors. Returns the result in the form of a RichGeomPhoton
     *
     *  @param[in]  rich        The RICH detector
     *  @param[in]  startPoint  The start point to use for the ray tracing
     *  @param[in]  startDir    The direction to ray trace from the start point
     *  @param[out] hitPosition The result of the tracing, the hit point on the HPD panel
     *  @param[in]  mode        The ray tracing mode configuration
     *  @param[in]  forcedSide  If configured to do so, the forced side to use
     *
     *  @return Status of the ray tracing
     *  @retval StatusCode::SUCCESS Ray tracing was successful
     *  @retval StatusCode::FAILURE Ray tracing fell outside acceptance, as defined by mode
     */
    virtual StatusCode
    traceToDetector ( const Rich::DetectorType rich,
                      const Gaudi::XYZPoint& startPoint,
                      const Gaudi::XYZVector& startDir,
                      Gaudi::XYZPoint& hitPosition,
                      const LHCb::RichTraceMode mode = LHCb::RichTraceMode(),
                      const Rich::Side forcedSide = Rich::top ) const = 0;

    /** Raytraces from a point in the detector panel back to the spherical mirror
     *  returning the mirror intersection point and the direction a track would have
     *  in order to hit that point in the detector panel.
     *
     *  @param[in]  startPoint The start point to use for the ray tracing
     *  @param[in]  startDir   The direction to ray trace from the start point
     *  @param[out] endPoint   The required mirror intersection point
     *  @param[out] endDir     The required track direction
     *
     *  @return Status of the ray tracing
     *  @retval StatusCode::SUCCESS Ray tracing was successful
     *  @retval StatusCode::FAILURE Ray tracing was unsuccessful
     */
    virtual StatusCode traceBackFromDetector ( const Gaudi::XYZPoint& startPoint,
                                               const Gaudi::XYZVector& startDir,
                                               Gaudi::XYZPoint& endPoint,
                                               Gaudi::XYZVector& endDir ) const = 0;

    /** Intersection a given direction, from a given point with a given plane.
     *
     *  @param[in]  position      The start point to use for the ray tracing
     *  @param[in]  direction     The direction to ray trace from the start point
     *  @param[in]  plane         The plane to intersect
     *  @param[out] intersection  The intersection point of the direction with the plane
     *
     *  @return Status of the ray tracing
     *  @retval StatusCode::SUCCESS Ray tracing was successful
     *  @retval StatusCode::FAILURE Ray tracing was unsuccessful
     */
    virtual StatusCode
    intersectPlane ( const Gaudi::XYZPoint& position,
                     const Gaudi::XYZVector& direction,
                     const Gaudi::Plane3D& plane,
                     Gaudi::XYZPoint& intersection ) const = 0;

    /** Reflect a given direction off a spherical mirror. Can be used for intersection.
     *
     *  @param[in,out] position   The start point to use for the ray tracing.
     *                            Afterwards gives the reflection point on the spherical mirror.
     *  @param[in,out] direction  The direction to ray trace from the start point.
     *                            Afterwards represents the reflection direction from the spherical mirror.
     *  @param[in] CoC        The centre of curvature of the spherical mirror
     *  @param[in] radius     The radius of curvature of the spherical mirror
     *
     *  @return StatusCode indicating if the ray tracing was succesful
     *  @retval StatusCode::SUCCESS Ray tracing was successful
     *  @retval StatusCode::FAILURE Ray tracing was unsuccessful
     */
    virtual StatusCode
    reflectSpherical ( Gaudi::XYZPoint& position,
                       Gaudi::XYZVector& direction,
                       const Gaudi::XYZPoint& CoC,
                       const double radius ) const = 0;

    /** Ray trace from given position in given direction off flat mirrors
     *
     *  @param[in,out] position  On input the start point. On output the reflection point
     *  @param[in,out] direction On input the starting direction. On output the reflected direction.
     *
     *  @return StatusCode indicating if the ray tracing was succesful
     *  @retval StatusCode::SUCCESS Ray tracing was successful
     *  @retval StatusCode::FAILURE Ray tracing was unsuccessful
     */
    virtual StatusCode
    reflectFlatPlane ( Gaudi::XYZPoint& position,
                       Gaudi::XYZVector& direction,
                       const Gaudi::Plane3D& plane ) const = 0;

  };

}

#endif // RICHKERNEL_IRICHRAYTRACING_H
