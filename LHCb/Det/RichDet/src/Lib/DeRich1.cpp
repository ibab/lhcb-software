
/** @file DeRich1.cpp
 *
 *  Implementation file for detector description class : DeRich1
 *
 *  $Id: DeRich1.cpp,v 1.27 2007-02-01 16:41:13 jonrob Exp $
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */

#define DERICH1_CPP

// Include files
#include "RichDet/DeRich1.h"
#include "RichDet/DeRichHPDPanel.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IUpdateManagerSvc.h"

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

  const std::vector<double> nominalCoC = param<std::vector<double> >("NominalSphMirrorCoC");
  m_nominalCentreOfCurvature =
    Gaudi::XYZPoint( nominalCoC[0], nominalCoC[1], nominalCoC[2]);
  m_nominalCentreOfCurvatureBottom =
    Gaudi::XYZPoint( nominalCoC[0], -nominalCoC[1], nominalCoC[2]);

  msg << MSG::DEBUG << "Nominal centre of curvature"
      << m_nominalCentreOfCurvature << " ," << m_nominalCentreOfCurvatureBottom
      << endmsg;

  m_sphMirrorRadius = param<double>("SphMirrorRadius");

  // get the parameters of the nominal flat mirror plane in the form
  // Ax+By+Cz+D=0
  std::vector<double> nominalFMirrorPlane;
  nominalFMirrorPlane = param<std::vector<double> >("NominalSecMirrorPlane");

  m_nominalPlaneTop = Gaudi::Plane3D(nominalFMirrorPlane[0],nominalFMirrorPlane[1],
                                     nominalFMirrorPlane[2],nominalFMirrorPlane[3]);
  m_nominalPlaneBottom = Gaudi::Plane3D(nominalFMirrorPlane[0],-nominalFMirrorPlane[1],
                                        nominalFMirrorPlane[2],nominalFMirrorPlane[3]);

  m_nominalNormal = m_nominalPlaneTop.Normal();
  m_nominalNormalBottom = m_nominalPlaneBottom.Normal();

  msg << MSG::DEBUG << "Nominal normal " << Gaudi::XYZVector( m_nominalNormal )
      << Gaudi::XYZVector( m_nominalNormalBottom ) << endmsg;

  const IPVolume* pvGasWindow( 0 );
  const IPVolume* pvRich1SubMaster = geometry()->lvolume()->pvolume("pvRich1SubMaster");
  if ( pvRich1SubMaster ) {
    pvGasWindow = pvRich1SubMaster->lvolume()->pvolume("pvRich1MagShH0:0")->
      lvolume()->pvolume("pvRich1GQuartzWH0:0");
  }
  if ( pvGasWindow ) {
    const Material::Tables& quartzWinTabProps = pvGasWindow->lvolume()->
      material()->tabulatedProperties();
    Material::Tables::const_iterator matIter;
    for (matIter=quartzWinTabProps.begin(); matIter!=quartzWinTabProps.end(); ++matIter) {
      if( (*matIter) ){
        if ( (*matIter)->type() == "RINDEX" ) 
        {
          msg << MSG::DEBUG << "Loaded gas window refIndex from: " << (*matIter)->name()
              << endmsg;
          m_gasWinRefIndex = new RichTabulatedProperty1D( *matIter );
          if ( !m_gasWinRefIndex->valid() )
          {
            msg << MSG::ERROR
                << "Invalid RINDEX RichTabulatedProperty1D for " << (*matIter)->name() << endreq;
            return StatusCode::FAILURE;
          }
        }
        if ( (*matIter)->type() == "ABSLENGTH" ) 
        {
          m_gasWinAbsLength = new RichTabulatedProperty1D( *matIter );
          if ( !m_gasWinAbsLength->valid() )
          {
            msg << MSG::ERROR
                << "Invalid ABSLENGTH RichTabulatedProperty1D for " << (*matIter)->name() << endreq;
            return StatusCode::FAILURE;
          }
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
    surfLoc = "BeforeMagnetRegion/";

  const std::string sphMirrorReflLoc = "/dd/Geometry/"+surfLoc+
    "Rich1/Rich1SurfaceTabProperties/Rich1Mirror1SurfaceIdealReflectivityPT";
  SmartDataPtr<TabulatedProperty> sphMirrorRefl( dataSvc(), sphMirrorReflLoc );
  if ( !sphMirrorRefl )
    msg << MSG::ERROR << "No info on spherical mirror reflectivity" << endmsg;
  else {
    msg << MSG::DEBUG << "Loaded spherical mirror reflectivity from: "
        << sphMirrorReflLoc << endmsg;
    m_nominalSphMirrorRefl = new RichTabulatedProperty1D( sphMirrorRefl );
    if ( !m_nominalSphMirrorRefl->valid() )
    {
      msg << MSG::ERROR
          << "Invalid RichTabulatedProperty1D for " << sphMirrorRefl->name() << endreq;
      return StatusCode::FAILURE;
    }
  }

  // get the nominal reflectivity of the secondary mirror
  const std::string secMirrorReflLoc = "/dd/Geometry/"+surfLoc+
    "Rich1/Rich1SurfaceTabProperties/Rich1Mirror2SurfaceIdealReflectivityPT";
  SmartDataPtr<TabulatedProperty> secMirrorRefl(dataSvc(),secMirrorReflLoc);
  if ( !secMirrorRefl )
    msg << MSG::ERROR << "No info on secondary mirror reflectivity" << endmsg;
  else {
    msg << MSG::DEBUG << "Loaded secondary mirror reflectivity from: "
        << secMirrorReflLoc << endmsg;
    m_nominalSecMirrorRefl = new RichTabulatedProperty1D( secMirrorRefl );
    if ( !m_nominalSecMirrorRefl->valid() )
    {
      msg << MSG::ERROR
          << "Invalid RichTabulatedProperty1D for " << secMirrorRefl->name() << endreq;
      return StatusCode::FAILURE;
    }
  }

  // get pointers to HPD panels
  SmartDataPtr<DeRichHPDPanel> panel0(dataSvc(),DeRichHPDPanelLocation::Rich1Panel0);
  if ( !panel0 ) {
    msg << MSG::FATAL << "Cannot load " << DeRichHPDPanelLocation::Rich1Panel0
        << endmsg;
    return StatusCode::FAILURE;
  }
  SmartDataPtr<DeRichHPDPanel> panel1(dataSvc(),DeRichHPDPanelLocation::Rich1Panel1);
  if ( !panel1 ) {
    msg << MSG::FATAL << "Cannot load " << DeRichHPDPanelLocation::Rich1Panel1
        << endmsg;
    return StatusCode::FAILURE;
  }
  m_HPDPanels[panel0->side()] = panel0;
  m_HPDPanels[panel1->side()] = panel1;

  // conditions for mirror alignment
  m_sphMirAlignCond = condition( "Rich1Mirror1Align" );
  if ( !m_sphMirAlignCond ) {
    msg << MSG::FATAL << "Cannot load Condition Rich1Mirror1Align" << endmsg;
    return StatusCode::FAILURE;
  }
  m_secMirAlignCond = condition( "Rich1Mirror2Align" );
  if ( !m_secMirAlignCond ) {
    msg << MSG::FATAL << "Cannot load Condition Rich1Mirror2Align" << endmsg;
    return StatusCode::FAILURE;
  }

  IUpdateManagerSvc* ums = updMgrSvc();
  ums->registerCondition(this,m_sphMirAlignCond.path(),&DeRich1::alignSphMirrors);
  ums->registerCondition(this,m_secMirAlignCond.path(),&DeRich1::alignSecMirrors);
  StatusCode upsc = ums->update(this);

  // initialize all child detector elements. Thsi triggers the update
  // of the radiator properties
  childIDetectorElements();

  msg << MSG::DEBUG << "Initialisation Complete" << endreq;
  return upsc;
}

//=========================================================================
//  alignSphMirrors
//=========================================================================
StatusCode DeRich1::alignSphMirrors()
{
  std::vector<const ILVolume*> mirrorCont;
  // (mis)align spherical mirrors
  const ILVolume* lvRich1Gas = geometry()->lvolume()->pvolume(0)->lvolume();
  mirrorCont.push_back( lvRich1Gas );
  StatusCode sc = alignMirrors(mirrorCont, "Rich1Mirror1Q",
                               m_sphMirAlignCond, "RichSphMirrorRs");
  if (sc == StatusCode::FAILURE) return sc;

  // (mis)align spherical mirrors (2nd layer)
  sc = alignMirrors(mirrorCont, "Rich1Mirror1Be",
                    m_sphMirAlignCond, "RichSphMirrorRs");
  if (sc == StatusCode::FAILURE) return sc;

  return StatusCode::SUCCESS;
}

//=========================================================================
//  alignSecMirrors
//=========================================================================
StatusCode DeRich1::alignSecMirrors()
{
  std::vector<const ILVolume*> mirrorCont;
  const ILVolume* lvRich1Gas = geometry()->lvolume()->pvolume(0)->lvolume();
  mirrorCont.push_back( lvRich1Gas );

  // (mis)align secondary mirrors
  StatusCode sc = alignMirrors(mirrorCont, "Rich1Mirror2:",
                               m_secMirAlignCond, "RichSecMirrorRs");
  if (sc == StatusCode::FAILURE) return sc;

  return StatusCode::SUCCESS;
}


//=========================================================================
//  nominalCentreOfCurvature
//=========================================================================
const Gaudi::XYZPoint& DeRich1::nominalCentreOfCurvature(const Rich::Side side) const
{
  return ( Rich::bottom == side ? m_nominalCentreOfCurvatureBottom :
           m_nominalCentreOfCurvature );
}

//=========================================================================
//  nominalNormal
//=========================================================================
const Gaudi::XYZVector& DeRich1::nominalNormal(const Rich::Side side) const
{
  return ( Rich::bottom == side ? m_nominalNormalBottom : m_nominalNormal );
}

//=========================================================================
//  nominalPlane
//=========================================================================
const Gaudi::Plane3D& DeRich1::nominalPlane(const Rich::Side side) const
{
  return ( Rich::top == side ? m_nominalPlaneTop : m_nominalPlaneBottom );
}

//=========================================================================
//  side
//=========================================================================
Rich::Side DeRich1::side( const Gaudi::XYZPoint & point) const
{
  return ( point.y() >= 0.0 ? Rich::top : Rich::bottom );
}
