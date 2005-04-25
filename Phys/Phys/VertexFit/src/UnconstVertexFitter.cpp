// $Id: UnconstVertexFitter.cpp,v 1.7 2005-04-25 13:52:54 pkoppenb Exp $
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
#include "Kernel/IParticleTransporter.h"
#include "Kernel/IPhotonParams.h"

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
// Modified by: Luis Fernandez, 07/12/2004, reviewed 20/04/2005
//              special treatment for gammas
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
  declareProperty("ScaleFactor", m_scale = 2.0); 
  declareProperty("UseDaughters", m_useDaughters = true);
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
    fatal() << "Unable to retrieve PhotonParams tool" << endmsg;
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

  debug() << "Hello From Vertex Fitter With Daughters" << endmsg;

  int npList = pList.size();
  if(npList < 2) {
    info() << "Original Particle Vector size is less than 2" << endmsg;   
    return StatusCode::FAILURE;
  }
  else{
    debug() << "Original Particle Vector size is " << npList << endmsg;   
  }
  
  // All the gammas with an origin, including gammas from resonances
  // such as gammas from pi0
  ParticleVector gammasToAdd;
  // Particles to use in the fit: final states from resonances or stable composites
  ParticleVector partsToFit;
  
  // Count how many composites not of the type X -> nGammas:
  // ->  if only one and gamma(s), do not refit, but just add the gamma(s)
  int nCompositesNotToNGammas = 0;

  // Loop of input particles
  for(ParticleVector::const_iterator iPart = pList.begin(); iPart != pList.end(); ++iPart){  
    
    // The pointer should exist, no protection
    Particle* part = *iPart;
    
    int partID = part->particleID().pid();
    debug() << "Considering input particle ID " << partID << endmsg;
    
    // Gammas with origin
    if(part->origin() && m_gammaID == partID){
      debug() << " input is a gamma with origin" << endmsg;
      // Add it to gammas list
      gammasToAdd.push_back(part);
    }
    // Resonances with an endVertex
    else if(m_useDaughters && part->isResonance() && part->endVertex()){
      debug() << " input is a resonance, will find all its final states or long-lived composites descendants" << endmsg; 

      // Get the final states (particles with an origin) and the long-lived daughters of this resonance
      ParticleVector productsForFit;
      StatusCode scFS = getProductsForFit(part, productsForFit);
      if (!scFS){
        fatal() << "Failed in getProductsForFit" << endmsg;
        return StatusCode::FAILURE;
      }
      debug() << "  this resonance has " << productsForFit.size() 
              << " final states or long-lived composites in its decay tree" << endmsg;
      
      int ng = 0; // number of final states that are gammas
      int ne = 0; // number of final state or long-lived composites that are not gammas
      
      // Now separate gammas and the rest
      for(ParticleVector::iterator iProd = productsForFit.begin(); iProd != productsForFit.end(); ++iProd){  
        
        // All the final states have an origin or the long-lived composites
        Particle* state = *iProd;
        
        verbose() << "    resonance's descendant final state or long-lived composite ID " <<state->particleID().pid() << endmsg;  

        // Gammas with origin from the resonance
        if(m_gammaID == state->particleID().pid()){
          ng++;
          // Add it to gammas list
          gammasToAdd.push_back(state);
        }
        // Else with origin but not gammas, or long-lived composites from the resonance
        else{
          ne++;
          // Add it to the particles to use for fit
          partsToFit.push_back(state);
        }
      } // iProd

      if(ng>0 && ne == 0) debug() << "  this resonance only decays to " << ng << " gamma(s) with origin" << endmsg;
      else if(ng>0 && ne !=0) debug() << "  this resonance has " << ng << " gamma(s) with origin and " 
                                      << ne << " particles with origin or long-lived composites that are not gammas"
                                      << endmsg;
      else debug() << "  this resonance has " << ne 
                   << " particles with origin or long-lived composites that are not gammas" << endmsg;

      // Increment counter
      if( !(ng>0 && ne == 0) ){
        nCompositesNotToNGammas++;
      }
    }
    // Long-lived composites with an endVertex
    else if(m_useDaughters && !(part->isResonance()) && part->endVertex()){
      debug() << " input is a long-lived composite" << endmsg;
      // Add it to the particles to use for fit
      partsToFit.push_back(part);
      // Increment counter
      nCompositesNotToNGammas++;
    }
    // All the other cases: charged particles with origin, ...
    else{
      
      if(m_useDaughters){
        debug() << " input is not a gamma and has an origin" << endmsg; 
      }
      else{
        debug() << " input is not a gamma, and not using daughters" << endmsg; 
      }
      
     // Add it to the particles to use for fit
     partsToFit.push_back(part);
    }
  } // iParts
  
  // Number of particles to be used for the vertex
  int nPartsToFit = partsToFit.size();
   
  // Number of gammas to be added to the vertex
  int nGammasToAdd = gammasToAdd.size();
  
  // Check whether we have enough particles for the fit or to add gammas. Cases:
  // a) - one composite and gammas: just add them to the existing vertex
  //    - only one composite that does not decay to nGamma: just add them to the existing vertex
  // b) - not enough particles
  // c) - at least two particle to consider for the fit, and maybe gammas to add

  // In some cases we don't want to refit the vertex to save time
  bool noRefit = false;
  
  // Check this one first!
  if(nCompositesNotToNGammas == 1 && nGammasToAdd > 0){
    debug() << "Special case: won't refit vertex but re-evaluate " 
            << nGammasToAdd << " gamma(s) to the existing vertex" << endmsg;
    // Do not refit the vertex
    noRefit = true;
  }
  else if ((nPartsToFit == 0) || (nPartsToFit == 1)){

    // fatal() << "Not enough particles to fit!" << endmsg;
    // return StatusCode::FAILURE;

    // *TEMPORARY*
    // We should not try to fit only gammas ...
    Warning("Not enough particles to fit! Trying to fit only gammas!");
    // Dirty fix for some channels trying to fit e.g. pi0 -> gamma gamma
    debug() << "Will consider " << nGammasToAdd 
            << " gammas for fit assuming they have already been moved to some vertex" << endmsg;
    StatusCode sc = doFitVertex(gammasToAdd, myVertex);
    if(!sc) return StatusCode::FAILURE;        

    // Add products to vertex, from original particle list not to destroy already existing resonances
    for(ParticleVector::const_iterator iterP = pList.begin(); iterP != pList.end(); ++iterP){
      debug() << " adding to vertex products particle from original list with ID " << (*iterP)->particleID().pid() << endmsg;
      // Add the product from the original list
      myVertex.addToProducts(*iterP);
    }

    debug() << "Returning vertex size: " << myVertex.products().size() << " with chi2: "
            << myVertex.chi2() << endmsg;
    debug() << "Returned vertex position: " << myVertex.position() << endmsg;
    debug() << "Returned vertex error: " <<  myVertex.positionErr() << endmsg;
    
    return StatusCode::SUCCESS;
    // End *TEMPORARY*
  }
  else{
    debug() << "Number of non-gammas descendants that will be in the vertex: " << nPartsToFit << endmsg;    
    debug() << "Number of gammas descendants that will be re-evaluated at the vertex: " << nGammasToAdd << endmsg;    
  }

  // Now we can fit and/or/not add gammas
  
  // The vertex: either obtained from the fit or already existing for the special case
  Vertex vtx;
  
  // Get the existing vertex in case of no refit, excluding X->n gammas
  if(noRefit){
    Vertex* vtemp = singleCompositeVertex(pList); 
    if(vtemp) vtx = *vtemp;
    else{
      fatal() << "No existing vertex found!" << endmsg;
      return StatusCode::FAILURE;
    }
  }
  // Else not using existing vertex
  // Fit the particles to consider
  else{
    debug() << "Will consider " << nPartsToFit << " particles for fit" << endmsg;
    StatusCode sc = doFitVertex(partsToFit, vtx);
    if(!sc) return StatusCode::FAILURE;    
  }

  // If no gammas to add
  if(nGammasToAdd == 0){
    debug() << "No gammas to add, will add original particles to vertex" << endmsg;  

    // Add products to vertex, from original particle list not to destroy already existing resonances
    for(ParticleVector::const_iterator iterP = pList.begin(); iterP != pList.end(); ++iterP){
      debug() << " adding to vertex products particle from original list with ID " << (*iterP)->particleID().pid() << endmsg;
      // Add the product from the original list
      vtx.addToProducts(*iterP);
    }
    // Assignment operator: the original product particles are kept
    myVertex = vtx;

  } // if no gammas to add
  // Now add gammas
  else{

    // Just add gammas:
    StatusCode sc = moveGammas(gammasToAdd, vtx);
    if(!sc) return StatusCode::FAILURE;

    // Here the vertex vtx already has some products since no refit
    // -> set the different parameters by hand, products added later
    myVertex.setPosition(vtx.position());
    myVertex.setChi2(vtx.chi2());
    myVertex.setPositionErr(vtx.positionErr());
    myVertex.setNDoF(vtx.nDoF());
    myVertex.setType(Vertex::Decay);

    // Warning: in case if one of the resonances to make the vertex is X -> ngammas, 
    // its momentum is *not* recomputed with the transported photons
    // X -> ngammas are not moved to the mother vertex
    debug() << "After moving gammas, will add original particles to vertex" << endmsg;  

    // Add products to vertex, from original particle list not to destroy already existing resonances
    for(ParticleVector::const_iterator iterP = pList.begin(); iterP != pList.end(); ++iterP){
      debug() << " adding to vertex products particle from original list with ID " << (*iterP)->particleID().pid() << endmsg;
      // Add the product from the original list
      myVertex.addToProducts(*iterP);
    } // for iterP
  } // else neutrals
  
  debug() << "Returning vertex size: " << myVertex.products().size() << " with chi2: "
          << myVertex.chi2() << endmsg;
  debug() << "Returned vertex position: " << myVertex.position() << endmsg;
  debug() << "Returned vertex error: " <<  myVertex.positionErr() << endmsg;

  return StatusCode::SUCCESS;

}

