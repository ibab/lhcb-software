// $Id: UnconstVertexFitter.cpp,v 1.2 2002-03-28 17:48:49 gcorti Exp $
// Include files
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"

#include "DaVinciTools/IParticleTransporter.h"

// from Event
#include "Event/Particle.h"
#include "Event/Vertex.h"
// from CLHEP
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Vector/ThreeVector.h" 
// local
#include "UnconstVertexFitter.h"

//--------------------------------------------------------------------
//
//  ClassName  : UnconstVertexFitter
// 
//  Description: Perform an unconstrained vertex fit.
//               
//
//  Author     : S. Amato  
//
//--------------------------------------------------------------------

//==================================================================
// Instantiation of a static factory class used by clients to create
// instances of this service
//==================================================================
static ToolFactory<UnconstVertexFitter> s_factory;
const IToolFactory& UnconstVertexFitterFactory = s_factory;

//==================================================================
// Standard Constructor
//==================================================================
UnconstVertexFitter::UnconstVertexFitter(const std::string& type, 
                                         const std::string& name, 
                                         const IInterface* parent) 
  : AlgTool( type, name, parent ) {

  declareInterface<IVertexFitter>(this);

  // This tool needs to use internally the ToolSvc to retrieve the
  // transporter tool
  
  m_pToolSvc = 0; 
  StatusCode sc = StatusCode::FAILURE;
  sc = serviceLocator()->service( "ToolSvc", m_pToolSvc );
   

}

