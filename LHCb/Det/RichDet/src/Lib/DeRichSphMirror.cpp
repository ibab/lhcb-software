// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/RichDet/src/Lib/DeRichSphMirror.cpp,v 1.3 2002-11-11 12:34:11 cattanem Exp $
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

  StatusCode sc = StatusCode::SUCCESS;

  MsgStream log(msgSvc(), "DeRichSphMirror" );
  log << MSG::DEBUG <<"Start initialisation for DeRichSphMirror" << endreq;

  m_solid = geometry()->lvolume()->solid();

  //m_alignmentConstantX = userParameterAsDouble("AlignmentConstantX");
  //m_alignmentConstantY = userParameterAsDouble("AlignmentConstantY");
  m_radius = userParameterAsDouble("Radius");

  //HepRotateX3D alignX(m_alignmentConstantY);
  //HepRotateY3D alignY(-m_alignmentConstantX);

  //const ISolid* mysolid = geometry()->lvolume()->solid();
  //cout << mysolid->name()<< std::endl;  
  //m_radius = mySphere->insideRadius();
  //cout << "radius: " << m_radius << std::endl;
  

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

  // Now let's find the mirror centre.
  //HepPoint3D mirCentre(0.0, 0.0, m_radius);
  //m_mirrorCentre = geometry()->toGlobal(mirCentre);
  //cout << "Centre of mirror in global coord: "<< m_mirrorCentre<< std::endl;

  // extract mirror number from detector element name
  const std::string::size_type pos = name().find(':');
  if( std::string::npos != pos ) {
    m_mirrorNumber = atoi( name().substr(pos + 1).c_str() );
  }
  else {
    log << MSG::ERROR <<"A spherical mirror without a number!"<< endreq;
    sc = StatusCode::FAILURE;
  }

  log << MSG::DEBUG <<"End initialisation for DeRichSphMirror" << endreq;
  return sc;
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