//==================================================================
// Get recursively all the final states (with origin) or long-lived 
// daughters from the resonances in the original list
//==================================================================
StatusCode UnconstVertexFitter::getProductsForFit(Particle*& composite, ParticleVector& productsForFit){

  // This should never be the case:
  if(!composite->endVertex()){
    return StatusCode::FAILURE;
  }

  ParticleVector source;
  ParticleVector target;
 
  source.push_back(composite);
 
  // The first iteration is for the composite particle, which has an endVertex
  do {
    target.clear();
    for(Particles::iterator isource = source.begin(); isource != source.end(); ++isource){
      
      // A composite which is a resonance
      if((*isource)->endVertex() && (*isource)->isResonance()){
 
        ParticleVector tmp;
        SmartRefVector<Particle>::iterator it;
        for(it = (*isource)->endVertex()->products().begin();
            it != (*isource)->endVertex()->products().end();
            ++it){
          tmp.push_back(*it);
        }
        
        Particles::iterator itmp;
        for(itmp = tmp.begin(); itmp!=tmp.end(); ++itmp){
          target.push_back(*itmp);
 
          // Add the final states with origin
          if((*itmp)->origin()) productsForFit.push_back(*itmp);
       }
      } // if endVertex
      // else a stable composite: use it without going down (e.g. the D0 for D*(D0 pi) pi)
      else if((*isource)->endVertex() && !(*isource)->isResonance()){
        // Add the long-lived composite
        productsForFit.push_back(*isource);
      }
   } // isource
    source = target;
  } while(target.size() > 0);

  return StatusCode::SUCCESS;  
}


