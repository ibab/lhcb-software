// $Id: DeRichRadiator.h,v 1.2 2003-04-01 13:01:49 jonrob Exp $

#ifndef DERICHRADIATOR_H
#define DERICHRADIATOR_H 1


// Include files
#include "CLHEP/Geometry/Point3D.h"

#include "DetDesc/DetectorElement.h"
#include "DetDesc/ISolid.h"

#include "RichKernel/RichRadiatorType.h"

// External declarations
extern const CLID& CLID_DeRichRadiator;


/** @class DeRichRadiator DeRichRadiator.h
 *
 * This is the definition of the Rich Radiator detector class
 *
 * @author Antonis Papanestis
 */

class DeRichRadiator: public DetectorElement {

public:

  /**
   * Constructor for this class
   */
  DeRichRadiator();

  /**
   * Default destructor
   */
  virtual ~DeRichRadiator();

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
   * Retrieves the id of this radiator
   * @return the id of this radiator (Aerogel, C4F10, CF4)
   */
  inline  Rich::RadiatorType radiatorID(){
    return m_radiatorID;
  }

  /**
   * Finds the next intersection point with radiator.
   * @return FAILURE if there is no intersection
   */
  StatusCode nextIntersectionPoint(const HepPoint3D& pGlobal,
                                   const HepVector3D& vGlobal,
                                   HepPoint3D& returnPoint);
  
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
  
  Rich::RadiatorType m_radiatorID;
  
};

#endif    // DERICHRADIATOR_H
