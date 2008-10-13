// $Id: Bd2KstarMuMuAngleCalculator.cpp,v 1.6 2008-10-13 08:50:28 jpalac Exp $
// Include files 

// from Gaudi
//#include "GaudiKernel/ToolFactory.h" 
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "Event/Particle.h" 
#include "Kernel/IParticleDescendants.h"
#include "Kernel/IP2VVAngleCalculator.h"

// local
#include "Bd2KstarMuMuAngleCalculator.h"
#include "Math/Boost.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Bd2KstarMuMuAngleCalculator
//
// 2007-08-02 : Thomas Blake
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( Bd2KstarMuMuAngleCalculator );
// Declaration of the AlgTool Factory

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Bd2KstarMuMuAngleCalculator::Bd2KstarMuMuAngleCalculator( const std::string& type,
                                                          const std::string& name,
                                                          const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IP2VVPartAngleCalculator>(this);    
}
//=============================================================================
// Destructor
//=============================================================================
Bd2KstarMuMuAngleCalculator::~Bd2KstarMuMuAngleCalculator() {} 

//=============================================================================


StatusCode Bd2KstarMuMuAngleCalculator::initialize() {
  
  StatusCode sc = GaudiTool::initialize() ;
  if (!sc) return sc ;
  
  if (msgLevel(MSG::DEBUG)) debug() << "Initializing Angle Calculator Tool" << endmsg ;
  
  m_descendants = tool<IParticleDescendants>("ParticleDescendants ",this);
  m_angle = tool<IP2VVAngleCalculator>("P2VVAngleCalculator",this);

  return sc ;
}



StatusCode Bd2KstarMuMuAngleCalculator::calculateAngles( 
   const LHCb::Particle* particle, double& thetal, 
   double& thetak, double& phi)
{

  LHCb::Particle::ConstVector descendants = m_descendants->finalStates(particle) ; //->daughtersVector() ;
  int motherID = particle->particleID().pid();
  
  if ( !(descendants.size() >= 4) ) return StatusCode::FAILURE;
  
  const LHCb::Particle* particleLepton1 = 0;
  const LHCb::Particle* particleLepton2 = 0; 
  const LHCb::Particle* particleKaon = 0;
  const LHCb::Particle* particlePion = 0;
  
  LHCb::ParticleID id ;
  
  for ( LHCb::Particle::ConstVector::const_iterator idaughter = descendants.begin() ;
        idaughter != descendants.end(); ++idaughter ){
    
    id = (*idaughter)->particleID() ;

    if ( id.isLepton() ){
      if ( motherID*id.pid() < 0 ) particleLepton1 = (*idaughter);
      else particleLepton2 = (*idaughter);
    }
    
    else if ( 321 == id.abspid() ) particleKaon = (*idaughter); 
    else if ( 211 == id.abspid() ) particlePion = (*idaughter);
  }
  
  if ( !particleKaon || !particleLepton1 || !particleLepton2 || !particlePion){
    error() << "Could not find required particles !" << endmsg;
    return StatusCode::FAILURE ;
  }
  
  
  thetak = m_angle->calculatePolarAngle( particle->momentum() , 
                                         particleKaon->momentum(),
                                         particlePion->momentum() );
  thetak = Gaudi::Units::pi - thetak; // def of axis  

  phi    = m_angle->calculatePlaneAngle( particle->momentum(), 
                                         particleLepton1->momentum(), 
                                         particleLepton2->momentum(), 
                                         particleKaon->momentum(),
                                         particlePion->momentum() );
  
  thetal = m_angle->calculatePolarAngle( particle->momentum(), 
                                         particleLepton1->momentum(),
                                         particleLepton2->momentum() );
  
  return StatusCode::SUCCESS ;
}

