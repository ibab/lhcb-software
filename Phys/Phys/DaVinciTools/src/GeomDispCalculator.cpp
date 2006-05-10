// $Id: GeomDispCalculator.cpp,v 1.12 2006-05-10 17:00:59 pkoppenb Exp $

// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "Kernel/IParticleTransporter.h"

// From LHCb


#include "Kernel/GeomFun.h"
// from Event
#include "Event/Particle.h"

// local
#include "GeomDispCalculator.h"
//--------------------------------------------------------------------
//
//  ClassName  : GeomDispCalculator
// 
//  Description: These methods return the impact parameter, the
//               distance between two vertices and the distance of
//               closest approach
//
//  Authors    : Miriam Gandelman, Juan Palacios 
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
                                       const std::string& name, 
                                       const IInterface* parent) 
  : 
  GaudiTool( type, name, parent),
  m_pTransporter(0),
  m_transporterType("ParticleTransporter")
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
StatusCode GeomDispCalculator::calcImpactPar( const LHCb::Particle& particle,
                                              const LHCb::Vertex& vertex, 
                                              double& ip, 
                                              double& ipErr,
                                              Gaudi::XYZVector& ipVector, 
                                              Gaudi::SymMatrix9x9& errMatrix ) const
{ 

  // Get the displacemente vector between a point on the track and 
  // the vertex. Use the transporter tool to move the first
  // measured point closer to the vertex position.

  Gaudi::XYZPoint vtx = vertex.position();
  Gaudi::SymMatrix3x3 vtxPosError = vertex.covMatrix();

  LHCb::Particle transParticle;
  StatusCode sctrans = m_pTransporter->transport(&particle,
                                                 vtx.z(),
                                                 transParticle);
  if ( !sctrans.isSuccess() ) {
    debug() << "Transporter failed" << endreq;
    return sctrans;
  }

  return calcImpactPar(transParticle, vtx, vtxPosError, 
                       ip, ipErr, ipVector, errMatrix);
  
  
}
//=============================================================================
StatusCode GeomDispCalculator::calcImpactPar(const LHCb::Particle& particle,
                                             const Gaudi::XYZPoint& pos,
                                             const Gaudi::SymMatrix3x3& posErr,
                                             double& ip, 
                                             double& ipErr,
                                             Gaudi::XYZVector& ipVector, 
                                             Gaudi::SymMatrix9x9& errMatrix ) const
{
  
  Gaudi::XYZVector displacement = particle.referencePoint() - pos;

  const Gaudi::XYZVector pUnit = particle.momentum().Vect().Unit();

  //now calculate the impact parameter:
  ipVector = displacement.Cross(pUnit);
  ip = sqrt( ipVector.mag2() );
  if (ip == 0) return StatusCode::FAILURE;

  // Turn ipVector 90 degrees to get the right vector to return
  ipVector = pUnit.Cross(ipVector);

  calcErrorMatrix(particle, posErr, errMatrix);

  Gaudi::Vector9 derivTotal = totalDeriv(particle, displacement);

  ipErr = sqrt(std::fabs(ROOT::Math::Dot(derivTotal,errMatrix*derivTotal)));

  return StatusCode::SUCCESS;

}
//=============================================================================
void GeomDispCalculator::calcErrorMatrix(const LHCb::Particle& particle,
                                         const Gaudi::SymMatrix3x3& vtxErr, 
                                         Gaudi::SymMatrix9x9& errMatrix) const
{
  
  errMatrix.Place_at(particle.covMatrix().Sub<Gaudi::SymMatrix6x6>(0,0),0,0);
  errMatrix.Place_at(vtxErr, 6, 6);

}
//=============================================================================
Gaudi::Vector9 GeomDispCalculator::totalDeriv(const LHCb::Particle& particle, 
                                              const Gaudi::XYZVector& displ) const
{
  //Calculate the error on the impact parameter
  const double pmag = sqrt( particle.momentum().Vect().mag2() );
  const Gaudi::XYZVector pUnit = particle.momentum().Vect().Unit();
  Gaudi::XYZVector ipVector = displ.Cross(pUnit);

  Gaudi::XYZVector ipUnit = ipVector.Unit();
  Gaudi::XYZVector derivPoint = pUnit.Cross(ipUnit);
  Gaudi::XYZVector derivVtx = -1*derivPoint;
  Gaudi::XYZVector derivP = (ipUnit.Cross(displ) - 
                             ipUnit.Dot(ipVector)*pUnit) / pmag; 

  return Gaudi::Vector9(derivPoint.x(), derivPoint.y(), derivPoint.z(), 
                        derivP.x(),     derivP.y(),     derivP.z(),
                        derivVtx.x(),   derivVtx.y(),   derivVtx.z() );

}

