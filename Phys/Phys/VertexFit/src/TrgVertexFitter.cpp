// $Id: TrgVertexFitter.cpp,v 1.1 2005-02-04 10:12:53 pkoppenb Exp $
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
// Just finds the pair of corresponding TrgTracks and does the vertex
//=============================================================================

StatusCode TrgVertexFitter::fitVertex(Particle& iPart, Particle& jPart,  Vertex& V)
{

  // Get first track
  const TrgTrack* iTrackPointer = (dynamic_cast<const TrgTrack*>(iPart.origin()));
  if (!iTrackPointer){
    fatal() << "Could not dynamic_cast TrgTrack from particle" << endreq;
    fatal() << "Try with fitVertex(const ParticleVector&, Vertex&)" << endreq;
    return StatusCode::FAILURE;
  }
  const TrgTrack& iTrack = *iTrackPointer;

  // Get second track
  const TrgTrack* jTrackPointer = (dynamic_cast<const TrgTrack*>(jPart.origin()));
  if (!jTrackPointer){
    fatal() << "Could not dynamic_cast TrgTrack from particle" << endreq;
    fatal() << "Try with fitVertex(const ParticleVector&, Vertex&)" << endreq;
    return StatusCode::FAILURE;
  }
  const TrgTrack& jTrack = *jTrackPointer;

  // Get vertex from tracks
  StatusCode status = fitVertexFromTracks(iTrack,jTrack,V);
  if (!status) {
    fatal() << "fitVertexFromTracks(iTrack,jTrack,V) failed" << endreq;
    fatal() << "Try with fitVertex(const ParticleVector&, Vertex&)" << endreq;
    return StatusCode::FAILURE;
  };

  // Add vertex daughters
  V.addToProducts(&iPart);
  V.addToProducts(&jPart);
  debug() << "Returning vertex " << V.position() << " with error " 
          <<  V.positionErr() << " size: " << V.products().size() << endmsg ;  

  return status;
}

//=============================================================================
// Method to fit the vertex between three given Particles
// Just finds the the three corresponding TrgTracks and does the vertex
//=============================================================================

  StatusCode TrgVertexFitter::fitVertex(Particle& iPart, Particle& jPart, Particle& kPart,  Vertex& V)
{

  // Get first track
  const TrgTrack* iTrackPointer = (dynamic_cast<const TrgTrack*>(iPart.origin()));
  if (!iTrackPointer){
    fatal() << "Could not dynamic_cast TrgTrack from particle" << endreq;
    fatal() << "Try with fitVertex(const ParticleVector&, Vertex&)" << endreq;
    return StatusCode::FAILURE;
  }
  const TrgTrack& iTrack = *iTrackPointer;

  // Get second track
  const TrgTrack* jTrackPointer = (dynamic_cast<const TrgTrack*>(jPart.origin()));
  if (!jTrackPointer){
    fatal() << "Could not dynamic_cast TrgTrack from particle" << endreq;
    fatal() << "Try with fitVertex(const ParticleVector&, Vertex&)" << endreq;
    return StatusCode::FAILURE;
  }
  const TrgTrack& jTrack = *jTrackPointer;

  // Get third track
  const TrgTrack* kTrackPointer = (dynamic_cast<const TrgTrack*>(kPart.origin()));
  if (!kTrackPointer){
    fatal() << "Could not dynamic_cast TrgTrack from particle" << endreq;
    fatal() << "Try with fitVertex(const ParticleVector&, Vertex&)" << endreq;
    return StatusCode::FAILURE;
  }
  const TrgTrack& kTrack = *kTrackPointer;

  // Fit vertex from tracks
  StatusCode status = fitVertexFromTracks(iTrack,jTrack,kTrack,V);
  if (!status) {
    fatal() << "fitVertexFromTracks(iTrack,jTrack,kTrack,V) failed" << endreq;
    return StatusCode::FAILURE;
  };

  // Add vertex daughters
  V.addToProducts(&iPart);
  V.addToProducts(&jPart);
  V.addToProducts(&kPart);
  debug() << "Returning vertex " << V.position() << " with error " 
          <<  V.positionErr() << " size: " << V.products().size() << endmsg ;  

  return status;
}


//=============================================================================
// Method to fit the vertex from a vector of Particles
// Just finds the vector of corresponding TrgTracks and does the vertex
//=============================================================================

