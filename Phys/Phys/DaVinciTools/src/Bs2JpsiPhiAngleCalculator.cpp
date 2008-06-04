// $Id: Bs2JpsiPhiAngleCalculator.cpp,v 1.1 2008-06-04 16:18:14 pkoppenb Exp $
// Include files 

// from Gaudi
//#include "GaudiKernel/ToolFactory.h" 
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "Event/Particle.h" 
#include "Kernel/IParticleDescendants.h"
#include "Kernel/IP2VVAngleCalculator.h"

// local
#include "Bs2JpsiPhiAngleCalculator.h"
#include "Math/Boost.h"
//-----------------------------------------------------------------------------
// Implementation file for class : Bs2JpsiPhiAngleCalculator
//
// 2008-06-02 : Greig Cowan
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( Bs2JpsiPhiAngleCalculator );
// Declaration of the AlgTool Factory

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Bs2JpsiPhiAngleCalculator::Bs2JpsiPhiAngleCalculator( const std::string& type,
                                                      const std::string& name,
                                                      const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IP2VVPartAngleCalculator>(this);    
}
//=============================================================================
// Destructor
//=============================================================================
Bs2JpsiPhiAngleCalculator::~Bs2JpsiPhiAngleCalculator() {} 

//=============================================================================


StatusCode Bs2JpsiPhiAngleCalculator::initialize() {
  
  StatusCode sc = GaudiTool::initialize() ;
  if (!sc) return sc ;
  
  if (msgLevel(MSG::DEBUG)) debug() << "Initializing Angle Calculator Tool" 
                                    << endmsg ;
  
  m_descendants = tool<IParticleDescendants>("ParticleDescendants ",this);
  m_angle = tool<IP2VVAngleCalculator>("P2VVAngleCalculator",this);

  return sc ;
}



StatusCode Bs2JpsiPhiAngleCalculator::calculateAngles( 
   const LHCb::Particle* particle, double& thetal,
   double& thetak, double& phi )
{
   // When Calculating the transversity angles it is important
   // to make the distinction between +ve and -ve leptons and 
   // kaons since the angle are defined relative to them.
   
  LHCb::Particle::ConstVector descendants = m_descendants->
     finalStates(particle); //->daughtersVector() ;
  
  if ( !(descendants.size() >= 4) ) return StatusCode::FAILURE;
  
  const LHCb::Particle* particleLeptonPlus = 0;
  const LHCb::Particle* particleLeptonMinus = 0; 
  const LHCb::Particle* particleKaonPlus = 0;
  const LHCb::Particle* particleKaonMinus = 0;
  
  LHCb::ParticleID id ;
  
  for ( LHCb::Particle::ConstVector::const_iterator 
           idaughter = descendants.begin() ;
        idaughter != descendants.end(); ++idaughter ){
    
    id = (*idaughter)->particleID() ;

    //if ( id.isLepton() ){
    if ( id.isLepton() ){
      if ( id.pid() > 0 ) particleLeptonMinus = (*idaughter);
      else particleLeptonPlus = (*idaughter);
    }
    
    else if ( 321 == id.abspid() ) {
       if (id.pid() < 0) particleKaonMinus = (*idaughter); 
       else particleKaonPlus = (*idaughter);
    }
    
  }
  
  if ( !particleKaonMinus || !particleLeptonMinus || !particleLeptonPlus 
       || !particleKaonPlus){
    error() << "Could not find required particles !" << endmsg;
    return StatusCode::FAILURE ;
  }
  
  
  thetak = m_angle->calculatePolarAngle( particle->momentum() , 
                                         particleKaonPlus->momentum(),
                                         particleKaonMinus->momentum() );
  thetak = Gaudi::Units::pi - thetak; // def of axis  

  phi    = m_angle->calculatePlaneAngle( particle->momentum(), 
                                         particleLeptonPlus->momentum(), 
                                         particleLeptonMinus->momentum(), 
                                         particleKaonPlus->momentum(),
                                         particleKaonMinus->momentum() );
  
  thetal = m_angle->calculatePolarAngle( particle->momentum(), 
                                         particleLeptonPlus->momentum(),
                                         particleLeptonMinus->momentum() );

  return StatusCode::SUCCESS ;
}


