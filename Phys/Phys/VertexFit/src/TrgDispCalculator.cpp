// $Id: TrgDispCalculator.cpp,v 1.3 2005-02-28 17:45:56 hruiz Exp $

// Include files
// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// from Event
#include "Event/Particle.h"

// local
#include "TrgDispCalculator.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/Vector.h"

//--------------------------------------------------------------------
//
//  ClassName  : TrgDispCalculator
// 
//  Description: Calculation of impact parameter, distance between 
//  two vertices and distance of closest approach of two particles
//
//  THIS TOOL IS MEANT FOR HLT, WHERE TRACKS ARE ASSUMED TO HAVE A
//  'CYLINDRICAL' ERROR, THAT IS, THE COVARIANCE MATRIX HAS
//  COV(1,1)=COV(2,2)!=0 AND ZERO IN ANY OTHER ELEMENT
//  Could give reasonable approximations in other situations?
//
//  The closest thing to a documentation available by now is a talk at
//  http://agenda.cern.ch/fullAgenda.php?ida=a05940#2005-02-07
//
//  Author     : Hugo Ruiz 
//
//--------------------------------------------------------------------

//==================================================================
// Instantiation of a static factory class used by clients to create
// instances of this service
//==================================================================
static ToolFactory<TrgDispCalculator> s_factory;
const IToolFactory& TrgDispCalculatorFactory = s_factory;

//==================================================================
// Standard Constructor
//==================================================================
TrgDispCalculator::TrgDispCalculator(const std::string& type, 
                    const std::string& name, const IInterface* parent) 
  : GaudiTool( type, name, parent)
{

  declareInterface<IGeomDispCalculator>(this);
}

//==================================================================
// Impact parameter of a particle to a vertex
// Return IP as a scalar and error, and vector and error matrix
//==================================================================
StatusCode TrgDispCalculator::calcImpactPar( const Particle& part,
                                             const Vertex& vertex, double& ip, double& ipErr,
                                             Hep3Vector& ipVector, HepSymMatrix& errMatrix ) { 

  StatusCode status;
  Hep3Vector errVector;

  status = calcImpactPar( part, vertex, ip, ipErr, ipVector, errVector );
  if (!status) {
    fatal() << "Failure from calcImpactPar( part, vertex, ip, ipErr, ipVector, errVector )" << endreq;
    return StatusCode::FAILURE;
  }

  warning() << "The error matrix of the IP vector is a bit dummy" << endreq;

  // A diagonal matrix assuming no correlation between coordinates is computed
  // Very dummy

  errMatrix(1,1) = errVector(1) * errVector(1);
  errMatrix(2,2) = errVector(2) * errVector(2);
  errMatrix(3,3) = errVector(3) * errVector(3);
  errMatrix(1,2) =0.;
  errMatrix(1,3) =0.;
  errMatrix(2,3) =0.;

  return StatusCode::SUCCESS;

}
//==================================================================
// Impact parameter of a particle to a vertex
// Return IP as a scalar and its error
//==================================================================
StatusCode TrgDispCalculator::calcImpactPar( const Particle& part,
                         const Vertex& vertex, double& ip, double& ipErr ) {

  // Point on track and its error
  const Hep3Vector& trackPoint = Hep3Vector(part.pointOnTrack());
  const HepSymMatrix& trackPointErr = HepSymMatrix(part.pointOnTrackErr());

  // Particle direction
  Hep3Vector pNotNorm = Hep3Vector(part.slopeX(),part.slopeY(),1.);
  Hep3Vector uniDirPart = pNotNorm / pNotNorm.mag();
  
  // Vertex position and its error
  const Hep3Vector& vertexPosition = vertex.position();
  const HepSymMatrix& vertexPositionErr = vertex.positionErr();

  // Vector linking vertex and point on track
  Hep3Vector difference = vertexPosition - trackPoint;

  // Impact parameter vector and norm
  Hep3Vector ipVector = uniDirPart.cross(difference.cross(uniDirPart));
  ip = ipVector.mag();
  HepVector ipUniVector(3); // Ned to use HepVector to multiply by matrices
  ipUniVector(1) = ipVector.x()/ip;
  ipUniVector(2) = ipVector.y()/ip;
  ipUniVector(3) = ipVector.z()/ip;

  // Error on impact parameter
  double projTrackErrIp = fabs(dot(ipUniVector,trackPointErr*ipUniVector));
  double projVertexErrIp = fabs(dot(ipUniVector,vertexPositionErr*ipUniVector));
  ipErr = sqrt( projTrackErrIp + projVertexErrIp );

  return StatusCode::SUCCESS;
}

//==================================================================
// Impact parameter of a particle to a vertex
// Return IP as a scalar and error, and vector and error on components
//==================================================================

///////////////////////////////////////////////////////////////////////
// CODE DUPLICATED IN ORDER TO MAKE THE METHOD THAT RETURNS IP
// AS SCALAR MORE TIME-EFFICIENT, as this is meant for HLT!
///////////////////////////////////////////////////////////////////////

