
/** @file DeRich1.cpp
 *
 *  Implementation file for detector description class : DeRich1
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */

#define DERICH1_CPP

// Include files
#include "RichDet/DeRich1.h"
#include "RichDet/DeRichHPDPanel.h"
#include "RichDet/DeRichSphMirror.h"

// Gaudi
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IUpdateManagerSvc.h"

// DetDesc
#include "DetDesc/Material.h"
#include "DetDesc/IGeometryInfo.h"

//-----------------------------------------------------------------------------

const CLID CLID_DERich1 = 12001;  // User defined

// Standard Constructors
DeRich1::DeRich1(const std::string & name) : DeRich ( name )
{
  m_rich = Rich::Rich1;
  setMyName("DeRich1");
}

// Standard Destructor
DeRich1::~DeRich1() { }

// Retrieve Pointer to class defininition structure
const CLID& DeRich1::classID()
{
  return CLID_DERich1;
}

//===========================================================================

StatusCode DeRich1::initialize()
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
      updMgrSvc()->registerCondition( this, &*mirror, &DeRich1::updateMirrorParams );
    }
  }

  const IPVolume* pvGasWindow( nullptr );
  const IPVolume* pvRich1SubMaster = geometry()->lvolume()->pvolume("pvRich1SubMaster");
  if ( pvRich1SubMaster )
  {
    pvGasWindow = pvRich1SubMaster->lvolume()->pvolume("pvRich1MagShH0:0")->
      lvolume()->pvolume("pvRich1GQuartzWH0:0");
  }
  if ( pvGasWindow )
  {
    const Material::Tables& quartzWinTabProps = pvGasWindow->lvolume()->
      material()->tabulatedProperties();
    for ( auto matIter = quartzWinTabProps.begin(); matIter != quartzWinTabProps.end(); ++matIter )
    {
      if( (*matIter) )
      {
        if ( (*matIter)->type() == "RINDEX" )
        {
          if ( msgLevel(MSG::DEBUG) )
            debug() << "Loaded gas window refIndex from: " << (*matIter)->name()
                    << endmsg;
          m_gasWinRefIndex.reset( new Rich::TabulatedProperty1D( *matIter ) );
          if ( !m_gasWinRefIndex->valid() )
          {
            error() << "Invalid RINDEX Rich::TabulatedProperty1D for " << (*matIter)->name() << endmsg;
            return StatusCode::FAILURE;
          }
        }
        if ( (*matIter)->type() == "ABSLENGTH" )
        {
          m_gasWinAbsLength.reset( new Rich::TabulatedProperty1D( *matIter ) );
          if ( !m_gasWinAbsLength->valid() )
          {
            error() << "Invalid ABSLENGTH Rich::TabulatedProperty1D for " << (*matIter)->name() << endmsg;
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
  const std::string sphCondname("NominalSphericalMirrorReflectivityLoc");
  const std::string sphMirrorReflLoc =
    ( exists             ( sphCondname ) ?
      param<std::string> ( sphCondname ) :
      "/dd/Geometry/BeforeMagnetRegion/Rich1/Rich1SurfaceTabProperties/Rich1Mirror1SurfaceIdealReflectivityPT" );

  SmartDataPtr<TabulatedProperty> sphMirrorRefl( dataSvc(), sphMirrorReflLoc );
  if ( !sphMirrorRefl )
  {
    error() <<"No info on spherical mirror reflectivity at " << sphMirrorReflLoc << endmsg;
    return StatusCode::FAILURE;
  }
  else
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << "Loaded spherical mirror reflectivity from: "
              << sphMirrorReflLoc << endmsg;
    m_nominalSphMirrorRefl.reset( new Rich::TabulatedProperty1D( sphMirrorRefl ) );
    if ( !m_nominalSphMirrorRefl->valid() )
    {
      error() <<"Invalid Rich::TabulatedProperty1D for " << sphMirrorRefl->name() << endmsg;
      return StatusCode::FAILURE;
    }
  }

  // get the nominal reflectivity of the secondary mirror
  const std::string secCondName("NominalSecondaryMirrorReflectivityLoc");
  const std::string secMirrorReflLoc =
    ( exists             ( secCondName ) ?
      param<std::string> ( secCondName ) :
      "/dd/Geometry/BeforeMagnetRegion/Rich1/Rich1SurfaceTabProperties/Rich1Mirror2SurfaceIdealReflectivityPT" );

  SmartDataPtr<TabulatedProperty> secMirrorRefl(dataSvc(),secMirrorReflLoc);
  if ( !secMirrorRefl )
  {
    error() << "No info on secondary mirror reflectivity at " << secMirrorReflLoc << endmsg;
    return StatusCode::FAILURE;
  }
  else
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() <<"Loaded secondary mirror reflectivity from: "<<secMirrorReflLoc<<endmsg;
    m_nominalSecMirrorRefl.reset( new Rich::TabulatedProperty1D( secMirrorRefl ) );
    if ( !m_nominalSecMirrorRefl->valid() )
    {
      error() << "Invalid Rich::TabulatedProperty1D for " << secMirrorRefl->name() << endmsg;
      return StatusCode::FAILURE;
    }
  }

  // Force loading of the HPD panels now
  //hpdPanel(Rich::top);
  //hpdPanel(Rich::bottom);

  // initialize all child detector elements. This triggers the update
  // of the radiator properties
  childIDetectorElements();

  // Trigger first update
  const auto sc = updMgrSvc()->update(this);
  if ( sc.isFailure() ) { fatal() << "UMS updates failed" << endmsg; }

  return sc;
}

//===========================================================================

StatusCode DeRich1::updateMirrorParams()
{

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "CoC Top      " << param< std::vector<double> >("NominalSphMirrorTopCoC") << endmsg;
    debug() << "CoC Bot      " << param< std::vector<double> >("NominalSphMirrorBotCoC") << endmsg;
    debug() << "CoC          " << param< std::vector<double> >("NominalSphMirrorCoC") << endmsg;
    debug() << "SecPlane Top " << param< std::vector<double> >("NominalSecMirrorPlaneTop") << endmsg;
    debug() << "SecPlane Bot " << param< std::vector<double> >("NominalSecMirrorPlaneBot") << endmsg;
    debug() << "SecPlane     " << param< std::vector<double> >("NominalSecMirrorPlane") << endmsg;
    debug() << "RoC          " << param<double>("SphMirrorRadius") << endmsg;
  }

  // const auto nominalCoCTop = param< std::vector<double> >("NominalSphMirrorCoC");
  // const auto nominalCoCBot = param< std::vector<double> >("NominalSphMirrorCoC");
  // m_nominalCentreOfCurvatureTop    =
  //   Gaudi::XYZPoint( nominalCoCTop[0],  nominalCoCTop[1], nominalCoCTop[2] );
  // m_nominalCentreOfCurvatureBottom =
  //   Gaudi::XYZPoint( nominalCoCBot[0], -nominalCoCBot[1], nominalCoCBot[2] );

  // const auto nominalCoCTop = param< std::vector<double> >("NominalSphMirrorTopCoC");
  // const auto nominalCoCBot = param< std::vector<double> >("NominalSphMirrorBotCoC");
  // m_nominalCentreOfCurvatureTop    =
  //  Gaudi::XYZPoint( nominalCoCTop[0], nominalCoCTop[1], nominalCoCTop[2] );
  // m_nominalCentreOfCurvatureBottom =
  //  Gaudi::XYZPoint( nominalCoCBot[0], nominalCoCBot[1], nominalCoCBot[2] );

  // Load the primary mirror segments to compute the 'nominal' settings
  {
    m_nominalCentreOfCurvatureTop    = Gaudi::XYZPoint(0,0,0);
    m_nominalCentreOfCurvatureBottom = Gaudi::XYZPoint(0,0,0);
    unsigned int nTop(0), nBot(0);
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
      if ( cen.y() > 0 ) { ++nTop; m_nominalCentreOfCurvatureTop    += Gaudi::XYZVector(coc); }
      else               { ++nBot; m_nominalCentreOfCurvatureBottom += Gaudi::XYZVector(coc); }
      avroc += roc;
    }
    m_nominalCentreOfCurvatureTop    /= (double)nTop;
    m_nominalCentreOfCurvatureBottom /= (double)nBot;
    m_sphMirrorRadius = avroc / (double)(nTop+nBot);
  }

  //m_sphMirrorRadius = param<double>("SphMirrorRadius");

  if ( msgLevel(MSG::DEBUG) )
    debug() << "Nominal centre of curvature "
            << m_nominalCentreOfCurvatureTop << " , " << m_nominalCentreOfCurvatureBottom
            << endmsg;

  if ( msgLevel(MSG::DEBUG) )
    debug() << "Nominal Radius of Curvature = " << m_sphMirrorRadius << endmsg;

  // get the parameters of the nominal flat mirror plane in the form
  // Ax+By+Cz+D=0

  // const auto nominalFMirrorPlaneTop = param< std::vector<double> >("NominalSecMirrorPlane");
  // const auto nominalFMirrorPlaneBot = param< std::vector<double> >("NominalSecMirrorPlane");
  // m_nominalPlaneTop    = Gaudi::Plane3D(nominalFMirrorPlaneTop[0], nominalFMirrorPlaneTop[1],
  //                                       nominalFMirrorPlaneTop[2], nominalFMirrorPlaneTop[3]);
  // m_nominalPlaneBottom = Gaudi::Plane3D(nominalFMirrorPlaneBot[0],-nominalFMirrorPlaneBot[1],
  //                                       nominalFMirrorPlaneBot[2], nominalFMirrorPlaneBot[3]);

  // const auto nominalFMirrorPlaneTop = param< std::vector<double> >("NominalSecMirrorPlaneTop");
  // const auto nominalFMirrorPlaneBot = param< std::vector<double> >("NominalSecMirrorPlaneBot");
  // m_nominalPlaneTop    = Gaudi::Plane3D(nominalFMirrorPlaneTop[0],nominalFMirrorPlaneTop[1],
  //                                       nominalFMirrorPlaneTop[2],nominalFMirrorPlaneTop[3]);
  // m_nominalPlaneBottom = Gaudi::Plane3D(nominalFMirrorPlaneBot[0],nominalFMirrorPlaneBot[1],
  //                                       nominalFMirrorPlaneBot[2],nominalFMirrorPlaneBot[3]);

  // Load the secondary mirrors to compute the 'nominal' settings
  {
    std::vector<double> nominalFMirrorPlaneTop{0,0,0,0};
    std::vector<double> nominalFMirrorPlaneBot{0,0,0,0};
    unsigned int nTop(0), nBot(0);
    for ( const auto& loc : paramVect<std::string>("SecondaryMirrorDetElemLocations") )
    {
      SmartDataPtr<DeRichSphMirror> mirror( dataSvc(), loc );
      auto plane = mirror->centreNormalPlane();
      const auto cen = mirror->mirrorCentre();
      if ( msgLevel(MSG::DEBUG) )
        debug() << loc
                << " Centre = " << mirror->mirrorCentre()
                << " Plane = " << plane.Normal() << " " << plane.HesseDistance()
                << " RoC = " << mirror->radius()
                << " CoC = " << mirror->centreOfCurvature() << endmsg;
      auto & params = ( cen.y() > 0 ? nominalFMirrorPlaneTop : nominalFMirrorPlaneBot );
      params[0] += plane.A();
      params[1] += plane.B();
      params[2] += plane.C();
      params[3] += plane.D();
      ++( cen.y() > 0 ? nTop : nBot );
    }
    m_nominalPlaneTop    = Gaudi::Plane3D( nominalFMirrorPlaneTop[0]/(double)nTop,
                                           nominalFMirrorPlaneTop[1]/(double)nTop,
                                           nominalFMirrorPlaneTop[2]/(double)nTop,
                                           nominalFMirrorPlaneTop[3]/(double)nTop );
    m_nominalPlaneBottom = Gaudi::Plane3D( nominalFMirrorPlaneBot[0]/(double)nBot,
                                           nominalFMirrorPlaneBot[1]/(double)nBot,
                                           nominalFMirrorPlaneBot[2]/(double)nBot,
                                           nominalFMirrorPlaneBot[3]/(double)nBot );
  }

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Nominal Plane Top " << m_nominalPlaneTop.Normal()
            << " " << m_nominalPlaneTop.HesseDistance() << endmsg;
    debug() << "Nominal Plane Bot " << m_nominalPlaneBottom.Normal()
            << " " << m_nominalPlaneBottom.HesseDistance() << endmsg;
  }

  m_nominalNormalTop    = m_nominalPlaneTop.Normal();
  m_nominalNormalBottom = m_nominalPlaneBottom.Normal();

  if ( msgLevel(MSG::DEBUG) )
    debug() << "Nominal normal " << m_nominalNormalTop << " "
            << m_nominalNormalBottom << endmsg;

  return StatusCode::SUCCESS;
}

