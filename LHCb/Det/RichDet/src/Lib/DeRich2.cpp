/** @file DeRich2.cpp
 *
 *  Implementation file for detector description class : DeRich2
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */

#define DERICH2_CPP

// Include files
#include "RichDet/DeRich2.h"
#include "RichDet/DeRichHPDPanel.h"
#include "RichDet/DeRichRadiator.h"
#include "RichDet/DeRichSphMirror.h"

// Gaudi
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IUpdateManagerSvc.h"

// DetDesc
#include "DetDesc/Material.h"
#include "DetDesc/IGeometryInfo.h"

//-----------------------------------------------------------------------------

const CLID CLID_DERich2 = 12002;  // User defined

// Standard Constructors
DeRich2::DeRich2(const std::string & name) : DeRich ( name )
{
  m_rich = Rich::Rich2;
  setMyName("DeRich2");
}

// Standard Destructor
DeRich2::~DeRich2() = default;

// Retrieve Pointer to class defininition structure
const CLID& DeRich2::classID()
{
  return CLID_DERich2;
}

//===========================================================================

StatusCode DeRich2::initialize()
{
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Initialize " << name() << endmsg;

  if ( !DeRich::initialize() ) return StatusCode::FAILURE;

  // Declare dependency on mirrors
  for ( const auto mirrType : {"SphericalMirrorDetElemLocations","SecondaryMirrorDetElemLocations"} )
  {
    for ( const auto& loc : paramVect<std::string>(mirrType) )
    {
      SmartDataPtr<DeRichSphMirror> mirror( dataSvc(), loc );
      updMgrSvc()->registerCondition( this, &*mirror, &DeRich2::updateMirrorParams );
    }
  }

  const auto * pvGasWindow = geometry()->lvolume()->pvolume("pvRich2QuartzWindow:0");
  if ( pvGasWindow )
  {
    const auto & quartzWinTabProps = pvGasWindow->lvolume()->material()->tabulatedProperties();
    for (auto matIter=quartzWinTabProps.begin(); matIter!=quartzWinTabProps.end(); ++matIter)
    {
      if( (*matIter) )
      {
        if ( (*matIter)->type() == "RINDEX" )
        {
          m_gasWinRefIndex.reset( new Rich::TabulatedProperty1D(*matIter) );
          if ( !m_gasWinRefIndex->valid() )
          {
            error()
              << "Invalid RINDEX Rich::TabulatedProperty1D for " << (*matIter)->name() << endmsg;
            return StatusCode::FAILURE;
          }
        }
        if ( (*matIter)->type() == "ABSLENGTH" )
        {
          m_gasWinAbsLength.reset( new Rich::TabulatedProperty1D( *matIter ) );
          if ( !m_gasWinAbsLength->valid() )
          {
            error()
              << "Invalid ABSLENGTH Rich::TabulatedProperty1D for " << (*matIter)->name() << endmsg;
            return StatusCode::FAILURE;
          }
        }
      }
    }
  }
  else
  {
    error() << "Could not find gas window properties" << endmsg;
    return StatusCode::FAILURE;
  }

  // get the nominal reflectivity of the spherical mirror
  const std::string sphCondName("NominalSphericalMirrorReflectivityLoc");
  const std::string sphMirrorReflLoc =
    ( exists            ( sphCondName ) ?
      param<std::string>( sphCondName ) :
      "/dd/Geometry/AfterMagnetRegion/Rich2/Rich2SurfaceTabProperties/Rich2Mirror1SurfaceIdealReflectivityPT" );

  SmartDataPtr<TabulatedProperty> sphMirrorRefl( dataSvc(), sphMirrorReflLoc );
  if ( !sphMirrorRefl )
  {
    error() << "No info on spherical mirror reflectivity at " << sphMirrorReflLoc << endmsg;
    return StatusCode::FAILURE;
  }
  else
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << "Loaded spherical mirror reflectivity from: "<<sphMirrorReflLoc<<endmsg;
    m_nominalSphMirrorRefl.reset( new Rich::TabulatedProperty1D( sphMirrorRefl ) );
    if ( !m_nominalSphMirrorRefl->valid() )
    {
      error()<<"Invalid Rich::TabulatedProperty1D for "<<sphMirrorRefl->name()<<endmsg;
      return StatusCode::FAILURE;
    }
  }

  // get the nominal reflectivity of the secondary mirror
  const std::string secCondName("NominalSecondaryMirrorReflectivityLoc");
  const std::string secMirrorReflLoc =
    ( exists            ( secCondName ) ?
      param<std::string>( secCondName ) :
      "/dd/Geometry/AfterMagnetRegion/Rich2/Rich2SurfaceTabProperties/Rich2Mirror2SurfaceIdealReflectivityPT" );

  SmartDataPtr<TabulatedProperty> secMirrorRefl(dataSvc(),secMirrorReflLoc);
  if ( !secMirrorRefl )
  {
    error() << "No info on secondary mirror reflectivity at " << secMirrorReflLoc << endmsg;
    return StatusCode::FAILURE;
  }
  else
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << "Loaded secondary mirror reflectivity from: "<<secMirrorReflLoc<<endmsg;
    m_nominalSecMirrorRefl.reset( new Rich::TabulatedProperty1D( secMirrorRefl ) );
    if ( !m_nominalSecMirrorRefl->valid() )
    {
      error()
        << "Invalid Rich::TabulatedProperty1D for " << secMirrorRefl->name() << endmsg;
      return StatusCode::FAILURE;
    }
  }

  // Force loading of the HPD panels now
  //hpdPanel(Rich::left);
  //hpdPanel(Rich::right);

   // Trigger first update
  const auto sc = updMgrSvc()->update(this);
  if ( sc.isFailure() ) { fatal() << "UMS updates failed" << endmsg; }

  return sc;
}

