// $Id: DeRich1HPDPanel.h,v 1.4 2003-10-22 10:48:28 papanest Exp $

#ifndef DERICH1HPDPANEL_H
#define DERICH1HPDPANEL_H 1


// Include files
#include "RichDet/DeRichHPDPanel.h"

// External declarations
extern const CLID& CLID_DeRich1HPDPanel;


/** @class DeRich1HPDPanel DeRich1HPDPanel.h
 *
 * This is the definition of the Rich HPDPanel detector class
 *
 * @author Antonis Papanestis
 */
class DeRich1HPDPanel: public DeRichHPDPanel {

public:
  /**
   * Constructor for this class
   */
  DeRich1HPDPanel();

  /**
   * Default destructor
   */
  virtual ~DeRich1HPDPanel();

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
    return PD/HPDRows;
  }
  
  inline unsigned int PDCol(unsigned int PD) {
    return PD%HPDRows;
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

  /// the Vertical Edge of the HPD grid.
  double panelVerticalEdge;

  /// these are the inner-most points to ensure that a point 
  /// is within HPD covered area 
  double panelHorizEdge;
  
  double rowPitch;
  double columnPitch;

  /// number of HPD rows and columns
  int HPDRows;
  int HPDColumns;

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

#endif    // DERICH1HPDPANEL_H