StatusCode TrgVertexFitter::fitVertex( const  ParticleVector& parts,  Vertex& V)
{

  StatusCode status;
  
  // Get list of tracks
  std::vector<const TrgTrack*> tracks;
  for (ParticleVector::const_iterator iPart=parts.begin();iPart!=parts.end();++iPart) {

    const Particle& par = *(*iPart);
    const TrgTrack* tra = dynamic_cast<const TrgTrack*>(par.origin());
    if (!tra){
      if (!par.isResonance()){
        fatal() << "Fed with a composite which is not a resonance: " << par.particleID() << endreq;
        fatal() << "TrgVertexFitter cannot handle with that!" << endreq;
        return StatusCode::FAILURE;
      }
      else{
        const Vertex& iVertex =  *(par.endVertex());
        const SmartRefVector<Particle>& daughters= iVertex.products();
        for (SmartRefVector<Particle>::const_iterator iDaught=daughters.begin();iDaught!=daughters.end();++iDaught) {
          const Particle& daught = *(*iDaught);
          const TrgTrack* daughtTra = dynamic_cast<const TrgTrack*>(daught.origin());
          if (!daughtTra){
            fatal() << "A daughter of a composite is a composite." << endreq;
            fatal() << "TrgVertexFitter cannot handle with that!" << endreq;
            return StatusCode::FAILURE;
          }
          else{
            tracks.push_back(daughtTra);
          }
        }
      }
    }
    else{
      tracks.push_back(tra);
    }
  }

  // Fit vertex with tracks
  status = fitVertexFromTracks(tracks,V);
  if (!status) {
    fatal() << "fitVertexFromTracks(tracks,V) failed" << endreq;
    return StatusCode::FAILURE;
  };

  // Define daugthers
  for(ParticleVector::const_iterator iterP = parts.begin(); iterP != parts.end(); iterP++) {
    V.addToProducts(*iterP);
  }

  debug() << "Returning vertex " << V.position() << " with error " 
          <<  V.positionErr() << " size: " << V.products().size() << endmsg ;  

  return status;
}

/*

//=============================================================================
// Fit the vertex from a vector of Particles
// Just finds the vector of corresponding TrgTracks and does the vertex
//=============================================================================

StatusCode TrgVertexFitter::getTracksFromElementaries ( const  ParticleVector& parts,
                                                        ParticleVector& tracks,  Vertex& V)
{
  for (ParticleVector::const_iterator iPart=parts.begin();iPart!=parts.end();++iPart) {
    const Particle& par = *(*iPart);
    const TrgTrack* tra = dynamic_cast<const TrgTrack*>(par.origin());
    if (!tra){
      allElementary = false;
      break;
    }
    tracks.push_back(tra);

{
*/

//=============================================================================
// Method to fit a vertex between two given tracks
// Code duplicated in order to get optimal time performance!
//=============================================================================

StatusCode TrgVertexFitter::fitVertexFromTracks( const TrgTrack& iTrack, const TrgTrack& jTrack,  Vertex& V)
{

  const TrgState& iState = iTrack.firstState();
  const TrgState& jState = jTrack.firstState();
  
  double iInvSig2X = 1/iState.covariance()(1,1);
  double iInvSig2Y = 1/iState.covariance()(2,2);
  double jInvSig2X = 1/jState.covariance()(1,1);
  double jInvSig2Y = 1/jState.covariance()(2,2);
  
  double iMX = iState.xSlope();
  double iMY = iState.ySlope();
  double jMX = jState.xSlope();
  double jMY = jState.ySlope();

  // x0 and y0, at z=0
  double iX0 = iState.x() - iMX * iState.z();
  double iY0 = iState.y() - iMY * iState.z();
  double jX0 = jState.x() - jMX * jState.z();
  double jY0 = jState.y() - jMY * jState.z();
  
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
  StatusCode status = vertexPositionAndError( AX,  BX,  CX,  DX,  EX,
                                              AY,  BY,  CY,  DY,  EY,
                                              vX, vY, vZ, V);

  if (!status){fatal() << "vertexPositionAndError failed" << endreq;};


  // Chi2
  double chi2 = pow( (iX0+iMX*vZ-vX), 2)*iInvSig2X + pow( (jX0+jMX*vZ-vX), 2)*jInvSig2X +
    pow( (iY0+iMY*vZ-vY), 2)*iInvSig2Y + pow( (jY0+jMY*vZ-vY), 2)*jInvSig2Y;
  V.setChi2(chi2);
  V.setNDoF(1);

  return StatusCode::SUCCESS;

}

//=============================================================================
// Method to fit a vertex between three given TrgTracks
// Code duplicated in order to get optimal time performance!
//=============================================================================

