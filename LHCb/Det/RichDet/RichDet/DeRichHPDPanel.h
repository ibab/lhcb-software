// $Id: DeRichHPDPanel.h,v 1.2 2002-10-30 11:36:56 papanest Exp $

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
  virtual StatusCode initialize();
  

  /**
   * Returns a RichSmartID for a given point in global coordinates.
   * @return StatusCode
   */
  StatusCode smartID(const HepPoint3D& globalPoint, RichSmartID& id);

  /**
   * Returns the detection point given a smartID
   * @return StatusCode
   */
  StatusCode detectionPoint(const RichSmartID& smartID, 
                            HepPoint3D& windowHitGlobal); // this is the HPD
                                                          // window
  /**
   * Returns the intersection point with an HPD window given a vector 
   * and a point.
   * @return StatusCode
   */

  StatusCode HPDWindowPoint(const HepVector3D& vGlobal, // vector and point
                            const HepPoint3D& pGlobal,  // define direction
                            HepPoint3D& windowPointGlobal, // return point
                            RichSmartID& smartID );
  /**
   * Returns the detection plane of the HPD panel, defined at the top of the 
   * HPDs (a plane resting on the HPDs touching the window).
   * @return HepPlane3D
   */
  inline HepPlane3D detectionPlane() const  {
    return detectionPlane_m;
  }
  
  
  
private:

  ///
  double pixelSize;
  double siliconHalfLengthX;
  double siliconHalfLengthY;

  /// the Horizontal Edge of the HPD grid (beggining of numbers). Even rows 
  /// are differenent from odd rows in Rich1
  double panelHorizEdgeOdd;
  double panelHorizEdgeEven;

  /// the Vertical Edge of the HPD grid. Even columns are
  /// different from odd columns in Rich2
  double panelVerticalEdgeEven;
  double panelVerticalEdgeOdd;

  /// these are the inner-most points to ensure that a point 
  /// is within HPD covered area 
  double panelVerticalEdge;
  double panelHorizEdge;
  
  double rowPitch;
  double columnPitch;

  /// number of HPD rows and columns
  int HPDRows;
  int HPDColumns;

  /// the top of the HPD window in silicon coordinates
  HepPoint3D HPDTop;
  
  /// 
  HepPlane3D detectionPlane_m;

  /// deferenciate between Rich1 and Rich2
  bool m_Rich2;
  
};

#endif    // DERICHHPDPANEL_H
