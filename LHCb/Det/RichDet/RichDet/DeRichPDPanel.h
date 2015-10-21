
//=============================================================================
/** @file DeRichPDPanel.h
 *
 *  Header file for detector description class : DeRichPDPanel
 *
 *  @author Chris Jones   christopher.rob.jones@cern.ch
 *  @date   2011-09-23
 */
//=============================================================================

#ifndef RICHDET_DERICHPDPANEL_H
#define RICHDET_DERICHPDPANEL_H 1

// Gaudi
#include "GaudiKernel/Transform3DTypes.h"
#include "GaudiKernel/Plane3DTypes.h"

// RichDet
#include "RichDet/DeRichBase.h"
#include "RichDet/DeRichPD.h"

// LHCbKernel
#include "Kernel/RichSmartID.h"
#include "Kernel/RichTraceMode.h"

//=============================================================================
/** @class DeRichPDPanel DeRichPDPanel.h RichDet/DeRichPDPanel.h
 *
 *  Base class for all photon detector panels (HPDs, MaPMTs).
 *  Used by Brunel, Boole etc., so they are neutral to the PD technology.
 *
 *  @author Chris Jones  christopher.rob.jones@cern.ch
 *  @date   2011-09-23
 */
//=============================================================================
class DeRichPDPanel : public DeRichBase
{

public:

  /// Standard constructor
  DeRichPDPanel( const std::string & name = "" );

  /// Destructor
  virtual ~DeRichPDPanel( );

public:

  /// Access the RICH detector type
  inline Rich::DetectorType rich() const { return m_rich; }

  /// Access the PD panel side
  inline Rich::Side side() const { return m_side; }

  /// Access the PD type
  inline LHCb::RichSmartID::IDType pdType() const { return m_pdType; }

  /// The number of PD columns in this panel
  inline unsigned int nPDColumns() const { return m_PDColumns; }
  
  /// The number of PDs in each columns
  inline unsigned int nPDsPerCol() const { return m_PDNumInCol; }
  
  /// The total number of PDs in this panel
  inline unsigned int nPDs() const { return m_PDMax; }

  /** @brief Retrieves the detection plane of the PD panel.
   *
   *  The plane is defined at the top of the PDs
   *
   *  @return The detection plane
   */
  inline const Gaudi::Plane3D& detectionPlane() const
  {
    return m_detectionPlane;
  }

  /**
   * Returns the Z coordinate of the detection plane in the local coordinate system.
   *
   * @return The detection plane Z coordinate (local system).
   */
  inline double detectPlaneZcoord() const { return m_detPlaneZ; }

  /**
   * Returns the offset (y in Rich1, x in Rich2) so that the two panels of
   * each detector appear side-by-side using the globalToPanel method.
   *
   * @return The offset for the globalToPanel method
   */
  inline double localOffset() const { return m_localOffset; }

  /** @brief Converts a RichSmartID to a point in global coordinates.
   *
   *  The point is given on the inside of the PD window, on the photocathode.
   *
   *  @param[in]  smartID     The HPD channel ID
   *  @param[out] detectPoint The detection point
   *  @param[in]  photoCathodeSide  If false use the outside of the PD window and correct
   *                                for refraction. If true use the photocathode side
   *
   *  @return StatusCode indicating if the conversion was successful or not
   *  @retval StatusCode::SUCCESS Conversion to photocathode was OK
   *  @retval StatusCode::FAILURE Impossible conversion to photocathode
   */
  StatusCode detectionPoint( const LHCb::RichSmartID smartID,
                             Gaudi::XYZPoint& detectPoint,
                             bool photoCathodeSide = false ) const
  {
    return this->dePD(smartID)->detectionPoint(smartID,detectPoint,photoCathodeSide);
  }

  /** Access the DeRichPD object for a given PD RichSmartID
   *  @param[in] pdID The PD RichSmartID identifier
   *  @return Pointer to the associated DeRichPD object
   */
  const DeRichPD * dePD( const LHCb::RichSmartID pdID ) const
  {
    return this->dePD(this->pdNumber(pdID));
  }

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
  
public: // virtual methods. Derived classes must implement these

