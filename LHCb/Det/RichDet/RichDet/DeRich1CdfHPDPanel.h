// $Id: DeRich1CdfHPDPanel.h,v 1.8 2003-11-22 18:40:49 jonesc Exp $
#ifndef RICHDET_DERICH1CDFHPDPANEL_H
#define RICHDET_DERICH1CDFHPDPANEL_H 1

// Include files
#include "RichDet/DeRichHPDPanel.h"

// External declarations
extern const CLID& CLID_DeRich1CdfHPDPanel;

/** @class DeRich1CdfHPDPanel DeRich1CdfHPDPanel.h
 *
 * This is the definition of the Rich1 HPDPanel detector class.
 * This HPD panel grows in columns, with the first and last (and some middle) 
 * columns having an extra HPD.  The numbering scheme look like this:
 *
 *
 * 3   10
 *   6
 * 2   9
 *   5
 * 1   8   and so on...
 *   4
 * 0   7        numbers 0,4,7... belong to the same row. The top row has some
 *              gaps.
 * Note that even columns (starting at 0) have an extra HPD, and that goes for 
 * the 1st and last columns.
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
    return static_cast<int>(0.5*m_HPDColumns)*m_HPDsIn2Cols + m_HPDsInBigCol;
  }
  
  /**  
   * Returns the HPD row in the panel, given the HPD number
   */
  inline unsigned int PDRow(unsigned int PD) {
    unsigned int HPDsLeft = PD%m_HPDsIn2Cols;
    if (HPDsLeft >= m_HPDsInBigCol){
      return (HPDsLeft - m_HPDsInBigCol);
    } else {
      return HPDsLeft;
    }
  }
  
  /**  
   * Returns the HPD column in the panel, given the HPD number
   */
  inline unsigned int PDCol(unsigned int PD) {
    unsigned int HPDsLeft = PD%m_HPDsIn2Cols;
    if (HPDsLeft >= m_HPDsInBigCol) {
      return 2*(PD/m_HPDsIn2Cols)+1;
    } else {
      return 2*(PD/m_HPDsIn2Cols);
    }
    
    
  }
  
  /**  
   * Returns the HPD at the next row/column depending on panel configurartion 
   */
  inline virtual unsigned int HPDForNS() {
    return static_cast<int>(ceil(m_HPDRows/2.0));
  }

  /**  
   * Returns an HPD number that can be used as the second point for the
   * detection plane.
   */
  inline virtual unsigned int HPDForB() {
    return static_cast<int>(ceil(m_HPDRows/2.0)) -1;
  }
  
  /**  
   * Returns an HPD number that can be used as the third point for the
   * detection plane.
   */
  inline virtual unsigned int HPDForC() {
    return static_cast<int>(0.5*m_HPDColumns)*m_HPDRows;
  }

  /**  
   * Converts an HPD row and column to a number corresponding
   * to the position of this physical volume in the physical volumes vector 
   */
  inline virtual unsigned int HPDRowColToNum(unsigned int HPDRow, 
                                             unsigned int HPDCol ) {
    unsigned int HPDNumber;
    unsigned int HPDColDiv2 = HPDCol/2;
    if (0 == HPDCol%2) {
       HPDNumber = HPDColDiv2 * m_HPDsIn2Cols + HPDRow;
    } else {
       HPDNumber = HPDColDiv2 * m_HPDsIn2Cols + HPDRow + m_HPDsInBigCol;
    }
    return HPDNumber;
  }

  /**  
   * Finds the HPD row and column that corresponds to the x,y coordinates
   * of a point in the panel.
   * @returns true if the HPD is found, false if the point is outside the
   * coverage of the HPDs. The row and column are retuned in the smartID.
   */
  virtual bool findHPDRowCol(const HepPoint3D& inPanel, RichSmartID& id);

private:


  /// the Horizontal Edge of the HPD grid (beggining of numbers). 
  double m_panelHorizEdge;
  double m_fabs_panelHorizEdge;
  double m_panelHorizEdgeEven;
  double m_panelHorizEdgeOdd;

  /// the Vertical Edge of the HPD grid. Even columns are
  /// different from odd columns in Rich1Cdf
  double m_panelVerticalEdgeEven;
  double m_panelVerticalEdgeOdd;

  /// these are the inner-most points to ensure that a point 
  /// is within HPD covered area 
  double m_panelVerticalEdge;  

};

#endif    // RICHDET_DERICH1CDFHPDPANEL_H
