// $Id: $
#ifndef DEFTLAYER_H
#define DEFTLAYER_H 1

// DetDesc
#include "DetDesc/DetectorElement.h"
#include "DetDesc/IGeometryInfo.h"

// Kernel
#include "Kernel/FTChannelID.h"
#include "Kernel/DetectorSegment.h" // Geom. representation of the FT cell/channel

/** @class DeFTLayer DeFTLayer.h "FTDet/DeFTLayer.h"
 *
 *  This is the detector element class of the Fibre Tracker (FT) layer.
 *  As it is the FT sub-structure det. element of highest granularity it provides
 *  the actual methods for making the conversion geometrical point <--> FTChannelID.
 *
 *  Below is graphical representation of a layer with no stereoAngle. It shows
 *  the numbering convention for Quarters and SiPMs. nSiPM (per quarter) is
 *  determined during initialize from the relevant FT DDDB xml parameters.
 *  It is assumed that the stereo Layers (u/v) have their left-right-half
 *  boundary tilted by the same stereo angle.
 *  Unless oterwise stated "Right" and "Left" refer to the directions of
 *  an observer sitting at (0,0,0), i.e. "Left" is the positive x direction.
 *
 *  @verbatim
                                                                     ^ Y
                   Quarter 3                 Quarter 2               |
                                                                     |
SiPM ID: | nSiPM-1  <-----------  0 # 0  ----------->  nSiPM-1 |     |
         |-----------------------------------------------------|     |
         |  |  |  |  |  |  |  |  |  #  |  |  |  |  |  |  |  |  |     |
         |  |  |  |  |  |  |  |  |  #  |  |  |  |  |  |  |  |  |     |
         |  |  |  |  |  |  |  |  |  #  |  |  |  |  |  |  |  |  |     |
         |  |  |  |  |  |  |  |  |  #  |  |  |  |  |  |  |  |  |     |
         |  |  |  |  |  |  |  |  |  #  |  |  |  |  |  |  |  |  |     |
         |  |  |  |  |  |  |  |  |  #  |  |  |  |  |  |  |  |  |     |
         |  |  |  |  |  |  |  |  |__#__|  |  |  |  |  |  |  |  |     |
         |  |  |  |  |  |  |  |  |     |  |  |  |  |  |  |  |  |     |
         |=======================|     |=======================|     |
         |  |  |  |  |  |  |  |  |_____|  |  |  |  |  |  |  |  |     |
         |  |  |  |  |  |  |  |  |  #  |  |  |  |  |  |  |  |  |     |
         |  |  |  |  |  |  |  |  |  #  |  |  |  |  |  |  |  |  |     |
         |  |  |  |  |  |  |  |  |  #  |  |  |  |  |  |  |  |  |     |
         |  |  |  |  |  |  |  |  |  #  |  |  |  |  |  |  |  |  |     |
         |  |  |  |  |  |  |  |  |  #  |  |  |  |  |  |  |  |  |     |
         |  |  |  |  |  |  |  |  |  #  |  |  |  |  |  |  |  |  |     |
         |  |  |  |  |  |  |  |  |  #  |  |  |  |  |  |  |  |  |     |
         |-----------------------------------------------------|     |
SiPM ID: | nSiPM-1  <-----------  0 # 0  ----------->  nSiPM-1 |     |
                                                                     |
                   Quarter 1                 Quarter 0               |
                                                                     |
   <------------------------------------------------------------------
   X

 *  @endverbatim
 *
 *  NB: The term 'cell' refers to a parallelepiped with dimensions determined
 *  by FT xml DDDB parameters (roughly 0.25mm x FTHeight/2 x 1.2mm). The term
 *  'channel' is used for the logical representation of _sensitive_ cells
 *  (the 'SiPM edge' and 'inner SiPM gap' cells are non-sensitive)
 *
 *  SiPM cell numbering:
 *  From 0 to 130, inclusive, always increasing from small to large x,
 *  i.e. from right to left in the figure above. The cell with ID 0(130)
 *  is the right(left) inactive edge of the SiPM. The cell with ID 65 is
 *  the inner inactive gap in the SiPM. Functions are used for converting
 *  between these 'gross' cellIDs and the sensitive ('net') cellIDs.
 * 
 *  @author Plamen Hopchev
 *  @date   2012-04-25
 */

static const CLID CLID_DeFTLayer = 18004;

