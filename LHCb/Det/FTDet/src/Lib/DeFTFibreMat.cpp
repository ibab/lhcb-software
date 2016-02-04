#include <boost/lexical_cast.hpp>

// DetDesc
#include "DetDesc/SolidSubtraction.h"
#include "DetDesc/SolidChild.h"
#include "DetDesc/SolidCons.h"

// Smartrefs
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ISvcLocator.h"

// Gaudi/LHCb Math
#include "GaudiKernel/Plane3DTypes.h"
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/GenericMatrixTypes.h"
#include "LHCbMath/Line.h"
#include "LHCbMath/GeomFun.h"
#include "LHCbMath/LineTypes.h"
#include "GaudiKernel/GaudiException.h"

// FTDet
#include "FTDet/DeFTDetector.h"
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

namespace {
     // temparary utility until Gaudi defines an operator<< for std::array...
     template <typename Container>
     std::vector<std::reference_wrapper<const typename Container::value_type>> to_vector(const Container& c) {
         return { std::begin(c), std::end(c) };
     }

}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DeFTFibreMat::DeFTFibreMat( const std::string& name ) :
  DetectorElement ( name )
{
  //tolerances settings (beware : sensitive stuff)
  m_Ztolerance=0.001;     //multiplicative: more or less arbitrary for now
  m_dZtolerance=0.007;     //additive: also arbitrary
  m_ARtolerance=1e-4;      //machine (Geant ?) arithmetic tolerance
}

