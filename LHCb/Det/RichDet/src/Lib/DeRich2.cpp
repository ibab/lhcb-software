// $Id: DeRich2.cpp,v 1.1 2002-07-16 16:02:41 papanest Exp $
#define DERICH2_CPP

// Include files
#include "RichDet/DeRich2.h"
#include "RichDet/DeRichFlatMirror.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"
//------------------------------------------------------------------------------
//
// Implementation of class :  DeRich2
//
//------------------------------------------------------------------------------

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

  double d = userParameterAsDouble("D");
  
  m_nominalPlaneLeft = HepPlane3D(nominalNorX, nominalNorY, nominalNorZ, d);
  m_nominalPlaneRight = HepPlane3D(-nominalNorX, nominalNorY, nominalNorZ, d);

  log << MSG::DEBUG <<"Finished initialisation for DeRich2"<< endreq;
  return sc;
}

//============================================================================

HepPoint3D DeRich2::nominalCentreOfCurvature(Rich::Side side) const
{
  HepPoint3D  myPoint = m_nominalCentreOfCurvature;
  
  if (Rich::right == side) {
    myPoint.setX(-m_nominalCentreOfCurvature.x());
  }
  
  return myPoint;
}

HepNormal3D DeRich2::nominalNormal(Rich::Side side) const
{
  HepNormal3D  myNormal = m_nominalNormal;
  
  if (Rich::right == side) {
    myNormal.setX(-m_nominalNormal.x());
  }
  
  return myNormal;
}

HepPlane3D DeRich2::nominalPlane(Rich::Side side) const
{
  
  if (Rich::left == side) {
    return m_nominalPlaneLeft;
  } else {
    return m_nominalPlaneRight;
  }
  
}
