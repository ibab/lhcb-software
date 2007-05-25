
//----------------------------------------------------------------------------
/** @file DeRichHPDPanel.h
 *
 *  Header file for detector description class : DeRichHPDPanel
 *
 *  $Id: DeRichHPDPanel.h,v 1.46 2007-05-25 14:04:31 cattanem Exp $
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */
//----------------------------------------------------------------------------

#ifndef RICHDET_DERICHHPDPANEL_H
#define RICHDET_DERICHHPDPANEL_H 1

// STL
#include <sstream>

// Gaudi
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Plane3DTypes.h"
#include "GaudiKernel/GaudiException.h"

// LHCbKernel
#include "Kernel/RichSmartID.h"
#include "Kernel/RichTraceMode.h"

// DetDesc
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/DetectorElement.h"
#include "DetDesc/ISolid.h"

// RichDet
#include "RichDet/DeRich.h"
#include "RichDet/DeRichHPD.h"
#include "RichDet/DeRichLocations.h"

class DeRichSystem;


/** @class DeRichHPDPanel DeRichHPDPanel.h
 *
 * DeRichHPDPanel provides geometry information for the HPD panels, converts
 * RichSmartIDs to space points and finds intersections with the HPDs
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
  inline const double localOffset() const
  {
    return m_localOffset;
  }

  /** @brief Converts a Gaudi::XYZPoint in global coordinates to a RichSmartID.
   *
   *  The point is assumed to be on the actual detection volume (silicon pixel sensor).
   *
   *  @param[in]  globalPoint The point in global coordinates
   *  @param[out] id          The RichSmartID for the given point
   *
   *  @return Status of conversion
   *  @retval StatusCode::FAILURE Point outside silicon pixel sensor
   */
  StatusCode smartID( const Gaudi::XYZPoint& globalPoint,
                      LHCb::RichSmartID& id ) const;

  /** @brief Converts a RichSmartID to a point in global coordinates. 
   *
   *  The point is given on the inside of the HPD window, on the photocathode.
   *
   *  @param[in]  smartID     The HPD channel ID
   *  @param[out] detectPoint The detection point
   *
   *  @return Status of connersion
   *  @retval StatusCode::SUCCESS Conversion to photocathode was OK
   *  @retval StatusCode::FAILURE Impossible conversion to photocathode
   */
  StatusCode detectionPoint( const LHCb::RichSmartID smartID,
                             Gaudi::XYZPoint& detectPoint ) const;

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
   *  @retval StatusCode::SUCCESS Intersection was successfully calculated
   *  @retval StatusCode::FAILURE Intersection was not found
   */
  StatusCode PDWindowPoint( const Gaudi::XYZVector& vGlobal,
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
   * @retval true  Intersection was inside the acceptance as defined by mode
   * @retval false Intersection fell outside acceptance as defined by mode
   */
  bool detPlanePoint( const Gaudi::XYZPoint& pGlobal,
                      const Gaudi::XYZVector& vGlobal,
                      Gaudi::XYZPoint& hitPosition,
                      const LHCb::RichTraceMode mode ) const;

  /** @brief Converts a global position to the coordinate system of the
   *  photodetector panel. 
   *
   *  The local coordinate system is shifted to allow placing panels side by side
   *
   *  @param[in] globalPoint The point in global coordinates
   *
   *  @return Local (panel) point
   */
  Gaudi::XYZPoint globalToPDPanel( const Gaudi::XYZPoint& globalPoint ) const;


  /**
   * Returns the global position given a local position and panel number.
   * Assumes a shifted panel as previous method
   *
   * @param localPoint The point in local coordinates
   * @param side The detector side
   *
   * @return Global point.
   */
  Gaudi::XYZPoint globalPosition( const Gaudi::XYZPoint& localPoint,
                                  const Rich::Side side ) const;

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
  virtual const int sensitiveVolumeID(const Gaudi::XYZPoint& globalPoint) const;

private:

  /**
   * Finds the HPD row and column that corresponds to the x,y coordinates
   * of a point in the panel. The row and column are retuned in the smartID.
   *
   * @return Status of request
   * @retval true   HPD is found
   * @retval false  The point is outside the coverage of the HPDs.
   */
  bool findHPDColAndPos(const Gaudi::XYZPoint& inPanel, LHCb::RichSmartID& id) const;

  /** Returns the name of this particular HPD panel
   *  @return HPD panel name
   */
  inline const std::string & myName() const { return m_name; }

  /** Convert the HPD number (number in the panel) to a copy number
   *  (which is unique in the Rich system).
   *  @return The Copy Number
   */
  // CRJ : Is this method still needed ? 
  //     : Would the one from DeRichSystem be better ?
  int copyNumber( unsigned int HPDNumber ) const;

  /// Returns the HPD number for the given RichSmartID
  inline unsigned int hpdNumber( const LHCb::RichSmartID smartID ) const
  {
    return smartID.hpdCol() * m_HPDNumInCol + smartID.hpdNumInCol();
  }

  /// Returns the IPVolume master volume for the given HPD number
  inline const IPVolume* HPDMaster( const unsigned int HPDNumber ) const
  {
    const IPVolume* pvHPDMaster = m_pvHPDMaster[HPDNumber];
    if ( HPDNumber>m_HPDMax || !pvHPDMaster )
    {
      std::ostringstream mess;
      mess << "HPDMaster:: Inappropriate HPDNumber : " << HPDNumber;
      throw GaudiException( mess.str(), "*DeRichHPDPanel*", StatusCode::FAILURE );
    }
    return pvHPDMaster;
  }

private: // data

  std::string m_name;           ///< The name of this HPD panel
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

  std::vector<Gaudi::XYZPoint> m_HPDCentres; ///< The centres of all HPDs

  Gaudi::Plane3D m_detectionPlane;     ///< detection plane in global coordinates
  Gaudi::Plane3D m_localPlane;         ///< detection plane in PDPanel coordinates
  Gaudi::XYZVector m_localPlaneNormal; ///< The normal vector of det plane in local coordinates

  /** Plane2 is defined ging through all HPDs at the edge of photocathode coverage on
   *  HPD window. It is used for HPD row/column purposes */
  Gaudi::Plane3D m_localPlane2;
  Gaudi::XYZVector m_localPlaneNormal2;  ///< Normal vector of plane2

  Gaudi::Transform3D m_vectorTransf;  ///< Transform from global to panel coordinates

  double m_panelColumnSideEdge;    ///< Edge of the panel along the columns
  double m_panelStartColPosEven;   ///< Bottom/Start point of the even HPD columns
  double m_panelStartColPosOdd;    ///< Bottom/Start point of the odd HPD columns
  double m_panelStartColPos;       ///< abs max of even and odd start points used as the edge across columns
  double m_localOffset;            ///< offset applied in the global to panel coordinate transformation

  DeRichSystem* m_deRichS;         ///< Pointer to the overall RICH system object

  Rich::DetectorType m_rich;       ///< The RICH detector type
  Rich::Side m_side;               ///< The RICH HPD panel (up, down, left or right)
  LHCb::RichSmartID m_panelRichID; ///< RichSmartID for the panel
  const ISolid* m_kaptonSolid;     ///< Pointer to the kapton solid

  std::vector<const IPVolume*> m_pvHPDMaster;
  std::vector<const IPVolume*> m_pvHPDSMaster;
  std::vector<const IPVolume*> m_pvSilicon;
  std::vector<const IPVolume*> m_pvWindow;
  std::vector<const IPVolume*> m_pvKapton;
  std::vector<Gaudi::Transform3D> m_HPDWindowToGlobal;
  std::vector<Gaudi::Transform3D> m_panelToSilicon;

  std::vector<DeRichHPD*> m_DeHPDs; ///< Container for the HPDs as Det Elements

};

//=========================================================================
//  convert a SmartID to a point on the anode (global coord system)
//=========================================================================
inline Gaudi::XYZPoint DeRichHPDPanel::detPointOnAnode( const LHCb::RichSmartID smartID ) const
{
  return m_DeHPDs[hpdNumber(smartID)]->detPointOnAnode(smartID);
}

//=========================================================================
//  convert a point from the global to the panel coodinate system
//=========================================================================
inline Gaudi::XYZPoint DeRichHPDPanel::globalToPDPanel( const Gaudi::XYZPoint& globalPoint ) const
{
  const Gaudi::XYZPoint localPoint( geometry()->toLocal( globalPoint ) );
  return Gaudi::XYZPoint( localPoint.x(), localPoint.y(), localPoint.z()-m_detPlaneZ );
}

//=========================================================================
//  convert a smartID to a point on the inside of the HPD window
//=========================================================================
inline StatusCode DeRichHPDPanel::detectionPoint ( const LHCb::RichSmartID smartID,
                                                   Gaudi::XYZPoint& detectPoint ) const
{
  return m_DeHPDs[hpdNumber(smartID)]->detectionPoint(smartID,detectPoint);
}

#endif    // RICHDET_DERICHHPDPANEL_H