//=============================================================================
// Destructor
//=============================================================================
DeFTFibreMat::~DeFTFibreMat() = default;

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
  if ( !m_msg ) m_msg.reset( new MsgStream( msgSvc(), "DeFTFibreMat" ) );
  if( m_msg->level() <= MSG::DEBUG) debug() << "==> Initialize DeFTFibreMat" << endmsg;


  //init depending of detector geometry version
  int FTversion(-1);
  SmartDataPtr<DetectorElement> deFibreTracker (dataSvc(), m_DeFTLocation);
  if( deFibreTracker ){
    if( deFibreTracker -> exists( "FTversion" )) FTversion = deFibreTracker -> param<int>("FTversion");
  }
  if (FTversion < 0) {
    throw GaudiException( "Can't access FT version", "DeFTFibreMat.cpp", StatusCode::FAILURE );
  }
  else {
    m_FTGeomversion=FTversion;
  }

  //---Set the FibreMat ID and stereo angle from the xml DDDB
  m_FibreMatID = this->params()->param<int>("FibreMatID");
  m_angle = this->params()->param<double>("stereoAngle");
  m_tanAngle = tan(m_angle);
  m_cosAngle = cos(m_angle);

  //---Set sections IDs and some flags
  if(m_FTGeomversion < 50) {   //geom < 5x
    // Given the AABBCC convention for the FibreMatID,
    // AA layer number T1( 00 01 02 03 ) T2( 04 05 06 07 ) T3 ( 08 09 10 11 )
    // BB module number 00 01 02 03 04 10(LeftH) 11(RightH) 05 06 07 08 09
    // CC 00(Top) 01(Bottom)
    m_mat = m_FibreMatID%10;
    m_module = int( ( m_FibreMatID %10000 ) / 100 );

    if      ( (m_module <= 4 || m_module == 10) && m_mat ) m_quarter = 1;
    else if ( (m_module <= 4 || m_module == 10) && !m_mat ) m_quarter = 3;
    else if ( ((m_module >= 5 && m_module <= 9) || m_module == 11) && m_mat ) m_quarter = 0;
    else if ( ((m_module >= 5 && m_module <= 9) || m_module == 11) && !m_mat ) m_quarter = 2;
    else{
      m_quarter = -99;
      if( m_msg->level() <= MSG::DEBUG ) debug() << "Aborting calculateHits: not possible to find module/quarter" << endmsg;
      return StatusCode::FAILURE;
    }
    m_layer  = int( m_FibreMatID / 10000 );
  }
  else {
    // convention change: TTLLQQMMM (binary 9 bits, MSB to the left)
    // simplified from hardware format, but same conventions
    // T station (1-3), L layer (0-3), Q quarter (0-3),
    // M module in quarter (1-6, full Left:1->5, 5->1 full right, holes: 6)
    const unsigned int nFMbits=9;
    std::bitset<nFMbits> bset(std::to_string(m_FibreMatID));
    std::bitset<nFMbits> bT(bset.to_string(),0,2);
    std::bitset<nFMbits> bL(bset.to_string(),2,2);
    std::bitset<nFMbits> bQ(bset.to_string(),4,2);
    std::bitset<nFMbits> bM(bset.to_string(),6,3);
    int dT=bT.to_ulong();
    int dL=bL.to_ulong();
    int dQ=bQ.to_ulong();
    int dM=bM.to_ulong();

    //temporary hack to stick to old convention
    //(and for compatibility with other codes also...)
    if(dQ%2>0) {    //0,2= right,  1,3=left
      if(dM<6) m_module=dM-1;
      else m_module=10;
    }
    else {
      if(dM<6) m_module=2*5-dM;
      else m_module=11;
    }
    m_quarter=dQ;
    m_layer=dL+4*(dT-1);
    if(dQ>1) m_mat=0;
    else m_mat=1;
  }

  //derived numberings
  m_relativemodule =
    ( m_module == 10 || m_module == 11 )? 0 :
    ( m_module == 4  || m_module == 5  )? 1 :
    ( m_module == 3  || m_module == 6  )? 2 :
    ( m_module == 2  || m_module == 7  )? 3 :
    ( m_module == 1  || m_module == 8  )? 4 :
    ( m_module == 0  || m_module == 9  )? 5 : 99;

  m_layerID = m_layer;    //DBL: obsolete ?
  m_holey = ( m_module == 10 || m_module == 11 );   //central fibremats with holes


  //-----------------Get geometrical info

  //-------------fibremat geometries

  //----------fibremat geometry v20
  if(m_FTGeomversion <= m_FTGeomVersion_reference) {

    //other flags
    m_RightHoleAxesXZInversion=false;   //do NOT invert Right Hole XZ axes if v2 geometry (temporary)

    const SolidBox *outerBox(0);
    if( !m_holey ){   //fibremat without holes
      outerBox = dynamic_cast<const SolidBox*>( this->geometry()->lvolume()->solid() );
      if ( 0 == outerBox ) {
        throw GaudiException( "Can't acquire layer geometry (SolidBox not holey)", "DeFTFibreMat.cpp", StatusCode::FAILURE );
      }
      m_innerHoleRadius = 0.0;
    }
    else {   //with holes
      const SolidSubtraction *subtrObject = dynamic_cast<const SolidSubtraction*>( this->geometry()->lvolume()->solid() );
      if ( 0 == subtrObject ) {
        throw GaudiException( "Can't acquire layer geometry (SolidSubtraction)", "DeFTFibreMat.cpp", StatusCode::FAILURE );
      }
      outerBox = dynamic_cast<const SolidBox*>( subtrObject->coverTop() );
      if ( 0 == outerBox ) {
        throw GaudiException( "Can't acquire layer geometry (SolidBox holey)", "DeFTFibreMat.cpp", StatusCode::FAILURE );
      }
      //Hole geometry
      const SolidChild *tmpChild = dynamic_cast<const SolidChild*>( (*subtrObject)[0] );
      if ( 0 == tmpChild ) {
        throw GaudiException( "Can't acquire layer geometry (SolidChild)", "DeFTFibreMat.cpp", StatusCode::FAILURE );
      }
      const SolidCons *innerCons = dynamic_cast<const SolidCons*>( tmpChild->solid() );
      if ( 0 == innerCons ) {
        throw GaudiException( "Can't acquire layer geometry (SolidCons)", "DeFTFibreMat.cpp", StatusCode::FAILURE );
      }
      m_innerHoleRadius = innerCons->outerRadiusAtPlusZ();
    }
    m_fibreMatHalfSizeX = outerBox->xHalfLength();
    m_fibreMatHalfSizeY = outerBox->yHalfLength();
    m_fibreMatHalfSizeZ = outerBox->zHalfLength();

    //Fibremat center and corners (Right, Left, Bottom, Top ...) in global frame
    m_fibreMatGlobalCenter = this->geometry()->toGlobal( Gaudi::XYZPoint(0.,0.,0.) );
    m_fibreMatGlobalRT = this->geometry()->toGlobal( Gaudi::XYZPoint(-m_fibreMatHalfSizeX,m_fibreMatHalfSizeY,0) );
    m_fibreMatGlobalLT = this->geometry()->toGlobal( Gaudi::XYZPoint(m_fibreMatHalfSizeX,m_fibreMatHalfSizeY,0) );
    m_fibreMatGlobalRB = this->geometry()->toGlobal( Gaudi::XYZPoint(-m_fibreMatHalfSizeX,-m_fibreMatHalfSizeY,0) );
    m_fibreMatGlobalLB = this->geometry()->toGlobal( Gaudi::XYZPoint(m_fibreMatHalfSizeX,-m_fibreMatHalfSizeY,0) );

    //sipm or fibre end y position (local frame)
    if(m_mat) m_fibreMatSipmY = -m_fibreMatHalfSizeY;
    else m_fibreMatSipmY = m_fibreMatHalfSizeY;
  }


  //-----------fibremat geometry v4x and later
  if (m_FTGeomversion > m_FTGeomVersion_reference) {
    //other flags
    m_RightHoleAxesXZInversion=false;   ///DBL, back to normal for this

    //FibreMat size and position
    m_fibreMatHalfSizeX = 0.5*this->params()->param<double>("FTFibreMatSizeX");
    m_fibreMatHalfSizeY = 0.5*this->params()->param<double>("FTFibreMatSizeY");
    m_fibreMatHalfSizeZ = 0.5*this->params()->param<double>("FTFibreMatSizeZ");

    //Fibremat center and corners (Right, Left, Bottom, Top ...) in global frame
    //with temporary patch for XZ module 11 inversion
    m_fibreMatGlobalCenter = this->geometry()->toGlobal( Gaudi::XYZPoint(0.,0.,0.) );
    if(m_RightHoleAxesXZInversion) {
      m_fibreMatGlobalRT = this->geometry()->toGlobal( Gaudi::XYZPoint(m_fibreMatHalfSizeX,m_fibreMatHalfSizeY,0) );
      m_fibreMatGlobalLT = this->geometry()->toGlobal( Gaudi::XYZPoint(-m_fibreMatHalfSizeX,m_fibreMatHalfSizeY,0) );
      m_fibreMatGlobalRB = this->geometry()->toGlobal( Gaudi::XYZPoint(m_fibreMatHalfSizeX,-m_fibreMatHalfSizeY,0) );
      m_fibreMatGlobalLB = this->geometry()->toGlobal( Gaudi::XYZPoint(-m_fibreMatHalfSizeX,-m_fibreMatHalfSizeY,0) );
    }
    else {
      m_fibreMatGlobalRT = this->geometry()->toGlobal( Gaudi::XYZPoint(-m_fibreMatHalfSizeX,m_fibreMatHalfSizeY,0) );
      m_fibreMatGlobalLT = this->geometry()->toGlobal( Gaudi::XYZPoint(m_fibreMatHalfSizeX,m_fibreMatHalfSizeY,0) );
      m_fibreMatGlobalRB = this->geometry()->toGlobal( Gaudi::XYZPoint(-m_fibreMatHalfSizeX,-m_fibreMatHalfSizeY,0) );
      m_fibreMatGlobalLB = this->geometry()->toGlobal( Gaudi::XYZPoint(m_fibreMatHalfSizeX,-m_fibreMatHalfSizeY,0) );
    }

    //sipm or fibre end y position (local frame)
    if(m_mat) m_fibreMatSipmY = -m_fibreMatHalfSizeY;   //bottom
    else m_fibreMatSipmY = m_fibreMatHalfSizeY;

    //Hole(s)
    if( m_holey ) {   //fibremat with holes
      //Hole position (local frame): same for all sub-boxes
      m_HoleShiftX=this->params()->param<double>("HoleShiftX");
      if(m_RightHoleAxesXZInversion) m_HoleShiftX = -m_HoleShiftX;

      m_HoleShiftY=this->params()->param<double>("HoleShiftY");
      m_posHole.SetXYZ(m_HoleShiftX, m_HoleShiftY, m_fibreMatGlobalCenter.z());

      //Hole sub boxes sizes (local frame)
      m_halfHole1X=this->params()->param<double>("Hole1X")/2;
      m_halfHole2X=this->params()->param<double>("Hole2X")/2;
      m_halfHole3X=this->params()->param<double>("Hole3X")/2;
      m_halfHole4X=this->params()->param<double>("Hole4X")/2;
      m_halfHole1Y=this->params()->param<double>("Hole1Y")/2;
      m_halfHole2Y=this->params()->param<double>("Hole2Y")/2;
      m_halfHole3Y=this->params()->param<double>("Hole3Y")/2;
      m_halfHole4Y=this->params()->param<double>("Hole4Y")/2;
    }
  }



  //--------------layer module geometries (local coordinates)
  double FTFibreModuleSizeX(0),FTFibreModuleSizeY(0),FTDeadHSizeZ(0),FTFibreModuleSizeZ(0),FTFibreSizeZ(0);
  IDetectorElement *parentDet = this->parentIDetectorElement();
  if(!parentDet) {
   throw GaudiException( "Can't acquire parent detector element of FibreMat", "DeFTFibreMat.cpp", StatusCode::FAILURE );
  }

  //-------------layer geometry before v4x and v5x
  if(m_FTGeomversion <= m_FTGeomVersion_reference) {
    //Left/right vertical dead zone
    const SolidBase *sbDeadV(0);
    if( ((sc=findSolidBase(parentDet,"pvDeadVLeft",sbDeadV)) == StatusCode::SUCCESS) ||
        ((sc=findSolidBase(parentDet,"pvDeadVRight",sbDeadV)) == StatusCode::SUCCESS) )
    {
      m_moduleEdgeSizeX=sbDeadV->xMax()-sbDeadV->xMin();
    }
    else {
      fatal() << "Can't find PVolume: "<<"DeadV"<< endmsg;
      return sc;
    }
    //Horizontal dead zone
    const SolidBase *sbDeadH(0);
    if( ((sc=findSolidBase(parentDet,"pvDeadH",sbDeadH)) == StatusCode::SUCCESS) ||
        ((sc=findSolidBase(parentDet,"pvDeadHHoleL",sbDeadH)) == StatusCode::SUCCESS) ||
        ((sc=findSolidBase(parentDet,"pvDeadHHoleR",sbDeadH)) == StatusCode::SUCCESS) )
    {
      m_moduleGapH=sbDeadH->yMax()-sbDeadH->yMin();
    }
    else {
      fatal() << "Can't find PVolume (DeadH)"<< endmsg;
      return sc;
    }
    //Fibre zone
    const SolidBase *sbFibreTop(0);
    if( ((sc=findSolidBase(parentDet,"pvFibreTop",sbFibreTop)) == StatusCode::SUCCESS) ||
        ((sc=findSolidBase(parentDet,"pvFibreMatHoleLTop",sbFibreTop)) == StatusCode::SUCCESS) ||
        ((sc=findSolidBase(parentDet,"pvFibreMatHoleRTop",sbFibreTop)) == StatusCode::SUCCESS) )
    {
      FTFibreSizeZ = sbFibreTop->zMax()-sbFibreTop->zMin();
    }
    else {
      fatal() << "Can't find PVolume (FibreTop)"<< endmsg;
      return sc;
    }

    //get parent to access other useful dimensions
    double FTCarbonSizeZ(0),FTHoneycombSizeZ(0),FTKaptonSizeZ(0);
    IDetectorElement *parent2Det = parentDet->parentIDetectorElement();
    if(0 != parent2Det) {
     //Carbon
      const SolidBase *sbCarbonFullA(0);
      if( ((sc=findSolidBase(parent2Det,"pvCarbonFullA",sbCarbonFullA)) == StatusCode::SUCCESS) ||
          ((sc=findSolidBase(parent2Det,"pvCarbonHoleLA",sbCarbonFullA)) == StatusCode::SUCCESS) ||
          ((sc=findSolidBase(parent2Det,"pvCarbonHoleRA",sbCarbonFullA)) == StatusCode::SUCCESS) )
      {
        FTCarbonSizeZ = sbCarbonFullA->zMax()-sbCarbonFullA->zMin();
      }
      else {
        fatal() << "Can't find PVolume (CarbonFullA)"<< endmsg;
        return sc;
      }
      //Honeycomb
      const SolidBase *sbHoneycombFullA(0);
      if( ((sc=findSolidBase(parent2Det,"pvHoneycombFullA",sbHoneycombFullA)) == StatusCode::SUCCESS)  ||
          ((sc=findSolidBase(parent2Det,"pvHoneycombHoleLA",sbHoneycombFullA)) == StatusCode::SUCCESS) ||
          ((sc=findSolidBase(parent2Det,"pvHoneycombHoleRA",sbHoneycombFullA)) == StatusCode::SUCCESS) )
      {
        FTHoneycombSizeZ = sbHoneycombFullA->zMax()-sbHoneycombFullA->zMin();
      }
      else {
        fatal() << "Can't find PVolume (HoneycombFullA)"<< endmsg;
        return sc;
      }
     //Kapton
      const SolidBase *sbKaptonFullA(0);
      if( ((sc=findSolidBase(parent2Det,"pvKaptonFullA",sbKaptonFullA)) == StatusCode::SUCCESS) ||
          ((sc=findSolidBase(parent2Det,"pvKaptonHoleLA",sbKaptonFullA)) == StatusCode::SUCCESS) ||
          ((sc=findSolidBase(parent2Det,"pvKaptonHoleRA",sbKaptonFullA)) == StatusCode::SUCCESS) )
      {
        FTKaptonSizeZ = sbKaptonFullA->zMax()-sbKaptonFullA->zMin();
      }
      else {
        fatal() << "Can't find PVolume (KaptonFullA)"<< endmsg;
        return sc;
      }
      FTFibreModuleSizeX = 2*(m_fibreMatHalfSizeX + m_moduleEdgeSizeX);
      FTFibreModuleSizeY = sbDeadV->yMax()-sbDeadV->yMin();
      FTDeadHSizeZ = FTCarbonSizeZ + FTHoneycombSizeZ + FTKaptonSizeZ;
      FTFibreModuleSizeZ = 2*FTDeadHSizeZ+FTFibreSizeZ;
    }
    else {
      throw GaudiException( "Can't find 2nd level parent detector element of Fibremat (geometry v2)",
                            "DeFTFibreMat.cpp", StatusCode::FAILURE );
    }
  }


  //-------------layer geometry v4x and v5x
  if(m_FTGeomversion > m_FTGeomVersion_reference) {
    //Left vertical dead zone
    m_moduleEdgeSizeX = parentDet->params()->param<double>("FTDeadVSizeX") +
                        parentDet->params()->param<double>("FTModuleGapX");
    m_moduleGapH = parentDet->params()->param<double>("FTDeadHSizeY");

    //Fibre zone
    FTFibreSizeZ = this->params()->param<double>("FTFibreMatSizeZ");

    IDetectorElement *parent2Det = parentDet->parentIDetectorElement();
    if(!parent2Det) {
      throw GaudiException( "Can't find 2nd level parent detector element of Fibremat (geometry v4/v5)",
                            "DeFTFibreMat.cpp", StatusCode::FAILURE );
    }
    FTFibreModuleSizeX = parent2Det->params()->param<double>("FTFullModuleSizeX");
    FTFibreModuleSizeY = parent2Det->params()->param<double>("FTFullModuleSizeY");
    FTFibreModuleSizeZ = parent2Det->params()->param<double>("FTFullModuleSizeZ");
  }

  //all geometries
  //fibreModule dimensions (in local frame)
  m_FibreModuleHalfSizeX = FTFibreModuleSizeX/2;
  m_FibreModuleHalfSizeY = FTFibreModuleSizeY/2.;
  m_FibreModuleHalfSizeZ = FTFibreModuleSizeZ/2.;

  //final layer dimensions (in local frame)
  m_layerHalfSizeX = 12*m_FibreModuleHalfSizeX;    //to be read from DB
  m_layerHalfSizeY = m_FibreModuleHalfSizeY;
  m_layerHalfSizeZ = m_FibreModuleHalfSizeZ;
  m_layerMinX = -m_layerHalfSizeX;
  m_layerMaxX = +m_layerHalfSizeX;
  m_layerMinY = -m_layerHalfSizeY;
  m_layerMaxY = +m_layerHalfSizeY;
  m_layerMinZ = m_fibreMatGlobalCenter.Z()-m_layerHalfSizeZ;  //DBL
  m_layerMaxZ = m_fibreMatGlobalCenter.Z()+m_layerHalfSizeZ;  //use here m_fibreMatGlobalCenter, different from layerCenter

  /// Determine the slope in the global y-z plane (All geometries)
  Gaudi::XYZPoint tmpLocPoint1(0., 0., 0.);
  Gaudi::XYZPoint tmpLocPoint2(0., 1000000., 0.);
  Gaudi::XYZPoint tmpGlobPoint1 = this->geometry()->toGlobal( tmpLocPoint1 );
  Gaudi::XYZPoint tmpGlobPoint2 = this->geometry()->toGlobal( tmpLocPoint2 );
  m_dzDy = (tmpGlobPoint2.z() - tmpGlobPoint1.z()) / (tmpGlobPoint2.y() - tmpGlobPoint1.y());


  //-----------sipm geometries
  m_sipmNChannels=128;
  m_nSipmPerModule = 16;
  if(m_FTGeomversion <= m_FTGeomVersion_reference) {   //----sipm geometry v2x
    m_sipmSizeX=32.25;
    m_cellSizeX=0.25;
    m_sipmEdgeSizeX=0.625;  //1 dead cell (0.25) + 0.375
  }
  else if(m_FTGeomversion<50) {   //----sipm geometry v4x,v5x
    m_sipmSizeX=32.25;
    double deadXTot=2*m_fibreMatHalfSizeX-m_nSipmPerModule*m_sipmSizeX;
    m_cellSizeX=0.25;
    ///m_sipmEdgeSizeX=0.2;   ///DBL: size of fibremats ?
    m_sipmEdgeSizeX=0.5*deadXTot/m_nSipmPerModule;
  }
  else if (m_FTGeomversion >= 50) {    //-----sipm geometry v5x
     m_sipmEdgeSizeX=this->params()->param<double>("FTSiPMGapX");        //dead egdge area (air+half fiber)
     m_sipmSizeX=this->params()->param<double>("FTSiPMSizeX")-2*m_sipmEdgeSizeX;      //active part
     m_cellSizeX=this->params()->param<double>("FTSiPMCellSizeX");
  }

  m_sipmPitchX = m_sipmSizeX + 2*m_sipmEdgeSizeX;
  //origin of sipm numbering in local frame (number 0 SiPM closest to
  //global ref frame origin, X axis points to the "left")
  //sipm total size (pitch) signed to account for pm numbering direction wrto local frame X axis
  m_sipmOriginX = (m_quarter%2)? -m_fibreMatHalfSizeX : m_fibreMatHalfSizeX;
  m_sipmPitchXsigned   = (m_quarter%2)? m_sipmPitchX : -m_sipmPitchX;


  //debuf stuff
  if( m_msg->level() <= MSG::DEBUG) {
    debug() << "FT geometry parameters:"
    << "\n\tm_layer: " << m_layer
    << "\n\tm_module: " << m_module
    << "\n\tm_quarter: " << m_quarter
    << "\n\tm_mat: " << m_mat
    << "\n\tm_angle: " << m_angle
    << "\n\tm_dzdy: " << m_dzDy
    << "\n\tfibremat size X: " << 2*m_fibreMatHalfSizeX
    << "\n\tfibremat size Y: " << 2*m_fibreMatHalfSizeY
    << "\n\tfibremat size Z: " << 2*m_fibreMatHalfSizeZ
    << "\n\tlayer size X: " << 2*m_layerHalfSizeX
    << "\n\tlayer size Y: " << 2*m_layerHalfSizeY
    << "\n\tlayer size Z: " << 2*m_layerHalfSizeZ
    << "\n\tlayer min X: " << m_layerMinX << "\tmax X: " << m_layerMaxX
    << "\n\tlayer min Y: " << m_layerMinY << "\tmax Y: " << m_layerMaxY
    << "\n\tlayer min Z: " << m_layerMinZ << "\tmax Z: " << m_layerMaxZ
    << "\n\tm_moduleEdgeSizeX : " << m_moduleEdgeSizeX
    << "\n\tSipm Y pos: " << m_fibreMatSipmY
    << "\n\tSipm pitch X: " << m_sipmPitchX
    << "\n\tSipm N per module: " << m_nSipmPerModule
    << endmsg;
    if (m_holey) {
      debug() << "FT Hole geometry parameters:"
      << "\n\tm_posHole: " << m_posHole
      << "\n\tm_halfHole1 X: " << m_halfHole1X << "\tY: " << m_halfHole1Y
      << "\n\tm_halfHole2 X: " << m_halfHole2X << "\tY: " << m_halfHole2Y
      << "\n\tm_halfHole3 X: " << m_halfHole3X << "\tY: " << m_halfHole3Y
      << "\n\tm_halfHole4 X: " << m_halfHole4X << "\tY: " << m_halfHole4Y
      << endmsg;
    }
  }
  return StatusCode::SUCCESS;
}



