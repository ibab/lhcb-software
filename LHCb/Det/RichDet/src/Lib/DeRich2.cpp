// $Id: DeRich2.cpp,v 1.4 2003-06-16 09:50:15 jonrob Exp $
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
  
  double nominalCoCX = userParameterAsDouble("Rich2NominalCoCX");
  double nominalCoCY = userParameterAsDouble("Rich2NominalCoCY");
  double nominalCoCZ = userParameterAsDouble("Rich2NominalCoCZ");

  m_nominalCentreOfCurvature = 
    HepPoint3D(nominalCoCX, nominalCoCY, nominalCoCZ);

  double nominalNorX = userParameterAsDouble("Rich2NominalNorX");
  double nominalNorY = userParameterAsDouble("Rich2NominalNorY");
  double nominalNorZ = userParameterAsDouble("Rich2NominalNorZ");
  
  m_nominalNormal = HepVector3D(nominalNorX, nominalNorY, nominalNorZ);

  m_sphMirrorRadius = userParameterAsDouble("Rich2SphMirrorRadius");
  m_sphMirrorMaxX = userParameterAsDouble("Rich2SphMirrorMaxX");
  m_sphMirrorMaxY = userParameterAsDouble("Rich2SphMirrorMaxY");

  double d = userParameterAsDouble("Rich2DParam");
  
  m_nominalPlaneLeft = HepPlane3D(nominalNorX, nominalNorY, nominalNorZ, d);
  m_nominalPlaneRight = HepPlane3D(-nominalNorX, nominalNorY, nominalNorZ, d);

  log << MSG::DEBUG <<"Finished initialisation for DeRich2"<< endreq;
  return sc;
}

//============================================================================

HepPoint3D DeRich2::nominalCentreOfCurvature(Rich::Side side) const
{

  if ( Rich::right == side ) {
    return HepPoint3D( -m_nominalCentreOfCurvature.x(), 
                       m_nominalCentreOfCurvature.y(),
                       m_nominalCentreOfCurvature.z() );
  } else {
    return m_nominalCentreOfCurvature;
  }
  
}

//============================================================================

HepNormal3D DeRich2::nominalNormal(Rich::Side side) const
{
  
  if ( Rich::right == side ) {
    return HepNormal3D( -m_nominalNormal.x(),
                        m_nominalNormal.y(),
                        m_nominalNormal.z() );
  } else {
    return m_nominalNormal;
  }
  
}

//============================================================================

HepPlane3D DeRich2::nominalPlane(Rich::Side side) const
{ 
  return ( Rich::left == side ? m_nominalPlaneLeft : m_nominalPlaneRight );
}
