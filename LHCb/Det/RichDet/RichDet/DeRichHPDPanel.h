// $Id: DeRichHPDPanel.h,v 1.1 2002-07-16 16:02:35 papanest Exp $

#ifndef DERICHHPDPANEL_H
#define DERICHHPDPANEL_H 1


// Include files
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"

#include "DetDesc/DetectorElement.h"
#include "DetDesc/ISolid.h"

#include "RichKernel/RichSmartID.h"

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
   * @return RichSmartID identifying the exact pixel location in the Rich
   * system.
   */
  StatusCode smartID(const HepPoint3D& globalPoint, RichSmartID& id);

  /**
   * Returns 
   * @return
   */
  StatusCode detectionPoint(const RichSmartID& smartID, 
                            HepPoint3D& windowHitGlobal); // this is the HPD
                                                          // window
  
private:

  ///
  double pixelSize;
  double siliconHalfLengthX;
  double siliconHalfLengthY;

  /// the leftmost point of the HPD grid (x coord)
  double panelLeft;

  /// the bottommost point of the HPD grid (y coord). Even columns are
  /// different from odd columns
  double panelBottomEven;
  double panelBottomOdd;

  double rowPitch;
  double columnPitch;
  /// the top of the HPD window in silicon coordinates
  HepPoint3D HPDTop;
  
};

#endif    // DERICHHPDPANEL_H