//=========================================================================
//  nominalCentreOfCurvature
//=========================================================================
const Gaudi::XYZPoint&
DeRich1::nominalCentreOfCurvature(const Rich::Side side) const
{
  return ( Rich::bottom == side ?
           m_nominalCentreOfCurvatureBottom :
           m_nominalCentreOfCurvatureTop   );
}

//=========================================================================
//  nominalNormal
//=========================================================================
const Gaudi::XYZVector& DeRich1::nominalNormal(const Rich::Side side) const
{
  return ( Rich::bottom == side ? m_nominalNormalBottom : m_nominalNormalTop );
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

//=========================================================================
// Access the name for a given panel
//=========================================================================
const std::string DeRich1::panelName( const Rich::Side panel ) const
{

  std::string pname = ( Rich::top == panel ?
                        DeRichLocations::Rich1Panel0 :
                        DeRichLocations::Rich1Panel1 );
 
  if(  RichPhotoDetConfig() == Rich::HPDConfig ) {

    if ( exists("Rich1HPDPanelDetElemLocations") )
    {
      const auto & panelLoc = paramVect<std::string>("Rich1HPDPanelDetElemLocations");
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

    if ( exists("Rich1PMTPanelDetElemLocations") )
    {
      const auto& panelLoc = paramVect<std::string>("Rich1PMTPanelDetElemLocations");
      pname = panelLoc[panel];
    }

  }

  return pname;
}