//=============================================================================
// Calculate the impact parameter of a particle with respect to
// a given vertex
//=============================================================================
StatusCode GeomDispCalculator::calcImpactPar( const LHCb::Particle& particle,
                                              const LHCb::Vertex& vertex, 
                                              double& ip, 
                                              double& ipErr ) const
{

  Gaudi::XYZVector ipVector;
  Gaudi::SymMatrix9x9 errMatrix;
  return calcImpactPar( particle, vertex, ip, ipErr, ipVector, errMatrix );  

}

//=============================================================================
/// Calculate the impact parameter of a particle with respect to
/// a given vertex - output the ipVector and the error in its
/// components as well
//=============================================================================
StatusCode GeomDispCalculator::calcImpactPar( const LHCb::Particle& particle,
                                              const LHCb::Vertex& vertex, 
                                              double& ip, 
                                              double& ipErr,
                                              Gaudi::XYZVector& ipVector, 
                                              Gaudi::XYZVector& errVector ) const 
{ 

  // Get the displacemente vector between a point on the track and 
  // the vertex. Use the transporter tool to move the first
  // measured point closer to the vertex position.
  Gaudi::SymMatrix9x9  errMatrix;
  calcImpactPar( particle, vertex, ip, ipErr, ipVector, errMatrix);

// Now calculate the error for each component

// First for ip_x:
  const Gaudi::XYZVector vX(1.,0.,0);
  double errX = calcErrComponent(vX, particle, vertex, errMatrix);

// Now for ip_y:
  const Gaudi::XYZVector vY(0.,1.,0);
  double errY = calcErrComponent(vY, particle, vertex, errMatrix);

// Now for ip_z:
  const Gaudi::XYZVector vZ(0.,0,1.);
  double errZ = calcErrComponent(vZ, particle, vertex, errMatrix);

  errVector.SetXYZ(errX, errY, errZ);
  
  return StatusCode::SUCCESS;

}

//=============================================================================
/// Calculate the impact parameter of a particle with respect to
/// a given 3D point
//=============================================================================
StatusCode GeomDispCalculator::calcImpactPar( const LHCb::Particle& particle,
                                              const Gaudi::XYZPoint& position, 
                                              double& ip, 
                                              double& ipErr,
                                              Gaudi::XYZVector& ipVector, 
                                              Gaudi::SymMatrix9x9& errMatrix ) const
{


  // Get the displacemente vector between a point on the track and
  // the 3D point. Use the transporter tool to move the first
  // measured point closer to the 3D point.
  LHCb::Particle transParticle;
  StatusCode sctrans = m_pTransporter->transport(&particle,
                                                 position.z(),
                                                 transParticle);
  if ( !sctrans.isSuccess() ) {
    debug() << "Transporter failed" << endreq;
    return sctrans;
  }

  Gaudi::SymMatrix3x3 posError;
  return calcImpactPar(transParticle, position, posError, 
                      ip, ipErr, ipVector, errMatrix);

}
//==================================================================
/// Calculate the impact parameter of a particle with respect to
/// a given 3D point
//==================================================================
StatusCode GeomDispCalculator::calcImpactPar( const LHCb::Particle& particle,
                                              const Gaudi::XYZPoint& point, 
                                              double& ip, 
                                              double& ipErr ) const 
{

  Gaudi::XYZVector ipVector;
  Gaudi::SymMatrix9x9 errMatrix;
  calcImpactPar( particle, point, ip, ipErr, ipVector, errMatrix );

  return StatusCode::SUCCESS;
}

//==================================================================
// Calculate the distance of closest approach between two
// particles outside the magnetic field
//==================================================================
StatusCode GeomDispCalculator::calcCloseAppr( const LHCb::Particle& particle1,
                                              const LHCb::Particle& particle2, 
                                              double& dist, 
                                              double& distErr ) const {

  typedef ROOT::Math::SMatrix<double, 12, 12, 
    ROOT::Math::MatRepSym<double, 12> > SymMatrix12x12;

  //  typedef Gaudi::Line<Gaudi::XYZPoint, Gaudi::XYZVector> Line;

  const Gaudi::XYZLine part0(particle1.referencePoint(), 
                             particle1.momentum().Vect());
  const Gaudi::XYZLine part1(particle2.referencePoint(), 
                             particle2.momentum().Vect());

  dist = Gaudi::Math::distance(part0, part1);
  
  // calculate error on distance

  SymMatrix12x12 errMatrix;
  errMatrix.Place_at(particle1.covMatrix().Sub<Gaudi::SymMatrix6x6>(0,0),0,0);
  errMatrix.Place_at(particle2.covMatrix().Sub<Gaudi::SymMatrix6x6>(0,0),6,6);

  // calculate the vector of derivatives

  Gaudi::XYZVector u;
  Gaudi::XYZVector u1;
  Gaudi::XYZVector u2;

  calcDerivVectors(part0, part1, u, u1, u2);

  ROOT::Math::SVector<double,12> derivTotal;
  derivTotal.Place_at(Gaudi::Vector6( u.x(),  u.y(),  u.z(), 
                                      u1.x(), u1.y(), u1.z() ), 0);
  derivTotal.Place_at(Gaudi::Vector6( -u.x(),  -u.y(),  -u.z(), 
                                      u2.x(), u2.y(), u2.z() ),6 );
  
  distErr = sqrt(std::fabs(ROOT::Math::Dot(derivTotal,errMatrix*derivTotal)));

  return StatusCode::SUCCESS;

}


