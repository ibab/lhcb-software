// $Id: $
#include <boost/lexical_cast.hpp>

// DetDesc
#include "DetDesc/SolidSubtraction.h"
#include "DetDesc/SolidChild.h"
#include "DetDesc/SolidBox.h"
#include "DetDesc/SolidCons.h"

// Smartrefs
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ISvcLocator.h"

// Gaudi/LHCb Math
#include "GaudiKernel/Plane3DTypes.h"
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "LHCbMath/Line.h"
#include "LHCbMath/GeomFun.h"
#include "LHCbMath/LineTypes.h"

// FTDet
#include "FTDet/DeFTFibreMat.h"


/** @file DeFTFibreMat.cpp
 *
 *  Implementation of class : DeFTFibreMat
 *
 *  @author Plamen Hopchev
 *  @author Eric Cogneras
 *  @author Diego Milanes
 *  @date   2013-07-12
 */

using namespace LHCb;

typedef Gaudi::Math::Line<Gaudi::XYZPoint,Gaudi::XYZVector> Line;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DeFTFibreMat::DeFTFibreMat( const std::string& name ) :
  DetectorElement ( name ),
  m_FibreMatID(0),
  m_layerID(0),
  // Layer stereo angle
  m_angle(0.0),
  m_tanAngle(0.0),
  m_dzDy(0.0),
  // fibremat identifiers 
  m_bottom( false ),
  m_module( 0 ),
  m_layer( 0 ),
  m_holey( false ),
  // Layer dimensions
  m_layerMinX(0.),
  m_layerMaxX(0.),
  m_layerMinY(0.),
  m_layerMaxY(0.),
  m_layerMinZ(0.),
  m_layerMaxZ(0.),
  m_layerHalfSizeX(0.),
  m_layerHalfSizeY(0.),
  m_layerHalfSizeZ(0.),
  m_innerHoleRadius(0.),
  m_layerPosZ(0.),
  // fibremat dimension
  m_fibreMatMinX(0.),
  m_fibreMatMaxX(0.),
  m_fibreMatMinY(0.),
  m_fibreMatMaxY(0.),
  m_fibreMatMinZ(0.),
  m_fibreMatMaxZ(0.),
  m_fibreMatHalfSizeX(0.),
  m_fibreMatHalfSizeY(0.),
  m_fibreMatHalfSizeZ(0.),
  // SiPM and cell sizes
  m_sipmNChannels(128),
  m_cellSizeX(0.25),
  m_sipmSizeX(32.25),
  // Gaps
  m_sipmEdgeSizeX(0.25), /// in the sipm layout it is 0.17 (we add a bit)
  m_moduleEdgeSizeX(2.0), ///
  m_moduleGapH(2.0), ///
  m_moduleGapV(2.0), ///
  m_gapXLayerHalves(0.0),
  // Derived params
  m_sipmPitchX(0.),
  m_nSipmPerQuarter(0),
  m_gapXLayerOuterEdge(0.0),
//
  m_nSipmPerModule(0),
  m_SipmGapInModule(0),
  m_msg(NULL)
{
  
}

//=============================================================================
// Destructor
//=============================================================================
DeFTFibreMat::~DeFTFibreMat(){} 