//=============================================================================
// Finalization
//=============================================================================
StatusCode DeFTFibreMat::finalize(){
  // destroy the MsgService object
  m_msg.reset();
  return StatusCode::SUCCESS;
}


//=============================================================================
// Function to determine which FT channels are traversed by a trajectory
// determined by globalPointEntry and globalPointExit. Fills a vector of
// pairs <FTChannelID, energyFraction>. The latter is the fraction of the energy
// of the MC particle deposited in the corresponding SiPM cell. A light sharing
// model is used in the determination of the energyFractions.
//=============================================================================
StatusCode DeFTFibreMat::calculateListOfFiredChannels(const LHCb::MCHit&  fthit,
                                                      VectFTPairs&         vectChanAndEnergy) const
{

  /// Create a vector of FT pairs which will hold <FTChannel, FractionalPosition>
  /// This is the 'working' quantity. In the final step of the calculateHits method
  /// we use it to create the final vectChanAndEnergy
  VectFTPairs vectChanAndFracPos;

  StatusCode sc = calculateHits(fthit, vectChanAndFracPos);
  if(sc.isFailure()) return sc;

  /// Call the light-sharing method using vectChanAndFracPos,
  /// create the final vector of pairs <FTChannel, EnergyFractions>
  vectChanAndEnergy = createLightSharingChannels( vectChanAndFracPos );

  // Finally, to return the energy deposited in each channel by the hit, each energy fraction
  // is multiplied by the energy
  std::for_each(vectChanAndEnergy.begin(),vectChanAndEnergy.end(),
                [&](FTPair& p) { p.second *=fthit.energy(); } );

  return StatusCode::SUCCESS;
}


