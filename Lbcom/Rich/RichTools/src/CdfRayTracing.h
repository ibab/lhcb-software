// $Id: CdfRayTracing.h,v 1.1.1.1 2004-06-17 12:04:08 cattanem Exp $
#ifndef RICHDETTOOLS_CDFRAYTRACING_H 
#define RICHDETTOOLS_CDFRAYTRACING_H 1

// Include files
// from STL
#include <string>

// base class and interface
#include "RichKernel/RichToolBase.h"
#include "RichKernel/IRichRayTracing.h"

// Event model
#include "Event/TrStoredTrack.h"

// Kernel
#include "Kernel/RichSmartID.h"

// RichKernel
#include "RichKernel/Rich1DTabProperty.h"

// local
#include "Rich1.h"
#include "Rich2.h"


/** @class CdfRayTracing CdfRayTracing.h
 *  
 *  Rich detector tool which traces photons to the photodetectors. Uses
 *  the "cdf" geometry without segmented mirrors.
 *
 *  @author Antonis Papanestis
 *  @date   2003-10-28
 */
class CdfRayTracing : public RichToolBase,  
                      virtual public IRichRayTracing {

public:

  /// Standard constructor
  CdfRayTracing( const std::string& type, 
                 const std::string& name,
                 const IInterface* parent);

  virtual ~CdfRayTracing( ); ///< Destructor

  virtual StatusCode initialize();
  virtual StatusCode finalize  ();

  /// For a given detector, raytraces a given direction from a given point to
  /// the photo detectors. Returns the result in the form of a RichGeomPhoton
  virtual StatusCode traceToDetector ( Rich::DetectorType,
                                       const HepPoint3D&,
                                       const HepVector3D&,
                                       RichGeomPhoton&,
                                       DeRichHPDPanel::traceMode mode =
                                       DeRichHPDPanel::circle ) const;

  virtual StatusCode traceToDetectorWithoutEff( Rich::DetectorType,
                                                const HepPoint3D&,
                                                const HepVector3D&,
                                                HepPoint3D&,
                                                DeRichHPDPanel::traceMode =
                                                DeRichHPDPanel::loose) const;


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
  virtual StatusCode reflectSpherical ( HepPoint3D& position,
                                        HepVector3D& direction,
                                        const HepPoint3D& CoC,
                                        double radius) const;

private:

  RichX::PointerCollection m_rich;

  // for fast reference. All radiators (both riches) in one container
  RichRadiator::PointerCollection m_radiator;

  // Reflectors/detectors in separate container for each rich1/2
  RichReflector::PointerCollection m_reflector[2];
  RichDetector::PointerCollection  m_detector[2];

  /// Allowable tolerance on state z positions
  std::vector<double> m_zTolerance;
  /// Nominal z positions of states at RICHes
  std::vector<double> m_nomZstates;

};
#endif // RICHDETTOOLS_CDFRAYTRACING_H
