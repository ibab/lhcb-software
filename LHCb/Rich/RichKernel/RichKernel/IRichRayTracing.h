// $Id: IRichRayTracing.h,v 1.3 2004-07-01 11:12:09 papanest Exp $
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
 *  A tool to trace photons (or similar) from a point all the way to
 *  the photo detectors.
 *
 *  @author Antonis Papanestis
 *  @date   2003-10-28
 */

static const InterfaceID IID_IRichRayTracing( "IRichRayTracing", 1, 0 );

class IRichRayTracing : public virtual IAlgTool {

public:

  static const InterfaceID& interfaceID() { return IID_IRichRayTracing; }

  /// For a given detector, raytraces a given direction from a given point to
  /// the photo detectors. Returns the result in the form of a RichGeomPhoton
  virtual StatusCode traceToDetector ( Rich::DetectorType rich,
                                       const HepPoint3D& startPoint,
                                       const HepVector3D& startDir,
                                       RichGeomPhoton& photon, 
                                       RichTraceMode mode = RichTraceMode(),
                                       Rich::Side forcedSide = Rich::top
                                       ) const = 0;

  /// For a given detector, raytraces a given direction from a given point to
  /// the average photon detector plane (no HPD acceptance). Result is a HepPoint3D
  virtual StatusCode traceToDetectorWithoutEff( Rich::DetectorType rich,
                                                const HepPoint3D& startPoint,
                                                const HepVector3D& startDir,
                                                HepPoint3D& hitPosition,
                                                RichTraceMode mode = RichTraceMode(),
                                                Rich::Side forcedSide = Rich::top
                                                ) const = 0;

  /// For a given detector, ray traces a given direction from a given point
  /// to the average photo detector plane. Returns the result in the form
  /// of a RichGeomPhoton
  virtual StatusCode intersectPDPanel ( Rich::DetectorType,
                                        const HepPoint3D&,
                                        const HepVector3D&,
                                        RichGeomPhoton& ) const = 0;

  /// Intersection a given direction, from a given point with a given plane.
  /// Result is a HepPoint3D.
  virtual StatusCode intersectPlane ( const HepPoint3D& position,
                                      const HepVector3D& direction,
                                      const HepPlane3D& plane,
                                      HepPoint3D& intersection ) const = 0;

  /// Reflect off a spherical mirror. Can be used for intersection.
  virtual StatusCode reflectSpherical ( HepPoint3D& position,
                                        HepVector3D& direction,
                                        const HepPoint3D& CoC,
                                        double radius,
                                        RichTraceMode mode = RichTraceMode() ) const = 0;
};

#endif // RICHKERNEL_IRICHRAYTRACING_H