//=============================================================================
// Function to determine the mean SiPM channel (ChannelID + fraction) associated
// to the track between entry and exit point.
// @param fthit : hit from Geant4
// @param ChanAndFrac : std::pair (ChannelID + fraction) associated to the mean channel
//=============================================================================
StatusCode DeFTFibreMat::calculateMeanChannel(const LHCb::MCHit&  fthit,
                                              FTPair&             ChanAndFrac) const
{
  /// Create a vector of FT pairs which will hold <FTChannel, FractionalPosition>
  /// This is the 'working' quantity. In the final step of the calculateHits method
  /// we use it to create the final vectChanAndEnergy
  VectFTPairs vectChanAndFracPos;


  StatusCode sc = calculateHits(fthit, vectChanAndFracPos);
  if(sc.isFailure()) return StatusCode::FAILURE;

  //Compute mean channel ID + fractional part associated to the mean fired channel of the hit
  auto vectChanAndFracPosEdgeStart = vectChanAndFracPos.end();
  auto vectChanAndFracPosEdgeEnd = vectChanAndFracPos.begin();

  for (auto itPair = vectChanAndFracPos.begin(); itPair != vectChanAndFracPos.end(); ++itPair) {
    if( m_msg->level() <= MSG::DEBUG) debug() << "==calculateMeanChannel==  channelID="<<itPair->first <<" + " << itPair->second
                                      << "\t diff="<< itPair->first - (vectChanAndFracPos.begin())->first
                                      << "\t End-Start="<<vectChanAndFracPosEdgeEnd->first - vectChanAndFracPosEdgeStart->first
                                      << "\t Start-End="<< vectChanAndFracPosEdgeStart->first - vectChanAndFracPosEdgeEnd->first
                                      << "\t size="<< vectChanAndFracPos.size()
                                      <<endmsg;
    if((itPair->first.layer()<  12) && (vectChanAndFracPosEdgeStart == vectChanAndFracPos.end()))
      vectChanAndFracPosEdgeStart = itPair;

    if(itPair->first.layer() < 12) vectChanAndFracPosEdgeEnd = itPair;

  }

  // Define the output of the function : list of channels and fractional parts
  FTChannelID meanChannel = std::min(vectChanAndFracPosEdgeStart->first, vectChanAndFracPosEdgeEnd->first);

  double fractional = (vectChanAndFracPos.size() + (vectChanAndFracPos.begin())->second
                       +(vectChanAndFracPos.end()-1)->second - 1)/2;
  if( m_msg->level() <= MSG::DEBUG) debug() <<"==calculateMeanChannel==  vectChanAndFracPosEdgeStart->first="<<vectChanAndFracPosEdgeStart->first
                                    << "\t vectChanAndFracPosEdgeEnd->first=" << vectChanAndFracPosEdgeEnd->first
                                    << "\t Min Both diff = "
                                    << std::min(vectChanAndFracPosEdgeStart->first-vectChanAndFracPosEdgeEnd->first,
                                                vectChanAndFracPosEdgeEnd->first-vectChanAndFracPosEdgeStart->first)
                                    << std::endl;

  if((vectChanAndFracPosEdgeStart != vectChanAndFracPos.begin())||(vectChanAndFracPosEdgeEnd !=(vectChanAndFracPos.end()-1)))
  {
    fractional = fractional - vectChanAndFracPos.size() + 1 +
      std::min(vectChanAndFracPosEdgeStart->first-vectChanAndFracPosEdgeEnd->first,
               vectChanAndFracPosEdgeEnd->first-vectChanAndFracPosEdgeStart->first);
  }


  if( m_msg->level() <= MSG::DEBUG) debug() << "==calculateMeanChannel==  Starting channelID="<<meanChannel
                                    << "\t fractional=" << fractional
                                    <<endmsg;
  while(fractional > 0.5)
  {
    meanChannel = meanChannel+ 1;
    fractional -=1;
  }

  if( m_msg->level() <= MSG::DEBUG) debug() << "==calculateMeanChannel==  Final channelID="<<meanChannel
                                    << "\t fractional=" << fractional
                                    <<endmsg;

  ChanAndFrac = std::make_pair(meanChannel,fractional);

  return StatusCode::SUCCESS;
}


//======================================================================================
// Function to determine the mean SiPM channel (ChannelID + fraction) associated
// to the track between entry and exit point.
// @param fthit : hit from Geant4
// @param ChanAndFrac : std::pair (ChannelID + fraction) associated to the mean channel
// November 2014: return Status modified to add a severity code (in the line string...)
// to get some info on rejection reason
//======================================================================================
StatusCode DeFTFibreMat::calculateHits(const LHCb::MCHit&  fthit,
                                       VectFTPairs&        vectChanAndFracPos) const
{

  // get hit position in global and local coordinate systems and perform checks.
  Gaudi::XYZPoint enPLocal  = geometry() -> toLocal( fthit.entry() );
  Gaudi::XYZPoint exPLocal  = geometry() -> toLocal( fthit.exit()  );
  //temporary workaround for Right Hole (module 11, v4 geometry) axes inversion
  if(m_RightHoleAxesXZInversion) {
    doRHAxesInversion(enPLocal);
    doRHAxesInversion(exPLocal);
  }
  if( m_msg->level() <= MSG::DEBUG){
    debug() << "Entry Point in Local frame: " << enPLocal << endmsg;
    debug() << "Exit  Point in Local frame: " << exPLocal << endmsg;
  }

  //test hit validity and get sipm info
  unsigned int enPSipmID, enPCellID; double enPFraction, ensipmREdgeX;
  unsigned int exPSipmID, exPCellID; double exPFraction, exsipmREdgeX;
  StatusCode sc = CalculateSipmInfo(enPLocal, exPLocal,
                                    enPSipmID, enPCellID, enPFraction, ensipmREdgeX,
                                    exPSipmID, exPCellID, exPFraction, exsipmREdgeX);
  if (sc.isFailure()) return sc;


  ///////////////////////////////////////////////////////
  /// Create pairs of FT channels and fractional position
  ///////////////////////////////////////////////////////
  FTChannelID channel;

  /// Case where entry and exit points are in the same cell
  if ( (enPSipmID==exPSipmID) && (enPCellID==exPCellID) ) {
    if( m_msg->level() <= MSG::DEBUG) debug() << "Entry and Exit points are in the same cell!" << endmsg;

    double frac = (enPFraction + exPFraction)/2;
    if( m_msg->level() <= MSG::DEBUG) debug() << "Average fract dist to cell center: " << frac << endmsg;

    /// Create and push-back the pair (FTChannelID, fraction)
    channel = createChannel( layer(), m_module, m_mat, enPSipmID, enPCellID );
    vectChanAndFracPos.push_back( std::make_pair(channel, frac) );
  } //end single cell

  /// Case where entry and exit points are in different cells
  else {

    if( m_msg->level() <= MSG::DEBUG) debug() << "Entry and Exit points are in different cells!" << endmsg;
    /// The procedure is to determine the crossing point between the particle
    /// trajectory and the planes separating the adjacent cells. First we determine
    /// the u-coordinate of the cell border between the first and second cells
    /// and between the last and last-but-once cells. Then we determine the crossing
    /// points. The hits in the middle cells will have fractional position equal
    /// to 0 (i.e. at the cell center).

    /// local X coordinate decreases or increases with z?
    double dUExEn = exPLocal.X() - enPLocal.X();
    int uDir = (dUExEn>0) ? 1 : -1;

    ///Hit Cell edge next to the entry point (can be on the left or on the right)
    double enPCellSeparEdge, enPDistToSeparEdge;
    //DBL
    if(enPCellID==0) enPCellSeparEdge = ensipmREdgeX + (uDir+1)/2*m_sipmEdgeSizeX;
    else if(enPCellID==130) enPCellSeparEdge = ensipmREdgeX + m_sipmPitchX + (uDir-1)/2*m_sipmEdgeSizeX;
    else enPCellSeparEdge = enPLocal.X() - (enPFraction - 0.5*uDir) * m_cellSizeX;
    enPDistToSeparEdge = enPLocal.X() - enPCellSeparEdge;
    if( m_msg->level() <= MSG::DEBUG) debug() << "Entry point: uDir / cellSeparEdge / distToSeparEdge: "
                                      << uDir << ", " << enPCellSeparEdge << ", " << enPDistToSeparEdge << endmsg;

    /// Hit Cell edge next to the exit point
    double exPCellSeparEdge, exPDistToSeparEdge;
    if(exPCellID==0) exPCellSeparEdge = exsipmREdgeX - (uDir-1)/2*m_sipmEdgeSizeX;
    else if(exPCellID==130) exPCellSeparEdge = exsipmREdgeX + m_sipmPitchX - (uDir+1)/2*m_sipmEdgeSizeX;
    else exPCellSeparEdge = exPLocal.X() - (exPFraction + 0.5*uDir) * m_cellSizeX;
    exPDistToSeparEdge = exPLocal.X() - exPCellSeparEdge;
    if( m_msg->level() <= MSG::DEBUG) debug() << "Exit point: uDir / cellSeparEdge / distToSeparEdge: "
                                      << uDir << ", " << exPCellSeparEdge << ", " << exPDistToSeparEdge << endmsg;

    //////////////////////////////////////////////////////////////////
    /// Calculate the fractional position of the particle path center inside
    /// each traversed cell. Create and push_back the relevant FT pairs
    //////////////////////////////////////////////////////////////////
    Gaudi::XYZPoint pIntersect(0.,0.,0.);
    double fracP1, fracP2;

    ///--------The cell of the entry point
    double fracPosFirstCell = 999.;
    sc = cellCrossingPointLocal( enPCellSeparEdge, enPLocal, exPLocal, pIntersect );   //DBL:local/global problem
    // in case of a problem --> directly exit the function
    if ( sc.isFailure() ) {
      if( m_msg->level() <= MSG::DEBUG) debug() << "Aborting calculateHits(...), because of unsuccessful "
                                        << "call to cellCrossingPointLocal" << endmsg;
      return StatusCode(sc, IssueSeverity(IssueSeverity::NIL, -6, "") );
    }

    fracP1 = enPFraction;
    fracP2 = (dUExEn>0) ? 0.5 : -0.5;
    //DBL
    if(enPCellID==0) fracPosFirstCell=-0.5;
    else if(enPCellID==130) fracPosFirstCell=0.5;
    else fracPosFirstCell = (fracP1 + fracP2)/2;
    if( m_msg->level() <= MSG::DEBUG){
      debug() << "Entry Point, SC = " << sc << " Intersection point: " << pIntersect << endmsg;
      debug() << "Entry Point, Frac = " << fracPosFirstCell << endmsg;
    }

    /// Create and push-back the pair (FTChannelID, fraction)
    channel = createChannel( layer(), m_module, m_mat, enPSipmID, enPCellID );
    vectChanAndFracPos.emplace_back( channel, fracPosFirstCell );


    ///--------The middle cells
    unsigned int midCellSipmID, midCellID;
    double midCellFraction, midsipmREdgeX;

    //DBL
    double midX = enPCellSeparEdge;
    while (std::abs(midX - exPCellSeparEdge) > m_ARtolerance) {
      double midCellCenterX = midX + uDir*m_cellSizeX*0.5;
      if( m_msg->level() <= MSG::DEBUG) debug() << "\tMid Cell " << " midCellCenterX = " << midCellCenterX << endmsg;
      cellIDCoordinates( midCellCenterX, m_quarter, midCellSipmID, midCellID, midCellFraction, midsipmREdgeX);

      /// Create and push-back the pair (FTChannelID, fraction)
      channel = createChannel( layer(), m_module, m_mat, midCellSipmID, midCellID );
      vectChanAndFracPos.push_back( std::make_pair(channel, midCellFraction) );

      if(midCellID==0 || midCellID==130) midX+=uDir*m_sipmEdgeSizeX;
      else midX+=uDir*m_cellSizeX;
    }// end loop over mid cells


    ///-------The cell of the exit point
    double fracPosLastCell = 999.;
    sc = cellCrossingPointLocal( exPCellSeparEdge, enPLocal, exPLocal, pIntersect );
    // in case of a problem --> directly exit the function
    if ( sc.isFailure() ) {
      if( m_msg->level() <= MSG::DEBUG) debug() << "Aborting calculateHits(...), because of unsuccessful "
                                        << "call to cellCrossingPointLocal" << endmsg;
      return StatusCode(sc, IssueSeverity(IssueSeverity::NIL, -7, "") );
    }

    //DBL
    fracP1 = exPFraction;
    fracP2 = (dUExEn>0) ? -0.5 : 0.5;
    if(exPCellID==0) fracPosLastCell=-0.5;
    else if(exPCellID==130) fracPosLastCell=0.5;
    else fracPosLastCell = (fracP1 + fracP2)/2;
    if( m_msg->level() <= MSG::DEBUG){
      debug() << "Exit Point, SC = " << sc << " Intersection point: " << pIntersect << endmsg;
      debug() << "Exit Point, Frac = " << fracPosLastCell << endmsg;
    }

    /// Create and push-back the pair (FTChannelID, fraction)
    channel = createChannel( layer(), m_module, m_mat, exPSipmID, exPCellID );
    vectChanAndFracPos.push_back( std::make_pair(channel, fracPosLastCell) );
  }//end more than 1 hit cells



  if( m_msg->level() <= MSG::DEBUG) debug() << "Finished creating FTPairs\n" << endmsg;

  /// Printout the vector of FT pairs
  if( m_msg->level() <= MSG::DEBUG) debug() << "Size of vector of FT pairs: " << vectChanAndFracPos.size() << endmsg;
  DetectorSegment tmpDetSeg;
  for (auto itPair = vectChanAndFracPos.begin(); itPair != vectChanAndFracPos.end(); ++itPair) {
    if( m_msg->level() <= MSG::DEBUG) debug() << itPair->first << ", FractPos: " << itPair->second << endmsg;
    /// Test the inverse function ///
    if( m_msg->level() <= MSG::DEBUG) debug() << "Test of function cellUCoordinate(FTChannelID): "
            << cellUCoordinate(itPair->first) << endmsg;
    /// Test of functions nextChannelLeft/Right
    if( m_msg->level() <= MSG::DEBUG) debug() << "Test of function nextChannelLeft:"
                                      << this->nextChannelLeft( itPair->first ) << endmsg;
    if( m_msg->level() <= MSG::DEBUG) debug() << "Test of function nextChannelRight:"
                                      << this->nextChannelRight( itPair->first ) << endmsg;
    /// Test the function FTChannel+Fraction --> DetSegment
    if( m_msg->level() <= MSG::DEBUG) debug() << "Test of function createDetSegment(FTChannelID, fracPos)" << endmsg;
    tmpDetSeg = createDetSegment( itPair->first, itPair->second );
  }

  return StatusCode(StatusCode::SUCCESS, IssueSeverity(IssueSeverity::NIL, 1, "") );
}



