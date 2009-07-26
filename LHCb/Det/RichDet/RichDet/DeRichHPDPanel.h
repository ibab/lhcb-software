
//----------------------------------------------------------------------------
/** @file DeRichHPDPanel.h
 *
 *  Header file for detector description class : DeRichHPDPanel
 *
 *  $Id: DeRichHPDPanel.h,v 1.56 2009-07-26 18:13:17 jonrob Exp $
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */
//----------------------------------------------------------------------------

#ifndef RICHDET_DERICHHPDPANEL_H
#define RICHDET_DERICHHPDPANEL_H 1

// Gaudi
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Plane3DTypes.h"

// LHCbKernel
#include "Kernel/RichSmartID.h"
#include "Kernel/RichTraceMode.h"

// DetDesc
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/ISolid.h"

// RichDet
#include "RichDet/DeRichBase.h"
#include "RichDet/DeRich.h"
#include "RichDet/DeRichHPD.h"
#include "RichDet/DeRichLocations.h"

// Boost
#include "boost/array.hpp"

class DeRichSystem;

/** @class DeRichHPDPanel DeRichHPDPanel.h
 *
 * DeRichHPDPanel provides geometry information for the HPD panels, converts
 * RichSmartIDs to space points and finds intersections with the HPDs
 *
 * @author Antonis Papanestis a.papanestis@rl.ac.uk
 *
 * @todo See if the HPD planes in DeRichHPDPanel::detPlanePoint and
 *       DeRichHPDPanel::PDWindowPoint could be made the same. At the moment the
 *       hits obtained from these methods are on slightly different planes.
 */
class DeRichHPDPanel: public DeRichBase
{

public:

  /**
   * Constructor for this class
   */
  DeRichHPDPanel(const std::string & name = "");

  /**
   * Default destructor
   */
  virtual ~DeRichHPDPanel();

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
   *
   * @return Status of initialisation
   * @retval StatusCode::FAILURE Initialisation failed, program should
   * terminate
   */
  virtual StatusCode initialize();

  /** @brief Retrieves the detection plane of the HPD panel.
   *
   *  The plane is defined at the top of the HPDs (a plane resting on
   *  the HPDs "touching" the INSIDE surface of the window).
   *
   *  @return The detection plane
   */
  inline const Gaudi::Plane3D& detectionPlane() const
  {
    return m_detectionPlane;
  }

  /**
   * Returns the offset (y in Rich1, x in Rich2) so that the two panels of
   * each detector appear side-by-side using the globalToPanel method.
   *
   * @return The offset for the globalToPanel method
   */
  inline double localOffset() const { return m_localOffset; }

  /**
   * Returns the Z coordinate of the detection plane in the local coordinate system.
   *
   * @return The detection plane Z coordinate (local system).
   */
  inline double detectPlaneZcoord() const { return m_detPlaneZ; }

  /** @brief Converts a Gaudi::XYZPoint in global coordinates to a RichSmartID.
   *
   *  The point is assumed to be on the actual detection volume (silicon pixel sensor).
   *
   *  @param[in]  globalPoint The point in global coordinates
   *  @param[out] id          The RichSmartID for the given point
   *
   *  @return Status of conversion
   *  @retval StatusCode::FAILURE Point outside silicon pixel sensor or in a position that
   *          could not have originated from the photocathode.
   */
  StatusCode smartID( const Gaudi::XYZPoint& globalPoint,
                      LHCb::RichSmartID& id ) const;

  /** @brief Converts a RichSmartID to a point in global coordinates.
   *
   *  The point is given on the inside of the HPD window, on the photocathode.
   *
   *  @param[in]  smartID     The HPD channel ID
   *  @param[out] detectPoint The detection point
   *  @param[in]  photoCathodeSide  If false use the outside of the HPD window and correct
   *                                for refraction. If true use the photocathode side
   *
   *  @return StatusCode indicating if the conversion was successful or not
   *  @retval StatusCode::SUCCESS Conversion to photocathode was OK
   *  @retval StatusCode::FAILURE Impossible conversion to photocathode
   */
  StatusCode detectionPoint( const LHCb::RichSmartID smartID,
                             Gaudi::XYZPoint& detectPoint,
                             bool photoCathodeSide = false ) const;

  /** Converts a RichSmartID to a point on the anode in global coordinates.
   *  @param[in] smartID      The HPD channel ID
   *  @return The detection anode point in global coordinates
   */
  Gaudi::XYZPoint detPointOnAnode( const LHCb::RichSmartID smartID ) const;