StatusCode Bd2KstarMuMuAngleCalculator::calculateTransversityAngles( 
   const LHCb::Particle* particle,
   double& Theta_tr, double& Phi_tr, double& Theta_V )
{

  LHCb::Particle::ConstVector descendants = m_descendants->finalStates(particle) ; //->daughtersVector() ;
  int motherID = particle->particleID().pid();
  
  if ( !(descendants.size() >= 4) ) return StatusCode::FAILURE;
  
  const LHCb::Particle* particleLepton1 = 0;
  const LHCb::Particle* particleLepton2 = 0; 
  const LHCb::Particle* particleKaon = 0;
  const LHCb::Particle* particlePion = 0;
  
  LHCb::ParticleID id ;
  
  for ( LHCb::Particle::ConstVector::const_iterator idaughter = descendants.begin() ;
        idaughter != descendants.end(); ++idaughter ){
    
    id = (*idaughter)->particleID() ;

    if ( id.isLepton() ){
      if ( motherID*id.pid() < 0 ) particleLepton1 = (*idaughter);
      else particleLepton2 = (*idaughter);
    }
    
    else if ( 321 == id.abspid() ) particleKaon = (*idaughter); 
    else if ( 211 == id.abspid() ) particlePion = (*idaughter);
  }
  
  if ( !particleKaon || !particleLepton1 || !particleLepton2 || !particlePion){
    error() << "Could not find required particles !" << endmsg;
    return StatusCode::FAILURE ;
  }
  
  
  Theta_tr = m_angle->calculateThetaTr( particleLepton1->momentum(), 
                                        particleLepton2->momentum(),
                                        particleKaon->momentum(),
                                        particlePion->momentum() );
  
  Phi_tr = m_angle->calculatePhiTr( particleLepton1->momentum(), 
                                    particleLepton2->momentum(),
                                    particleKaon->momentum(),
                                    particlePion->momentum() );
  
  Theta_V = m_angle->calculatePolarAngle( particle->momentum(),
                                          particleKaon->momentum(),
                                          particlePion->momentum() );
  
      
  return StatusCode::SUCCESS ;
}



double Bd2KstarMuMuAngleCalculator::calculatePhi( const LHCb::Particle* particle )
{
  LHCb::Particle::ConstVector descendants = m_descendants->finalStates(particle) ; //->daughtersVector() ;
  int motherID = particle->particleID().pid();
  
  const LHCb::Particle* particleLepton1 = 0;
  const LHCb::Particle* particleLepton2 = 0; 
  const LHCb::Particle* particleKaon = 0;
  const LHCb::Particle* particlePion = 0;
  
  LHCb::ParticleID id ;
  
  for ( LHCb::Particle::ConstVector::const_iterator idaughter = descendants.begin() ;
        idaughter != descendants.end(); ++idaughter ){
    
    id = (*idaughter)->particleID() ;

    if ( id.isLepton() ){
      if ( motherID*id.pid() < 0 ) particleLepton1 = (*idaughter);
      else particleLepton2 = (*idaughter);
    }

    else if ( 321 == id.abspid() ) particleKaon = (*idaughter); 
    else if ( 211 == id.abspid() ) particlePion = (*idaughter);
  }
  
  if ( !particleKaon || !particleLepton1 || !particleLepton2 || !particlePion){
    Exception("Could not find required particles !");
  }
  
  double phi = m_angle->calculatePlaneAngle( particle->momentum(), 
                                             particleLepton1->momentum(), 
                                             particleLepton2->momentum(), 
                                             particleKaon->momentum(),
                                             particlePion->momentum() );
  
  return phi ;
}


double Bd2KstarMuMuAngleCalculator::calculateThetaL( const LHCb::Particle* particle )
{
  
  LHCb::Particle::ConstVector descendants = m_descendants->finalStates(particle) ;
  int motherID = particle->particleID().pid();
  
  const LHCb::Particle* particleLepton1 = 0;
  const LHCb::Particle* particleLepton2 = 0;
  
  for ( LHCb::Particle::ConstVector::const_iterator idaughter = descendants.begin() ;
        idaughter != descendants.end(); ++idaughter ){
    
    LHCb::ParticleID id = (*idaughter)->particleID() ;
    if ( id.isLepton() ){
      if ( motherID*id.pid() < 0 ) particleLepton1 = (*idaughter);
      else particleLepton2 = (*idaughter);
    }
  }
  
  if ( !particleLepton1 || !particleLepton2 ){
    Exception("Could not find required particles !");
  }
  
  double thetal = m_angle->calculatePolarAngle( particle->momentum(), 
                                                particleLepton1->momentum() ,
                                                particleLepton2->momentum() );
  
  return thetal ;
}


double Bd2KstarMuMuAngleCalculator::calculateThetaK( const LHCb::Particle* particle ) 
{
  LHCb::Particle::ConstVector descendants = m_descendants->finalStates(particle) ;  
 
  const LHCb::Particle* particleKaon = 0;
  const LHCb::Particle* particlePion = 0;
  
  for ( LHCb::Particle::ConstVector::const_iterator idaughter = descendants.begin() ;
        idaughter != descendants.end(); ++idaughter ){
    
    LHCb::ParticleID id = (*idaughter)->particleID() ;
   
    if ( 321 == id.abspid() ) particleKaon = (*idaughter); 
    else if ( 211 == id.abspid() ) particlePion = (*idaughter);
  }
  
  if ( !particleKaon || !particlePion){
    Exception("Could not find required particles !");
  }
  
  double thetak = m_angle->calculatePolarAngle( particle->momentum() , 
                                                particleKaon->momentum(), 
                                                particlePion->momentum() );  
  return Gaudi::Units::pi - thetak ;
}



