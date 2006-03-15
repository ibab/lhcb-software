// $Id: TrgDispCalculator.cpp,v 1.2 2006-03-15 13:41:00 pkoppenb Exp $

// Include files
// from Gaudi
#include "GaudiKernel/ToolFactory.h"

#include "Math/Functions.h"

// from Event
#include "Event/Particle.h"

// local
#include "TrgDispCalculator.h"

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
StatusCode TrgDispCalculator::calcImpactPar( const LHCb::Particle& part,
                                             const LHCb::Vertex& vertex, double& ip, double& ipErr,
                                             Gaudi::XYZVector& ipVector, Gaudi::SymMatrix3x3& errMatrix ) { 

  StatusCode status;
  Gaudi::XYZVector errVector;

  status = calcImpactPar( part, vertex, ip, ipErr, ipVector, errVector );
  if (!status) {
    fatal() << "Failure from calcImpactPar( part, vertex, ip, ipErr, ipVector, errVector )" << endreq;
    return StatusCode::FAILURE;
  }

  warning() << "The error matrix of the IP vector is a bit dummy" << endreq;

  // A diagonal matrix assuming no correlation between coordinates is computed
  // Very dummy

  errMatrix(1,1) = errVector.X() * errVector.X();
  errMatrix(2,2) = errVector.Y() * errVector.Y();
  errMatrix(3,3) = errVector.Z() * errVector.Z();
  errMatrix(1,2) = 0.;
  errMatrix(1,3) = 0.;
  errMatrix(2,3) = 0.;

  return StatusCode::SUCCESS;

}
//==================================================================
// Impact parameter of a particle to a vertex
// Return IP as a scalar and its error
//==================================================================
StatusCode TrgDispCalculator::calcImpactPar( const LHCb::Particle& part,
                                             const LHCb::Vertex& vertex, double& ip, double& ipErr ) {
  Gaudi::XYZVector ipVector;
  Gaudi::XYZVector errVector;
  return calcImpactPar( part, vertex, ip, ipErr,ipVector,errVector);
}

//==================================================================
// Impact parameter of a particle to a vertex
// Return IP as a scalar and error, and vector and error on components
//==================================================================

StatusCode TrgDispCalculator::calcImpactPar( const LHCb::Particle& part,
                                             const LHCb::Vertex& vertex, double& ip, double& ipErr,
                                             Gaudi::XYZVector& ipVector, Gaudi::XYZVector& errVector ) { 

  // Point on track and its error
  const Gaudi::XYZPoint& trackPoint = part.referencePoint();
  const Gaudi::SymMatrix3x3& trackPointErr = part.posCovMatrix();

  // LHCb::Particle direction
  Gaudi::XYZVector pNotNorm = Gaudi::XYZVector(part.slopeX(),part.slopeY(),1.);
  Gaudi::XYZVector uniDirPart = pNotNorm / pNotNorm.R();
  
  // LHCb::Vertex position and its error
  const Gaudi::XYZPoint& vertexPosition = vertex.position();
  const Gaudi::SymMatrix3x3& vertexPositionErr = vertex.covMatrix();

  // Vector linking vertex and point on track
  Gaudi::XYZVector difference = vertexPosition - trackPoint;

  // Impact parameter vector and norm
  ipVector = uniDirPart.Cross(difference.Cross(uniDirPart));
  ip = ipVector.R();
  Gaudi::XYZVector ipUniVector = ipVector.Unit();

  /// @todo I'd like a constructor from XYZVector to Vector3 and back
  Gaudi::Vector3 ipUniVector3 = Gaudi::Vector3(ipUniVector.X(),ipUniVector.Y(),ipUniVector.Z()); 

  // Error on impact parameter
  /// @todo Mabe have Gaudi:: shortcuts for Dot...
  double projTrackErrIp = fabs(ROOT::Math::Dot<double,3>(trackPointErr*ipUniVector3,ipUniVector3));
  double projVertexErrIp = fabs(ROOT::Math::Dot<double,3>(vertexPositionErr*ipUniVector3,ipUniVector3));
  ipErr = sqrt( projTrackErrIp + projVertexErrIp );


  // Error on impact parameter vector
  errVector = Gaudi::XYZVector ( sqrt ( trackPointErr(1,1) + vertexPositionErr(1,1)),
                                 sqrt ( trackPointErr(2,2) + vertexPositionErr(2,2)),
                                 sqrt ( trackPointErr(3,3) + vertexPositionErr(3,3)));

  return StatusCode::SUCCESS;

}

//==================================================================
// Impact parameter of a particle to a point
// Return IP as a scalar with its error, and vector with its error matrix
//==================================================================

