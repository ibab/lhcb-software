
//----------------------------------------------------------------------------
/** @file DeRich2HPDPanel.h
 *
 *  Header file for detector description class : DeRich2HPDPanel
 *
 *  CVS Log :-
 *  $Id: DeRich2HPDPanel.h,v 1.16 2005-02-22 13:53:50 jonrob Exp $
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */
//----------------------------------------------------------------------------

#ifndef RICHDET_RICHDET_DERICH2HPDPANEL_H
#define RICHDET_RICHDET_DERICH2HPDPANEL_H 1

// Include files
#include "RichDet/DeRichHPDPanel.h"

// External declarations
extern const CLID& CLID_DeRich2HPDPanel;

/** @class DeRich2HPDPanel DeRich2HPDPanel.h
 *
 * This is the definition of the Rich2 HPDPanel detector class.
 * This HPD panel grows in columns, and each column has the same number
 * of HPDs.  So, the numbering is a little like this:
 *
 *   5   11
 * 2   8
 *   4   10
 * 1   7     and so on...
 *   3   9
 * 0   6              numbers 0, 3, 6 and 9 belong to the same row.
 * (please look at the actual .h file to see the correct layout of HPDs)
 *
 * @author Antonis Papanestis a.papanestis@rl.ac.uk
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
   * Returns the offset (x in Rich2) so that the two panels of
   * each detector appear side-by-side using the globalToPanel method.
   * @return The offset for the globalToPanel method
   */
  virtual const double localOffset() const;

  /**
   * Returns the global position given a local position and panel number.
   * Assumes a shifted panel.
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
  virtual unsigned int HPDRowColToNum( const unsigned int HPDRow,
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

  /// The vertical Edge of the HPD grid for even columns
  double m_panelVerticalEdgeEven;

  /// The vertical Edge of the HPD grid for odd columns
  double m_panelVerticalEdgeOdd;

  /// The smaller (absolute value) of even and odd columns, to ensure coverage
  double m_panelVerticalEdge;

};

#endif    // RICHDET_DERICH2HPDPANEL_H