double Bd2KstarMuMuAngleCalculator::calculateTransThetaTr( 
   const LHCb::Particle* particle ) 
{

   LHCb::Particle::ConstVector descendants = m_descendants->finalStates(particle); //->daughtersVector() ;
   int motherID = particle->particleID().pid();
  
   const LHCb::Particle* particleLepton1 = 0;
   const LHCb::Particle* particleLepton2 = 0; 
   const LHCb::Particle* particleKaon = 0;
   const LHCb::Particle* particlePion = 0;
  
   LHCb::ParticleID id ;
  
   for ( LHCb::Particle::ConstVector::const_iterator idaughter = 
            descendants.begin() ;
         idaughter != descendants.end(); ++idaughter ){
    
      id = (*idaughter)->particleID() ;

      if ( id.isLepton() ){
         if ( motherID*id.pid() < 0 ) particleLepton1 = (*idaughter);
         else particleLepton2 = (*idaughter);
      }
      else if ( 321 == id.abspid() ) particleKaon = (*idaughter); 
      else if ( 211 == id.abspid() ) particlePion = (*idaughter);
   }
  
   if ( !particleKaon || !particleLepton1 || !particleLepton2 
        || !particlePion){
     Exception("Could not find required particles !");
   }
  
  
  double Theta_tr = m_angle->calculateThetaTr( particleLepton1->momentum(), 
                                               particleLepton2->momentum(),
                                               particleKaon->momentum(),
                                               particlePion->momentum() );
  
  return Theta_tr;
   
}

double Bd2KstarMuMuAngleCalculator::calculateTransPhiTr( 
   const LHCb::Particle* particle ) 
{

   LHCb::Particle::ConstVector descendants = m_descendants->finalStates(particle); //->daughtersVector() ;
   int motherID = particle->particleID().pid();
   
   const LHCb::Particle* particleLepton1 = 0;
   const LHCb::Particle* particleLepton2 = 0; 
   const LHCb::Particle* particleKaon = 0;
   const LHCb::Particle* particlePion = 0;
  
   LHCb::ParticleID id ;
  
   for ( LHCb::Particle::ConstVector::const_iterator idaughter = 
            descendants.begin() ;
         idaughter != descendants.end(); ++idaughter ){
    
      id = (*idaughter)->particleID() ;

      if ( id.isLepton() ){
         if ( motherID*id.pid() < 0 ) particleLepton1 = (*idaughter);
         else particleLepton2 = (*idaughter);
      }
      else if ( 321 == id.abspid() ) particleKaon = (*idaughter); 
      else if ( 211 == id.abspid() ) particlePion = (*idaughter);
   }
  
   if ( !particleKaon || !particleLepton1 || !particleLepton2 
        || !particlePion){
     Exception("Could not find required particles !");
   }
  
  
   double Phi_tr = m_angle->calculatePhiTr( particleLepton1->momentum(), 
                                            particleLepton2->momentum(),
                                            particleKaon->momentum(),
                                            particlePion->momentum() );
   
  return Phi_tr;

}

double Bd2KstarMuMuAngleCalculator::calculateTransThetaV( 
   const LHCb::Particle* particle ) 
{

   LHCb::Particle::ConstVector descendants = m_descendants->finalStates(particle); //->daughtersVector() ;
  
   const LHCb::Particle* particleKaon = 0;
   const LHCb::Particle* particlePion = 0;
  
   LHCb::ParticleID id ;
  
   for ( LHCb::Particle::ConstVector::const_iterator idaughter = 
            descendants.begin() ;
         idaughter != descendants.end(); ++idaughter ){
    
      id = (*idaughter)->particleID() ;
      
      if ( 321 == id.abspid() ) particleKaon = (*idaughter); 
      if ( 211 == id.abspid() ) particlePion = (*idaughter);
   }
  
   if ( !particleKaon || !particlePion ){
     Exception("Could not find required particles !");
   }
  
   
   double Theta_V = m_angle->calculatePolarAngle( particle->momentum(),
                                                  particleKaon->momentum(),
                                                  particlePion->momentum() );
  
   return Theta_V;
}

