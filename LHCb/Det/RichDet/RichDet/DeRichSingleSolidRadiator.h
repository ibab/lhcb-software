
/** @file DeRichSingleSolidRadiator.h
 *
 *  Header file for detector description class : DeRichSingleSolidRadiator
 *
 *  CVS Log :-
 *  $Id: DeRichSingleSolidRadiator.h,v 1.5 2004-10-20 16:16:36 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.4  2004/09/01 15:20:19  papanest
 *  added functions for TabProps
 *
 *  Revision 1.3  2004/07/2D7 08:55:22  jonrob
 *  Add doxygen file documentation and CVS information
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */

#ifndef RICHDET_DERICHSINGLESOLIDRADIATOR_H
#define RICHDET_DERICHSINGLESOLIDRADIATOR_H 1

// Include files
#include "RichDet/DeRichRadiator.h"

// External declarations
extern const CLID& CLID_DeRichSingleSolidRadiator;


/** @class DeRichSingleSolidRadiator DeRichSingleSolidRadiator.h
 *
 * Implementation of DeRichRadiator for single solids (can be boolean solids)
 *
 * @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */

class DeRichSingleSolidRadiator: public DeRichRadiator {

public:

  /**
   * Constructor for this class
   */
  DeRichSingleSolidRadiator();

  /**
   * Default destructor
   */
  virtual ~DeRichSingleSolidRadiator();

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
   * Finds the next intersection point with radiator.
   * @return Status of intersection
   * @retval StatusCode::FAILURE No intersection
   */
  StatusCode nextIntersectionPoint(const HepPoint3D& pGlobal,
                                   const HepVector3D& vGlobal,
                                   HepPoint3D& returnPoint) const;

  /**
   * Finds the entry and exit points of the radiator. For boolean solids
   * this is the first and last intersection point.
   * @return Status of intersection
   * @retval StatusCode::FAILURE if there is no intersection
   */
  virtual StatusCode intersectionPoints(const HepPoint3D& position,
                                        const HepVector3D& direction,
                                        HepPoint3D& entryPoint,
                                        HepPoint3D& exitPoint ) const;
  /**
   * Finds the intersection points with radiator. For boolean solids there
   * can be more than two intersection points
   *
   * @return The number of intersection points.
   * @retval Zero if there is no intersction.
   */
  unsigned int intersectionPoints(const HepPoint3D& pGlobal,
                                  const HepVector3D& vGlobal,
                                  std::vector<HepPoint3D>& points) const;


private:

  const ISolid* m_solid;  ///< topmost solid of the radiator

};

#endif    // RICHDET_DERICHSINGLESOLIDRADIATOR_H
