// $Id: GeomDispCalculator.cpp,v 1.3 2005-02-09 17:18:48 pkoppenb Exp $

// Include files
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "Kernel/IParticleTransporter.h"

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
  : GaudiTool( type, name, parent)
  , m_pTransporter(0)
  , m_transporterType("CombinedTransporter")
{

  declareInterface<IGeomDispCalculator>(this);

  declareProperty("Transporter", m_transporterType);
}
//==================================================================
// Initialize
//==================================================================
StatusCode GeomDispCalculator::initialize() {
  
  StatusCode sc = GaudiTool::initialize();
  if (!sc) return sc;
  m_pTransporter = tool<IParticleTransporter>(m_transporterType, this);
  if(!m_pTransporter) {
    fatal() << "    Unable to retrieve ParticleTransporter  tool" ;
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
  
}

//==================================================================
// Calculate the impact parameter of a particle with respect to
// a given vertex
//==================================================================
StatusCode GeomDispCalculator::calcImpactPar( const Particle& particle,
                         const Vertex& vertex, double& ip, double& ipErr,
                         Hep3Vector& ipVector, HepSymMatrix& errMatrix ) { 

  // Get the displacemente vector between a point on the track and 
  // the vertex. Use the transporter tool to move the first
  // measured point closer to the vertex position.
  Hep3Vector vtx = vertex.position();

  Particle transParticle;
  StatusCode sctrans = m_pTransporter->transport(particle,
                                                 vtx.z(),
                                                 transParticle);
    if ( !sctrans.isSuccess() ) {
      debug() << "Transporter failed" << endreq;
      return sctrans;
    }

  Hep3Vector point = transParticle.pointOnTrack();
  Hep3Vector displacement = point - vtx;

  //find the momentum unitary vector:
  Hep3Vector p(transParticle.momentum().v()); 
  double pmag = p.mag();
  Hep3Vector pUnit(p/pmag);

  //now calculate the impact parameter:
  ipVector = displacement.cross(pUnit);
  ip = ipVector.mag();
  if (ip == 0) return StatusCode::FAILURE;

  //Calculate the error on the impact parameter
  Hep3Vector ipUnit = ipVector/ip;
  Hep3Vector derivPoint = pUnit.cross(ipUnit);
  Hep3Vector derivVtx = - pUnit.cross(ipUnit);
  Hep3Vector derivP = (ipUnit.cross(displacement) - 
                      ipUnit.dot(ipVector)*pUnit) / pmag; 
  HepSymMatrix pointErr = transParticle.pointOnTrackErr();
  HepSymMatrix momErr = transParticle.momentumErr().sub(1,3);
  HepMatrix errMatrix1 = dsum(pointErr,momErr);
  HepMatrix posMomCorr = transParticle.posMomCorr().sub(1,3,1,3);
  errMatrix1.sub(4,1,posMomCorr);
  errMatrix1.sub(1,4,posMomCorr.T());
  HepSymMatrix vtxErr = vertex.positionErr();
  HepMatrix errMatrixTotal = dsum(errMatrix1,vtxErr);
  errMatrix.assign(errMatrixTotal);

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
  ipErr = sqrt(fabs(dot(derivTotal,errMatrix*derivTotal)));

// Turn ipVector 90 degrees to get the right vector to return
  ipVector = pUnit.cross(ipVector);

  return StatusCode::SUCCESS;

}
//==================================================================
// Calculate the impact parameter of a particle with respect to
// a given vertex
//==================================================================
StatusCode GeomDispCalculator::calcImpactPar( const Particle& particle,
                         const Vertex& vertex, double& ip, double& ipErr ) {

     Hep3Vector ipVector;
     HepSymMatrix errMatrix;
     calcImpactPar( particle, vertex, ip, ipErr, ipVector, errMatrix );  

  return StatusCode::SUCCESS;
}

//==================================================================
//// Calculate the impact parameter of a particle with respect to
//// a given vertex - output the ipVector and the error in its
//// components as well
////==================================================================
StatusCode GeomDispCalculator::calcImpactPar( const Particle& particle,
                         const Vertex& vertex, double& ip, double& ipErr,
                         Hep3Vector& ipVector, Hep3Vector& errVector ) { 

  // Get the displacemente vector between a point on the track and 
  // the vertex. Use the transporter tool to move the first
  // measured point closer to the vertex position.
  Hep3Vector vtx = vertex.position();

  Particle transParticle;
  StatusCode sctrans = m_pTransporter->transport(particle,
                                                 vtx.z(),
                                                 transParticle);
    if ( !sctrans.isSuccess() ) {
      debug() << "Transporter failed" << endreq;
      return sctrans;
    }

  Hep3Vector point = transParticle.pointOnTrack();
  Hep3Vector displacement = point - vtx;

  //find the momentum unitary vector:
  Hep3Vector p(transParticle.momentum().v()); 
  double pmag = p.mag();
  Hep3Vector pUnit(p/pmag);

  //now calculate the impact parameter:
  ipVector = displacement.cross(pUnit);
  ip = ipVector.mag();
  if (ip == 0) return StatusCode::FAILURE;

  //Calculate the error on the impact parameter
  Hep3Vector ipUnit = ipVector/ip;
  Hep3Vector derivPoint = pUnit.cross(ipUnit);
  Hep3Vector derivVtx = - pUnit.cross(ipUnit);
  Hep3Vector derivP = (ipUnit.cross(displacement) -
                       ipUnit.dot(ipVector)*pUnit) / pmag;
  HepSymMatrix pointErr = transParticle.pointOnTrackErr();
  HepSymMatrix momErr = transParticle.momentumErr().sub(1,3);
  HepMatrix errMatrix1 = dsum(pointErr,momErr);
  HepMatrix posMomCorr = transParticle.posMomCorr().sub(1,3,1,3);
  errMatrix1.sub(4,1,posMomCorr);
  errMatrix1.sub(1,4,posMomCorr.T());
  HepSymMatrix vtxErr = vertex.positionErr();
  HepMatrix errMatrixTotal = dsum(errMatrix1,vtxErr);
  HepSymMatrix  errMatrix;
  errMatrix.assign(errMatrixTotal);

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
  ipErr = sqrt(fabs(dot(derivTotal,errMatrix*derivTotal)));

// Turn ipVector 90 degrees to get the right vector to return
  ipVector = pUnit.cross(ipVector);

// Now calculate the error for each component
// First for ip_x:
  HepVector derivPointx(3);
  derivPointx(1) = 1. -pUnit.x()*pUnit.x();
  derivPointx(2) = -pUnit.y()*pUnit.x();
  derivPointx(3) = -pUnit.z()*pUnit.x();
    
  HepVector derivVtxx(3);
  derivVtxx(1) = pUnit.x()*pUnit.x() - 1.;
  derivVtxx(2) = pUnit.y()*pUnit.x();
  derivVtxx(3) = pUnit.z()*pUnit.x();

  HepVector derivPx(3);
  derivPx(1) = (-displacement.x()*pUnit.x()/pmag) + 
	        (displacement.dot(pUnit)/pmag)*(-1. + 2.* pUnit.x()*pUnit.x()); 
  derivPx(2) = (-displacement.y()*pUnit.x()/pmag) + 
	        (displacement.dot(pUnit)/pmag)*(2.* pUnit.y()*pUnit.x()); 
  derivPx(3) = (-displacement.z()*pUnit.x()/pmag) + 
	        (displacement.dot(pUnit)/pmag)*(2.* pUnit.z()*pUnit.x()); 
  deriv1 = dsum(derivPointx,derivPx);
  derivTotal = dsum(deriv1,derivVtxx);
  errVector.setX(sqrt(fabs(dot(derivTotal,errMatrix*derivTotal)))); 

// Now for ip_y:
  HepVector derivPointy(3);
  derivPointy(1) = -pUnit.x()*pUnit.y();
  derivPointy(2) = 1. -pUnit.y()*pUnit.y();
  derivPointy(3) = -pUnit.z()*pUnit.y();
    
  HepVector derivVtxy(3);
  derivVtxy(1) = pUnit.x()*pUnit.y();
  derivVtxy(2) = pUnit.y()*pUnit.y() - 1.;
  derivVtxy(3) = pUnit.z()*pUnit.y();

  HepVector derivPy(3);
  derivPy(1) = (-displacement.x()*pUnit.y()/pmag) + 
	        (displacement.dot(pUnit)/pmag)*(2.* pUnit.x()*pUnit.y()); 
  derivPy(2) = (-displacement.y()*pUnit.y()/pmag) + 
	        (displacement.dot(pUnit)/pmag)*(-1. + 2.* pUnit.y()*pUnit.y()); 
  derivPy(3) = (-displacement.z()*pUnit.y()/pmag) + 
	        (displacement.dot(pUnit)/pmag)*(2.* pUnit.z()*pUnit.y()); 
  deriv1 = dsum(derivPointy,derivPy);
  derivTotal = dsum(deriv1,derivVtxy);
  errVector.setY(sqrt(fabs(dot(derivTotal,errMatrix*derivTotal)))); 

// Now for ip_z:
  HepVector derivPointz(3);
  derivPointz(1) = -pUnit.x()*pUnit.z();
  derivPointz(2) = -pUnit.y()*pUnit.z();
  derivPointz(3) = 1. -pUnit.z()*pUnit.z();
    
  HepVector derivVtxz(3);
  derivVtxz(1) = pUnit.x()*pUnit.z();
  derivVtxz(2) = pUnit.y()*pUnit.z();
  derivVtxz(3) = pUnit.z()*pUnit.z() - 1.;

  HepVector derivPz(3);
  derivPz(1) = (-displacement.x()*pUnit.z()/pmag) + 
	        (displacement.dot(pUnit)/pmag)*(2.* pUnit.x()*pUnit.z()); 
  derivPz(2) = (-displacement.y()*pUnit.z()/pmag) + 
	        (displacement.dot(pUnit)/pmag)*(2.* pUnit.y()*pUnit.z()); 
  derivPz(3) = (-displacement.z()*pUnit.z()/pmag) + 
	        (displacement.dot(pUnit)/pmag)*(-1. + 2.* pUnit.z()*pUnit.z()); 
  deriv1 = dsum(derivPointz,derivPz);
  derivTotal = dsum(deriv1,derivVtxz);
  errVector.setZ(sqrt(fabs(dot(derivTotal,errMatrix*derivTotal)))); 

  return StatusCode::SUCCESS;

}

//==================================================================
// Calculate the impact parameter of a particle with respect to
// a given HepPoint3D point
//==================================================================
StatusCode GeomDispCalculator::calcImpactPar( const Particle& particle,
                             HepPoint3D& point, double& ip, double& ipErr,
                             Hep3Vector& ipVector, HepSymMatrix& errMatrix ) {


  // Get the displacemente vector between a point on the track and
  // the HepPoint3D point. Use the transporter tool to move the first
  // measured point closer to the HepPoint3D point.
  Particle transParticle;
  StatusCode sctrans = m_pTransporter->transport(particle,
                                                 point.z(),
                                                 transParticle);
    if ( !sctrans.isSuccess() ) {
      debug() << "Transporter failed" << endreq;
      return sctrans;
    }

  Hep3Vector particlePoint = transParticle.pointOnTrack();
  Hep3Vector displacement = particlePoint - point;

  //find the momentum unitary vector:
  Hep3Vector p(transParticle.momentum().v());
  double pmag = p.mag();
  Hep3Vector pUnit(p/pmag);

  //now calculate the impact parameter:
  ipVector = displacement.cross(pUnit);
  ip = ipVector.mag();
  if (ip == 0) return StatusCode::FAILURE;

  //Calculate the error on the impact parameter
  Hep3Vector ipUnit = ipVector/ip;
  Hep3Vector derivPoint = pUnit.cross(ipUnit);
  Hep3Vector derivVtx = - pUnit.cross(ipUnit);
  Hep3Vector derivP = (ipUnit.cross(displacement) -
                       ipUnit.dot(ipVector)*pUnit) / pmag;
  HepSymMatrix pointErr = transParticle.pointOnTrackErr();
  HepSymMatrix momErr = transParticle.momentumErr().sub(1,3);
  HepMatrix posMomCorr = transParticle.posMomCorr().sub(1,3,1,3);
  HepMatrix errMatrix1 = dsum(pointErr,momErr);
  errMatrix1.sub(1,4,posMomCorr);
  errMatrix1.sub(1,4,posMomCorr.T());
  // the point has zero error
  HepSymMatrix vtxErr(3,0);  
  //
  HepMatrix errMatrixTotal = dsum(errMatrix1,vtxErr);
  errMatrix.assign(errMatrixTotal);

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
  ipErr = sqrt(fabs(dot(derivTotal,errMatrix*derivTotal)));

// Turn ipVector 90 degrees to get the right vector to return
  ipVector = pUnit.cross(ipVector);

  return StatusCode::SUCCESS;
}
//==================================================================
// Calculate the impact parameter of a particle with respect to
// a given HepPoint3D point
//==================================================================
StatusCode GeomDispCalculator::calcImpactPar( const Particle& particle,
                       HepPoint3D& point, double& ip, double& ipErr ) {

     Hep3Vector ipVector;
     HepSymMatrix errMatrix;
     calcImpactPar( particle, point, ip, ipErr, ipVector, errMatrix );

  return StatusCode::SUCCESS;
}

//==================================================================
// Calculate the distance of closest approach between two
// particles outside the magnetic field
//==================================================================
StatusCode GeomDispCalculator::calcCloseAppr( const Particle& particle1,
                             const Particle& particle2, double& dist, 
                             double& distErr ) {


  //Flag to indicate parallel tracks
  int aux = 0;
  // Get the direction perpendicular to both particle tracks:
  Hep3Vector mom1(particle1.momentum().v()); 
  Hep3Vector mom2(particle2.momentum().v()); 
  Hep3Vector perpDirection = mom1.cross(mom2);
  // Get the displacement vector between the two particles
  HepPoint3D disp3D = particle1.pointOnTrack() - particle2.pointOnTrack();
  Hep3Vector disp(disp3D.x(),disp3D.y(),disp3D.z()); // quite stupid, no?

  // Calculate the perpendicular direction when the tracks are parallel:
  if (perpDirection == 0) {
     aux = 1;
     perpDirection = (disp.cross(mom1)).cross(mom1);
  }

  Hep3Vector perpUnit = perpDirection/perpDirection.mag();       

  // Calculate the distance of closest approach as the projection
  // of this vector in the perpendicular direction:
  dist = fabs(disp.dot(perpUnit));

  //Calculate the error on the distance
  HepSymMatrix point1Err = particle1.pointOnTrackErr(); 
  HepSymMatrix point2Err = particle2.pointOnTrackErr(); 
  HepSymMatrix mom1Err = particle1.momentumErr().sub(1,3);
  HepSymMatrix mom2Err = particle2.momentumErr().sub(1,3);
  HepMatrix posMomCorr1 = particle1.posMomCorr().sub(1,3,1,3);
  HepMatrix posMomCorr2 = particle2.posMomCorr().sub(1,3,1,3);
  HepMatrix errMatrix1 = dsum(point1Err,mom1Err);
  errMatrix1.sub(4,1,posMomCorr1);
  errMatrix1.sub(1,4,posMomCorr1.T());
  HepMatrix errMatrix2 = dsum(point2Err,mom2Err);
  errMatrix2.sub(4,1,posMomCorr2);
  errMatrix2.sub(1,4,posMomCorr2.T());
  HepMatrix errMatrixTotal = dsum(errMatrix1,errMatrix2);
  HepSymMatrix errMatrix(12,0);
  errMatrix.assign(errMatrixTotal);

  HepVector u(3);
  HepVector u1(3);
  HepVector u2(3);
  Hep3Vector derivP1;
  Hep3Vector derivP2;
  Hep3Vector derivR1;
  //The derivatives depend if the tracks are parallel or not
  if(aux == 0){
  derivP1 = (((disp.dot(perpUnit)*perpUnit) - disp) /
                     perpDirection.mag()).cross(mom2);
  derivP2 = -(((disp.dot(perpUnit)*perpUnit) - disp) /
                     perpDirection.mag()).cross(mom1);
  u(1) = perpUnit.x();
  u(2) = perpUnit.y();
  u(3) = perpUnit.z();
  u2(1) = derivP2.x();
  u2(2) = derivP2.y();
  u2(3) = derivP2.z();
  }
  else {
  derivP1 = (1./perpDirection.mag())*(
            (disp.dot(mom1)*disp) - (disp*disp*mom1) - 
            (disp.dot(perpUnit))*(disp.dot(mom1))*perpUnit +
            (disp.dot(perpUnit))*(disp.dot(perpUnit))*mom1 -
            (disp.cross(mom1)).cross(perpUnit)*(disp.dot(perpUnit)));
  derivR1 = perpUnit + (1./perpDirection.mag())*(
                        mom1*(disp.dot(mom1)) - mom1*mom1*disp -
                        disp.dot(perpUnit)*perpUnit.dot(mom1)*mom1 +
                        mom1*mom1*disp.dot(perpUnit)*perpUnit);
  u(1) = derivR1.x();
  u(2) = derivR1.y();
  u(3) = derivR1.z();
  u2(1) = 0.;
  u2(2) = 0.;
  u2(3) = 0.;
  }
  u1(1) = derivP1.x();
  u1(2) = derivP1.y();
  u1(3) = derivP1.z();
  HepVector deriv1 = dsum(u,u1);
  HepVector deriv2 = dsum(-u,u2);
  HepVector derivTotal = dsum(deriv1,deriv2);

  distErr = sqrt(fabs(dot(derivTotal,errMatrix*derivTotal)));

  return StatusCode::SUCCESS;
}

//==================================================================
// Calculate the distance between two vertices
//==================================================================

StatusCode GeomDispCalculator::calcVertexDis( const Vertex& vertex1,
                             const Vertex& vertex2, double& dist, 
                             double& distErr ) {

  //Calculate the distance between two vectors:
  HepPoint3D diff = vertex1.position() - vertex2.position();
  dist = diff.mag();
  if (dist == 0) return StatusCode::FAILURE;

  //Calculate the error on the distance:  
  HepSymMatrix v1Err = vertex1.positionErr();
  HepSymMatrix v2Err = vertex2.positionErr();
  HepSymMatrix errMatrix = dsum(v1Err,v2Err);
  HepPoint3D unitario(diff.unit());
  HepVector u(3);
  u(1) = unitario.x();
  u(2) = unitario.y();
  u(3) = unitario.z();
  HepVector derivDist = dsum(u,-u);

  distErr = sqrt(fabs(dot(derivDist,errMatrix*derivDist)));
  
  return StatusCode::SUCCESS;
}