//==================================================================
// Get the reference vertex for the case of one composite and gammas
//==================================================================
Vertex* UnconstVertexFitter::singleCompositeVertex(const ParticleVector& pList){

  // Get the composite vertex
  for(ParticleVector::const_iterator iPart = pList.begin(); iPart != pList.end(); iPart++) {    

    Particle* composite = *iPart;

    // Redundant checks
    // Should be a composite
    if(!composite->endVertex()) continue;
    // Should not have an origin
    if(composite->origin()) continue;

    Vertex* V = composite->endVertex();

    //Should not only decay to gammas    
    unsigned int ngammas = 0;
    SmartRefVector<Particle>::iterator iProducts;
    for(iProducts = V->products().begin(); iProducts != V->products().end(); ++iProducts){
      if((*iProducts)->origin() && m_gammaID == (*iProducts)->particleID().pid()) ngammas++;
    } // iProducts
    
    // Do not use if only decays to gammas, e.g. J/Psi eta(2g)
    if(ngammas == V->products().size()) continue;
    
    // We have our vertex
    debug() << " composite ID " << composite->particleID().pid() 
            << " will be used to add gammas" << endmsg;
    
    debug() << " composite decay vertex position: " << V->position() 
            << " , and chi2 " << V->chi2() << endmsg;

    return V;
    
  } // for ipart  

  return NULL;
}

