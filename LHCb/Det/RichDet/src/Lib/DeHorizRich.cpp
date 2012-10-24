/** @file DeHorizRich.cpp
 *
 *  Implementation file for detector description class : DeHorizRich
 *
 *  @author Antonis Papanestis antonis.papanestis@stfc.ac.uk
 *  @date   2012-10-18
 */

#define DERICH2_CPP

// Include files
#include "RichDet/DeHorizRich.h"
#include "RichDet/DeRichHPDPanel.h"
#include "RichDet/DeRichRadiator.h"

// Gaudi
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IUpdateManagerSvc.h"

// DetDesc
#include "DetDesc/Material.h"
#include "DetDesc/IGeometryInfo.h"

//-----------------------------------------------------------------------------

const CLID CLID_DEHorizRich = 12003;  // User defined

// Standard Constructors
DeHorizRich::DeHorizRich(const std::string & name)
  : DeRich ( name )
{
  setMyName("DeHorizRich");
}

// Standard Destructor
DeHorizRich::~DeHorizRich() { }

// Retrieve Pointer to class defininition structure
const CLID& DeHorizRich::classID()
{
  return CLID_DEHorizRich;
}

//=========================================================================
//  initialize
//=========================================================================
StatusCode DeHorizRich::initialize()
{
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Initialize " << name() << endmsg;

  if ( !DeRich::initialize() ) return StatusCode::FAILURE;

  const std::vector<double>& nominalCoC = param<std::vector<double> >("NominalSphMirrorCoC");
  m_nominalCentreOfCurvatureLeft  =
    Gaudi::XYZPoint(  nominalCoC[0], nominalCoC[1], nominalCoC[2] );
  m_nominalCentreOfCurvatureRight =
    Gaudi::XYZPoint( -nominalCoC[0], nominalCoC[1], nominalCoC[2] );

  m_sphMirrorRadius = param<double>("SphMirrorRadius");

  // get the parameters of the nominal flat mirror plane in the form
  // Ax+By+Cz+D=0
  std::vector<double> nominalFMirrorPlane;
  nominalFMirrorPlane = param<std::vector<double> >("NominalSecMirrorPlane");

  m_nominalPlaneLeft = Gaudi::Plane3D(nominalFMirrorPlane[0],nominalFMirrorPlane[1],
                                      nominalFMirrorPlane[2],nominalFMirrorPlane[3]);
  m_nominalPlaneRight = Gaudi::Plane3D(-nominalFMirrorPlane[0],nominalFMirrorPlane[1],
                                       nominalFMirrorPlane[2],nominalFMirrorPlane[3]);

  m_nominalNormalLeft  = m_nominalPlaneLeft.Normal();
  m_nominalNormalRight = m_nominalPlaneRight.Normal();

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Spherical Mirror radius:" << m_sphMirrorRadius << endmsg;
    debug() << "Nominal centre of curvature" << m_nominalCentreOfCurvatureLeft
            << " , " << m_nominalCentreOfCurvatureRight << endmsg;

    debug() << "Nominal normal " << Gaudi::XYZVector( m_nominalNormalLeft ) << " "
            << Gaudi::XYZVector( m_nominalNormalRight ) << endmsg;
  }

  // get the refractive index of the window between gas radiator and photon detectors
  const std::string gasWinRefIndxLoc = param<std::string>("RichGasWindowRefIndexLoc");
  SmartDataPtr<TabulatedProperty> gasWinRefIndxTab( dataSvc(), gasWinRefIndxLoc );
  if ( gasWinRefIndxTab )
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << "Loaded gas win ref index from: "<<gasWinRefIndxLoc<<endmsg;
    m_gasWinRefIndex = new RichTabulatedProperty1D( gasWinRefIndxTab );
    if ( !m_gasWinRefIndex->valid() )
    {
      error()
        << "Invalid RINDEX RichTabulatedProperty1D for " << gasWinRefIndxTab->name() << endmsg;
      return StatusCode::FAILURE;
    }
  }
  else
  {
    error() << "No info on gas window ref index at " << gasWinRefIndxLoc << endmsg;
    return StatusCode::FAILURE;
  }

  // get the absorption length of the window between gas radiator and photon detectors
  const std::string gasWinAbsLenLoc = param<std::string>("RichGasWindowRefIndexLoc");
  SmartDataPtr<TabulatedProperty> gasWinAbsLenTab( dataSvc(), gasWinAbsLenLoc );
  if ( gasWinAbsLenTab )
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << "Loaded gas win abs length from: "<<gasWinAbsLenLoc<<endmsg;
    m_gasWinRefIndex = new RichTabulatedProperty1D( gasWinAbsLenTab );
    if ( !m_gasWinRefIndex->valid() )
    {
      error() << "Invalid ABSLENGTH RichTabulatedProperty1D for " << gasWinAbsLenTab->name() << endmsg;
      return StatusCode::FAILURE;
    }
  }
  else
  {
    error() << "No info on gas win abs length at " << gasWinAbsLenLoc << endmsg;
    return StatusCode::FAILURE;
  }


  // get the nominal reflectivity of the spherical mirror
  const std::string sphMirrorReflLoc = param<std::string>("NominalSphericalMirrorReflectivityLoc");
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
    m_nominalSphMirrorRefl = new RichTabulatedProperty1D( sphMirrorRefl );
    if ( !m_nominalSphMirrorRefl->valid() )
    {
      error()<<"Invalid RichTabulatedProperty1D for "<<sphMirrorRefl->name()<<endmsg;
      return StatusCode::FAILURE;
    }
  }

  // get the nominal reflectivity of the secondary mirror
  const std::string secMirrorReflLoc = param<std::string>("NominalSecondaryMirrorReflectivityLoc");

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
    m_nominalSecMirrorRefl = new RichTabulatedProperty1D( secMirrorRefl );
    if ( !m_nominalSecMirrorRefl->valid() )
    {
      error()
        << "Invalid RichTabulatedProperty1D for " << secMirrorRefl->name() << endmsg;
      return StatusCode::FAILURE;
    }
  }

  // initialize RichRadiators
  std::vector<std::string> radLocs = paramVect<std::string>("RadiatorDetElemLocations");
  for (unsigned int rad=0; rad<radLocs.size(); rad++)
  {
    SmartDataPtr<DeRichRadiator> richRad(dataSvc(),radLocs[rad]);
    if ( !richRad )
      error() << "Cannot initialize Rich Radiator:" << radLocs[rad] << endmsg;
  }

  return StatusCode::SUCCESS;
}


