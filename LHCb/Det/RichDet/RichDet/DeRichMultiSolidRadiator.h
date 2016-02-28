
/** @file DeRichMultiSolidRadiator.h
 *
 *  Header file for detector description class : DeRichMultiSolidRadiator
 *
 *  @author Antonis Papanestis  a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */

#ifndef RICHDET_DERICHMULTISOLIDRADIATOR_H
#define RICHDET_DERICHMULTISOLIDRADIATOR_H 1

// Include files
#include "RichDet/DeRichRadiator.h"
#include "GaudiKernel/Transform3DTypes.h"

// External declarations
extern const CLID CLID_DeRichMultiSolidRadiator;

/** @class DeRichMultiSolidRadiator RichDet/DeRichMultiSolidRadiator.h
 *
 * Implementation of the DeRichRadiator for a radiator with multiple solids.
 * WARNING: The detector element is attached to the physical volume that
 * contain all the physical volumes of the radiator material, so the use of
 * geometryInfo should be done with care.
 *
 * @author Antonis Papanestis a.papanestis@rl.ac.uk
 */

class DeRichMultiSolidRadiator : public DeRichRadiator
{

public:

  /**
   * Constructor for this class
   */
  DeRichMultiSolidRadiator( const std::string & name = "" );

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

  // Finds the next intersection point with radiator.
  StatusCode nextIntersectionPoint( const Gaudi::XYZPoint& pGlobal,
                                    const Gaudi::XYZVector& vGlobal,
                                    Gaudi::XYZPoint& returnPoint ) const;

  // Finds the entry and exit points of the radiator after looking
  // at all the solids to find the first and last intersection point.
  virtual StatusCode intersectionPoints( const Gaudi::XYZPoint& position,
                                         const Gaudi::XYZVector& direction,
                                         Gaudi::XYZPoint& entryPoint,
                                         Gaudi::XYZPoint& exitPoint ) const;

  // Finds the intersection points with radiator after looking at all the solids
  virtual unsigned int intersectionPoints( const Gaudi::XYZPoint& pGlobal,
                                           const Gaudi::XYZVector& vGlobal,
                                           std::vector<Gaudi::XYZPoint>& points ) const;

  // Finds the intersections (entry/exit) with radiator. For boolean solids there
  virtual unsigned int intersections( const Gaudi::XYZPoint& pGlobal,
                                      const Gaudi::XYZVector& vGlobal,
                                      Rich::RadIntersection::Vector& intersections ) const;

  // Returns the refractive index at the given photon energy for this radiator
  virtual double refractiveIndex( const double energy, const bool hlt = false ) const;

public:

  /** Access to the list of single solid radiators. 
   *  Will be either sub-tiles, or tiles, depending on which are active in the DB
   */
  inline const DeRichRadiator::Vector& radiators() const { return m_radiators; }

  /// Explicit Access to sub-tiles (if active)
  inline const DeRichRadiator::Vector& subTileRadiators() const { return radiators(); }

  /// Explicit Access to full tiles
  inline const DeRichRadiator::Vector& fullTileRadiators() const { return m_fullTiles; }

private:

  /// Add radiator volumes
  StatusCode addVolumes(const ILVolume* lv,
                        const std::string& volName,
                        const Gaudi::Transform3D& toUpperLevel, const int VolIndex);

  StatusCode addFullTileVolumes(const ILVolume* lv,
                        const std::string& volName,
                        const Gaudi::Transform3D& toUpperLevel);

  StatusCode addSubTileVolumes(const ILVolume* lv,
                        const std::string& volName,
                        const Gaudi::Transform3D& toUpperLevel);

  /// UMS update on geometry changes
  StatusCode geometryUpdate();

private:

  /// vector of solids
  typedef std::vector<const ISolid*> Solids;
  /// vector of physical volumes
  typedef std::vector<const IPVolume*> PVolumes;
  /// vector of transformation matrices
  typedef std::vector<Gaudi::Transform3D> Transforms;

  Solids m_solids;         ///< The solids that make up this radiator
  PVolumes m_pVolumes;     ///< The physical volumes
  Transforms m_toTopLevel; ///< Tranforms to top level of the detector element
  Transforms m_toLowLevel; ///< Tranforms to low (solid) level of the detector element

  /// The DeRichRadiators (sub-tiles) that make up this radiator
  DeRichRadiator::Vector m_radiators;

  /// Explicit list of full tiles
  DeRichRadiator::Vector m_fullTiles;

  /// First update flag
  bool m_firstUpdate{true};

};

#endif    // RICHDET_DERICHMULTISOLIDRADIATOR_H
