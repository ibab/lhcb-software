// $Id: RichReflector.h,v 1.1.1.1 2004-06-17 12:04:08 cattanem Exp $
#ifndef RICHDETTOOLS_RICHREFLECTOR_H
#define RICHDETTOOLS_RICHREFLECTOR_H 1

// Include files
#include <vector>
#include <cmath>

// from GSL
#include "gsl/gsl_math.h"
#include "gsl/gsl_complex.h"
#include "gsl/gsl_complex_math.h"
#include "gsl/gsl_poly.h"

#include "CLHEP/Geometry/Plane3D.h"
#include "RichDet/DeRichFlatMirror.h"
#include "RichDet/DeRichSphMirror.h"

// Include the private quartic solving code
// Eventually this will be in GSL
#include "zsolve_quartic.h"

/** @class RichReflector RichReflector.h RichDetTools/RichReflector.h
 *
 *  Utility class representing a RICH mirror
 *
 *  @author Artur Barczyk
 *  @date   18/05/2002
 */

class RichReflector {

public:

  typedef std::vector<RichReflector>                   Collection;
  typedef std::vector<RichReflector>::iterator         Iterator;
  typedef std::vector<RichReflector>::const_iterator   ConstIterator;
  typedef std::vector<RichReflector*>                  PointerCollection;
  typedef std::vector<RichReflector*>::iterator        PointerIterator;
  typedef std::vector<RichReflector*>::const_iterator  ConstPointerIterator;
  typedef std::vector<DeRichFlatMirror*>               FlatMirrorPtrVector;
  typedef std::vector<DeRichSphMirror*>                SphMirrorPtrVector;

  /// Standard constructor
  RichReflector( );

  /// Constructor from detector elements
  RichReflector( FlatMirrorPtrVector&,
                 SphMirrorPtrVector&,
                 const HepPlane3D&,
                 const HepPoint3D&,
                 const double );

  virtual ~RichReflector( ); ///< Destructor

  /// copy constructor
  RichReflector ( const RichReflector& );

  /// assignment constructor
  RichReflector& operator= ( const RichReflector& );

  int reflectionPoint( const HepPoint3D&,
                       const HepPoint3D&,
                       HepPoint3D&, HepPoint3D& ) const;

  int reflect(HepPoint3D&, HepVector3D&) const;

  int reflectSph(HepPoint3D&, HepVector3D&) const;
  int reflectFlat(HepPoint3D&, HepVector3D&) const;

  int intersectNominalSphMirror(const HepPoint3D&,
                                const HepVector3D&,
                                HepPoint3D&) const;

  int intersectNominalFlatMirror(const HepPoint3D&,
                                 const HepVector3D&,
                                 HepPoint3D&) const;

private: // methods

  // Solve the quartic equation
  static int quarticEquation ( const double a[5],
                               gsl_complex z[4] );

  enum MirrorType {Sph = 0, Flat};
  int findMirrorSegment(const HepPoint3D& reflPoint,
                        MirrorType type) const;

private: // data
  
  // pointers to DetElements (mirror segments)
  FlatMirrorPtrVector m_deFlatSegments;
  SphMirrorPtrVector m_deSphericalSegments;

  // nominal values
  HepPlane3D  m_nominalFlatMirrorPlane;
  HepNormal3D m_nominalFlatMirrorPlaneNormal;
  HepPoint3D  m_nominalSphMirrorCentre;

  double m_sphMirrorRadius;
  double m_sphMirrorRadius2;

};

inline int RichReflector::quarticEquation( const double a[5],
                                           gsl_complex z[4] ) 
{
  return ( 0 == a[0] ? 0 :
           gsl_poly_complex_solve_quartic( a[1]/a[0], a[2]/a[0], a[3]/a[0], a[4]/a[0], 
                                           &z[0], &z[1], &z[2], &z[3] ) );
}

#endif // RICHDETTOOLS_RICHREFLECTOR_H
