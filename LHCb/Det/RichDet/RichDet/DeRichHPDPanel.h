
/** @file DeRichHPDPanel.h
 *
 *  Header file for detector description class : DeRichHPDPanel
 *
 *  CVS Log :-
 *  $Id: DeRichHPDPanel.h,v 1.17 2004-10-18 09:21:49 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.16  2004/07/28 13:41:16  jonrob
 *  Add doxygen file documentation and CVS information
 *
 *  Revision 1.15  2004/07/27 08:55:22  jonrob
 *  Add doxygen file documentation and CVS information
 *
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */

#ifndef RICHDET_DERICHHPDPANEL_H
#define RICHDET_DERICHHPDPANEL_H 1

// Include files
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Geometry/Plane3D.h"

// LHCbKernel
#include "Kernel/CLHEPStreams.h"
#include "Kernel/RichSmartID.h"

// DetDesc
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/DetectorElement.h"
#include "DetDesc/ISolid.h"

// RichKernel
#include "RichKernel/RichTraceMode.h"

// RichDet
#include "RichDet/DeRich1.h"
#include "RichDet/DeRich2.h"

/** @namespace DeRichHPDPanelLocation
 *
 *  Namespace for the location of HPD panels in xml
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */
namespace DeRichHPDPanelLocation {
  /// Location of Rich1 top panel
  static const std::string& Rich1Panel0 = "/dd/Structure/LHCb/Rich1/PDPanel0";
  /// Location of Rich1 bottom panel
  static const std::string& Rich1Panel1 = "/dd/Structure/LHCb/Rich1/PDPanel1";
  /// Location of Rich2 left panel
  static const std::string& Rich2Panel0 = "/dd/Structure/LHCb/Rich2/PDPanel0";
  /// Location of Rich2 right panel
  static const std::string& Rich2Panel1 = "/dd/Structure/LHCb/Rich2/PDPanel1";
}

/** @class DeRichHPDPanel DeRichHPDPanel.h
 *
 * DeRichHPDPanel provides geometry information for the HPD panels, converts RichSmartIDs
 * to space points and finds intersections with the HPDs
 *
 * @author Antonis Papanestis a.papanestis@rl.ac.uk
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
  virtual const HepPlane3D & detectionPlane() const = 0;

  /**
   * Returns the offset (y in Rich1, x in Rich2) so that the two panels of
   * each detector appear side-by-side using the globalToPanel method.
   * @return The offset for the globalToPanel method
   */
  virtual const double localOffset() const = 0;


  /**
   * Converts a HepPoint3D in global coordinates to a RichSmartID.
   * The point is assumed to be on the actual detection volume
   * (silicon pixel sensor).
   *
   * @return Status of conversion
   * @retval StatusCode::FAILURE Point outside silicon pixel sensor
   */
  virtual StatusCode smartID( const HepPoint3D& globalPoint,
                              RichSmartID& id );

  /**
   * Converts a RichSmartID to a point in global coordinates. The point is
   * given on the inside of the HPD window, on the photocathode.
   *
   * @return Status of conversion
   */
  virtual StatusCode detectionPoint( const RichSmartID& smartID,
                                     HepPoint3D& windowHitGlobal );
  /**
   * Returns the intersection point with an HPD window given a vector
   * and a point. With the "circle" option a quick check is performed
   * to test if there would be an intersection with a flat circle instead
   * of the HPD window.
   *
   * @param vGlobal  A vector in global coordinates used for direction
   * @param pGlobal  A point in global coordinates
   * @param windowPointGlobal The returned intersection point
   * @param smartID  The returned smartID with hit HPD info
   * @param mode     The ray tracing mode configuration
   *
   * @return Status of intersection
   * @retval StatusCode::FAILURE Intersection failed, as defined by mode
   */

  virtual StatusCode PDWindowPoint( const HepVector3D& vGlobal,
                                    const HepPoint3D& pGlobal,
                                    HepPoint3D& windowPointGlobal, // return
                                    RichSmartID& smartID,          // return
                                    const RichTraceMode mode );

  /**
   * Returns the intersection point with the detector plane given a vector
   * and a point. If mode is tight, returns true only if point is within
   * the detector coverage.
   *
   * @return Intersection status
   * @retval false Intersection fell outside acceptance, as defined by mode
   */
  virtual bool detPlanePoint( const HepPoint3D& pGlobal,
                              const HepVector3D& vGlobal,
                              HepPoint3D& hitPosition,
                              const RichTraceMode mode );

  /**
   * Converts a global position to the coordinate system of the
   * photodetector panel. The local coordinate system is shifted to allow
   * placing panels side by side
   *
   * @return Local (panel) point
   */
  virtual HepPoint3D globalToPDPanel( const HepPoint3D& globalPoint );


  /**
   * Returns the global position given a local position and panel number.
   * Assumes a shifted panel as previous method
   *
   * @return Global point.
   */
  virtual HepPoint3D globalPosition( const HepPoint3D& localPoint,
                                     const Rich::Side side) = 0;


  /**
   * Returns a list with all the available readout channels, in form of
   * RichSmartIDs.
   *
   * @return Status
   */
  virtual StatusCode readoutChannelList(std::vector<RichSmartID>&
                                        readoutChannels);


