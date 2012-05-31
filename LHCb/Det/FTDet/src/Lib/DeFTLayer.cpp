// $Id: $

// DetDesc
#include "DetDesc/SolidSubtraction.h"
#include "DetDesc/SolidChild.h"
#include "DetDesc/SolidBox.h"
#include "DetDesc/SolidTubs.h"

// Gaudi/LHCb Math
#include "GaudiKernel/Plane3DTypes.h"
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "LHCbMath/Line.h"
#include "LHCbMath/GeomFun.h"
#include "LHCbMath/LineTypes.h"

// FTDet
#include "FTDet/DeFTLayer.h"

/** @file DeFTLayer.cpp
 *
 *  Implementation of class : DeFTLayer
 *
 *  @author Plamen Hopchev
 *  @date   2012-04-25
 */

using namespace LHCb;

typedef Gaudi::Math::Line<Gaudi::XYZPoint,Gaudi::XYZVector> Line;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DeFTLayer::DeFTLayer( const std::string& name ) :
  DetectorElement ( name ),
  m_layerID(0),
  // Layer stereo angle
  m_angle(0.0),
  m_tanAngle(0.0),
  // Layer dimensions
  m_layerMinX(0.),
  m_layerMinY(0.),
  m_layerMaxX(0.),
  m_layerMaxY(0.),
  m_layerHalfSizeX(0.),
  m_layerHalfSizeY(0.),
  m_layerHalfSizeZ(0.),
  m_innerHoleRadius(0.),
  m_layerPosZ(0.),
  // SiPM and cell sizes
  m_sipmNChannels(128),
  m_cellSizeX(0.25),
  m_sipmSizeX(32.25),
  // Gaps
  m_sipmEdgeSizeX(0.25), /// in the sipm layout it is 0.17 (we add a bit)
  m_gapXLayerHalves(0.0),
  // Derived params
  m_sipmPitchX(0.),
  m_nSipmPerQuarter(0),
  m_gapXLayerOuterEdge(0.0)
{

}

//=============================================================================
// Destructor
//=============================================================================
DeFTLayer::~DeFTLayer(){} 

