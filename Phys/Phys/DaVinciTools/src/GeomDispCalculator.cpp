// $Id: GeomDispCalculator.cpp,v 1.1 2002-03-27 20:34:59 gcorti Exp $

// Include files
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"

// from Event
#include "Event/Particle.h"

// local
#include "GeomDispCalculator.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/DiagMatrix.h"
#include "CLHEP/Matrix/Vector.h"

//--------------------------------------------------------------------
//
//  ClassName  : GeomDispCalculator
// 
//  Description: These methods return the impact parameter, the
//               distance between two vertices and the distance of
//               closest approach
//
//  Author     : Miriam Gandelman 
//
//--------------------------------------------------------------------

//==================================================================
// Instantiation of a static factory class used by clients to create
// instances of this service
//==================================================================
static ToolFactory<GeomDispCalculator> s_factory;
const IToolFactory& GeomDispCalculatorFactory = s_factory;

//==================================================================
// Standard Constructor
//==================================================================
GeomDispCalculator::GeomDispCalculator(const std::string& type, 
                    const std::string& name, const IInterface* parent) 
    : AlgTool( type, name, parent ) {

  declareInterface<IGeomDispCalculator>(this);
}

//==================================================================
// Calculate the impact parameter of a particle with respect to
// a given vertex
//==================================================================
void GeomDispCalculator::calcImpactPar( const Particle& particle,
                         const Vertex& vertex, double& ip, double& ipErr,
                         Hep3Vector& ipVector, HepSymMatrix& errMatrix ) { 
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Impact Parameter Calculation Tool 1" << endreq;
 
  // get the displacemente vector between a point on the track and 
  // the vertex:
  Hep3Vector point = particle.pointOnTrack();
  Hep3Vector vtx = vertex.position();
  Hep3Vector displacement = point - vtx;

  //find the momentum unitary vector:
  Hep3Vector p(particle.momentum().v()); 
  double pmag = p.mag();
  Hep3Vector pUnit(p/pmag);

  //now calculate the impact parameter:
  ipVector = displacement.cross(pUnit);
  ip = ipVector.mag();

  //Calculate the error on the impact parameter
  Hep3Vector ipUnit = ipVector/ip;
  Hep3Vector derivPoint = pUnit.cross(ipUnit);
  Hep3Vector derivVtx = - pUnit.cross(ipUnit);
  Hep3Vector derivP = (ipVector.cross(displacement) - 
                      ip*ip*pUnit) / pmag; 
  HepSymMatrix pointErr = particle.pointOnTrackErr();
  HepSymMatrix momErr = particle.momentumErr().sub(1,3);
  HepSymMatrix vtxErr = vertex.positionErr();
  HepSymMatrix errMatrix1 = dsum(pointErr,momErr);
  errMatrix = dsum(errMatrix1,vtxErr);

  HepVector u(3);
  u(1) = derivPoint.x();
  u(2) = derivPoint.y();
  u(3) = derivPoint.z();
  HepVector u1(3);
  u1(1) = derivP.x();
  u1(2) = derivP.y();
  u1(3) = derivP.z();
  HepVector u2(3);
  u2(1) = derivVtx.x();
  u2(2) = derivVtx.y();
  u2(3) = derivVtx.z();
  HepVector deriv1 = dsum(u,u1);
  HepVector derivTotal = dsum(deriv1,u2);
  ipErr = sqrt(dot(derivTotal,errMatrix*derivTotal));

}
//==================================================================
// Calculate the impact parameter of a particle with respect to
// a given vertex
//==================================================================
void GeomDispCalculator::calcImpactPar( const Particle& particle,
                         const Vertex& vertex, double& ip, double& ipErr ) {

     Hep3Vector ipVector;
     HepSymMatrix errMatrix;
     calcImpactPar( particle, vertex, ip, ipErr, ipVector, errMatrix );  

}


