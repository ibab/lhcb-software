
/** @file DeRichFlatMirror.h
 *
 *  Header file for detector description class : DeRichFlatMirror
 *
 *  CVS Log :-
 *  $Id: DeRichFlatMirror.h,v 1.8 2005-07-13 15:28:24 papanest Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.7  2005/02/25 23:28:54  jonrob
 *  set printing to debug level
 *
 *  Revision 1.6  2004/10/20 16:16:36  jonrob
 *  More minor updates to functions (adding const etc.)
 *
 *  Revision 1.5  2004/10/18 09:21:49  jonrob
 *  Minor updates to functions (adding const etc.)
 *
 *  Revision 1.4  2004/07/27 08:55:22  jonrob
 *  Add doxygen file documentation and CVS information
 *
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
#include "DetDesc/TabulatedProperty.h"

#include "Kernel/RichDetectorType.h"

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
   * Retrieves the normal vector of this flat mirror
   * @return The normal vector of this mirror as a HepNormal3D
   */
  inline const HepNormal3D & normalVector() const
  {
    return m_normalVector;
  }

  /**
   * Retrieves the centre of this flat mirror
   * @return The centre of this flat mirror on the reflective surface
   */
  inline const HepPoint3D & mirrorCentre() const
  {
    return m_mirrorCentre;
  }

  /**
   * Retrieves the plane of this flat mirror
   * @return The plane of this flat mirror as a HepPlane3D
   */
  inline const HepPlane3D & mirrorPlane() const
  {
    return m_mirrorPlane;
  }

  /**
   * Retrieves the number of this mirror (segment)
   * @return the number of this mirror
   */
  inline int mirrorNumber() const
  {
    return m_mirrorNumber;
  }

  /**
   * Checks if the direction intersects with the mirror
   * @return Status of intersection
   * @retval StatusCode::FAILURE No intersection
   */
  StatusCode intersects ( const HepPoint3D& globalP,
                          const HepVector3D& globalV ) const;

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

private:

  Rich::DetectorType m_rich;        ///< The Rich detector of this mirror
  /// mirror reflectivity
  const TabulatedProperty* m_reflectivity;

  const ISolid* m_solid;       ///< The mirror solid

  HepNormal3D m_normalVector;  ///< The normal vector of the flat mirror
  HepPoint3D  m_mirrorCentre;  ///< The mirror centre on the reflective surface
  HepPlane3D  m_mirrorPlane;   ///< The flat mirror plane

  /// Alignment constant for x alignment.  Not used presently
  double m_alignmentConstantX;
  /// Alignment constant for y alignment.  Not used presently
  double m_alignmentConstantY;

  int m_mirrorNumber;           ///< mirror (segment) number

  std::string m_name; ///< The name of this mirror

};

#endif    // RICHDET_DERICHFLATMIRROR_H
