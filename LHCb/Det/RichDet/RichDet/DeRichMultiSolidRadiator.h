
/** @file DeRichMultiSolidRadiator.h
 *
 *  Header file for detector description class : DeRichMultiSolidRadiator
 *
 *  CVS Log :-
 *  $Id: DeRichMultiSolidRadiator.h,v 1.9 2005-12-14 09:34:52 papanest Exp $
 *
 *  @author Antonis Papanestis  a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */

#ifndef RICHDET_DERICHMULTISOLIDRADIATOR_H
#define RICHDET_DERICHMULTISOLIDRADIATOR_H 1

// Include files
#include "RichDet/DeRichRadiator.h"
#include "Kernel/Transform3DTypes.h"

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
   * Finds the next intersection point with radiator.
   * @return Status of intersection
   * @retval StatusCode::FAILURE No intersection
   */
  StatusCode nextIntersectionPoint(const Gaudi::XYZPoint& pGlobal,
                                   const Gaudi::XYZVector& vGlobal,
                                   Gaudi::XYZPoint& returnPoint) const;

  /**
   * Finds the entry and exit points of the radiator after looking
   * at all the solids to find the first and last intersection point.
   * @return Status of intersection
   * @retval StatusCode::FAILURE if there is no intersection
   */
  virtual StatusCode intersectionPoints(const Gaudi::XYZPoint& position,
                                        const Gaudi::XYZVector& direction,
                                        Gaudi::XYZPoint& entryPoint,
                                        Gaudi::XYZPoint& exitPoint ) const;
  /**
   * Finds the intersection points with radiator after looking at all the
   * solids
   *
   * @return The number of intersection points.
   * @retval Zero if there is no intersction.
   */
  unsigned int intersectionPoints(const Gaudi::XYZPoint& pGlobal,
                                  const Gaudi::XYZVector& vGlobal,
                                  std::vector<Gaudi::XYZPoint>& points) const;


private:

  StatusCode addVolumes(const ILVolume* lv, 
                        const std::string volName,
                        const Gaudi::Transform3D& toUpperLevel);


  /// vector of solids
  typedef std::vector<const ISolid*> Solids;
  /// vector of physical volumes
  typedef std::vector<const IPVolume*> PVolumes;
  /// vector of transformation matrices
  typedef std::vector<Gaudi::Transform3D> Transforms;
   /// vector of tabulated properties
  typedef std::vector<const TabulatedProperty*> TabProps;

  Solids m_solids;        ///< The solids that make up this radiator
  PVolumes m_pVolumes;    ///< The physical volumes
  Transforms m_toTopLevel; ///< Tranforms to top level of the detector element
  Transforms m_toLowLevel; ///< Tranforms to low (solid) level of the detector element
  TabProps m_refIndices;  ///< refractive indices (for each solid)
  TabProps m_rayleighVector; ///< Rayleigh scattering properties
};

#endif    // RICHDET_DERICHMULTISOLIDRADIATOR_H
