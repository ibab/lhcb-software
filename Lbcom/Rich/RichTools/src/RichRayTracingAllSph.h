
//-----------------------------------------------------------------------------
/** @file RichRayTracingAllSph.h
 *
 *  Header file for tool : RichDetParameters
 *
 *  CVS History :
 *  $Id: RichRayTracingAllSph.h,v 1.4 2005-11-15 13:39:28 jonrob Exp $
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
#include "RichKernel/RichHistoToolBase.h"
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

class RichRayTracingAllSph : public RichHistoToolBase,
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

  /// Ray trace from given position in given direction of both mirrors
  StatusCode reflectBothMirrors ( const Rich::DetectorType rich,
                                  HepPoint3D& position,
                                  HepVector3D& direction,
                                  RichGeomPhoton& photon,
                                  const RichTraceMode mode,
                                  const Rich::Side fSide ) const;

  /// Ray trace from given position in given direction of flat mirrors
  StatusCode reflectFlatPlane ( HepPoint3D& position,
                                HepVector3D& direction,
                                const HepPlane3D& plane ) const;

private: // data

  /// Rich1 and Rich2 pointers
  std::vector< const DeRich* > m_rich;

  /// photodetector panels per rich
  typedef boost::array<DeRichHPDPanel*, 2> HPDPanelsPerRich;
  /// photodetector for each rich
  boost::array<HPDPanelsPerRich, Rich::NRiches> m_photoDetPanels;

  std::vector<int> m_sphMirrorSegRows;
  std::vector<int> m_sphMirrorSegCols;
  std::vector<int> m_secMirrorSegRows;
  std::vector<int> m_secMirrorSegCols;

  /// Mirror segment finder tool
  const IRichMirrorSegFinder* m_mirrorSegFinder;

  /// a z point that separates Rich 1 from Rich2 (anything 3000-9000mm)
  double m_RichDetSeparationPointZ;

};

#endif // RICHTOOLS_RICHRAYTRACINGALLSPH_H
