
/** @file DeRich1CdfHPDPanel.h
 *
 *  Header file for detector description class : DeRich1CdfHPDPanel
 *
 *  CVS Log :-
 *  $Id: DeRich1CdfHPDPanel.h,v 1.16 2004-10-20 22:41:54 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.15  2004/10/20 17:02:44  jonrob
 *  Updates for windows
 *
 *  Revision 1.14  2004/10/20 16:16:36  jonrob
 *  More minor updates to functions (adding const etc.)
 *
 *  Revision 1.13  2004/10/18 09:21:48  jonrob
 *  Minor updates to functions (adding const etc.)
 *
 *  Revision 1.12  2004/07/27 08:55:22  jonrob
 *  Add doxygen file documentation and CVS information
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */

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
 * (please look at the actual .h file to see the correct layout of HPDs)
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
   * Returns the offset (y in Rich1) so that the two panels of
   * each detector appear side-by-side using the globalToPanel method.
   * @return The offset for the globalToPanel method
   */
  virtual const double localOffset() const;

  /**
   * Returns the global position given a local position and panel number.
   * Assumes a shifted panel
   *
   * @return Global point.
   */
  virtual HepPoint3D globalPosition( const HepPoint3D& localPoint,
                                     const Rich::Side side) const;

protected:

  /**
   * Returns the number of HPDs in the panel
   */
  virtual unsigned int PDMax() const;

  /**
   * Returns the HPD row in the panel, given the HPD number
   */
  virtual unsigned int PDRow(const unsigned int PD) const;

  /**
   * Returns the HPD column in the panel, given the HPD number
   */
  virtual unsigned int PDCol(const unsigned int PD) const;

  /**
   * Returns the HPD at the next row/column depending on panel configurartion
   */
  virtual unsigned int HPDForNS() const;

  /**
   * Returns an HPD number that can be used as the second point for the
   * detection plane.
   */
  virtual unsigned int HPDForB() const;

  /**
   * Returns an HPD number that can be used as the third point for the
   * detection plane.
   */
  virtual unsigned int HPDForC() const;

  /**
   * Converts an HPD row and column to a number corresponding
   * to the position of this physical volume in the physical volumes vector
   */
  virtual unsigned int HPDRowColToNum(const unsigned int HPDRow,
                                      const unsigned int HPDCol ) const;

  /**
   * Finds the HPD row and column that corresponds to the x,y coordinates
   * of a point in the panel. The row and column are retuned in the smartID.
   *
   * @returns Status
   * @retval true   HPD is found
   * @retval false  The point is outside the coverage of the HPDs.
   */
  virtual bool findHPDRowCol(const HepPoint3D& inPanel, RichSmartID& id) const;

private:

  /// The horizontal Edge of the HPD grid coverage
  double m_panelHorizEdge;
  /// Absolute value of the horizontal Edge of the HPD grid coverage
  double m_fabs_panelHorizEdge;

  /// The vertical edge of the HPD grid for even columns
  double m_panelVerticalEdgeEven;
  /// The vertical edge of the HPD grid for odd columns
  double m_panelVerticalEdgeOdd;
  /// The smaller (absolute value) of even and odd columns, to ensure coverage
  double m_panelVerticalEdge;

};

#endif    // RICHDET_DERICH1CDFHPDPANEL_H
