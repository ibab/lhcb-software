// $Id: TrgVertexFitter.cpp,v 1.30 2009-09-11 17:15:09 jonrob Exp $
// Include files
#include <cmath>
#include "gsl/gsl_sys.h"

// from Gaudi
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
//  COV(0,0)=COV(1,1)!=0 AND ZERO IN ANY OTHER ELEMENT
//
//  The tool should work for other tracks, but correlations
//  will be neglected
//
//  The fact that COV(0,0)=COV(1,1) is not assumed anywhere, allowing the
//  two elements to be different, useful for instance for composites
//
//  Some documentation available in a talk at
//  http://agenda.cern.ch/fullAgenda.php?ida=a05940#2005-02-07
//
// 2005-01-31 : Hugo Ruiz Perez
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY(TrgVertexFitter)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrgVertexFitter::TrgVertexFitter( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
: GaudiTool ( type, name , parent )
  ,m_photonID(22)
  ,m_pi0ID(111)
  ,m_stuffer()
{
  declareInterface<IVertexFit>(this);
  declareInterface<IParticleCombiner>(this);
  declareInterface<IParticleReFitter>(this);
  declareProperty( "useDaughters", m_useDaughters = true );
  declareProperty( "Epsilon", m_epsilon = 1.e-16 );
}
//=============================================================================
// Destructor
//=============================================================================
TrgVertexFitter::~TrgVertexFitter() {}
//=============================================================================
// Initialize
//=============================================================================
StatusCode TrgVertexFitter::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if (!sc) return sc;
  m_stuffer = tool<IParticleStuffer>("ParticleStuffer");
  return sc;
}
//=============================================================================
// Fit the vertex from a vector of Particles
//=============================================================================
StatusCode TrgVertexFitter::fit
( const LHCb::Particle::ConstVector& parts,
  LHCb::Vertex&   V ,
  LHCb::Particle& P ) const
{
  StatusCode sc = fit(V,parts);
  if (!sc) return sc ;
  sc = m_stuffer->fillParticle(parts,V,P);
  if ( msgLevel(MSG::DEBUG) ) {
    if (!sc) debug() << "Stuffing failed" << endmsg ;
    else debug() << "ParticleStuffer returns particle "
                 << P.particleID().pid() << " " << P.momentum()
                 << " " << P.measuredMass() << endmsg ;
  }
  return sc ;
}
//=============================================================================
// Fit the vertex from a vector of Particles
//=============================================================================
StatusCode TrgVertexFitter::fit( LHCb::Vertex& V ,
                                 const LHCb::Particle::ConstVector& parts ) const
{
  if (msgLevel(MSG::DEBUG)) 
    debug() << "Hello from TrgVertexFitter vertexing " << parts.size() << " particles" 
            << endmsg ;

  // Vector of particles to use in fit, can contain daughters of input particles
  LHCb::Particle::ConstVector partsToFit;
  partsToFit.reserve( parts.size() );
  //Vector of input neutrals from the mother
  Particle::ConstVector inputNeutralsFromMother;
  //Vector of input neutrals from a resonance
  Particle::ConstVector inputNeutralsFromResonance;

  bool fitNeeded = true ;

  StatusCode sc = classify(parts,partsToFit, 
                           inputNeutralsFromMother,
                           inputNeutralsFromResonance, 
                           V, fitNeeded);
  if (!sc) return sc ;

  if (fitNeeded )
  {
    // Do the fit!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    const StatusCode scFit = doFit( partsToFit, V );
    if ( !scFit) 
    {
      return Warning("doFit failed",StatusCode::FAILURE,0);
    }
  }

  // Add daugthers
  for(Particle::ConstVector::const_iterator iterP = partsToFit.begin(); 
      iterP != partsToFit.end(); ++iterP ) 
  {
    V.addToOutgoingParticles(*iterP);
    if (msgLevel(MSG::VERBOSE)) verbose() << "Particle added to vertex outgoingParticles: "
                                          << (*iterP)->particleID().pid() << endmsg;
  }


  //Add the neutrals daughters from  the resonances  once the fit is done
  for( Particle::ConstVector::const_iterator iterNeutrals = inputNeutralsFromResonance.begin();
       iterNeutrals != inputNeutralsFromResonance.end(); ++iterNeutrals )
  {
    V.addToOutgoingParticles(*iterNeutrals);
    if (msgLevel(MSG::VERBOSE)) 
      verbose() << "Neutral Particle  from the resonances  are added to vertex outgoingParticles: "
                << (*iterNeutrals)->particleID().pid() << endmsg;
  }

  //Add the neutrals daughters from  the mother once the fit is done
  for(Particle::ConstVector::const_iterator iterNeutrals =inputNeutralsFromMother.begin();
      iterNeutrals != inputNeutralsFromMother.end(); iterNeutrals++ ){
    V.addToOutgoingParticles(*iterNeutrals);
    if (msgLevel(MSG::VERBOSE)) 
      verbose() << "Neutral Particle  from the mother are added to vertex outgoingParticles: "
                << (*iterNeutrals)->particleID().pid() << endmsg;
  }

  if (msgLevel(MSG::DEBUG)) 
    debug() << "Returning vertex " << V.position() << " with error\n"
            <<  V.covMatrix() << " Size: " << V.outgoingParticles().size() 
            << endmsg ;

  return StatusCode::SUCCESS;
}


