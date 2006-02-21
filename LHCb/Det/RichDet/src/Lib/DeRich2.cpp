
/** @file DeRich2.cpp
 *
 *  Implementation file for detector description class : DeRich2
 *
 *  $Id: DeRich2.cpp,v 1.22 2006-02-21 15:17:25 jonrob Exp $
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */

#define DERICH2_CPP

// Include files
#include "RichDet/DeRich2.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IUpdateManagerSvc.h"

// DetDesc
#include "DetDesc/Material.h"
#include "DetDesc/IGeometryInfo.h"

//-----------------------------------------------------------------------------

const CLID& CLID_DERich2 = 12002;  // User defined

// Standard Constructors
DeRich2::DeRich2() { m_name = "DeRich2"; }

// Standard Destructor
DeRich2::~DeRich2() {}

// Retrieve Pointer to class defininition structure
const CLID& DeRich2::classID()
{
  return CLID_DERich2;
}

//===========================================================================

StatusCode DeRich2::initialize()
{

  MsgStream msg( msgSvc(), myName() );
  msg << MSG::DEBUG << "Initialize " << name() << endmsg;

  if ( !DeRich::initialize() ) return StatusCode::FAILURE;

  const std::vector<double> nominalCoC = param<std::vector<double> >("Rich2NominalCoC");
  m_nominalCentreOfCurvature =
    Gaudi::XYZPoint( nominalCoC[0], nominalCoC[1], nominalCoC[2]);
  m_nominalCentreOfCurvatureRight =
    Gaudi::XYZPoint( -nominalCoC[0], nominalCoC[1], nominalCoC[2]);

  msg << MSG::DEBUG << "Nominal centre of curvature"
      << m_nominalCentreOfCurvature << " ," << m_nominalCentreOfCurvatureRight
      << endmsg;

  m_sphMirrorRadius = param<double>("Rich2SphMirrorRadius");

  // get the parameters of the nominal flat mirror plane in the form
  // Ax+By+Cz+D=0
  std::vector<double> nominalFMirrorPlane;
  if ( exists("Rich2NominalSecMirrorPlane") )
    nominalFMirrorPlane = param<std::vector<double> >("Rich2NominalSecMirrorPlane");
  else
    nominalFMirrorPlane = param<std::vector<double> >("Rich2NominalFlatMirrorPlane");

  m_nominalPlaneLeft = Gaudi::Plane3D(nominalFMirrorPlane[0],nominalFMirrorPlane[1],
                                      nominalFMirrorPlane[2],nominalFMirrorPlane[3]);
  m_nominalPlaneRight = Gaudi::Plane3D(-nominalFMirrorPlane[0],nominalFMirrorPlane[1],
                                       nominalFMirrorPlane[2],nominalFMirrorPlane[3]);

  m_nominalNormal = m_nominalPlaneLeft.Normal();
  m_nominalNormalRight = m_nominalPlaneRight.Normal();

  msg << MSG::DEBUG << "Nominal normal " << Gaudi::XYZVector( m_nominalNormal )
      << Gaudi::XYZVector( m_nominalNormalRight ) << endmsg;

  const IPVolume* pvGasWindow = geometry()->lvolume()->pvolume("pvRich2QuartzWindow:0");
  // compatibilty will DC04 xml
  if ( !pvGasWindow )
    pvGasWindow = geometry()->lvolume()->pvolume("pvRich2QuartzWindow1");

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

  // get the nominal reflectivity of the spherical mirror
  std::string surfLoc;
  if (name().find("Magnet") == std::string::npos)
    surfLoc = "";
  else
    surfLoc = "AfterMagnetRegion/";

  const std::string sphMirrorReflLoc = "/dd/Geometry/"+surfLoc+
    "Rich2/Rich2SurfaceTabProperties/Rich2Mirror1SurfaceIdealReflectivityPT";
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
    "Rich2/Rich2SurfaceTabProperties/Rich2Mirror2SurfaceIdealReflectivityPT";
  SmartDataPtr<TabulatedProperty> secMirrorRefl(dataSvc(),secMirrorReflLoc);
  if ( !secMirrorRefl )
    msg << MSG::ERROR << "No info on secondary mirror reflectivity" << endmsg;
  else {
    m_nominalSecMirrorRefl = secMirrorRefl;
    msg << MSG::DEBUG << "Loaded secondary mirror reflectivity from: "
        << secMirrorReflLoc << endmsg;
  }

  // update mirror alignment
  m_sphMirAlignCond = condition( "Rich2SphMirrorAlign" );
  if ( !m_sphMirAlignCond ) {
    msg << MSG::FATAL << "Cannot load Condition Rich2SphMirrorAlign" << endmsg;
    return StatusCode::SUCCESS;
  }
  m_secMirAlignCond = condition( "Rich2SecMirrorAlign" );
  if ( !m_secMirAlignCond ) {
    msg << MSG::FATAL << "Cannot load Condition Rich2SecMirrorAlign" << endmsg;
    return StatusCode::SUCCESS;
  }
  IUpdateManagerSvc* ums = updMgrSvc();
  ums->registerCondition(this,m_sphMirAlignCond.path(),&DeRich2::alignSphMirrors );
  ums->registerCondition(this,m_secMirAlignCond.path(),&DeRich2::alignSecMirrors );
  StatusCode upsc = ums->update(this);

  msg << MSG::DEBUG << "Initialisation Complete" << endreq;
  return upsc;
}

