
/** @file DeRichFlatMirror.cpp
 *
 *  Implementation file for detector description class : DeRichFlatMirror
 *
 *  CVS Log :-
 *  $Id: DeRichFlatMirror.cpp,v 1.6 2004-09-01 15:20:19 papanest Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.5  2004/07/27 08:55:23  jonrob
 *  Add doxygen file documentation and CVS information
 *
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */

#define DERICHFLATMIRROR_CPP

// Include files
#include "GaudiKernel/MsgStream.h"
#include "Kernel/CLHEPStreams.h"

// CLHEP files
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Vector/Rotation.h"
#include "CLHEP/Geometry/Normal3D.h"

/// Detector description classes
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/SolidBox.h"

#include "RichDet/DeRichFlatMirror.h"

//----------------------------------------------------------------------------

const CLID& CLID_DeRichFlatMirror = 12031;  // User defined

// Standard Constructor
DeRichFlatMirror::DeRichFlatMirror()
  : m_alignmentConstantX(0.0), m_alignmentConstantY(0.0), m_mirrorNumber(-1)
{}

// Standard Destructor
DeRichFlatMirror::~DeRichFlatMirror() {}

// Retrieve Pointer to class defininition structure
const CLID& DeRichFlatMirror::classID() {
  return CLID_DeRichFlatMirror;
}


StatusCode DeRichFlatMirror::initialize() {

  StatusCode sc = StatusCode::SUCCESS;

  MsgStream log(msgSvc(), "DeRichFlatMirror" );
  log << MSG::DEBUG <<"Starting initialisation for DeRichFlatMirror"<< endreq;
  this->printOut(log);

  m_solid = geometry()->lvolume()->solid();

  m_alignmentConstantX = userParameterAsDouble("AlignmentConstantX");
  m_alignmentConstantY = userParameterAsDouble("AlignmentConstantY");

  HepRotateX3D alignX(-m_alignmentConstantY);
  HepRotateY3D alignY(m_alignmentConstantX);

  // get the z half length, to find the centre on the mirror surface
  // where reflection will take place
  const ISolid* mysolid = geometry()->lvolume()->solid();
  const SolidBox* myBox = dynamic_cast<const SolidBox*>(mysolid);
  double centreOnSurfaceZ = myBox->zHalfLength();

  HepPoint3D localCentre(0.0, 0.0, centreOnSurfaceZ);
  // in global coordinates
  m_mirrorCentre = geometry()->toGlobal(localCentre);

  HepNormal3D localNormal(0.0, 0.0, 1.0);

  // Rotate local normal dependig on the alignment constants
  // Rotating around X changes Y, while leaving X unchanged
  HepNormal3D alignedLocalNormal = localNormal;
  alignedLocalNormal.transform(alignY);
  //cout << "X alignment: " << alignedLocalNormal  << std::endl;

  alignedLocalNormal.transform(alignX);
  //cout << "Y alignment: " << alignedLocalNormal  << std::endl;

  // go back to the global coord system.
  HepTransform3D localToGlobal = geometry()->matrixInv();
  m_normalVector = alignedLocalNormal.transform(localToGlobal);
  //cout << "Normal in glob coord: " << m_normalVector << std::endl;


  // create the mirror plane
  HepPlane3D aPlane(m_normalVector, m_mirrorCentre);
  m_mirrorPlane = aPlane;
  //std::cout << "Mirror plane" << m_mirrorPlane << std::endl;

  // extract mirror number from detector element name
  const std::string::size_type pos = name().find(':');
  if( std::string::npos != pos ) {
    m_mirrorNumber = atoi( name().substr(pos + 1).c_str() );
  }
  else {
    log << MSG::FATAL <<"A mirror without a number!"<< endreq;
    sc = StatusCode::FAILURE;
  }
  
  log << MSG::DEBUG << "Mirror #" << m_mirrorNumber 
      <<" Centre (on reflective surface) " << m_mirrorCentre  << endmsg;
  log << MSG::DEBUG <<"Normal vector " << m_normalVector << endmsg;
  log << MSG::DEBUG <<"Finished initialisation for DeRichFlatMirror"<< endmsg;

  return sc;
}


//=========================================================================
//  intersection with solid
//=========================================================================

StatusCode DeRichFlatMirror:: intersects(const HepPoint3D& globalP,
                                         const HepVector3D& globalV)
{
  HepPoint3D pLocal = geometry()->toLocal(globalP);
  HepVector3D vLocal = globalV;
  vLocal.transform( geometry()->matrix() );

  ISolid::Ticks ticks;
  unsigned int noTicks = m_solid->intersectionTicks(pLocal, vLocal, ticks);

  if (0 == noTicks) {
    return StatusCode::FAILURE;
  }
  else {
    return StatusCode::SUCCESS;
  }

}