//=============================================================================
// This function determines the fibre lengh and relative position of the hit
// in the fibre according to the Hit position.
// These returned value can be used to determine the attenuation coefficient to be applied
// on the deposited energy to take into acocunt the light attenuation
// through the fibre.
// !!!Works only if entry and exit in the same FibreMat!!!
//=============================================================================
StatusCode DeFTFibreMat::hitPositionInFibre(const LHCb::MCHit&  fthit,
                                            double& meanfibreMaxLength,
                                            double& fibreLenghFrac) const
{
  Gaudi::XYZPoint enPLocal = this->geometry()->toLocal( fthit.entry() );
  Gaudi::XYZPoint exPLocal = this->geometry()->toLocal( fthit.exit() );

  //temporary workaround for Right Hole (module 11, v4 geometry) axes inversion
  if(m_RightHoleAxesXZInversion) {
    doRHAxesInversion(enPLocal);
    doRHAxesInversion(exPLocal);
  }

  double enfibrefullLength,exfibrefullLength;
  inBeamHole(enPLocal, enfibrefullLength);
  inBeamHole(exPLocal, exfibrefullLength);
  meanfibreMaxLength=(enfibrefullLength+exfibrefullLength)/2.;

  double meanY= (enPLocal.y()+exPLocal.y())/2;
  // Fractional lengh is relative to the SiPM position which is on the top
  // for top fibremats & on the bottom for bottom fibremats
  fibreLenghFrac = std::abs(meanY - m_fibreMatSipmY)/meanfibreMaxLength;

  return StatusCode::SUCCESS;
}






//=============================================================================
// Function dealing with the light sharing between neighbouring SiPM cells
//=============================================================================
VectFTPairs DeFTFibreMat::createLightSharingChannels(VectFTPairs& inputVectPairs) const
{
  // First determine the weighting of the energy deposits by taking into account
  // the particle path length in the cells
  //Nota: hits with weight 0.5 or -0.5 are not taken into account in the weights
  //(see also computation of hit cell geometrical fraction)
  double sumPathWeights = std::accumulate( inputVectPairs.begin(), inputVectPairs.end(),
                                           0., [](double spw, const FTPair& ivp) {
                                               return spw + (1 - 2.*std::abs(ivp.second));
                                           });

  std::vector<double> vectPathWeights;
  if(std::abs(sumPathWeights)>1.e-6) {
    std::transform( inputVectPairs.begin(),inputVectPairs.end(),
                    std::back_inserter(vectPathWeights) ,
                    [&](const FTPair& p) {
                        return (1 - 2.*std::abs(p.second)) / sumPathWeights;
    });
  } else {
    vectPathWeights.resize( inputVectPairs.size(), 0. );
  }
  if( m_msg->level() <= MSG::DEBUG) debug() << "Path weights: " << vectPathWeights << endmsg;


  // append the channel before the first one with a direct deposit from the MC particle
  // the sequence of channels in the vector can be both right -> left or the opposite
  std::vector<LHCb::FTChannelID> vectChannels;
  int sequenceRightToLeft = ( inputVectPairs.front().first.sipmCell() < inputVectPairs.back().first.sipmCell() );

  if ( inputVectPairs.size() > 1 && sequenceRightToLeft ) {
    vectChannels.push_back( this->nextChannelRight(inputVectPairs.front().first) );
  }
  else { vectChannels.push_back( this->nextChannelLeft(inputVectPairs.front().first) ); }

  std::vector< std::array<double,3> > vectTriplets; // vector of the lcrFractions
  vectTriplets.reserve(inputVectPairs.size());
  for (auto itPair = inputVectPairs.begin(); itPair != inputVectPairs.end(); ++itPair) {
    // append the FT channel
    vectChannels.push_back( itPair->first );
    // calculate the left/center/right sharing of each channel
    vectTriplets.push_back( this->lightSharing(itPair->second) );
    if( m_msg->level() <= MSG::DEBUG) {
      debug() << "FractPos: " << itPair->second << " ; Sharing: " << to_vector(vectTriplets.back()) << endmsg;
    }
  }

  // append the channel after the last one with a direct deposit from the MC particle
  if ( inputVectPairs.size() > 1 && sequenceRightToLeft ) {
    vectChannels.push_back( nextChannelLeft(inputVectPairs.back().first) );
  }
  else { vectChannels.push_back( nextChannelRight(inputVectPairs.back().first) ); }
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
  if( m_msg->level() <= MSG::DEBUG){
    debug() << "Final Fractions: " << finalFractions << endmsg;
    debug() << "Final Channels: "  << vectChannels << endmsg;

    debug() << "Size of vector of triplets / channels / finalFracts: " << vectTriplets.size()
            << " / " << vectChannels.size() << " / " << finalFractions.size() << endmsg;
  }

  // Create the final vector of FTPairs
  // The second part of the pair is the MCP energy fraction for this channel

  VectFTPairs outputVectPairs; outputVectPairs.reserve(vectChannels.size());
  for (ind=0; ind<vectChannels.size(); ++ind) {
    outputVectPairs.emplace_back( vectChannels.at(ind), finalFractions.at(ind) );
  }
  if( m_msg->level() <= MSG::DEBUG) debug() << "Created vector of light-sharing FT pairs with size: " << outputVectPairs.size() << endmsg;

  return outputVectPairs;
}

