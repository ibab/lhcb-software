// $Id: TrgVertexFitter.cpp,v 1.2 2005-02-08 08:50:22 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 


#include "Event/Vertex.h"
#include "Event/Particle.h"

// local
#include "TrgVertexFitter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrgVertexFitter
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
{
  declareInterface<IVertexFitter>(this);

}
//=============================================================================
// Destructor
//=============================================================================
TrgVertexFitter::~TrgVertexFitter() {}; 



//=============================================================================
// Method to fit the vertex between two given Particles
//=============================================================================

StatusCode TrgVertexFitter::fitVertex(Particle& iPart, Particle& jPart,  Vertex& V)
{
  // Track point and error on point
  const Hep3Vector& iPoint = iPart.pointOnTrack();
  const Hep3Vector& jPoint = jPart.pointOnTrack();
  const HepSymMatrix& iCov = iPart.pointOnTrackErr();
  const HepSymMatrix& jCov = jPart.pointOnTrackErr();
  
  // Slopes
  const double& iMX = iPart.slopeX();
  const double& iMY = iPart.slopeY();
  const double& jMX = jPart.slopeX();
  const double& jMY = jPart.slopeY();

  // x0 and y0, at z=0
  double iX0 = iPoint.x() - iMX * iPoint.z();
  double iY0 = iPoint.y() - iMY * iPoint.z();
  double jX0 = jPoint.x() - jMX * jPoint.z();
  double jY0 = jPoint.y() - jMY * jPoint.z();

  // Inverse of x and y errors of track
  double iInvSig2X = 1/iCov(1,1);
  double iInvSig2Y = 1/iCov(2,2);
  double jInvSig2X = 1/jCov(1,1);
  double jInvSig2Y = 1/jCov(2,2);
  
  // Terms needed
  double AX = iX0*iInvSig2X       + jX0*jInvSig2X;
  double BX = iInvSig2X           + jInvSig2X;
  double CX = iMX*iInvSig2X       + jMX*jInvSig2X;
  double DX = iMX*iMX*iInvSig2X   + jMX*jMX*jInvSig2X;
  double EX = iX0*iMX*iInvSig2X   + jX0*jMX*jInvSig2X;
  
  double AY = iY0 * iInvSig2Y     + jY0*jInvSig2Y;
  double BY = iInvSig2Y           + jInvSig2Y;
  double CY = iMY*iInvSig2Y       + jMY*jInvSig2Y;
  double DY = iMY*iMY*iInvSig2Y   + jMY*jMY*jInvSig2Y;
  double EY = iY0*iMY*iInvSig2Y   + jY0*jMY*jInvSig2Y;

  double vX,vY,vZ;
  StatusCode status = vertexPositionAndError( AX, BX, CX, DX, EX, AY, BY, CY, DY, EY, vX, vY, vZ, V);

  if (!status){
    fatal() << "vertexPositionAndError failed" << endreq;
    return StatusCode::FAILURE;
  };


  // Chi2
  double chi2 = pow( (iX0+iMX*vZ-vX), 2)*iInvSig2X + pow( (jX0+jMX*vZ-vX), 2)*jInvSig2X +
    pow( (iY0+iMY*vZ-vY), 2)*iInvSig2Y + pow( (jY0+jMY*vZ-vY), 2)*jInvSig2Y;
  V.setChi2(chi2);
  V.setNDoF(1);


  // Add vertex daughters
  V.addToProducts(&iPart);
  V.addToProducts(&jPart);
  debug() << "Returning vertex " << V.position() << " with error " 
          <<  V.positionErr() << " size: " << V.products().size() << endmsg ;  

  return StatusCode::SUCCESS;
}