  /** @brief Returns the intersection point with an HPD window given a vector
   *  and a point.
   *
   *  With the "circle" option a quick check is performed
   *  to test if there would be an intersection with a flat circle instead
   *  of the HPD window.
   *
   *  @param[in]  vGlobal           The intersection direction
   *  @param[in]  pGlobal           The intersection start point
   *  @param[out] windowPointGlobal The return point on the HPD window
   *  @param[out] smartID           The returned smartID with hit HPD info
   *  @param[in]  mode              The ray-tracing configuration mode
   *
   *  @return Status of intersection
   */
  LHCb::RichTraceMode::RayTraceResult
  PDWindowPoint( const Gaudi::XYZVector& vGlobal,
                 const Gaudi::XYZPoint& pGlobal,
                 Gaudi::XYZPoint& windowPointGlobal,
                 LHCb::RichSmartID& smartID,
                 const LHCb::RichTraceMode mode ) const;

  /** @brief Returns the intersection point with the detector plane given a vector
   * and a point.
   *
   * If mode is tight, returns true only if point is within
   * the detector coverage.
   *
   * @param[in]  vGlobal     The intersection direction
   * @param[in]  pGlobal     The intersection start point
   * @param[out] hitPosition The intersection point on the HPD plane
   * @param[in]  mode        The ray-tracing configuration mode
   *
   * @return Intersection status
   */
  LHCb::RichTraceMode::RayTraceResult
  detPlanePoint( const Gaudi::XYZPoint& pGlobal,
                 const Gaudi::XYZVector& vGlobal,
                 Gaudi::XYZPoint& hitPosition,
                 LHCb::RichSmartID& smartID,
                 const LHCb::RichTraceMode mode ) const;

  /**
   * Adds to the given vector all the available readout channels in this HPD panel
   *
   * @param readoutChannels Vector of valid pixel IDs
   *
   * @return Status code
   * @retval StatusCode::SUCCESS Readout channel list was successfully determined
   * @retval StatusCode::FAILURE An error occured whilst determining the readout channels
   */
  StatusCode readoutChannelList( LHCb::RichSmartID::Vector& readoutChannels ) const;

  /// Access the RICH detector type
  inline Rich::DetectorType rich() const { return m_rich; }

  /// Access the HPD panel side
  inline Rich::Side side() const { return m_side; }

  /// sensitive volume identifier
  virtual int sensitiveVolumeID(const Gaudi::XYZPoint& globalPoint) const;

  /// The number of HPD columns in this panel
  inline unsigned int nHPDColumns() const { return m_HPDColumns; }

  /// The number of HPDs in each columns
  inline unsigned int nHPDsPerCol() const { return m_HPDNumInCol; }

  /// The total number of HPDs in this panel
  inline unsigned int nHPDs() const { return m_HPDMax; }

  /** Access the DeRichHPD object for a given HPD RichSmartID
   *  @param[in] hpdID The HPD RichSmartID identifier
   *  @return Pointer to the associated DeRichHPD object
   */
  const DeRichHPD * deHPD( const LHCb::RichSmartID hpdID ) const;

  /** Get the global to local (PD panel) transform. The local frame is defined with an offset
   *  to accommodate both detector panels in one histogram and the x=0 in Rich1 and y=0 in
   *  Rich2 are aligned with the global x=0 and y=0.
   */
  inline const Gaudi::Transform3D& globalToPDPanelMatrix() const
  {
    return m_globalToPDPanelTransform;
  }

  /// Get the transform from the local to global frame. See comments at the globalToPDPanelMatrix method
  inline const Gaudi::Transform3D& PDPanelToGlobalMatrix() const
  {
    return m_PDPanelToGlobalTransform;
  }

private: // methods

  /** Finds the HPD row and column that corresponds to the x,y coordinates
   *  of a point in the panel. The row and column are retuned in the smartID.
   *
   * @return Status of request
   * @retval true   HPD is found
   * @retval false  The point is outside the coverage of the HPDs.
   */
  bool findHPDColAndPos(const Gaudi::XYZPoint& inPanel, LHCb::RichSmartID& id) const;

  /// Returns the HPD number for the given RichSmartID
  unsigned int hpdNumber( const LHCb::RichSmartID smartID ) const;

  /// Returns the detector element for the given HPD number
  const DeRichHPD* DeHPD( const unsigned int HPDNumber ) const;

  /// Check HPD panel acceptance
  LHCb::RichTraceMode::RayTraceResult checkPanelAcc( const Gaudi::XYZPoint & point ) const;

  /// generate the transforms from the global frame to the local photodetector frame
  StatusCode generateGlobalToPDPanelTransforms();

private: // data

  unsigned int m_HPDColumns;    ///< Number of HPD columns in the panel
  unsigned int m_HPDNumInCol;   ///< Number of HPDs in each column
  unsigned int m_HPDMax;        ///< Total number of HPDs in this panel

  double m_HPDPitch;               ///< distance between HPDs
  double m_HPDColPitch;            ///< distance between HPD columns

  unsigned int m_pixelRows;        ///< Number of pixel rows
  unsigned int m_pixelColumns;     ///< Number of pixel columns