//=============================================================================
// A linear light-sharing model. From the fractional position inside a cell
// determine the energy fractions deposited in the left/central/right SiPM cells
//=============================================================================
std::array<double,3> DeFTFibreMat::lightSharing( double position ) const {
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
  return { frLeft, frCenter, frRight };
}


//=============================================================================
// Function encapsulating the creation of FTChannelIDs
//=============================================================================
FTChannelID DeFTFibreMat::createChannel(unsigned int hitLayer,
                                        int          module,
                                        int          mat,
                                        unsigned int sipmID,
                                        unsigned int grossCellID) const
{
  // Convert the grossCellID to netCellID
  unsigned int id = netCellID(grossCellID);

  /// Create and push_back the corresponding FT pair
  if ( id >= m_sipmNChannels ) {
    if( m_msg->level() <= MSG::DEBUG) debug() << "Gross cellID " << grossCellID << " corresponds to insensitive cell."
                       << " Creating invalid FT channel (the signature is: layer=" << m_BadChannelLayerFlag << ")."
                       << endmsg;
    //keep some info on the cell
    unsigned int deadZoneID;
    if(grossCellID==0) deadZoneID=0;
    else if(grossCellID==65) deadZoneID=1;
    else if(grossCellID==130) deadZoneID=2;
    else deadZoneID=3;
    return FTChannelID( m_BadChannelLayerFlag, module, mat, sipmID, deadZoneID );   ///DBL: careful here with bit fields sizes
  }
  return FTChannelID( hitLayer, module, mat, sipmID, id );
}


//========================================================================-===========
// gross cell id (count inactive cells) to net cell id (discard them in the numbering)
//====================================================================================
unsigned int DeFTFibreMat::netCellID(const unsigned int grossID) const {
  unsigned int netID=999; // any number above m_sipmNChannels
  if      ( grossID >  0 && grossID <  65 ) netID = grossID-1;
  else if ( grossID > 65 && grossID < 130 ) netID = grossID-2;
  return netID;
}

//========================================================================
// net cell id (inactive cells not counted) to gross cell id (count them)
//========================================================================
unsigned int DeFTFibreMat::grossCellID(const unsigned int netID) const {
  unsigned int grossID=999; // any number above m_sipmNChannels
  if      ( netID <  64 ) grossID = netID+1;
  else if ( netID < 128 ) grossID = netID+2;
  return grossID;
}



//=============================================================================
// Function to determine the local U coordinate at the center of an FT cell
// from a FTChannelID. In case of non-valid channel: return a large value.
// This is the reciprocal function to 'cellIDCoordinates(lCoord, quarter, ...)'.
// Make sure that the changes in one of these two functions are reflected in
// the conjugate function as well.
// Warning: works only for current fibremat
//=============================================================================
double DeFTFibreMat::cellUCoordinate(const FTChannelID& channel) const {

  // check if it is a valid channel or one that corresponds to non-sensitive cell
  if ( channel.layer() == m_BadChannelLayerFlag ) {
    if( m_msg->level() <= MSG::DEBUG) debug() << "Function cellUCoordinate: cannot determine uCoord for "
                                      << " non-valid channel " << channel << endmsg;
    return 99999.;
  }
  double qLR = (m_quarter%2) ? 1 : -1;   //left or right quarter
  double moduleSizeX = 2*m_FibreModuleHalfSizeX;
  int sipm = channel.sipmId();

  //right or left sipm edge (closest to global origin)
  double sipmREdge = qLR*(m_relativemodule*moduleSizeX) +
                     qLR*(m_moduleEdgeSizeX+(sipm + !(m_quarter%2))*m_sipmPitchX);

  // Determine gross cellID
  unsigned int grossID = grossCellID(channel.sipmCell());
  // offset of the cell center wrt SiPM right edge (cellID always increases from right to left)
  double cellOffset = (grossID + 0.5) * m_cellSizeX;

  // correction in the case that the SiPM edge is not exactly 1 cell width
  double corr = -m_cellSizeX+m_sipmEdgeSizeX;
  cellOffset += corr;

  return  sipmREdge+cellOffset;
}




//=============================================================================
//Find local fibremat frame cell coordinate from FTChannelID
//=============================================================================
double DeFTFibreMat::cellLocalX(const FTChannelID& channel) const {

  // check if it is a valid channel or one that corresponds to non-sensitive cell
  if ( channel.layer() == m_BadChannelLayerFlag ) {
    if( m_msg->level() <= MSG::DEBUG) debug() << "Function cellLocalX: cannot determine local Coord for "
                                              << " non-valid channel " << channel << endmsg;
    return 99999.;
  }
  int sipm = channel.sipmId();
  double qLR = (m_quarter%2) ? 1 : -1;   //right or left quarter

  //right or left sipm edge (closest to global origin)
  double sipmREdgeX = qLR*(-m_fibreMatHalfSizeX + (sipm + !(m_quarter%2))*m_sipmPitchX);

  // Determine gross cellID
  unsigned int grossID = grossCellID(channel.sipmCell());
  // offset of the cell center wrt SiPM right edge (cellID always increases from right to left)
  // correction for the SiPM edge counted as 1 cell width...
  double cellOffset = (grossID + 0.5 - 1) * m_cellSizeX + m_sipmEdgeSizeX;

  return sipmREdgeX+cellOffset;
}





//=============================================================================
// Function to determine the sipmID, gross cellID (counts also insensitive cells
// and SiPM edge gaps) and fractional position inside a cell. Input params are
// the hit x fibremat local coordinate and quarterID.
// fatal error is sipm out of range
//=============================================================================
void DeFTFibreMat::cellIDCoordinates( const double  Xlocal,
                                      unsigned int  quarter,
                                      unsigned int& sipmID,
                                      unsigned int& cellID,
                                      double&       fracDistCellCenter,
                                      double&       sipmREdgeX) const
{
  /// Get sipmID and local position of its right edge (don't use simple division: edge effects !)
  unsigned int lsipm = 0;
  for( unsigned int i = 0; i < m_nSipmPerModule; ++i ){
    if( std::abs(Xlocal - m_sipmOriginX) > (double)i*m_sipmPitchX ) lsipm = i;
    else break;								
  }
  if( lsipm >= m_nSipmPerModule ) {
    fatal() << "In function cellIDCoordinates: SipmID must be between 0 and " << m_nSipmPerModule-1 << endmsg;
    fatal() << " local X: " << Xlocal  << " local sipm: " << lsipm << endmsg;
  }
  sipmID = lsipm;

  //hit sipm 'right' edge in local frame (ie looking in beam direction)
  sipmREdgeX = m_sipmOriginX + (double)(lsipm + !(quarter%2))*m_sipmPitchXsigned;

  if( m_msg->level() <= MSG::DEBUG) debug() << "quarter, sipmID, sipmREdgeX = "
                                    << quarter << ", "<< sipmID << ", " << sipmREdgeX << endmsg;

  /// Get cellID inside the SiPM with some tolerance
  double distSipmREdge = Xlocal - sipmREdgeX;
  if ( distSipmREdge < -m_ARtolerance ) {
    fatal() << "In function cellIDCoordinates: negative distance ("<<distSipmREdge<<")"
            << " between the hit and the sipmEdge (tolerance: "<<m_ARtolerance<<")"<<endmsg;
    fatal() << Xlocal << " " << sipmREdgeX << endmsg;
  }
  else if ((distSipmREdge > -m_ARtolerance) && (distSipmREdge<0)) distSipmREdge=0;


  //cell edge, extreme (dead) channels 0 and 130, fractionnal distance
  //wrto cell center (same axis orientation as mats)
  //get cellID, sensitive cells have ids: 1-64, 66-129 incl.
  //cellID 65 is the inner SiPM hole!
  double cellREdgeX;
  if ( distSipmREdge < m_sipmEdgeSizeX ) {
    cellID = 0;
    cellREdgeX = sipmREdgeX;
    fracDistCellCenter = -0.5;
  }
  else if ( distSipmREdge > m_sipmEdgeSizeX + m_sipmSizeX ) {   //DBL:tolerance ???
    cellID = 130;
    cellREdgeX = sipmREdgeX + m_sipmEdgeSizeX + m_sipmSizeX;
    fracDistCellCenter = 0.5;
  }
  else {
    double distActiveArea = Xlocal - (sipmREdgeX + m_sipmEdgeSizeX);
    if ( distActiveArea < -m_ARtolerance ) {
      fatal() << "In function cellIDCoordinates: got negative distance ("<<distActiveArea<<") between "
              << " the hit and the first sensitive cell (tolerance: "<<m_ARtolerance<<")"<<endmsg;
    }
    else if ((distActiveArea>-m_ARtolerance) && (distActiveArea<0)) distActiveArea=0;
    cellID = (unsigned int) (1 + distActiveArea/m_cellSizeX);      /// >=1 normally
    cellREdgeX = (sipmREdgeX + m_sipmEdgeSizeX) + (cellID-1)*m_cellSizeX;
    fracDistCellCenter = (Xlocal - (cellREdgeX + m_cellSizeX/2)) / m_cellSizeX;
  }

  if( m_msg->level() <= MSG::DEBUG) debug() << " In cellIDCoordinates: "
                         << "\n\tXlocal: " << Xlocal
                         << "\n\tsipmREdgeX: " << sipmREdgeX << "\tdistSipmREdge: " << distSipmREdge
                         << "\n\tsiPMID: " << sipmID << "\n"
                         << "\n\tcellREdgeX: " << cellREdgeX
                         << "\n\tGross cellID: " << cellID
                         << ", distToCellREdge = " << Xlocal - cellREdgeX
                         << ", distToCellCenter = " << Xlocal - (cellREdgeX + m_cellSizeX/2)
                         << "\n\tfracDistCellCenter: " << fracDistCellCenter << endmsg;
}