//==================================================================
// Calculate the distance between two vertices
//==================================================================

StatusCode GeomDispCalculator::calcVertexDis( const LHCb::Vertex& vertex1,
                                              const LHCb::Vertex& vertex2, 
                                              double& dist, 
                                              double& distErr ) const
{
  
  const Gaudi::XYZVector diff = vertex1.position() - vertex2.position();
  dist = diff.R();

  if (dist == 0) return StatusCode::FAILURE;

  Gaudi::SymMatrix6x6 errMatrix;
  errMatrix.Place_at(vertex1.covMatrix(),0,0);
  errMatrix.Place_at(vertex2.covMatrix(),3,3);
  const Gaudi::XYZVector u = diff.Unit();
  const Gaudi::Vector6 derivDist( u.x(),  u.y(),  u.z(),
                                 -u.x(), -u.y(), -u.z() );

  distErr = sqrt(std::fabs(ROOT::Math::Dot(derivDist,errMatrix*derivDist)));
  
  return StatusCode::SUCCESS;
}

//==================================================================
// 
//==================================================================
double GeomDispCalculator::calcErrComponent(const Gaudi::XYZVector& vProj,
                                            const LHCb::Particle& particle,
                                            const LHCb::Vertex& vertex,
                                            const Gaudi::SymMatrix9x9 errMtx) const
{
 
  const Gaudi::XYZVector displ = particle.referencePoint()-vertex.position();
  const Gaudi::XYZVector pUnit = particle.momentum().Vect().Unit();
  const double pmag = sqrt( particle.momentum().Vect().mag2() );
  const double scale = pUnit.Dot(vProj);
  const Gaudi::XYZVector pUnitProj  = pUnit*scale;
  const Gaudi::XYZVector derivPoint = vProj - pUnitProj;
  const Gaudi::XYZVector derivVtx   = -1*derivPoint;
  const Gaudi::XYZVector v_tmp      = displ.Dot(pUnit)*(2*pUnitProj-vProj)/pmag;
  const Gaudi::XYZVector derivP     = displ*(-scale/pmag) + v_tmp;

  const Gaudi::Vector9 derivTot(derivPoint.x(), derivPoint.y(), derivPoint.z(),
                                    derivP.x(),     derivP.y(),     derivP.z(),
                                  derivVtx.x(),   derivVtx.y(),   derivVtx.z());

  return sqrt(std::fabs(ROOT::Math::Dot(derivTot,errMtx*derivTot) ) );

}

//=============================================================================
void GeomDispCalculator::calcDerivVectors(const Gaudi::XYZLine& part0,
                                          const Gaudi::XYZLine& part1,
                                          Gaudi::XYZVector& u,
                                          Gaudi::XYZVector& u1,
                                          Gaudi::XYZVector& u2) const 
{

  const Gaudi::XYZVector disp = part0.beginPoint() - part1.beginPoint();
  const Gaudi::XYZLine::Vector mom0 = part0.direction();
  const Gaudi::XYZLine::Vector mom1 = part1.direction();
  Gaudi::XYZVector perpDirection = mom0.Cross( mom1 );  
  const bool parallel = std::fabs(perpDirection.mag2()) < 1e-20;
  
  if( parallel ) {
    perpDirection = ( disp.Cross( mom0 ) ).Cross( mom1 );
  }

  const Gaudi::XYZVector perpUnit = perpDirection.Unit();  

  const double fact = 1./sqrt( perpDirection.mag2() );
  const double dispPU=disp.Dot(perpUnit);

  if (parallel) {
    const double dispMom1=disp.Dot(mom1);
    u1 = fact*( (dispMom1*disp) - (disp.mag2()*mom1) - 
                dispPU*dispMom1*perpUnit + dispPU*dispPU*mom1 -
                (disp.Cross(mom1)).Cross(perpUnit)*dispPU  );

    u = perpUnit + fact*(mom1*dispMom1 - mom1.mag2()*disp -
                         dispPU*perpUnit.Dot(mom1)*mom1 +
                         mom1.mag2()*dispPU*perpUnit           );
  } else {
    u1 = ( fact*( dispPU*perpUnit - disp) ).Cross(mom1 );
    u2 = -(fact*( dispPU*perpUnit - disp) ).Cross(mom0 );
    u = perpUnit;
  }
}
//=============================================================================
