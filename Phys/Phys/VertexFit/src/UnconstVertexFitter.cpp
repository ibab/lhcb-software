// $Id: UnconstVertexFitter.cpp,v 1.3 2004-12-15 15:29:03 pkoppenb Exp $
// Include files
// from Gaudi
#include "GaudiKernel/ToolFactory.h"


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
#include "DaVinciTools/IParticleTransporter.h"
#include "DaVinciTools/IPhotonParams.h"

#include "Event/ProtoParticle.h"
#include "Event/TrgTrack.h"

//--------------------------------------------------------------------
//
//  ClassName  : UnconstVertexFitter
// 
//  Description: Perform an unconstrained vertex fit.
//               
//
//  Author     : S. Amato  
//
//
// Modified by: Luis Fernandez, 07/12/2004 
// - fit with neutral(s) with origin
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
  : GaudiTool( type, name, parent )
    , m_pTransporter(0)
    , m_pPhotonParams(0) 
    , m_gammaID(22){

  declareInterface<IVertexFitter>(this);

  declareProperty("Transporter", m_transporterType = "CombinedTransporter");

  declareProperty("PhotonParamsType", m_PhotonParamsType = "PhotonParameters");
  declareProperty ("ScaleFactor", m_scale = 2.0); // for cov matrix of neutrals with origin
}



