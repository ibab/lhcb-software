
/** @file DeRichFlatMirror.h
 *
 *  Header file for detector description class : DeRichFlatMirror
 *
 *  CVS Log :-
 *  $Id: DeRichFlatMirror.h,v 1.4 2004-07-27 08:55:22 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */

#ifndef RICHDET_DERICHFLATMIRROR_H
#define RICHDET_DERICHFLATMIRROR_H 1

// Include files
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Normal3D.h"
#include "CLHEP/Geometry/Plane3D.h"

#include "DetDesc/DetectorElement.h"
#include "DetDesc/ISolid.h"


// External declarations
extern const CLID& CLID_DeRichFlatMirror;

/** @class DeRichFlatMirror DeRichFlatMirror.h
 *
 * Detector element for flat mirrors providing geometry info
 *
 * @author Antonis Papanestis a.papanestis@rl.ac.uk
 */
class DeRichFlatMirror: public DetectorElement {

public:
  /**
   * Default constructor for this class
   */
  DeRichFlatMirror();

  /**
   * Default destructor
   */
  virtual ~DeRichFlatMirror();

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
   * Retrieves the normal vector of this flat mirror
   * @return The normal vector of this mirror as a HepNormal3D
   */
  inline HepNormal3D normalVector(){
    return m_normalVector;
  }

  /**
   * Retrieves the centre of this flat mirror
   * @return The centre of this flat mirror on the reflective surface
   */
  inline HepPoint3D mirrorCentre(){
    return m_mirrorCentre;
  }

  /**
   * Retrieves the plane of this flat mirror
   * @return The plane of this flat mirror as a HepPlane3D
   */
  inline HepPlane3D mirrorPlane(){
    return m_mirrorPlane;
  }

  /**
   * Retrieves the number of this mirror (segment)
   * @return the number of this mirror
   */
  inline int mirrorNumber(){
    return m_mirrorNumber;
  }

  /**
   * Checks if the direction intersects with the mirror
   * @return Status of intersection
   * @retval StatusCode::FAILURE No intersection
   */
  StatusCode intersects(const HepPoint3D& globalP, const HepVector3D& globalV);

private:

  const ISolid* m_solid;       ///< The mirror solid

  HepVector3D m_normalVector;  ///< The normal vector of the flat mirror
  HepPoint3D  m_mirrorCentre;  ///< The mirror centre on the reflective surface
  HepPlane3D  m_mirrorPlane;   ///< The flat mirror plane

  /// Alignment constant for x alignment.  Not used presently
  double m_alignmentConstantX;
  /// Alignment constant for y alignment.  Not used presently
  double m_alignmentConstantY;

  int m_mirrorNumber;           ///< mirror (segment) number


};

#endif    // RICHDET_DERICHFLATMIRROR_H