StatusCode TrgDispCalculator::calcImpactPar( const Particle& part,
                         const Vertex& vertex, double& ip, double& ipErr,
                         Hep3Vector& ipVector, Hep3Vector& errVector ) { 

  // Point on track and its error
  const Hep3Vector& trackPoint = Hep3Vector(part.pointOnTrack());
  const HepSymMatrix& trackPointErr = HepSymMatrix(part.pointOnTrackErr());

  // Particle direction
  Hep3Vector pNotNorm = Hep3Vector(part.slopeX(),part.slopeY(),1.);
  Hep3Vector uniDirPart = pNotNorm / pNotNorm.mag();
  
  // Vertex position and its error
  const Hep3Vector& vertexPosition = vertex.position();
  const HepSymMatrix& vertexPositionErr = vertex.positionErr();

  // Vector linking vertex and point on track
  Hep3Vector difference = vertexPosition - trackPoint;

  // Impact parameter vector and norm
  ipVector = uniDirPart.cross(difference.cross(uniDirPart));
  ip = ipVector.mag();
  HepVector ipUniVector(3); // Ned to use HepVector to multiply by matrices
  ipUniVector(1) = ipVector.x()/ip;
  ipUniVector(2) = ipVector.y()/ip;
  ipUniVector(3) = ipVector.z()/ip;

  // Error on impact parameter
  double projTrackErrIp = fabs(dot(ipUniVector,trackPointErr*ipUniVector));
  double projVertexErrIp = fabs(dot(ipUniVector,vertexPositionErr*ipUniVector));
  ipErr = sqrt( projTrackErrIp + projVertexErrIp );


  // Error on impact parameter vector
  errVector = Hep3Vector ( sqrt ( trackPointErr(1,1) + vertexPositionErr(1,1)),
                           sqrt ( trackPointErr(2,2) + vertexPositionErr(2,2)),
                           sqrt ( trackPointErr(3,3) + vertexPositionErr(3,3)));

  return StatusCode::SUCCESS;

}

//==================================================================
// Impact parameter of a particle to a point
// Return IP as a scalar with its error, and vector with its error matrix
//==================================================================

StatusCode TrgDispCalculator::calcImpactPar( const Particle& part,
                             HepPoint3D& point, double& ip, double& ipErr,
                             Hep3Vector& ipVector, HepSymMatrix& errMatrix ) {

  StatusCode status;

  Vertex dummyVertex;
  dummyVertex.setPosition(point);
  
  status = calcImpactPar( part, dummyVertex, ip, ipErr, ipVector, errMatrix );
  if (!status){
    fatal() << "calcImpactPar( part, dummyVertex, ip, ipErr, ipVector, errMatrix ) failed " << endreq;
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

//==================================================================
// Impact parameter of a particle to a point
// Return IP as a scalar and its error
//==================================================================

StatusCode TrgDispCalculator::calcImpactPar( const Particle& part,
                       HepPoint3D& point, double& ip, double& ipErr ) {

  StatusCode status;
  
  Hep3Vector ipVector;
  HepSymMatrix ipErrMatrix;
  status = calcImpactPar( part, point, ip, ipErr, ipVector, ipErrMatrix);
  if (!status){
    fatal() << "calcImpactPar( part, point, ip, ipErr, ipVector, ipErrVector) failed"<< endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//==================================================================
// Distance of closest approach between two particles
//==================================================================
StatusCode TrgDispCalculator::calcCloseAppr( const Particle& part1,
                                             const Particle& part2, 
                                             double& dist, double& distErr ) {
 
  //Flag to indicate parallel tracks
  int aux = 0;

  // Get the direction perpendicular to both particle tracks:
  Hep3Vector mom1(part1.momentum().v()); 
  Hep3Vector mom2(part2.momentum().v()); 
  Hep3Vector perpDirection = mom1.cross(mom2);

  // Get points on particles and the displacement vector between the two particles
  const Hep3Vector& trackPoint1 = part1.pointOnTrack();
  const Hep3Vector& trackPoint2 = part2.pointOnTrack();
  Hep3Vector disp = trackPoint1 - trackPoint2;

  // Calculate the perpendicular direction when the tracks are parallel:
  if (perpDirection == 0) {
     aux = 1;
     perpDirection = (disp.cross(mom1)).cross(mom1);
  }
  Hep3Vector tempPerpUnit = perpDirection/perpDirection.mag();       
  HepVector perpUnit(3);
  perpUnit(1) = tempPerpUnit.x(); // 
  perpUnit(2) = tempPerpUnit.y(); // >:-(
  perpUnit(3) = tempPerpUnit.z(); // 
  
  // Calculate the distance of closest approach as the projection
  // of this vector in the perpendicular direction:
  dist = fabs(disp.dot(tempPerpUnit)); 
  
  // Calculate error on distance
  const HepSymMatrix& trackErr1 = part1.pointOnTrackErr();
  const HepSymMatrix& trackErr2 = part2.pointOnTrackErr();
  double projTrackErr1 = fabs(dot(perpUnit,trackErr1*perpUnit));
  double projTrackErr2 = fabs(dot(perpUnit,trackErr2*perpUnit));
  distErr = sqrt( projTrackErr1 + projTrackErr2 );

  return StatusCode::SUCCESS;

}

//==================================================================
// Distance between two vertices
//==================================================================

StatusCode TrgDispCalculator::calcVertexDis( const Vertex& vertex1,
                             const Vertex& vertex2, double& dist, 
                             double& distErr ) {

  // Code copied from that of Sandra Amato in GeomDispCalculator
 
  //Calculate the distance between two vectors:
  HepPoint3D d3D = vertex1.position() - vertex2.position();
  Hep3Vector diff(d3D.x(),d3D.y(),d3D.z());
  dist = diff.mag();
  if (dist == 0) return StatusCode::FAILURE;

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

  distErr = sqrt(fabs(dot(derivDist,errMatrix*derivDist)));
  
  return StatusCode::SUCCESS;
}
