
//----------------------------------------------------------------------------
/** @file DeRichRadiator.h
 *
 *  Header file for detector description class : DeRichRadiator
 *
 *  CVS Log :-
 *  $Id: DeRichRadiator.h,v 1.14 2005-02-23 10:26:00 jonrob Exp $
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */
//----------------------------------------------------------------------------

#ifndef RICHDET_DERICHRADIATOR_H
#define RICHDET_DERICHRADIATOR_H 1

// Include files
#include "CLHEP/Geometry/Point3D.h"

// DetDesc
#include "DetDesc/DetectorElement.h"
#include "DetDesc/ISolid.h"
#include "DetDesc/TabulatedProperty.h"
#include "DetDesc/Material.h"
#include "DetDesc/IGeometryInfo.h"

#include "Kernel/RichRadiatorType.h"
#include "Kernel/RichDetectorType.h"

/** @namespace DeRichRadiatorLocation
 *
 *  Namespace for the location of radiators in xml
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */
namespace DeRichRadiatorLocation {
  /// Aerogel segment Q0 location
  static const std::string& AerogelQ0 = "/dd/Structure/LHCb/Rich1/AerogelQ0";
  /// Aerogel segment Q1 location
  static const std::string& AerogelQ1 = "/dd/Structure/LHCb/Rich1/AerogelQ1";
  /// Aerogel segment Q2 location
  static const std::string& AerogelQ2 = "/dd/Structure/LHCb/Rich1/AerogelQ2";
  /// Aerogel segment Q3 location
  static const std::string& AerogelQ3 = "/dd/Structure/LHCb/Rich1/AerogelQ3";
  /// Multi solid aerogel location
  static const std::string& Aerogel = "/dd/Structure/LHCb/Rich1/Aerogel";
  /// Rich1 gas (C4F10) location
  static const std::string& C4F10     = "/dd/Structure/LHCb/Rich1/C4F10";
  /// Rich2 gas (CF4) location
  static const std::string& CF4       = "/dd/Structure/LHCb/Rich2/CF4";
}

//----------------------------------------------------------------------------
/** @class DeRichRadiator DeRichRadiator.h
 *
 * DeRichRadiator provides information about the Rich radiators.
 * This is the common base class of the Rich Single/Multi Solid Radiator
 * detector classes
 *
 * @author Antonis Papanestis a.papanestis@rl.ac.uk
 */
//----------------------------------------------------------------------------

class DeRichRadiator: public DetectorElement {

public:

  /// Default constructor
  DeRichRadiator::DeRichRadiator()
    : DetectorElement (),
      m_radiatorID    ( Rich::InvalidRadiator ),
      m_rich          ( Rich::InvalidDetector ),
      m_refIndex      ( 0                     ),
      m_rayleigh      ( 0                     ),
      m_name          ( "UnInitialized"       ) { }

  /**
   * This is where most of the geometry is read and variables initialised
   * @return Status of initialisation
   * @retval StatusCode::FAILURE Initialisation failed, program should
   * terminate
   */
  virtual StatusCode initialize();

  /**
   * Retrieves the id of this radiator
   * @return The id of this radiator (Aerogel, C4F10, CF4)
   */
  inline Rich::RadiatorType radiatorID() const
  {
    return m_radiatorID;
  }

  /**
   * Retrieves the rich detector of this radiator
   * @return the rich detector of this radiator (Rich1/2)
   */
  inline Rich::DetectorType rich() const
  {
    return m_rich;
  }

  /**
   * Retrieves the refractive index of the radiator
   * @return A pointer to the refractive index of the radiator
   */
  inline const TabulatedProperty* refIndex() const
  {
    return m_refIndex;
  }

  /**
   * Retrieves the Rayleigh properties of the radiator
   * @return A pointer to the Rayleigh tab property
   * @retval NULL No Rayleigh tab property
   */
  inline const TabulatedProperty* rayleigh() const
  {
    return m_rayleigh;
  }

  /**
   * Finds the next intersection point with radiator.
   * @return Status of intersection
   * @retval StatusCode::FAILURE No intersection
   */
  virtual StatusCode nextIntersectionPoint( const HepPoint3D& pGlobal,
                                            const HepVector3D& vGlobal,
                                            HepPoint3D& returnPoint ) const = 0;
  /**
   * Finds the entry and exit points of the radiator. For boolean solids
   * this is the first and last intersection point.
   * @return Status of intersection
   * @retval StatusCode::FAILURE if there is no intersection
   */
  virtual StatusCode intersectionPoints( const HepPoint3D& pGlobal,
                                         const HepVector3D& vGlobal,
                                         HepPoint3D& entryPoint,
                                         HepPoint3D& exitPoint ) const = 0;

  /**
   * Finds the intersection points with radiator. For boolean solids there
   * can be more than two intersection points
   *
   * @return The number of intersection points.
   * @retval Zero if there is no intersction.
   */
  virtual unsigned int intersectionPoints( const HepPoint3D& pGlobal,
                                           const HepVector3D& vGlobal,
                                           std::vector<HepPoint3D>& points ) const = 0;


  /** Returns the name of this particular radiator medium
   *  @return radiator name
   */
  inline const std::string & myName() const { return m_name; }

protected:

  Rich::RadiatorType m_radiatorID;  ///< The radiator id (Aerogel, CF4, C4F10)
  Rich::DetectorType m_rich;        ///< The Rich detector of this radiator

  /// pointer to the refractive index of the material
  const TabulatedProperty* m_refIndex;

  /// pointer to the Rayleigh scattering properties
  const TabulatedProperty* m_rayleigh;

private:

  std::string m_name; ///< The name of this radiator

};

#endif    // RICHDET_DERICHRADIATOR_H
