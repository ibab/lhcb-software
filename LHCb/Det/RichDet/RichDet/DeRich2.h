
/** @file DeRich2.h
 *
 *  Header file for detector description class : DeRich2
 *
 *  CVS Log :-
 *  $Id: DeRich2.h,v 1.16 2007-02-28 18:31:07 marcocle Exp $
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */

#ifndef RICHDET_DERICH2_H
#define RICHDET_DERICH2_H 1

// local
#include "RichDet/DeRich.h"

// External declarations
extern const CLID CLID_DERich2;


/** @class DeRich2 DeRich2.h
 *
 * This is the DeRich detector element for Rich2
 *
 * @author Antonis Papanestis a.papanestis@rl.ac.uk
 */

class DeRich2: public DeRich 
{

public:
  /**
   * Constructor for this class
   */
  DeRich2();

  /**
   * Default destructor
   */
  virtual ~DeRich2();

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
   * Specific Rich2 initialisation
   * @return Status of initialisation
   */
  virtual StatusCode initialize();

  /**
   * Returns the nominal centre of curvature of the spherical mirror for this Rich
   *
   * @param side Which side: left or right
   * @return The nominal centre of curvature
   */
  virtual const Gaudi::XYZPoint& nominalCentreOfCurvature(const Rich::Side side) const;

  /**
   * Returns the nominal normal vector of the flat mirror plane for this Rich
   *
   * @param side Which side: left or right
   * @return The nominal normal vector
   */
  virtual const Gaudi::XYZVector& nominalNormal(const Rich::Side side) const;

  /**
   * Returns the nominal flat mirror plane for this Rich
   *
   * @param side Which side: left or right
   * @return The nominal flat mirror plane
   */
  virtual const Gaudi::Plane3D& nominalPlane(const Rich::Side side) const;

  /**
   * Check on which side of this Rich lies this point
   *
   * @param point A point in the global coordinate system
   * @return The side for this point
   */
  virtual Rich::Side side( const Gaudi::XYZPoint& point ) const;

  virtual StatusCode alignSphMirrors();
  virtual StatusCode alignSecMirrors();

private:

  Gaudi::Plane3D m_nominalPlaneLeft;   ///< The left nominal flat mirror plane
  Gaudi::Plane3D m_nominalPlaneRight;  ///< The right nominal flat mirror plane

  /// The nominal centre of curvature of the spherical mirror (negative side)
  Gaudi::XYZPoint  m_nominalCentreOfCurvatureRight;

  /// The nominal normal vector of the flat mirror plane (negative side)
  Gaudi::XYZVector m_nominalNormalRight;

};

#endif    // RICHDET_DERICH2_H
