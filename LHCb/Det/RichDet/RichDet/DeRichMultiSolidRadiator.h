// $Id: DeRichMultiSolidRadiator.h,v 1.2 2004-07-22 10:49:55 papanest Exp $

#ifndef RICHDET_DERICHMULTISOLIDRADIATOR_H
#define RICHDET_DERICHMULTISOLIDRADIATOR_H 1

// Include files
#include "RichDet/DeRichRadiator.h"

// External declarations
extern const CLID& CLID_DeRichMultiSolidRadiator;


/** @class DeRichMultiSolidRadiator DeRichMultiSolidRadiator.h
 *
 * Implementation of the DeRichRadiator for a radiator with multiple solids.
 * WARNING: The detector element is attached to the physical volume that
 * contain all the physical volumes of the radiator material, so the use of
 * geometryInfo should be done with care.
 *
 * @author Antonis Papanestis a.papanestis@rl.ac.uk
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
                                   HepPoint3D& returnPoint);

  /**
   * Finds the entry and exit points of the radiator after looking
   * at all the solids to find the first and last intersection point.
   * @return Status of intersection
   * @retval StatusCode::FAILURE if there is no intersection
   */
  virtual StatusCode intersectionPoints(const HepPoint3D& position,
                                        const HepVector3D& direction,
                                        HepPoint3D& entryPoint,
                                        HepPoint3D& exitPoint );
  /**
   * Finds the intersection points with radiator after looking at all the
   * solids
   *
   * @return The number of intersection points.
   * @retval Zero if there is no intersction.
   */
  unsigned int intersectionPoints(const HepPoint3D& pGlobal,
                                  const HepVector3D& vGlobal,
                                  std::vector<HepPoint3D>& points);


private:

  /// vector of solids
  typedef std::vector<const ISolid*> Solids;
  /// vector of physical volumes
  typedef std::vector<const IPVolume*> PVolumes;
  /// vector of refractive indices
  typedef std::vector<const TabulatedProperty*> RIndices;

  Solids m_solids;        ///< The solids that make up this radiator
  PVolumes m_pVolumes;    ///< The physical volumes
  RIndices m_refIndices;  ///< refractive indices (for each solid)
};

#endif    // RICHDET_DERICHMULTISOLIDRADIATOR_H
