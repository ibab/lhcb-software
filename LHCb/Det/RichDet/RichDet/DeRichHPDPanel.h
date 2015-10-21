//----------------------------------------------------------------------------
/** @file DeRichHPDPanel.h
 *
 *  Header file for detector description class : DeRichHPDPanel
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

// DetDesc
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/ISolid.h"

// RichDet
#include "RichDet/DeRichPDPanel.h"
#include "RichDet/DeRich.h"
#include "RichDet/DeRichHPD.h"
#include "RichDet/DeRichLocations.h"

//----------------------------------------------------------------------------
/** @class DeRichHPDPanel RichDet/DeRichHPDPanel.h
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
//----------------------------------------------------------------------------
class DeRichHPDPanel: public DeRichPDPanel
{

public:

  /**
   * Constructor for this class
   */
  DeRichHPDPanel( const std::string & name = "" );

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

public:

  // Converts a Gaudi::XYZPoint in global coordinates to a RichSmartID.
  virtual StatusCode smartID( const Gaudi::XYZPoint& globalPoint,
                              LHCb::RichSmartID& id ) const;

  /** Converts a RichSmartID to a point on the anode in global coordinates.
   *  @param[in] smartID      The HPD channel ID
   *  @return The detection anode point in global coordinates
   */
  Gaudi::XYZPoint detPointOnAnode( const LHCb::RichSmartID& smartID ) const;

  // Returns the intersection point with an HPD window given a vector and a point.
  virtual LHCb::RichTraceMode::RayTraceResult
  PDWindowPoint( const Gaudi::XYZVector& vGlobal,
                 const Gaudi::XYZPoint& pGlobal,
                 Gaudi::XYZPoint& windowPointGlobal,
                 LHCb::RichSmartID& smartID,
                 const LHCb::RichTraceMode mode ) const;

  // Returns the intersection point with the detector plane given a vector and a point.
  virtual LHCb::RichTraceMode::RayTraceResult
  detPlanePoint( const Gaudi::XYZPoint& pGlobal,
                 const Gaudi::XYZVector& vGlobal,
                 Gaudi::XYZPoint& hitPosition,
                 LHCb::RichSmartID& smartID,
                 const LHCb::RichTraceMode mode ) const;

  /// Returns the detector element for the given PD number
  virtual const DeRichPD* dePD( const unsigned int PDNumber ) const;

  /// Returns the detector element for the given PD number
  inline const DeRichHPD* deHPD( const unsigned int HPDNumber ) const
  {
    // CRJ : should this just be < ??
    const DeRichHPD * deHPD = ( HPDNumber <= nPDs() ? m_DeHPDs[HPDNumber] : NULL );
#ifndef NDEBUG
    if ( !deHPD )
    {
      std::ostringstream mess;
      mess << "Inappropriate HPDNumber : " << HPDNumber;
      throw GaudiException( mess.str(), "*DeRichHPDPanel*", StatusCode::FAILURE );
    }
#endif
    return deHPD;
  }

  /// Adds to the given vector all the available readout channels in this HPD panel
  virtual StatusCode readoutChannelList( LHCb::RichSmartID::Vector& readoutChannels ) const;

  /// sensitive volume identifier
  virtual int sensitiveVolumeID(const Gaudi::XYZPoint& globalPoint) const;

private: // methods

  /** Finds the HPD row and column that corresponds to the x,y coordinates
   *  of a point in the panel. The row and column are retuned in the smartID.
   *
   * @return Status of request
   * @retval true   HPD is found
   * @retval false  The point is outside the coverage of the HPDs.
   */
  bool findHPDColAndPos(const Gaudi::XYZPoint& inPanel, LHCb::RichSmartID& id) const;

  /// Returns the PD number for the given RichSmartID
  unsigned int pdNumber( const LHCb::RichSmartID& smartID ) const;

  /// Need to ask Sajan about this
  inline bool pdGrandSize( const LHCb::RichSmartID& /** smartID **/ ) const { return false; }
  
  /// Check HPD panel acceptance
  LHCb::RichTraceMode::RayTraceResult checkPanelAcc( const Gaudi::XYZPoint & point ) const;

  /// Update cached information on geometry changes
  StatusCode geometryUpdate();

private: // data

  double m_HPDPitch;               ///< distance between HPDs
  double m_HPDColPitch;            ///< distance between HPD columns
  double m_OneOverHPDPitch;        ///< 1 / distance between HPDs (cached for speed)
  double m_OneOverHPDColPitch;     ///< 1 / distance between HPD columns (cached for speed)

  unsigned int m_pixelRows;        ///< Number of pixel rows
  unsigned int m_pixelColumns;     ///< Number of pixel columns

  /// The active HPD window radius (photocathode coverage) Squared
  double m_activeRadiusSq;

  double m_pixelSize;              ///< The pixel size on the silicon sensor
  double m_subPixelSize;           ///< The size of the subpixel (Alice mode)
  double m_siliconHalfLengthX;     ///< Half size (x) of silicon sensor
  double m_siliconHalfLengthY;     ///< Half size (y) of silicon sensor

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

  std::vector<IDetectorElement*> m_DeSiSensors; ///< Container for the Si sensors as Det Elements

  std::vector<DeRichHPD*> m_DeHPDs; ///< Container for the HPDs as Det Elements

};

//=========================================================================
//  convert a SmartID to a point on the anode (global coord system)
//=========================================================================
inline Gaudi::XYZPoint DeRichHPDPanel::detPointOnAnode( const LHCb::RichSmartID& smartID ) const
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
// Check HPD panel acceptance
//=========================================================================
inline LHCb::RichTraceMode::RayTraceResult
DeRichHPDPanel::checkPanelAcc( const Gaudi::XYZPoint & point ) const
{
  const auto uv = ( rich() == Rich::Rich1 ? 
                    std::make_pair(point.y(),point.x()) :
                    std::make_pair(point.x(),point.y()) );
  return ( ( fabs(uv.first)  >= fabs(m_panelColumnSideEdge) ||
             fabs(uv.second) >= m_panelStartColPos ) ?
           LHCb::RichTraceMode::OutsideHPDPanel : LHCb::RichTraceMode::InHPDPanel );
}

#endif    // RICHDET_DERICHHPDPANEL_H
