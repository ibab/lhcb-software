// $Id: DeRich1HPDPanel.h,v 1.7 2003-11-22 18:40:50 jonesc Exp $
#ifndef RICHDET_DERICH1HPDPANEL_H
#define RICHDET_DERICH1HPDPANEL_H 1

// Include files

#include "RichDet/DeRichHPDPanel.h"

// External declarations
extern const CLID& CLID_DeRich1HPDPanel;

/** @class DeRich1HPDPanel DeRich1HPDPanel.h
 *
 * This is the definition of the Rich1 HPDPanel detector class
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
   * Returns the detection plane of the HPD panel, defined at the top of the 
   * HPDs (a plane resting on the HPDs touching the window).
   * @return HepPlane3D
   */
  inline virtual const HepPlane3D & detectionPlane() const { return m_detectionPlane; }
  
protected:

  /**  
   * Returns the number of HPDs in the panel
   */
  inline virtual unsigned int PDMax() {
    return  m_HPDColumns * m_HPDRows;
  }

  /**  
   * Returns the HPD row in the panel, given the HPD number
   */
  inline unsigned int PDRow(unsigned int PD) {
    return PD/m_HPDRows;
  }
  
  /**  
   * Returns the HPD column in the panel, given the HPD number
   */
  inline unsigned int PDCol(unsigned int PD) {
    return PD%m_HPDRows;
  }
  
  inline virtual unsigned int HPDForNS() {
    return m_HPDColumns;
  }

  inline virtual unsigned int HPDForB() {
    return m_HPDColumns -1;
  }
  
  inline virtual unsigned int HPDForC() {
    return m_HPDRows*m_HPDColumns - static_cast<unsigned int>(0.5*m_HPDRows);
  }

  /**  
   * Finds the HPD row and column that corresponds to the x,y coordinates
   * of a point in the panel.
   * @returns true if the HPD is found, false if the point is outside the
   * coverage of the HPDs. The row and column are retuned in the smartID.
   */
  virtual bool findHPDRowCol(const HepPoint3D& inPanel, RichSmartID& id);

  /**  
   * Converts an HPD row and column to a number corresponding
   * to the position of this physical volume in the physical volumes vector 
   */
  inline virtual unsigned int HPDRowColToNum(unsigned int HPDRow, 
                                             unsigned int HPDCol ) {
      return HPDRow * m_HPDColumns + HPDCol;
  }

private:



  /// the Horizontal Edge of the HPD grid (beggining of numbers). Even rows 
  /// are differenent from odd rows in Rich1
  double m_panelHorizEdgeOdd;
  double m_panelHorizEdgeEven;

  /// the Vertical Edge of the HPD grid.
  double m_panelVerticalEdge;

  /// these are the inner-most points to ensure that a point 
  /// is within HPD covered area 
  double m_panelHorizEdge;  
  
};

#endif    // DERICH1HPDPANEL_H