//=============================================================================
// classID function
//=============================================================================
const CLID& DeFTFibreMat::clID () const {
  return DeFTFibreMat::classID();
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode DeFTFibreMat::initialize(){

  StatusCode sc = DetectorElement::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;

  /// Create a MsgStream object with an add-hoc name (the second argument),
  /// instead of the ususual name(), which gives a too long string
  if ( 0 == m_msg ) m_msg = new MsgStream( msgSvc(), "DeFTFibreMat" );

  debug() << "==> Initialize DeFTFibreMat" << endmsg;
  
  /// Set the layer ID and stereo angle: take them from the xml DDDB
  m_angle = this->params()->param<double>("stereoAngle");
  m_tanAngle = tan(m_angle);
  m_FibreMatID = this->params()->param<int>("FibreMatID");

  // Given the AABBCC convention for the FibreMatID, 
  // AA layer number T1( 00 01 02 03 ) T2( 04 05 06 07 ) T3 ( 08 09 10 11 )
  // BB module number 00 01 02 03 04 10(LeftH) 11(RightH) 05 06 07 08 09
  // CC 00(Top) 01(Bottom) 
  m_bottom = m_FibreMatID%10;
  m_module = int( ( m_FibreMatID %10000 ) / 100 );
  m_layer  = int( m_FibreMatID / 10000 ); 
  m_layerID = m_layer;
  m_holey = ( m_module == 10 || m_module == 11 );

  /// Get geometrical limits of the layer
  /// Note that the following approach is not robust in the sense that every
  /// time a 'structural' modification is made to the layer geometry (xml DDDB)
  /// we need to revisit the way we obtain the detector element geometrical limits

  if( !m_holey ){
    const SolidBox* outerBox = dynamic_cast<const SolidBox*>( this->geometry()->lvolume()->solid() );
    if ( 0 == outerBox ) {
      fatal() << "Can't acquire layer geometry (SolidBox)" << endmsg;
      return StatusCode::FAILURE;
    }
    m_fibreMatHalfSizeX = outerBox->xHalfLength();
    m_fibreMatHalfSizeY = outerBox->yHalfLength();
    m_fibreMatHalfSizeZ = outerBox->zHalfLength();
    m_innerHoleRadius = 0.0;
  }
  else{
    const SolidSubtraction* subtrObject = dynamic_cast<const SolidSubtraction*>( this->geometry()->lvolume()->solid() );
    if ( 0 == subtrObject ) {
      fatal() << "Can't acquire layer geometry (SolidSubtraction)" << endmsg;
      return StatusCode::FAILURE;
    }
    const SolidBox* outerBox = dynamic_cast<const SolidBox*>( subtrObject->coverTop() );
    if ( 0 == outerBox ) {
      fatal() << "Can't acquire layer geometry (SolidBox)" << endmsg;
      return StatusCode::FAILURE;
    }
    const SolidChild* tmpChild = dynamic_cast<const SolidChild*>( (*subtrObject)[0] );
    if ( 0 == tmpChild ) {
      fatal() << "Can't acquire layer geometry (SolidChild)" << endmsg;
      return StatusCode::FAILURE;
    }
    const SolidCons* innerCons = dynamic_cast<const SolidCons*>( tmpChild->solid() );
    if ( 0 == innerCons ) {
      fatal() << "Can't acquire layer geometry (SolidCons)" << endmsg;
      return StatusCode::FAILURE;
    }
    m_fibreMatHalfSizeX = outerBox->xHalfLength();
    m_fibreMatHalfSizeY = outerBox->yHalfLength();
    m_fibreMatHalfSizeZ = outerBox->zHalfLength();
    m_innerHoleRadius = innerCons->outerRadiusAtPlusZ();
  }
  

  Gaudi::XYZPoint fibreMatCenter = this->geometry()->toGlobal( Gaudi::XYZPoint(0.,0.,0.) );

  m_fibreMatMinX = fibreMatCenter.x() - m_fibreMatHalfSizeX; 
  m_fibreMatMaxX = fibreMatCenter.x() + m_fibreMatHalfSizeX; 
  m_fibreMatMinY = fibreMatCenter.y() - m_fibreMatHalfSizeY; 
  m_fibreMatMaxY = fibreMatCenter.y() + m_fibreMatHalfSizeY; 
  m_fibreMatMinZ = fibreMatCenter.z() - m_fibreMatHalfSizeZ; 
  m_fibreMatMaxZ = fibreMatCenter.z() + m_fibreMatHalfSizeZ; 
  m_layerPosZ = fibreMatCenter.z();

  
    

  double CarHoneyKapWidth = 0.15 + 20. + 0.05;

  m_sipmPitchX    = m_sipmSizeX + 2*m_sipmEdgeSizeX;
  m_layerHalfSizeX =  12.*( m_fibreMatHalfSizeX + m_moduleEdgeSizeX );
  m_nSipmPerQuarter = int(m_layerHalfSizeX/m_sipmPitchX);
  m_gapXLayerOuterEdge = m_layerHalfSizeX - m_nSipmPerQuarter*m_sipmPitchX;
  m_layerMinX = -m_layerHalfSizeX - m_gapXLayerOuterEdge;
  m_layerMaxX = -m_layerMinX;

  m_layerMinY = -2.*m_fibreMatHalfSizeY - 0.5*m_moduleGapH;
  m_layerMaxY = -m_layerMinY;

  m_layerMinZ = m_fibreMatMinZ - CarHoneyKapWidth;
  m_layerMaxZ = m_fibreMatMaxZ + CarHoneyKapWidth;

  /// Determine the slope in the y-z plane
  Gaudi::XYZPoint tmpLocPoint1(0.,    0., 0.);
  Gaudi::XYZPoint tmpLocPoint2(0., 1000., 0.);
  Gaudi::XYZPoint tmpGlobPoint1 = this->geometry()->toGlobal( tmpLocPoint1 );
  Gaudi::XYZPoint tmpGlobPoint2 = this->geometry()->toGlobal( tmpLocPoint2 );
  m_dzDy = (tmpGlobPoint2.z() - tmpGlobPoint1.z()) / (tmpGlobPoint2.y() - tmpGlobPoint1.y());

  debug() << "mdzdy = " << m_dzDy << endmsg; 

  /*
  debug() << "Derived parameters:"
          << "\n\tpitch X: " << m_sipmPitchX
          << "\n\tN of SiPM per quarter: " << m_nSipmPerQuarter
          << "\n\tOuter edge width X: " << m_gapXLayerOuterEdge
          << "\n\tLayer min X: " << m_layerMinX
          << "\n\tLayer max X: " << m_layerMaxX 
          << "\n\tLayer min Y: " << m_layerMinY
          << "\n\tLayer max Y: " << m_layerMaxY 
          << "\n\tLayer min Z: " << m_layerMinZ
          << "\n\tLayer max Z: " << m_layerMaxZ
          << endmsg;
  */
  ///fill in the vectors holding the SiPM x origin and step in each quarter
  double xOrigin, xStep;
  unsigned int iQ;
  for (iQ=0; iQ<4; ++iQ) {
    xOrigin = 0.;
    xStep = (iQ%2) ? m_sipmPitchX : -m_sipmPitchX;
    m_sipmOriginX.push_back(xOrigin);
    m_sipmStepX.push_back(xStep);
    //debug() << "iQ = " << iQ << ", SiPM Origin = " << m_sipmOriginX[iQ]
    //        << ", Step = " << m_sipmStepX[iQ] << endmsg;
  }

  //On getting Sipm connected only to sensitive areas
  m_nSipmPerModule = int(  2.*m_fibreMatHalfSizeX/m_sipmPitchX );
  m_SipmGapInModule = 0.5*(-m_nSipmPerModule + 2.*m_fibreMatHalfSizeX/m_sipmPitchX);
  double fullModuleGapX =  m_SipmGapInModule + m_moduleGapV;
  debug()<<fullModuleGapX<<endmsg;
  
    return StatusCode::SUCCESS;
}

//=============================================================================
// Finalization
//=============================================================================
StatusCode DeFTFibreMat::finalize(){
  // destroy the MsgService object
  if ( m_msg ) {
    delete m_msg;
    m_msg = 0;
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// Function to determine which FT channels are traversed by a trajectory
// determined by globalPointEntry and globalPointExit. Fills a vector of
// pairs <FTChannelID, energyFraction>. The latter is the fraction of the energy
// of the MC particle deposited in the corresponding SiPM cell. A light sharing
// model is used in the determination of the energyFractions.
//=============================================================================
StatusCode DeFTFibreMat::calculateHits(const LHCb::MCHit*  fthit,
                                    VectFTPairs&         vectChanAndEnergy) const
{


  /*
  //local coord option 1, depending on u, v angles it might generate troubles
  double entryX = fthit->entry().x();
  double entryY = fthit->entry().y();
  double exitX = fthit->exit().x();
  double exitY = fthit->exit().y();
  Gaudi::XYZPoint enP( cos(-m_angle)*entryX+sin(-m_angle)*entryY, -sin(-m_angle)*entryX+cos(-m_angle)*entryY, fthit->entry().z() );
  Gaudi::XYZPoint exP( cos(-m_angle)*exitX+sin(-m_angle)*exitY  , -sin(-m_angle)*exitX+cos(-m_angle)*exitY  , fthit->exit().z() );
  */

  
  //local coord option 2 -- stable
  std::string station = 
    ( m_layer >= 0 && m_layer <= 3  )? "T1" : 
    ( m_layer >= 4 && m_layer <= 7  )? "T2" : 
    ( m_layer >= 8 && m_layer <= 11 )? "T3" : ""; 
  std::string loc = "/dd/Structure/LHCb/AfterMagnetRegion/T/FT/" + station + "/MonoLayer" + boost::lexical_cast<std::string>( m_layer );
  SmartDataPtr<DetectorElement> layer ( dataSvc(), loc.c_str() );
  Gaudi::XYZPoint enP = layer -> geometry() -> toLocal( fthit -> entry() );
  Gaudi::XYZPoint exP = layer -> geometry() -> toLocal( fthit -> exit()  );
  
  //returns local hit points wrt fibremat
  //Gaudi::XYZPoint enP = this->geometry()->toLocal(fthit->entry());
  //Gaudi::XYZPoint exP = this->geometry()->toLocal(fthit->exit());
  
  debug() << "Entry Point in Global / Local: " << fthit->entry() << enP << endmsg;
  debug() << "Exit  Point in Global / Local: " << fthit->exit() << exP << endmsg;

  unsigned int hitLayer = this->layer();
  debug() << "LayerID = " << hitLayer
          << ", Stereo angle = " << this->angle() << endmsg;

  /// Make a check if the entry and exit points are in the same z plane
  if ( std::abs(fthit->entry().z() - fthit->exit().z()) < 1.e-2 ) {
    debug() << "Aborting calculateHits(...) because z-distance "
            << "between entry and exit points is less than 10 micron." << endmsg;
    return StatusCode::FAILURE;
  }
  
  /// Make a check if both the entry and exit points are inside the active area.
  /// This will be done by determining the SiPM ID corresponding to the point
  /// and comparing with nSiPMPerQuarter.

  /// Check if the entry or exit points are inside the beam hole: assume a circle
  if( m_holey ){
    if ( (std::pow(enP.x(),2) + std::pow(enP.y(),2)) < std::pow(m_innerHoleRadius,2) || 
	 (std::pow(exP.x(),2) + std::pow(exP.y(),2)) < std::pow(m_innerHoleRadius,2) ) {
      debug() << "Aborting calculateHits(...) because entry or exit points are inside "
	      << "the beam pipe hole (circle)" << endmsg;
      return StatusCode::FAILURE;
    }
  }
  /// Create a vector of FT pairs which will hold <FTChannel, FractionalPosition>
  /// This is the 'working' quantity. In the final step of the calculateHits method
  /// we use it to create the final vectChanAndEnergy
  VectFTPairs vectChanAndFracPos;
  
  ///////////////////////////////////////////////////////
  /// Get cell coordinates of the entry and exit points
  ///////////////////////////////////////////////////////

  /// extrapolate points along the fibre to y=0
  double enPU = xAtYEq0( enP.x(), enP.y());
  double exPU = xAtYEq0( exP.x(), exP.y());
  debug() << "U Coordinates of enP and exP: " << enPU << ", " << exPU << endmsg;

  int enPQuarter=0, exPQuarter=0;
	if ( enPU    > 0 ) enPQuarter += 1;
	if ( enP.y() > 0 ) enPQuarter += 2;   /// The separation top-bottom is at y=0, not perpendicular to the fibre.
	if ( exPU    > 0 ) exPQuarter += 1;
	if ( exP.y() > 0 ) exPQuarter += 2;

  if ( enPQuarter != exPQuarter ) {
    debug() << "Aborting calculateHits(...) because entry "
            << "and exit points are in different quarters!" << endmsg;
    return StatusCode::FAILURE;
  }

  int quarter = enPQuarter;
  unsigned int enPSipmID, enPCellID;
  unsigned int exPSipmID, exPCellID;
  double enPFraction, exPFraction;
  FTChannelID channel;

  cellIDCoordinates( enPU, enPQuarter, enPSipmID, enPCellID, enPFraction );
  cellIDCoordinates( exPU, exPQuarter, exPSipmID, exPCellID, exPFraction );

  if ( (enPSipmID > m_nSipmPerQuarter) || (exPSipmID > m_nSipmPerQuarter) ) {
    debug() << "Aborting calculateHits(...) because entry "
            << "or exit points are outside acceptance (we get too large sipmID)" << endmsg;
    return StatusCode::FAILURE;
  }

  ///////////////////////////////////////////////////////
  /// Create pairs of FT channels and fractional position
  ///////////////////////////////////////////////////////

  /// Case where entry and exit points are in the same cell
  if ( (enPSipmID==exPSipmID) && (enPCellID==exPCellID) ) {
    debug() << "Entry and Exit points are in the same cell!" << endmsg;
    
    double frac = (enPFraction + exPFraction)/2;
    debug() << "Average fract dist to cell center: " << frac << endmsg;

    /// Create and push-back the pair (FTChannelID, fraction)
    channel = createChannel( hitLayer, quarter, enPSipmID, enPCellID );
    vectChanAndFracPos.push_back( std::make_pair(channel, frac) );

  } //end single cell

  /// Case where entry and exit points are in different cells
  else {
    debug() << "Entry and Exit points are in different cells!" << endmsg;
    /// The procedure is to determine the crossing point between the particle
    /// trajectory and the planes separating the adjacent cells. First we determine
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

    debug() << "Entry point: uDir / cellSeparEdge / distToSeparEdge: "
            << uDir << ", " << enPCellSeparEdge << ", " << enPDistToSeparEdge << endmsg;

    /// Cell edge next to the exit point
    double exPCellSeparEdge, exPDistToSeparEdge;
    exPCellSeparEdge = exPU - (exPFraction + 0.5*uDir) * m_cellSizeX;
    exPDistToSeparEdge = exPU - exPCellSeparEdge;

    debug() << "Exit point: uDir / cellSeparEdge / distToSeparEdge: "
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
    sc = cellCrossingPoint( enPCellSeparEdge, fthit->entry(), fthit->exit(), pIntersect );
    // in case of a problem --> directly exit the function
    if ( sc.isFailure() ) {
      debug() << "Aborting calculateHits(...), because of unsuccessful "
              << "call to cellCrossingPoint(EntryPoint)" << endmsg;
      return sc;
    }
    
    fracP1 = enPFraction;
    fracP2 = (dUExEn>0) ? 0.5 : -0.5;
    fracPosFirstCell = (fracP1 + fracP2)/2;
    debug() << "Entry Point, SC = " << sc << " Intersection point: " << pIntersect << endmsg;
    debug() << "Entry Point, Frac = " << fracPosFirstCell << endmsg;

    /// Create and push-back the pair (FTChannelID, fraction)
    channel = createChannel( hitLayer, quarter, enPSipmID, enPCellID );
    vectChanAndFracPos.push_back( std::make_pair(channel, fracPosFirstCell) );
    
    /// The middle cells
    double eps = 1.e-3; /// add this distance when determining the number of middle cells
    int nMiddleCells( (std::abs(exPCellSeparEdge - enPCellSeparEdge) + eps) / m_cellSizeX );
    unsigned int midCellSipmID, midCellID;
    double midCellFraction;
    int i;
    for (i=0; i<nMiddleCells; ++i) {
      double midCellCenterU = enPCellSeparEdge + uDir*m_cellSizeX*(i+0.5);
      debug() << "\tMid Cell " << i << " midCellCenterU = " << midCellCenterU << endmsg;
      cellIDCoordinates( midCellCenterU, quarter, midCellSipmID, midCellID, midCellFraction );

      /// Create and push-back the pair (FTChannelID, fraction)
      channel = createChannel( hitLayer, quarter, midCellSipmID, midCellID );
      vectChanAndFracPos.push_back( std::make_pair(channel, midCellFraction) );
    }// end loop over mid cells
    
    /// The cell of the exit point

    double fracPosLastCell = 999.;
    sc = cellCrossingPoint( exPCellSeparEdge, fthit->entry() , fthit->exit(), pIntersect );
    // in case of a problem --> directly exit the function
    if ( sc.isFailure() ) {
      debug() << "Aborting calculateHits(...), because of unsuccessful "
              << "call to cellCrossingPoint(ExitPoint)" << endmsg;
      return sc;
    }

    fracP1 = exPFraction;
    fracP2 = (dUExEn>0) ? -0.5 : 0.5;
    fracPosLastCell = (fracP1 + fracP2)/2;
    debug() << "Exit Point, SC = " << sc << " Intersection point: " << pIntersect << endmsg;
    debug() << "Exit Point, Frac = " << fracPosLastCell << endmsg;

    /// Create and push-back the pair (FTChannelID, fraction)
    channel = createChannel( hitLayer, quarter, exPSipmID, exPCellID );
    vectChanAndFracPos.push_back( std::make_pair(channel, fracPosLastCell) );
  }//end more than 1 hit cells
  
  debug() << "Finished creating FTPairs\n" << endmsg;
  
  /// Printout the vector of FT pairs
  debug() << "Size of vector of FT pairs: " << vectChanAndFracPos.size() << endmsg;
  VectFTPairs::const_iterator itPair;
  DetectorSegment tmpDetSeg;
  for (itPair = vectChanAndFracPos.begin(); itPair != vectChanAndFracPos.end(); ++itPair) {
    debug() << itPair->first << ", FractPos: " << itPair->second << endmsg;
    /// Test the inverse function ///
    debug() << "Test of function cellUCoordinate(FTChannelID): "
            << cellUCoordinate(itPair->first) << endmsg;
    /// Test of functions nextChannelLeft/Right
    debug() << "Test of function nextChannelLeft:"
            << this->nextChannelLeft( itPair->first ) << endmsg;
    debug() << "Test of function nextChannelRight:"
            << this->nextChannelRight( itPair->first ) << endmsg;
    /// Test the function FTChannel+Fraction --> DetSegment
    debug() << "Test of function createDetSegment(FTChannelID, fracPos)" << endmsg;
    tmpDetSeg = createDetSegment( itPair->first, itPair->second );
  }

  /// Call the light-sharing method using vectChanAndFracPos,
  /// create the final vector of pairs <FTChannel, EnergyFractions>
  vectChanAndEnergy = this->createLightSharingChannels( vectChanAndFracPos );

  // Finally, to return the energy deposited in each channel by the hit, each energy fraction
  // is multiplied by the energy
  for (VectFTPairs::iterator itPair = vectChanAndEnergy.begin(); itPair != vectChanAndEnergy.end(); ++itPair) {
    itPair->second *= fthit->energy();
  }

  return StatusCode::SUCCESS;
}


//=============================================================================
// This function determines the fibre lengh and relative position of the hit 
// in the fibre according to the Hit position.
// These returned value can be used to determine the attenuation coefficient to be applied
// on the deposited energy to take into acocunt the light attenuation 
// through the fibre.
//=============================================================================
StatusCode DeFTFibreMat::hitPositionInFibre(const LHCb::MCHit*  fthit,
                                         double& meanfibrefullLengh,
                                         double& fibreLenghFrac) const
{
  Gaudi::XYZPoint enP = this->geometry()->toLocal( fthit->entry() );
  Gaudi::XYZPoint exP = this->geometry()->toLocal( fthit->exit() );
  meanfibrefullLengh = FibreLengh(enP,exP);
  fibreLenghFrac= (enP.y()+exP.y())/2;
  // Fractional lengh is relative to the SiPM position which is on the top
  // for y>0 & on the bottom for y<0
  if(fibreLenghFrac > 0) 
    fibreLenghFrac = (m_fibreMatMaxY - fibreLenghFrac)/(cos(m_angle)*meanfibrefullLengh);
  else
    fibreLenghFrac = (fibreLenghFrac - m_fibreMatMinY)/(cos(m_angle)*meanfibrefullLengh);

  return StatusCode::SUCCESS;
}


//=============================================================================
// Function dealing with the light sharing between neighbouring SiPM cells
//=============================================================================
VectFTPairs DeFTFibreMat::createLightSharingChannels(VectFTPairs& inputVectPairs) const
{
  VectFTPairs::const_iterator itPair;
  // First determine the weighting of the energy deposits by taking into account
  // the particle path length in the cells
  double sumPathWeights = 0;
  for (itPair = inputVectPairs.begin(); itPair != inputVectPairs.end(); ++itPair) {
    sumPathWeights += (1 - 2.*std::abs(itPair->second));
  }
  std::vector<double> vectPathWeights;
  for (itPair = inputVectPairs.begin(); itPair != inputVectPairs.end(); ++itPair) {
    vectPathWeights.push_back( (1 - 2.*std::abs(itPair->second)) / sumPathWeights );
  }
  debug() << "Path weights: " << vectPathWeights << endmsg;

  std::vector<LHCb::FTChannelID> vectChannels;
  // append the channel before the first one with a direct deposit from the MC particle
  // the sequence of channels in the vector can be both right -> left or the opposite
  int sequenceRightToLeft = ( inputVectPairs.front().first.sipmCell() < inputVectPairs.back().first.sipmCell() );

  if ( inputVectPairs.size() > 1 && sequenceRightToLeft ) {
    vectChannels.push_back( this->nextChannelRight(inputVectPairs.front().first) );
  }
  else { vectChannels.push_back( this->nextChannelLeft(inputVectPairs.front().first) ); }

  std::vector<double> lcrFractions; // vector holding the left/center/right fractions
  std::vector< std::vector<double> > vectTriplets; // vector of the lcrFractions

  for (itPair = inputVectPairs.begin(); itPair != inputVectPairs.end(); ++itPair) {
    // append the FT channel
    vectChannels.push_back( itPair->first );
    // calculate the left/center/right sharing of each channel
    this->lightSharing(itPair->second, lcrFractions);
    debug() << "FractPos: " << itPair->second << " ; Sharing: " << lcrFractions << endmsg;
    vectTriplets.push_back( lcrFractions );
  }

  // append the channel after the last one with a direct deposit from the MC particle
  if ( inputVectPairs.size() > 1 && sequenceRightToLeft ) {
    vectChannels.push_back( this->nextChannelLeft(inputVectPairs.back().first) );
  }
  else { vectChannels.push_back( this->nextChannelRight(inputVectPairs.back().first) ); }
  
  // calculate the overall fractions for each channel
  std::vector<double> finalFractions( inputVectPairs.size()+2, 0. );

  unsigned int ind;
  for (ind=0; ind<vectTriplets.size(); ++ind) {
    // here we also apply the particle path-length weighting
    if ( inputVectPairs.size() > 1 && sequenceRightToLeft ) {
      finalFractions.at(ind)   += vectTriplets[ind][2] * vectPathWeights[ind];
      finalFractions.at(ind+1) += vectTriplets[ind][1] * vectPathWeights[ind];
      finalFractions.at(ind+2) += vectTriplets[ind][0] * vectPathWeights[ind];
    }
    else {
      finalFractions.at(ind)   += vectTriplets[ind][0] * vectPathWeights[ind];
      finalFractions.at(ind+1) += vectTriplets[ind][1] * vectPathWeights[ind];
      finalFractions.at(ind+2) += vectTriplets[ind][2] * vectPathWeights[ind];
    }
  }
  debug() << "Final Fractions: " << finalFractions << endmsg;
  debug() << "Final Channels: "  << vectChannels << endmsg;

  debug() << "Size of vector of triplets / channels / finalFracts: " << vectTriplets.size()
          << " / " << vectChannels.size() << " / " << finalFractions.size() << endmsg;

  // Create the final vector of FTPairs
  // The second part of the pair is the MCP energy fraction for this channel

  VectFTPairs outputVectPairs;
  for (ind=0; ind<vectChannels.size(); ++ind) {
    outputVectPairs.push_back( std::make_pair(vectChannels.at(ind), finalFractions.at(ind)) ); 
  }
  debug() << "Created vector of light-sharing FT pairs with size: " << outputVectPairs.size() << endmsg;

  return outputVectPairs;
}

//=============================================================================
// A linear light-sharing model. From the fractional position inside a cell
// determine the energy fractions deposited in the left/central/right SiPM cells
//=============================================================================
void DeFTFibreMat::lightSharing( double position, std::vector<double>& fractions ) const {
  fractions.clear();
  double frLeft, frCenter, frRight;
  if (position > 0) {
    frLeft  =  0.68 * position + 0.16;
    frRight = -0.32 * position + 0.16;
  }
  else {
    frLeft  =  0.32 * position + 0.16;
    frRight = -0.68 * position + 0.16;
  }
  frCenter = -0.36 * std::abs(position) + 0.68;
  fractions.push_back( frLeft );
  fractions.push_back( frCenter );
  fractions.push_back( frRight );
}


//=============================================================================
// Function encapsulating the creation of FTChannelIDs
//=============================================================================
FTChannelID DeFTFibreMat::createChannel(unsigned int hitLayer,
                                     int          quarter,
                                     unsigned int sipmID,
                                     unsigned int grossCellID) const
{
  FTChannelID channel;
  // Convert the grossCellID to netCellID
  unsigned int netCellID = this->netCellID(grossCellID);

  /// Create and push_back the corresponding FT pair
  if ( netCellID > (m_sipmNChannels-1) ) {
    debug() << "Gross cellID " << grossCellID << " corresponds to insensitive cell."
            << " Creating invalid FT channel (the signature is: layer=15)." << endmsg;
    channel = FTChannelID( 15, 0, 0, 0 );
  }
  else {
    channel = FTChannelID( hitLayer, quarter, sipmID, netCellID );
  }
  return channel;
}

unsigned int DeFTFibreMat::netCellID(const unsigned int grossID) const {
  unsigned int netID=999; // any number above m_sipmNChannels
  if      ( grossID >  0 && grossID <  65 ) netID = grossID-1;
  else if ( grossID > 65 && grossID < 130 ) netID = grossID-2;
  return netID;
}

unsigned int DeFTFibreMat::grossCellID(const unsigned int netID) const {
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
double DeFTFibreMat::cellUCoordinate(const FTChannelID& channel) const {
  double uCoord;

  // check if it is a valid channel or one that corresponds to non-sensitive cell
  if ( channel.layer() == 15u ) {
    debug() << "Function cellUCoordinate: cannot determine uCoord for "
            << " non-valid channel " << channel << endmsg;
    uCoord = 99999.;
  }
  else {
    int quarter = channel.quarter();
    /// First get the SiPM right edge, then add the cell offset 
    double sipmREdge = m_sipmOriginX[quarter] + channel.sipmId()*m_sipmStepX[quarter];
    // for u < 0 need additional offset of 1 SiPM
    debug() << "quarter, sipmREdge = " << quarter << ", " << sipmREdge << endmsg;
    if ( !(quarter%2) ) sipmREdge += m_sipmStepX[quarter];
    debug() << "sipmREdge after corr = " << sipmREdge << endmsg;

    // Determine gross cellID
    unsigned int grossID = grossCellID(channel.sipmCell());
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
void DeFTFibreMat::cellIDCoordinates( const double  uCoord,
				      unsigned int  quarter,
				      unsigned int& sipmID,
				      unsigned int& cellID,
				      double&       fracDistCellCenter ) const
{
  /// Get sipmID and local position of its right edge
	sipmID = (unsigned int) ((uCoord - m_sipmOriginX[quarter]) / m_sipmStepX[quarter]);
	double sipmREdgeU = m_sipmOriginX[quarter] + (sipmID + !(quarter%2)) * m_sipmStepX[quarter];
	debug() << "quarter, sipmID, sipmREdgeU = "
		<< quarter << ", "<< sipmID << ", " << sipmREdgeU << endmsg;
  
  /// Get cellID inside the SiPM
  double distSipmREdge = uCoord - sipmREdgeU;
  if ( distSipmREdge < 0 ) {
    error() << "In function cellIDCoordinates: got negative distance between "
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
      error() << "In function cellIDCoordinates: got negative distance between "
              << " the hit and the first sensitive cell. Must be non-negative!)" << endmsg;
    }
    cellID = (unsigned int) (1 + distActiveArea/m_cellSizeX); // >= 1 by construction
    cellREdgeU = (sipmREdgeU + m_sipmEdgeSizeX) + (cellID-1)*m_cellSizeX;
    //use "(cellID-1)" as we added 1 in the preceeding line
  }
  
  /// Determine fractional distance between uCoord and cell center
  fracDistCellCenter = (uCoord - (cellREdgeU + m_cellSizeX/2)) / m_cellSizeX;

  debug() << "\n\tdistSipmREdge: " << distSipmREdge 
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
StatusCode DeFTFibreMat::cellCrossingPoint(const double cellEdgeU,
                                        const Gaudi::XYZPoint& gpEntry,
                                        const Gaudi::XYZPoint& gpExit,
                                        Gaudi::XYZPoint& pIntersect) const
{
  /// Create the plane of the border between two cells
  /// set small, but signed yInit for the calculation of the cell x at the top/bottom of the layer
  double yInit = (gpEntry.y()>0) ? 1.e-6 : -1.e-6;
  double cellYAtVertBorder = (gpEntry.y()>0) ? m_layerMaxY : m_layerMinY;
  double cellXAtVertBorder = xAtVerticalBorder( cellEdgeU, yInit );

  double cellZMin = gpEntry.z();
  Gaudi::XYZPoint gp1(cellEdgeU, 0., cellZMin);
  Gaudi::XYZPoint gp2(cellEdgeU, 0., cellZMin+2*m_fibreMatHalfSizeZ);
  Gaudi::XYZPoint gp3(cellXAtVertBorder, cellYAtVertBorder, cellZMin);
  debug() << "P1, P2, P3:\n\t" << gp1 << "\n\t"
          << gp2 << "\n\t" << gp3 << endmsg;
  
  /// Create the plane, the particle trajectory and their intersection
  Gaudi::Plane3D cellBorderPlane(gp1, gp2, gp3);
  debug() << "CELL BORDER PLANE:" << cellBorderPlane << endmsg;

  Line partTraj(gpEntry, (gpExit - gpEntry).Unit());
  double paramIntersect;
  debug() << "PART TRAJ:" << partTraj << endmsg;

  bool sc = Gaudi::Math::intersection( partTraj, cellBorderPlane, pIntersect, paramIntersect );

  debug() << "Intersection P, param, line(param):"
          << sc << " : " << pIntersect << ", " << paramIntersect << endmsg;
  
  debug() << "Position(p=0), Position(p=xx): " << partTraj.position(0.)
          << partTraj(paramIntersect) << endmsg;

  return StatusCode(sc);
}

//=============================================================================
// Function to create a DetectorSegment (straight line representing an FT channel)
// from a FTChannelID
//=============================================================================
DetectorSegment DeFTFibreMat::createDetSegment(const FTChannelID& channel,
                                            double fracPos) const {

  /// Determine the x coordinate at y=0 of the det. segment
  double cellCenter = cellUCoordinate(channel);
  double segmentU = cellCenter + fracPos*m_cellSizeX;

  /// Determine the upper and lower boundaries of the det. segment
  /// For speed, we do not take into account that below/above the beam pipe
  /// the fibres are shorter
  
  double ds_yMin, ds_yMax;
  double z;
  if ( channel.quarter()>1 ) {
    ds_yMin = 0.;
    ds_yMax = m_layerMaxY;
    z = m_layerPosZ - m_dzDy* (0.5*m_moduleGapH+m_fibreMatHalfSizeY)* cos(m_angle);
    //z = m_layerPosZ - m_dzDy* (0.5*m_moduleGapH+m_fibreMatHalfSizeY);
  }
  else{
    ds_yMin = m_layerMinY;
    ds_yMax = 0.;
    //z = m_layerPosZ + m_dzDy* (0.5*m_moduleGapH+m_fibreMatHalfSizeY);
    z = m_layerPosZ + m_dzDy* (0.5*m_moduleGapH+m_fibreMatHalfSizeY)* cos(m_angle);
  }

  
 
  
  

  //return DetectorSegment( segmentU, m_layerPosZ, m_tanAngle, m_dzDy, ds_yMin, ds_yMax );
  return DetectorSegment( segmentU, z, m_tanAngle, m_dzDy, ds_yMin, ds_yMax );
}

//=============================================================================
// Function to determine the x coordinate at the top/bottom of the layer
// by extrapolating the initial (x0,y0) along the fibres
//=============================================================================
double DeFTFibreMat::xAtVerticalBorder(double x0, double y0) const {
  if (std::abs(m_angle)<1.e-4) return x0;
  else {
    double yAtBorder = (y0>0) ? m_layerMaxY : m_layerMinY;
    return x0 + (yAtBorder-y0)*m_tanAngle;
  }
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
StatusCode DeFTFibreMat::beamPipeYCoord(const double x0, const int ySign, double& yIntersect) const {
  StatusCode sc;
  /// Solve the quadratic equation
  double a = 1 + pow(m_tanAngle,2);
  double b = 2 * x0 * m_tanAngle;
  double c = pow(x0,2) - pow(m_innerHoleRadius,2);
  double D  = pow(b,2) - 4*a*c;
  if ( D < 0 ) {
    /// No real solutions ==> no crossing points
    debug() << "In function beamPipeYCoord: no crossing points found" << endmsg;
    sc = StatusCode::FAILURE;
  }
  else {
    /// We have 2 solutions (can be degenerate)
    /// For ySign=1 (ySign=-1) will return the positive (negative) solution
    yIntersect = (-b + ySign*sqrt(D)) / (2*a);
    sc = StatusCode::SUCCESS;
    debug() << "y-coordinate of beam-pipe crossing point: " << yIntersect << endmsg;
  }
  return sc;
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
StatusCode DeFTFibreMat::beamPipeYCoord(const double xcoord,
                                     const double ycoord,
                                     double& yIntersect) const {
  StatusCode sc;
  /// Solve the quadratic equation
  double x0 = xAtYEq0(xcoord, ycoord);
  int ySign = 0;
  if(ycoord > 0) ySign = 1;
  if(ycoord < 0) ySign = -1;

  double a = 1 + pow(m_tanAngle,2);
  double b = 2 * x0 * m_tanAngle;
  double c = pow(x0,2) - pow(m_innerHoleRadius,2);
  double D  = pow(b,2) - 4*a*c;
  if ( D < 0 ) {
    /// No real solutions ==> no crossing points
    debug() << "In function beamPipeYCoord: no crossing points found" << endmsg;
    sc = StatusCode::FAILURE;
  }
  else {
    /// We have 2 solutions (can be degenerate)
    /// For ySign=1 (ySign=-1) will return the positive (negative) solution
    yIntersect = (-b + ySign*sqrt(D)) / (2*a);
    sc = StatusCode::SUCCESS;
    debug() << "y-coordinate of beam-pipe crossing point: " << yIntersect << endmsg;
  }
  return sc;
}

//=============================================================================
// Function to determine lengh of the fibre as a function of its location 
// to take into account the hole in the middle of the layer
// BEWARE : This hole is considered here as being a square.
//=============================================================================
double DeFTFibreMat::FibreLengh(const Gaudi::XYZPoint&  lpEntry,
                             const Gaudi::XYZPoint&  lpExit) const{

  // define y coordinate of the crossing point between fibre and beam hole
  double YFibreXHole = 0;

  //To check whether the hit fires fibres shorten by beam hole, the mean position of the hit is taken
  double MeanPointX = (lpExit.x() + lpEntry.x())/2.;
  double MeanPointY = (lpExit.y() + lpEntry.y())/2.;
  // checks if entry point of hit is on a fibre shorten by beam hole
  beamPipeYCoord(MeanPointX,MeanPointY, YFibreXHole);
  
  
  // return the lengh of the fibre taking into account the stereo angle
  return (2.*m_fibreMatHalfSizeY - std::abs(YFibreXHole))/cos(m_angle);
}


//=============================================================================
// Get the FTChannelID of the cell located on the left of a given cell (left/right assume
// an observer sitting at (0,0,0) and looking downstream (+z)). The cell
// numbering convention is that the cellID always increases with x, i.e. to the left.
//=============================================================================
FTChannelID DeFTFibreMat::nextChannelLeft(const FTChannelID& channel) const {
  unsigned int grossCellID = this->grossCellID(channel.sipmCell());
  unsigned int grossIDLeftCell = grossCellID + 1u;
  return createChannel( channel.layer(), channel.quarter(), channel.sipmId(), grossIDLeftCell );
}

//=============================================================================
// Get the FTChannelID of the cell located on the right of a given cell (left/right assume
// an observer sitting at (0,0,0) and looking downstream (+z)). The cell
// numbering convention is that the cellID always increases with x, i.e. to the left.
//=============================================================================
FTChannelID DeFTFibreMat::nextChannelRight(const FTChannelID& channel) const {
  unsigned int grossCellID = this->grossCellID(channel.sipmCell());
  unsigned int grossIDRightCell = grossCellID - 1u;
  return createChannel( channel.layer(), channel.quarter(), channel.sipmId(), grossIDRightCell );
}