typedef std::vector< std::pair<LHCb::FTChannelID, double> > FTPair;

class DeFTLayer : public DetectorElement {

public: 

  /// Standard constructor
  DeFTLayer( const std::string& name = "" );
  
  /// Destructor
  virtual ~DeFTLayer( );

  /** Retrieves reference to class identifier
   *  @return The class identifier for this class
   */
  const CLID& clID() const;

  /** Another reference to class identifier
   *  @return The class identifier for this class
   */
  static const CLID& classID() { return CLID_DeFTLayer; }

  /** Initialization method 
   *  @return Status of initialization
   */ 
  virtual StatusCode initialize();

  /** @return layerID */
  unsigned int layerID() const;

  /** Set the ID of the layer
   *  @param layerID The layer ID
   */
  void setLayerID(unsigned int layerID);

  /** @return Stereo angle of the layer (in radians) */
  double angle() const;

  /** @return Tangent of the stereo angle of the layer */
  double tanAngle() const;

  /** Set the stereo angle of the layer
   *  @param angle Angle in radians
   */
  void setAngle(double angle);
  
  /** Get the x-position at the top/bottom of the layer by extrapolating
   *  along the fibres the initial
   *  @param x0 x-position
   *  @param y0 y-position
   *  to the top/bottom of the layer.
   *  @return x-position at the layer top (if y0 > 0) or bottom (if y0 < 0)
   */
  double xAtVerticalBorder(double x0, double y0) const;

  /** Get the x-position at y=0 by extrapolating along the fibres the initial
   *  @param x0 x-position
   *  @param y0 y-position
   *  to y=0.
   *  @return x-position at y=0
   */
  double xAtYEq0(double x0, double y0) const;

  /** Get the list of traveresed SiPM channels. The particle trajectory is a
   *  straight line defined by:
   *  @param globalPointEntry Global entry point
   *  @param globalPointExit Global exit point
   *  Fills a vector of 'FT pairs' (channel ID and fractional position
   *  inside the relevant cell/channel) passed as an argument.
   *  @return Status of the execution
   */
  StatusCode calculateHits(const Gaudi::XYZPoint&  globalPointEntry,
                           const Gaudi::XYZPoint&  globalPointExit,
                           FTPair&                 vectChanAndFrac) const;

  /** Get the sipmID, cellID and fractional position of a hit.
   *  @param uCoord u-coordinate of the hit
   *  @param quarter FT quarter of the hit
   *  @param sipmID SiPM ID (set by the function)
   *  @param cellID cellID inside the relevant SiPM (set by the function)
   *  @param fracDistCellCenter Distance between the hit and the center
   *  of the relevant cell, as a fraction of the cell size (set by the function)
   */
  void cellIDCoordinates(const double        uCoord, 
                         const unsigned int  quarter,
                         unsigned int&       sipmID,
                         unsigned int&       cellID, 
                         double&             fracDistCellCenter) const;

  /// Get the u-coordinate of the cell center
  double cellUCoordinate(const LHCb::FTChannelID aChan) const;

  /** Convert 'gross' cellID (counts sensitive and non-sensitive cells/SiPM edges)
   *  to 'net' cellID (counts only sensitive cells).
   *  @param grossID Gross cellID
   *  @return Net cellID (in case that the provided grossID corresponds to
   *  non-sensitive cell returns a number larger than the total number of
   *  channels per SiPM)
   */
  unsigned int netCellID(const unsigned int grossID) const;

  /** Convert 'net' cellID (counts only sensitive cells) to 
   *  'gross' cellID (counts sensitive and non-sensitive cells/SiPM edges).
   *  @param netID Net cellID
   *  @return Gross cellID
   */
  unsigned int grossCellID(const unsigned int netID) const;

  /** Determine the XYZ crossing point of a straight line determined by
   *  @param gpEntry Global entry point
   *  @param gpExit  Global exit point
   *  and the vertical (or tilted in the case of u/v layers) plane between two cells.
   *  @param pIntersect Global intersection point (set by the function)
   *  @return Status of the execution
   */
  StatusCode cellCrossingPoint(const double            cellEdgeU,
                               const Gaudi::XYZPoint&  gpEntry,
                               const Gaudi::XYZPoint&  gpExit,
                               Gaudi::XYZPoint&        pIntersect) const;