//=============================================================================
// Fit a vertex from a vector of particles
//=============================================================================
StatusCode TrgVertexFitter::doFit( const LHCb::Particle::ConstVector& partsToFit,
                                   Vertex &V) const
{

  const unsigned int nPartsToFit = partsToFit.size();
  const unsigned int ndof = (2*nPartsToFit) - 3;

  // Initialize summatories and arrays
  std::vector<double> X0Array(nPartsToFit), Y0Array(nPartsToFit), MXArray(nPartsToFit);
  std::vector<double> MYArray(nPartsToFit), InvSig2XArray(nPartsToFit), InvSig2YArray(nPartsToFit);
  double AX = 0, BX = 0, CX = 0, DX = 0, EX = 0 , AY = 0, BY = 0, CY = 0, DY = 0, EY = 0;

  // Compute relevant summatories
  unsigned int index = 0;
  for ( std::vector<const Particle*>::const_iterator iPartIt = partsToFit.begin();
        iPartIt != partsToFit.end(); ++iPartIt, ++index )
  {
    const Particle* par = *iPartIt;
    const Gaudi::XYZPoint& point    = par->referencePoint();
    const Gaudi::SymMatrix3x3& pcov = par->posCovMatrix();

    //if (msgLevel(MSG::VERBOSE)) verbose() << par->particleID().pid() << " pcov\n " << pcov << endmsg ;
    //else if (msgLevel(MSG::DEBUG)) debug() << par->particleID().pid()<< " pcov " << pcov(0,0) << " " << pcov(1,1) << endmsg ;

    // Use direct references to the vectors instead of temporaries
    double & iMX = MXArray[index];
    double & iMY = MYArray[index];
    double & iX0 = X0Array[index];
    double & iY0 = Y0Array[index];
    double & iInvSig2X = InvSig2XArray[index];
    double & iInvSig2Y = InvSig2YArray[index];

    const Gaudi::XYZVector & slopes = par->slopes();
    iMX = slopes.X();
    iMY = slopes.Y();
    //if (msgLevel(MSG::VERBOSE)) verbose() << par->particleID().pid() << " Slopes  " << iMX << " " << iMY << endmsg ;
    iX0 = point.x() - slopes.X() * point.z();
    iY0 = point.y() - slopes.Y() * point.z();
    iInvSig2X = 1.0/pcov(0,0);
    iInvSig2Y = 1.0/pcov(1,1);

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

  }

  // Compute vertex position and error from the summaries
  double vX,vY,vZ;
  const StatusCode stPosAndErr = vertexPositionAndError(AX, BX, CX, DX, EX, AY, BY, CY, DY, EY,
                                                        vX, vY, vZ, V);
  if (!stPosAndErr)
  {
    return Warning("vertexPositionAndError failed", StatusCode::FAILURE, 0);
  }
  if (msgLevel(MSG::VERBOSE))
  {
    verbose() << "Got from VPAE X A " << AX << " B " << BX << " C " << CX << " D " << DX << " E " << EX << endmsg ;
    verbose() << "              Y A " << AY << " B " << BY << " C " << CY << " D " << DY << " E " << EY << endmsg ;
    verbose() << "              v " << vX << " " << vY << " " << vZ << "\n" << V << endmsg ;
  }

  // Chi2
  double chi2 = 0;
  for ( unsigned int i=0; i != nPartsToFit; ++i )
  {
    if (msgLevel(MSG::VERBOSE))
    {
      verbose() << "Adding to chi2 x: " << X0Array[i] << "+"
                << MXArray[i] << "*" << vZ << "-" << vX
                << " (=" << (X0Array[i]+MXArray[i]*vZ-vX) << ")^2 *"
                << InvSig2XArray[i] << " = "
                << std::pow( (X0Array[i]+MXArray[i]*vZ-vX), 2)*InvSig2XArray[i]
                << endmsg;
      verbose() << "Adding to chi2 y: " << Y0Array[i] << "+"
                << MYArray[i] << "*" << vZ << "-" << vY
                << " (=" << (Y0Array[i]+MYArray[i]*vZ-vY) << ")^2 *"
                << InvSig2YArray[i] << " = "
                << std::pow( (Y0Array[i]+MYArray[i]*vZ-vY), 2)*InvSig2YArray[i]
                << endmsg;
    }
    chi2 += std::pow( (X0Array[i]+MXArray[i]*vZ-vX), 2) * InvSig2XArray[i];
    chi2 += std::pow( (Y0Array[i]+MYArray[i]*vZ-vY), 2) * InvSig2YArray[i];
  }
  if (msgLevel(MSG::DEBUG)) debug() << "Chi2 is " << chi2 << endmsg ;

  V.setChi2(chi2);
  V.setNDoF(ndof);

  return StatusCode::SUCCESS;
}
//=============================================================================
// Compute vertex position and covariance matrix
//=============================================================================
StatusCode TrgVertexFitter::vertexPositionAndError(const double& AX,
                                                   const double& BX,
                                                   const double& CX,
                                                   const double& DX,
                                                   const double& EX,
                                                   const double& AY,
                                                   const double& BY,
                                                   const double& CY,
                                                   const double& DY,
                                                   const double& EY,
                                                   double& vX,
                                                   double& vY,
                                                   double& vZ,
                                                   Vertex& V) const
{

  const double R1 = CX*AX/BX + CY*AY/BY - EX - EY ;
  const double R2 = DX + DY - CX*CX/BX - CY*CY/BY ;

  if (msgLevel(MSG::VERBOSE))
  {
    verbose() << "X : " << std::setprecision(10)
              << AX << " "<< BX << " "<< CX << " " << DX << " " << EX << endmsg ;
    verbose() << "Y : "  << AY << " "<< BY << " "<< CY << " "<< DY << " "<< EY << " " << V << endmsg ;
    verbose() << "CX*AX/BX + CY*AY/BY - EX - EY = " << CX << " * " << AX << " / " << BX << " + "
              << CY << " * "  << AY << " / " << BY << "  - " << EX
              << " - "  << EY << endmsg;
    verbose() << " = "  << CX*AX/BX << " + " << CY*AY/BY << " - "
              << EX - EY << endmsg;
    verbose() << " = " << R1 << endmsg ;
    verbose() << "DX + DY - CX*CX/BX - CY*CY/BY = "
              << DX << " + " << DY << " - "  << CX << " * "
              << CX << " / "  << BX << " - " << CY << " * "
              << CY  << " / "  << BY   << endmsg;
    verbose() << " = "  << DX + DY << " - "
              << CX*CX/BX << " - " << CY*CY/BY << endmsg;
    verbose() << " = " << R2 << endmsg ;
  }

  if ( R2 < m_epsilon )
  {
    Warning("R2 is too small, leaving.", StatusCode::FAILURE, 0).ignore();
    debug() << " R1 " << R1 << " R2 " << R2 << endmsg ;
    return StatusCode::FAILURE;
  }

  // Vertex position
  vZ = R1 / R2 ;
  if (msgLevel(MSG::VERBOSE)) verbose() << " => vZ = " << vZ << endmsg ;

  vY = CY*vZ/BY + AY/BY;
  vX = CX*vZ/BX + AX/BX;
  const Gaudi::XYZPoint vPos(vX, vY, vZ);
  if (msgLevel(MSG::VERBOSE)) verbose() << " => v = " << vPos << endmsg ;

  V.setPosition(vPos);

  Gaudi::SymMatrix3x3 fastCov;

  const double det = ( BX*BY*( DX + DY ) - CX*CX*BY - CY*CY*BX );
  if ( 0 == gsl_fcmp ( det , 0 , 1.e-8 ) )
  {
    Warning("Position covariance matrix determinant is zero",StatusCode::FAILURE,0).ignore();
    return StatusCode::FAILURE;
  }
  const double invDet = 1./det ;
  if ( !std::isfinite(invDet) || std::isnan(invDet) )
  { // that's probably not needed anymore
    return Warning("Position covariance matrix determinant cannot be inverted",StatusCode::FAILURE,0);
  }

  fastCov(0,0) = invDet * ( -CY*CY + ( BY * ( DX + DY )));
  fastCov(1,0) = invDet * (  CX*CY );
  fastCov(2,0) = invDet * (  CX*BY );
  fastCov(1,1) = invDet * ( -CX*CX + ( BX * ( DX + DY )));
  fastCov(1,2) = invDet * (  BX*CY );
  fastCov(2,2) = invDet * (  BX*BY );

  if (msgLevel(MSG::VERBOSE)) verbose() << "DET: " << invDet << " fastCov \n" << fastCov << endmsg ;

  V.setCovMatrix(fastCov);

  return StatusCode::SUCCESS;
}
//=============================================================================
StatusCode TrgVertexFitter::reFit( LHCb::Particle& particle ) const
{
  LHCb::Vertex * vertex = particle.endVertex() ;
  LHCb::Particle::ConstVector t;
  t.reserve( vertex->outgoingParticles().size() );
  for ( SmartRefVector<LHCb::Particle>::const_iterator i = vertex->outgoingParticles().begin();
        i != vertex->outgoingParticles().end(); ++i ) { t.push_back(*i); }
  return fit( t , *vertex , particle ) ;
}
//=============================================================================
StatusCode TrgVertexFitter::combine( const LHCb::Particle::ConstVector& daughters ,
                                     LHCb::Particle&        mother   ,
                                     LHCb::Vertex&          vertex   ) const
{
  return fit( daughters , vertex , mother ) ;
}
//=============================================================================
/// add not active for fast vertex fitter
StatusCode TrgVertexFitter::add(const LHCb::Particle* p,
                                LHCb::Vertex& v) const
{
  if (msgLevel(MSG::VERBOSE)) verbose() << "Print " << v.position() << " and " << p
                                        << " to inhibit compilation warnings" << endmsg ;
  Error("Adding is not allowed by TrgVertexFitter");
  return StatusCode::FAILURE;
}
//=============================================================================
/// remove not active for fast vertex fitter
StatusCode TrgVertexFitter::remove(const LHCb::Particle* p,
                                   LHCb::Vertex& v) const
{
  if (msgLevel(MSG::VERBOSE)) verbose() << "Print " << v.position() << " and " << p
                                        << " to inhibit compilation warnings" << endmsg ;
  Error("Removing is not allowed by TrgVertexFitter");
  return StatusCode::FAILURE;
}
//=============================================================================
//=============================================================================
// Classify particles
//=============================================================================
StatusCode TrgVertexFitter::classify( const LHCb::Particle::ConstVector& parts,
                                      LHCb::Particle::ConstVector& partsToFit,
                                      LHCb::Particle::ConstVector& inputNeutralsFromMother,
                                      LHCb::Particle::ConstVector& inputNeutralsFromResonance,
                                      LHCb::Vertex& V,
                                      bool& fitNeeded
                                      ) const
{

  //Counter of the input neutrals from the mother
  unsigned int nNeutralsFromMother =0;
  //Counter of input neutrals from the resonance
  unsigned int nNeutralsFromResonance =0;
  //count the number of long lived particles
  unsigned int nLongLived = 0;

  // Main loop on input particles
  for ( Particle::ConstVector::const_iterator iPart=parts.begin(); iPart!=parts.end(); ++iPart )
  {
    const Particle* par = *iPart;
    if ( !par) {
      fatal() << "Pointer to particle failed: "  << endmsg;
      return StatusCode::FAILURE;
    }
    if (msgLevel(MSG::DEBUG))
    {
      debug() << " VERTEXING Particle " << par->key() << " " << par->particleID().pid()
              << " " << par->momentum() << endmsg ;
    }
    // Take actions 1) 2) 3) or 4) according to particle type
    // 2) For resonances, use daughter particles for fit if m_useDaughters is set to true
    // if the daughters of the resonance are neutral ignore them in the fit
    // they will be added after the fit

    if ( classifyNeutrals(par,inputNeutralsFromMother)) ++nNeutralsFromMother;
    else {
      const Vertex* endVertex = par->endVertex();

      if ( m_useDaughters && endVertex && isResonance(*par) ){
        const SmartRefVector<Particle>& daughters= endVertex->outgoingParticles();
        for ( SmartRefVector<Particle>::const_iterator iDaught=daughters.begin();
              iDaught!=daughters.end();++iDaught) {
          const Particle* daught = *iDaught;
          if ( !daught) 
          {
            fatal() << "Pointer to daughter particle failed: " << daught->particleID().pid() << endmsg;
            return StatusCode::FAILURE;
          }
          if (classifyNeutrals(daught,inputNeutralsFromResonance))  nNeutralsFromResonance++;
          else {
            if (msgLevel(MSG::VERBOSE)) verbose() << "Daughter particle added to list for fit: "
                                                  << daught->particleID().pid() << endmsg;
            partsToFit.push_back(daught);
          }
        }

      }
      // 3) FL: Long lived composite
      else if ( m_useDaughters && endVertex && !(isResonance(*par)) )
      {
        if (msgLevel(MSG::VERBOSE)) verbose() << "Long lived particle added to list for fit: "
                                              << par->particleID().pid() << endmsg;
        partsToFit.push_back(par);
        ++nLongLived;
      }


      // 4) In any other case, particle will be used directly in the fit
      else {
        partsToFit.push_back(par);
        if (msgLevel(MSG::DEBUG)) debug() << "Used directly : "
                                          << par->particleID().pid() << endmsg;
        if (msgLevel(MSG::VERBOSE)) verbose() << "Point on particle: "
                                              << par->referencePoint()
                                              << ", Error:\n " << par->covMatrix() << endmsg;
      }
    }
  }

  // Number of particles to be used for the fit
  // Can be different than # of input particles!
  const unsigned int nPartsToFit = partsToFit.size();
  if (msgLevel(MSG::VERBOSE)) 
    verbose() << "Number of particles that will be used for the fit: "
              << nPartsToFit << endmsg;

  //Number of neutrals from the mother that will be added at the end
  if (msgLevel(MSG::VERBOSE))
    verbose() << "Number of neutrals from the mother  that will be added to the vertex: "
              << inputNeutralsFromMother.size() << endmsg;
  
  if ( nLongLived==1 && inputNeutralsFromMother.size()>0 )
  {
    if (msgLevel(MSG::VERBOSE)) 
      verbose() << "Special case: won't refit, but add "
                << inputNeutralsFromMother.size()
                << " gamma(s)/pi0s to existing vertex" << endmsg;
    // Get the composite vertex in case of no fit (no X -> n gammas)

    for ( std::vector<const Particle*>::const_iterator iPart=partsToFit.begin();
          iPart!=partsToFit.end(); ++iPart )
    {
      const Particle* composite = *iPart;
      const Vertex* Vtemp = composite->endVertex();

      //Should not only decay to gammas
      //    unsigned int ngammas = 0;

      const SmartRefVector<Particle>& daughters = Vtemp->outgoingParticles();
      for ( SmartRefVector<Particle>::const_iterator iDaught=daughters.begin();
            iDaught!=daughters.end();++iDaught)
      {
        if( m_photonID == (*iDaught)->particleID().pid()
            || m_pi0ID == (*iDaught)->particleID().pid() )
        {
          inputNeutralsFromMother.push_back(*iDaught);
        }
        ++nNeutralsFromMother;
      } // iDaught

      // Do not use if only decays to gammas, e.g. J/Psi eta(2g)
      if ( nNeutralsFromMother == Vtemp->outgoingParticles().size() ) continue;

      // We have our vertex
      if (msgLevel(MSG::VERBOSE))
      {
        verbose() << " composite ID " << composite->particleID().pid()
                  << " will be used to add gammas" << endmsg;
        verbose() << " composite decay vertex position: "
                  << Vtemp->position()
                  << " , and chi2 " << Vtemp->chi2() << endmsg;
      }

      if(Vtemp) V = *Vtemp;
      else{
        fatal() << "No existing vertex found!" << endmsg;
        return StatusCode::FAILURE;
      }
    } // iPart
    fitNeeded = false ; // no need to refit
  }
  // Check wether enough particles
  else if ( (nPartsToFit == 0) || (nPartsToFit == 1))
  {
    fatal() << "Not enough particles to fit! " << nPartsToFit << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS ;
}
