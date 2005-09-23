
/** @file DeRichSphMirror.h
 *
 *  Header file for detector description class : DeRichSphMirror
 *
 *  CVS Log :-
 *  $Id: DeRichSphMirror.h,v 1.11 2005-09-23 15:27:28 papanest Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.10  2005/07/13 15:28:24  papanest
 *  Individual reflectivity for all mirror segments
 *
 *  Revision 1.9  2005/02/25 23:28:54  jonrob
 *  set printing to debug level
 *
 *  Revision 1.8  2004/10/20 16:16:36  jonrob
 *  More minor updates to functions (adding const etc.)
 *
 *  Revision 1.7  2004/10/18 09:21:49  jonrob
 *  Minor updates to functions (adding const etc.)
 *
 *  Revision 1.6  2004/07/27 08:55:22  jonrob
 *  Add doxygen file documentation and CVS information
 *
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */

#ifndef  RICHDET_DERICHSPHMIRROR_H
#define  RICHDET_DERICHSPHMIRROR_H 1

// Include files
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Geometry/Plane3D.h"

#include "DetDesc/DetectorElement.h"
#include "DetDesc/ISolid.h"
#include "DetDesc/TabulatedProperty.h"

#include "Kernel/RichDetectorType.h"

// External declarations
extern const CLID& CLID_DeRichSphMirror;

/** @class DeRichSphMirror DeRichSphMirror.h
 *
 * Detector element for spherical mirrors providing geometry info
 *
 * @author Antonis Papanestis a.papanestis@rl.ac.uk
 * @date   2004-06-18
 */
class DeRichSphMirror: public DetectorElement {

public:

  /**
   * Constructor for this class
   */
  DeRichSphMirror();

  /**
   * Default destructor
   */
  virtual ~DeRichSphMirror();

  /**
   * Retrieves reference to class identifier
   * @return the class identifier for this class
   */
  const CLID& clID() const
  {
    return classID();
  }

  /**
   * Retrieves reference to class identifier
   * @return the class identifier for this class
   */
  static const CLID& classID();

  /**
   * This is where most of the geometry is read and variables initialised
   * @return Status of initialisation
   * @retval StatusCode::FAILURE Initialisation failed, program should
   * terminate
   */
  virtual StatusCode initialize();

  /**
   * Retrieves the centre of curvarute of this mirror
   * @return The centre of curvature of this mirror as a HepPoint3D
   */
  inline const HepPoint3D& centreOfCurvature() const
  {
    return m_centreOfCurvature;
  }

  /**
   * Retrieves the centre this mirror on the reflective surface.
   * @return The mirror centre as HepPoint3D
   */
  inline const HepPoint3D& mirrorCentre() const
  {
    return m_mirrorCentre;
  }

  /**
   * Retrieves the segment number of this spherical mirror
   * @return The number of this mirror
   */
  inline int mirrorNumber() const
  {
    return m_mirrorNumber;
  }

  /**
   * Retrieves the radius of this spherical mirror
   * @return The radius of this spherical mirror
   */
  inline double radius() const
  {
    return m_radius;
  }

  /**
   * Checks if the direction intersects with the mirror
   * @return Status of intersection
   * @retval StatusCode::FAILURE No intersection
   */
  StatusCode intersects ( const HepPoint3D & globalP,
                          const HepVector3D & globalV) const;

  /**
   * Checks if the direction intersects with the mirror and returns the
   * intersction point
   * @return Intersection point
   */
  StatusCode intersects ( const HepPoint3D & globalP,
                          const HepVector3D & globalV,
                          HepPoint3D& intersectionPoint ) const;

  /** Returns the name of this particular mirror
   *  @return mirror name
   */
  inline const std::string & myName() const { return m_name; }

  /**
   * Returns a pointer to the tabulated property that holds the reflectivity
   * of the mirror.
   *
   * @return Pointer to mirror reflectivity
   */
  inline const TabulatedProperty* reflectivity() const
  {
    return m_reflectivity;
  }

  /**
   * Retrieves the normal vector at the centre of the mirror
   * @return Normal vector at the mirror centre
   */
  inline const HepVector3D& centreNormal() const
  {
    return m_centreNormal;
  }

  /**
   * Retrieves the normal vector at the centre of the mirror
   * @return Normal vector at the mirror centre
   */
  inline const HepPlane3D& centreNormalPlane() const
  {
    return m_centreNormalPlane;
  }


private:

  Rich::DetectorType m_rich;        ///< The Rich detector of this mirror
  /// mirror reflectivity
  const TabulatedProperty* m_reflectivity;

  const ISolid* m_solid;             ///< The mirror solid

  HepPoint3D m_centreOfCurvature;    ///< The centre of curvature
  HepPoint3D m_mirrorCentre;         ///< The mirror centre

  /// Alignment constant for x alignment.  Not used presently
  double m_alignmentConstantX;
  /// Alignment constant for y alignment.  Not used presently
  double m_alignmentConstantY;

  double m_radius;                  ///< Spherical mirror radius

  int m_mirrorNumber;               ///< mirror (segment) number

  std::string m_name; ///< The name of this mirror

  ///< The normal vector at the centre of the mirror
  HepVector3D m_centreNormal; 
  /// The normal vector at the centre of the mirror
  HepPlane3D m_centreNormalPlane; 
};

#endif    //  RICHDET_DERICHSPHMIRROR_H