//=============================================================================
// Method to fit the vertex between three given Particles
// Just finds the the three corresponding TrgTracks and does the vertex
//=============================================================================

  StatusCode TrgVertexFitter::fitVertex(Particle& iPart, Particle& jPart, Particle& kPart,  Vertex& V)
{
  // Track point and error on point
  const Hep3Vector& iPoint = iPart.pointOnTrack();
  const Hep3Vector& jPoint = jPart.pointOnTrack();
  const Hep3Vector& kPoint = kPart.pointOnTrack();
  const HepSymMatrix& iCov = iPart.pointOnTrackErr();
  const HepSymMatrix& jCov = jPart.pointOnTrackErr();
  const HepSymMatrix& kCov = kPart.pointOnTrackErr();
  
  // Slopes
  const double& iMX = iPart.slopeX();
  const double& iMY = iPart.slopeY();
  const double& jMX = jPart.slopeX();
  const double& jMY = jPart.slopeY();
  const double& kMX = kPart.slopeX();
  const double& kMY = kPart.slopeY();

  // x0 and y0, at z=0
  double iX0 = iPoint.x() - iMX * iPoint.z();
  double iY0 = iPoint.y() - iMY * iPoint.z();
  double jX0 = jPoint.x() - jMX * jPoint.z();
  double jY0 = jPoint.y() - jMY * jPoint.z();
  double kX0 = kPoint.x() - kMX * kPoint.z();
  double kY0 = kPoint.y() - kMY * kPoint.z();

  // Inverse of x and y errors of track
  double iInvSig2X = 1/iCov(1,1);
  double iInvSig2Y = 1/iCov(2,2);
  double jInvSig2X = 1/jCov(1,1);
  double jInvSig2Y = 1/jCov(2,2);
  double kInvSig2X = 1/kCov(1,1);
  double kInvSig2Y = 1/kCov(2,2);
  
  // Terms needed
  double AX = iX0*iInvSig2X       + jX0*jInvSig2X       + kX0*kInvSig2X;
  double BX = iInvSig2X           + jInvSig2X           + kInvSig2X;
  double CX = iMX*iInvSig2X       + jMX*jInvSig2X       + kMX*kInvSig2X;
  double DX = iMX*iMX*iInvSig2X   + jMX*jMX*jInvSig2X   + kMX*kMX*kInvSig2X;
  double EX = iX0*iMX*iInvSig2X   + jX0*jMX*jInvSig2X   + kX0*kMX*kInvSig2X;
  
  double AY = iY0 * iInvSig2Y     + jY0*jInvSig2Y       + kY0*kInvSig2Y;
  double BY = iInvSig2Y           + jInvSig2Y           + kInvSig2Y;
  double CY = iMY*iInvSig2Y       + jMY*jInvSig2Y       + kMY*kInvSig2Y;
  double DY = iMY*iMY*iInvSig2Y   + jMY*jMY*jInvSig2Y   + kMY*kMY*kInvSig2Y;
  double EY = iY0*iMY*iInvSig2Y   + jY0*jMY*jInvSig2Y   + kY0*kMY*kInvSig2Y;

  double vX,vY,vZ;
  StatusCode status = vertexPositionAndError( AX, BX, CX, DX, EX, AY, BY, CY, DY, EY, vX, vY, vZ, V);
  if (!status){
    fatal() << "vertexPositionAndError failed" << endreq;
    return StatusCode::FAILURE;
  };
  
  // Chi2
  double chi2 = pow( (iX0+iMX*vZ-vX), 2)*iInvSig2X + pow( (jX0+jMX*vZ-vX), 2)*jInvSig2X  + pow( (kX0+kMX*vZ-vX), 2)*kInvSig2X +
    pow( (iY0+iMY*vZ-vY), 2)*iInvSig2Y + pow( (jY0+jMY*vZ-vY), 2)*jInvSig2Y + pow( (kY0+kMY*vZ-vY), 2)*kInvSig2Y;
  V.setChi2(chi2);
  V.setNDoF(3);

  // Add vertex daughters
  V.addToProducts(&iPart);
  V.addToProducts(&jPart);
  V.addToProducts(&kPart);

  debug() << "Returning vertex " << V.position() << " with error " 
          <<  V.positionErr() << " size: " << V.products().size() << endmsg ;  

  return StatusCode::SUCCESS;
}

