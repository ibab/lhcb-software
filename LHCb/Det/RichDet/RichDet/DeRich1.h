
/** @file DeRich1.h
 *
 *  Header file for detector description class : DeRich1
 *
 *  CVS Log :-
 *  $Id: DeRich1.h,v 1.10 2004-10-18 09:21:48 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.9  2004/07/27 08:55:22  jonrob
 *  Add doxygen file documentation and CVS information
 *
 *
 *  @author Antonis Papanestis   a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */

#ifndef RICHDET_DERICH1_H
#define RICHDET_DERICH1_H 1

// local
#include "RichDet/DeRich.h"


// External declarations
extern const CLID& CLID_DERich1;

/** @class DeRich1 DeRich1.h
 *
 *  This is the DeRich detector element for Rich1
 *
 *  @author Antonis Papanestis   a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */

class DeRich1 : public DeRich {

public:

  /**
   * Constructor for this class
   */
  DeRich1();

  /**
   * Default destructor
   */
  virtual ~DeRich1();

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
   * Specific Rich1 initialisation
   * @return Status of initialisation
   */
  virtual StatusCode initialize();

  /**
   * Returns the nominal centre of curvature of the spherical mirror for this Rich
   *
   * @param side Which side: top or bottom
   * @return The nominal centre of curvature
   */
  virtual inline const HepPoint3D & nominalCentreOfCurvature(const Rich::Side side) const
  {
    return ( Rich::bottom == side ? m_nominalCentreOfCurvatureBottom :
             m_nominalCentreOfCurvature );
  }

  /**
   * Returns the nominal normal vector of the flat mirror plane for this Rich
   *
   * @param side Which side: top or bottom
   * @return The nominal normal vector
   */
  virtual inline const HepNormal3D & nominalNormal(const Rich::Side side) const
  {
    return ( Rich::bottom == side ? m_nominalNormalBottom : m_nominalNormal );
  }

  /**
   * Returns the nominal flat mirror plane for this Rich
   *
   * @param side Which side: top or bottom
   * @return The nominal flat mirror plane
   */
  virtual inline const HepPlane3D & nominalPlane(const Rich::Side side) const
  {
    return ( Rich::top == side ? m_nominalPlaneTop : m_nominalPlaneBottom );
  }

  /**
   * Check on which side of this Rich lies this point
   *
   * @param point A point in the global coordinate system
   * @return The side for this point
   */
  inline virtual Rich::Side side( const HepPoint3D & point) const
  {
    return ( point.y() >= 0.0 ? Rich::top : Rich::bottom );
  }

private:

  HepPlane3D m_nominalPlaneTop;    ///< The top nominal flat mirror plane
  HepPlane3D m_nominalPlaneBottom; ///< The bottom nominal flat mirror plane


  /// The nominal centre of curvature of the spherical mirror (negative side)
  HepPoint3D  m_nominalCentreOfCurvatureBottom;

  /// The nominal normal vector of the flat mirror plane (negative side)
  HepNormal3D m_nominalNormalBottom;

};

#endif    // RICHDET_DERICH1_H
