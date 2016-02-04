#ifndef DEFTFIBREMAT_H
#define DEFTFIBREMAT_H 1

// DetDesc
#include "DetDesc/DetectorElement.h"
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/SolidBox.h"
#include "DetDesc/LogVolBase.h"  //DBL: those 2 includes are mandatory
#include "DetDesc/PVolume.h"     //DBL


// Kernel
#include "Kernel/FTChannelID.h"
#include "Kernel/DetectorSegment.h" // Geom. representation of the FT cell/channel

// from Event
#include "Event/MCHit.h"


/** @class Deftfibremat DeFTFibreMat.h "FTDet/DeFTFibreMat.h"
 *
 *  This is the detector element class of the Fibre Tracker (FT) fibremat.
 *  As it is the FT sub-structure det. element of highest granularity, for version 20 in xml, 
 *  it provides the actual methods for making the conversion geometrical point <--> FTChannelID.
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
 *  Consequently, the 'net' cellID can have values in [0, 127].
 * 
 *  @author Plamen Hopchev
 *  @author Eric Cogneras
 *  @author Diego Milanes
 *  @date   2013-07-12
 */

static const CLID CLID_DeFTFibreMat = 8607;

typedef std::pair<LHCb::FTChannelID, double> FTPair;
typedef std::vector< FTPair > VectFTPairs;

class DeFTFibreMat : public DetectorElement {

public: 

  /// Standard constructor
  DeFTFibreMat( const std::string& name = "" );
  
  /// Destructor
  ~DeFTFibreMat( ) override;

  /** Initialization method 
   *  @return Status of initialization
   */ 
  StatusCode initialize() override;

  /** Finalization method - delete objects created with new
   *  @return Status of finalization
   */ 
  StatusCode finalize(); //@TODO: note: no override!!! (so who calls this?)

  /** Retrieves reference to class identifier
   *  @return The class identifier for this class
   */
  const CLID& clID() const override;

  /** Another reference to class identifier
   *  @return The class identifier for this class
   */
  static const CLID& classID() { return CLID_DeFTFibreMat; }

  /** Get the list of SiPM channels traversed by the hit, (including the light sharing)
   *  The particle trajectory is a straight line defined by:
   *  @param MChit providing globalPointEntry Global entry and exit point
   *  Fills a vector of 'FT pairs' (channel ID and fraction of the energy
   *  deposited in it by the MC particle passed as an argument.
   *  @return Status of the execution
   */
  StatusCode calculateListOfFiredChannels(const LHCb::MCHit&  fthit,
                                          VectFTPairs&        vectChanAndFrac) const;

  /** Get the position of the mean SiPM channels plus fractionnal position within the channel
   *  of the hit.
   *  The particle trajectory is a straight line defined by:
   *  @param MChit providing globalPointEntry Global entry and exit point
   *  Fills a 'FT pairs' (channel ID and fraction of the energy
   *  deposited in it by the MC particle passed as an argument.
   *  @return Status of the execution
   */
  StatusCode calculateMeanChannel(const LHCb::MCHit&  fthit,
                                  FTPair&             ChanAndFrac) const;

  /** Get the list of SiPM channels traversed by the hit.
   *  The particle trajectory is a straight line defined by:
   *  @param MChit providing globalPointEntry Global entry and exit point
   *  Fills a vector of 'FT pairs' (channel ID and fraction of the crossed length).
   *  @return Status of the execution
   */
  StatusCode calculateHits(const LHCb::MCHit&  fthit,
                           VectFTPairs&        vectChanAndFracPos) const;


  /** This function returns the fibre lengh and relative position of the hit 
   *  in the fibre according to the Hit position.
   *  @param globalPointEntry Global entry point
   *  @param globalPointExit Global exit point
   *  @param fibre lengh for the mean y-value of the hit (from entry to exit point)
   *  @param relative position of the hit in the fibre wrt the SiPm position
   */
  StatusCode hitPositionInFibre(const LHCb::MCHit&  fthit,
                                double& meanfibrefullLengh,
                                double& fibreLenghFrac)const;
                                
                                
                                

  /** Get the sipmID, cellID and fractional position of a hit.
   *  @param uCoord u-coordinate of the hit
   *  @param quarter FT quarter of the hit
   *  @param sipmID SiPM ID (set by the function)
   *  @param cellID cellID inside the relevant SiPM (set by the function)
   *  @param fracDistCellCenter Distance between the hit and the center
   *  of the relevant cell, as a fraction of the cell size (set by the function)
   *  @param sipmREdgeX "right" (ie when looking in z>0 global frame direction) sipm active edge
   */
  void cellIDCoordinates(const double        uCoord, 
                         const unsigned int  quarter,
                         unsigned int&       sipmID,
                         unsigned int&       cellID, 
                         double&             fracDistCellCenter,
                         double&             sipmREdgeX) const;
  
