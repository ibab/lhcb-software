// $Id: TrgDispCalculator.cpp,v 1.6 2006-07-20 13:19:21 jpalac Exp $

// Include files
// from Gaudi
#include "GaudiKernel/ToolFactory.h"

#include "Math/Functions.h"

// from Event
#include "Event/Particle.h"

// from LHCb
#include "LHCbMath/Line.h"
#include "LHCbMath/GeomFun.h"
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
                                             const LHCb::VertexBase& vertex, 
                                             double& ip, 
                                             double& ipErr,
                                             Gaudi::XYZVector& ipVector, 
                                             Gaudi::SymMatrix9x9& errMatrix ) const
{ 

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
                                             const LHCb::VertexBase& vertex, 
                                             double& ip, 
                                             double& ipErr ) const {
  Gaudi::XYZVector ipVector;
  Gaudi::XYZVector errVector;
  return calcImpactPar( part, vertex, ip, ipErr,ipVector,errVector);
}

//==================================================================
// Impact parameter of a particle to a vertex
// Return IP as a scalar and error, and vector and error on components
//==================================================================

StatusCode TrgDispCalculator::calcImpactPar( const LHCb::Particle& part,
                                             const LHCb::VertexBase& vertex, 
                                             double& ip, 
                                             double& ipErr,
                                             Gaudi::XYZVector& ipVector, 
                                             Gaudi::XYZVector& errVector ) const
{ 

  // Point on track and its error
  const Gaudi::XYZPoint& trackPoint = part.referencePoint();
  const Gaudi::SymMatrix3x3& trackPointErr = part.posCovMatrix();

  // LHCb::Particle direction
  const Gaudi::XYZVector pNotNorm = Gaudi::XYZVector(part.slopes().X(),
                                                     part.slopes().Y(),1.);
  const Gaudi::XYZVector uniDirPart = pNotNorm / pNotNorm.R();
  
  // LHCb::VertexBase position and its error
  const Gaudi::XYZPoint& vertexPosition = vertex.position();
  const Gaudi::SymMatrix3x3& vertexPositionErr = vertex.covMatrix();

  // Vector linking vertex and point on track
  const Gaudi::XYZVector difference = vertexPosition - trackPoint;

  // Impact parameter vector and norm
  ipVector = uniDirPart.Cross(difference.Cross(uniDirPart));
  ip = ipVector.R();
  const Gaudi::XYZVector ipUniVector = ipVector.Unit();

  const Gaudi::Vector3 ipUniVector3 = Gaudi::Vector3(ipUniVector.X(),
                                                     ipUniVector.Y(),
                                                     ipUniVector.Z()); 

  // Error on impact parameter
  double projTrackErrIp = 
    fabs(ROOT::Math::Dot(trackPointErr*ipUniVector3,ipUniVector3));
  double projVertexErrIp = 
    fabs(ROOT::Math::Dot(vertexPositionErr*ipUniVector3,ipUniVector3));

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
                                             const Gaudi::XYZPoint& point, 
                                             double& ip, 
                                             double& ipErr,
                                             Gaudi::XYZVector& ipVector, 
                                             Gaudi::SymMatrix9x9& errMatrix ) const
{

  StatusCode status;

  LHCb::VertexBase dummyVertex;
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
                                             const Gaudi::XYZPoint& point, 
                                             double& ip, 
                                             double& ipErr ) const {

  StatusCode status;
  
  Gaudi::XYZVector ipVector;
  Gaudi::SymMatrix9x9 ipErrMatrix;
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
StatusCode TrgDispCalculator::calcCloseAppr( const LHCb::Particle& particle1,
                                             const LHCb::Particle& particle2, 
                                             double& dist, 
                                             double& distErr ) const {
 
  typedef Gaudi::Line<Gaudi::XYZPoint, Gaudi::XYZVector> XYZLine;

  const XYZLine part0(particle1.referencePoint(), 
                      particle1.momentum().Vect());
  const XYZLine part1(particle2.referencePoint(), 
                      particle2.momentum().Vect());

  dist = Gaudi::Math::distance(part0, part1);

  // Get the direction perpendicular to both particle tracks:
  const Gaudi::XYZVector mom1 = particle1.momentum().Vect();
  const Gaudi::XYZVector mom2 = particle2.momentum().Vect();

  // Get points on particles and the displacement vector between the two particles
  const Gaudi::XYZVector disp = part0.beginPoint() - part0.beginPoint();

  // Calculate the perpendicular direction when the tracks are parallel:
  Gaudi::XYZVector perpUnit;
  
  if ( Gaudi::Math::parallel(part0, part1) ) {
    perpUnit = ( (disp.Cross(mom1)).Cross(mom1) ).Unit();
  } else {
    perpUnit = ( mom1.Cross(mom2) ).Unit() ;    
  }
  
  // Calculate error on distance
  const Gaudi::SymMatrix3x3& trackErr1 = particle1.posCovMatrix();
  const Gaudi::SymMatrix3x3& trackErr2 = particle2.posCovMatrix();

  const Gaudi::Vector3 perpUnit2 = Gaudi::Vector3(perpUnit.X(),
                                                  perpUnit.Y(),
                                                  perpUnit.Z());

  const double projTrackErr1 = 
    fabs(ROOT::Math::Dot(trackErr1*perpUnit2,perpUnit2));
  const double projTrackErr2 = 
    fabs(ROOT::Math::Dot(trackErr2*perpUnit2,perpUnit2));

  distErr = sqrt( projTrackErr1 + projTrackErr2 );

  return StatusCode::SUCCESS;

}

//==================================================================
// Distance between two vertices
//==================================================================

StatusCode TrgDispCalculator::calcVertexDis( const LHCb::VertexBase& vertex1,
                                             const LHCb::VertexBase& vertex2, 
                                             double& dist, 
                                             double& distErr ) const 
{

  // Code copied from that of Sandra Amato in GeomDispCalculator
 
  //Calculate the distance between two vectors:
  Gaudi::XYZVector diff = vertex1.position() - vertex2.position();
  dist = diff.R();
  if (dist == 0) return StatusCode::FAILURE;

  Gaudi::SymMatrix6x6 errMatrix ;
  errMatrix.Place_at(vertex1.covMatrix(),0,0);
  errMatrix.Place_at(vertex2.covMatrix(),3,3);
  const Gaudi::XYZVector u = diff.Unit();
  const Gaudi::Vector6 derivDist( u.x(),  u.y(),  u.z(),
                                 -u.x(), -u.y(), -u.z() );

  distErr = sqrt(std::fabs(ROOT::Math::Dot(errMatrix*derivDist,derivDist)));
  
  return StatusCode::SUCCESS;
}
