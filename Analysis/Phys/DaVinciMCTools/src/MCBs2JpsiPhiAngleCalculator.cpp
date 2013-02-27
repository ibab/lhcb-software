// $Id: MCBs2JpsiPhiAngleCalculator.cpp,v 1.3 2010-04-22 11:52:53 gcowan Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/ToolFactory.h" 
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "Event/Particle.h"
#include "Event/MCParticle.h"
#include "Math/Boost.h"
// DaVinci
#include "Kernel/DaVinciP2VVAngles.h"
// local
#include "Kernel/IP2VVAngleCalculator.h"
#include "MCBs2JpsiPhiAngleCalculator.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MCBs2JpsiPhiAngleCalculator
//
// 2008-05-12 : Greig Cowan
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( MCBs2JpsiPhiAngleCalculator )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCBs2JpsiPhiAngleCalculator::MCBs2JpsiPhiAngleCalculator( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IP2VVMCPartAngleCalculator>(this);
  declareProperty( "DecayDepth" , m_depth = 2 );
}

//=============================================================================
// Destructor
//=============================================================================
MCBs2JpsiPhiAngleCalculator::~MCBs2JpsiPhiAngleCalculator() {} 

//=============================================================================

void MCBs2JpsiPhiAngleCalculator::fillDescendants( const LHCb::MCParticle* particle, 
                                                   LHCb::MCParticle::ConstVector& descendants, int depth )
{
  int pdgID = particle->particleID().abspid() ;
  if ( depth < 0 || pdgID == 22) return ;
  
  if ( pdgID == 321 || pdgID == 211 ){
    descendants.push_back( particle );
    return;
  }
  
  SmartRefVector<LHCb::MCVertex>::const_iterator vertex;
  
  for ( vertex = particle->endVertices().begin() ; 
        vertex != particle->endVertices().end() ; ++vertex ){
    
    SmartRefVector<LHCb::MCParticle>::const_iterator ipart;
    for ( ipart = (*vertex)->products().begin() ; 
          ipart != (*vertex)->products().end() ; ++ipart ){
      fillDescendants( *ipart, descendants, depth-1 ); 
    }       
  }
  
  descendants.push_back( particle ) ;
  return ;
}


StatusCode MCBs2JpsiPhiAngleCalculator::calculateAngles( const LHCb::MCParticle* particle, 
                                                           double& thetal, double& thetak, double& phi )
{
  LHCb::MCParticle::ConstVector descendants ;
  fillDescendants( particle, descendants, m_depth );
  
  if ( !(descendants.size() >= 4) ) return StatusCode::FAILURE;

  const LHCb::MCParticle* particleLeptonMinus = 0;
  const LHCb::MCParticle* particleLeptonPlus = 0;
  const LHCb::MCParticle* particleKaonMinus = 0;
  const LHCb::MCParticle* particleKaonPlus = 0;

  StatusCode sc = getParticles(particle, descendants, particleLeptonPlus, particleLeptonMinus, particleKaonPlus, particleKaonMinus);
  if (!sc) return sc;

  phi    = DaVinci::P2VVAngles::calculatePlaneAngle( particleLeptonPlus->momentum()
                                                   , particleLeptonMinus->momentum()
                                                   , particleKaonPlus->momentum()
                                                   , particleKaonMinus->momentum() );

  thetak = DaVinci::P2VVAngles::calculateHelicityPolarAngle( particleLeptonPlus->momentum()
                                                   , particleLeptonMinus->momentum()
                                                   , particleKaonPlus->momentum()
                                                   , particleKaonMinus->momentum());

  thetal = DaVinci::P2VVAngles::calculateHelicityPolarAngle( particleKaonPlus->momentum()
                                                   , particleKaonMinus->momentum()
                                                   , particleLeptonPlus->momentum()
                                                   , particleLeptonMinus->momentum());
  return StatusCode::SUCCESS ;
}


double MCBs2JpsiPhiAngleCalculator::calculateThetaK( const LHCb::MCParticle* particle  )
{
  LHCb::MCParticle::ConstVector descendants;
  fillDescendants(particle, descendants, m_depth );
 
  if ( !(descendants.size() >= 4) ) return StatusCode::FAILURE;

  const LHCb::MCParticle* particleLeptonMinus = 0;
  const LHCb::MCParticle* particleLeptonPlus = 0;
  const LHCb::MCParticle* particleKaonMinus = 0;
  const LHCb::MCParticle* particleKaonPlus = 0;
  
  StatusCode sc = getParticles(particle, descendants, particleLeptonPlus, particleLeptonMinus, particleKaonPlus, particleKaonMinus);
  if (!sc) return sc;

  double thetak = DaVinci::P2VVAngles::calculateHelicityPolarAngle( particleLeptonPlus->momentum()
                                                          , particleLeptonMinus->momentum()
                                                          , particleKaonPlus->momentum()
                                                          , particleKaonMinus->momentum());
  return thetak;
}