  //Various accessors
  bool isBottom() const { return m_mat; }
  int module() const { return m_module; }
  int quarter() const { return m_quarter; }
  int layer() const { return m_layer; }
  bool isHoley () const { return m_holey; }

  /// Get the layer ID
  unsigned int FibreMatID() const { return m_FibreMatID; }
  unsigned int layerID() const { return m_layerID; }

  /// Get the layer stereo angle
  double angle() const { return m_angle; }

  /** @return Tangent of the stereo angle of the layer */
  double tanAngle() const { return m_tanAngle; }

  /** @return Layer slope in the y-z plane */
  double slopeDzDy() const { return m_dzDy; }

  /// Get the u-coordinate of the cell center
  double cellUCoordinate(const LHCb::FTChannelID& channel) const;

  /// Get the fibremat local coordinate of the cell center
  double cellLocalX(const LHCb::FTChannelID& channel) const;

  /// Accessor to the minimal x-position of the layer area covered with fibres
  double layerMinX() const { return m_layerMinX; }

  /// Accessor to the maximal x-position of the layer area covered with fibres
  double layerMaxX() const { return m_layerMaxX; }

  /// Accessor to the minimal y-position of the layer area covered with fibres
  double layerMinY() const { return m_layerMinY; }

  /// Accessor to the maximal y-position of the layer area covered with fibres
  double layerMaxY() const { return m_layerMaxY; }

  /// Accessor to the minimal z-position of the layer
  double layerMinZ() const { return m_layerMinZ; }

  /// Accessor to the maximal z-position of the layer
  double layerMaxZ() const { return m_layerMaxZ; }


  /// Accessor to the minimal x-position of the fibreMat area
  double fibreMatMinX() const { return m_fibreMatGlobalRT.X(); }

  /// Accessor to the maximal x-position of the fibreMat area
  double fibreMatMaxX() const { return m_fibreMatGlobalLT.X(); }

  /// Accessor to the minimal y-position of the fibreMat area
  double fibreMatMinY() const { return m_fibreMatGlobalRB.Y(); }

  /// Accessor to the maximal y-position of the fibreMat area
  double fibreMatMaxY() const { return m_fibreMatGlobalRT.Y(); }

  /// Accessor to the minimal z-position of the fibreMat area
  double fibreMatMinZ() const { return fibreMatGlobalCenter().Z()-m_fibreMatHalfSizeZ; }

  /// Accessor to the maximal z-position of the fibreMat area
  double fibreMatMaxZ() const { return fibreMatGlobalCenter().Z()+m_fibreMatHalfSizeZ; }


  /// Accessor to the GLOBAL frame position of fibreMat local frame origin
  Gaudi::XYZPoint fibreMatGlobalCenter() const { return m_fibreMatGlobalCenter; }

  /// Accessor to the GLOBAL frame position of (local) Right Top fibreMat active area corner
  Gaudi::XYZPoint fibreMatGlobalRT() const { return m_fibreMatGlobalRT; }
  
  /// Accessor to the GLOBAL frame position of (local) Left Top fibreMat active area corner
  Gaudi::XYZPoint fibreMatGlobalLT() const { return m_fibreMatGlobalLT; }
  
  /// Accessor to the GLOBAL frame position of(local)  Right Bottom fibreMat active area corner
  Gaudi::XYZPoint fibreMatGlobalRB() const { return m_fibreMatGlobalRB; }

  /// Accessor to the GLOBAL frame position of(local)  Left Bottom fibreMat active area corner
  Gaudi::XYZPoint fibreMatGlobalLB() const { return m_fibreMatGlobalLB; }

  /// Accessor to the fibre end y-position in local frame (top/bottom related)
  double fibreMatSipmY() const { return m_fibreMatSipmY; }



  /// Accessor to the z-position of the layer center
  double layerCenterZ() const { return m_fibreMatGlobalCenter.Z(); }

  /// Accessor to the layer beam-pipe radius (v2 geometry)
  double layerInnerHoleRadius() const { return m_innerHoleRadius; }

  /// Get the FTChannelID of the cell located on the left of the given cell
  LHCb::FTChannelID nextChannelLeft(const LHCb::FTChannelID& channel) const;

  /// Get the FTChannelID of the cell located on the right of the given cell
  LHCb::FTChannelID nextChannelRight(const LHCb::FTChannelID& channel) const;


  /** Create a DetectorSegment (straight line representing an FT cell)
   *  from an FTChannelID and fractional position within the relevant cell
   */
  DetectorSegment createDetSegment(const LHCb::FTChannelID& channel, double fracPos) const;


