
/** @file DeRichSphMirror.h
 *
 *  Header file for detector description class : DeRichSphMirror
 *
 *  CVS Log :-
 *  $Id: DeRichSphMirror.h,v 1.6 2004-07-27 08:55:22 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */

#ifndef  RICHDET_DERICHSPHMIRROR_H
#define  RICHDET_DERICHSPHMIRROR_H 1

// Include files
#include "CLHEP/Geometry/Point3D.h"

#include "DetDesc/DetectorElement.h"
#include "DetDesc/ISolid.h"

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
  const CLID& clID() const {
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
  inline HepPoint3D centreOfCurvature(){
    return m_centreOfCurvature;
  }

  /**
   * Retrieves the centre this mirror on the reflective surface.
   * @return The mirror centre as HepPoint3D
   */
  inline HepPoint3D mirrorCentre(){
    return m_mirrorCentre;
  }

  /**
   * Retrieves the segment number of this spherical mirror
   * @return The number of this mirror
   */
  inline int mirrorNumber(){
    return m_mirrorNumber;
  }

  /**
   * Retrieves the radius of this spherical mirror
   * @return The radius of this spherical mirror
   */
  inline double radius(){
    return m_radius;
  }

  /**
   * Checks if the direction intersects with the mirror
   * @return Status of intersection
   * @retval StatusCode::FAILURE No intersection
   */
  StatusCode intersects(const HepPoint3D& globalP, const HepVector3D& globalV);

  /**
   * Checks if the direction intersects with the mirror and returns the
   * intersction point
   * @return Intersection point
   */
  StatusCode intersects(const HepPoint3D& globalP,
                        const HepVector3D& globalV,
                        HepPoint3D& intersectionPoint);

private:

  const ISolid* m_solid;             ///< The mirror solid

  HepPoint3D m_centreOfCurvature;    ///< The centre of curvature
  HepPoint3D m_mirrorCentre;         ///< The mirror centre

  /// Alignment constant for x alignment.  Not used presently
  double m_alignmentConstantX;
  /// Alignment constant for y alignment.  Not used presently
  double m_alignmentConstantY;

  double m_radius;                  ///< Spherical mirror radius

  int m_mirrorNumber;               ///< mirror (segment) number
};

#endif    //  RICHDET_DERICHSPHMIRROR_H
