
//-----------------------------------------------------------------------------
/** @file RichRayTracing.h
 *
 *  Header file for tool : RichDetParameters
 *
 *  CVS History :
 *  $Id: RichRayTracing.h,v 1.15 2005-12-13 15:07:11 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2004-03-29
 */
//-----------------------------------------------------------------------------

#ifndef RICHTOOLS_RICHRAYTRACING_H
#define RICHTOOLS_RICHRAYTRACING_H 1

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
#include "Kernel/Point3DTypes.h"
#include "Kernel/Vector3DTypes.h"

// LHCbKernel
#include "Kernel/RichSmartID.h"

#include "GaudiKernel/IHistogramSvc.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

// RichDet
#include "RichDet/DeRichSphMirror.h"
#include "RichDet/DeRichFlatMirror.h"
#include "RichDet/DeRichHPDPanel.h"

//-----------------------------------------------------------------------------
/** @class RichRayTracing RichRayTracing.h
 *
 *  Rich detector tool which traces photons to the photodetectors. Mirror
 *  segmentation is included.
 *
 *  @author Antonis Papanestis
 *  @date   2003-11-04
 */
//-----------------------------------------------------------------------------

class RichRayTracing : public RichToolBase,
                       virtual public IRichRayTracing 
{

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichRayTracing( const std::string& type,
                  const std::string& name,
                  const IInterface* parent);

  virtual ~RichRayTracing( ); ///< Destructor

  // Initialization of the tool after creation
  virtual StatusCode initialize();

  // Finalization of the tool before deletion
  virtual StatusCode finalize  ();

public: // methods (and doxygen comments) inherited from interface

  // For a given detector, raytraces a given direction from a given point to
  // the photo detectors. Returns the result in the form of a RichGeomPhoton
  StatusCode traceToDetector( const Rich::DetectorType rich,
                              const Gaudi::XYZPoint& startPoint,
                              const Gaudi::XYZVector& startDir,
                              RichGeomPhoton& photon,
                              const RichTraceMode mode = RichTraceMode(),
                              const Rich::Side forcedSide = Rich::top ) const;

  // For a given detector, raytraces a given direction from a given point to
  // the average photon detector plane (no HPD acceptance). Result is a Gaudi::XYZPoint
  StatusCode
  traceToDetectorWithoutEff( const Rich::DetectorType rich,
                             const Gaudi::XYZPoint& startPoint,
                             const Gaudi::XYZVector& startDir,
                             Gaudi::XYZPoint& hitPosition,
                             const RichTraceMode mode = RichTraceMode(),
                             const Rich::Side forcedSide = Rich::top ) const;


  // Raytraces from a point in the detector panel back to the spherical mirror
  // returning the mirror intersection point and the direction a track would have
  // in order to hit that point in the detector panel.

  virtual StatusCode traceBackFromDetector ( const Gaudi::XYZPoint& startPoint,
                                             const Gaudi::XYZVector& startDir,
                                             Gaudi::XYZPoint& endPoint,
                                             Gaudi::XYZVector& endDir ) const;

  // For a given detector, ray traces a given direction from a given point
  // to the average photo detector plane. Returns the result in the form
  // of a RichGeomPhoton
  StatusCode intersectPDPanel( const Rich::DetectorType rich,
                               const Gaudi::XYZPoint& point,
                               const Gaudi::XYZVector& dir,
                               RichGeomPhoton& photon ) const;

  // Intersection a given direction, from a given point with a given plane.
  StatusCode intersectPlane( const Gaudi::XYZPoint& position,
                             const Gaudi::XYZVector& direction,
                             const Gaudi::Plane3D& plane,
                             Gaudi::XYZPoint& intersection ) const;

  // Reflect a given direction off a spherical mirror. Can be used for intersection.
  StatusCode reflectSpherical ( Gaudi::XYZPoint& position,
                                Gaudi::XYZVector& direction,
                                const Gaudi::XYZPoint& CoC,
                                const double radius ) const;

private: // methods

  StatusCode reflectBothMirrors ( const Rich::DetectorType rich,
                                  Gaudi::XYZPoint& position,
                                  Gaudi::XYZVector& direction,
                                  RichGeomPhoton& photon,
                                  const RichTraceMode mode,
                                  const Rich::Side fSide ) const;

  StatusCode reflectFlat ( Gaudi::XYZPoint& position,
                           Gaudi::XYZVector& direction,
                           const Gaudi::Plane3D& plane ) const;

  StatusCode bookHistos();

  IHistogramSvc* histoSvc() const;

private: // data

  /// Rich1 and Rich2
  DeRich* m_rich[Rich::NRiches];

  // photodetector panels
  typedef boost::array<DeRichHPDPanel*, 2> HPDPanelsPerRich;
  boost::array<HPDPanelsPerRich, 2> m_photoDetPanels;

  /// Spherical mirror nominal center of curvature
  Gaudi::XYZPoint m_nominalCoC[Rich::NRiches][2];

  /// Flat mirror nominal planes
  Gaudi::Plane3D m_nominalFlatMirrorPlane[Rich::NRiches][2];
  double m_nomSphMirrorRadius[Rich::NRiches];
  int m_sphMirrorSegRows[Rich::NRiches];
  int m_sphMirrorSegCols[Rich::NRiches];
  int m_flatMirrorSegRows[Rich::NRiches];
  int m_flatMirrorSegCols[Rich::NRiches];

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
  /// photons missed on the outer boundary of the flat mirror
  IHistogram2D* m_flatMirMissedOut[2];
  /// photons missed in the gaps of the spherical mirror
  IHistogram2D* m_sphMirMissedGap[2];
  /// photons missed in the gaps of the flat mirror
  IHistogram2D* m_flatMirMissedGap[2];

  /// a z point that separates Rich 1 from Rich2 (anything 3000-9000mm)
  double m_RichDetSeparationPointZ;

};

#endif // RICHTOOLS_RICHRAYTRACING_H
