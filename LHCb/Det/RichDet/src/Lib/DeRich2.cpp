// $Id: DeRich2.cpp,v 1.7 2003-11-22 18:40:51 jonesc Exp $
#define DERICH2_CPP

// Include files
#include "RichDet/DeRich2.h"
#include "RichDet/DeRichFlatMirror.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "Kernel/CLHEPStreams.h"
//-----------------------------------------------------------------------------
//
// Implementation of class :  DeRich2
//
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

StatusCode DeRich2::initialize() {
  StatusCode sc = StatusCode::SUCCESS;

  MsgStream log(msgSvc(), "DeRich2" );
  log << MSG::DEBUG <<"Starting initialisation for DeRich2"<< endreq;
  
  double nominalCoCX = userParameterAsDouble("Rich2NominalCoCX");
  double nominalCoCY = userParameterAsDouble("Rich2NominalCoCY");
  double nominalCoCZ = userParameterAsDouble("Rich2NominalCoCZ");

  m_nominalCentreOfCurvature = 
    HepPoint3D(nominalCoCX, nominalCoCY, nominalCoCZ);
  m_nominalCentreOfCurvatureRight =
    HepPoint3D(-nominalCoCX, nominalCoCY, nominalCoCZ);

  //  std::vector<double> nominalCoC = paramVector("Rich2NominalCoC");
  //  m_nominalCentreOfCurvature = 
  //    HepPoint3D( nominalCoC[0], nominalCoC[1], nominalCoC[2]);
  log << MSG::DEBUG << "Nominal centre of curvature" 
      << m_nominalCentreOfCurvature << endreq;

  double nominalNorX = userParameterAsDouble("Rich2NominalNorX");
  double nominalNorY = userParameterAsDouble("Rich2NominalNorY");
  double nominalNorZ = userParameterAsDouble("Rich2NominalNorZ");
  m_nominalNormal = HepVector3D(nominalNorX, nominalNorY, nominalNorZ);
  m_nominalNormalRight = HepVector3D(-nominalNorX, nominalNorY, nominalNorZ);

  m_sphMirrorRadius = userParameterAsDouble("Rich2SphMirrorRadius");
  m_sphMirrorMaxX = userParameterAsDouble("Rich2SphMirrorMaxX");
  m_sphMirrorMaxY = userParameterAsDouble("Rich2SphMirrorMaxY");

  double d = userParameterAsDouble("Rich2DParam");
  m_nominalPlaneLeft = HepPlane3D(nominalNorX, nominalNorY, nominalNorZ, d);
  m_nominalPlaneRight =HepPlane3D(-nominalNorX, nominalNorY, nominalNorZ, d);

// get the parameters of the nominal flat mirror plane in the form
// Ax+By+Cz+D=0
// std::vector<double> nominalFMirrorPlane =
//   paramVector("Rich2NominalFlatMirrorPlane");
//m_nominalPlaneLeft=HepPlane3D(nominalFMirrorPlane[0],nominalFMirrorPlane[1],
//                              nominalFMirrorPlane[2],nominalFMirrorPlane[3]);
//  m_nominalNormal = m_nominalPlaneLeft.normal();
//  m_nominalPlaneRight=
//  HepPlane3D(-nominalFMirrorPlane[0],nominalFMirrorPlane[1],
//             nominalFMirrorPlane[2],nominalFMirrorPlane[3]);
  
  m_nominalPlaneLeft.normalize();
  m_nominalPlaneRight.normalize();

  log << MSG::DEBUG <<"Finished initialisation for DeRich2"<< endreq;
  return sc;
}
