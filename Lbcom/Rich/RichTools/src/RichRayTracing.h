// $Id: RichRayTracing.h,v 1.2 2004-07-01 11:10:08 papanest Exp $
#ifndef RICHDETTOOLS_RICHRAYTRACING_H
#define RICHDETTOOLS_RICHRAYTRACING_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// Base class and interface
#include "RichKernel/RichToolBase.h"
#include "RichKernel/IRichRayTracing.h"

// RichKernel
#include "RichKernel/IRichMirrorSegFinder.h"
#include "RichKernel/BoostArray.h"
#include "RichKernel/Rich1DTabProperty.h"
#include "RichKernel/RichTraceMode.h"

// RichEvent
#include "RichEvent/RichGeomPhoton.h"

//CLHEP
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"

// LHCbKernel
#include "Kernel/RichSmartID.h"

// RichDet
#include "RichDet/DeRichSphMirror.h"
#include "RichDet/DeRichFlatMirror.h"
#include "RichDet/DeRichHPDPanel.h"


/** @class RichRayTracing RichRayTracing.h
 *
 *  Rich detector tool which traces photons to the photodetectors. Mirror
 *  segmentation is included.
 *
 *  @author Antonis Papanestis
 *  @date   2003-11-04
 */
class RichRayTracing : public RichToolBase,
                       virtual public IRichRayTracing {

public:

  /// Standard constructor
  RichRayTracing( const std::string& type,
                  const std::string& name,
                  const IInterface* parent);

  virtual ~RichRayTracing( ); ///< Destructor

  virtual StatusCode initialize();
  virtual StatusCode finalize  ();

  /// For a given detector, raytraces a given direction from a given point to
  /// the photo detectors. Returns the result in the form of a RichGeomPhoton
  virtual StatusCode traceToDetector ( Rich::DetectorType rich,
                                       const HepPoint3D& startPoint,
                                       const HepVector3D& startDir,
                                       RichGeomPhoton& photon,
                                       RichTraceMode mode = RichTraceMode(),
                                       Rich::Side fSide = Rich::top
                                       ) const;

  virtual StatusCode traceToDetectorWithoutEff( Rich::DetectorType rich,
                                                const HepPoint3D& position,
                                                const HepVector3D& direction,
                                                HepPoint3D& hiPosition,
                                                RichTraceMode mode = RichTraceMode(),
                                                Rich::Side fSide = Rich::top
                                                ) const;


  /// For a given detector, raytraces a given direction from a given point
  /// to the average photo detector plane. Returns the result in the form
  /// of a RichGeomPhoton
  virtual StatusCode intersectPDPanel ( Rich::DetectorType,
                                        const HepPoint3D&,
                                        const HepVector3D&,
                                        RichGeomPhoton& ) const;

  virtual StatusCode intersectPlane (const HepPoint3D& position,
                                     const HepVector3D& direction,
                                     const HepPlane3D& plane,
                                     HepPoint3D& intersection ) const;

  /// Reflect off a spherical mirror. Can be used for intersection
  StatusCode reflectSpherical ( HepPoint3D& position,
                                HepVector3D& direction,
                                const HepPoint3D& CoC,
                                double radius,
                                RichTraceMode mode = RichTraceMode() ) const;
private:

  // methods
  StatusCode reflectBothMirrors ( Rich::DetectorType rich,
                                  HepPoint3D& position,
                                  HepVector3D& direction,
                                  RichGeomPhoton& photon,
                                  RichTraceMode mode,
                                  Rich::Side fSide ) const;


  StatusCode reflectFlat ( HepPoint3D& position,
                           HepVector3D& direction,
                           const HepPlane3D& plane) const;

  // data

  /// Rich1 and Rich2
  DeRich* m_rich[Rich::NRiches];

  // photodetector panels
  typedef boost::array<DeRichHPDPanel*, 2> HPDPanelsPerRich;
  boost::array<HPDPanelsPerRich, 2> m_photoDetPanels;

  HepPoint3D m_nominalCoC[Rich::NRiches][2];
  HepPlane3D m_nominalFlatMirrorPlane[Rich::NRiches][2];
  double m_nomSphMirrorRadius[Rich::NRiches];

  IRichMirrorSegFinder* m_mirrorSegFinder;

};
#endif // RICHDETTOOLS_RICHRAYTRACING_H
