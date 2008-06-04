// $Id: MCBs2JpsiPhiAngleCalculator.cpp,v 1.1 2008-06-04 16:17:29 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/ToolFactory.h" 
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "Event/Particle.h"
#include "Event/MCParticle.h"
#include "Math/Boost.h"
// local
#include "Kernel/IP2VVAngleCalculator.h"
#include "MCBs2JpsiPhiAngleCalculator.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MCBs2JpsiPhiAngleCalculator
//
// 2008-05-12 : Greig Cowan
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( MCBs2JpsiPhiAngleCalculator );


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


StatusCode MCBs2JpsiPhiAngleCalculator::initialize() {
  
  StatusCode sc = GaudiTool::initialize() ;
  if (!sc) return sc ;
  
  info() << "Initializing Angle Calculator Tool" << endmsg ;
  
  m_angle = tool<IP2VVAngleCalculator>("P2VVAngleCalculator",this);

  return sc ;
}


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
  fillDescendants( particle, descendants , m_depth );
  
  if ( !(descendants.size() >= 4) ) return StatusCode::FAILURE;

  const LHCb::MCParticle* particleLeptonMinus = 0;
  const LHCb::MCParticle* particleLeptonPlus = 0;
  const LHCb::MCParticle* particleKaonMinus = 0;
  const LHCb::MCParticle* particleKaonPlus = 0;

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
    return StatusCode::FAILURE ;
  }  
    
  
  thetak = m_angle->calculatePolarAngle( particle->momentum() , 
                                         particleKaonPlus->momentum(),
                                         particleKaonMinus->momentum() );
  thetak = Gaudi::Units::pi -thetak;
  
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

double MCBs2JpsiPhiAngleCalculator::calculateThetaK( const LHCb::MCParticle* particle  )
{
  LHCb::MCParticle::ConstVector descendants;
  fillDescendants(particle, descendants , m_depth );
 
  
  const LHCb::MCParticle* particleKaonMinus = 0;
  const LHCb::MCParticle* particleKaonPlus = 0;
  LHCb::ParticleID id;
  
  for ( LHCb::MCParticle::ConstVector::const_iterator idaughter = descendants.begin() ;
        idaughter != descendants.end(); ++idaughter ){
    
    id = (*idaughter)->particleID() ;
   
    if ( 321 == id.abspid() ) {
      if ( id.pid() < 0)  particleKaonMinus = (*idaughter); 
      else particleKaonPlus = (*idaughter);
    }
  }
  
  
  if ( !particleKaonMinus || !particleKaonPlus){
    Exception("Could not find required particles !");
  }
  
  double thetak = m_angle->calculatePolarAngle( particle->momentum() , 
                                         particleKaonPlus->momentum(), 
                                         particleKaonMinus->momentum() );  

  thetak = Gaudi::Units::pi - thetak ;//flip angle due to definition 
  return thetak;
}

double MCBs2JpsiPhiAngleCalculator::calculatePhi( const LHCb::MCParticle* particle  )
{
  LHCb::MCParticle::ConstVector descendants;
  fillDescendants(particle, descendants , m_depth );

  const LHCb::MCParticle* particleLeptonMinus = 0;
  const LHCb::MCParticle* particleLeptonPlus = 0;
  const LHCb::MCParticle* particleKaonMinus = 0;
  const LHCb::MCParticle* particleKaonPlus = 0;

  LHCb::ParticleID id;
  
  for ( LHCb::MCParticle::ConstVector::const_iterator ipart = descendants.begin(); 
        ipart != descendants.end(); ++ipart ){
    
    id = (*ipart)->particleID();
    
    if ( id.isLepton() ){       
      if (  id.pid() > 0 ) particleLeptonMinus = (*ipart);
      else particleLeptonPlus = (*ipart);
    }
    
    else if ( 321 == id.abspid() ) {
       if (id.pid() < 0) particleKaonMinus = (*ipart); 
       else particleKaonPlus = (*ipart);
    }
  }

  if ( !particleLeptonMinus || !particleLeptonPlus || 
       !particleKaonMinus || !particleKaonPlus ) {
    Exception("Could not find required particles !");
  }
  
  double phi    = m_angle->calculatePlaneAngle( particle->momentum(), 
                                         particleLeptonPlus->momentum(), 
                                         particleLeptonMinus->momentum(), 
                                         particleKaonPlus->momentum(),
                                         particleKaonMinus->momentum() );
  return phi;
}