  /// The active HPD window radius (photocathode coverage) Squared
  double m_activeRadiusSq;

  /// The z position of the detection plane in an HPD panel
  double m_detPlaneZ;

  double m_pixelSize;      ///< The pixel size on the silicon sensor
  double m_subPixelSize;   ///< The size of the subpixel (Alice mode)
  double m_siliconHalfLengthX;     ///< Half size (x) of silicon sensor
  double m_siliconHalfLengthY;     ///< Half size (y) of silicon sensor

  Gaudi::Plane3D m_detectionPlane;     ///< detection plane in global coordinates
  Gaudi::Plane3D m_localPlane;         ///< detection plane in PDPanel coordinates
  Gaudi::XYZVector m_localPlaneNormal; ///< The normal vector of det plane in local coordinates

  /** Plane2 is defined going through all HPDs at the edge of photocathode coverage on
   *  HPD window. It is used for HPD row/column purposes
   *  This plane is parrallel to m_localPlane, thus share the same normal vector
   */
  Gaudi::Plane3D m_localPlane2;
  double m_localPlaneZdiff; ///< Shift in Z between localPlane2 and localPlane

  double m_panelColumnSideEdge;    ///< Edge of the panel along the columns
  double m_panelStartColPosEven;   ///< Bottom/Start point of the even HPD columns
  double m_panelStartColPosOdd;    ///< Bottom/Start point of the odd HPD columns
  double m_panelStartColPos;       ///< abs max of even and odd start points used as the edge across columns
  double m_localOffset;            ///< offset applied in the global to panel coordinate transformation

  Gaudi::Transform3D m_globalToPDPanelTransform;  ///< global to PD plane (local) transform
  Gaudi::Transform3D m_PDPanelToGlobalTransform;  ///< local (PD plane) to global transform

  DeRichSystem* m_deRichS;         ///< Pointer to the overall RICH system object

  Rich::DetectorType m_rich;       ///< The RICH detector type
  Rich::Side m_side;               ///< The RICH HPD panel (up, down, left or right)

  std::vector<DeRichHPD*> m_DeHPDs; ///< Container for the HPDs as Det Elements
  std::vector<IDetectorElement*> m_DeSiSensors; ///< Container for the Si sensors as Det Elements

};

//=========================================================================
// Returns the HPD number for the given RichSmartID
//=========================================================================
inline unsigned int DeRichHPDPanel::hpdNumber( const LHCb::RichSmartID smartID ) const
{
  return ( smartID.rich() == rich() && smartID.panel() == side() ?
           smartID.hpdCol() * m_HPDNumInCol + smartID.hpdNumInCol() :
           m_HPDMax+1 );
}

//=========================================================================
// Access the DeRichHPD object for a given HPD RichSmartID
//=========================================================================
inline const DeRichHPD * DeRichHPDPanel::deHPD( const LHCb::RichSmartID hpdID ) const
{
  return DeHPD(hpdNumber(hpdID));
}

//=========================================================================
//  convert a SmartID to a point on the anode (global coord system)
//=========================================================================
inline Gaudi::XYZPoint DeRichHPDPanel::detPointOnAnode( const LHCb::RichSmartID smartID ) const
{
  return deHPD(smartID)->detPointOnAnode(smartID);
}

//=========================================================================
//  convert a point from the global to the panel coodinate system
//=========================================================================
// inline Gaudi::XYZPoint DeRichHPDPanel::globalToPDPanel( const Gaudi::XYZPoint& globalPoint ) const
// {
//   const Gaudi::XYZPoint localPoint( geometry()->toLocal( globalPoint ) );
//   return Gaudi::XYZPoint( localPoint.x(), localPoint.y(), localPoint.z()-m_detPlaneZ );
// }

//=========================================================================
//  convert a smartID to a point on the inside of the HPD window
//=========================================================================
inline StatusCode DeRichHPDPanel::detectionPoint ( const LHCb::RichSmartID smartID,
                                                   Gaudi::XYZPoint& detectPoint,
                                                   bool photoCathodeSide ) const
{
  return deHPD(smartID)->detectionPoint(smartID,detectPoint,photoCathodeSide);
}

//=========================================================================
// Check HPD panel acceptance
//=========================================================================
inline LHCb::RichTraceMode::RayTraceResult
DeRichHPDPanel::checkPanelAcc( const Gaudi::XYZPoint & point ) const
{
  const double u = ( rich() == Rich::Rich1 ? point.y() : point.x() );
  const double v = ( rich() == Rich::Rich1 ? point.x() : point.y() );
  return ( ( fabs(u) >= fabs(m_panelColumnSideEdge) ||
             fabs(v) >= m_panelStartColPos ) ?
           LHCb::RichTraceMode::OutsideHPDPanel : LHCb::RichTraceMode::InHPDPanel );
}

#endif    // RICHDET_DERICHHPDPANEL_H