//==================================================================
// Initialize
//==================================================================
StatusCode UnconstVertexFitter::initialize() {
  
  StatusCode sc = GaudiTool::initialize();
  if (!sc) return sc;

  m_pTransporter = tool<IParticleTransporter>( m_transporterType, this);
  if(!m_pTransporter) {
    fatal() << "    Unable to retrieve ParticleTransporter  tool" ;
    return StatusCode::FAILURE;
  }

  m_pPhotonParams = tool<IPhotonParams>(m_PhotonParamsType, this);

  if (!m_pPhotonParams){
    fatal() << "Unable to retrieve PhotonParams tool" << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;

}

//==================================================================
// Perform vertex fit between two particles
//==================================================================
StatusCode UnconstVertexFitter::fitVertex( Particle& particle1,
                                           Particle& particle2,
                                           Vertex& myVertex ) {
  ParticleVector pList;
  pList.push_back(&particle1);
  pList.push_back(&particle2);
  
  return fitVertex(pList,myVertex);

}
//==================================================================
// Perform vertex fit between three particles
//==================================================================
StatusCode UnconstVertexFitter::fitVertex( Particle& particle1,
                                           Particle& particle2,
                                           Particle& particle3,
                                           Vertex& myVertex ) {
  ParticleVector pList;
  pList.push_back(&particle1);
  pList.push_back(&particle2);
  pList.push_back(&particle3);
  
  return fitVertex(pList,myVertex);

}
//==================================================================
// Perform vertex fit for a vector of particles.
// If one of them is a resonance, use the daughters to fit
//==================================================================
StatusCode UnconstVertexFitter::fitVertex( const ParticleVector& pList,
                                           Vertex& myVertex ) {

  debug() << "Hello From Vertex Fitter With Daughters" << endreq;

  if (pList.size() < 2) {
    info() << "Particle Vector size is less than 2" << endreq;   
    return StatusCode::FAILURE;
  }
  
  // Split vector in two lists:
  // charged tracks, or composite particles with an endVertex (including neutrals, gammas from e+e-)
  ParticleVector fromChargedOrResoList;

  // neutrals with origin and no endVertex: gammas
  ParticleVector fromNeutralWithOriList;

  // Count number of resonances not to refit when one only needs to add gammas
  int nResonances = splitIntoNeutralsAndCharged(pList,fromChargedOrResoList,fromNeutralWithOriList);
  
  if((fromChargedOrResoList.size()<2) && (!fromNeutralWithOriList.empty())){
    info() << "Needs at least two charged tracks when using neutrals with origin" << endreq;
    return StatusCode::FAILURE;
  }

  // The vertex without neutrals at the beginning
  Vertex fromChargedOrResoVtx;

  // Do not refit if only *one* composite particle and neutrals with origin (e.g. B_s0 -> (phi(1020) -> K+ K-) gamma)
  // special case of only one resonance and neutrals with origin
  bool special = false ;  
  if ((nResonances == 1) && (!fromNeutralWithOriList.empty())){
    Vertex* vp = singleResonanceVertex(pList); 
    if (vp){
      special = true;
      fromChargedOrResoVtx = *vp;
    }
  } // special case of only one resonance and neutrals with origin

  // If not the special case of only one resonance and neutrals with origin
  // Fit the charged tracks and resonances first
  // Vertex from charged tracks and resonances
  if(!special){
    debug() << "Will fit the charged tracks and resonances" << endreq;
    StatusCode sc = doFitVertex(fromChargedOrResoList,fromChargedOrResoVtx);
    if (!sc) return StatusCode::FAILURE;
  }
  
  // If no neutrals
  if (fromNeutralWithOriList.empty()){
    debug() << "No neutrals" << endreq;    

    // Add products to vertex, from original particle list not to destroy already existing resonances
    for (ParticleVector::const_iterator iterP = pList.begin(); iterP != pList.end(); iterP++) {
      fromChargedOrResoVtx.addToProducts(*iterP);
    }

    // Assignment operator: the original product particles are kept
    myVertex = fromChargedOrResoVtx;

  // Now add neutrals
  } else{
    StatusCode sc = addNeutrals(fromNeutralWithOriList,fromChargedOrResoList,fromChargedOrResoVtx);

    // Refit with the neutrals
    info() << "Refit vertex with neutrals" << endreq;
    sc = doFitVertex(fromChargedOrResoList,myVertex);
    if (!sc ) return StatusCode::FAILURE;
    
    // Add products to vertex, from original particle list not to destroy already existing resonances
    for(ParticleVector::const_iterator iterP = pList.begin(); iterP != pList.end(); iterP++) {
      myVertex.addToProducts(*iterP);
    }
  } // else neutrals


  //------------------------------------------------------------------------------------  
  

  return StatusCode::SUCCESS;

}

//==================================================================
// Add neutrals
//==================================================================
StatusCode UnconstVertexFitter::addNeutrals( const ParticleVector& fromNeutralWithOriList, 
                                             ParticleVector& fromChargedOrResoList, 
                                             Vertex& fromChargedOrResoVtx){
  info() << "Will add " << fromNeutralWithOriList.size() << " neutral(s)" << endreq;
  
  // Re-evaluate the photon(s) parameters at the vertex from charged tracks or resonance
  
  for (ParticleVector::const_iterator iNeutral = fromNeutralWithOriList.begin(); 
       iNeutral != fromNeutralWithOriList.end(); 
       iNeutral++) {
    
  // If offline photon with origin
    
    const ProtoParticle* proto = dynamic_cast<const ProtoParticle*>((*iNeutral)->origin());
    
    if (proto){
      
      info() << "Offline neutral ID " << (*iNeutral)->particleID().pid() 
             << " and momentum before re-evaluation " << (*iNeutral)->momentum() << endreq;
      
      StatusCode sc = m_pPhotonParams->process((*iNeutral), &fromChargedOrResoVtx);
      if (!sc ) return StatusCode::FAILURE;      
    
      debug() << "Offline neutral ID " << (*iNeutral)->particleID().pid() 
             << " and momentum after re-evaluation " << (*iNeutral)->momentum() << endreq;
    } else{ // online *TEMPORARY*
      
      Warning("Using temporary hack - waiting for PhotonParams tool");
      debug() << "Online neutral ID " << (*iNeutral)->particleID().pid() 
             << " and momentum " << (*iNeutral)->momentum() << endreq;
      
      // Vertex determined with charged tracks:
      const HepPoint3D& Vtx = fromChargedOrResoVtx.position();      
      // Covariance matrix, determined with charged tracks:
      const HepSymMatrix& VtxCov = fromChargedOrResoVtx.positionErr();      
      // modify the photon parameters
      // Update Point at which the momentum is given in LHCb reference frame
      (*iNeutral)->setPointOnTrack(Vtx);
      // Update Covariance matrix relative to point at which the momentum is given (3x3)
      (*iNeutral)->setPointOnTrackErr(m_scale * VtxCov);
      
    }
    
    // Add this neutral to all the particles
    fromChargedOrResoList.push_back(*iNeutral);      
  }
  
  debug() << "After adding neutrals total number of particles " << fromChargedOrResoList.size() << endreq;
  return StatusCode::SUCCESS;
}

    
//==================================================================
// Do not refit if only *one* composite particle and neutrals with origin (e.g. B_s0 -> (phi(1020) -> K+ K-) gamma)
// special case of only one resonance and neutrals with origin
//==================================================================
Vertex* UnconstVertexFitter::singleResonanceVertex(const ParticleVector& pList){
  debug() << "Only one resonance and neutral(s), won't refit the resonance" << endreq;
  // Get the resonance vertex
  for(ParticleVector::const_iterator iPart = pList.begin(); iPart != pList.end(); iPart++) {    
    int partID = (*iPart)->particleID().pid();    
    if(!((*iPart)->origin() && m_gammaID == partID)){// not neutrals with origin       
      if ((*iPart)->isResonance()){
        Vertex* fromChargedOrResoVtx = (*iPart)->endVertex();
        if (fromChargedOrResoVtx){
          debug() << "Resonance decay vertex position " << fromChargedOrResoVtx->position() 
                  << " , and chi2 " << fromChargedOrResoVtx->chi2() << endreq;
          return fromChargedOrResoVtx;
        } // if resonanceVtx
      } // if resonance
    } // if not neutrals with origin
  } // for ipart  
  return NULL;
}
 
  
//==================================================================
// Perform a fit between a vector of particles 
// Split vector in two lists:
// - charged tracks, or composite particles with an endVertex (including neutrals, gammas from e+e-)
// - neutrals with origin and no endVertex: gammas
//==================================================================
int UnconstVertexFitter::splitIntoNeutralsAndCharged(const ParticleVector& pList, 
                                                     ParticleVector& fromChargedOrResoList, 
                                                     ParticleVector& fromNeutralWithOriList){

  int nResonances = 0 ;
  for(ParticleVector::const_iterator iPart = pList.begin(); iPart != pList.end(); iPart++) {
    
    int partID = (*iPart)->particleID().pid();
    
    if((*iPart)->origin() && m_gammaID == partID){// neutrals with origin
      verbose() << "Particle in list: " << partID << endreq;
      fromNeutralWithOriList.push_back(*iPart);
    }
    else{ // charged or resonance
      verbose() << "Particle in list: " << partID << endreq; 
      if ((*iPart)->isResonance()){
        nResonances++;
        for ( SmartRefVector<Particle>::iterator iResoProducts = (*iPart)->endVertex()->products().begin();
              iResoProducts != (*iPart)->endVertex()->products().end(); 
              iResoProducts++ ){
          fromChargedOrResoList.push_back(*iResoProducts);
        }
      } // if resonance
      else{
        fromChargedOrResoList.push_back(*iPart);     
      } // else charged and not a resonance
    } // else if charged or resonance
  } // for iPart
  
  debug() << "Particle Vector size is " << pList.size() << endreq;
  debug() << "-> Number of charged tracks or resonances products is " << fromChargedOrResoList.size() << endreq;

  if (!fromNeutralWithOriList.empty()){  
    debug() << "-> Number of neutrals with origin is " << fromNeutralWithOriList.size() << endreq;
  }
  return nResonances;
}


//==================================================================
// Perform a fit between a vector of particles 
//==================================================================
StatusCode UnconstVertexFitter::doFitVertex( const ParticleVector& particleList,
                                           Vertex& myVertex ) {

  debug() << "Hello From Vertex Fitter" << endreq;
  
  if (particleList.size() < 2) {
   info() << "Particle Vector size is less than 2" << endreq;   
   return StatusCode::FAILURE;
  } else {
    debug() << "Particle Vector size is " << 
      particleList.size()<< endreq;    
  }
  
    
  // NOTE use of 1..n style indexing in () type brackets.
  
  // get zestimate (The formula is in the LHC-B/TN/95-01
  double zEstimate = getZEstimate (particleList);
  
  debug() << "   zestimate " << zEstimate << endreq;
  
  HepSymMatrix cov(2,0); // 2x2 empty  symmetic matrix
  HepSymMatrix hessian(3,0); // 3x3 empty  symmetic matrix
  HepVector divChi(3,0); //empty vector
  double vertexChi2=0;
  HepVector vertex(3,0);
  
  ParticleVector::const_iterator iterP;
  for(iterP = particleList.begin(); iterP != particleList.end(); iterP++) {
    

    // transport particle to zEstimate 

    Particle transParticle;
    StatusCode sctrans = m_pTransporter->transport(iterP,
                                                   zEstimate,
                                                   transParticle);

    if ( !sctrans.isSuccess() ) {
      debug() << "Track extrapolation failed" << endreq;
      return sctrans;
    }
    
    HepPoint3D newPoint =transParticle.pointOnTrack();
    HepSymMatrix newpointErr = transParticle.pointOnTrackErr();
    HepSymMatrix newSlpMomCorrErr = transParticle.slopesMomErr();
    HepMatrix newPointSlpMomCorrErr = transParticle.posSlopesCorr();
    


    // Get the track covariance matrix
    cov(1,1)=newpointErr(1,1); 
    cov(1,2)=newpointErr(1,2);
    cov(2,2)=newpointErr(2,2);
    
    //std::cout << " covariance Matrix "<<(*iterP)->pointOnTrackErr()<<endl;

    //std::cout << " new Cov Matrix "<< newpointErr <<endl;

    int iFail;
    cov.invert(iFail); 
    if( 0 != iFail ){
      warning() << "Could not invert covariance matrix" << endreq;
      return StatusCode::FAILURE;
    }
    //    std::cout << " new Cov Matrix Inverted "<< cov <<endl;
    
    // see LHC-B/TN/95-01 for equations
    // note divChi[2] (ie z) sign  (wrong in LHCb note)
    hessian(1,1) += cov(1,1);
    hessian(1,2) += cov(1,2);
    hessian(2,2) += cov(2,2);
    hessian(1,3) -= (cov(1,1)*transParticle.slopeX() +
                     cov(1,2)*transParticle.slopeY());
    hessian(2,3) -= (cov(2,2)*transParticle.slopeY() +
                     cov(1,2)*transParticle.slopeX());
    hessian(3,3) += (cov(1,1)*transParticle.slopeX()*
                     transParticle.slopeX() +
                     cov(2,2)*transParticle.slopeY()*
                     transParticle.slopeY() +
                     2.*cov(1,2)*transParticle.slopeX()*
                     transParticle.slopeY());
    
    //    HepPoint3D firstp = (*iterP)->pointOnTrack();
    //    debug() << " x firstp  " << firstp.x() << endreq;
    
    
    divChi(1) += (cov(1,1)*newPoint.x() +  cov(1,2)*newPoint.y());
    divChi(2) += (cov(2,2)*newPoint.y() +cov(1,2)*newPoint.x());
    divChi(3) -= (cov(1,1)*newPoint.x()* transParticle.slopeX() +
                  cov(2,2)*newPoint.y()* transParticle.slopeY() +
                  cov(1,2)*(newPoint.x()*transParticle.slopeY() +
                            newPoint.y()*  transParticle.slopeX()));
    
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
  
  debug() << "vertexChi2 " << vertexChi2 << endreq;
  
  //offset with  initally estimated z: all calculated relative to that
  vertex(3)+=zEstimate;
  debug() << "vertex position " << vertex(1) << "  " << vertex(2) 
      <<  " "   <<vertex(3) << endreq;
  
  // update tracks : x,y from vertex, extrapolate to z, I hope
  if(vertex(3)<-500. || vertex(3)>10000.){
    debug() << "Bad vertex reconstruction." << endreq;
    debug() << "Final vertex estimate " << vertex(1) <<" "
        << vertex(2) <<" " << vertex(3) <<endreq;
    return StatusCode::FAILURE;
  }
  
  int inv;
  HepSymMatrix errMat = hessian.inverse(inv);
  if (inv != 0) {
    debug() << "Could not invert hessian matrix" << endreq;
    return StatusCode::FAILURE; 
  }

  myVertex.setPosition(HepPoint3D(vertex(1),vertex(2),vertex(3)));
  myVertex.setChi2(vertexChi2);
  myVertex.setPositionErr(errMat);

  int ndof = (2*particleList.size())-3;
  myVertex.setNDoF(ndof);

  myVertex.setType(Vertex::Decay);  
  
   


 
  return StatusCode::SUCCESS;
  
}
//=======================================================================
// First estimate of z position of vertex
//=======================================================================
double UnconstVertexFitter::getZEstimate( const ParticleVector& particleList) {

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
    info() << "Unable to make z estimate, set it to 0.0 " << endreq;
    return 0.;
  }
// **** End Estimate the Vertex Position *****
}