//==================================================================
// Moves gammas to existing vertex
//==================================================================
StatusCode UnconstVertexFitter::moveGammas(const ParticleVector& gammasToAdd, 
                                           Vertex& vtx){

  debug() << "Will add " << gammasToAdd.size() << " gammas(s)" << endmsg;
  
  // Re-evaluate the gammas(s) parameters at the vertex from charged tracks or resonance
  
  ParticleVector::const_iterator iGammas;
  for(iGammas = gammasToAdd.begin(); iGammas != gammasToAdd.end(); ++iGammas){

    // If offline gamma with origin
    const ProtoParticle* proto = dynamic_cast<const ProtoParticle*>((*iGammas)->origin());
    if(proto){
      
      verbose() << " offline gamma ID " << (*iGammas)->particleID().pid() 
                << " and momentum before re-evaluation " << (*iGammas)->momentum() << endmsg;
      
      StatusCode sc = m_pPhotonParams->process((*iGammas), &vtx);
      if (!sc) return StatusCode::FAILURE;      
      
      verbose() << " offline gamma ID " << (*iGammas)->particleID().pid() 
                << " and momentum after re-evaluation " << (*iGammas)->momentum() << endmsg;
    } // offline 
    // online *TEMPORARY*
    else{ 
      Warning("Using temporary hack - waiting for online PhotonParams tool");
      verbose() << " online gamma ID " << (*iGammas)->particleID().pid() 
                << " and momentum " << (*iGammas)->momentum() << endmsg;

      // Only the pointOnTrack and pointOnTrackErr are modified
      // and not the momentum, momentumErr, posMomCorr
      
      // Vertex determined with charged tracks:
      const HepPoint3D& Vtx = vtx.position();      
      // Covariance matrix, determined with charged tracks:
      const HepSymMatrix& VtxCov = vtx.positionErr();      
      // modify the photon parameters
      // Update Point at which the momentum is given in LHCb reference frame
      (*iGammas)->setPointOnTrack(Vtx);
      // Update Covariance matrix relative to point at which the momentum is given (3x3)
      (*iGammas)->setPointOnTrackErr(m_scale * VtxCov);

    }
  } // else online
  
  return StatusCode::SUCCESS;
}

//==================================================================
// Perform a fit between a vector of particles 
//==================================================================
StatusCode UnconstVertexFitter::doFitVertex( const ParticleVector& particleList,
                                           Vertex& myVertex ) {

  debug() << "Hello From Vertex Fitter" << endmsg;
  
  if (particleList.size() < 2) {
   info() << "Particle Vector size is less than 2" << endmsg;   
   return StatusCode::FAILURE;
  } else {
    debug() << "Particle Vector size is " << particleList.size()<< endmsg;    
  }
    
  // NOTE use of 1..n style indexing in () type brackets.
  
  // get zestimate (The formula is in the LHC-B/TN/95-01
  double zEstimate = getZEstimate (particleList);
  
  debug() << "z estimate is " << zEstimate << endmsg;
  
  HepSymMatrix hessian(3,0); // 3x3 empty  symmetic matrix
  HepVector divChi(3,0); //empty vector
  double vertexChi2=0;
  
  ParticleVector::const_iterator iterP;
  for(iterP = particleList.begin(); iterP != particleList.end(); iterP++) {
    // transport particle to zEstimate
    Particle transParticle;
    verbose() << " Input XYZ " << (*iterP)->pointOnTrack() << endmsg ;
    verbose() << " Input Err " << (*iterP)->pointOnTrackErr() << endmsg ;
    StatusCode sctrans = m_pTransporter->transport(iterP,zEstimate,transParticle);
    verbose() << "Output XYZ " << transParticle.pointOnTrack() << endmsg ;
    verbose() << "Output Err " << transParticle.pointOnTrackErr() << endmsg ;

    if ( !sctrans.isSuccess() ) {
      debug() << "Track extrapolation failed" << endmsg;
      return sctrans;
    }
    StatusCode sc = matrixMath(hessian,divChi,vertexChi2,transParticle);
    if (!sc) return sc ;
  }
  if (vertexChi2<0) {
    warning() << "Vertex chi2 is negative: " << vertexChi2 << endmsg ;
    return StatusCode::FAILURE ;
  }
  
  int ndof = (2*particleList.size())-3;
  return makeVertex( myVertex,zEstimate,ndof,hessian,divChi,vertexChi2);
  
}
//=======================================================================
// Vertex math
//=======================================================================
StatusCode UnconstVertexFitter::makeVertex( Vertex& myVertex, double zEstimate,
                                           int ndof,
                                           HepSymMatrix& hessian, 
                                           HepVector& divChi, double vertexChi2){
  // now solve with solve! (HepMatrix::solve as it happens)
  // it inverts the matrix and multiplies by the vector using LU decompositon
  HepMatrix fullHessian = hessian; // need normal matrix for this
  debug() << "Hessian is  :" << hessian << endmsg ;
  debug() << "divChi is   :" << divChi << endmsg;
  HepVector vertex(3,0) ;
  vertex = solve(fullHessian,divChi);
  debug() << "solution is :" << vertex << endmsg; // FAST //
  
  // calculate chi2 terms : from axvcalc.F
  
  // which is VChiZr + (vertex)^T(Hessian)(vertex) - 2*(vertex)^T(divChi)
  vertexChi2 += dot(vertex,(hessian*vertex)) - 2*dot(vertex,divChi);
  
  debug() << "vertexChi2 " << vertexChi2 << endmsg;
  
  //offset with  initally estimated z: all calculated relative to that
  vertex(3)+= zEstimate;
  debug() << "vertex position " << vertex(1) << "  " << vertex(2) 
      <<  " "   <<vertex(3) << endmsg;
  
  // update tracks : x,y from vertex, extrapolate to z, I hope
  if(vertex(3)<-500. || vertex(3)>10000.){
    debug() << "Bad vertex reconstruction." << endmsg;
    debug() << "Final vertex estimate " << vertex(1) <<" "
        << vertex(2) <<" " << vertex(3) <<endmsg;
    return StatusCode::FAILURE;
  }
  
  int inv;
  HepSymMatrix errMat = hessian.inverse(inv);
  if (inv != 0) {
    debug() << "Could not invert hessian matrix" << endmsg;
    return StatusCode::FAILURE; 
  }

  myVertex.setPosition(HepPoint3D(vertex(1),vertex(2),vertex(3)));
  myVertex.setChi2(vertexChi2);
  myVertex.setPositionErr(errMat);

  myVertex.setNDoF(ndof);

  myVertex.setType(Vertex::Decay);
  return StatusCode::SUCCESS ;
}