StatusCode Bs2JpsiPhiAngleCalculator::calculateTransversityAngles( 
   const LHCb::Particle* particle, double& Theta_tr, 
   double& Phi_tr, double& Theta_V )
{
   // When Calculating the transversity angles it is important
   // to make the distinction between +ve and -ve leptons and 
   // kaons since the angle are defined relative to them.

   LHCb::Particle::ConstVector descendants = m_descendants->
     finalStates(particle); //->daughtersVector() ;
  
  if ( !(descendants.size() >= 4) ) return StatusCode::FAILURE;
  
  const LHCb::Particle* particleLeptonPlus = 0;
  const LHCb::Particle* particleLeptonMinus = 0; 
  const LHCb::Particle* particleKaonPlus = 0;
  const LHCb::Particle* particleKaonMinus = 0;
  
  LHCb::ParticleID id ;
  
  for ( LHCb::Particle::ConstVector::const_iterator 
           idaughter = descendants.begin() ;
        idaughter != descendants.end(); ++idaughter ){
    
    id = (*idaughter)->particleID() ;

    //if ( id.isLepton() ){
    if ( id.isLepton() ){
      if ( id.pid() > 0 ) particleLeptonMinus = (*idaughter);
      else particleLeptonPlus = (*idaughter);
    }
    
    else if ( 321 == id.abspid() ) {
       if (id.pid() < 0) particleKaonMinus = (*idaughter); 
       else particleKaonPlus = (*idaughter);
    }
    
  }
  
  if ( !particleKaonMinus || !particleLeptonMinus || !particleLeptonPlus 
       || !particleKaonPlus){
    error() << "Could not find required particles !" << endmsg;
    return StatusCode::FAILURE ;
  }
  
  
  Theta_tr = m_angle->calculateThetaTr( particleLeptonPlus->momentum(), 
                                         particleLeptonMinus->momentum(),
                                         particleKaonPlus->momentum(),
                                         particleKaonMinus->momentum() );
  
  Phi_tr = m_angle->calculatePhiTr( particleLeptonPlus->momentum(), 
                                     particleLeptonMinus->momentum(),
                                     particleKaonPlus->momentum(),
                                     particleKaonMinus->momentum() );
  
  Theta_V = m_angle->calculatePolarAngle( particle->momentum(), 
                                          particleKaonPlus->momentum(),
                                          particleKaonMinus->momentum() );
  

      
  return StatusCode::SUCCESS ;
}



double Bs2JpsiPhiAngleCalculator::calculatePhi( const LHCb::Particle* particle)
{
  LHCb::Particle::ConstVector descendants = m_descendants->finalStates(particle) ; //->daughtersVector() ;
  
  const LHCb::Particle* particleLeptonPlus = 0;
  const LHCb::Particle* particleLeptonMinus = 0; 
  const LHCb::Particle* particleKaonPlus = 0;
  const LHCb::Particle* particleKaonMinus = 0;
  
  LHCb::ParticleID id ;
  
  for ( LHCb::Particle::ConstVector::const_iterator idaughter = 
           descendants.begin() ;
        idaughter != descendants.end(); ++idaughter ){
    
    id = (*idaughter)->particleID() ;

    if ( id.isLepton() ){
      if ( id.pid() > 0 ) particleLeptonMinus = (*idaughter);
      else particleLeptonPlus = (*idaughter);
    }
     
    else if ( 321 == id.abspid() ) {
       if (id.pid() < 0) particleKaonMinus = (*idaughter); 
       else particleKaonPlus = (*idaughter);
    }   
  }
  
  if ( !particleKaonMinus || !particleLeptonMinus || !particleLeptonPlus 
       || !particleKaonPlus){
    Exception("Could not find required particles !");
  }
  
  
  double phi = m_angle->calculatePlaneAngle( particle->momentum(), 
                                             particleLeptonPlus->momentum(), 
                                             particleLeptonMinus->momentum(), 
                                             particleKaonPlus->momentum(),
                                             particleKaonMinus->momentum() );
  
  return phi ;
}


double Bs2JpsiPhiAngleCalculator::calculateThetaL( 
const LHCb::Particle* particle )
{
  
  LHCb::Particle::ConstVector descendants = 
     m_descendants->finalStates(particle) ; //->daughtersVector() ;

  const LHCb::Particle* particleLeptonPlus = 0;
  const LHCb::Particle* particleLeptonMinus = 0;

  for ( LHCb::Particle::ConstVector::const_iterator idaughter = 
           descendants.begin() ;
        idaughter != descendants.end(); ++idaughter ){
    
     LHCb::ParticleID id = (*idaughter)->particleID() ;

     if ( id.isLepton() ){
        if ( id.pid() > 0 ) particleLeptonMinus = (*idaughter);
        else particleLeptonPlus = (*idaughter);
     }
  }
  
  if ( !particleLeptonPlus || !particleLeptonMinus ){
    Exception("Could not find required particles !");
  }
  
  double thetal = m_angle->calculatePolarAngle( particle->momentum(), 
                                                particleLeptonPlus->momentum(),
                                                particleLeptonMinus->momentum() );
  return thetal ;
}


