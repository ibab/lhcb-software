
/** @file DeRich2.cpp
 *
 *  Implementation file for detector description class : DeRich2
 *
 *  $Id: DeRich2.cpp,v 1.14 2005-02-09 13:39:26 cattanem Exp $
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */

#define DERICH2_CPP

// Include files
#include "RichDet/DeRich2.h"
#include "RichDet/DeRichFlatMirror.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"

// DetDesc
#include "DetDesc/Material.h"
#include "DetDesc/IGeometryInfo.h"

//-----------------------------------------------------------------------------

const CLID& CLID_DERich2 = 12002;  // User defined

// Standard Constructors
DeRich2::DeRich2() {}

// Standard Destructor
DeRich2::~DeRich2() {}

// Retrieve Pointer to class defininition structure
const CLID& DeRich2::classID() {
  return CLID_DERich2;
}

//===========================================================================

StatusCode DeRich2::initialize()
{

  MsgStream log(msgSvc(), "DeRich2" );
  log << MSG::DEBUG <<"Starting initialisation for DeRich2"<< endmsg;

  StatusCode sc = StatusCode::SUCCESS;
  StatusCode fail = StatusCode::FAILURE;

  if ( !DeRich::initialize() ) return fail;

  std::vector<double> nominalCoC = paramVector("Rich2NominalCoC");
  m_nominalCentreOfCurvature =
    HepPoint3D( nominalCoC[0], nominalCoC[1], nominalCoC[2]);
  m_nominalCentreOfCurvatureRight =
    HepPoint3D( -nominalCoC[0], nominalCoC[1], nominalCoC[2]);

  log << MSG::DEBUG << "Nominal centre of curvature"
      << m_nominalCentreOfCurvature << " ," << m_nominalCentreOfCurvatureRight
      << endmsg;

  m_sphMirrorRadius = paramAsDouble("Rich2SphMirrorRadius");

  // get the parameters of the nominal flat mirror plane in the form
  // Ax+By+Cz+D=0
  std::vector<double> nominalFMirrorPlane = paramVector("Rich2NominalFlatMirrorPlane");
  m_nominalPlaneLeft = HepPlane3D(nominalFMirrorPlane[0],nominalFMirrorPlane[1],
                                  nominalFMirrorPlane[2],nominalFMirrorPlane[3]);
  m_nominalPlaneRight = HepPlane3D(-nominalFMirrorPlane[0],nominalFMirrorPlane[1],
                                   nominalFMirrorPlane[2],nominalFMirrorPlane[3]);

  m_nominalPlaneLeft.normalize();
  m_nominalPlaneRight.normalize();
  m_nominalNormal = m_nominalPlaneLeft.normal();
  m_nominalNormalRight = m_nominalPlaneRight.normal();

  log << MSG::DEBUG << "Nominal normal " << HepVector3D( m_nominalNormal )
      << HepVector3D( m_nominalNormalRight ) << endmsg;

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
    log << MSG::ERROR << "Could not find gas window properties" << endmsg;
    return fail;
  }

  // get the nominal reflectivity of the spherical mirror
  std::string sphMirrorReflLoc = "/dd/Geometry/Rich2/Rich2SurfaceTabProperties/Rich2Mirror1SurfaceIdealReflectivityPT";
  SmartDataPtr<TabulatedProperty> sphMirrorRefl( dataSvc(), sphMirrorReflLoc );
  if ( !sphMirrorRefl )
    log << MSG::ERROR << "No info on spherical mirror reflectivity" << endmsg;
  else {
    m_nominalSphMirrorRefl = sphMirrorRefl;
    log << MSG::DEBUG << "Loaded spherical mirror reflectivity from:"
        << sphMirrorReflLoc << endmsg;
  }

  // get the nominal reflectivity of the flat mirror
  std::string flatMirrorReflLoc = "/dd/Geometry/Rich2/Rich2SurfaceTabProperties/Rich2Mirror2SurfaceIdealReflectivityPT";
  SmartDataPtr<TabulatedProperty> flatMirrorRefl(dataSvc(),flatMirrorReflLoc);
  if ( !flatMirrorRefl )
    log << MSG::ERROR << "No info on flat mirror reflectivity" << endmsg;
  else {
    m_nominalFlatMirrorRefl = flatMirrorRefl;
    log << MSG::DEBUG << "Loaded flat mirror reflectivity from:"
        << flatMirrorReflLoc << endmsg;
  }

  log << MSG::DEBUG <<"Finished initialisation for DeRich2"<< endmsg;
  return sc;
}

const HepPoint3D & DeRich2::nominalCentreOfCurvature(const Rich::Side side) const
{
  return ( Rich::right == side ? m_nominalCentreOfCurvatureRight :
           m_nominalCentreOfCurvature );
}

const HepNormal3D & DeRich2::nominalNormal(const Rich::Side side) const
{
  return ( Rich::right == side ? m_nominalNormalRight : m_nominalNormal );
}

const HepPlane3D & DeRich2::nominalPlane(const Rich::Side side) const
{
  return ( Rich::left == side ? m_nominalPlaneLeft : m_nominalPlaneRight );
}

Rich::Side DeRich2::side( const HepPoint3D & point ) const
{
  return ( point.x() >= 0.0 ? Rich::left : Rich::right );
}