  /** Function encapsulating the creation of FTChannelIDs.
   *  @param hitLayer FT layer ID of the channel
   *  @param quarter FT quarter of the channel
   *  @param simpID SiPM ID of the channel
   *  @param grossCellID Used to determine the CellID of the channel
   *  @return FTChannelID
   */
  LHCb::FTChannelID createChannel(unsigned int hitLayer,
                                  int          module,
                                  int          mat,
                                  unsigned int sipmID,
                                  unsigned int grossCellID) const;


  /// Make the Test algo a friend so that it can call private methods
  friend class DeFTTestAlg;



private: //-----private member functions

  /** Get the x-position at the top/bottom of the layer by extrapolating
   *  along the fibres the initial
   *  @param x0 x-position
   *  @param y0 y-position
   *  to the top/bottom of the layer.
   *  @return x-position at the layer top (if y0 > 0) or bottom (if y0 < 0)
   *  It is assumed that the stereo angle is positive when the angle between
   *  x and y' is > 90 deg (and therefore dx/dy is < 0).
   */
  double xAtVerticalBorder(double x0, double y0) const;

  /** Get the x-position at y=0 by extrapolating along the fibres the initial
   *  @param x0 x-position
   *  @param y0 y-position
   *  to y=0.
   *  @return x-position at y=0
   */
  double xAtYEq0(double x0, double y0) const {
    //return x0 - y0*m_tanAngle;
    return x0 + y0*m_tanAngle;    //DBL
  }


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
   *  In LOCAL fibremat frame
   */
  StatusCode cellCrossingPointLocal(const double       lcellEdgeX,
                                    const Gaudi::XYZPoint&  lpEntry,
                                    const Gaudi::XYZPoint&  lpExit,
                                    Gaudi::XYZPoint&        pIntersect) const;
                               


  /** Function to determine the y coordinate of the crossing point between
   *  the beam-pipe hole (circle) and the fibres. Purely geometrical function.
   *  @param X coordinate
   *  @param Y coordinate
   *  @param yIntersect y-coordinate of the crossing point (set by the function)
   *  @return StatusCode: does the fibre trajectory cross the beam-pipe circle
   */
  double  beamPipeYCoord(double xcoord, double ycoord) const;

  /** Function for light sharing between neighbouring SiPM cells.
   *  This model uses straight lines for describing the fibre fractions
   *  falling into a left, central and right SiPM cells.
   *  @param vectChanAndFrac vector of FT pairs (FTChannels and the associated
   *  fractional positions of the MC particle trajectory in this SiPM cell).
   *  @return A new vector of FT pairs (created inside the function), where the
   *  scalars associated with the FT channels indicate the fraction of the energy
   *  of the MC hit in that channel.
   */
  VectFTPairs createLightSharingChannels(VectFTPairs& inputVectPairs) const;

  /** Function to calculate the light sharing fractions in the left/central/right SiPM cells.
   *  @param normalized position of the MC particle trajectory in the central cell.
   *  @param fractions vector to be filled with the left/central/right energy fractions.
   */
  std::array<double,3> lightSharing( double position ) const;

  /** FibreLengh function determines the full lengh of the fibre as a function of its location 
   *  to take the beam-pipe hole (circle) into account, but also the stereo angle.
   *  @param lpEntry Lobal entry point
   *  @param lpEntry Lobal entry point
   *  @return fibre lengh 
   */
  double FibreLengh(const Gaudi::XYZPoint&  lpEntry,
                    const Gaudi::XYZPoint&  lpExit) const;


private: // private data members


  ///Detector info
  std::string  m_DeFTLocation = {"/dd/Structure/LHCb/AfterMagnetRegion/T/FT"};
  unsigned int m_FTGeomversion;   ///Geometry version
  unsigned int m_FTGeomVersion_reference = 20;
  unsigned int m_BadChannelLayerFlag = 15;  ///used for flagging problematich channels (layer=15)
  
  ///general geometry info 
  double m_angle = 0;               ///< stereo angle of the layer
  double m_tanAngle = 0;            ///< tangent of stereo angle
  double m_cosAngle = 0;            ///< cos of stereo angle
  double m_dzDy = 0;                ///< layer slope in the y-z plane
  bool m_RightHoleAxesXZInversion;   ///flag for temporary Right Holes axes inversion (v4)
  double m_Ztolerance;     ///tolerances used in testing some spatial limits
  double m_dZtolerance;      ///
  double m_ARtolerance;  //machine arithmetic toerance

  ///ChannelID info and related
  unsigned int m_FibreMatID = 0;
  unsigned int m_layerID = 0;
  int m_mat = 0;
  int m_module = 0;
  int m_layer = 0;
  int m_quarter = 0;
  int m_relativemodule = 0;
  bool m_holey = false;

