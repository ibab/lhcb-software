// $Id: DeRich2HPDPanel.h,v 1.3 2003-06-20 14:34:32 papanest Exp $
#ifndef RICHDET_DERICH2HPDPANEL_H
#define RICHDET_DERICH2HPDPANEL_H 1

// Include files
#include "RichDet/DeRichHPDPanel.h"

// External declarations
extern const CLID& CLID_DeRich2HPDPanel;

/** @class DeRich2HPDPanel DeRich2HPDPanel.h
 *
 * This is the definition of the Rich HPDPanel detector class
 *
 * @author Antonis Papanestis
 */

class DeRich2HPDPanel: public DeRichHPDPanel {

public:
  /**
   * Constructor for this class
   */
  DeRich2HPDPanel();

  /**
   * Default destructor
   */
  virtual ~DeRich2HPDPanel();

  /**
   * Retrieves reference to class identifier
   * @return the class identifier for this class
   */
  const CLID& clID() const { return classID(); }

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
  inline HepPlane3D detectionPlane() const { return m_detectionPlane; }
  
protected:

  inline unsigned int PDRow(unsigned int PD) {
    return PD%m_HPDRows;
  }
  
  inline unsigned int PDCol(unsigned int PD) {
    return PD/m_HPDRows;
  }

private:

  ///
  double m_pixelSize;
  double m_siliconHalfLengthX;
  double m_siliconHalfLengthY;

  /// the Horizontal Edge of the HPD grid (beggining of numbers).
  double m_panelHorizEdge, m_fabs_panelHorizEdge;

  /// the Vertical Edge of the HPD grid. Even columns are
  /// different from odd columns in Rich2
  double m_panelVerticalEdgeEven;
  double m_panelVerticalEdgeOdd;

  /// these are the inner-most points to ensure that a point 
  /// is within HPD covered area 
  double m_panelVerticalEdge;
  
  double m_rowPitch;
  double m_columnPitch;

  /// number of HPD rows and columns
  int m_HPDRows;
  int m_HPDColumns;

  /// the top of the HPD window in silicon coordinates
  HepPoint3D m_HPDTop;
  
  /// detection plane
  HepPlane3D m_detectionPlane;

  // CRJ : cache variables
  HepTransform3D m_vectorTransf;
  const ISolid* m_HPDPanelSolid;
  std::vector<const IPVolume*> m_pvHPDMasters;
  std::vector<const IPVolume*> m_pvHPDSMasters;
  std::vector<const IPVolume*> m_pvWindows;
  std::vector<const ISolid*>   m_windowSolids;
  std::vector<HepTransform3D>  m_vectorTransfHPD2s;

};

#endif    // RICHDET_DERICH2HPDPANEL_H
