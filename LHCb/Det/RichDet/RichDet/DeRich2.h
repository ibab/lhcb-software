// $Id: DeRich2.h,v 1.9 2004-07-22 10:49:55 papanest Exp $
#ifndef DERICH2_H
#define DERICH2_H 1

// local
#include "RichDet/DeRich.h"

// External declarations
extern const CLID& CLID_DERich2;


/** @class DeRich2 DeRich2.h
 *
 * This is the DeRich detector element for Rich2
 *
 * @author Antonis Papanestis a.papanestis@rl.ac.uk
 */


class DeRich2: public DeRich {

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
  virtual inline const HepPoint3D & nominalCentreOfCurvature(Rich::Side side) const
  {
    return ( Rich::right == side ? m_nominalCentreOfCurvatureRight :
             m_nominalCentreOfCurvature );
  }

  /**
   * Returns the nominal normal vector of the flat mirror plane for this Rich
   *
   * @param side Which side: left or right
   * @return The nominal normal vector
   */
  virtual inline const HepNormal3D & nominalNormal(Rich::Side side) const
  {
    return ( Rich::right == side ? m_nominalNormalRight : m_nominalNormal );
  }

  /**
   * Returns the nominal flat mirror plane for this Rich
   *
   * @param side Which side: left or right
   * @return The nominal flat mirror plane
   */
  virtual inline const HepPlane3D & nominalPlane(Rich::Side side) const
  {
    return ( Rich::left == side ? m_nominalPlaneLeft : m_nominalPlaneRight );
  }

  /**
   * Check on which side of this Rich lies this point
   *
   * @param point A point in the global coordinate system
   * @return The side for this point
   */
  inline virtual Rich::Side side( const HepPoint3D & point ) const
  {
    return ( point.x() >= 0.0 ? Rich::left : Rich::right );
  }

private:

  HepPlane3D m_nominalPlaneLeft;   ///< The left nominal flat mirror plane
  HepPlane3D m_nominalPlaneRight;  ///< The right nominal flat mirror plane

  /// The nominal centre of curvature of the spherical mirror (negative side)
  HepPoint3D  m_nominalCentreOfCurvatureRight;
  /// The nominal normal vector of the flat mirror plane (negative side)
  HepNormal3D m_nominalNormalRight;

};

#endif    // DERICH2_H
