
/** @file DeRichSphMirror.cpp
 *
 *  Implementation file for detector description class : DeRichSphMirror
 *
 *  CVS Log :-
 *  $Id: DeRichSphMirror.cpp,v 1.11 2004-10-20 16:16:36 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.10  2004/09/01 15:20:19  papanest
 *  added functions for TabProps
 *
 *  Revision 1.9  2004/07/27 08:55:23  jonrob
 *  Add doxygen file documentation and CVS information
 *
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */

#define DERICHSPHMIRROR_CPP

// Include files
#include "GaudiKernel/MsgStream.h"
#include "Kernel/CLHEPStreams.h"

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
DeRichSphMirror::DeRichSphMirror():
  m_alignmentConstantX(0.0), m_alignmentConstantY(0.0), m_mirrorNumber(-1)
{}

// Standard Destructor
DeRichSphMirror::~DeRichSphMirror() {}

// Retrieve Pointer to class defininition structure
const CLID& DeRichSphMirror::classID() {
  return CLID_DeRichSphMirror;
}


StatusCode DeRichSphMirror::initialize() {

  StatusCode fail = StatusCode::FAILURE;

  MsgStream log(msgSvc(), "DeRichSphMirror" );
  log << MSG::DEBUG <<"Start initialisation for DeRichSphMirror" << endreq;

  SmartDataPtr<DeRich> deRich2(dataSvc(), DeRichLocation::Rich2);

  m_solid = geometry()->lvolume()->solid();
  const std::string type = m_solid->typeName();

  double hexRadius;  
  if ( deRich2->hasParam("Rich2SphMirrorHexDiameter") ) 
    hexRadius = deRich2->userParameterAsDouble("Rich2SphMirrorHexDiameter")/2.0;
  else
    hexRadius = 510.0*mm;
  
  const double flatToCentre = hexRadius*sin(60*degree);

  const SolidSphere* sphereSolid = 0;
  // find the sphere of the spherical mirror
  if ( type == "SolidSphere" ) {
    sphereSolid = dynamic_cast<const SolidSphere*>(m_solid);
  } else {
    //assume bollean solid
    log << MSG::DEBUG << "About to do a dynamic_cast" << endreq;
    const SolidBoolean* compSolid = dynamic_cast<const SolidBoolean*>(m_solid);
    //    compSolid->printOut();

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
      log << MSG::FATAL << "Problem finding a sphere solid in:"<< endreq;
      compSolid->printOut();
      return fail;
    }      
  }
  
  // Now that we have the sphere, try to find out its radius
  m_radius = sphereSolid->insideRadius();

  // and its centre
  HepPoint3D zero(0.0, 0.0, 0.0);
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
    log << MSG::FATAL << "Problem getting mirror radius, noTicks:" 
        << sphTicksSize << endreq;
    return fail;
  }
  HepPoint3D localMirrorCentre(sphTicks[0]*toSphCentre);
  m_mirrorCentre = geometry()->toGlobal(localMirrorCentre);

  HepPoint3D middleRightSide(sqrt(m_radius*m_radius-flatToCentre*flatToCentre),
                             0.0,
                             flatToCentre);  
  HepPoint3D middleLeftSide(sqrt(m_radius*m_radius-flatToCentre*flatToCentre),
                             0.0,
                             -flatToCentre);

  //  m_alignmentConstantX = userParameterAsDouble("AlignmentConstantX");
  //  m_alignmentConstantY = userParameterAsDouble("AlignmentConstantY");
  //  m_radius = userParameterAsDouble("Radius");

  //  HepRotateX3D alignX(m_alignmentConstantY);
  //  HepRotateY3D alignY(-m_alignmentConstantX);


  // get the rotation and translation from local to global
  //HepRotation rotToGlobal = this->geometry()->matrixInv().getRotation();

  //  HepScale3D s;
  //  HepRotate3D rotToGlobal;
  //  HepTranslate3D translToGlobal;
  //  geometry()->matrixInv().getDecomposition(s, rotToGlobal, translToGlobal);
  
  // we need to vector that points from the centre of the mirror
  // segment to the centre of curvature.
  // Start with a vector that points back along the z
  //HepVector3D toCoC(0.0, 0.0, -m_radius);

  // and then rotate it.
  //toCoC.transform(rotToGlobal);
  //cout << "Rotated vector: " << toCoC << std::endl;

  // we now have the vector that points from the centre of the mirror
  // segment to the centre of curvature in the local (mirror) coordinate
  // system.  We need to rotate it dependig on the alignment constants
  // Rotating around X changes Y, while leaving X unchanged
  //HepVector3D aligned = toCoC;
  //aligned.transform(alignY);
  //cout << "X alignment: " << aligned  << std::endl;

  //aligned.transform(alignX);
  //cout << "Y alignment: " << aligned  << std::endl;

  // go back to the coornidate system where the CoC is at 0,0,0 (centre
  // of the sphere
  //HepVector3D centreVector = aligned - toCoC;
  //cout << "Offset from original CoC: " << centreVector << std::endl;

  // go back to the global coord system. Ignore the rotation, it has been
  // taken care of
  //m_centreOfCurvature = centreVector;
  m_centreOfCurvature = geometry()->toGlobal(zero);
  //cout << "CoC in global coord system: "<< m_centreOfCurvature<< std::endl;

  // extract mirror number from detector element name
  const std::string::size_type pos = name().find(':');
  if( std::string::npos != pos ) {
    m_mirrorNumber = atoi( name().substr(pos + 1).c_str() );
  }
  else {
    log << MSG::FATAL <<"A spherical mirror without a number!"<< endreq;
    return fail;
  }

  log << MSG::DEBUG << "Mirror #" << m_mirrorNumber << " Radius:" << m_radius 
      << " Centre of curvature " << m_centreOfCurvature << endreq;
  log << MSG::DEBUG << "Centre of mirror " << m_mirrorCentre << endreq;

  if( (m_mirrorNumber == 31) || (m_mirrorNumber < 28 && m_mirrorNumber != 0) ) 
    log << MSG::VERBOSE << "Right middle " 
        << geometry()->toGlobal(middleRightSide) << endreq;
  else
    log << MSG::VERBOSE << "Left middle " 
        << geometry()->toGlobal(middleLeftSide) << endreq;

  log << MSG::DEBUG <<"End initialisation for DeRichSphMirror" << endreq;
  return StatusCode::SUCCESS;
}

//=============================================================================

StatusCode DeRichSphMirror:: intersects(const HepPoint3D& globalP, 
                                        const HepVector3D& globalV,
                                        HepPoint3D& intersectionPoint) const
{
  const HepPoint3D pLocal( geometry()->toLocal(globalP) );
  HepVector3D vLocal( globalV );
  vLocal.transform( geometry()->matrix() );

  ISolid::Ticks ticks;
  const unsigned int noTicks = m_solid->intersectionTicks(pLocal, vLocal, ticks);
  
  if (0 == noTicks) {
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
                                        const HepVector3D& globalV) const
{
  //const HepPoint3D pLocal( geometry()->toLocal(globalP) );
  HepVector3D vLocal( globalV );
  vLocal.transform( geometry()->matrix() );

  ISolid::Ticks ticks;
  const unsigned int noTicks = m_solid->intersectionTicks(geometry()->toLocal(globalP), vLocal, ticks);
  
  return ( 0 == noTicks ? StatusCode::FAILURE : StatusCode::SUCCESS );
}
