// $Id: RichPhotonReco.h,v 1.1.1.1 2004-06-17 12:04:08 cattanem Exp $
#ifndef RICHDETTOOLS_RICHPHOTONRECO_H
#define RICHDETTOOLS_RICHPHOTONRECO_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// Base class and interface
#include "RichKernel/RichToolBase.h"
#include "RichKernel/IRichPhotonReconstruction.h"

// RichKernel
#include "RichKernel/IRichMirrorSegFinder.h"
#include "RichKernel/IRichRayTracing.h"
#include "RichKernel/IRichSmartIDTool.h"
#include "RichKernel/BoostArray.h"
#include "RichKernel/Rich1DTabProperty.h"

// RichEvent
#include "RichEvent/RichTrackSegment.h"
#include "RichEvent/RichGeomPhoton.h"

//CLHEP
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"

// LHCbKernel
#include "Kernel/RichSmartID.h"

// RichDet
#include "RichDet/DeRichHPDPanel.h"
#include "RichDet/DeRichSphMirror.h"
#include "RichDet/DeRichFlatMirror.h"

// from GSL
#include "gsl/gsl_math.h"
#include "gsl/gsl_complex.h"
#include "gsl/gsl_complex_math.h"
#include "gsl/gsl_poly.h"

// Include the private quartic solving code
// Eventually this will be in GSL
#include "zsolve_quartic.h"


/** @class RichPhotonReco RichPhotonReco.h
 *
 *  Rich detector tool which reconstructs photons from track segments
 *  and smartIDs
 *
 *  @author Antonis Papanestis
 *  @date   2003-11-14
 */
class RichPhotonReco : public RichToolBase,
                       virtual public IRichPhotonReconstruction {

public:

  /// Standard constructor
  RichPhotonReco( const std::string& type,
                  const std::string& name,
                  const IInterface* parent);

  virtual ~RichPhotonReco( ); ///< Destructor

  virtual StatusCode initialize();
  virtual StatusCode finalize  ();

  /// Reconstructs the geometrical photon candidate for a given track segment
  /// and pixel smart ID
  virtual StatusCode reconstructPhoton( const RichTrackSegment& trSeg,
                                        const RichSmartID& smartID,
                                        RichGeomPhoton& gPhoton) const;

  /// Reconstructs the geometrical photon candidate for a given track segment
  /// and pixel global position
  virtual StatusCode reconstructPhoton( const RichTrackSegment& trSeg,
                                        const HepPoint3D& detectionPoint,
                                        RichGeomPhoton& gPhoton ) const;

private: // methods

  // Solve the quartic equation
  static int quarticEquation ( const double a[5], gsl_complex z[4] );

  // Solve quartic equation for given mirror segments
  bool solveQuarticEq (const HepPoint3D& emissionPoint,
                       const HepPoint3D& CoC,
                       const HepPoint3D& virtDetPoint,
                       const double radius,
                       HepPoint3D& sphReflPoint) const;

private: // data

  /// Rich1 and Rich2
  DeRich* m_rich[2];

  // photodetector panels
  typedef boost::array<DeRichHPDPanel*, 2> HPDPanelsPerRich;
  boost::array<HPDPanelsPerRich, 2> m_photoDetPanels;

  HepPoint3D m_nominalCoC[2][2];
  HepPlane3D m_nominalFlatMirrorPlane[2][2];
  double m_nomSphMirrorRadius[2];

  IRichMirrorSegFinder* m_mirrorSegFinder;
  IRichRayTracing* m_rayTracing;
  IRichSmartIDTool* m_idTool;
  IRichRefractiveIndex * m_refIndex;

};

inline int RichPhotonReco::quarticEquation( const double a[5],
                                            gsl_complex z[4] )
{
  return ( 0 == a[0] ? 0 :
           gsl_poly_complex_solve_quartic( a[1]/a[0], a[2]/a[0], a[3]/a[0],
                                           a[4]/a[0],
                                           &z[0], &z[1], &z[2], &z[3] ) );
}


#endif // RICHDETTOOLS_RICHPHOTONRECO_H
