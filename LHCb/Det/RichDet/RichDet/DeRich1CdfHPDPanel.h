// $Id: DeRich1CdfHPDPanel.h,v 1.6 2003-10-22 10:48:28 papanest Exp $
#ifndef RICHDET_DERICH1CDFHPDPANEL_H
#define RICHDET_DERICH1CDFHPDPANEL_H 1

// Include files
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

  StatusCode PDWindowPoint(const HepVector3D& vGlobal, // vector and point
                            const HepPoint3D& pGlobal,  // define direction
                            HepPoint3D& windowPointGlobal, // return point
                            RichSmartID& smartID );
  /**
   * Returns the detection plane of the HPD panel, defined at the top of the 
   * HPDs (a plane resting on the HPDs touching the window).
   * @return HepPlane3D
   */
  inline HepPlane3D detectionPlane() const  {
    return m_detectionPlane;
  }

protected:
  
  inline unsigned int PDRow(unsigned int PD) {
    int HPDsLeft = PD%HPDsIn2Cols;
    if (HPDsLeft >= HPDsInBigCol){
      return (HPDsLeft - HPDsInBigCol);
    } else {
      return HPDsLeft;
    }
  }
  
  inline unsigned int PDCol(unsigned int PD) {
    int HPDsLeft = PD%HPDsIn2Cols;
    if (HPDsLeft >= HPDsInBigCol) {
      return 2*(PD/HPDsIn2Cols)+1;
    } else {
      return 2*(PD/HPDsIn2Cols);
    }
    
    
  }
  
private:

  ///
  double pixelSize;
  double siliconHalfLengthX;
  double siliconHalfLengthY;

  /// the Horizontal Edge of the HPD grid (beggining of numbers). 
  double panelHorizEdge;
  double m_fabs_panelHorizEdge;
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

  /// the top of the HPD window in silicon coordinates
  HepPoint3D HPDTop;
  
  // CRJ : cache variables
  const ISolid* m_HPDPanelSolid;
  std::vector<const IPVolume*> m_pvHPDMasters;
  std::vector<const IPVolume*> m_pvHPDSMasters;
  std::vector<const IPVolume*> m_pvWindows;
  std::vector<const ISolid*>   m_windowSolids;
  std::vector<HepTransform3D>  m_vectorTransfHPD2s;

};

#endif    // RICHDET_DERICH1CDFHPDPANEL_H
