// $Id: DeRich1.cpp,v 1.1 2002-10-30 11:36:56 papanest Exp $
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
  log << MSG::DEBUG <<"Starting initialisation for DeRich1"<< endreq;
  
  double nominalCoCX = userParameterAsDouble("NominalCoCX");
  double nominalCoCY = userParameterAsDouble("NominalCoCY");
  double nominalCoCZ = userParameterAsDouble("NominalCoCZ");

  m_nominalCentreOfCurvature = 
    HepPoint3D(nominalCoCX, nominalCoCY, nominalCoCZ);

  double nominalNorX = userParameterAsDouble("NominalNorX");
  double nominalNorY = userParameterAsDouble("NominalNorY");
  double nominalNorZ = userParameterAsDouble("NominalNorZ");
  
  m_nominalNormal = HepVector3D(nominalNorX, nominalNorY, nominalNorZ);

  m_sphMirrorRadius = userParameterAsDouble("SphMirrorRadius");
  m_sphMirrorMaxX = userParameterAsDouble("SphMirrorMaxX");
  m_sphMirrorMaxY = userParameterAsDouble("SphMirrorMaxY");

  double d = userParameterAsDouble("D");
  
  m_nominalPlaneTop = HepPlane3D(nominalNorX, nominalNorY, nominalNorZ, d);
  m_nominalPlaneBottom = HepPlane3D(nominalNorX, -nominalNorY, nominalNorZ, d);

  log << MSG::DEBUG <<"Finished initialisation for DeRich1"<< endreq;
  return sc;
}

//============================================================================

HepPoint3D DeRich1::nominalCentreOfCurvature(Rich::Side side) const
{
  HepPoint3D  myPoint = m_nominalCentreOfCurvature;
  
  if (Rich::bottom == side) {
    myPoint.setY(-m_nominalCentreOfCurvature.y());
  }
  
  return myPoint;
}

//============================================================================

HepNormal3D DeRich1::nominalNormal(Rich::Side side) const
{
  HepNormal3D  myNormal = m_nominalNormal;
  
  if (Rich::bottom == side) {
    myNormal.setY(-m_nominalNormal.y());
  }
  
  return myNormal;
}

//============================================================================

HepPlane3D DeRich1::nominalPlane(Rich::Side side) const
{
  
  if (Rich::top == side) {
    return m_nominalPlaneTop;
  } else {
    return m_nominalPlaneBottom;
  }
  
}
