// $Id: DeRich1CdfHPDPanel.h,v 1.1 2003-01-16 17:46:04 papanest Exp $

#ifndef DERICH1CDFHPDPANEL_H
#define DERICH1CDFHPDPANEL_H 1


// Include files
//#include "CLHEP/Geometry/Point3D.h"
//#include "CLHEP/Geometry/Vector3D.h"
//#include "CLHEP/Geometry/Plane3D.h"

//#include "DetDesc/DetectorElement.h"
//#include "DetDesc/ISolid.h"

//#include "RichKernel/RichSmartID.h"
//#include "RichKernel/RichDefinitions.h"

#include "RichDet/DeRichHPDPanel.h"

// External declarations
extern const CLID& CLID_DeRich1CdfHPDPanel;


/** @class DeRich1CdfHPDPanel DeRich1CdfHPDPanel.h
 *
 * This is the definition of the Rich HPDPanel detector class
 *
 * @author Antonis Papanestis
 */
class DeRich1CdfHPDPanel: public DeRichHPDPanel {

public:
  /**
   * Constructor for this class
   */
  DeRich1CdfHPDPanel();

  /**
   * Default destructor
   */
  virtual ~DeRich1CdfHPDPanel();

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

  /// the Horizontal Edge of the HPD grid (beggining of numbers). 
  double panelHorizEdge;
  double panelHorizEdgeEven;
  double panelHorizEdgeOdd;

  /// the Vertical Edge of the HPD grid. Even columns are
  /// different from odd columns in Rich1Cdf
  double panelVerticalEdgeEven;
  double panelVerticalEdgeOdd;

  /// these are the inner-most points to ensure that a point 
  /// is within HPD covered area 
  double panelVerticalEdge;
  
  double rowPitch;
  double columnPitch;

  /// number of HPD rows and columns
  int HPDRows;
  int HPDColumns;
  int HPDsInBigCol;
  int HPDsIn2Cols;
  int halfHPDCols;

  /// the top of the HPD window in silicon coordinates
  HepPoint3D HPDTop;
  
  /// 
  HepPlane3D detectionPlane_m;
  
};

#endif    // DERICH1CDFHPDPANEL_H