//=======================================================================
// matrix math
//=======================================================================
StatusCode UnconstVertexFitter::matrixMath(HepSymMatrix& hessian, 
                                           HepVector& divChi, 
                                           double& vertexChi2, 
                                           Particle& transParticle) {
   
  HepPoint3D newPoint = transParticle.pointOnTrack();
  HepSymMatrix newpointErr = transParticle.pointOnTrackErr();
  HepSymMatrix newSlpMomCorrErr = transParticle.slopesMomErr();
  HepMatrix newPointSlpMomCorrErr = transParticle.posSlopesCorr();

  verbose() << "Particle " << transParticle.particleID().pid() << " " << transParticle.momentum() << endmsg ;
  verbose() << "ErrMat: " << newpointErr << endmsg ;
  
  HepSymMatrix cov(2,0); // 2x2 empty  symmetic matrix
  // Get the track covariance matrix
  cov(1,1)=newpointErr(1,1);
  cov(1,2)=newpointErr(1,2);
  cov(2,2)=newpointErr(2,2);
  
  int iFail;
  cov.invert(iFail); 
  if( 0 != iFail ){
    warning() << "Could not invert covariance matrix" << endmsg;
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
  //    debug() << " x firstp  " << firstp.x() << endmsg;
  
  
  divChi(1) += (cov(1,1)*newPoint.x() +  cov(1,2)*newPoint.y());
  divChi(2) += (cov(2,2)*newPoint.y() +cov(1,2)*newPoint.x());
  divChi(3) -= (cov(1,1)*newPoint.x()* transParticle.slopeX() +
                cov(2,2)*newPoint.y()* transParticle.slopeY() +
                cov(1,2)*(newPoint.x()*transParticle.slopeY() +
                          newPoint.y()*  transParticle.slopeX()));
  
  vertexChi2 += (cov(1,1)*newPoint.x()*newPoint.x() +
                 cov(2,2)*newPoint.y()*newPoint.y() +
                 2.*cov(1,2)*newPoint.x()*newPoint.y() ); 

  if (vertexChi2<0) debug() << "Vertex chi2 got negative: " << vertexChi2 << endmsg ;
  else verbose() << "Vertex chi2 is positive: " << vertexChi2 << endmsg ;
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
    info() << "Unable to make z estimate, set it to 0.0 " << endmsg;
    return 0.;
  }
// **** End Estimate the Vertex Position *****
}
