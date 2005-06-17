
//-----------------------------------------------------------------------------
/** @file RichPhotonRecoUsingQuarticSoln.h
 *
 *  Header file for tool : RichPhotonRecoUsingQuarticSoln
 *
 *  CVS Log :-
 *  $Id: RichPhotonRecoUsingQuarticSoln.h,v 1.1 2005-06-17 15:15:55 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @author Antonis Papanestis
 *  @date   2003-11-14
 */
//-----------------------------------------------------------------------------

#ifndef RICHTOOLS_RICHPHOTONRECOUSINGQUARTICSOLN_H
#define RICHTOOLS_RICHPHOTONRECOUSINGQUARTICSOLN_H 1

// from Gaudi
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/ToolFactory.h"

// Base class and interfaces
#include "RichKernel/RichToolBase.h"
#include "RichKernel/IRichPhotonReconstruction.h"
#include "RichKernel/IRichMirrorSegFinder.h"
#include "RichKernel/IRichRayTracing.h"
#include "RichKernel/IRichSmartIDTool.h"
#include "RichKernel/IRichRefractiveIndex.h"

// RichKernel
#include "RichKernel/BoostArray.h"
#include "RichKernel/Rich1DTabProperty.h"
#include "Kernel/RichSide.h"
#include "Kernel/RichSmartID.h"

// RichEvent
#include "RichEvent/RichTrackSegment.h"
#include "RichEvent/RichGeomPhoton.h"

//CLHEP
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Units/PhysicalConstants.h"

// RichDet
#include "RichDet/DeRichHPDPanel.h"
#include "RichDet/DeRichSphMirror.h"
#include "RichDet/DeRichFlatMirror.h"
#include "RichDet/DeRich1.h"
#include "RichDet/DeRich2.h"

// from GSL
#include "gsl/gsl_math.h"
#include "gsl/gsl_complex.h"
#include "gsl/gsl_complex_math.h"
#include "gsl/gsl_poly.h"

// Include the private quartic solving code
// Eventually this might be in GSL ...
#include "zsolve_quartic.h"
#include "zsolve_quartic_RICH.h"

//-----------------------------------------------------------------------------
/** @class RichPhotonRecoUsingQuarticSoln RichPhotonRecoUsingQuarticSoln.h
 *
 *  Rich detector tool which reconstructs photons from track segments
 *  and smartIDs or global hit positions.
 *
 *  Takes into account the individual mirror allignments by (optionally) first
 *  trying to figure out which mirror segments the photon cound have been
 *  reflected off.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @author Antonis Papanestis
 *  @date   2003-11-14
 */
//-----------------------------------------------------------------------------

