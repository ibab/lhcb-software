
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

// Gaudi
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IUpdateManagerSvc.h"

// DetDesc
#include "DetDesc/Material.h"
#include "DetDesc/IGeometryInfo.h"

//-----------------------------------------------------------------------------

const CLID CLID_DERich1 = 12001;  // User defined

// Standard Constructors
DeRich1::DeRich1(const std::string & name)
  : DeRich ( name )
{ 
  setMyName("DeRich1"); 
}

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
  debug() << "Initialize " << name() << endmsg;

  if ( !DeRich::initialize() ) return StatusCode::FAILURE;

  const std::vector<double>& nominalCoC = param<std::vector<double> >("NominalSphMirrorCoC");
  m_nominalCentreOfCurvatureTop    =
    Gaudi::XYZPoint( nominalCoC[0],  nominalCoC[1], nominalCoC[2] );
  m_nominalCentreOfCurvatureBottom =
    Gaudi::XYZPoint( nominalCoC[0], -nominalCoC[1], nominalCoC[2] );

  debug() << "Nominal centre of curvature"
          << m_nominalCentreOfCurvatureTop << " , " << m_nominalCentreOfCurvatureBottom
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

  m_nominalNormalTop    = m_nominalPlaneTop.Normal();
  m_nominalNormalBottom = m_nominalPlaneBottom.Normal();

  debug() << "Nominal normal " << Gaudi::XYZVector( m_nominalNormalTop ) << " "
          << Gaudi::XYZVector( m_nominalNormalBottom ) << endmsg;

  const IPVolume* pvGasWindow( 0 );
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
    Material::Tables::const_iterator matIter;
    for (matIter=quartzWinTabProps.begin(); matIter!=quartzWinTabProps.end(); ++matIter) 
    {
      if( (*matIter) )
      {
        if ( (*matIter)->type() == "RINDEX" )
        {
          debug() << "Loaded gas window refIndex from: " << (*matIter)->name()
                  << endmsg;
          m_gasWinRefIndex = new RichTabulatedProperty1D( *matIter );
          if ( !m_gasWinRefIndex->valid() )
          {
            error() << "Invalid RINDEX RichTabulatedProperty1D for " << (*matIter)->name() << endmsg;
            return StatusCode::FAILURE;
          }
        }
        if ( (*matIter)->type() == "ABSLENGTH" )
        {
          m_gasWinAbsLength = new RichTabulatedProperty1D( *matIter );
          if ( !m_gasWinAbsLength->valid() )
          {
            error() << "Invalid ABSLENGTH RichTabulatedProperty1D for " << (*matIter)->name() << endmsg;
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
    debug() << "Loaded spherical mirror reflectivity from: "
            << sphMirrorReflLoc << endmsg;
    m_nominalSphMirrorRefl = new RichTabulatedProperty1D( sphMirrorRefl );
    if ( !m_nominalSphMirrorRefl->valid() )
    {
      error() <<"Invalid RichTabulatedProperty1D for " << sphMirrorRefl->name() << endmsg;
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
    debug() <<"Loaded secondary mirror reflectivity from: "<<secMirrorReflLoc<<endmsg;
    m_nominalSecMirrorRefl = new RichTabulatedProperty1D( secMirrorRefl );
    if ( !m_nominalSecMirrorRefl->valid() )
    {
      error() << "Invalid RichTabulatedProperty1D for " << secMirrorRefl->name() << endmsg;
      return StatusCode::FAILURE;
    }
  }

  // Force loading of the HPD panels now
  //hpdPanel(Rich::top);
  //hpdPanel(Rich::bottom);

  // DC06 compatible mirror (mis)alignment
  bool alignMirros( false );
  if ( hasCondition( "Rich1Mirror1Align" ) )
  {
    m_sphMirAlignCond = condition( "Rich1Mirror1Align" );
    alignMirros = true;
    updMgrSvc()->registerCondition(this,m_sphMirAlignCond.path(),&DeRich1::alignSphMirrors);
  }
  else 
  {
    m_sphMirAlignCond = NULL;
  }

  if ( hasCondition( "Rich1Mirror2Align" ) )
  {
    m_secMirAlignCond = condition( "Rich1Mirror2Align" );
    alignMirros = true;
    updMgrSvc()->registerCondition(this,m_secMirAlignCond.path(),&DeRich1::alignSecMirrors);
  }
  else {
    m_secMirAlignCond = NULL;
  }

  const StatusCode upsc = 
    ( alignMirros ? updMgrSvc()->update(this) : StatusCode::SUCCESS );

  // initialize all child detector elements. This triggers the update
  // of the radiator properties
  childIDetectorElements();

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
  if ( exists("HPDPanelDetElemLocations") )
  {
    const std::vector<std::string>& panelLoc 
      = paramVect<std::string>("HPDPanelDetElemLocations");
    pname = panelLoc[panel];
  }
  return pname;
}
