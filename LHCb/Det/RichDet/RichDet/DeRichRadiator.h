// $Id: DeRichRadiator.h,v 1.4 2003-12-03 17:46:47 papanest Exp $

#ifndef RICHDET_DERICHRADIATOR_H
#define RICHDET_DERICHRADIATOR_H 1

// Include files
#include "CLHEP/Geometry/Point3D.h"

// DetDesc
#include "DetDesc/DetectorElement.h"
#include "DetDesc/ISolid.h"
#include "DetDesc/TabulatedProperty.h"
#include "DetDesc/Material.h"

#include "RichKernel/RichRadiatorType.h"
#include "RichKernel/RichDetectorType.h"


/** @class DeRichRadiator DeRichRadiator.h
 *
 * This is the common base class of the Rich Single/Multi Solid Radiator 
 * detector classes
 *
 * @author Antonis Papanestis
 */

class DeRichRadiator: public DetectorElement {

public:

  virtual StatusCode initialize();
  
  /**
   * Retrieves the id of this radiator
   * @return the id of this radiator (Aerogel, C4F10, CF4)
   */
  inline virtual Rich::RadiatorType radiatorID() {
    return m_radiatorID;
  }
  
  /**
   * Retrieves the rich of this radiator
   * @return the rich of this radiator (Rich1/2)
   */
  inline virtual Rich::DetectorType rich() {
    return m_rich;
  }
  

  /**
   * Finds the next intersection point with radiator.
   * @return FAILURE if there is no intersection
   */
  virtual StatusCode nextIntersectionPoint(const HepPoint3D& pGlobal,
                                   const HepVector3D& vGlobal,
                                   HepPoint3D& returnPoint) = 0;
  /**
   * Finds the entry and exit points of the radiator.
   * @return FAILURE if there is no intersection
   */
  virtual StatusCode intersectionPoints(const HepPoint3D& pGlobal,
                                        const HepVector3D& vGlobal,
                                        HepPoint3D& entryPoint,
                                        HepPoint3D& exitPoint ) = 0;
  
  /**
   * Finds the intersection points with radiator.
   * @return the number of intersection points. Zero if there is no
   * intersction.
   */
  virtual unsigned int intersectionPoints(const HepPoint3D& pGlobal,
                                          const HepVector3D& vGlobal,
                                          std::vector<HepPoint3D>& points) = 0;
  

protected:
  
  Rich::RadiatorType m_radiatorID;
  Rich::DetectorType m_rich;
  
};

#endif    // RICHDET_DERICHRADIATOR_H
