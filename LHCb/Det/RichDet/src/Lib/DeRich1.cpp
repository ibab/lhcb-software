// $Id: DeRich1.cpp,v 1.7 2004-07-01 11:02:52 papanest Exp $
#define DERICH1_CPP

// Include files
#include "RichDet/DeRich1.h"
#include "RichDet/DeRichFlatMirror.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "Kernel/CLHEPStreams.h"
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
  StatusCode fail = StatusCode::FAILURE;

  if ( !DeRich::initialize() ) return fail;

  MsgStream log(msgSvc(), "DeRich1" );
  log << MSG::DEBUG << "Starting initialisation for DeRich1" << endreq;

  double nominalCoCX = userParameterAsDouble("Rich1Mirror1NominalCoCX");
  double nominalCoCY = userParameterAsDouble("Rich1Mirror1NominalCoCY");
  double nominalCoCZ = userParameterAsDouble("Rich1Mirror1NominalCoCZ");

  m_nominalCentreOfCurvature = 
    HepPoint3D(nominalCoCX, nominalCoCY, nominalCoCZ);
  m_nominalCentreOfCurvatureBottom = 
    HepPoint3D(nominalCoCX, -nominalCoCY, nominalCoCZ);

  //  std::vector<double> nominalCoC = paramVector("Rich1NominalCoC");
  //  m_nominalCentreOfCurvature = 
  //    HepPoint3D( nominalCoC[0], nominalCoC[1], nominalCoC[2]);
  //  m_nominalCentreOfCurvatureBottom = 
  //    HepPoint3D( nominalCoC[0], -nominalCoC[1], nominalCoC[2]);

  log << MSG::DEBUG << "Nominal centre of curvature" 
      << m_nominalCentreOfCurvature << " ," << m_nominalCentreOfCurvatureBottom
      << endmsg;

  m_sphMirrorRadius = paramAsDouble("Rich1SphMirror1Radius");

  // get the parameters of the nominal flat mirror plane in the form
  // Ax+By+Cz+D=0
  std::vector<double> nominalFMirrorPlane = paramVector("Rich1NominalFlatMirrorPlane");
  m_nominalPlaneTop = HepPlane3D(nominalFMirrorPlane[0],nominalFMirrorPlane[1],
                                  nominalFMirrorPlane[2],nominalFMirrorPlane[3]);
  m_nominalPlaneBottom = HepPlane3D(nominalFMirrorPlane[0],-nominalFMirrorPlane[1],
                                   nominalFMirrorPlane[2],nominalFMirrorPlane[3]);
  
  m_nominalPlaneTop.normalize();
  m_nominalPlaneBottom.normalize();
  m_nominalNormal = m_nominalPlaneTop.normal();
  m_nominalNormalBottom = m_nominalPlaneBottom.normal();

  log << MSG::DEBUG << "Nominal normal " << HepVector3D( m_nominalNormal ) 
      << HepVector3D( m_nominalNormalBottom ) << endmsg;
  log << MSG::DEBUG << "Finished initialisation for DeRich1" << endreq;
  return sc;
}
