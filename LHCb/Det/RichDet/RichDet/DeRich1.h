
/** @file DeRich1.h
 *
 *  Header file for detector description class : DeRich1
 *
 *  @author Antonis Papanestis   a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */

#ifndef RICHDET_DERICH1_H
#define RICHDET_DERICH1_H 1

// local
#include "RichDet/DeRich.h"


// External declarations
extern const CLID CLID_DERich1;

/** @class DeRich1 RichDet/DeRich1.h
 *
 *  This is the DeRich detector element for Rich1
 *
 *  @author Antonis Papanestis   a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */

class DeRich1 : public DeRich 
{

public:

  /**
   * Constructor for this class
   */
  DeRich1(const std::string & name = "");

  /**
   * Default destructor
   */
  virtual ~DeRich1();

  /**
   * Retrieves reference to class identifier
   * @return the class identifier for this class
   */
  const CLID& clID() const { return classID(); }

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
  virtual const Gaudi::XYZPoint& nominalCentreOfCurvature(const Rich::Side side) const;

  /**
   * Returns the nominal normal vector of the flat mirror plane for this Rich
   *
   * @param side Which side: top or bottom
   * @return The nominal normal vector
   */
  virtual const Gaudi::XYZVector& nominalNormal(const Rich::Side side) const;

  /**
   * Returns the nominal flat mirror plane for this Rich
   *
   * @param side Which side: top or bottom
   * @return The nominal flat mirror plane
   */
  virtual const Gaudi::Plane3D& nominalPlane(const Rich::Side side) const;

  /**
   * Check on which side of this Rich lies this point
   *
   * @param point A point in the global coordinate system
   * @return The side for this point
   */
  virtual Rich::Side side( const Gaudi::XYZPoint& point) const;

  /// Align the primary mirrors
  virtual StatusCode alignSphMirrors();

  /// Align the secondary mirrors
  virtual StatusCode alignSecMirrors();

private:

  /// Access the name for a given panel
  virtual const std::string panelName( const Rich::Side panel ) const;

  /// Update Nominal mirror parameters
  StatusCode updateMirrorParams();

private:

  Gaudi::Plane3D m_nominalPlaneTop;    ///< The top nominal flat mirror plane
  Gaudi::Plane3D m_nominalPlaneBottom; ///< The bottom nominal flat mirror plane

  /// The nominal centre of curvature of the spherical mirror (negative side)
  Gaudi::XYZPoint  m_nominalCentreOfCurvatureBottom;

  /// The nominal centre of curvature of the spherical mirror (positive side)
  Gaudi::XYZPoint  m_nominalCentreOfCurvatureTop;

  /// The nominal normal vector of the flat mirror plane (negative side)
  Gaudi::XYZVector m_nominalNormalBottom;

  /// The nominal normal vector of the flat mirror plane (positive side)
  Gaudi::XYZVector m_nominalNormalTop;

  /// Condition for the alignment of the spherical mirrors
  SmartRef<Condition> m_sphMirAlignCond;

  /// Condition for the alignment of the secondary mirrors
  SmartRef<Condition> m_secMirAlignCond;

};

#endif    // RICHDET_DERICH1_H
