
/** @file DeRichFlatMirror.cpp
 *
 *  Implementation file for detector description class : DeRichFlatMirror
 *
 *  $Id: DeRichFlatMirror.cpp,v 1.9 2005-02-25 23:28:54 jonrob Exp $
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */

#define DERICHFLATMIRROR_CPP

// Include files
#include "GaudiKernel/MsgStream.h"

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


StatusCode DeRichFlatMirror::initialize() 
{

  const std::string::size_type pos2 = name().find("Rich");
  m_name = ( std::string::npos != pos2 ? name().substr(pos2) : 
             "DeRichFlatMirror_NO_NAME" );

  StatusCode sc = StatusCode::SUCCESS;

  MsgStream msg( msgSvc(), myName() );
  msg << MSG::DEBUG << "Initializing flat mirror" << endreq;
  //this->printOut(msg);

  m_solid = geometry()->lvolume()->solid();

  m_alignmentConstantX = userParameterAsDouble( "AlignmentConstantX" );
  m_alignmentConstantY = userParameterAsDouble( "AlignmentConstantY" );

  const HepRotateX3D alignX(-m_alignmentConstantY);
  const HepRotateY3D alignY(m_alignmentConstantX);

  // get the z half length, to find the centre on the mirror surface
  // where reflection will take place
  const ISolid* mysolid = geometry()->lvolume()->solid();
  const SolidBox* myBox = dynamic_cast<const SolidBox*>(mysolid);
  const double centreOnSurfaceZ = myBox->zHalfLength();

  HepPoint3D localCentre(0.0, 0.0, centreOnSurfaceZ);
  // in global coordinates
  m_mirrorCentre = geometry()->toGlobal(localCentre);

  HepNormal3D localNormal(0.0, 0.0, 1.0);

  // Rotate local normal dependig on the alignment constants
  // Rotating around X changes Y, while leaving X unchanged
  HepNormal3D alignedLocalNormal = localNormal;
  alignedLocalNormal.transform(alignY);
  alignedLocalNormal.transform(alignX);

  // go back to the global coord system.
  const HepTransform3D localToGlobal = geometry()->matrixInv();
  m_normalVector = alignedLocalNormal.transform(localToGlobal);

  // create the mirror plane
  m_mirrorPlane = HepPlane3D(m_normalVector, m_mirrorCentre);

  // extract mirror number from detector element name
  const std::string::size_type pos = name().find(':');
  if( std::string::npos != pos ) {
    m_mirrorNumber = atoi( name().substr(pos + 1).c_str() );
  }
  else {
    msg << MSG::FATAL << "A mirror without a number!" << endreq;
    sc = StatusCode::FAILURE;
  }
  
  msg << MSG::DEBUG << "Mirror #" << m_mirrorNumber 
      << " Centre (on reflective surface) " << m_mirrorCentre  << endmsg;
  msg << MSG::DEBUG <<"Normal vector " << m_normalVector << endmsg;
  msg << MSG::DEBUG <<"Finished initialisation for DeRichFlatMirror"<< endmsg;

  return sc;
}


//=========================================================================
//  intersection with solid
//=========================================================================

StatusCode DeRichFlatMirror::intersects( const HepPoint3D& globalP,
                                         const HepVector3D& globalV) const
{
  HepVector3D vLocal = globalV;
  vLocal.transform( geometry()->matrix() );

  ISolid::Ticks ticks;
  const unsigned int noTicks = 
    m_solid->intersectionTicks(geometry()->toLocal(globalP), vLocal, ticks);

  return ( 0 == noTicks ? StatusCode::FAILURE : StatusCode::SUCCESS );
}