/*
//=============================================================================
// Fit the vertex from a vector of Particles
//=============================================================================

StatusCode TrgVertexFitter::fitVertex( const  ParticleVector& parts,  Vertex& V)
{

  StatusCode status;

  // Number of particles used for the vertexing, can be different from size of particlevector!!
  int nParts = 0;

  // Vects of track data
  std::vector<double> MXVect, MYVect, X0Vect, Y0Vect, InvSig2XVect, InvSig2YVect;


  // Main loop on particles

  for ( ParticleVector::const_iterator iPart=parts.begin(); iPart!=parts.end(); ++iPart ) {
    const Particle* parPointer = *iPart;
    if ( !parPointer) {
      fatal() << "Pointer to mother particle failed!" << endreq;
      return StatusCode::FAILURE;
    }
    const Particle& par = *(parPointer);
    const Vertex* endVertexPointer = par.endVertex();
    
    if ( ( endVertexPointer ) && par.isResonance() ){
      const Vertex& endvert =  *(endVertexPointer);
      const SmartRefVector<Particle>& daughters= endvert.products();
      for ( SmartRefVector<Particle>::const_iterator iDaught=daughters.begin();iDaught!=daughters.end();++iDaught) {
        const Particle* daughtPointer = *iDaught;
        if ( !daughtPointer) {
          fatal() << "Pointer to daughter particle failed!" << endreq;
          return StatusCode::FAILURE;
        }
        const Particle& daught = *(daughtPointer);
        const Hep3Vector& pointDaught = daught.pointOnTrack();
        const HepSymMatrix& cov = daught.pointOnTrackErr();
        MXVect.push_back(daught.slopeX());
        MYVect.push_back(daught.slopeY());
        X0Vect.push_back(pointDaught.x() - daught.slopeX() * pointDaught.z());
        Y0Vect.push_back(pointDaught.y() - daught.slopeY() * pointDaught.z());
        InvSig2XVect.push_back(1/cov(1,1));
        InvSig2YVect.push_back(1/cov(2,2));
        nParts+=1;
        debug() << "Added one doughter" << endreq;
        
      }
      
    }
    else{    // Elementary particles
      const Hep3Vector& point = par.pointOnTrack();
      const HepSymMatrix& cov = par.pointOnTrackErr();
      MXVect.push_back(par.slopeX());
      MYVect.push_back(par.slopeY());
      X0Vect.push_back(point.x() - par.slopeX() * point.z());
      Y0Vect.push_back(point.y() - par.slopeY() * point.z());
      InvSig2XVect.push_back(1/cov(1,1));
      InvSig2YVect.push_back(1/cov(2,2));
      nParts+=1;
      debug() << "Added one mother" << endreq;
    }
  }

  debug() << "Particles used for the fit:" << nParts << endreq;

  int ndof = (2*nParts) - 3;

  // Initialize relevant terms
  double AX = 0, BX = 0, CX = 0, DX = 0, EX = 0 , AY = 0, BY = 0, CY = 0, DY = 0, EY = 0;

  for (int i = 0; i != nParts; i++ ){
    // Terms needed
    AX += X0Vect[i] * InvSig2XVect[i];
    BX += InvSig2XVect[i];
    CX += MXVect[i]*InvSig2XVect[i];
    DX += MXVect[i]*MXVect[i]*InvSig2XVect[i];
    EX += X0Vect[i]*MXVect[i]*InvSig2XVect[i];
    
    AY += Y0Vect[i] * InvSig2YVect[i];
    BY += InvSig2YVect[i];
    CY += MYVect[i]*InvSig2YVect[i];
    DY += MYVect[i]*MYVect[i]*InvSig2YVect[i];
    EY += Y0Vect[i]*MYVect[i]*InvSig2YVect[i];
  }
    
  double vX,vY,vZ;

  status = vertexPositionAndError( AX, BX, CX, DX, EX, AY, BY, CY, DY, EY, vX, vY, vZ, V);
  if (!status){
    fatal() << "vertexPositionAndError failed" << endreq;
    return StatusCode::FAILURE;
    };

  // Chi2
  double chi2 = 0;
  for (int i=0; i!=nParts; i++){
    chi2 += pow( (X0Vect[i]+MXVect[i]*vZ-vX), 2)*InvSig2XVect[i];
    chi2 += pow( (Y0Vect[i]+MYVect[i]*vZ-vY), 2)*InvSig2YVect[i];
  }

  V.setChi2(chi2);
  V.setNDoF(ndof);

  // Define daugthers
  for(ParticleVector::const_iterator iterP = parts.begin(); iterP != parts.end(); iterP++) {
    V.addToProducts(*iterP);
  }

  debug() << "Returning vertex " << V.position() << " with error " 
          <<  V.positionErr() << " size: " << V.products().size() << endmsg ;  

  return StatusCode::SUCCESS;
}

*/

//=============================================================================
// Fit the vertex from a vector of Particles
//=============================================================================