//=========================================================================
//  nominalCentreOfCurvature
//=========================================================================
const Gaudi::XYZPoint&
DeHorizRich::nominalCentreOfCurvature(const Rich::Side side) const
{
  return ( Rich::right == side ?
           m_nominalCentreOfCurvatureRight :
           m_nominalCentreOfCurvatureLeft );
}

//=========================================================================
//  nominalNormal
//=========================================================================
const Gaudi::XYZVector& DeHorizRich::nominalNormal(const Rich::Side side) const
{
  return ( Rich::right == side ? m_nominalNormalRight : m_nominalNormalLeft );
}

//=========================================================================
//  nominalPlane
//=========================================================================
const Gaudi::Plane3D& DeHorizRich::nominalPlane(const Rich::Side side) const
{
  return ( Rich::left == side ? m_nominalPlaneLeft : m_nominalPlaneRight );
}

//=========================================================================
//  side
//=========================================================================
Rich::Side DeHorizRich::side( const Gaudi::XYZPoint& point ) const
{
  return ( point.x() >= 0.0 ? Rich::left : Rich::right );
}

//=========================================================================
// Access the name for a given panel
//=========================================================================
const std::string DeHorizRich::panelName( const Rich::Side panel ) const
{

  const std::vector<std::string>& panelLoc
    = paramVect<std::string>("PDPanelDetElemLocations");
  const std::string pname = panelLoc[panel];

  return pname;
}



