
/** @file DeRich1HPDPanel.h
 *
 *  Header file for detector description class : DeRich1HPDPanel
 *
 *  CVS Log :-
 *  $Id: DeRich1HPDPanel.h,v 1.14 2004-10-20 16:16:36 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.13  2004/10/18 09:21:49  jonrob
 *  Minor updates to functions (adding const etc.)
 *
 *  Revision 1.12  2004/09/16 08:52:26  papanest
 *  bug with row/column calculation
 *
 *  Revision 1.11  2004/07/27 08:55:22  jonrob
 *  Add doxygen file documentation and CVS information
 *
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */

#ifndef RICHDET_DERICH1HPDPANEL_H
#define RICHDET_DERICH1HPDPANEL_H 1

// Include files

#include "RichDet/DeRichHPDPanel.h"

// External declarations
extern const CLID& CLID_DeRich1HPDPanel;

/** @class DeRich1HPDPanel DeRich1HPDPanel.h
 *
 * This is the definition of the Rich1 HPDPanel detector class.
 * This HPD panel grows in rows, and each row has the same number
 * of HPDs.  So, the numbering is a little like this:
 *
 * 00  01  02  03             
 *   04  05  06  07
 * 08  09  10  11
 *   12  13  14  15  and so on..
 *
 * Numbers 00 04 08 12 ... belong to the same column.
 * (please look at the actual .h file to see the correct layout of HPDs)
 *
 * @author Antonis Papanestis
 * @date   2004-06-18
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
  const CLID& clID() const 
  {
    return classID();
  }

  /**
   * Retrieves reference to class identifier
   * @return the class identifier for this class
   */
  static const CLID& classID();

  /**
   * This is where most of the geometry is read and variables initialised
   * @return Status of initialisation
   * @retval StatusCode::FAILURE Initialisation failed, program should
   * terminate
   */
  virtual StatusCode initialize();

  /**
   * Retrieves the detection plane of the HPD panel. The plane is defined
   * at the top of the HPDs (a plane resting on the HPDs "touching" the
   * INSIDE surface of the window).
   * @return The detection plane
   */
  inline virtual const HepPlane3D & detectionPlane() const 
  {
    return m_detectionPlane;
  }

  /**
   * Returns the offset (y in Rich1) so that the two panels of
   * each detector appear side-by-side using the globalToPanel method.
   * @return The offset for the globalToPanel method
   */
  virtual const double localOffset() const 
  {
    return m_detPlaneVertEdge;
  }

  /**
   * Returns the global position given a local position and panel number.
   * Assumes a shifted panel
   *
   * @return Global point.
   */
  virtual HepPoint3D globalPosition( const HepPoint3D& localPoint,
                                     Rich::Side side);

protected:

  /**
   * Returns the number of HPDs in the panel
   */
  inline virtual unsigned int PDMax() const
  {
    return  m_HPDColumns * m_HPDRows;
  }

  /**
   * Returns the HPD row in the panel, given the HPD number
   */
  inline unsigned int PDRow(const unsigned int PD) const 
  {
    return PD/m_HPDColumns;
  }

  /**
   * Returns the HPD column in the panel, given the HPD number
   */
  inline unsigned int PDCol(const unsigned int PD) const
  {
    return PD%m_HPDColumns;
  }

  /**
   * Returns the HPD at the next row/column depending on panel configurartion
   */
  inline virtual unsigned int HPDForNS() const 
  {
    return m_HPDColumns;
  }

  /**
   * Returns an HPD number that can be used as the second point for the
   * detection plane.
   */
  inline virtual unsigned int HPDForB() const 
  {
    return m_HPDColumns -1;
  }

  /**
   * Returns an HPD number that can be used as the third point for the
   * detection plane.
   */
  inline virtual unsigned int HPDForC() const 
  {
    return m_HPDRows*m_HPDColumns - static_cast<unsigned int>(0.5*m_HPDRows);
  }

  /**
   * Finds the HPD row and column that corresponds to the x,y coordinates
   * of a point in the panel. The row and column are retuned in the smartID.
   *
   * @returns Status
   * @retval true   HPD is found
   * @retval false  The point is outside the coverage of the HPDs.
   */
  virtual bool findHPDRowCol(const HepPoint3D& inPanel, RichSmartID& id) const;

  /**
   * Converts an HPD row and column to a number corresponding
   * to the position of this physical volume in the physical volumes vector
   */
  inline virtual unsigned int HPDRowColToNum(const unsigned int HPDRow,
                                             const unsigned int HPDCol ) const 
  {
    return HPDRow * m_HPDColumns + HPDCol;
  }

private:

  /// The horizontal edge of the HPD grid for odd rows
  double m_panelHorizEdgeOdd;

  /// The horizontal edge of the HPD grid for even rows
  double m_panelHorizEdgeEven;

  /// The smaller of the above (in absolute value) for ensuring HPD coverage
  double m_panelHorizEdge;

  /// The vertical Edge of the HPD grid.
  double m_panelVerticalEdge;

};

#endif    // DERICH1HPDPANEL_H
