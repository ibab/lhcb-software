// $Id: DeRich1.cpp,v 1.6 2003-11-22 18:40:51 jonesc Exp $
#define DERICH1_CPP

// Include files
#include "RichDet/DeRich1.h"
#include "RichDet/DeRichFlatMirror.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"
//------------------------------------------------------------------------------
//
// Implementation of class :  DeRich1
//
//------------------------------------------------------------------------------

const CLID& CLID_DERich1 = 12001;  // User defined

// Standard Constructors
DeRich1::DeRich1() {}
  
// Standard Destructor
DeRich1::~DeRich1() {}

// Retrieve Pointer to class defininition structure
const CLID& DeRich1::classID() {
  return CLID_DERich1;
}

//===========================================================================

StatusCode DeRich1::initialize() {
  StatusCode sc = StatusCode::SUCCESS;

  MsgStream log(msgSvc(), "DeRich1" );
  log << MSG::DEBUG << "Starting initialisation for DeRich1" << endreq;
  
  double nominalCoCX = userParameterAsDouble("Rich1Mirror1NominalCoCX");
  double nominalCoCY = userParameterAsDouble("Rich1Mirror1NominalCoCY");
  double nominalCoCZ = userParameterAsDouble("Rich1Mirror1NominalCoCZ");

  m_nominalCentreOfCurvature = 
    HepPoint3D(nominalCoCX, nominalCoCY, nominalCoCZ);
  m_nominalCentreOfCurvatureBottom = 
    HepPoint3D(nominalCoCX, -nominalCoCY, nominalCoCZ);

  double nominalNorX = userParameterAsDouble("Rich1NominalNorX");
  double nominalNorY = userParameterAsDouble("Rich1NominalNorY");
  double nominalNorZ = userParameterAsDouble("Rich1NominalNorZ");
  
  m_nominalNormal = HepVector3D(nominalNorX, nominalNorY, nominalNorZ);
  m_nominalNormalBottom = HepVector3D(nominalNorX, -nominalNorY, nominalNorZ);

  m_sphMirrorRadius = userParameterAsDouble("Rich1SphMirror1Radius");
  m_sphMirrorMaxX = userParameterAsDouble("Rich1SphMirror1MaxX");
  m_sphMirrorMaxY = userParameterAsDouble("Rich1SphMirror1MaxY");

  double d = userParameterAsDouble("Rich1DParam");
  
  m_nominalPlaneTop = HepPlane3D(nominalNorX, nominalNorY, nominalNorZ, d);
  m_nominalPlaneBottom = HepPlane3D(nominalNorX, -nominalNorY, nominalNorZ, d);
 
  m_nominalPlaneTop.normalize();
  m_nominalPlaneBottom.normalize();

  log << MSG::DEBUG << "Finished initialisation for DeRich1" << endreq;
  return sc;
}
