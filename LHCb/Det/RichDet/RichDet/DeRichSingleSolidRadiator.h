// $Id: DeRichSingleSolidRadiator.h,v 1.1 2003-12-03 17:46:47 papanest Exp $

#ifndef RICHDET_DERICHSINGLESOLIDRADIATOR_H
#define RICHDET_DERICHSINGLESOLIDRADIATOR_H 1

// Include files
#include "RichDet/DeRichRadiator.h"

// External declarations
extern const CLID& CLID_DeRichSingleSolidRadiator;


/** @class DeRichSingleSolidRadiator DeRichSingleSolidRadiator.h
 *
 * This is the definition of the Rich Radiator detector class
 *
 * @author Antonis Papanestis
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

  virtual StatusCode initialize();
  
  /**
   * Finds the next intersection point with radiator.
   * @return FAILURE if there is no intersection
   */
  StatusCode nextIntersectionPoint(const HepPoint3D& pGlobal,
                                   const HepVector3D& vGlobal,
                                   HepPoint3D& returnPoint);
  
  /**
   * Finds the entry and exit points of the radiator.
   * @return FAILURE if there is no intersection
   */
  virtual StatusCode intersectionPoints(const HepPoint3D& position,
                                        const HepVector3D& direction,
                                        HepPoint3D& entryPoint,
                                        HepPoint3D& exitPoint );
  /**
   * Finds the intersection points with radiator.
   * @return the number of intersection points. Zero if there is no
   * intersction.
   */
  unsigned int intersectionPoints(const HepPoint3D& pGlobal,
                                  const HepVector3D& vGlobal,
                                  std::vector<HepPoint3D>& points);
  

private:

  /// topmost solid of the radiator
  const ISolid* m_solid;
  /// pointer to the refractive index of the material
  const TabulatedProperty* m_refIndex;  
};

#endif    // RICHDET_DERICHSINGLESOLIDRADIATOR_H