StatusCode TrgDispCalculator::calcImpactPar( const LHCb::Particle& part,
                                             const Gaudi::XYZPoint& point, double& ip, double& ipErr,
                                             Gaudi::XYZVector& ipVector, Gaudi::SymMatrix3x3& errMatrix ) {

  StatusCode status;

  LHCb::Vertex dummyVertex;
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

StatusCode TrgDispCalculator::calcImpactPar( const LHCb::Particle& part,
                                             const Gaudi::XYZPoint& point, double& ip, double& ipErr ) {

  StatusCode status;
  
  Gaudi::XYZVector ipVector;
  Gaudi::SymMatrix3x3 ipErrMatrix;
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
StatusCode TrgDispCalculator::calcCloseAppr( const LHCb::Particle& part1,
                                             const LHCb::Particle& part2, 
                                             double& dist, double& distErr ) {
 
  //Flag to indicate parallel tracks
  int aux = 0;

  // Get the direction perpendicular to both particle tracks:

  /// Problem: The version we are using now does not take Vec() as const method
  //  const Gaudi::XYZVector mom1 = (part1.momentum()).Vec();
  //  const Gaudi::XYZVector mom2 = part2.momentum().Vec(); 
  Gaudi::XYZTVector tmp1 = part1.momentum() ;
  const Gaudi::XYZVector mom1 = tmp1.Vect();
  Gaudi::XYZTVector tmp2 = part2.momentum() ;
  const Gaudi::XYZVector mom2 = tmp2.Vect(); 

  Gaudi::XYZVector perpDirection = mom1.Cross(mom2) ;

  // Get points on particles and the displacement vector between the two particles
  const Gaudi::XYZPoint& trackPoint1 = part1.referencePoint();
  const Gaudi::XYZPoint& trackPoint2 = part2.referencePoint();
  Gaudi::XYZVector disp = trackPoint1 - trackPoint2;

  // Calculate the perpendicular direction when the tracks are parallel:
  if (perpDirection.R() == 0.) {
    aux = 1;
    perpDirection = (disp.Cross(mom1)).Cross(mom1);
  }
  Gaudi::XYZVector perpUnit = perpDirection/perpDirection.R();       
  
  // Calculate error on distance
  const Gaudi::SymMatrix3x3& trackErr1 = part1.posCovMatrix();
  const Gaudi::SymMatrix3x3& trackErr2 = part2.posCovMatrix();

  // Calculate the distance of closest approach as the projection
  // of this vector in the perpendicular direction:
  dist = fabs(disp.Dot(perpUnit));
  
  /// @todo I'd like a constructor from XYZVector to Vector3 and back
  Gaudi::Vector3 perpUnit2 = Gaudi::Vector3(perpUnit.X(),perpUnit.Y(),perpUnit.Z());

  double projTrackErr1 = fabs(ROOT::Math::Dot<double,3>(trackErr1*perpUnit2,perpUnit2));
  double projTrackErr2 = fabs(ROOT::Math::Dot<double,3>(trackErr2*perpUnit2,perpUnit2));
  distErr = sqrt( projTrackErr1 + projTrackErr2 );

  return StatusCode::SUCCESS;

}

//==================================================================
// Distance between two vertices
//==================================================================

StatusCode TrgDispCalculator::calcVertexDis( const LHCb::Vertex& vertex1,
                                             const LHCb::Vertex& vertex2, double& dist, 
                                             double& distErr ) {

  // Code copied from that of Sandra Amato in GeomDispCalculator
 
  //Calculate the distance between two vectors:
  Gaudi::XYZVector diff = vertex1.position() - vertex2.position();
  dist = diff.R();
  if (dist == 0) return StatusCode::FAILURE;

  //Calculate the error on the distance:  
  Gaudi::SymMatrix3x3 v1Err = vertex1.covMatrix();
  Gaudi::SymMatrix3x3 v2Err = vertex2.covMatrix();
  //  Gaudi::SymMatrix3x3 errMatrix = dsum(v1Err,v2Err);
  Gaudi::SymMatrix6x6 errMatrix ;
  errMatrix.Place_at(v1Err,0,0);
  errMatrix.Place_at(v2Err,3,3);

  Gaudi::XYZVector unitario(diff/dist);
  Gaudi::Vector3 u = Gaudi::Vector3(unitario.X(),unitario.Y(),unitario.X());
  //  HepVector derivDist = dsum(u,-u);
  Gaudi::Vector6 derivDist  ;
  derivDist.Place_at(u,0);
  derivDist.Place_at(-u,3);

  distErr = sqrt(fabs(ROOT::Math::Dot<double,6>(errMatrix*derivDist,derivDist)));
  
  return StatusCode::SUCCESS;
}
