// $Id: DeRichHPDPanel.h,v 1.3 2003-01-16 17:46:05 papanest Exp $

#ifndef DERICHHPDPANEL_H
#define DERICHHPDPANEL_H 1


// Include files
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Geometry/Plane3D.h"

#include "DetDesc/DetectorElement.h"
#include "DetDesc/ISolid.h"

#include "RichKernel/RichSmartID.h"
//#include "RichKernel/RichDefinitions.h"

// External declarations
extern const CLID& CLID_DeRichHPDPanel;


/** @class DeRichHPDPanel DeRichHPDPanel.h
 *
 * This is the definition of the Rich HPDPanel detector class
 *
 * @author Antonis Papanestis
 */
class DeRichHPDPanel: public DetectorElement {

public:
  /**
   * Constructor for this class
   */
  DeRichHPDPanel();

  /**
   * Default destructor
   */
  virtual ~DeRichHPDPanel();

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
   * This is where most of the geometry is read
   * @return StatusCode
   */
  virtual StatusCode initialize() = 0;
  

  /**
   * Returns a RichSmartID for a given point in global coordinates.
   * @return StatusCode
   */
  virtual StatusCode smartID(const HepPoint3D& globalPoint, 
                             RichSmartID& id) = 0;

  /**
   * Returns the detection point given a smartID
   * @return StatusCode
   */
  virtual StatusCode detectionPoint(const RichSmartID& smartID, 
                            HepPoint3D& windowHitGlobal) = 0; //this is the HPD
                                                          // window point
  /**
   * Returns the intersection point with an HPD window given a vector 
   * and a point.
   * @return StatusCode
   */

  virtual StatusCode HPDWindowPoint(const HepVector3D& vGlobal, //vector and
                            const HepPoint3D& pGlobal, //point define direction
                            HepPoint3D& windowPointGlobal, // return point
                            RichSmartID& smartID ) = 0;
  /**
   * Returns the detection plane of the HPD panel, defined at the top of the 
   * HPDs (a plane resting on the HPDs touching the window).
   * @return HepPlane3D
   */
  virtual HepPlane3D detectionPlane() const = 0;  
  
};

#endif    // DERICHHPDPANEL_H