double MCBs2JpsiPhiAngleCalculator::calculatePhi( const LHCb::MCParticle* particle  )
{
  LHCb::MCParticle::ConstVector descendants;
  fillDescendants(particle, descendants, m_depth );

  if ( !(descendants.size() >= 4) ) return StatusCode::FAILURE;

  const LHCb::MCParticle* particleLeptonMinus = 0;
  const LHCb::MCParticle* particleLeptonPlus = 0;
  const LHCb::MCParticle* particleKaonMinus = 0;
  const LHCb::MCParticle* particleKaonPlus = 0;

  StatusCode sc = getParticles(particle, descendants, particleLeptonPlus, particleLeptonMinus, particleKaonPlus, particleKaonMinus);
  if (!sc) return sc;

  double phi = DaVinci::P2VVAngles::calculatePlaneAngle( particleLeptonPlus->momentum()
                                                       , particleLeptonMinus->momentum()
                                                       , particleKaonPlus->momentum()
                                                       , particleKaonMinus->momentum() );
  return phi;
}

double MCBs2JpsiPhiAngleCalculator::calculateThetaL( const LHCb::MCParticle* particle )
{
  LHCb::MCParticle::ConstVector descendants;
  fillDescendants(particle, descendants, m_depth );
  
  if ( !(descendants.size() >= 4) ) return StatusCode::FAILURE;

  const LHCb::MCParticle* particleLeptonMinus = 0;
  const LHCb::MCParticle* particleLeptonPlus = 0;
  const LHCb::MCParticle* particleKaonMinus = 0;
  const LHCb::MCParticle* particleKaonPlus = 0;
  
  StatusCode sc = getParticles(particle, descendants, particleLeptonPlus, particleLeptonMinus, particleKaonPlus, particleKaonMinus);
  if (!sc) return sc;
  
  double thetal = DaVinci::P2VVAngles::calculateHelicityPolarAngle( particleKaonPlus->momentum()
                                                          , particleKaonMinus->momentum()
                                                          , particleLeptonPlus->momentum()
                                                          , particleLeptonMinus->momentum());
  return thetal;
}

double MCBs2JpsiPhiAngleCalculator::calculateMass( const LHCb::MCParticle* particle )
{  
   LHCb::MCParticle::ConstVector descendants ;
   fillDescendants( particle, descendants , 2 );  
   
   LHCb::ParticleID id ;
   
   const LHCb::MCParticle* leptonMinus = 0;
   const LHCb::MCParticle* leptonPlus = 0; 
  
   LHCb::MCParticle::ConstVector::const_iterator ipart;
   
   for ( ipart = descendants.begin(); ipart != descendants.end() ; ++ipart ){  
      id = (*ipart)->particleID();
      if ( id.isLepton() ){
         if ( id.pid() > 0 ) leptonMinus = (*ipart) ;
         else leptonPlus = (*ipart) ; 
      } 
   }
  
   if ( !leptonMinus || !leptonPlus ) Exception("Could not find required particles !") ;
   
   const Gaudi::LorentzVector leptonPair = ( (leptonPlus->momentum()) + 
                                             (leptonMinus->momentum()) );
  
   double mass = leptonPair.M(); 
   return mass ;
}

StatusCode MCBs2JpsiPhiAngleCalculator::calculateTransversityAngles( 
   const LHCb::MCParticle* particle, double& Theta_tr, 
   double& Phi_tr, double& Theta_V )
{
   // When Calculating the transversity angles it is important
   // to make the distinction between +ve and -ve leptons and 
   // kaons since the angle are defined relative to them.

   LHCb::MCParticle::ConstVector descendants;
   fillDescendants( particle, descendants , m_depth );
  
   if ( !(descendants.size() >= 4) ) return StatusCode::FAILURE;

   const LHCb::MCParticle* particleLeptonPlus = 0;
   const LHCb::MCParticle* particleLeptonMinus = 0; 
   const LHCb::MCParticle* particleKaonPlus = 0;
   const LHCb::MCParticle* particleKaonMinus = 0;
  
   StatusCode sc = getParticles(particle, descendants, particleLeptonPlus, particleLeptonMinus, particleKaonPlus, particleKaonMinus);
   if (!sc) return sc;
 
   Theta_tr = DaVinci::P2VVAngles::calculateThetaTr( particleLeptonPlus->momentum()
                                                   , particleLeptonMinus->momentum()
                                                   , particleKaonPlus->momentum()
                                                   , particleKaonMinus->momentum());

   Phi_tr   = DaVinci::P2VVAngles::calculatePhiTr( particleLeptonPlus->momentum()
                                                 , particleLeptonMinus->momentum()
                                                 , particleKaonPlus->momentum()
                                                 , particleKaonMinus->momentum() );

   Theta_V  = DaVinci::P2VVAngles::calculateHelicityPolarAngle( particleLeptonPlus->momentum()
                                                , particleLeptonMinus->momentum()
                                                , particleKaonPlus->momentum()
                                                , particleKaonMinus->momentum() );
   return StatusCode::SUCCESS ;
}