//===========================================================================

StatusCode DeRich2::updateMirrorParams()
{

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "CoC      " << param< std::vector<double> >("NominalSphMirrorCoC") << endmsg;
    debug() << "SecPlane " << param< std::vector<double> >("NominalSecMirrorPlane") << endmsg;
    debug() << "RoC      " << param<double>("SphMirrorRadius") << endmsg;
  }

  //const std::vector<double>& nominalCoC = param<std::vector<double> >("NominalSphMirrorCoC");
  //m_nominalCentreOfCurvatureLeft  =
  //  Gaudi::XYZPoint(  nominalCoC[0], nominalCoC[1], nominalCoC[2] );
  //m_nominalCentreOfCurvatureRight =
  //  Gaudi::XYZPoint( -nominalCoC[0], nominalCoC[1], nominalCoC[2] );

  // Load the primary mirror segments to compute the 'nominal' settings
  {
    m_nominalCentreOfCurvatureLeft  = Gaudi::XYZPoint(0,0,0);
    m_nominalCentreOfCurvatureRight = Gaudi::XYZPoint(0,0,0);
    unsigned int nLeft(0), nRight(0);
    double avroc{0};
    for ( const auto& loc : paramVect<std::string>("SphericalMirrorDetElemLocations") )
    {
      SmartDataPtr<DeRichSphMirror> mirror( dataSvc(), loc );
      const auto plane = mirror->centreNormalPlane();
      const auto coc = mirror->centreOfCurvature();
      const auto cen = mirror->mirrorCentre();
      const auto roc = mirror->radius();
      if ( msgLevel(MSG::DEBUG) )
        debug() << loc
                << " Centre = " << cen
                << " Plane " << plane.Normal() << " " << plane.HesseDistance()
                << " RoC = " << roc
                << " CoC = " << coc << endmsg;
      if ( cen.x() > 0 ) { ++nLeft;  m_nominalCentreOfCurvatureLeft  += Gaudi::XYZVector(coc); }
      else               { ++nRight; m_nominalCentreOfCurvatureRight += Gaudi::XYZVector(coc); }
      avroc += roc;
    }
    m_nominalCentreOfCurvatureLeft  /= (double)nLeft;
    m_nominalCentreOfCurvatureRight /= (double)nRight;
    m_sphMirrorRadius = avroc / (double)(nLeft+nRight);
  }

  //m_sphMirrorRadius = param<double>("SphMirrorRadius");

  if ( msgLevel(MSG::DEBUG) )
    debug() << "Nominal centre of curvature "
            << m_nominalCentreOfCurvatureLeft << " , " << m_nominalCentreOfCurvatureRight
            << endmsg;

  if ( msgLevel(MSG::DEBUG) )
    debug() << "Nominal Radius of Curvature = " << m_sphMirrorRadius << endmsg;

  // get the parameters of the nominal flat mirror plane in the form
  // Ax+By+Cz+D=0

  // std::vector<double> nominalFMirrorPlane;
  // nominalFMirrorPlane = param<std::vector<double> >("NominalSecMirrorPlane");

  // m_nominalPlaneLeft = Gaudi::Plane3D(nominalFMirrorPlane[0],nominalFMirrorPlane[1],
  //                                     nominalFMirrorPlane[2],nominalFMirrorPlane[3]);
  // m_nominalPlaneRight = Gaudi::Plane3D(-nominalFMirrorPlane[0],nominalFMirrorPlane[1],
  //                                      nominalFMirrorPlane[2],nominalFMirrorPlane[3]);

  // Load the secondary mirrors to compute the 'nominal' settings
  {
    std::vector<double> nominalFMirrorPlaneLeft{0,0,0,0};
    std::vector<double> nominalFMirrorPlaneRight{0,0,0,0};
    unsigned int nLeft(0), nRight(0);
    for ( const auto& loc : paramVect<std::string>("SecondaryMirrorDetElemLocations") )
    {
      SmartDataPtr<DeRichSphMirror> mirror( dataSvc(), loc );
      auto plane = mirror->centreNormalPlane();
      const auto cen = mirror->mirrorCentre();
      if ( msgLevel(MSG::DEBUG) )
        debug() << loc
                << " Centre = " << mirror->mirrorCentre()
                << " Plane " << plane.Normal() << " " << plane.HesseDistance()
                << " RoC = " << mirror->radius()
                << " CoC = " << mirror->centreOfCurvature() << endmsg;
      auto & params = ( cen.x() > 0 ? nominalFMirrorPlaneLeft : nominalFMirrorPlaneRight );
      params[0] += plane.A();
      params[1] += plane.B();
      params[2] += plane.C();
      params[3] += plane.D();
      ++( cen.x() > 0 ? nLeft : nRight );
    }
    m_nominalPlaneLeft  = Gaudi::Plane3D( nominalFMirrorPlaneLeft[0]/(double)nLeft,
                                          nominalFMirrorPlaneLeft[1]/(double)nLeft,
                                          nominalFMirrorPlaneLeft[2]/(double)nLeft,
                                          nominalFMirrorPlaneLeft[3]/(double)nLeft );
    m_nominalPlaneRight = Gaudi::Plane3D( nominalFMirrorPlaneRight[0]/(double)nRight,
                                          nominalFMirrorPlaneRight[1]/(double)nRight,
                                          nominalFMirrorPlaneRight[2]/(double)nRight,
                                          nominalFMirrorPlaneRight[3]/(double)nRight );
  }

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Nominal Plane Left " << m_nominalPlaneLeft.Normal()
            << " " << m_nominalPlaneLeft.HesseDistance() << endmsg;
    debug() << "Nominal Plane Right " << m_nominalPlaneRight.Normal()
            << " " << m_nominalPlaneRight.HesseDistance() << endmsg;
  }

  m_nominalNormalLeft  = m_nominalPlaneLeft.Normal();
  m_nominalNormalRight = m_nominalPlaneRight.Normal();

  if ( msgLevel(MSG::DEBUG) )
    debug() << "Nominal normal " << m_nominalNormalLeft << " "
            << m_nominalNormalRight << endmsg;

  return StatusCode::SUCCESS;
}

