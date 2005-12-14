
/** @file DeRich1.cpp
 *
 *  Implementation file for detector description class : DeRich1
 *
 *  $Id: DeRich1.cpp,v 1.17 2005-12-14 09:34:52 papanest Exp $
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */

#define DERICH1_CPP

// Include files
#include "RichDet/DeRich1.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"

// DetDesc
#include "DetDesc/Material.h"
#include "DetDesc/IGeometryInfo.h"

//-----------------------------------------------------------------------------

const CLID& CLID_DERich1 = 12001;  // User defined

// Standard Constructors
DeRich1::DeRich1() { m_name = "DeRich1"; }

// Standard Destructor
DeRich1::~DeRich1() {}

// Retrieve Pointer to class defininition structure
const CLID& DeRich1::classID()
{
  return CLID_DERich1;
}

//===========================================================================

StatusCode DeRich1::initialize()
{

  MsgStream msg( msgSvc(), myName() );
  msg << MSG::DEBUG << "Initialize " << name() << endmsg;

  if ( !DeRich::initialize() ) return StatusCode::FAILURE;

  const double nominalCoCX = param<double>("Rich1Mirror1NominalCoCX");
  const double nominalCoCY = param<double>("Rich1Mirror1NominalCoCY");
  const double nominalCoCZ = param<double>("Rich1Mirror1NominalCoCZ");

  m_nominalCentreOfCurvature =
    Gaudi::XYZPoint(nominalCoCX, nominalCoCY, nominalCoCZ);
  m_nominalCentreOfCurvatureBottom =
    Gaudi::XYZPoint(nominalCoCX, -nominalCoCY, nominalCoCZ);

  msg << MSG::DEBUG << "Nominal centre of curvature"
      << m_nominalCentreOfCurvature << " ," << m_nominalCentreOfCurvatureBottom
      << endmsg;

  m_sphMirrorRadius = param<double>("Rich1SphMirror1Radius");

  // get the parameters of the nominal flat mirror plane in the form
  // Ax+By+Cz+D=0
  std::vector<double> nominalFMirrorPlane;
  if ( exists("Rich1NominalSecMirrorPlane") )
    nominalFMirrorPlane = param<std::vector<double> >("Rich1NominalSecMirrorPlane");
  else
    nominalFMirrorPlane = param<std::vector<double> >("Rich1NominalFlatMirrorPlane");

  m_nominalPlaneTop = Gaudi::Plane3D(nominalFMirrorPlane[0],nominalFMirrorPlane[1],
                                     nominalFMirrorPlane[2],nominalFMirrorPlane[3]);
  m_nominalPlaneBottom = Gaudi::Plane3D(nominalFMirrorPlane[0],-nominalFMirrorPlane[1],
                                        nominalFMirrorPlane[2],nominalFMirrorPlane[3]);

  m_nominalNormal = m_nominalPlaneTop.Normal();
  m_nominalNormalBottom = m_nominalPlaneBottom.Normal();

  msg << MSG::DEBUG << "Nominal normal " << Gaudi::XYZVector( m_nominalNormal )
      << Gaudi::XYZVector( m_nominalNormalBottom ) << endmsg;

  const IPVolume* pvGasWindow = geometry()->lvolume()->
    pvolume("pvRich1SubMaster")->lvolume()->pvolume("pvRich1MagShH0:0")->
    lvolume()->pvolume("pvRich1GQuartzWH0:0");

  if ( pvGasWindow ) {
    const Material::Tables& quartzWinTabProps = pvGasWindow->lvolume()->
      material()->tabulatedProperties();
    Material::Tables::const_iterator matIter;
    for (matIter=quartzWinTabProps.begin(); matIter!=quartzWinTabProps.end(); ++matIter) {
      if( (*matIter) ){
        if ( (*matIter)->type() == "RINDEX" ) {
          m_gasWinRefIndex = (*matIter);
        }
        if ( (*matIter)->type() == "ABSLENGTH" ) {
          m_gasWinAbsLength = (*matIter);
        }
      }
    }
  } else {
    msg << MSG::ERROR << "Could not find gas window properties" << endmsg;
    return StatusCode::FAILURE;
  }

  // find the HPD quantum efficiency
  std::string HPD_QETabPropLoc;
  if ( exists( "RichHpdQETableName" ) )
    HPD_QETabPropLoc = param<std::string>( "RichHpdQETableName" );
  else
    HPD_QETabPropLoc = "/dd/Materials/RichMaterialTabProperties/HpdQuantumEff";

  SmartDataPtr<TabulatedProperty> tabQE (dataSvc(), HPD_QETabPropLoc);
  if ( !tabQE )
    msg << MSG::ERROR << "No info on HPD Quantum Efficiency" << endmsg;
  else {
    m_HPDQuantumEff = tabQE;
    msg << MSG::DEBUG << "Loaded HPD QE from: " << HPD_QETabPropLoc << endmsg;
  }

  // get the nominal reflectivity of the spherical mirror
  std::string surfLoc;
  if (name().find("Magnet") == std::string::npos)
    surfLoc = "";
  else
    surfLoc = "BeforeMagnetRegion/";

  const std::string sphMirrorReflLoc = "/dd/Geometry/"+surfLoc+
    "Rich1/Rich1SurfaceTabProperties/Rich1Mirror1SurfaceIdealReflectivityPT";
  SmartDataPtr<TabulatedProperty> sphMirrorRefl( dataSvc(), sphMirrorReflLoc );
  if ( !sphMirrorRefl )
    msg << MSG::ERROR << "No info on spherical mirror reflectivity" << endmsg;
  else {
    m_nominalSphMirrorRefl = sphMirrorRefl;
    msg << MSG::DEBUG << "Loaded spherical mirror reflectivity from: "
        << sphMirrorReflLoc << endmsg;
  }

  // get the nominal reflectivity of the secondary mirror
  const std::string secMirrorReflLoc = "/dd/Geometry/"+surfLoc+
    "Rich1/Rich1SurfaceTabProperties/Rich1Mirror2SurfaceIdealReflectivityPT";
  SmartDataPtr<TabulatedProperty> secMirrorRefl(dataSvc(),secMirrorReflLoc);
  if ( !secMirrorRefl )
    msg << MSG::ERROR << "No info on secondary mirror reflectivity" << endmsg;
  else {
    m_nominalSecMirrorRefl = secMirrorRefl;
    msg << MSG::DEBUG << "Loaded secondary mirror reflectivity from: "
        << secMirrorReflLoc << endmsg;
  }

  return StatusCode::SUCCESS;
}

const Gaudi::XYZPoint& DeRich1::nominalCentreOfCurvature(const Rich::Side side) const
{
  return ( Rich::bottom == side ? m_nominalCentreOfCurvatureBottom :
           m_nominalCentreOfCurvature );
}

const Gaudi::XYZVector& DeRich1::nominalNormal(const Rich::Side side) const
{
  return ( Rich::bottom == side ? m_nominalNormalBottom : m_nominalNormal );
}

const Gaudi::Plane3D& DeRich1::nominalPlane(const Rich::Side side) const
{
  return ( Rich::top == side ? m_nominalPlaneTop : m_nominalPlaneBottom );
}

Rich::Side DeRich1::side( const Gaudi::XYZPoint & point) const
{
  return ( point.y() >= 0.0 ? Rich::top : Rich::bottom );
}