StatusCode TrgVertexFitter::fitVertexFromTracks( const TrgTrack& iTrack, const TrgTrack& jTrack,
                                                 const TrgTrack& kTrack, Vertex& V)
{

  const TrgState& iState = iTrack.firstState();
  const TrgState& jState = jTrack.firstState();
  const TrgState& kState = kTrack.firstState();

  double iInvSig2X = 1/iState.covariance()(1,1);
  double iInvSig2Y = 1/iState.covariance()(2,2);
  double jInvSig2X = 1/jState.covariance()(1,1);
  double jInvSig2Y = 1/jState.covariance()(2,2);
  double kInvSig2X = 1/kState.covariance()(1,1);
  double kInvSig2Y = 1/kState.covariance()(2,2);
  
  double iMX = iState.xSlope();
  double iMY = iState.ySlope();
  double jMX = jState.xSlope();
  double jMY = jState.ySlope();
  double kMX = kState.xSlope();
  double kMY = kState.ySlope();

  // x0 and y0, at z=0
  double iX0 = iState.x() - iMX * iState.z();
  double iY0 = iState.y() - iMY * iState.z();
  double jX0 = jState.x() - jMX * jState.z();
  double jY0 = jState.y() - jMY * jState.z();
  double kX0 = kState.x() - kMX * kState.z();
  double kY0 = kState.y() - kMY * kState.z();
  
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
  StatusCode status = vertexPositionAndError( AX,  BX,  CX,  DX,  EX,
                                              AY,  BY,  CY,  DY,  EY,
                                              vX, vY, vZ, V);

  if (!status){fatal() << "vertexPositionAndError failed" << endreq;};
  
  // Chi2
  double chi2 = pow( (iX0+iMX*vZ-vX), 2)*iInvSig2X + pow( (jX0+jMX*vZ-vX), 2)*jInvSig2X  + pow( (kX0+kMX*vZ-vX), 2)*kInvSig2X +
    pow( (iY0+iMY*vZ-vY), 2)*iInvSig2Y + pow( (jY0+jMY*vZ-vY), 2)*jInvSig2Y + pow( (kY0+kMY*vZ-vY), 2)*kInvSig2Y;
  V.setChi2(chi2);
  V.setNDoF(3);

  return StatusCode::SUCCESS;
}


//=============================================================================
// Method to fit the vertex from a vector of TrgTracks
// Works on the assumption that tracks are straight lines
// And they have cylindrical errors (as in HLT when error comes from 1/pT param)
//=============================================================================

StatusCode TrgVertexFitter::fitVertexFromTracks( const std::vector<const TrgTrack*> tracks,  Vertex& V)
{

  int nParts=tracks.size();
  int ndof = (2*nParts) - 3;

  double AX = 0, BX = 0, CX = 0, DX = 0, EX = 0 , AY = 0, BY = 0, CY = 0, DY = 0, EY = 0;

  double MXArray [nParts], MYArray [nParts];
  double X0Array [nParts], Y0Array [nParts];
  double InvSig2XArray [nParts], InvSig2YArray [nParts];
  
  int i = 0;
  for (std::vector<const TrgTrack*>::const_iterator iTrack=tracks.begin();iTrack!=tracks.end();++iTrack) {

    const TrgTrack* traPointer = *iTrack;
    if ( !traPointer ) {
      fatal() << "Empty pointer passed to fitVertexFromTracks" << endreq;
      return StatusCode::FAILURE;
    }

    const TrgTrack& tra = *traPointer;
    const TrgState& iState = tra.firstState();
  
    double iInvSig2X = 1/iState.covariance()(1,1);
    double iInvSig2Y = 1/iState.covariance()(2,2);
    InvSig2XArray[i] = iInvSig2X;
    InvSig2YArray[i] = iInvSig2Y;

    double iMX = iState.xSlope();
    double iMY = iState.ySlope();
    MXArray[i] = iMX;
    MYArray[i] = iMY;
  
    // x0 and y0, at z=0
    double iX0 = iState.x() - iMX * iState.z();
    double iY0 = iState.y() - iMY * iState.z();
    X0Array[i] = iX0;
    Y0Array[i] = iY0;
  
    // Terms needed
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

    i++;
  }
    
  double vX,vY,vZ;
  StatusCode status = vertexPositionAndError( AX,  BX,  CX,  DX,  EX,
                                              AY,  BY,  CY,  DY,  EY,
                                              vX, vY, vZ, V);
  if (!status){fatal() << "vertexPositionAndError failed" << endreq;};

  // Chi2
  double chi2 = 0;
  for (int i=0; i!=nParts; i++){
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
