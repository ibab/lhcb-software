
//-----------------------------------------------------------------------------
/** @file RichPhotonRecoUsingQuarticSoln.h
 *
 *  Header file for tool : RichPhotonRecoUsingQuarticSoln
 *
 *  CVS Log :-
 *  $Id: RichPhotonRecoUsingQuarticSoln.h,v 1.10 2006-12-01 17:05:09 cattanem Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @author Antonis Papanestis
 *  @date   2003-11-14
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RichPhotonRecoUsingQuarticSoln_H
#define RICHRECTOOLS_RichPhotonRecoUsingQuarticSoln_H 1

// STL
#include <sstream>

// Base class and interfaces
#include "RichKernel/RichToolBase.h"
#include "RichRecBase/IRichPhotonReconstruction.h"
#include "RichKernel/IRichMirrorSegFinder.h"
#include "RichKernel/IRichRayTracing.h"
#include "RichKernel/IRichSmartIDTool.h"
#include "RichKernel/IRichRefractiveIndex.h"

// RichKernel
#include "Kernel/RichSide.h"
#include "Kernel/RichSmartID.h"

// RichEvent
#include "RichEvent/RichTrackSegment.h"
#include "RichEvent/RichGeomPhoton.h"

// LHCbDefinitions
#include "Kernel/Point3DTypes.h"
#include "Kernel/Vector3DTypes.h"

// RichDet
#include "RichDet/DeRichHPDPanel.h"
#include "RichDet/DeRichSphMirror.h"
#include "RichDet/DeRich1.h"
#include "RichDet/DeRich2.h"
#include "RichDet/DeRichBeamPipe.h"

// from GSL
#include "gsl/gsl_math.h"
#include "gsl/gsl_complex.h"
#include "gsl/gsl_complex_math.h"
#include "gsl/gsl_poly.h"

// Include the private quartic solving code
// Eventually this might be in GSL ...
//#include "zsolve_quartic.h"
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
 *  @author Chris Jones         Christopher.Rob.Jones@cern.ch
 *  @author Antonis Papanestis  A.Papanestis@rl.ac.uk
 *  @date   2003-11-14
 *
 *  @todo Review if "Unit()" method is needed in secondary reflection
 *        plane construction
 *  @todo Fix HPD quartz window refraction bias at source in RichDet and remove 
 *        fudge correction here
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
  virtual StatusCode reconstructPhoton( const LHCb::RichTrackSegment& trSeg,
                                        const LHCb::RichSmartID smartID,
                                        LHCb::RichGeomPhoton& gPhoton ) const;

  // Reconstructs the geometrical photon candidate for a given RichTrackSegment
  // and hit position in global LHCb coordinates.
  virtual StatusCode reconstructPhoton ( const LHCb::RichTrackSegment& trSeg,
                                         const Gaudi::XYZPoint& detectionPoint,
                                         LHCb::RichGeomPhoton& gPhoton,
                                         const LHCb::RichSmartID smartID = LHCb::RichSmartID() ) const;

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
  bool solveQuarticEq( const Gaudi::XYZPoint& emissionPoint,
                       const Gaudi::XYZPoint& CoC,
                       const Gaudi::XYZPoint& virtDetPoint,
                       const double radius,
                       Gaudi::XYZPoint& sphReflPoint ) const;

  /** Correct Aerogel Cherenkov angle theta for refraction at exit of aerogel
   *
   *  @param trSeg           The track segment the photon is reconstructed against
   *  @param photonDirection The reconstructed photon direction
   *  @param thetaCerenkov   The Cherenkov theta angle to correct
   */
  void correctAeroRefraction( const LHCb::RichTrackSegment& trSeg,
                              Gaudi::XYZVector& photonDirection,
                              double & thetaCerenkov ) const;

  /// Find mirror segments and reflection points for given data
  bool findMirrorData( const Rich::DetectorType rich,
                       const Rich::Side side,
                       const Gaudi::XYZPoint & virtDetPoint,
                       const Gaudi::XYZPoint & emissionPoint,
                       const DeRichSphMirror *& sphSegment,
                       const DeRichSphMirror *& secSegment,
                       Gaudi::XYZPoint & sphReflPoint,
                       Gaudi::XYZPoint & secReflPoint ) const;

  /// Compute the best emission point for the gas radiators using
  /// the given spherical mirror reflection points
  bool getBestGasEmissionPoint( const Rich::RadiatorType radiator,
                                const Gaudi::XYZPoint & sphReflPoint1,
                                const Gaudi::XYZPoint & sphReflPoint2,
                                const Gaudi::XYZPoint & detectionPoint,
                                const LHCb::RichTrackSegment& trSeg,
                                Gaudi::XYZPoint & emissionPoint,
                                double & fraction ) const;

private: // data

  /// Rich1 and Rich2 detector elements
  const DeRich * m_rich[Rich::NRiches];

  /// Tool to locate the appropriate mirror segments for a given reflection point
  const IRichMirrorSegFinder* m_mirrorSegFinder;

  /// Raytracing tool
  const IRichRayTracing* m_rayTracing;

  /// RichSmartID tool
  const IRichSmartIDTool* m_idTool;

  /// Refractive index tool
  const IRichRefractiveIndex * m_refIndex;

  /** @brief Flag to indicate if the unambiguous photon test should be performed
   *  for each radiator
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
  std::vector<bool> m_testForUnambigPhots;

  /** Flag to turn on rejection of ambiguous photons 
   *
   *  If set true photons which are not unambiguous will be rejected
   *  Note, setting this true automatically means m_testForUnambigPhots
   *  will be set true
   */ 
  std::vector<bool> m_rejectAmbigPhots;

  /** @brief Flag to indicate if Cherenkov angles should be computed using the
   *  absolute mirror segment alignment.
   *
   *  If set to true, then the reconstruction will be performed twice,
   *  once with the nominal mirror allignment in order to find the appropriate
   *  mirror segments, and then again with thos segments.
   *
   *  If false, only the nominal mirror allignment will be used
   *
   *  @attention
   *  This is really only a 'developer' option, used for reconstruction studies.
   *  Turning it off will degrade the Cherenkov resolution so in general should
   *  not be done.
   */
  std::vector<bool> m_useAlignedMirrSegs;

  /** @brief Flag to force the assumption that the secondary mirrors are perfectly flat
   *
   *  If set to true then the photon reconstruction will assume the secondary mirrors
   *  are perfectly flat. If false (default) then the true spherical nature of the
   *  secondary mirrors is taken into account
   */
  bool m_forceFlatAssumption;

  /** Number of iterations of the quartic solution, for each radiator, in order
   *  to account for the non-flat secondary mirrors.
   */
  std::vector<int> m_nQits;

  /** Flag to turn on use of secondary mirrors (Default)
   *
   *  Set to false if processing test-beam data
   */
  bool m_useSecMirs;

  /** Fudge factors to correct small bias in CK theta from quartz windows refration
   *  correction in RichDet
   */
  std::vector<double> m_ckFudge;

  /// RICH beampipe object for each radiator
  std::vector<DeRichBeamPipe*> m_deBeam;

  /// Turn on/off the checking of photon trajectories against the beam pipe
  std::vector<bool> m_checkBeamPipe;

  /// Check for photons that cross between the different RICH 'sides'
  std::vector<bool> m_checkPhotCrossSides;

};

#endif // RICHRECTOOLS_RichPhotonRecoUsingQuarticSoln_H
