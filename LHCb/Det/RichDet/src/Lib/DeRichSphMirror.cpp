// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/RichDet/src/Lib/DeRichSphMirror.cpp,v 1.4 2003-10-22 10:48:29 papanest Exp $
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


//#include "RichDet/DeRichSphMirror.h"
#include "RichDet/DeRichSphMirror.h"


//------------------------------------------------------------------------
//
// Implementation of class :  DeRichSphMirror
//
//--------------------------------------------------------------------------

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

  m_solid = geometry()->lvolume()->solid();
  std::string type = m_solid->typeName();

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

  //m_alignmentConstantX = userParameterAsDouble("AlignmentConstantX");
  //m_alignmentConstantY = userParameterAsDouble("AlignmentConstantY");
  //  m_radius = userParameterAsDouble("Radius");

  //HepRotateX3D alignX(m_alignmentConstantY);
  //HepRotateY3D alignY(-m_alignmentConstantX);


  // get the rotation and translation from local to global
  //HepRotation rotToGlobal = this->geometry()->matrixInv().getRotation();

  HepScale3D s;
  HepRotate3D rotToGlobal;
  HepTranslate3D translToGlobal;
  geometry()->matrixInv().getDecomposition(s, rotToGlobal, translToGlobal);
  
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
  m_centreOfCurvature = HepPoint3D(0.0, 0.0, 0.0);
  m_centreOfCurvature.transform(translToGlobal);
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
      << " Centre of curvature "
      << m_centreOfCurvature << " Centre of mirror " << m_mirrorCentre 
      << endreq;

  log << MSG::DEBUG <<"End initialisation for DeRichSphMirror" << endreq;
  return StatusCode::SUCCESS;
}

//=============================================================================

StatusCode DeRichSphMirror:: intersects(const HepPoint3D& globalP, 
                                        const HepVector3D& globalV,
                                        HepPoint3D& intersectionPoint)
{
  HepPoint3D pLocal = geometry()->toLocal(globalP);
  const HepTransform3D vTrans = geometry()->matrix();
  HepVector3D vLocal = globalV;
  vLocal.transform(vTrans);

  ISolid::Ticks ticks;
  unsigned int noTicks = m_solid->intersectionTicks(pLocal, vLocal, ticks);
  
  if (0 == noTicks) {
    return StatusCode::FAILURE;
  }
  else {
    HepPoint3D tempPointLocal = pLocal + ticks[0] * vLocal;
    intersectionPoint = geometry()->toGlobal(tempPointLocal);
    return StatusCode::SUCCESS;  
  }
  
}

//=============================================================================

StatusCode DeRichSphMirror:: intersects(const HepPoint3D& globalP, 
                                        const HepVector3D& globalV)
{
  HepPoint3D pLocal = geometry()->toLocal(globalP);
  const HepTransform3D vTrans = geometry()->matrix();
  HepVector3D vLocal = globalV;
  vLocal.transform(vTrans);

  ISolid::Ticks ticks;
  unsigned int noTicks = m_solid->intersectionTicks(pLocal, vLocal, ticks);
  
  if (0 == noTicks) {
    return StatusCode::FAILURE;
  }
  else {
    return StatusCode::SUCCESS;  
  }
  
}