  /** Create a DetectorSegment (straight line representing an FT cell)
   *  from a global XYZ point
   */
  DetectorSegment createDetSegment(const Gaudi::XYZPoint& globalPoint) const;

  /** Create a DetectorSegment (straight line representing an FT cell)
   *  from an FTChannelID
   */
  DetectorSegment createDetSegment(const LHCb::FTChannelID& aChan, double fracPos) const;

  /** Function to determine the y coordinate of the crossing point between
   *  the beam-pipe hole (circle) and the fibres. Purely geometrical function.
   *  @param x0 u-coordinate of the fibre (i.e. x@y=0)
   *  @param ySign is the fibre at the top or at the bottom
   *  @param yIntersect y-coordinate of the crossing point (set by the function)
   *  @return StatusCode: does the fibre trajectory cross the beam-pipe circle
   */
  StatusCode beamPipeYCoord(const double x0, const int ySign, double& yIntersect) const;

  /** Get the FTChannelID of the cell located on the left of the given cell. */
  LHCb::FTChannelID nextChannelLeft(const LHCb::FTChannelID& aChan) const;

  /** Get the FTChannelID of the cell located on the right of the given cell. */
  LHCb::FTChannelID nextChannelRight(const LHCb::FTChannelID& aChan) const;
  

protected:

private:

  //?? Some of these params to go into the xml DDDB?
  
  unsigned int m_layerID;
  
  double m_angle;               ///< stereo angle of the layer
  double m_tanAngle;            ///< tangent of stereo angle

  /// Layer dimensions
  double m_layerMinX, m_layerMinY;
  double m_layerMaxX, m_layerMaxY;
  double m_layerHalfSizeX, m_layerHalfSizeY, m_layerHalfSizeZ;
  double m_innerHoleRadius;
  double m_layerPosZ;           ///< center of the layer in z
  /// SiPM and cell sizes
  unsigned int m_sipmNChannels; ///< number of channels per sipm
  double m_cellSizeX;
  double m_sipmSizeX;
  /// Gaps
  double m_sipmEdgeSizeX;       ///< x-gap between the active area and the outer edge of a sipm
  double m_gapXLayerHalves;     ///< half x-gap between left and right detector halves
  /// Parameters derived from the above values
  double m_sipmPitchX;          /// = m_sipmSizeX + 2*m_gapXsipmEdge
  unsigned int m_nSipmPerQuarter;  /// = int(m_layerHalfSizeX/m_sipmPitchX)
  double m_gapXLayerOuterEdge;  /// = m_layerHalfSizeX - m_nSipmPerQuarter*m_sipmPitchX
  /// global x@y=0 positions of the SiPM origins in the 4 quarters. Due to the SiPM numbering
  /// these positions are chosen to signify the right/left SiPM edge for the (x>0)/(x<0) halves.
  std::vector<double> m_sipmOriginX;
  /// Distance between two SiPMs in the 4 quarters. Can be positive or negative which reflects
  /// the direction of increasing the SiPM ID.
  std::vector<double> m_sipmStepX;

};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------

inline unsigned int DeFTLayer::layerID() const { return m_layerID; }
inline void DeFTLayer::setLayerID(unsigned int layerID) { m_layerID = layerID; }

inline double DeFTLayer::angle()    const { return m_angle; }
inline double DeFTLayer::tanAngle() const { return m_tanAngle; }
inline void   DeFTLayer::setAngle(double angle) {
  m_angle = angle;
  m_tanAngle = tan(angle);
}

/**
 * Calculate the x-position at the top/bottom of a FT layer (where the SiPM are).
 * Extrapolate the initial (x0,y0) along a straight line inclined
 * by m_stereoAngle wrt the y axis. It is assumed that the stereo angle is positive
 * when the angle between x and y' is > 90 deg (and therefore dx/dy is < 0).
 */
inline double DeFTLayer::xAtVerticalBorder(double x0, double y0) const {
  // in case of negligible stereo angle return x0
  if (std::abs(m_angle)<1.e-4) return x0;
  else {
    double yAtBorder = (y0>0) ? m_layerMaxY : m_layerMinY;
    return x0 + (yAtBorder-y0)*m_tanAngle;
  }
}

inline double DeFTLayer::xAtYEq0(double x0, double y0) const {
  // in case of negligible stereo angle return x0
  if (std::abs(m_angle)<1.e-4) return x0;
  else return x0 - y0*m_tanAngle;
}


#endif // DEFTLAYER_H