//=========================================================================
//  alignSphMirrors
//=========================================================================
StatusCode DeRich2::alignSphMirrors()
{

  std::vector<const ILVolume*> mirrorCont;
  // (mis)align spherical mirrors
  const IPVolume* pvRich2Gas = geometry()->lvolume()->pvolume(0);
  mirrorCont.push_back( pvRich2Gas->lvolume() );
  StatusCode sc = alignMirrors(mirrorCont, "Rich2SphMirror",
                               m_sphMirAlignCond, "RichSphMirrorRs");
  if (sc == StatusCode::FAILURE) return sc;

  return StatusCode::SUCCESS;
}

//=========================================================================
//  alignSecMirrors
//=========================================================================
StatusCode DeRich2::alignSecMirrors()
{

  std::vector<const ILVolume*> mirrorCont;

  // (mis)align secondary mirrors in both containers
  const IPVolume* pvRich2SecMirrorCont0 = geometry()->lvolume()->pvolume(0)->
    lvolume()->pvolume("pvRich2SecMirrorCont0");
  mirrorCont.push_back( pvRich2SecMirrorCont0->lvolume() );
  const IPVolume* pvRich2SecMirrorCont1 = geometry()->lvolume()->pvolume(0)->
    lvolume()->pvolume("pvRich2SecMirrorCont1");
  mirrorCont.push_back( pvRich2SecMirrorCont1->lvolume() );

  StatusCode sc = alignMirrors(mirrorCont, "Rich2SecMirror",
                               m_secMirAlignCond, "RichSecMirrorRs");
  if (sc == StatusCode::FAILURE) return sc;

  return StatusCode::SUCCESS;
}

//=========================================================================
//  nominalCentreOfCurvature
//=========================================================================
const Gaudi::XYZPoint& DeRich2::nominalCentreOfCurvature(const Rich::Side side) const
{
  return ( Rich::right == side ? m_nominalCentreOfCurvatureRight :
           m_nominalCentreOfCurvature );
}

//=========================================================================
//  nominalNormal
//=========================================================================
const Gaudi::XYZVector& DeRich2::nominalNormal(const Rich::Side side) const
{
  return ( Rich::right == side ? m_nominalNormalRight : m_nominalNormal );
}

//=========================================================================
//  nominalPlane
//=========================================================================
const Gaudi::Plane3D& DeRich2::nominalPlane(const Rich::Side side) const
{
  return ( Rich::left == side ? m_nominalPlaneLeft : m_nominalPlaneRight );
}

//=========================================================================
//  side
//=========================================================================
Rich::Side DeRich2::side( const Gaudi::XYZPoint& point ) const
{
  return ( point.x() >= 0.0 ? Rich::left : Rich::right );
}