//==================================================================
// Calculate the impact parameter of a particle with respect to
// a given HepPoint3D point
//==================================================================
void GeomDispCalculator::calcImpactPar( const Particle& particle,
                             HepPoint3D& point, double& ip, double& ipErr,
                             Hep3Vector& ipVector, HepSymMatrix& errMatrix ) {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Impact Parameter Calculation Tool 2" << endreq;
 
  // get the displacemente vector between a point on the track and
  // the HepPoint3D point:
  Hep3Vector particlePoint = particle.pointOnTrack();
  Hep3Vector displacement = particlePoint - point;

  //find the momentum unitary vector:
  Hep3Vector p(particle.momentum().v());
  double pmag = p.mag();
  Hep3Vector pUnit(p/pmag);

  //now calculate the impact parameter:
  ipVector = displacement.cross(pUnit);
  ip = ipVector.mag();

  //Calculate the error on the impact parameter
  Hep3Vector ipUnit = ipVector/ip;
  Hep3Vector derivPoint = pUnit.cross(ipUnit);
  Hep3Vector derivVtx = - pUnit.cross(ipUnit);
  Hep3Vector derivP = (ipVector.cross(displacement) -
                      ip*ip*pUnit) / pmag;
  HepSymMatrix pointErr = particle.pointOnTrackErr();
  HepSymMatrix momErr = particle.momentumErr().sub(1,3);
  // the point has zero error
  HepSymMatrix vtxErr(3,0);  
  //
  HepSymMatrix errMatrix1 = dsum(pointErr,momErr);
  errMatrix = dsum(errMatrix1,vtxErr);

  HepVector u(3);
  u(1) = derivPoint.x();
  u(2) = derivPoint.y();
  u(3) = derivPoint.z();
  HepVector u1(3);
  u1(1) = derivP.x();
  u1(2) = derivP.y();
  u1(3) = derivP.z();
  HepVector u2(3);
  u2(1) = derivVtx.x();
  u2(2) = derivVtx.y();
  u2(3) = derivVtx.z();
  HepVector deriv1 = dsum(u,u1);
  HepVector derivTotal = dsum(deriv1,u2);
  ipErr = sqrt(dot(derivTotal,errMatrix*derivTotal));

}
//==================================================================
// Calculate the impact parameter of a particle with respect to
// a given HepPoint3D point
//==================================================================
void GeomDispCalculator::calcImpactPar( const Particle& particle,
                             HepPoint3D& point, double& ip, double& ipErr ) {

     Hep3Vector ipVector;
     HepSymMatrix errMatrix;
     calcImpactPar( particle, point, ip, ipErr, ipVector, errMatrix );

}

//==================================================================
// Calculate the distance of closest approach between two
// particles outside the magnetic field
//==================================================================
void GeomDispCalculator::calcCloseAppr( const Particle& particle1,
                             const Particle& particle2, double& dist, 
                             double& distErr ) {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Distance of Closest Approach Calculation Tool" 
                   << endreq;
 
  // Get the direction perpendicular to both particle tracks:
  Hep3Vector mom1(particle1.momentum().v()); 
  Hep3Vector mom2(particle2.momentum().v()); 
  Hep3Vector perpDirection = mom1.cross(mom2);
  Hep3Vector perpUnit = perpDirection/perpDirection.mag();                           
 
  // Get the displacement vector between the two particles
  Hep3Vector disp = particle1.pointOnTrack() - particle2.pointOnTrack();

  // Calculate the distance of closest approach as the projection
  // of this vector in the perpendicular direction:
  dist = fabs(disp.dot(perpUnit));

  //Calculate the error on the distance
  HepSymMatrix point1Err = particle1.pointOnTrackErr(); 
  HepSymMatrix point2Err = particle2.pointOnTrackErr(); 
  HepSymMatrix mom1Err = particle1.momentumErr().sub(1,3);
  HepSymMatrix mom2Err = particle2.momentumErr().sub(1,3);
  HepSymMatrix errMatrix1 = dsum(point1Err,mom1Err);
  HepSymMatrix errMatrix2 = dsum(point2Err,mom2Err);
  HepSymMatrix errMatrix = dsum(errMatrix1,errMatrix2);
  Hep3Vector derivP1 = (((disp.dot(perpUnit)*perpUnit) - disp) /
                     perpDirection.mag()).cross(mom2);
  Hep3Vector derivP2 = -(((disp.dot(perpUnit)*perpUnit) - disp) /
                     perpDirection.mag()).cross(mom1);
  HepVector u(3);
  u(1) = perpUnit.x();
  u(2) = perpUnit.y();
  u(3) = perpUnit.z();
  HepVector u1(3);
  u1(1) = derivP1.x();
  u1(2) = derivP1.y();
  u1(3) = derivP1.z();
  HepVector u2(3);
  u2(1) = derivP2.x();
  u2(2) = derivP2.y();
  u2(3) = derivP2.z();
  HepVector deriv1 = dsum(u,u1);
  HepVector deriv2 = dsum(-u,u2);
  HepVector derivTotal = dsum(deriv1,deriv2);
  distErr = sqrt(dot(derivTotal,errMatrix*derivTotal));

}

//==================================================================
// Calculate the distance between two vertices
//==================================================================

void GeomDispCalculator::calcVertexDis( const Vertex& vertex1,
                             const Vertex& vertex2, double& dist, 
                             double& distErr ) {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Distance between Vertices Calculation Tool" << endreq;
 
  //Calculate the distance between two vectors:
  Hep3Vector diff = vertex1.position() - vertex2.position();
  dist = diff.mag();

  //Calculate the error on the distance:  
  HepSymMatrix v1Err = vertex1.positionErr();
  HepSymMatrix v2Err = vertex2.positionErr();
  HepSymMatrix errMatrix = dsum(v1Err,v2Err);
  Hep3Vector unitario(diff/dist);
  HepVector u(3);
  u(1) = unitario.x();
  u(2) = unitario.y();
  u(3) = unitario.z();
  HepVector derivDist = dsum(u,-u);

  distErr = sqrt(dot(derivDist,errMatrix*derivDist));
  
}
