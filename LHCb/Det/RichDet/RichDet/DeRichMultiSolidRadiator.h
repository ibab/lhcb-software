// $Id: DeRichMultiSolidRadiator.h,v 1.1 2003-12-03 17:46:47 papanest Exp $

#ifndef RICHDET_DERICHMULTISOLIDRADIATOR_H
#define RICHDET_DERICHMULTISOLIDRADIATOR_H 1

// Include files
#include "RichDet/DeRichRadiator.h"

// External declarations
extern const CLID& CLID_DeRichMultiSolidRadiator;


/** @class DeRichMultiSolidRadiator DeRichMultiSolidRadiator.h
 *
 * This is the definition of the Rich Radiator detector class
 *
 * @author Antonis Papanestis
 */

class DeRichMultiSolidRadiator: public DeRichRadiator {

public:

  /**
   * Constructor for this class
   */
  DeRichMultiSolidRadiator();

  /**
   * Default destructor
   */
  virtual ~DeRichMultiSolidRadiator();

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
  typedef std::vector<const ISolid*> Solids;
  typedef std::vector<const IPVolume*> PVolumes;
  typedef std::vector<const TabulatedProperty*> RIndices;

  Solids m_solids;  
  PVolumes m_pVolumes;
  RIndices m_refIndices;
  
};

#endif    // RICHDET_DERICHMULTISOLIDRADIATOR_H