  ///Fibremat info
  double m_fibreMatHalfSizeX=0, m_fibreMatHalfSizeY=0, m_fibreMatHalfSizeZ=0;
  //Fibremat global frame center and corners (Right,Left,Top,Bottom)
  Gaudi::XYZPoint m_fibreMatGlobalCenter;   ///Origin of local fibremat frame in global frame
  Gaudi::XYZPoint m_fibreMatGlobalRT;       ///Right Top (local) corner of fibremat global coordinates
  Gaudi::XYZPoint m_fibreMatGlobalLT;       ///Left Top (local) corner of fibremat global coordinates
  Gaudi::XYZPoint m_fibreMatGlobalRB;       ///Right Bottom (local) corner of fibremat global coordinates
  Gaudi::XYZPoint m_fibreMatGlobalLB;       ///Left Bottom (local) corner of fibremat global coordinates   

  ///FibreModules info
  double m_FibreModuleHalfSizeX;   ///FibreModule X half size
  double m_FibreModuleHalfSizeY;   ///FibreModule Y half size
  double m_FibreModuleHalfSizeZ;   ///FibreModule Z half size
  
  ///Layer info
  double m_layerMinX=0, m_layerMaxX=0;
  double m_layerMinY=0, m_layerMaxY=0;
  double m_layerMinZ=0, m_layerMaxZ=0;
  double m_layerHalfSizeX=0, m_layerHalfSizeY=0, m_layerHalfSizeZ=0;
  double m_innerHoleRadius=0;     ///for v2 and before
  
  
  ///Gaps
  double m_sipmEdgeSizeX, m_moduleEdgeSizeX; ///< x-gap between the active area and the outer edge
                                             ///of a sipm and same for module
  double m_moduleGapH;


  ///SiPM and cell sizes
  unsigned int m_sipmNChannels; ///< number of channels per sipm
  double m_cellSizeX;
  double m_sipmSizeX;
  /// Parameters derived from the above values
  double m_sipmPitchX=0;             /// X size of full sipm with gaps
  unsigned int m_nSipmPerQuarter;  
  //variables for sipm at fibremat level
  unsigned int m_nSipmPerModule=0;
  double m_fibreMatSipmY;    ///end fibre position in local frame (depends on top/bottom mats)
  double m_sipmOriginX;       ///X origin of sipms frame in fibremat
  double m_sipmPitchXsigned;


  /// Holes
  Gaudi::XYZPoint m_posHole={0,0,0};   ///Hole position
  double m_HoleShiftX=0;    ///X Hole shift in local frame
  double m_HoleShiftY=0;    ///Y Hole shift in local frame
  double m_halfHole1X=0;    ///Hole in local frame X, section 1
  double m_halfHole2X=0;    ///Hole in local frame X, section 2
  double m_halfHole3X=0;    ///Hole in local frame X, section 3
  double m_halfHole4X=0;    ///Hole in local frame X, section 4
  double m_halfHole1Y=0;    ///Hole in local frame Y, section 1, T station dependent
  double m_halfHole2Y=0;    ///Hole in local frame Y, section 2, T station dependent
  double m_halfHole3Y=0;    ///Hole in local frame Y, section 3, T station dependent
  double m_halfHole4Y=0;    ///Hole in local frame Y, section 4, T station dependent

  
  ///---Auxiliary stuff
  /// Use a single MsgStream instance (created in initialize)
  std::unique_ptr<MsgStream> m_msg;
  /// Print functions
  MsgStream& debug()   const { return *m_msg << MSG::DEBUG; }
  MsgStream& info()    const { return *m_msg << MSG::INFO; }
  MsgStream& error()   const { return *m_msg << MSG::ERROR; }
  MsgStream& fatal()   const { return *m_msg << MSG::FATAL; }
 
  ///Beam Hole acceptance, depending on geometry versions. Returns also maximum fibre length */
  bool inBeamHole(const Gaudi::XYZPoint hitLocal, double& fibrelengthMax) const;
 
  /// find solidbase for PVolume of given detector element */
  StatusCode findSolidBase(IDetectorElement *det, const std::string& pvolname, const SolidBase* &solidbase);

  ///temporary workaround for Right Hole (module 11) axes inversion
  void doRHAxesInversion(Gaudi::XYZPoint& xyzLocal) const;
  
  //check if point is in active area (local frame)
  bool inActiveArea(const Gaudi::XYZPoint& xyzLocal, double tolerance=0) const;
  
  //Validate acceptance and other things for MC hit, find sipm info for entry and exit hit
  StatusCode CalculateSipmInfo(const Gaudi::XYZPoint enPLocal, const Gaudi::XYZPoint exPLocal,
                               unsigned int &enPSipmID, unsigned int &enPCellID, double &enPFraction, double &ensipmREdgeX,
                               unsigned int &exPSipmID, unsigned int &exPCellID, double &exPFraction, double &exsipmREdgeX) const;
    
};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------

#endif // DEFTFIBREMAT_H
