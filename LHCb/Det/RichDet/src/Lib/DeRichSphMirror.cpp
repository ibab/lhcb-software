
/** @file DeRichSphMirror.cpp
 *
 *  Implementation file for detector description class : DeRichSphMirror
 *
 *  $Id: DeRichSphMirror.cpp,v 1.13 2005-02-25 23:28:54 jonrob Exp $
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */

#define DERICHSPHMIRROR_CPP

// Include files
#include "GaudiKernel/MsgStream.h"

#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Units/SystemOfUnits.h"

/// Detector description classes
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/SolidSphere.h"
#include "DetDesc/SolidBoolean.h"

#include "RichDet/DeRichSphMirror.h"
#include "RichDet/DeRich.h"

//======================================================================

const CLID& CLID_DeRichSphMirror = 12030;  // User defined

// Standard Constructor
DeRichSphMirror::DeRichSphMirror()
  : m_alignmentConstantX(0.0),
    m_alignmentConstantY(0.0),
    m_mirrorNumber(-1)
{}

// Standard Destructor
DeRichSphMirror::~DeRichSphMirror() {}

// Retrieve Pointer to class defininition structure
const CLID& DeRichSphMirror::classID()
{
  return CLID_DeRichSphMirror;
}

StatusCode DeRichSphMirror::initialize()
{

  const std::string::size_type pos = name().find("Rich");
  m_name = ( std::string::npos != pos ? name().substr(pos) : 
             "DeRichSphMirror_NO_NAME" );

  MsgStream msg( msgSvc(), myName() );
  msg << MSG::DEBUG << "Initializing spherical mirror" << endreq;
  //this->printOut(msg);

  SmartDataPtr<DeRich> deRich2(dataSvc(), DeRichLocation::Rich2);

  m_solid = geometry()->lvolume()->solid();
  const std::string type = m_solid->typeName();

  double hexRadius = 510.0*mm;
  if ( deRich2->hasParam("Rich2SphMirrorHexDiameter") )
    hexRadius = deRich2->userParameterAsDouble("Rich2SphMirrorHexDiameter")/2.0;

  const double flatToCentre = hexRadius*sin(60*degree);

  const SolidSphere* sphereSolid = 0;
  // find the sphere of the spherical mirror
  if ( type == "SolidSphere" ) {
    sphereSolid = dynamic_cast<const SolidSphere*>(m_solid);
  } else {
    //assume bollean solid
    const SolidBoolean* compSolid = dynamic_cast<const SolidBoolean*>(m_solid);
    //compSolid->printOut();

    if ( compSolid->first()->typeName() == "SolidSphere") {
      sphereSolid = dynamic_cast<const SolidSphere*>(compSolid->first());
    }  else {
      SolidBoolean::SolidChildrens components;
      SolidBoolean::SolidChildrens::const_iterator iter;
      for (iter=compSolid->childBegin(); iter!=compSolid->childEnd(); ++iter) {
        if ( (*iter)->solid()->typeName() == "SolidSphere")
          sphereSolid = dynamic_cast<const SolidSphere*>((*iter)->solid());

      }
    }
    if ( !sphereSolid) {
      msg << MSG::FATAL << "Problem finding a sphere solid in :- " << endreq;
      compSolid->printOut();
      return StatusCode::FAILURE;
    }
  }

  // Now that we have the sphere, try to find out its radius
  m_radius = sphereSolid->insideRadius();

  // and its centre
  const HepPoint3D zero(0.0, 0.0, 0.0);
  HepVector3D toSphCentre(0.0, 0.0, 1.0);
  if ( 0.0 != sphereSolid->startThetaAngle() ) {
    toSphCentre.setTheta( sphereSolid->startThetaAngle() +
                          sphereSolid->deltaThetaAngle()/2.0 );
    toSphCentre.setPhi( sphereSolid->startPhiAngle() +
                        sphereSolid->deltaPhiAngle()/2.0 );
  }


  ISolid::Ticks sphTicks;
  unsigned int sphTicksSize = sphereSolid->
    intersectionTicks(zero, toSphCentre, sphTicks);
  if (sphTicksSize != 2) {
    msg << MSG::FATAL << "Problem getting mirror radius, noTicks: "
        << sphTicksSize << endreq;
    return StatusCode::FAILURE;
  }
  const HepPoint3D localMirrorCentre(sphTicks[0]*toSphCentre);
  m_mirrorCentre = geometry()->toGlobal(localMirrorCentre);

  const HepPoint3D middleRightSide(sqrt(m_radius*m_radius-flatToCentre*flatToCentre),
                                   0.0,
                                   flatToCentre);
  const HepPoint3D middleLeftSide(sqrt(m_radius*m_radius-flatToCentre*flatToCentre),
                                  0.0,
                                  -flatToCentre);

  // go back to the global coord system.
  // Ignore the rotation, it has been taken care of
  m_centreOfCurvature = geometry()->toGlobal(zero);

  // extract mirror number from detector element name
  const std::string::size_type pos2 = name().find(':');
  if ( std::string::npos != pos2 ) {
    m_mirrorNumber = atoi( name().substr(pos2+1).c_str() );
  }
  else {
    msg << MSG::FATAL << "A spherical mirror without a number!" << endreq;
    return StatusCode::FAILURE;
  }

  msg << MSG::DEBUG << "Mirror #" << m_mirrorNumber << " Radius:" << m_radius
      << " Centre of curvature " << m_centreOfCurvature << endreq;
  msg << MSG::DEBUG << "Centre of mirror " << m_mirrorCentre << endreq;

  if( (m_mirrorNumber == 31) || (m_mirrorNumber < 28 && m_mirrorNumber != 0) )
    msg << MSG::VERBOSE << "Right middle "
        << geometry()->toGlobal(middleRightSide) << endreq;
  else
    msg << MSG::VERBOSE << "Left middle "
        << geometry()->toGlobal(middleLeftSide) << endreq;

  msg << MSG::DEBUG <<"End initialisation for DeRichSphMirror" << endreq;
  return StatusCode::SUCCESS;
}

//=============================================================================

StatusCode DeRichSphMirror:: intersects(const HepPoint3D& globalP,
                                        const HepVector3D& globalV,
                                        HepPoint3D& intersectionPoint ) const
{
  const HepPoint3D pLocal( geometry()->toLocal(globalP) );
  HepVector3D vLocal( globalV );
  vLocal.transform( geometry()->matrix() );

  ISolid::Ticks ticks;
  const unsigned int noTicks = m_solid->intersectionTicks(pLocal, vLocal, ticks);

  if ( 0 == noTicks ) {
    return StatusCode::FAILURE;
  }
  else {
    const HepPoint3D tempPointLocal = pLocal + ticks[0] * vLocal;
    intersectionPoint = geometry()->toGlobal(tempPointLocal);
    return StatusCode::SUCCESS;
  }

}

//=============================================================================

StatusCode DeRichSphMirror::intersects( const HepPoint3D& globalP,
                                        const HepVector3D& globalV ) const
{
  HepVector3D vLocal( globalV );
  vLocal.transform( geometry()->matrix() );

  ISolid::Ticks ticks;
  const unsigned int noTicks = m_solid->intersectionTicks(geometry()->toLocal(globalP), vLocal, ticks);

  return ( 0 == noTicks ? StatusCode::FAILURE : StatusCode::SUCCESS );
}
