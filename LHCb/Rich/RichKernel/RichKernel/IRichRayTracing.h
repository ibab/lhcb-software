// $Id: IRichRayTracing.h,v 1.7 2004-07-15 15:47:19 jonrob Exp $
#ifndef RICHKERNEL_IRICHRAYTRACING_H
#define RICHKERNEL_IRICHRAYTRACING_H 1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// from LHCbKernel
#include "Kernel/RichDetectorType.h"
#include "Kernel/RichSide.h"

//local
#include "RichKernel/RichTraceMode.h"

// CLHEP
class HepPoint3D;
class HepVector3D;
class HepPlane3D;

// RichEvent
class RichGeomPhoton;


/** @class IRichRayTracing IRichRayTracing.h RichDetTools/IRichRayTracing.h
 *
 *  A tool to trace photons (or similar) from a point all the way to
 *  the photo detectors.
 *
 *  @author Antonis Papanestis
 *  @date   2003-10-28
 */

static const InterfaceID IID_IRichRayTracing( "IRichRayTracing", 1, 0 );

class IRichRayTracing : public virtual IAlgTool {

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
  traceToDetector ( Rich::DetectorType rich,
                    const HepPoint3D& startPoint,
                    const HepVector3D& startDir,
                    RichGeomPhoton& photon,
                    const RichTraceMode mode = RichTraceMode(),
                    Rich::Side forcedSide = Rich::top ) const = 0;

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
  traceToDetectorWithoutEff ( Rich::DetectorType rich,         
                              const HepPoint3D& startPoint,     
                              const HepVector3D& startDir,            
                              HepPoint3D& hitPosition,                 
                              const RichTraceMode mode = RichTraceMode(),
                              Rich::Side forcedSide = Rich::top ) const = 0;

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
   *  @retval StatusCode::FAILURE Ray tracing fell outside acceptance, as defined by mode
   */
  virtual StatusCode 
  intersectPDPanel ( Rich::DetectorType rich,
                     const HepPoint3D& point,
                     const HepVector3D& dir,  
                     RichGeomPhoton& photon ) const = 0;

  /** Intersection a given direction, from a given point with a given plane.
   *
   *  @param position      The start point to use for the ray tracing
   *  @param direction     The direction to ray trace from the start point
   *  @param plane         The plane to intersect
   *  @param intersection  The intersection point of the direction with the plane
   *
   *  @return Status of the ray tracing
   *  @retval StatusCode::SUCCESS Ray tracing was successful
   *  @retval StatusCode::FAILURE Ray tracing fell outside acceptance, as defined by mode
   */
  virtual StatusCode 
  intersectPlane ( const HepPoint3D& position, 
                   const HepVector3D& direction, 
                   const HepPlane3D& plane,     
                   HepPoint3D& intersection ) const = 0;

  /** Reflect a given direction off a spherical mirror. Can be used for intersection.
   *
   *  @param position   The start point to use for the ray tracing
   *  @param direction  The direction to ray trace from the start point
   *  @param CoC        The centre of curvature of the spherical mirror
   *  @param radius     The radius of curvature of the spherical mirror
   *
   *  @return StatusCode indicating if the ray tracing was succesful
   */
  virtual StatusCode 
  reflectSpherical ( HepPoint3D& position,        
                     HepVector3D& direction,            
                     const HepPoint3D& CoC,                  
                     const double radius ) const = 0;

};

#endif // RICHKERNEL_IRICHRAYTRACING_H
