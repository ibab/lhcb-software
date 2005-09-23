
//-----------------------------------------------------------------------------
/** @file RichRayTracingAllSph.h
 *
 *  Header file for tool : RichDetParameters
 *
 *  CVS History :
 *  $Id: RichRayTracingAllSph.h,v 1.1 2005-09-23 15:48:33 papanest Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2004-03-29
 */
//-----------------------------------------------------------------------------

#ifndef RICHTOOLS_RICHRAYTRACINGALLSPH_H
#define RICHTOOLS_RICHRAYTRACINGALLSPH_H 1

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

#include "GaudiKernel/IHistogramSvc.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

// RichDet
#include "RichDet/DeRichSphMirror.h"
#include "RichDet/DeRichHPDPanel.h"

//-----------------------------------------------------------------------------
/** @class RichRayTracingAllSph RichRayTracingAllSph.h
 *
 *  Rich detector tool which traces photons to the photodetectors. Mirror
 *  segmentation is included.
 *
 *  @author Antonis Papanestis
 *  @date   2003-11-04
 */
//-----------------------------------------------------------------------------

class RichRayTracingAllSph : public RichToolBase,
                             virtual public IRichRayTracing 
{

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichRayTracingAllSph( const std::string& type,
                  const std::string& name,
                  const IInterface* parent);

  virtual ~RichRayTracingAllSph( ); ///< Destructor

  // Initialization of the tool after creation
  virtual StatusCode initialize();

  // Finalization of the tool before deletion
  virtual StatusCode finalize  ();

public: // methods (and doxygen comments) inherited from interface

  // For a given detector, raytraces a given direction from a given point to
  // the photo detectors. Returns the result in the form of a RichGeomPhoton
  StatusCode traceToDetector( const Rich::DetectorType rich,
                              const HepPoint3D& startPoint,
                              const HepVector3D& startDir,
                              RichGeomPhoton& photon,
                              const RichTraceMode mode = RichTraceMode(),
                              const Rich::Side forcedSide = Rich::top ) const;

  // For a given detector, raytraces a given direction from a given point to
  // the average photon detector plane (no HPD acceptance). Result is a HepPoint3D
  StatusCode
  traceToDetectorWithoutEff( const Rich::DetectorType rich,
                             const HepPoint3D& startPoint,
                             const HepVector3D& startDir,
                             HepPoint3D& hitPosition,
                             const RichTraceMode mode = RichTraceMode(),
                             const Rich::Side forcedSide = Rich::top ) const;


  // Raytraces from a point in the detector panel back to the spherical mirror
  // returning the mirror intersection point and the direction a track would have
  // in order to hit that point in the detector panel.

  virtual StatusCode traceBackFromDetector ( const HepPoint3D& startPoint,
                                             const HepVector3D& startDir,
                                             HepPoint3D& endPoint,
                                             HepVector3D& endDir ) const;

  // For a given detector, ray traces a given direction from a given point
  // to the average photo detector plane. Returns the result in the form
  // of a RichGeomPhoton
  StatusCode intersectPDPanel( const Rich::DetectorType rich,
                               const HepPoint3D& point,
                               const HepVector3D& dir,
                               RichGeomPhoton& photon ) const;

  // Intersection a given direction, from a given point with a given plane.
  StatusCode intersectPlane( const HepPoint3D& position,
                             const HepVector3D& direction,
                             const HepPlane3D& plane,
                             HepPoint3D& intersection ) const;

  // Reflect a given direction off a spherical mirror. Can be used for intersection.
  StatusCode reflectSpherical ( HepPoint3D& position,
                                HepVector3D& direction,
                                const HepPoint3D& CoC,
                                const double radius ) const;

private: // methods

  StatusCode reflectBothMirrors ( const Rich::DetectorType rich,
                                  HepPoint3D& position,
                                  HepVector3D& direction,
                                  RichGeomPhoton& photon,
                                  const RichTraceMode mode,
                                  const Rich::Side fSide ) const;

  StatusCode reflectFlatPlane ( HepPoint3D& position,
                                HepVector3D& direction,
                                const HepPlane3D& plane ) const;

  StatusCode bookHistos();

  IHistogramSvc* histoSvc() const;

private: // data

  /// Rich1 and Rich2
  DeRich* m_rich[Rich::NRiches];

  // photodetector panels
  typedef boost::array<DeRichHPDPanel*, 2> HPDPanelsPerRich;
  boost::array<HPDPanelsPerRich, 2> m_photoDetPanels;

  /// Spherical mirror nominal center of curvature
  HepPoint3D m_nominalCoC[Rich::NRiches][2];

  /// Secondary mirror nominal planes
  HepPlane3D m_nominalSecMirrorPlane[Rich::NRiches][2];
  double m_nomSphMirrorRadius[Rich::NRiches];
  int m_sphMirrorSegRows[Rich::NRiches];
  int m_sphMirrorSegCols[Rich::NRiches];
  int m_secMirrorSegRows[Rich::NRiches];
  int m_secMirrorSegCols[Rich::NRiches];

  /// Mirror segment finder tool
  const IRichMirrorSegFinder* m_mirrorSegFinder;

  /// Histogram service
  mutable IHistogramSvc* m_HDS;

  /// monitoring histograms (true/false)
  bool m_moni;
  /// directory path for the histograms
  std::string m_histPth;
  /// photons missed on the outer boundary of the spherical mirror
  IHistogram2D* m_sphMirMissedOut[2];
  /// photons missed on the outer boundary of the secondary mirror
  IHistogram2D* m_secMirMissedOut[2];
  /// photons missed in the gaps of the spherical mirror
  IHistogram2D* m_sphMirMissedGap[2];
  /// photons missed in the gaps of the secondary mirror
  IHistogram2D* m_secMirMissedGap[2];

  /// a z point that separates Rich 1 from Rich2 (anything 3000-9000mm)
  double m_RichDetSeparationPointZ;

};

#endif // RICHTOOLS_RICHRAYTRACINGALLSPH_H
