// $Id: TrgVertexFitter.cpp,v 1.12 2006-04-05 14:56:49 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 


#include "Event/Vertex.h"
#include "Event/Particle.h"

#include "Kernel/IParticleStuffer.h" 

// local
#include "TrgVertexFitter.h"

using namespace LHCb ;

//-----------------------------------------------------------------------------
// Implementation file for class : TrgVertexFitter
//
//  THIS TOOL IS MEANT FOR HLT, WHERE TRACKS ARE ASSUMED TO HAVE A
//  'CYLINDRICAL' ERROR, THAT IS, THE COVARIANCE MATRIX HAS
//  COV(1,1)=COV(2,2)!=0 AND ZERO IN ANY OTHER ELEMENT
//
//  The tool should work for other tracks, but correlations 
//  will be neglected
//  
//  The fact that COV(1,1)=COV(2,2) is not assumed anywhere, allowing the
//  two elements to be different, useful for instance for composites
//
//  Some documentation available in a talk at
//  http://agenda.cern.ch/fullAgenda.php?ida=a05940#2005-02-07
//
// 2005-01-31 : Hugo Ruiz Perez
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<TrgVertexFitter>          s_factory ;
const        IToolFactory& TrgVertexFitterFactory = s_factory ; 

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrgVertexFitter::TrgVertexFitter( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : GaudiTool ( type, name , parent )
  , m_photonID(22)
  , m_stuffer()
{
  declareInterface<IVertexFit>(this);
  declareProperty("useDaughters", m_useDaughters = true);
}
//=============================================================================
// Destructor
//=============================================================================
TrgVertexFitter::~TrgVertexFitter() {};
//=============================================================================
// Initialize
//=============================================================================
StatusCode TrgVertexFitter::initialize(){
  StatusCode sc = GaudiTool::initialize();
  if (!sc) return sc;
  m_stuffer = tool<IParticleStuffer>("ParticleStuffer");
  return sc;
};
//=============================================================================
// Fit the vertex from a vector of Particles
//=============================================================================
StatusCode TrgVertexFitter::fit( const LHCb::Particle::ConstVector& parts, 
                                 LHCb::Particle& P, LHCb::Vertex& V) const{
  fit(parts,V);
  return m_stuffer->fillParticle(parts,V,P);
  
}
//=============================================================================
// Fit the vertex from a vector of Particles
//=============================================================================
StatusCode TrgVertexFitter::fit( const LHCb::Particle::ConstVector& parts, 
                                 LHCb::Vertex& V) const{
  
  // Vector of particles to use in fit, can contain daughters of input particles
  LHCb::Particle::ConstVector partsToFit;

  // Vector of input photons
  Particle::ConstVector inputPhotons;
  int nLongLived = 0;
  //  bool noFit = false;
  
  // Main loop on input particles
  for ( Particle::ConstVector::const_iterator iPart=parts.begin(); iPart!=parts.end(); ++iPart ) {
    
    const Particle* parPointer = *iPart;
    if ( !parPointer) {
      fatal() << "Pointer to particle failed: " << parPointer->particleID() << endreq;
      return StatusCode::FAILURE;
    }
    const Particle& par = *(parPointer);
    const Vertex* endVertexPointer = par.endVertex();

    // Take actions 1) 2) 3) or 4) according to particle type

    // 2) For resonances, use daughter particles for fit if m_useDaughters is set to true
    if ( m_useDaughters && endVertexPointer && isResonance(par) ){
      const Vertex& endvert =  *(endVertexPointer);
      const SmartRefVector<Particle>& daughters= endvert.outgoingParticles();
      for ( SmartRefVector<Particle>::const_iterator iDaught=daughters.begin();
            iDaught!=daughters.end();++iDaught) {
        const Particle* daughtPointer = *iDaught;
        if ( !daughtPointer) {
          fatal() << "Pointer to daughter particle failed: " << daughtPointer->particleID() << endreq;
          return StatusCode::FAILURE;
        }
        verbose() << "Daughter particle added to list for fit: " << daughtPointer->particleID() << endreq;
        partsToFit.push_back(daughtPointer);
      }
      
    }
    // 3) FL: Long lived composite
    else if ( m_useDaughters && endVertexPointer && !(isResonance(par)) ){
      verbose() << "Long lived particle added to list for fit: " << par.particleID() << endreq;
      partsToFit.push_back(parPointer);
		  nLongLived++;
    }

    // 4) In any other case, particle will be used directly in the fit
    else {
      partsToFit.push_back(parPointer);
      verbose() << "Input particle added to list for fit: " << parPointer->particleID() << endreq;
      verbose() << "Point on particle: " << parPointer->referencePoint() << endreq;
      verbose() << "Error on point on particle: " << parPointer->covMatrix() << endreq;
    }
  }

  // Number of particles to be used for the fit
  // Can be different than # of input particles!
  int nPartsToFit = partsToFit.size();
  verbose() << "Number of particles that will be used for the fit: " << nPartsToFit << endreq;
  
  // Number of photons to be added at the end
  verbose() << "Number of photons that will be added to the vertex: " << inputPhotons.size() << endreq;
  
  // FL: check this first
  if ( nLongLived==1 && inputPhotons.size()>0 ){
  
    verbose() << "Special case: won't refit, but add " << inputPhotons.size() << " gamma(s) to existing vertex" << endreq;
	 
    // Get the composite vertex in case of no fit (no X -> n gammas)
      
    for ( std::vector<const Particle*>::const_iterator iPart=partsToFit.begin(); iPart!=partsToFit.end(); ++iPart ) {

      const Particle* composite = *iPart;
      const Vertex* Vtemp = composite->endVertex();
		
      //Should not only decay to gammas    
      unsigned int ngammas = 0;
      
      const SmartRefVector<Particle> daughters= Vtemp->outgoingParticles();
      for ( SmartRefVector<Particle>::const_iterator iDaught=daughters.begin();
            iDaught!=daughters.end();++iDaught) {
        if((*iDaught)->proto() && 
           m_photonID == (*iDaught)->particleID().pid()) ngammas++;
      } // iDaught
    
      // Do not use if only decays to gammas, e.g. J/Psi eta(2g)
      if(ngammas == Vtemp->outgoingParticles().size()) continue;
    
      // We have our vertex
      verbose() << " composite ID " << composite->particleID().pid() << " will be used to add gammas" << endmsg;
    
      verbose() << " composite decay vertex position: " << Vtemp->position() << " , and chi2 " << Vtemp->chi2() << endmsg;
				
      if(Vtemp) V = *Vtemp;
      else{
        fatal() << "No existing vertex found!" << endmsg;
        return StatusCode::FAILURE;
      }  
    } // iPart
 	 
  }  
  // Check wether enough particles
  else if ( (nPartsToFit == 0) || (nPartsToFit == 1)){
    fatal() << "Not enough particles to fit!" << endreq;
    return StatusCode::FAILURE;
  } else {  
  
    // Do the fit!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  
    StatusCode scFit = doFit( partsToFit, V );
    if ( !scFit) {
      fatal() << "doFit failed" << endreq;
      return StatusCode::FAILURE;

    }
  
  }
  
  // Add daugthers
  for(Particle::ConstVector::const_iterator iterP = parts.begin(); iterP != parts.end(); iterP++) {
    V.addToOutgoingParticles(*iterP);
    verbose() << "Particle added to vertex outgoingParticles: " << (*iterP)->particleID() << endreq;
  }

  debug() << "Returning vertex " << V.position() << " with error " 
          <<  V.covMatrix() << " Size: " << V.outgoingParticles().size() << endmsg ;  

  return StatusCode::SUCCESS;
}


//=============================================================================
// Fit a vertex from a vector of particles
//=============================================================================
StatusCode TrgVertexFitter::doFit(const LHCb::Particle::ConstVector& partsToFit, Vertex &V) const{

  int nPartsToFit = partsToFit.size();
  int ndof = (2*nPartsToFit) - 3;

  // Initialize summatories and arrays
  double iMX, iMY, iX0, iY0, iInvSig2X, iInvSig2Y;
  std::vector<double> X0Array,  Y0Array, MXArray ;
  std::vector<double> MYArray, InvSig2XArray, InvSig2YArray;
  double AX = 0, BX = 0, CX = 0, DX = 0, EX = 0 , AY = 0, BY = 0, CY = 0, DY = 0, EY = 0;

  // Compute relevant summatories
  int i=0;
  for ( std::vector<const Particle*>::const_iterator iPartIt=partsToFit.begin(); iPartIt!=partsToFit.end(); ++iPartIt ) {
    const Particle* parPointer = *iPartIt;
    const Particle& par = *(parPointer);
    const Gaudi::XYZPoint& point = par.referencePoint();
    const Gaudi::SymMatrix7x7& cov = par.covMatrix();
    const Gaudi::XYZVector slopes = par.slopes();
    iMX = slopes.X();
    iMY = slopes.Y();
    iX0 = point.x() - slopes.X() * point.z();
    iY0 = point.y() - slopes.Y() * point.z();
    iInvSig2X = 1/cov(1,1);
    iInvSig2Y = 1/cov(2,2);

    AX += iX0 * iInvSig2X;
    BX += iInvSig2X;
    CX += iMX*iInvSig2X;
    DX += iMX*iMX*iInvSig2X;
    EX += iX0*iMX*iInvSig2X;
    
    AY += iY0 * iInvSig2Y;
    BY += iInvSig2Y;
    CY += iMY*iInvSig2Y;
    DY += iMY*iMY*iInvSig2Y;
    EY += iY0*iMY*iInvSig2Y;
  
    X0Array.push_back(iX0);
    MXArray.push_back(iMX);
    InvSig2XArray.push_back(iInvSig2X);
    
    Y0Array.push_back(iY0);
    MYArray.push_back(iMY);
    InvSig2YArray.push_back(iInvSig2Y);
    i++;
  }
    
  // Compute vertex position and error from the summatories
  double vX,vY,vZ;
  StatusCode stPosAndErr = vertexPositionAndError(AX, BX, CX, DX, EX, AY, BY, CY, DY, EY, 
                                                  vX, vY, vZ, V);
  if (!stPosAndErr){
    fatal() << "vertexPositionAndError failed" << endreq;
    return StatusCode::FAILURE;
  };

  // Chi2
  double chi2 = 0;
  for (int i=0; i!=nPartsToFit; i++){
    chi2 += pow( (X0Array[i]+MXArray[i]*vZ-vX), 2)*InvSig2XArray[i];
    chi2 += pow( (Y0Array[i]+MYArray[i]*vZ-vY), 2)*InvSig2YArray[i];
  }
  V.setChi2(chi2);
  V.setNDoF(ndof);
  return StatusCode::SUCCESS;
}
//=============================================================================
// Compute vertex position and covariance matrix
//=============================================================================
StatusCode TrgVertexFitter::vertexPositionAndError(const double& AX, const double& BX, const double& CX,
                                                   const double& DX, const double& EX,
                                                   const double& AY, const double& BY, const double& CY,
                                                   const double& DY, const double& EY,
                                                   double& vX, double& vY, double& vZ, Vertex &V) const
{

  // Vertex position
  vZ = ( CX*AX/BX + CY*AY/BY - EX - EY ) / (DX +DY - CX*CX/BX - CY*CY/BY);
  vY = CY*vZ/BY + AY/BY;
  vX = CX*vZ/BX + AX/BX;
  const Gaudi::XYZPoint vPos(vX, vY, vZ);
  V.setPosition(vPos);
  
  Gaudi::SymMatrix3x3 fastCov;

  double invDet = 1./( BX*BY*( DX + DY ) - CX*CX*BY - CY*CY*BX );
  fastCov(1,1) = invDet * ( -CY*CY + ( BY * ( DX + DY )));
  fastCov(2,1) = invDet * (  CX*CY );
  fastCov(3,1) = invDet * (  CX*BY );
  fastCov(2,2) = invDet * ( -CX*CX + ( BX * ( DX + DY )));
  fastCov(2,3) = invDet * (  BX*CY );
  fastCov(3,3) = invDet * (  BX*BY );
  V.setCovMatrix(fastCov);

  return StatusCode::SUCCESS;
}