//=========================================================================
//  nominalCentreOfCurvature
//=========================================================================
const Gaudi::XYZPoint&
DeRich2::nominalCentreOfCurvature(const Rich::Side side) const
{
  return ( Rich::right == side ?
           m_nominalCentreOfCurvatureRight :
           m_nominalCentreOfCurvatureLeft );
}

//=========================================================================
//  nominalNormal
//=========================================================================
const Gaudi::XYZVector& DeRich2::nominalNormal(const Rich::Side side) const
{
  return ( Rich::right == side ? m_nominalNormalRight : m_nominalNormalLeft );
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

//=========================================================================
// Access the name for a given panel
//=========================================================================
const std::string DeRich2::panelName( const Rich::Side panel ) const
{

  std::string pname = ( Rich::left == panel ?
                        DeRichLocations::Rich1Panel0 :
                        DeRichLocations::Rich1Panel1 );
  
  if (  RichPhotoDetConfig() == Rich::HPDConfig )
  {

    if ( exists("Rich2HPDPanelDetElemLocations") )
    {
      const auto & panelLoc = paramVect<std::string>("Rich2HPDPanelDetElemLocations");
      pname = panelLoc[panel];
    } 
    else if (  exists("HPDPanelDetElemLocations") ) 
    {  //kept for backward compatibility
      const auto & panelLoc = paramVect<std::string>("HPDPanelDetElemLocations");
      pname = panelLoc[panel];
    }

  }
  else if ( RichPhotoDetConfig() == Rich::PMTConfig ) 
  {

    if ( exists("Rich2PMTPanelDetElemLocations") )
    {
      const auto & panelLoc = paramVect<std::string>("Rich2PMTPanelDetElemLocations");
      pname = panelLoc[panel];
    }
  }

  return pname;
}