//=============================================================================
// Function to determine the XYZ crossing point of a line (determined by the
// points lpEntry and lpExit) and the vertical (or tilted in the case of u/v layers)
// plane between two cells, using LOCAL frame (the other routine was bugged).
//=============================================================================
StatusCode DeFTFibreMat::cellCrossingPointLocal(const double lcellEdgeX,
                                                const Gaudi::XYZPoint& lpEntry,
                                                const Gaudi::XYZPoint& lpExit,
                                                Gaudi::XYZPoint& pIntersect) const
{
  /// Create the plane of the border between two cells
  Gaudi::XYZPoint gp1(lcellEdgeX, -m_fibreMatHalfSizeY, m_fibreMatHalfSizeZ);
  Gaudi::XYZPoint gp2(lcellEdgeX, -m_fibreMatHalfSizeY, -m_fibreMatHalfSizeZ);
  Gaudi::XYZPoint gp3(lcellEdgeX, m_fibreMatHalfSizeY, m_fibreMatHalfSizeZ);
  if( m_msg->level() <= MSG::DEBUG) debug() << "P1, P2, P3:\n\t" << gp1 << "\n\t"
                                    << gp2 << "\n\t" << gp3 << endmsg;

  /// Create the plane, the particle trajectory and their intersection (return sc=true if it exists)
  Gaudi::Plane3D cellBorderPlane(gp1, gp2, gp3);
  if( m_msg->level() <= MSG::DEBUG) debug() << "CELL BORDER PLANE:" << cellBorderPlane << endmsg;

  Line partTraj(lpEntry, (lpExit - lpEntry).Unit());
  double paramIntersect;
  if( m_msg->level() <= MSG::DEBUG) debug() << "PART TRAJ:" << partTraj << endmsg;

  bool sc = Gaudi::Math::intersection( partTraj, cellBorderPlane, pIntersect, paramIntersect );

  if( m_msg->level() <= MSG::DEBUG){
    debug() << "Intersection P, param, line(param):"
            << sc << " : " << pIntersect << ", " << paramIntersect << endmsg;

    debug() << "Position(p=0), Position(p=xx): " << partTraj.position(0.)
            << partTraj(paramIntersect) << endmsg;
  }


  return StatusCode(sc);
}



//=============================================================================
// Function to create a DetectorSegment (straight line representing an FT channel)
// from a FTChannelID
// For speed, we do not take into account that below/above the beam pipe
// the fibres are shorter
//=============================================================================
DetectorSegment DeFTFibreMat::createDetSegment(const FTChannelID& channel,
                                               double fracPos) const {

  /// Determine the x coordinate at y=0 of the det. segment
  double hitLocalX = cellLocalX(channel) + fracPos*m_cellSizeX;   //hit x in local fibremat frame

  //global y extrema of active fibre area
  Gaudi::XYZPoint hitGlobalActiveBottom = geometry()->toGlobal( Gaudi::XYZPoint(hitLocalX,-m_fibreMatHalfSizeY,0.) );
  Gaudi::XYZPoint hitGlobalActiveTop = geometry()->toGlobal( Gaudi::XYZPoint(hitLocalX,m_fibreMatHalfSizeY,0.) );
  float hitGlobalActiveMinY = hitGlobalActiveBottom.Y();
  float hitGlobalActiveMaxY = hitGlobalActiveTop.Y();

  //hit x,z at y=0 in global frame
  Gaudi::XYZPoint hitGlobala = geometry()->toGlobal(Gaudi::XYZPoint(hitLocalX, 0., 0.));
  Gaudi::XYZPoint hitGlobalb = geometry()->toGlobal(Gaudi::XYZPoint(hitLocalX, m_fibreMatHalfSizeY, 0.)); //just 2 points on the line
  double axy = (hitGlobala.X()-hitGlobalb.X())/(hitGlobala.Y()-hitGlobalb.Y());
  double azy = (hitGlobala.Z()-hitGlobalb.Z())/(hitGlobala.Y()-hitGlobalb.Y());
  float hitGlobalX0 = hitGlobalb.X()-axy*hitGlobalb.Y();
  float hitGlobalZ0 = hitGlobalb.Z()-azy*hitGlobalb.Y();

  if( m_msg->level() <= MSG::DEBUG) {
     debug() << "layer: " << channel.layer() << " quarter:" << m_quarter << " mat: " << m_mat << " angle: " << m_angle << "\n\t"
             << "hitGlobalX0: " << hitGlobalX0 << " hitLocalX: " << hitLocalX << "\n\t"
             << "hitGlobalZ0: " << hitGlobalZ0 << "\n\t"
             << "Ymin,max:" << hitGlobalActiveMinY << " " << hitGlobalActiveMaxY << "\n\t"
             << endmsg;
  }

  return { hitGlobalX0 , hitGlobalZ0,
           float(-m_tanAngle), float(m_dzDy),
           hitGlobalActiveMinY, hitGlobalActiveMaxY };
}