protected:

  /**
   * Returns the number of HPDs in the panel
   */
  virtual unsigned int PDMax() const = 0;

  /**
   * Returns the HPD row in the panel, given the HPD number
   */
  virtual unsigned int PDRow(const unsigned int PD) const = 0;

  /**
   * Returns the HPD column in the panel, given the HPD number
   */
  virtual unsigned int PDCol(const unsigned int PD) const = 0;

  /**
   * Returns the HPD at the next row/column depending on panel configurartion
   */
  virtual unsigned int HPDForNS() const = 0;

  /**
   * Returns an HPD number that can be used as the second point for the
   * detection plane.
   */
  virtual unsigned int HPDForB() const = 0;

  /**
   * Returns an HPD number that can be used as the third point for the
   * detection plane.
   */
  virtual unsigned int HPDForC() const = 0;

  /**
   * Converts an HPD row and column to a number corresponding
   * to the position of this physical volume in the physical volumes vector
   */
  virtual unsigned int HPDRowColToNum(const unsigned int HPDRow,
                                      const unsigned int HPDCol ) const = 0;

  /**
   * Finds the HPD row and column that corresponds to the x,y coordinates
   * of a point in the panel. The row and column are retuned in the smartID.
   *
   * @returns Status
   * @retval true   HPD is found
   * @retval false  The point is outside the coverage of the HPDs.
   */
  virtual bool findHPDRowCol(const HepPoint3D& inPanel, RichSmartID& id) = 0;

  // data

  std::string m_name;              ///< The name of this HPD panel
  unsigned int m_HPDRows;     ///< Number of HPD rows in the panel
  unsigned int m_HPDColumns;  ///< Number of HPD columns in the panel

  /// for cdf type panels: number of HPDs in a big column
  unsigned int m_HPDsInBigCol;
  /// for cdf type panels: number of HPDs in two adjacent columns
  unsigned int m_HPDsIn2Cols;

  unsigned int m_pixelRows;        /// Number of pixel rows
  unsigned int m_pixelColumns;     /// Number of pixel columns

  /// The active HPD window radius (photocathode coverage)
  double m_activeRadius;
  /// The active HPD window radius Squared
  double m_activeRadiusSq;

  /// The radius of the sphere of the HPD window
  double m_winR;
  /// Radius squared
  double m_winRsq;

  /// The z position of the detection plane in an HPD panel
  double m_detPlaneZ;
  /// the z difference between the planes defined at the top and bottom of HPD window
  double m_detPlaneZdiff;

  double m_pixelSize;      ///< The pixel size on the silicon sensor
  double m_subPixelSize;   ///< The size of the subpixel (Alice mode)
  double m_siliconHalfLengthX;     ///< Half size (x) of silicon sensor
  double m_siliconHalfLengthY;     ///< Half size (y) of silicon sensor

  double m_rowPitch;               ///< distance between HPD rows
  double m_columnPitch;            ///< distance between HPD columns

  /// The demagnification factor of the HPD.  Element [0] is the linear
  /// term, and element[1] the non-linear term for small corrections.
  double m_deMagFactor[2];

  /// The following points are used to get the row and column pitch
  /// Centre of HPD 0
  HepPoint3D m_HPD0Centre;
  HepPoint3D m_HPD1Centre;   ///< Centre of HPD 1
  HepPoint3D m_HPDNSCentre;  ///< Centre of HPD next set: either next row or next column

  /// the top of the HPD window in silicon coordinates
  HepPoint3D m_HPDTop;

  const ISolid* m_HPDPanelSolid;   ///< The solid (box) that contains the HPDs

  std::vector<HepPoint3D> m_HPDCentres; ///< The centres of all HPDs

  HepPlane3D m_detectionPlane;    ///< detection plane in global coordinates
  HepPlane3D m_localPlane;        ///< detection plane in PDPanel coordinates
  HepVector3D m_localPlaneNormal; ///< The normal vector of det plane in local coordinates

  /// Plane2 is defined ging through all HPDs at the edge of photocathode coverage on
  /// HPD window. It is used for HPD row/column purposes
  HepPlane3D m_localPlane2;
  HepVector3D m_localPlaneNormal2;  ///< Normal vector of plane2

  double m_detPlaneHorizEdge;     ///< Horizontal (x) edge of HPD coverage in the panel
  double m_detPlaneVertEdge;      ///< Vertical (y) edge of HPD coverage in the panel

  HepTransform3D m_vectorTransf;  ///< Transform from global to panel coordinates

};

#endif    // RICHDET_DERICHHPDPANEL_H