//=============================================================================
// classID function
//=============================================================================
const CLID& DeFTLayer::clID () const {
  return DeFTLayer::classID();
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode DeFTLayer::initialize(){

  StatusCode sc = DetectorElement::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;
  
  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "==> Initialize DeFTLayer" << endmsg;

  /// Get geometrical limits of the layer
  const SolidSubtraction* subtrObject = dynamic_cast<const SolidSubtraction*>( this->geometry()->lvolume()->solid() );
  const SolidBox*   outerBox = dynamic_cast<const SolidBox*>( subtrObject->coverTop() );
  const SolidChild* tmpChild = dynamic_cast<const SolidChild*>( (*subtrObject)[0] );
  const SolidTubs* innerTube = dynamic_cast<const SolidTubs*>( tmpChild->solid() );

  if ( !subtrObject || !outerBox || !innerTube ) {
    msg << MSG::FATAL << "Can't acquire FT layer geometrical properties. Break ..." << endmsg;
    return StatusCode::FAILURE;
  }
  
  m_layerHalfSizeX = outerBox->xHalfLength();
  m_layerHalfSizeY = outerBox->yHalfLength();
  m_layerHalfSizeZ = outerBox->zHalfLength();
  m_innerHoleRadius = innerTube->outerRadius();
  m_layerPosZ      = (this->geometry()->toGlobal(Gaudi::XYZPoint(0.,0.,0.))).z();

  m_sipmPitchX = m_sipmSizeX + 2*m_sipmEdgeSizeX;
  m_nSipmPerQuarter = int(m_layerHalfSizeX/m_sipmPitchX);
  m_gapXLayerOuterEdge = m_layerHalfSizeX - m_nSipmPerQuarter*m_sipmPitchX;
  m_layerMinX = -m_layerHalfSizeX + m_gapXLayerOuterEdge;
  m_layerMaxX = -m_layerMinX;

  m_layerMinY = -m_layerHalfSizeY;
  m_layerMaxY = -m_layerMinY;

  /*
  msg << MSG::DEBUG << "Derived parameters:"
      << "\n\tpitch X: " << m_sipmPitchX
      << "\n\tN of SiPM per quarter: " << m_nSipmPerQuarter
      << "\n\tOuter edge width X: " << m_gapXLayerOuterEdge
      << "\n\tLayer min X: " << m_layerMinX
      << "\n\tLayer max X: " << m_layerMaxX << endmsg;
  */    

  ///fill in the vectors holding the x starting positions (outermost SiPMs)
  double xOrigin, xStep;
  unsigned int iQ;
  for (iQ=0; iQ<4; ++iQ) {
    xOrigin = 0.;
    xStep = (iQ%2) ? m_sipmPitchX : -m_sipmPitchX;
    m_sipmOriginX.push_back(xOrigin);
    m_sipmStepX.push_back(xStep);
    //msg << MSG::DEBUG << "iQ = " << iQ << ", SiPM Origin = " << m_sipmOriginX[iQ]
    //    << ", Step = " << m_sipmStepX[iQ] << endmsg;
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Function to determine which FT channels are traversed by a trajectory
// determined by globalPointEntry and globalPointExit. Fills a vector of
// pairs <FTChannelID, fraction>. The latter is the "signed distance to the
// cell center normalized to the cell size" of the "middle of the trajectory
// path in the relevant cell". 'fraction' varies between -0.5 and 0.5.
//=============================================================================
StatusCode DeFTLayer::calculateHits(const Gaudi::XYZPoint& globalPointEntry,
                              const Gaudi::XYZPoint& globalPointExit,
                              FTPair& vectChanAndFrac) const
{
  Gaudi::XYZPoint enP  = this->geometry()->toLocal(globalPointEntry);
  Gaudi::XYZPoint exP  = this->geometry()->toLocal(globalPointExit);
  
  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Entry Point in Global / Local: " << globalPointEntry << enP << endmsg;
  msg << MSG::DEBUG << "Exit  Point in Global / Local: " << globalPointExit  << exP << endmsg;

  unsigned int hitLayer = this->layerID();
  msg << MSG::DEBUG << "LayerID = " << hitLayer
      << ", Stereo angle = " << this->angle() << endmsg;

  /// Make a check if the entry and exit points are in the same z plane
  if ( std::abs(globalPointEntry.z() - globalPointExit.z()) < 1.e-2 ) {
    msg << MSG::DEBUG << "Aborting calculateHits(...) because z-distance "
        << "between entry and exit points is less than 10 micron." << endmsg;
    return StatusCode::FAILURE;
  }
  
  /// Clear the vector holding FT pairs
  vectChanAndFrac.clear();
  
  /// Make a check if both the entry and exit points are inside the active area.
  /// This will be done by determining the SiPM ID corresponding to the point
  /// and comparing with nSiPMPerQuarter.

  /// Check if the entry or exit points are inside the beam hole: assume a square
  if ( ((std::abs(enP.x()) < m_innerHoleRadius) && (std::abs(enP.y()) < m_innerHoleRadius)) || 
       ((std::abs(exP.x()) < m_innerHoleRadius) && (std::abs(exP.y()) < m_innerHoleRadius)) ) {
    msg << MSG::DEBUG << "Aborting calculateHits(...) because entry or exit points are inside "
        << "the beam pipe hole (square)" << endmsg;
    return StatusCode::FAILURE;
  }

  ///////////////////////////////////////////////////////
  /// Get cell coordiantes of the entry and exit points
  ///////////////////////////////////////////////////////

  /// extrapolate points along the fibre to y=0
  double enPU = xAtYEq0( enP.x(), enP.y());
  double exPU = xAtYEq0( exP.x(), exP.y());
  msg << MSG::DEBUG << "U Coordinates of enP and exP: " << enPU << ", " << exPU << endmsg;

  int enPQuarter=0, exPQuarter=0;
	if ( enPU    > 0 ) enPQuarter += 1;
	if ( enP.y() > 0 ) enPQuarter += 2;   /// The separation top-bottom is at y=0, not perpendicular to the fibre.
	if ( exPU    > 0 ) exPQuarter += 1;
	if ( exP.y() > 0 ) exPQuarter += 2;

  if ( enPQuarter != exPQuarter ) {
    msg << MSG::DEBUG << "Aborting calculateHits(...) because entry "
        << "and exit points are in different quarters!" << endmsg;
    return StatusCode::FAILURE;
  }

  int quarter = enPQuarter;
  unsigned int enPSipmID, enPCellID, netCellID;
  unsigned int exPSipmID, exPCellID;
  double enPFraction, exPFraction;
  FTChannelID aChan;

  cellIDCoordinates( enPU, enPQuarter, enPSipmID, enPCellID, enPFraction );
  cellIDCoordinates( exPU, exPQuarter, exPSipmID, exPCellID, exPFraction );

  if ( (enPSipmID > m_nSipmPerQuarter) || (exPSipmID > m_nSipmPerQuarter) ) {
    msg << MSG::DEBUG << "Aborting calculateHits(...) because entry "
        << "or exit points are outside acceptance (we get too large sipmID)" << endmsg;
    return StatusCode::FAILURE;
  }

  ///////////////////////////////////////////////////////
  /// Create pairs of FT channels and fractional position
  ///////////////////////////////////////////////////////

  /// Case where entry and exit points are in the same cell
  if ( (enPSipmID==exPSipmID) && (enPCellID==exPCellID) ) {
    msg << MSG::DEBUG << "Entry and Exit points are in the same cell!" << endmsg;
    
    double frac = (enPFraction + exPFraction)/2;
    msg << MSG::DEBUG << "Average fract dist to cell center: " << frac << endmsg;

    // Convert the grossCellID to netCellID
    netCellID = this->netCellID(enPCellID);

    /// Create and push_back the corresponding FT pair
    if ( netCellID > (m_sipmNChannels-1) ) {
      msg << MSG::DEBUG << "Gross cellID " << enPCellID << " corresponds to insensitive cell."
          << " Creating invalid FT channel (the signature is: layerID=15)." << endmsg;
      aChan = FTChannelID( 15, 0, 0, 0 );
    }
    else {
      aChan = FTChannelID( hitLayer, quarter, enPSipmID, netCellID );
    }
    vectChanAndFrac.push_back( std::make_pair(aChan, frac) );
  } //end single cell

  /// Case where entry and exit points are in different cells
  else {
    msg << MSG::DEBUG << "Entry and Exit points are in different cells!" << endmsg;
    /// The procedure is to determine the crossing point between the particle
    /// trajectory and the planes seprating the adjacent cells. First we determine
    /// the u-coordinate of the cell border between the first and second cells
    /// and between the last and last-but-once cells. Then we determine the crossing
    /// points. The hits in the middle cells will have fractional position equal
    /// to 0 (i.e. at the cell center).

    /// u-coordinate decreases or increases with z?
    double dUExEn = exPU - enPU;
    int uDir = (dUExEn>0) ? 1 : -1;

    /// Cell edge next to the entry point (can be on the left or on the right)
    double enPCellSeparEdge, enPDistToSeparEdge;
    enPCellSeparEdge = enPU - (enPFraction - 0.5*uDir) * m_cellSizeX;
    enPDistToSeparEdge = enPU - enPCellSeparEdge;

    msg << MSG::DEBUG << "Entry point: uDir / cellSeparEdge / distToSeparEdge: "
        << uDir << ", " << enPCellSeparEdge << ", " << enPDistToSeparEdge << endmsg;

    /// Cell edge next to the exit point
    double exPCellSeparEdge, exPDistToSeparEdge;
    exPCellSeparEdge = exPU - (exPFraction + 0.5*uDir) * m_cellSizeX;
    exPDistToSeparEdge = exPU - exPCellSeparEdge;

    msg << MSG::DEBUG << "Exit point: uDir / cellSeparEdge / distToSeparEdge: "
        << uDir << ", " << exPCellSeparEdge << ", " << exPDistToSeparEdge << endmsg;

    //////////////////////////////////////////////////////////////////
    /// Calculate the fractional position of the particle path center inside
    /// each traversed cell. Create and push_back the relevant FT pairs
    //////////////////////////////////////////////////////////////////
    StatusCode sc;
    Gaudi::XYZPoint pIntersect(0.,0.,0.);
    double fracP1, fracP2;

    /// The cell of the entry point
    double fracPosFirstCell = 999.;
    sc = cellCrossingPoint( enPCellSeparEdge, globalPointEntry, globalPointExit, pIntersect );
    // in case of a problem --> directly exit the function
    if ( sc.isFailure() ) {
      msg << MSG::DEBUG << "Aborting calculateHits(...), because of unsuccessful "
          << "call to cellCrossingPoint(EntryPoint)" << endmsg;
      return sc;
    }
    
    fracP1 = enPFraction;
    fracP2 = (dUExEn>0) ? 0.5 : -0.5;
    fracPosFirstCell = (fracP1 + fracP2)/2;
    msg << MSG::DEBUG << "Entry Point, SC = " << sc << " Intersection point: " << pIntersect << endmsg;
    msg << MSG::DEBUG << "Entry Point, Frac = " << fracPosFirstCell << endmsg;

    // Convert the grossCellID to netCellID
    netCellID = this->netCellID(enPCellID);

    /// Create and push_back the corresponding FT pair
    if ( netCellID > (m_sipmNChannels-1) ) {
      msg << MSG::DEBUG << "Gross cellID " << enPCellID << " (entryCell) corresponds to insensitive cell."
          << " Creating invalid FT channel (the signature is: layerID=15)." << endmsg;
      aChan = FTChannelID( 15, 0, 0, 0 );
    }
    else {
      aChan = FTChannelID( hitLayer, quarter, enPSipmID, netCellID );
    }
    vectChanAndFrac.push_back( std::make_pair(aChan, fracPosFirstCell) );
    
    /// The middle cells
    double eps = 1.e-3; /// add this distance when determining the number of middle cells
    int nMiddleCells( (std::abs(exPCellSeparEdge - enPCellSeparEdge) + eps) / m_cellSizeX );
    unsigned int midCellSipmID, midCellID;
    double midCellFraction;
    int i;
    for (i=0; i<nMiddleCells; ++i) {
      double midCellCenterU = enPCellSeparEdge + uDir*m_cellSizeX*(i+0.5);
      msg << MSG::DEBUG << "\tMid Cell " << i << " midCellCenterU = " << midCellCenterU << endmsg;
      cellIDCoordinates( midCellCenterU, quarter, midCellSipmID, midCellID, midCellFraction );

      // Convert the grossCellID to netCellID
      netCellID = this->netCellID(midCellID);

      if ( netCellID > (m_sipmNChannels-1) ) {
        msg << MSG::DEBUG << "Gross cellID " << midCellID << " (midCell) corresponds to insensitive cell."
            << " Creating invalid FT channel (the signature is: layerID=15)." << endmsg;
        aChan = FTChannelID( 15, 0, 0, 0 );
      }
      else {
        aChan = FTChannelID( hitLayer, quarter, midCellSipmID, netCellID );
      }
      vectChanAndFrac.push_back( std::make_pair(aChan, midCellFraction) );
    }// end loop over mid cells
    
    /// The cell of the exit point

    double fracPosLastCell = 999.;
    sc = cellCrossingPoint( exPCellSeparEdge, globalPointEntry, globalPointExit, pIntersect );
    // in case of a problem --> directly exit the function
    if ( sc.isFailure() ) {
      msg << MSG::DEBUG << "Aborting calculateHits(...), because of unsuccessful "
          << "call to cellCrossingPoint(ExitPoint)" << endmsg;
      return sc;
    }

    fracP1 = exPFraction;
    fracP2 = (dUExEn>0) ? -0.5 : 0.5;
    fracPosLastCell = (fracP1 + fracP2)/2;
    msg << MSG::DEBUG << "Exit Point, SC = " << sc << " Intersection point: " << pIntersect << endmsg;
    msg << MSG::DEBUG << "Exit Point, Frac = " << fracPosLastCell << endmsg;

    // Convert the grossCellID to netCellID
    netCellID = this->netCellID(exPCellID);

    /// Create and push_back the corresponding FT pair
    if ( netCellID > (m_sipmNChannels-1) ) {
      msg << MSG::DEBUG << "Gross cellID " << exPCellID << " (exitCell) corresponds to insensitive cell."
          << " Creating invalid FT channel (the signature is: layerID=15)." << endmsg;
      aChan = FTChannelID( 15, 0, 0, 0 );
    }
    else {
      aChan = FTChannelID( hitLayer, quarter, exPSipmID, netCellID );
    }
    vectChanAndFrac.push_back( std::make_pair(aChan, fracPosLastCell) );
  }//end more than 1 hit cells
  
  msg << MSG::DEBUG << "Finished creating FTPairs\n" << endmsg;
  
  /// Prinout the vector of FT pairs
  msg << MSG::DEBUG << "Size of vector of FT pairs: " << vectChanAndFrac.size() << endmsg;
  FTPair::const_iterator itPair;
  DetectorSegment tmpDetSeg;
  for (itPair = vectChanAndFrac.begin(); itPair != vectChanAndFrac.end(); ++itPair) {
    msg << MSG::DEBUG << itPair->first << ", FractPos: " << itPair->second << endmsg;
    /// Test the inverse function ///
    msg << MSG::DEBUG << "Test of function cellUCoordinate(FTChannelID): "
        << cellUCoordinate(itPair->first) << endmsg;
    /// Test of functions nextChannelLeft/Right
    msg << MSG::DEBUG << "Test of function nextChannelLeft:"
        << this->nextChannelLeft( itPair->first ) << endmsg;
    msg << MSG::DEBUG << "Test of function nextChannelRight:"
        << this->nextChannelRight( itPair->first ) << endmsg;
    /// Test the function FTChannel+Fraction --> DetSegment
    msg << MSG::DEBUG << "Test of function createDetSegment(FTChannelID, fracPos)" << endmsg;
    tmpDetSeg = createDetSegment( itPair->first, itPair->second );
  }

  return StatusCode::SUCCESS;
}

unsigned int DeFTLayer::netCellID(const unsigned int grossID) const {
  unsigned int netID=999; // any number above m_sipmNChannels
  if      ( grossID >  0 && grossID <  65 ) netID = grossID-1;
  else if ( grossID > 65 && grossID < 130 ) netID = grossID-2;
  return netID;
}

unsigned int DeFTLayer::grossCellID(const unsigned int netID) const {
  unsigned int grossID=999; // any number above m_sipmNChannels
  if      ( netID <  64 ) grossID = netID+1;
  else if ( netID < 128 ) grossID = netID+2;
  return grossID;
}

//=============================================================================
// Function to determine the local U coordinate at the center of an FT cell
// from a FTChannelID. In case of non-valid channel: return a large value.
// This is the reciprocal function to 'cellIDCoordinates(uCoord, quarter, ...)'.
// Make sure that the changes in one of these two functions are reflected in
// the conjugate function as well.
//=============================================================================
double DeFTLayer::cellUCoordinate(const LHCb::FTChannelID aChan) const {
  MsgStream msg( msgSvc(), name() );
  double uCoord;

  // check if it is a valid channel or one that corresponds to non-sensitive cell
  if ( aChan.layer() == 15u ) {
    msg << MSG::DEBUG << "Function cellUCoordinate: cannot determine uCoord for "
        << " non-valid channel " << aChan << endmsg;
    uCoord = 99999.;
  }
  else {
    int quarter = aChan.quarter();
    /// First get the SiPM right edge, then add the cell offset 
    double sipmREdge = m_sipmOriginX[quarter] + aChan.sipmId()*m_sipmStepX[quarter];
    // for u < 0 need additional offset of 1 SiPM
    msg << MSG::DEBUG << "quarter, sipmREdge = " << quarter << ", " << sipmREdge << endmsg;
    if ( !(quarter%2) ) sipmREdge += m_sipmStepX[quarter];
    msg << MSG::DEBUG << "sipmREdge after corr = " << sipmREdge << endmsg;

    // Determine gross cellID
    unsigned int grossID = grossCellID(aChan.sipmCell());
    // offset of the cell center wrt SiPM right edge (cellID always increases from right to left)
    double cellOffset = (grossID + 0.5) * m_cellSizeX;
  
    // correction in the case that the SiPM edge is not exactly 1 cell width
    double corr = -m_cellSizeX+m_sipmEdgeSizeX;
    cellOffset += corr;
    
    uCoord = sipmREdge+cellOffset;
  }
  return uCoord;
}

//=============================================================================
// Function to determine the sipmID, gross cellID (counts also insensitive cells
// and SiPM edge gaps) and fractional position inside a cell. Input params are
// the cell u-coordinate and quarterID.
//=============================================================================
void DeFTLayer::cellIDCoordinates( const double  uCoord,
                                   unsigned int  quarter,
                                   unsigned int& sipmID,
                                   unsigned int& cellID,
                                   double&       fracDistCellCenter ) const
{
  MsgStream msg( msgSvc(), name() );

  /// Get sipmID and local position of its right edge
	sipmID = (unsigned int) ((uCoord - m_sipmOriginX[quarter]) / m_sipmStepX[quarter]);
	double sipmREdgeU = m_sipmOriginX[quarter] + (sipmID + !(quarter%2)) * m_sipmStepX[quarter];
  msg << MSG::DEBUG << "quarter, sipmID, sipmREdgeU = "
      << quarter << ", "<< sipmID << ", " << sipmREdgeU << endmsg;
  
  /// Get cellID inside the SiPM
  double distSipmREdge = uCoord - sipmREdgeU;
  if ( distSipmREdge < 0 ) {
    msg << MSG::ERROR << "In function cellIDCoordinates: got negative distance between "
        << " the hit and the sipmEdge. Must be non-negative!)" << endmsg;
  }
  double cellREdgeU;
  
  if ( distSipmREdge < m_sipmEdgeSizeX ) {
    cellID = 0;
    cellREdgeU = sipmREdgeU;
  }
  else if ( distSipmREdge > m_sipmEdgeSizeX + m_sipmSizeX ) {
    cellID = 130;
    cellREdgeU = sipmREdgeU + m_sipmEdgeSizeX + m_sipmSizeX;
  }
  else {
    //sensitive cells have ids: 1-64, 66-129 incl.
    //cellID 65 is the inner SiPM hole!
    double distActiveArea = uCoord - (sipmREdgeU + m_sipmEdgeSizeX);
    if ( distActiveArea < 0 ) {
      msg << MSG::ERROR << "In function cellIDCoordinates: got negative distance between "
          << " the hit and the first sensitive cell. Must be non-negative!)" << endmsg;
    }
    cellID = (unsigned int) (1 + distActiveArea/m_cellSizeX); // >= 1 by construction
    cellREdgeU = (sipmREdgeU + m_sipmEdgeSizeX) + (cellID-1)*m_cellSizeX;
    //use "(cellID-1)" as we added 1 in the preceeding line
  }
  
  /// Determine fractional distance between uCoord and cell center
  fracDistCellCenter = (uCoord - (cellREdgeU + m_cellSizeX/2)) / m_cellSizeX;

  msg << MSG::DEBUG << "\n\tdistSipmREdge: " << distSipmREdge 
      << "\n\tGross cellID: " << cellID
      << "\n\tcellREdgeU: " << cellREdgeU
      << ", distToCellREdge = " << uCoord - cellREdgeU
      << ", distToCellCenter = " << uCoord - (cellREdgeU + m_cellSizeX/2)
      << "\n\tfracDistCellCenter: " << fracDistCellCenter << endmsg;
}

//=============================================================================
// Function to determine the XYZ crossing point of a line (determined by the
// points gpEntry and gpExit) and the vertical (or tilted in the case of u/v layers)
// plane between two cells.
//=============================================================================
StatusCode DeFTLayer::cellCrossingPoint(const double cellEdgeU,
                                        const Gaudi::XYZPoint& gpEntry,
                                        const Gaudi::XYZPoint& gpExit,
                                        Gaudi::XYZPoint& pIntersect) const
{
  MsgStream msg( msgSvc(), name() );

  /// Create the plane of the border between two cells
  /// set small, but signed yInit for the calculation of the cell x at the top/bottom of the layer
  double yInit = (gpEntry.y()>0) ? 1.e-6 : -1.e-6;
  double cellYAtVertBorder = (gpEntry.y()>0) ? m_layerMaxY : m_layerMinY;
  double cellXAtVertBorder = xAtVerticalBorder( cellEdgeU, yInit );

  double cellZMin = gpEntry.z();
  Gaudi::XYZPoint gp1(cellEdgeU, 0., cellZMin);
  Gaudi::XYZPoint gp2(cellEdgeU, 0., cellZMin+2*m_layerHalfSizeZ);
  Gaudi::XYZPoint gp3(cellXAtVertBorder, cellYAtVertBorder, cellZMin);
  msg << MSG::DEBUG << "P1, P2, P3:\n\t" << gp1 << "\n\t"
      << gp2 << "\n\t" << gp3 << endmsg;
  
  /// Create the plane, the particle trajectory and their intersection
  Gaudi::Plane3D cellBorderPlane(gp1, gp2, gp3);
  msg << MSG::DEBUG << "CELL BORDER PLANE:" << cellBorderPlane << endmsg;

  Line partTraj(gpEntry, (gpExit - gpEntry).Unit());
  double paramIntersect;
  msg << MSG::DEBUG << "PART TRAJ:" << partTraj << endmsg;

  bool sc = Gaudi::Math::intersection( partTraj, cellBorderPlane, pIntersect, paramIntersect );

  msg << MSG::DEBUG << "Intersection P, param, line(param):"
      << sc << " : " << pIntersect << ", " << paramIntersect << endmsg;
  
  msg << MSG::DEBUG << "Position(p=0), Position(p=xx): " << partTraj.position(0.)
      << partTraj(paramIntersect) << endmsg;

  return StatusCode(sc);
}

//=============================================================================
// Function to create a DetectorSegment (straight line representing an FT channel)
// from a XYZPoint.
// -->FIXME: The function assumes that the local -> global transformation of the
// layer is a simple z translation!
//=============================================================================
DetectorSegment DeFTLayer::createDetSegment(const Gaudi::XYZPoint& globalPoint) const {

  MsgStream msg( msgSvc(), name() );
  if ( ! this->isInside(globalPoint) ) {
    msg << MSG::DEBUG << "Global point " << globalPoint << " is not inside the Layer!" << endmsg;
    return DetectorSegment();
  }
  else {
    Gaudi::XYZPoint localPoint = this->geometry()->toLocal( globalPoint );
  
    double ds_x0   = xAtYEq0(localPoint.x(), localPoint.y());
    double ds_z0   = globalPoint.z();
    double ds_dxDy = m_tanAngle;
    double ds_dzDy = 0.; ///!!! For now assume dzDy = 0
    double ds_yMin, ds_yMax;

    if ( localPoint.y() > 0 ) {
      ds_yMax = m_layerMaxY;
      /// if we are above the beam pipe yMin is not 0!
      if ( std::abs(ds_x0) < m_innerHoleRadius ) {
        StatusCode sc = beamPipeYCoord(ds_x0, 1, ds_yMin);
        if ( sc.isFailure() ) {
          msg << MSG::ERROR << "Function beamPipeYCoord found no crossing points, while"
              << " there must be at least one crossing point (|x0| is < beam-pipe radus)." << endmsg;
        }
      }
      else ds_yMin = 0.;
    } // end if (y>0)

    else{
      ds_yMin = m_layerMinY;
      /// if we are below the beam pipe yMax is not 0!
      if ( std::abs(ds_x0) < m_innerHoleRadius ) {
        StatusCode sc = beamPipeYCoord(ds_x0, -1, ds_yMax);
        if ( sc.isFailure() ) {
          msg << MSG::ERROR << "Function beamPipeYCoord found no crossing points, while"
              << " there must be at least one crossing point (|x0| is < beam-pipe radus)." << endmsg;
        }
      }
      else ds_yMax = 0;
    } // end y<=0

    msg << MSG::DEBUG << "Initial point: x, y = " << localPoint.x() << ", " << localPoint.y() << endmsg;
    msg << MSG::DEBUG << "Detector Segment: x0, z0, dxdy, ymin, ymax: "
        << ds_x0 << ", " << ds_z0 << ", " << ds_dxDy << ", " << ds_yMin << ", " << ds_yMax << endmsg;

    return DetectorSegment( ds_x0, ds_z0, ds_dxDy, ds_dzDy, ds_yMin, ds_yMax );
  }
}

//=============================================================================
// Function to create a DetectorSegment (straight line representing an FT channel)
// from a FTChannelID
//=============================================================================
DetectorSegment DeFTLayer::createDetSegment(const LHCb::FTChannelID& aChan,
                                            double fracPos) const {

  /// The procedure is to create an XYZ point with u-coordinate provided by the
  /// function 'cellUCoordinate' (gives the cell center) and then take into
  /// account the farctional position inside the cell.
  /// Then call the function which creates a detector segment from a XYZ point.

  double cellCenter = cellUCoordinate(aChan);
  double segmentU = cellCenter + fracPos*m_cellSizeX;

  // Create a point (along the segment trajectory) which is _inside_ the layer
  // (if it is not _inside_, the call to createDetSegment below won't work).
  // Whatever point we pass to createDetSegment it will determine x@y=0 properly
  double y = (aChan.quarter()>1) ? 1.e3 : -1.e3;
  double x = segmentU + y*m_tanAngle;
  double z = m_layerPosZ;
  return createDetSegment( Gaudi::XYZPoint(x, y, z) );
}

//=============================================================================
// Function to determine the y coordinate of the crossing point between
// the beam-pipe hole (circle) and the fibres. The procedure is to express
// the fibre trajectory as x = x0 + m_tanAngle * y and solve the circle
// equation x**2 + y**2 = R**2 to determine the y coords of the crossing points.
// For y>0 (y<0) we are interested only in the positive (negative) solution.
// x0 is the x-coord@y=0 (i.e. the u-coordinate). The returned status code
// signifies if there is at least one crossing point.
//=============================================================================
StatusCode DeFTLayer::beamPipeYCoord(const double x0, const int ySign, double& yIntersect) const {

  MsgStream msg( msgSvc(), name() );
  StatusCode sc;

  /// Solve the quadratic equation
  double a = 1 + pow(m_tanAngle,2);
  double b = 2 * x0 * m_tanAngle;
  double c = pow(x0,2) - pow(m_innerHoleRadius,2);
  double D  = pow(b,2) - 4*a*c;
  if ( D < 0 ) {
    /// No real solutions ==> no crossing points
    msg << MSG::DEBUG << "In function beamPipeYCoord: no crossing points found" << endmsg;
    sc = StatusCode::FAILURE;
  }
  else {
    /// We have 2 solutions (can be degenerate)
    /// For ySign=1 (ySign=-1) will return the positive (negative) solution
    yIntersect = (-b + ySign*sqrt(D)) / (2*a);
    sc = StatusCode::SUCCESS;
    msg << MSG::DEBUG << "y-coordinate of beam-pipe crossing point: " << yIntersect << endmsg;
  }
  return sc;
}

//=============================================================================
// Get the FTChannelID of the cell located on the left of a given cell (left/right assume
// an observer sitting at (0,0,0) and looking downstream (+z)). The cell
// numbering convention is that the cellID always increases with x, i.e. to the left.
//=============================================================================
LHCb::FTChannelID DeFTLayer::nextChannelLeft(const LHCb::FTChannelID& aChan) const {
  MsgStream msg( msgSvc(), name() );
  unsigned int grossCellID = this->grossCellID(aChan.sipmCell());
  unsigned int grossIDLeftCell = grossCellID + 1u;
  unsigned int netIDLeftCell = this->netCellID(grossIDLeftCell);
  LHCb::FTChannelID aChanLeft;
  if ( netIDLeftCell > (m_sipmNChannels-1) ) {
    msg << MSG::DEBUG << "Gross cellID " << grossIDLeftCell << " corresponds to insensitive cell."
        << " Creating invalid FT channel (the signature is: layerID=15)." << endmsg;
    aChanLeft = FTChannelID( 15, 0, 0, 0 );
  }
  else {
    aChanLeft = FTChannelID( aChan.layer(), aChan.quarter(), aChan.sipmId(), netIDLeftCell );
  }
  return aChanLeft;
}

//=============================================================================
// Get the FTChannelID of the cell located on the right of a given cell (left/right assume
// an observer sitting at (0,0,0) and looking downstream (+z)). The cell
// numbering convention is that the cellID always increases with x, i.e. to the left.
//=============================================================================
LHCb::FTChannelID DeFTLayer::nextChannelRight(const LHCb::FTChannelID& aChan) const {
  MsgStream msg( msgSvc(), name() );
  unsigned int grossCellID = this->grossCellID(aChan.sipmCell());
  unsigned int grossIDRightCell = grossCellID - 1u;
  unsigned int netIDRightCell = this->netCellID(grossIDRightCell);
  LHCb::FTChannelID aChanRight;
  if ( netIDRightCell > (m_sipmNChannels-1) ) {
    msg << MSG::DEBUG << "Gross cellID " << grossIDRightCell << " corresponds to insensitive cell."
        << " Creating invalid FT channel (the signature is: layerID=15)." << endmsg;
    aChanRight = FTChannelID( 15, 0, 0, 0 );
  }
  else {
    aChanRight = FTChannelID( aChan.layer(), aChan.quarter(), aChan.sipmId(), netIDRightCell );
  }
  return aChanRight;
}