  /// Returns the detector element for the given PD number
  virtual const DeRichPD* dePD( const unsigned int PDNumber ) const = 0;

  /** @brief Returns the intersection point with an HPD window given a vector
   *  and a point.
   *
   *  With the "circle" option a quick check is performed
   *  to test if there would be an intersection with a flat circle instead
   *  of the PD window.
   *
   *  @param[in]  vGlobal           The intersection direction
   *  @param[in]  pGlobal           The intersection start point
   *  @param[out] windowPointGlobal The return point on the PD window
   *  @param[out] smartID           The returned smartID with hit PD info
   *  @param[in]  mode              The ray-tracing configuration mode
   *
   *  @return Status of intersection
   */
  virtual LHCb::RichTraceMode::RayTraceResult
  PDWindowPoint( const Gaudi::XYZVector& vGlobal,
                 const Gaudi::XYZPoint& pGlobal,
                 Gaudi::XYZPoint& windowPointGlobal,
                 LHCb::RichSmartID& smartID,
                 const LHCb::RichTraceMode mode ) const = 0;

  /** @brief Returns the intersection point with the detector plane given a vector
   *         and a point.
   *
   * If mode is tight, returns true only if point is within
   * the detector coverage.
   *
   * @param[in]  vGlobal     The intersection direction
   * @param[in]  pGlobal     The intersection start point
   * @param[out] hitPosition The intersection point on the PD plane
   * @param[in]  mode        The ray-tracing configuration mode
   *
   * @return Intersection status
   */
  virtual LHCb::RichTraceMode::RayTraceResult
  detPlanePoint( const Gaudi::XYZPoint& pGlobal,
                 const Gaudi::XYZVector& vGlobal,
                 Gaudi::XYZPoint& hitPosition,
                 LHCb::RichSmartID& smartID,
                 const LHCb::RichTraceMode mode ) const = 0;

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
  virtual StatusCode smartID( const Gaudi::XYZPoint& globalPoint,
                              LHCb::RichSmartID& id ) const = 0;

  /// Returns the PD number for the given RichSmartID
  virtual unsigned int pdNumber( const LHCb::RichSmartID& smartID ) const = 0;
  ///Returns true when the given smartID corresponds to a GrandPMT.  
  // For hpds and for standard pmts returns false.

  virtual bool pdGrandSize( const LHCb::RichSmartID& smartID ) const = 0;

  /**
   * Adds to the given vector all the available readout channels in this PD panel
   *
   * @param readoutChannels Vector of valid pixel IDs
   *
   * @return Status code
   * @retval StatusCode::SUCCESS Readout channel list was successfully determined
   * @retval StatusCode::FAILURE An error occured whilst determining the readout channels
   */
  virtual StatusCode
  readoutChannelList( LHCb::RichSmartID::Vector& readoutChannels ) const = 0;

protected: // Parameters that must be properly configured in the derived classes

  unsigned int m_PDColumns;    ///< Number of PD columns in the panel
  unsigned int m_PDNumInCol;   ///< Number of PDs in each column
  unsigned int m_PDMax;        ///< Total number of PDs in this panel

  Rich::DetectorType m_rich;   ///< The RICH detector type
  Rich::Side m_side;           ///< The RICH PD panel (up, down, left or right)
  LHCb::RichSmartID::IDType m_pdType; ///< The PD technology type (HPD or MaPMT)

  double m_localOffset; ///< offset applied in the global to panel coordinate transformation

  double m_detPlaneZ;   ///< The z position of the detection plane in an PD panel

  Gaudi::Plane3D m_detectionPlane;     ///< detection plane in global coordinates

  Gaudi::Transform3D m_globalToPDPanelTransform;  ///< global to PD plane (local) transform
  Gaudi::Transform3D m_PDPanelToGlobalTransform;  ///< local (PD plane) to global transform

};

#endif // RICHDET_DERICHPDPANEL_H
