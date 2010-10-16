
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

// Gaudi
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IUpdateManagerSvc.h"

// DetDesc
#include "DetDesc/Material.h"
#include "DetDesc/IGeometryInfo.h"

//-----------------------------------------------------------------------------

const CLID CLID_DERich2 = 12002;  // User defined

// Standard Constructors
DeRich2::DeRich2(const std::string & name) 
  : DeRich ( name )
{ setMyName("DeRich2"); }

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

  debug() << "Initialize " << name() << endmsg;

  if ( !DeRich::initialize() ) return StatusCode::FAILURE;

  const std::vector<double> nominalCoC = param<std::vector<double> >("NominalSphMirrorCoC");
  m_nominalCentreOfCurvature =
    Gaudi::XYZPoint( nominalCoC[0], nominalCoC[1], nominalCoC[2]);
  m_nominalCentreOfCurvatureRight =
    Gaudi::XYZPoint( -nominalCoC[0], nominalCoC[1], nominalCoC[2]);

  debug() << "Nominal centre of curvature"
          << m_nominalCentreOfCurvature << " ," << m_nominalCentreOfCurvatureRight
          << endmsg;

  m_sphMirrorRadius = param<double>("SphMirrorRadius");

  // get the parameters of the nominal flat mirror plane in the form
  // Ax+By+Cz+D=0
  std::vector<double> nominalFMirrorPlane;
  nominalFMirrorPlane = param<std::vector<double> >("NominalSecMirrorPlane");

  m_nominalPlaneLeft = Gaudi::Plane3D(nominalFMirrorPlane[0],nominalFMirrorPlane[1],
                                      nominalFMirrorPlane[2],nominalFMirrorPlane[3]);
  m_nominalPlaneRight = Gaudi::Plane3D(-nominalFMirrorPlane[0],nominalFMirrorPlane[1],
                                       nominalFMirrorPlane[2],nominalFMirrorPlane[3]);

  m_nominalNormal = m_nominalPlaneLeft.Normal();
  m_nominalNormalRight = m_nominalPlaneRight.Normal();

  debug() << "Nominal normal " << Gaudi::XYZVector( m_nominalNormal )
          << Gaudi::XYZVector( m_nominalNormalRight ) << endmsg;

  const IPVolume* pvGasWindow = geometry()->lvolume()->pvolume("pvRich2QuartzWindow:0");
  if ( pvGasWindow ) {
    const Material::Tables& quartzWinTabProps = pvGasWindow->lvolume()->
      material()->tabulatedProperties();
    Material::Tables::const_iterator matIter;
    for (matIter=quartzWinTabProps.begin(); matIter!=quartzWinTabProps.end(); ++matIter) {
      if( (*matIter) ){
        if ( (*matIter)->type() == "RINDEX" )
        {
          m_gasWinRefIndex = new RichTabulatedProperty1D( *matIter );
          if ( !m_gasWinRefIndex->valid() )
          {
            error()
              << "Invalid RINDEX RichTabulatedProperty1D for " << (*matIter)->name() << endmsg;
            return StatusCode::FAILURE;
          }
        }
        if ( (*matIter)->type() == "ABSLENGTH" )
        {
          m_gasWinAbsLength = new RichTabulatedProperty1D( *matIter );
          if ( !m_gasWinAbsLength->valid() )
          {
            error()
              << "Invalid ABSLENGTH RichTabulatedProperty1D for " << (*matIter)->name() << endmsg;
            return StatusCode::FAILURE;
          }
        }
      }
    }
  } else {
    error() << "Could not find gas window properties" << endmsg;
    return StatusCode::FAILURE;
  }

  // get the nominal reflectivity of the spherical mirror
  std::string sphMirrorReflLoc;
  if ( exists("NominalSphericalMirrorReflectivityLoc" ) )
    sphMirrorReflLoc = param<std::string>( "NominalSphericalMirrorReflectivityLoc" );
  else
    sphMirrorReflLoc = "/dd/Geometry/AfterMagnetRegion/Rich2/Rich2SurfaceTabProperties/Rich2Mirror1SurfaceIdealReflectivityPT";

  SmartDataPtr<TabulatedProperty> sphMirrorRefl( dataSvc(), sphMirrorReflLoc );
  if ( !sphMirrorRefl )
    error() << "No info on spherical mirror reflectivity" << endmsg;
  else
  {
    debug() << "Loaded spherical mirror reflectivity from: "<<sphMirrorReflLoc<<endmsg;
    m_nominalSphMirrorRefl = new RichTabulatedProperty1D( sphMirrorRefl );
    if ( !m_nominalSphMirrorRefl->valid() )
    {
      error()<<"Invalid RichTabulatedProperty1D for "<<sphMirrorRefl->name()<<endmsg;
      return StatusCode::FAILURE;
    }
  }

  // get the nominal reflectivity of the secondary mirror
  std::string secMirrorReflLoc;
  if ( exists("NominalSecondaryMirrorReflectivityLoc" ) )
    secMirrorReflLoc = param<std::string>( "NominalSecondaryMirrorReflectivityLoc" );
  else
    secMirrorReflLoc = "/dd/Geometry/AfterMagnetRegion/Rich2/Rich2SurfaceTabProperties/Rich2Mirror2SurfaceIdealReflectivityPT";

  SmartDataPtr<TabulatedProperty> secMirrorRefl(dataSvc(),secMirrorReflLoc);
  if ( !secMirrorRefl )
    error() << "No info on secondary mirror reflectivity" << endmsg;
  else
  {
    debug() << "Loaded secondary mirror reflectivity from: "<<secMirrorReflLoc<<endmsg;
    m_nominalSecMirrorRefl = new RichTabulatedProperty1D( secMirrorRefl );
    if ( !m_nominalSecMirrorRefl->valid() )
    {
      error()
        << "Invalid RichTabulatedProperty1D for " << secMirrorRefl->name() << endmsg;
      return StatusCode::FAILURE;
    }
  }

  // get pointers to HPD panels. Check for the new locations.
  std::string panel0Location = DeRichLocations::Rich2Panel0;
  std::string panel1Location = DeRichLocations::Rich2Panel1;

  if ( exists("HPDPanelDetElemLocations") )
  {
    std::vector<std::string> panelLoc= paramVect<std::string>("HPDPanelDetElemLocations");
    panel0Location = panelLoc[0];
    panel1Location = panelLoc[1];
  }

  SmartDataPtr<DeRichHPDPanel> panel0(dataSvc(),panel0Location);
  if ( !panel0 ) {
    fatal() << "Cannot load " << panel0Location << endmsg;
    return StatusCode::FAILURE;
  }
  SmartDataPtr<DeRichHPDPanel> panel1(dataSvc(),panel1Location);
  if ( !panel1 ) {
    fatal() << "Cannot load " << panel1Location << endmsg;
    return StatusCode::FAILURE;
  }
  m_HPDPanels[panel0->side()] = panel0;
  m_HPDPanels[panel1->side()] = panel1;

  // initialize Rich2Gas
  SmartDataPtr<DeRichRadiator> rich2Gas(dataSvc(), DeRichLocations::Rich2Gas);
  if ( !rich2Gas )
    error() << "Cannot initialize Rich2Gas" << endmsg;


  // update mirror alignment
  IUpdateManagerSvc* ums = updMgrSvc();

  bool needUpdate( false );
  if ( hasCondition( "Rich2SphMirrorAlign" ) )
  {
    m_sphMirAlignCond = condition( "Rich2SphMirrorAlign" );
    ums->registerCondition(this,m_sphMirAlignCond.path(),&DeRich2::alignSphMirrors );
    needUpdate = true;
  }
  else {
    m_sphMirAlignCond = 0;
  }

  if ( hasCondition( "Rich2SecMirrorAlign" ) )
  {
    m_secMirAlignCond = condition( "Rich2SecMirrorAlign" );
    ums->registerCondition(this,m_secMirAlignCond.path(),&DeRich2::alignSecMirrors );
    needUpdate = true;
  }
  else {
    m_secMirAlignCond = 0;
  }

  StatusCode upsc = StatusCode::SUCCESS;
  if ( needUpdate ) upsc = ums->update(this);

  debug() << "Initialisation Complete. Update:" << upsc << endmsg;
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
  const ILVolume* lvRich2Gas = pvRich2Gas->lvolume();
  // ckeck if there are spherical mirror containers
  const IPVolume* pvSphMirCont0 = lvRich2Gas->pvolume("pvRich2SphMirrorCont0");
  if ( pvSphMirCont0 )
  {
    const ILVolume* lvSphMirCont0 = pvSphMirCont0->lvolume();
    mirrorCont.push_back( lvSphMirCont0 );
    const IPVolume* pvSphMirCont1 = lvRich2Gas->pvolume("pvRich2SphMirrorCont1");
    const ILVolume* lvSphMirCont1 = pvSphMirCont1->lvolume();
    mirrorCont.push_back( lvSphMirCont1 );
  }
  else
    mirrorCont.push_back( lvRich2Gas );

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