//==================================================================
// 
// 
//==================================================================
StatusCode UnconstVertexFitter::fitVertex( const ParticleVector& particleList,
                                           Vertex& myVertex ) {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "Hello From Vertex Fitter" << endreq;
  
  
  if( 0 ==  m_pToolSvc) {
    log << MSG::FATAL << "    Unable to locate Tool Service" ;
    return StatusCode::FAILURE;
  }
  StatusCode sc = m_pToolSvc->retrieveTool("ParticleTransporter", 
                                           m_pTransporter);
  if(sc.isFailure()) {
    log << MSG::FATAL << "    Unable to retrieve ParticleTransporter  tool" ;
    return sc;
  }
  
  // NOTE use of 1..n style indexing in () type brackets.
  
  // get zestimate (The formula is in the LHC-B/TN/95-01
  double zEstimate = getZEstimate (particleList);
  
  log << MSG::DEBUG << "   zestimate " << zEstimate << endreq;  

  
  HepSymMatrix cov(2,0); // 2x2 empty  symmetic matrix
  HepSymMatrix hessian(3,0); // 3x3 empty  symmetic matrix
  HepVector divChi(3,0); //empty vector
  double vertexChi2=0;
  HepVector vertex(3,0);
  
  ParticleVector::const_iterator iterP;
  for(iterP = particleList.begin(); iterP != particleList.end(); iterP++) {
    
    // transport particle to zEstimate 
    HepPoint3D newPoint;
    HepSymMatrix newpointErr(3, 0.0);
    HepSymMatrix newSlpMomCorrErr(3, 0.0);
    HepMatrix newPointSlpMomCorrErr(3, 3, 0.0);
    StatusCode sctrans = m_pTransporter->transport(iterP, 
                                                   zEstimate,
                                                   newPoint,
                                                   newpointErr,
                                                   newSlpMomCorrErr,
                                                   newPointSlpMomCorrErr);
    if ( !sctrans.isSuccess() ) {
      log << MSG::WARNING << "Track extrapolation failed" << endreq;
      return sctrans;
    }
    
    // Get the track covariance matrix
    cov(1,1)=newpointErr(1,1); 
    cov(1,2)=newpointErr(1,2);
    cov(2,2)=newpointErr(2,2);
    
    //std::cout << " covariance Matrix "<<(*iterP)->pointOnTrackErr()<<endl;

    //std::cout << " new Cov Matrix "<< newpointErr <<endl;

    int iFail;
    cov.invert(iFail); 
    if( 0 != iFail ){
      log << MSG::WARNING << "Could not invert covariance matrix" << endreq;
      return StatusCode::FAILURE;
    }
    //    std::cout << " new Cov Matrix Inverted "<< cov <<endl;
    
    // see LHC-B/TN/95-01 for equations
    // note divChi[2] (ie z) sign  (wrong in LHCb note)
    hessian(1,1) += cov(1,1);
    hessian(1,2) += cov(1,2);
    hessian(2,2) += cov(2,2);
    hessian(1,3) -= (cov(1,1)*(*iterP)->slopeX() +
                     cov(1,2)*(*iterP)->slopeY());
    hessian(2,3) -= (cov(2,2)*(*iterP)->slopeY() +
                     cov(1,2)*(*iterP)->slopeX());
    hessian(3,3) += (cov(1,1)*(*iterP)->slopeX()*
                     (*iterP)->slopeX() +
                     cov(2,2)*(*iterP)->slopeY()*
                     (*iterP)->slopeY() +
                     2.*cov(1,2)*(*iterP)->slopeX()*
                     (*iterP)->slopeY());
    
    //    HepPoint3D firstp = (*iterP)->pointOnTrack();
    //    log << MSG::DEBUG << " x firstp  " << firstp.x() << endreq;
    
    
    divChi(1) += (cov(1,1)*newPoint.x() +  cov(1,2)*newPoint.y());
    divChi(2) += (cov(2,2)*newPoint.y() +cov(1,2)*newPoint.x());
    divChi(3) -= (cov(1,1)*newPoint.x()* (*iterP)->slopeX() +
                  cov(2,2)*newPoint.y()* (*iterP)->slopeY() +
                  cov(1,2)*(newPoint.x()*(*iterP)->slopeY() +
                            newPoint.y()*  (*iterP)->slopeX()));
    
    vertexChi2 += (cov(1,1)*newPoint.x()* newPoint.x() +
                   cov(2,2)*newPoint.y()*newPoint.y() +
                   2.*cov(1,2)*newPoint.x()*newPoint.y() );
  }
  
  
  
  
  // now solve with solve! (HepMatrix::solve as it happens)
  // it inverts the matrix and multiplies by the vector using LU decompositon
  HepMatrix fullHessian = hessian; // need normal matrix for this
  vertex = solve(fullHessian,divChi);
  
  // calculate chi2 terms : from axvcalc.F
  
  // which is VChiZr + (vertex)^T(Hessian)(vertex) - 2*(vertex)^T(divChi)
  vertexChi2 += dot(vertex,(hessian*vertex)) - 2*dot(vertex,divChi);
  
  log << MSG::DEBUG << "vertexChi2 " << vertexChi2 << endreq;
  
  //offset with  initally estimated z: all calculated relative to that
  vertex(3)+=zEstimate;
  log << MSG::DEBUG << "vertex position" <<vertex(1) << "  " << vertex(2) 
      <<  " "   <<vertex(3) << endreq;
  
  // update tracks : x,y from vertex, extrapolate to z, I hope
  if(vertex(3)<-500. || vertex(3)>10000.){
    log << MSG::DEBUG << "Bad vertex reconstruction." << endreq;
    log << MSG::DEBUG << "Final vertex estimate " << vertex(1) <<" "
        << vertex(2) <<" " << vertex(3) <<endreq;
    return StatusCode::FAILURE;
  }
  // Remember to swim:
  // for(iterP = particleList.begin(); iterP != particleList.end(); iterP++)
  // {
  //    sc = (*iterP)->extrapolate(fExtrapolator,vertex(3));
  //    if(!sc.isSuccess()){
  //      log << MSG::WARNING << "Track extrapolation failed" << endreq;
  //      myVertex.setPosition(HepPoint3D(999.,999.,999.));
  //      myVertex.setChisquare(999.);
  //      return StatusCode::FAILURE;
  
  //    }
  //  }
  
  myVertex.setPosition(HepPoint3D(vertex(1),vertex(2),vertex(3)));
  myVertex.setChi2(vertexChi2);
  myVertex.setPositionErr(hessian);
  myVertex.setNDoF(3);
  myVertex.setType(Vertex::Decay);  
  for(iterP = particleList.begin(); iterP != particleList.end(); iterP++) {
    myVertex.addToProducts(*iterP);
  }
  
   


 
  return StatusCode::SUCCESS;
  
}
//=======================================================================
double UnconstVertexFitter::getZEstimate( const ParticleVector& particleList) {

  MsgStream log(msgSvc(), name());

  // **** Estimate the Vertex Position *****
  
  double xPosition = 0;
  double yPosition = 0;
  // sumSquaredSlopes = Sum(x'**2 + y'**2)
  double sumSquaredSlopes = 0.;
  // sumCrossedProduct = Sum(xx' + yy')
  double  sumCrossedProduct = 0;
  double sumX = 0;
  double sumY = 0;
  double sumSlopeX = 0;
  double sumSlopeY = 0;

  ParticleVector::const_iterator iterP;
  for(iterP = particleList.begin(); iterP != particleList.end(); iterP++) {
    
    // x position
    xPosition = (*iterP)->pointOnTrack().x() -  
      ((*iterP)->slopeX()*(*iterP)->pointOnTrack().z());
    
    yPosition = (*iterP)->pointOnTrack().y() -  
      ((*iterP)->slopeY()*(*iterP)->pointOnTrack().z());
    
    sumSquaredSlopes += ( (*iterP)->slopeX()*(*iterP)->slopeX() +
                          (*iterP)->slopeY()*(*iterP)->slopeY());
    
    sumCrossedProduct +=  ((*iterP)->slopeX()*xPosition) +
      ((*iterP)->slopeY()*yPosition);
    
    sumX += xPosition;
    sumY += yPosition;
    sumSlopeX += (*iterP)->slopeX();
    sumSlopeY += (*iterP)->slopeY();
    
  }
  // det = Sum(x'**2 + y'**2)  -   [(Sum(x'))**2 + (Sum(y'))**2]/n
  double det = sumSquaredSlopes - ((sumSlopeX*sumSlopeX + sumSlopeY*sumSlopeY))/
    particleList.size();
  double zEstimate = 0;
  if (det != 0) {
    // zEstimate = 
    return zEstimate = (((sumX*sumSlopeX + sumY*sumSlopeY)/particleList.size()) 
                        - sumCrossedProduct) /det;
  }
  else {
    log << MSG::INFO << "Unable to make z estimate, set it to 0.0 " << endreq;
    return 0.;
  }
// **** End Estimate the Vertex Position *****
}