double MCBs2JpsiPhiAngleCalculator::calculateThetaL( const LHCb::MCParticle* particle )
{
  LHCb::MCParticle::ConstVector descendants;
  fillDescendants(particle, descendants ,2 );
  
  const LHCb::MCParticle* particleLeptonMinus = 0;
  const LHCb::MCParticle* particleLeptonPlus = 0;
  
  LHCb::ParticleID id;
  
  // loop over descendants and find leptons 
  for ( LHCb::MCParticle::ConstVector::const_iterator ipart = descendants.begin() ;
        ipart != descendants.end() ; ++ipart ){
    
    id = (*ipart)->particleID();
    if ( id.isLepton() ){
      if ( id.pid() > 0 ) particleLeptonMinus = (*ipart);
      else particleLeptonPlus = (*ipart);
    }
  }

  if ( !particleLeptonMinus || !particleLeptonPlus ) {
    Exception("Could not find required particles !") ;
  }  

  double thetal = m_angle->calculatePolarAngle( particle->momentum(), 
                                         particleLeptonPlus->momentum(),
                                         particleLeptonMinus->momentum() ) ;

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
   
   LHCb::ParticleID id ;
  
  for ( LHCb::MCParticle::ConstVector::const_iterator 
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

double MCBs2JpsiPhiAngleCalculator::calculateTransThetaTr( 
   const LHCb::MCParticle* particle ) 
{

   LHCb::MCParticle::ConstVector descendants;
   fillDescendants( particle, descendants , m_depth );
  
   const LHCb::MCParticle* particleLeptonPlus = 0;
   const LHCb::MCParticle* particleLeptonMinus = 0; 
   const LHCb::MCParticle* particleKaonPlus = 0;
   const LHCb::MCParticle* particleKaonMinus = 0;
  
   LHCb::ParticleID id ;
  
   for ( LHCb::MCParticle::ConstVector::const_iterator idaughter = 
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

  
  double Theta_tr = m_angle->calculateThetaTr(particleLeptonPlus->momentum(), 
                                               particleLeptonMinus->momentum(),
                                               particleKaonPlus->momentum(),
                                               particleKaonMinus->momentum() );
  
  return Theta_tr;
   
}

double MCBs2JpsiPhiAngleCalculator::calculateTransPhiTr( 
   const LHCb::MCParticle* particle ) 
{

   LHCb::MCParticle::ConstVector descendants;
   fillDescendants( particle, descendants , m_depth );
  
   const LHCb::MCParticle* particleLeptonPlus = 0;
   const LHCb::MCParticle* particleLeptonMinus = 0; 
   const LHCb::MCParticle* particleKaonPlus = 0;
   const LHCb::MCParticle* particleKaonMinus = 0;
  
   LHCb::ParticleID id ;
  
   for ( LHCb::MCParticle::ConstVector::const_iterator idaughter = 
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
  
  
  double Phi_tr = m_angle->calculatePhiTr(particleLeptonPlus->momentum(), 
                                           particleLeptonMinus->momentum(),
                                           particleKaonPlus->momentum(),
                                           particleKaonMinus->momentum() );
  
  return Phi_tr;

}

double MCBs2JpsiPhiAngleCalculator::calculateTransThetaV( 
   const LHCb::MCParticle* particle ) 
{

   LHCb::MCParticle::ConstVector descendants;
   fillDescendants( particle, descendants , m_depth );
  
   const LHCb::MCParticle* particleKaonPlus = 0;
   const LHCb::MCParticle* particleKaonMinus = 0;
  
   LHCb::ParticleID id ;
  
   for ( LHCb::MCParticle::ConstVector::const_iterator idaughter = 
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