//=============================================================================
// Function to determine the x coordinate at the top/bottom of the layer
// by extrapolating the initial (x0,y0) along the fibres
//=============================================================================
double DeFTFibreMat::xAtVerticalBorder(double x0, double y0) const {
  if (std::abs(m_angle)<1.e-4) return x0;
  double yAtBorder = (y0>0) ? m_layerMaxY : m_layerMinY;
  return x0 + (yAtBorder-y0)*m_tanAngle;
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
double DeFTFibreMat::beamPipeYCoord(double xcoord, double ycoord) const {
  /// Solve the quadratic equation
  auto x0 = xAtYEq0(xcoord, ycoord);

  auto a = 1 + m_tanAngle*m_tanAngle;
  auto b = 2 * x0 * m_tanAngle;
  auto c = x0*x0 - m_innerHoleRadius*m_innerHoleRadius;
  auto D  = b*b - 4*a*c;
  if ( D < 0 ) {
    /// No real solutions ==> no crossing points
    // a dummy value to initialize the variable
    return 0.;
  }
  /// We have 2 solutions (can be degenerate)
  /// For ycoord>0 (<0) will return the positive (negative) solution
  return ( -b + std::copysign(std::sqrt(D),ycoord) )/ (2*a);
  //debug() << "y-coordinate of beam-pipe crossing point: " << yIntersect << endmsg;
}

//=============================================================================
// Function to determine lengh of the fibre as a function of its location
// to take into account the hole in the middle of the layer (local frame)
// KEPT for BACKCOMPATIBILITY (replaced by "inBeamHole")
//=============================================================================
double DeFTFibreMat::FibreLengh(const Gaudi::XYZPoint&  lpEntry,
                                const Gaudi::XYZPoint&  lpExit) const{
  double fibrefullLength;
  const Gaudi::XYZPoint lpMean((lpEntry.X()+lpExit.X())/2, (lpEntry.Y()+lpExit.Y())/2, (lpEntry.Z()+lpExit.Z())/2);
  inBeamHole(lpMean, fibrefullLength);
  return fibrefullLength;
}

//=============================================================================
// Get the FTChannelID of the cell located on the left of a given cell (left/right assume
// an observer sitting at (0,0,0) and looking downstream (+z)). The cell
// numbering convention is that the cellID always increases with x, i.e. to the left.
//=============================================================================
FTChannelID DeFTFibreMat::nextChannelLeft(const FTChannelID& channel) const {
  unsigned int grossIDLeftCell = grossCellID(channel.sipmCell()) + 1u;
  return createChannel( channel.layer(), channel.module(), channel.mat(), channel.sipmId(), grossIDLeftCell );
}

//=============================================================================
// Get the FTChannelID of the cell located on the right of a given cell (left/right assume
// an observer sitting at (0,0,0) and looking downstream (+z)). The cell
// numbering convention is that the cellID always increases with x, i.e. to the left.
//=============================================================================
FTChannelID DeFTFibreMat::nextChannelRight(const FTChannelID& channel) const {
  unsigned int grossIDRightCell = grossCellID(channel.sipmCell()) - 1u;
  return createChannel( channel.layer(), channel.module(), channel.mat(), channel.sipmId(), grossIDRightCell );
}

//=============================================================================
//Find solidbase in a named pvolume
//=============================================================================
StatusCode DeFTFibreMat::findSolidBase(IDetectorElement *det, const std::string& pvolname, const SolidBase* &solidbase) {

  if(!det) {
    fatal() << "Null input detector element"<< endmsg;
    return StatusCode::FAILURE;
  }

  const IPVolume *pvol=det->geometry()->lvolume()->pvolume(pvolname);
  if(!pvol) return StatusCode::FAILURE;

  solidbase = dynamic_cast<const SolidBase*>(pvol->lvolume()->solid());
  if(!solidbase) {
    fatal() << "Can't find SolidBase for PVolume: "<<pvolname<< endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
//Beam Hole acceptance, depending on geometry versions. Also returns maximum
//fiber length at the input point
//=============================================================================
bool DeFTFibreMat::inBeamHole(const Gaudi::XYZPoint hitLocal,
                              double& fibrelengthMax) const {

    bool inHole(false);
    if(m_holey) {
      if (m_FTGeomversion <= m_FTGeomVersion_reference) { //geom v20
        auto YFibreHole = beamPipeYCoord(hitLocal.X(), hitLocal.Y());   //may be bugged: local or global ?
        //get (maximum) fibre length
        fibrelengthMax=2.*m_fibreMatHalfSizeY - std::abs(YFibreHole)/m_cosAngle;   //Y=0 when no intersection
        //test hit y position
        Gaudi::XYZPoint hitGlobal=this->geometry()->toGlobal(hitLocal);
        if((std::pow(hitGlobal.x(),2) + std::pow(hitGlobal.y(),2)) < std::pow(m_innerHoleRadius,2)) inHole=true;
        else inHole=false;

        if( m_msg->level() <= MSG::DEBUG) {
          debug() << "inBeamHole geom v2:" << " hitLocal: " << hitLocal << "in: " << inHole << " fibrelengthMax: "
                  << fibrelengthMax << endmsg;
        }
      }
      else if (m_FTGeomversion > m_FTGeomVersion_reference) { //geom v4x,v5x
        double dxHit=std::abs(hitLocal.X()-m_posHole.X());        //absolute dx to hole center
        //Hole size for each sub-box
        double dyHole;
        if(dxHit<m_halfHole1X) dyHole=m_halfHole1Y;
        else if(dxHit<m_halfHole2X && dxHit>=m_halfHole1X) dyHole=m_halfHole2Y;
        else if(dxHit<m_halfHole3X && dxHit>=m_halfHole2X) dyHole=m_halfHole3Y;
        else if(dxHit<m_halfHole4X && dxHit>=m_halfHole3X) dyHole=m_halfHole4Y;
        else dyHole=0;

        //get (maximum) fibre length
        if(std::abs(m_fibreMatHalfSizeY-std::abs(m_posHole.Y())) < dyHole) {
          fibrelengthMax=m_fibreMatHalfSizeY+std::abs(m_posHole.Y())-dyHole;
        }
        else fibrelengthMax=2*m_fibreMatHalfSizeY;
        //test hit y position
        if(std::abs(hitLocal.Y()-m_posHole.Y())<dyHole) inHole=true;
        else inHole=false;

        if( m_msg->level() <= MSG::DEBUG) {
          debug() << "inBeamHole geom v4:" << " m_posHole: " << m_posHole <<  " dyHole: " << dyHole
                  <<" hitLocal: " << hitLocal << "in: " << inHole << " fibrelengthMax: "
                  << fibrelengthMax << endmsg;
        }
      }
      else {   //no clear geometry
        throw GaudiException( "Can't find geometry to define beam hole:" + std::to_string(m_FTGeomversion),
                              "DeFTFibreMat.cpp", StatusCode::FAILURE );
      }
    }
    else {   //not a hole fibremat
      fibrelengthMax=2*m_fibreMatHalfSizeY;
      inHole=false;
    }

    return inHole;
}



//temporary workaround for Right Hole (module 11) axes inversion (not to be done for v2 geometry)
void DeFTFibreMat::doRHAxesInversion(Gaudi::XYZPoint& xyzLocal) const {
  xyzLocal.SetXYZ(-xyzLocal.X(), xyzLocal.Y(), -xyzLocal.Z());
}

//=============================================================================
//check if point is in active area (local frame), optional tolerance on z
// (z is along the beam for crossing tracks)
// Some arithmetic or Geant tolerance on x y z
// used mainly for logical ORs of spatial limits tests
//=============================================================================
bool DeFTFibreMat::inActiveArea(const Gaudi::XYZPoint& xyzLocal, double ztolerance) const {

  return std::abs(xyzLocal.X()) <= m_fibreMatHalfSizeX+m_ARtolerance &&
         std::abs(xyzLocal.Y()) <= m_fibreMatHalfSizeY+m_ARtolerance &&
         std::abs(xyzLocal.Z()) <= (m_fibreMatHalfSizeZ+m_ARtolerance)*(1+ztolerance);
}


//=============================================================================
// test if MC passes some validity and acceptance tests
//=============================================================================
StatusCode DeFTFibreMat::CalculateSipmInfo(const Gaudi::XYZPoint enPLocal, const Gaudi::XYZPoint exPLocal,
                             unsigned int &enPSipmID, unsigned int &enPCellID, double &enPFraction, double &ensipmREdgeX,
                             unsigned int &exPSipmID, unsigned int &exPCellID, double &exPFraction, double &exsipmREdgeX) const {

  if( m_relativemodule == 99 ){
    if( m_msg->level() <= MSG::DEBUG) debug() << "FibreMat not found" << endmsg;
    return StatusCode(StatusCode::FAILURE, IssueSeverity(IssueSeverity::NIL, -1, "") );
  }

  //test if entry and exit are in the same fibremat
  if( !(inActiveArea(enPLocal, m_Ztolerance) && inActiveArea(exPLocal, m_Ztolerance)) ) { //Geant 4 problem
    if( m_msg->level() <= MSG::DEBUG) debug() << "Aborting calculateHits: entry and exit points in different fibremats" << endmsg;
    return StatusCode(StatusCode::FAILURE, IssueSeverity(IssueSeverity::NIL, -2, "") );
  }

  //test if entry and exit are not too close in z
  double pLocalSepZ = exPLocal.Z() - enPLocal.Z();
  if ( std::abs( pLocalSepZ ) < 2*m_fibreMatHalfSizeZ*m_dZtolerance ) {
    if( m_msg->level() <= MSG::DEBUG) debug() << "Aborting calculateHits because z-distance "
       << "between entry and exit points is less than 10 microns." << endmsg;
    return StatusCode(StatusCode::FAILURE, IssueSeverity(IssueSeverity::NIL, -3, "") );
  }

  //hole treatment and fibrelength
  double fibrelengthMax(2*m_fibreMatHalfSizeY);
  if( m_holey ){
    if(inBeamHole(enPLocal, fibrelengthMax) || inBeamHole(exPLocal, fibrelengthMax)) {
      if( m_msg->level() <= MSG::DEBUG) debug() << "Aborting calculateHits: entry or exit points are inside "
          << "the beam pipe hole" << endmsg;
      return StatusCode(StatusCode::FAILURE, IssueSeverity(IssueSeverity::NIL, -4, "") );
    }
  }

  ///information of the hit
  if( m_msg->level() <= MSG::DEBUG) debug() << "LayerID = "<< this->layer()
                                    << ", Module = "       << m_module
                                    << ", Isbottom = "     << m_mat
                                    << ", quarter = "      << m_quarter
                                    << ", Stereo angle = " << this->angle() << endmsg;


  ///////////////////////////////////////////////////////
  /// Get cell coordinates of the entry and exit points
  ///////////////////////////////////////////////////////

  if( m_msg->level() <= MSG::DEBUG) debug() << "Local Coordinates of enP and exP: "
                                            << enPLocal.X() << ", " << exPLocal.X() << endmsg;

  cellIDCoordinates( enPLocal.X(), m_quarter, enPSipmID, enPCellID, enPFraction, ensipmREdgeX );
  cellIDCoordinates( exPLocal.X(), m_quarter, exPSipmID, exPCellID, exPFraction, exsipmREdgeX);
  if ( (enPSipmID >= m_nSipmPerModule) || (exPSipmID >= m_nSipmPerModule) ) {   ///DBL: change for from > to >=
    if( m_msg->level() <= MSG::DEBUG) debug() << "Aborting calculateHits: entry or exit points "
                                              << "outside acceptance (out of range sipmID)" << endmsg;
    return StatusCode(StatusCode::FAILURE, IssueSeverity(IssueSeverity::NIL, -5, "") );
  }

  return StatusCode(StatusCode::SUCCESS, IssueSeverity(IssueSeverity::NIL, 1, "") );
}