double Bs2JpsiPhiAngleCalculator::calculateThetaK( 
   const LHCb::Particle* particle ) 
{
  LHCb::Particle::ConstVector descendants=m_descendants->finalStates(particle);
  
  const LHCb::Particle* particleKaonPlus = 0;
  const LHCb::Particle* particleKaonMinus = 0;
  
  for ( LHCb::Particle::ConstVector::const_iterator idaughter = 
           descendants.begin() ;
        idaughter != descendants.end(); ++idaughter ){
    
     LHCb::ParticleID id = (*idaughter)->particleID() ;

     if ( 321 == id.abspid() ) {
        if (id.pid() < 0) particleKaonMinus = (*idaughter); 
        else particleKaonPlus = (*idaughter);
     }
  }
  
  if ( !particleKaonPlus || !particleKaonMinus ){
    Exception("Could not find required particles !");
  }
  
  double thetak = m_angle->calculatePolarAngle( particle->momentum() , 
                                                particleKaonPlus->momentum(), 
                                                particleKaonMinus->momentum());
  return Gaudi::Units::pi - thetak ;
}

double Bs2JpsiPhiAngleCalculator::calculateTransThetaTr( 
   const LHCb::Particle* particle ) 
{

   LHCb::Particle::ConstVector descendants = m_descendants->finalStates(particle); //->daughtersVector() ;
  
   const LHCb::Particle* particleLeptonPlus = 0;
   const LHCb::Particle* particleLeptonMinus = 0; 
   const LHCb::Particle* particleKaonPlus = 0;
   const LHCb::Particle* particleKaonMinus = 0;
  
   LHCb::ParticleID id ;
  
   for ( LHCb::Particle::ConstVector::const_iterator idaughter = 
            descendants.begin() ;
         idaughter != descendants.end(); ++idaughter ){
    
      id = (*idaughter)->particleID() ;

      if ( id.isLepton() ){
         if ( id.pid() > 0 ) particleLeptonMinus = (*idaughter);
         else particleLeptonPlus = (*idaughter);
      }
     
      else if ( 321 == id.abspid() ) {
         if (id.pid() < 0) particleKaonMinus = (*idaughter); 
         else particleKaonPlus = (*idaughter);
      }   
   }
  
   if ( !particleKaonMinus || !particleLeptonMinus || !particleLeptonPlus 
        || !particleKaonPlus){
     Exception("Could not find required particles !");
   }

  
  double Theta_tr = m_angle->calculateThetaTr( particleLeptonPlus->momentum(), 
					       particleLeptonMinus->momentum(),
					       particleKaonPlus->momentum(),
                                               particleKaonMinus->momentum() );
  
  return Theta_tr;
   
}

double Bs2JpsiPhiAngleCalculator::calculateTransPhiTr( 
   const LHCb::Particle* particle ) 
{

   LHCb::Particle::ConstVector descendants = m_descendants->finalStates(particle); //->daughtersVector() ;
  
   const LHCb::Particle* particleLeptonPlus = 0;
   const LHCb::Particle* particleLeptonMinus = 0; 
   const LHCb::Particle* particleKaonPlus = 0;
   const LHCb::Particle* particleKaonMinus = 0;
  
   LHCb::ParticleID id ;
  
   for ( LHCb::Particle::ConstVector::const_iterator idaughter = 
            descendants.begin() ;
         idaughter != descendants.end(); ++idaughter ){
    
      id = (*idaughter)->particleID() ;

      if ( id.isLepton() ){
         if ( id.pid() > 0 ) particleLeptonMinus = (*idaughter);
         else particleLeptonPlus = (*idaughter);
      }
     
      else if ( 321 == id.abspid() ) {
         if (id.pid() < 0) particleKaonMinus = (*idaughter); 
         else particleKaonPlus = (*idaughter);
      }   
   }
  
   if ( !particleKaonMinus || !particleLeptonMinus || !particleLeptonPlus 
        || !particleKaonPlus){
       Exception("Could not find required particles !");
   }
  
  
  double Phi_tr = m_angle->calculatePhiTr( particleLeptonPlus->momentum(), 
                                           particleLeptonMinus->momentum(),
                                           particleKaonPlus->momentum(),
                                           particleKaonMinus->momentum() );
  
  return Phi_tr;

}

double Bs2JpsiPhiAngleCalculator::calculateTransThetaV( 
   const LHCb::Particle* particle ) 
{

   LHCb::Particle::ConstVector descendants = m_descendants->finalStates(particle); //->daughtersVector() ;
  
   const LHCb::Particle* particleKaonPlus = 0;
   const LHCb::Particle* particleKaonMinus = 0;
  
   LHCb::ParticleID id ;
  
   for ( LHCb::Particle::ConstVector::const_iterator idaughter = 
            descendants.begin() ;
         idaughter != descendants.end(); ++idaughter ){
    
      id = (*idaughter)->particleID() ;

      if ( 321 == id.abspid() ) {
        if (id.pid() < 0) particleKaonMinus = (*idaughter); 
        else particleKaonPlus = (*idaughter);
      }   
   }
  
   if ( !particleKaonMinus || !particleKaonPlus){
        Exception("Could not find required particles !");
   }
  
  
   double Theta_V = m_angle->calculatePolarAngle( particle->momentum(), 
                                                  particleKaonPlus->momentum(),
                                                  particleKaonMinus->momentum() );
   
   return Theta_V;
}