double MCBs2JpsiPhiAngleCalculator::calculateTransThetaTr( 
   const LHCb::MCParticle* particle ) 
{
   LHCb::MCParticle::ConstVector descendants;
   fillDescendants( particle, descendants , m_depth );
  
   if ( !(descendants.size() >= 4) ) return StatusCode::FAILURE;

   const LHCb::MCParticle* particleLeptonPlus = 0;
   const LHCb::MCParticle* particleLeptonMinus = 0; 
   const LHCb::MCParticle* particleKaonPlus = 0;
   const LHCb::MCParticle* particleKaonMinus = 0;
  
   StatusCode sc = getParticles(particle, descendants, particleLeptonPlus, particleLeptonMinus, particleKaonPlus, particleKaonMinus);
   if (!sc) return sc;

   double Theta_tr = DaVinci::P2VVAngles::calculateThetaTr( particleLeptonPlus->momentum()
                                                          , particleLeptonMinus->momentum()
                                                          , particleKaonPlus->momentum()
                                                          , particleKaonMinus->momentum());
   return Theta_tr;
}

double MCBs2JpsiPhiAngleCalculator::calculateTransPhiTr( 
   const LHCb::MCParticle* particle ) 
{
   LHCb::MCParticle::ConstVector descendants;
   fillDescendants( particle, descendants , m_depth );
  
   if ( !(descendants.size() >= 4) ) return StatusCode::FAILURE;

   const LHCb::MCParticle* particleLeptonPlus = 0;
   const LHCb::MCParticle* particleLeptonMinus = 0; 
   const LHCb::MCParticle* particleKaonPlus = 0;
   const LHCb::MCParticle* particleKaonMinus = 0;
  
   StatusCode sc = getParticles(particle, descendants, particleLeptonPlus, particleLeptonMinus, particleKaonPlus, particleKaonMinus);
   if (!sc) return sc;

   double Phi_tr = DaVinci::P2VVAngles::calculatePhiTr( particleLeptonPlus->momentum()
                                                     , particleLeptonMinus->momentum()
                                                     , particleKaonPlus->momentum()
                                                     , particleKaonMinus->momentum() );
   return Phi_tr;
}

double MCBs2JpsiPhiAngleCalculator::calculateTransThetaV( 
   const LHCb::MCParticle* particle ) 
{
   LHCb::MCParticle::ConstVector descendants;
   fillDescendants( particle, descendants , m_depth );
  
   if ( !(descendants.size() >= 4) ) return StatusCode::FAILURE;

   const LHCb::MCParticle* particleLeptonPlus = 0;
   const LHCb::MCParticle* particleLeptonMinus = 0;
   const LHCb::MCParticle* particleKaonPlus = 0;
   const LHCb::MCParticle* particleKaonMinus = 0;

   StatusCode sc = getParticles(particle, descendants, particleLeptonPlus, particleLeptonMinus, particleKaonPlus, particleKaonMinus);
   if (!sc) return sc;

   double Theta_V = DaVinci::P2VVAngles::calculateHelicityPolarAngle( particleLeptonPlus->momentum()
                                                           , particleLeptonMinus->momentum()
                                                           , particleKaonPlus->momentum()
                                                           , particleKaonMinus->momentum());
   return Theta_V;
}

StatusCode MCBs2JpsiPhiAngleCalculator::getParticles(const LHCb::MCParticle*& particle,
						     LHCb::MCParticle::ConstVector& descendants,
						     const LHCb::MCParticle*& particleLeptonPlus,
                                                     const LHCb::MCParticle*& particleLeptonMinus,
                                                     const LHCb::MCParticle*& particleKaonPlus,
                                                     const LHCb::MCParticle*& particleKaonMinus)
{
  LHCb::ParticleID id = particle->particleID() ;

  for ( LHCb::MCParticle::ConstVector::const_iterator ipart = descendants.begin() ;
        ipart != descendants.end() ; ++ipart ){
    
    id = (*ipart)->particleID();
    
    if ( id.isLepton()) {
       if (  id.pid() > 0 ) particleLeptonMinus = (*ipart);
       else particleLeptonPlus = (*ipart);
    }
    else if ( 321 == id.abspid() ) {
       if ( id.pid() < 0 ) particleKaonMinus = (*ipart);
       else particleKaonPlus = (*ipart);
    }
  }
  
  if ( !particleLeptonMinus || !particleLeptonPlus || 
       !particleKaonMinus || !particleKaonPlus ) {
    
    err() << "Could not find required particles !" << endmsg;
    return StatusCode::FAILURE;
  }
    return StatusCode::SUCCESS;
}