class RichPhotonRecoUsingQuarticSoln : public RichToolBase,
                                       virtual public IRichPhotonReconstruction
{

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichPhotonRecoUsingQuarticSoln( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent);

  virtual ~RichPhotonRecoUsingQuarticSoln( ); ///< Destructor

  // Initialization of the tool after creation
  virtual StatusCode initialize();

  // Finalization of the tool before deletion
  virtual StatusCode finalize();

public: // methods (and doxygen comments) inherited from interface

  // Reconstructs the geometrical photon candidate for a given RichTrackSegment
  // and RichSmartID channel identifier.
  virtual StatusCode reconstructPhoton( const RichTrackSegment& trSeg,
                                        const RichSmartID& smartID,
                                        RichGeomPhoton& gPhoton ) const;

  // Reconstructs the geometrical photon candidate for a given RichTrackSegment
  // and hit position in global LHCb coordinates.
  virtual StatusCode reconstructPhoton ( const RichTrackSegment& trSeg,
                                         const HepPoint3D& detectionPoint,
                                         RichGeomPhoton& gPhoton ) const;

private: // methods

  /** Solves the characteristic quartic equation for the RICH optical system.
   *
   *  See note LHCB/98-040 RICH section 3 for more details
   *
   *  @param emissionPoint Assumed photon emission point on track
   *  @param CoC           Spherical mirror centre of curvature
   *  @param virtDetPoint  Virtual detection point
   *  @param radius        Spherical mirror radius of curvature
   *  @param sphReflPoint  The reconstructed reflection pont on the spherical mirror
   *
   *  @return boolean indicating status of the quartic solution
   *  @retval true  Calculation was successful. sphReflPoint is valid.
   *  @retval false Calculation failed. sphReflPoint is not valid.
   */
  bool solveQuarticEq( const HepPoint3D& emissionPoint,
                       const HepPoint3D& CoC,
                       const HepPoint3D& virtDetPoint,
                       const double radius,
                       HepPoint3D& sphReflPoint ) const;

  /** Correct Aerogel Cherenkov angle theta for refraction at exit of aerogel
   *
   *  @param trSeg           The track segment the photon is reconstructed against
   *  @param photonDirection The reconstructed photon direction
   *  @param thetaCerenkov   The Cherenkov theta angle to correct
   */
  void correctAeroRefraction( const RichTrackSegment& trSeg,
                              HepVector3D& photonDirection,
                              double & thetaCerenkov ) const;

  /// Find mirror segments and reflection points for given data
  bool findMirrorData( const Rich::DetectorType rich,
                       const Rich::Side side,
                       const HepPoint3D & virtDetPoint,
                       const HepPoint3D & emissionPoint,
                       const DeRichSphMirror *& sphSegment,
                       const DeRichFlatMirror *& flatSegment,
                       HepPoint3D & sphReflPoint,
                       HepPoint3D & flatReflPoint ) const;

  /// Compute the best emission point for the gas radiators using
  /// the given spherical mirror reflection points
  bool getBestGasEmissionPoint( const Rich::RadiatorType radiator,
                                const HepPoint3D & sphReflPoint1,
                                const HepPoint3D & sphReflPoint2,
                                const HepPoint3D & detectionPoint,
                                const RichTrackSegment& trSeg,
                                HepPoint3D & emissionPoint,
                                double & fraction ) const;

private: // data

  /// Rich1 and Rich2 detector elements
  const DeRich * m_rich[Rich::NRiches];

  /// nominal centre of curvature for each RICH and side spherical mirrors
  HepPoint3D m_nominalCoC[Rich::NRiches][Rich::NHPDPanelsPerRICH];

  /// nominal normal vector for each RICH and side flat mirrors
  HepPlane3D m_nominalFlatMirrorPlane[Rich::NRiches][Rich::NHPDPanelsPerRICH];

  /// Nominal radius of curvature for each RICH spherical mirrors
  double m_nomSphMirrorRadius[Rich::NRiches];

  /// Tool to locate the appropriate mirror segments for a given reflection point
  IRichMirrorSegFinder* m_mirrorSegFinder;

  /// Raytracing tool
  IRichRayTracing* m_rayTracing;

  /// RichSmartID tool
  IRichSmartIDTool* m_idTool;

  /// Refractive index tool
  IRichRefractiveIndex * m_refIndex;

  /** Flag to indicate if the unambiguous photon test should be performed
   *
   *  If set to true the reconstruction is first performed twice, using the
   *  track segment start and end points as the assumed emission points and the
   *  nominal mirror geometry. If both calculations give the same mirror segments
   *  then this photon is flagged as unambiguous and the reconstruction is then
   *  re-performed using these mirror segments and the best-guess emission point
   *  (usually the track segment centre point)
   *
   *  If false, then the photon is first reconstructed using the segment centre
   *  point as the emission point and using the nominal mirror geometry. The mirror
   *  segments found for this calculation are then used to re-do the calculation.
   */
  bool m_testForUnambigPhots;

  /** Flag to indicate if Cherenkov angles should be computed using the
   *  absolute mirror segment alignment.
   *
   *  If set to true, then the reconstruction will be performed twice,
   *  once with the nominal mirror allignment in order to find the appropriate
   *  mirror segments, and then again with thos segments.
   *
   *  If false, only the nominal mirror allignment will be used
   */
  bool m_useAlignedMirrSegs;

};

#endif // RICHTOOLS_RICHPHOTONRECOUSINGQUARTICSOLN_H