StatusCode TrgVertexFitter::fitVertex( const  ParticleVector& parts,  Vertex& V)
{
  
  StatusCode status;
  
  // Vector of particles to use in fit
  std::vector<const Particle*> partsToFit;
  
  // Main loop on particles
  for ( ParticleVector::const_iterator iPart=parts.begin(); iPart!=parts.end(); ++iPart ) {
    
    const Particle* parPointer = *iPart;
    if ( !parPointer) {
      fatal() << "Pointer to mother particle failed!" << endreq;
      return StatusCode::FAILURE;
    }
    const Particle& par = *(parPointer);
    const Vertex* endVertexPointer = par.endVertex();
    
    if ( ( endVertexPointer ) && par.isResonance() ){
      const Vertex& endvert =  *(endVertexPointer);
      const SmartRefVector<Particle>& daughters= endvert.products();
      for ( SmartRefVector<Particle>::const_iterator iDaught=daughters.begin();iDaught!=daughters.end();++iDaught) {
        const Particle* daughtPointer = *iDaught;
        if ( !daughtPointer) {
          fatal() << "Pointer to daughter particle failed!" << endreq;
          return StatusCode::FAILURE;
        }
        debug() << "Added one doughter" << endreq;
        partsToFit.push_back(daughtPointer);
      }
      
    }
    else{    // Non-composite particles
      partsToFit.push_back(parPointer);
      debug() << "Added one mother" << endreq;
    }
  }

  // Number of particles used for the vertexing, can be different from size of particlevector!!
  int nParts = partsToFit.size();
  debug() << "Particles used for the fit:" << nParts << endreq;
  int ndof = (2*nParts) - 3;

  // Initialize relevant terms
  double iMX, iMY, iX0, iY0, iInvSig2X, iInvSig2Y;
  double X0Array [nParts],  Y0Array [nParts], MXArray [nParts], MYArray [nParts], InvSig2XArray [nParts], InvSig2YArray [nParts];
  double AX = 0, BX = 0, CX = 0, DX = 0, EX = 0 , AY = 0, BY = 0, CY = 0, DY = 0, EY = 0;
  int i=0;
  for ( ParticleVector::const_iterator iPart=parts.begin(); iPart!=parts.end(); ++iPart ) {
    const Particle* parPointer = *iPart;
    const Particle& par = *(parPointer);
    const Hep3Vector& point = par.pointOnTrack();
    const HepSymMatrix& cov = par.pointOnTrackErr();
    iMX = par.slopeX();
    iMY = par.slopeY();
    iX0 = point.x() - par.slopeX() * point.z();
    iY0 = point.y() - par.slopeY() * point.z();
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
  
    X0Array[i] = iX0;
    MXArray[i] = iMX;
    InvSig2XArray[i] = iInvSig2X;
    
    Y0Array[i] = iY0;
    MYArray[i] = iMY;
    InvSig2YArray[i] = iInvSig2Y;
    i++;
  }
    
  double vX,vY,vZ;
  status = vertexPositionAndError( AX, BX, CX, DX, EX, AY, BY, CY, DY, EY, vX, vY, vZ, V);
  if (!status){
    fatal() << "vertexPositionAndError failed" << endreq;
    return StatusCode::FAILURE;
    };

  // Chi2
  double chi2 = 0;
  for (int i=0; i!=nParts; i++){
    chi2 += pow( (X0Array[i]+MXArray[i]*vZ-vX), 2)*InvSig2XArray[i];
    chi2 += pow( (Y0Array[i]+MYArray[i]*vZ-vY), 2)*InvSig2YArray[i];
  }

  V.setChi2(chi2);
  V.setNDoF(ndof);

  // Define daugthers
  for(ParticleVector::const_iterator iterP = parts.begin(); iterP != parts.end(); iterP++) {
    V.addToProducts(*iterP);
  }

  debug() << "Returning vertex " << V.position() << " with error " 
          <<  V.positionErr() << " size: " << V.products().size() << endmsg ;  

  return StatusCode::SUCCESS;
}


//=============================================================================
// Compute vertex position and covariance matrix
//=============================================================================

StatusCode TrgVertexFitter::vertexPositionAndError(const double& AX, const double& BX, const double& CX,
                                                   const double& DX, const double& EX,
                                                   const double& AY, const double& BY, const double& CY,
                                                   const double& DY, const double& EY,
                                                   double& vX, double& vY, double& vZ, Vertex &V)
{

  // Vertex position
  vZ = ( CX*AX/BX + CY*AY/BY - EX - EY ) / (DX +DY - CX*CX/BX - CY*CY/BY);
  vY = CY*vZ/BY + AY/BY;
  vX = CX*vZ/BX + AX/BX;
  HepPoint3D vPos = HepPoint3D (vX, vY, vZ);
  V.setPosition(vPos);
  
   // Covariance matrix
  HepSymMatrix fastCov(3);
  double invDet = 1./( BX*BY*( DX + DY ) - CX*CX*BY - CY*CY*BX );
  fastCov(1,1) = invDet * ( -CY*CY + ( BY * ( DX + DY )));
  fastCov(2,1) = invDet * (  CX*CY );
  fastCov(3,1) = invDet * (  CX*BY );
  fastCov(2,2) = invDet * ( -CX*CX + ( BX * ( DX + DY )));
  fastCov(2,3) = invDet * (  BX*CY );
  fastCov(3,3) = invDet * (  BX*BY );
  V.setPositionErr(fastCov);

  return StatusCode::SUCCESS;
}
