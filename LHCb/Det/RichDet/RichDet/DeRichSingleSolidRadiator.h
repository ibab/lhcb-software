
//----------------------------------------------------------------------------
/** @file DeRichSingleSolidRadiator.h
 *
 *  Header file for detector description class : DeRichSingleSolidRadiator
 *
 *  CVS Log :-
 *  $Id: DeRichSingleSolidRadiator.h,v 1.7 2005-10-14 08:21:37 jonrob Exp $
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */
//----------------------------------------------------------------------------

#ifndef RICHDET_DERICHSINGLESOLIDRADIATOR_H
#define RICHDET_DERICHSINGLESOLIDRADIATOR_H 1

// Include files
#include "RichDet/DeRichRadiator.h"

// External declarations
extern const CLID& CLID_DeRichSingleSolidRadiator;

//----------------------------------------------------------------------------
/** @class DeRichSingleSolidRadiator DeRichSingleSolidRadiator.h
 *
 * Implementation of DeRichRadiator for single solids (can be boolean solids)
 *
 * @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */
//----------------------------------------------------------------------------

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
  inline const CLID& clID() const 
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
