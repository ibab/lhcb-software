
//-----------------------------------------------------------------------------
/** @file IRichRayTracing.h
 *
 *  Header file for tool interface : IRichRayTracing
 *
 *  $Id: IRichRayTracing.h,v 1.20 2006-04-06 14:13:54 jonrob Exp $
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
#include "Kernel/Point3DTypes.h"
#include "Kernel/Vector3DTypes.h"
#include "Kernel/Plane3DTypes.h"

// forward decs
namespace LHCb
{
  class RichGeomPhoton;
}

/// Static Interface Identification
static const InterfaceID IID_IRichRayTracing( "IRichRayTracing", 1, 0 );

//-----------------------------------------------------------------------------
/** @class IRichRayTracing IRichRayTracing.h RichKernel/IRichRayTracing.h
 *
 *  A tool to trace photons (or similar) from a point all the way to
 *  the photo detectors.
 *
 *  @author Antonis Papanestis
 *  @date   2003-10-28
 */
//-----------------------------------------------------------------------------

class IRichRayTracing : public virtual IAlgTool 
{

public:

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() { return IID_IRichRayTracing; }

  /** For a given detector, raytraces a given direction from a given point to
   *  the photo detectors. Returns the result in the form of a RichGeomPhoton.
   *
   *  @param rich       The RICH detector
   *  @param startPoint The start point to use for the ray tracing
   *  @param startDir   The direction to ray trace from the start point
   *  @param photon     The result of the raytracing, encapsulated as a RichGeomPhoton
   *  @param mode       The ray tracing mode configuration
   *  @param forcedSide If configured to do so, the forced side to use
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
                    const RichTraceMode mode = RichTraceMode(),
                    const Rich::Side forcedSide = Rich::top ) const = 0;

  /** For a given detector, raytraces a given direction from a given point to
   *  the average photon detector plane (no HPD acceptance).
   *
   *  @param rich        The RICH detector
   *  @param startPoint  The start point to use for the ray tracing
   *  @param startDir    The direction to ray trace from the start point
   *  @param hitPosition The result of the tracing, the hit point on the HPD panel
   *  @param mode        The ray tracing mode configuration
   *  @param forcedSide  If configured to do so, the forced side to use
   *
   *  @return Status of the ray tracing
   *  @retval StatusCode::SUCCESS Ray tracing was successful
   *  @retval StatusCode::FAILURE Ray tracing fell outside acceptance, as defined by mode
   */
  virtual StatusCode
  traceToDetectorWithoutEff ( const Rich::DetectorType rich,
                              const Gaudi::XYZPoint& startPoint,
                              const Gaudi::XYZVector& startDir,
                              Gaudi::XYZPoint& hitPosition,
                              const RichTraceMode mode = RichTraceMode(),
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


  /** For a given detector, ray traces a given direction from a given point
   *  to the average photo detector plane. Returns the result in the form
   *  of a RichGeomPhoton
   *
   *  @param rich     The RICH detector
   *  @param point    The start point to use for the ray tracing
   *  @param dir      The direction to ray trace from the start point
   *  @param photon   The result of the raytracing, encapsulated as a RichGeomPhoton
   *
   *  @return Status of the ray tracing
   *  @retval StatusCode::SUCCESS Ray tracing was successful
   *  @retval StatusCode::FAILURE Ray tracing was unsuccessful
   */
  virtual StatusCode
  intersectPDPanel ( const Rich::DetectorType rich,
                     const Gaudi::XYZPoint& point,
                     const Gaudi::XYZVector& dir,
                     LHCb::RichGeomPhoton& photon ) const = 0;

  /** Intersection a given direction, from a given point with a given plane.
   *
   *  @param position      The start point to use for the ray tracing
   *  @param direction     The direction to ray trace from the start point
   *  @param plane         The plane to intersect
   *  @param intersection  The intersection point of the direction with the plane
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
   *  @param position   The start point to use for the ray tracing
   *  @param direction  The direction to ray trace from the start point
   *  @param CoC        The centre of curvature of the spherical mirror
   *  @param radius     The radius of curvature of the spherical mirror
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

};

#endif // RICHKERNEL_IRICHRAYTRACING_H
