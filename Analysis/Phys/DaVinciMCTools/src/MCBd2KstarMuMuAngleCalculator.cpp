// $Id: MCBd2KstarMuMuAngleCalculator.cpp,v 1.1 2007-09-18 14:01:55 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "Event/Particle.h"
#include "Event/MCParticle.h"
#include "Math/Boost.h"
// local
#include "Kernel/IP2VVAngleCalculator.h"
#include "MCBd2KstarMuMuAngleCalculator.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MCBd2KstarMuMuAngleCalculator
//
// 2007-08-13 : Thomas Blake
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( MCBd2KstarMuMuAngleCalculator );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCBd2KstarMuMuAngleCalculator::MCBd2KstarMuMuAngleCalculator( const std::string& type,
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
MCBd2KstarMuMuAngleCalculator::~MCBd2KstarMuMuAngleCalculator() {} 

//=============================================================================


StatusCode MCBd2KstarMuMuAngleCalculator::initialize() {
  
  StatusCode sc = GaudiTool::initialize() ;
  if (!sc) return sc ;
  
  info() << "Initializing Angle Calculator Tool" << endmsg ;
  
  m_angle = tool<IP2VVAngleCalculator>("P2VVAngleCalculator",this);

  return sc ;
}


void MCBd2KstarMuMuAngleCalculator::fillDescendants( const LHCb::MCParticle* particle, 
                                         LHCb::MCParticle::ConstVector& descendants, int depth )
{
  if ( depth < 0 ) return ;
  
  SmartRefVector<LHCb::MCVertex>::const_iterator vertex;
  
  for ( vertex = particle->endVertices().begin() ; 
        vertex != particle->endVertices().end() ; ++vertex ){
    
    SmartRefVector<LHCb::MCParticle>::const_iterator ipart;
    for ( ipart = (*vertex)->products().begin() ; 
          ipart != (*vertex)->products().end() ; ++ipart ){
      fillDescendants( *ipart, descendants, depth-1 ); 
    }       
  }
  if ( particle->particleID().pid() != 22 ) 
    descendants.push_back( particle ) ;
  return ;
}


StatusCode MCBd2KstarMuMuAngleCalculator::calculateAngles( const LHCb::MCParticle* particle, 
                                                           double& thetal, double& thetak, double& phi )
{
  LHCb::MCParticle::ConstVector descendants ;
  fillDescendants( particle, descendants , m_depth );
  
  const LHCb::MCParticle* particleLepton1 = 0;
  const LHCb::MCParticle* particleLepton2 = 0;
  const LHCb::MCParticle* particleKaon = 0;
  const LHCb::MCParticle* particlePion = 0;

  int motherID  = particle->particleID().pid() ;
  if ( particle->hasOscillated() ) motherID = -motherID;

  LHCb::ParticleID id = particle->particleID() ;

  for ( LHCb::MCParticle::ConstVector::const_iterator ipart = descendants.begin() ;
        ipart != descendants.end() ; ++ipart ){
    
    id = (*ipart)->particleID();
    
    if ( id.isLepton() && (*ipart)->mother()->particleID().abspid() != 13 ){
      if (  motherID*id.pid() < 0 ) particleLepton1 = (*ipart);
      else particleLepton2 = (*ipart);
    }
    else if ( 321 == id.abspid() ) particleKaon = (*ipart); 
    else if ( 211 == id.abspid() ) particlePion = (*ipart);
  }

  if ( !particleLepton1 || !particleLepton2 || 
       !particleKaon || !particlePion ) {
    
    err() << "Could not find required particles !" << endmsg;
    return StatusCode::FAILURE ;
  }  
    
  
  thetak = m_angle->calculatePolarAngle( particle->momentum() , 
                                         particleKaon->momentum(),
                                         particlePion->momentum() );
  thetak = Gaudi::Units::pi -thetak;
  
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

double MCBd2KstarMuMuAngleCalculator::calculateThetaK( const LHCb::MCParticle* particle  )
{
  LHCb::MCParticle::ConstVector descendants;
  fillDescendants(particle, descendants , m_depth );
 
  const LHCb::MCParticle* particleKaon = 0;
  const LHCb::MCParticle* particlePion = 0;
  LHCb::ParticleID id;
  
  for ( LHCb::MCParticle::ConstVector::const_iterator idaughter = descendants.begin() ;
        idaughter != descendants.end(); ++idaughter ){
    
    id = (*idaughter)->particleID() ;
   
    if ( 321 == id.abspid() ) particleKaon = (*idaughter); 
    else if ( 211 == id.abspid() ) particlePion = (*idaughter);
  }
  
  if ( !particleKaon || !particlePion){
    Exception("Could not find required particles !");
  }
  
  double thetak = m_angle->calculatePolarAngle( particle->momentum() , 
                                         particleKaon->momentum(), 
                                         particlePion->momentum() );  

  thetak = Gaudi::Units::pi - thetak ;//flip angle due to definition 
  return thetak;
}

double MCBd2KstarMuMuAngleCalculator::calculatePhi( const LHCb::MCParticle* particle  )
{
  LHCb::MCParticle::ConstVector descendants;
  fillDescendants(particle, descendants , m_depth );

  const LHCb::MCParticle* particleLepton1 = 0;
  const LHCb::MCParticle* particleLepton2 = 0;
  const LHCb::MCParticle* particleKaon = 0;
  const LHCb::MCParticle* particlePion = 0;

  int motherID  = particle->particleID().pid() ;
  if ( particle->hasOscillated() ) motherID = -motherID;

  LHCb::ParticleID id;
  LHCb::MCParticle::ConstVector::const_iterator ipart;
  
  for ( ipart = descendants.begin(); ipart != descendants.end(); ++ipart ){
    
    id = (*ipart)->particleID();
    
    if ( id.isLepton() && (*ipart)->mother()->particleID().abspid() != 13 ){
      if (  motherID*id.pid() < 0 ) particleLepton1 = (*ipart);
      else particleLepton2 = (*ipart);
    }
    
    else if ( 321 == id.abspid() ) particleKaon = (*ipart); 
    else if ( 211 == id.abspid() ) particlePion = (*ipart);
  }

  if ( !particleLepton1 || !particleLepton2 || 
       !particleKaon || !particlePion ) {
    Exception("Could not find required particles !");
  }
  
  double phi    = m_angle->calculatePlaneAngle( particle->momentum(), 
                                         particleLepton1->momentum(), 
                                         particleLepton2->momentum(), 
                                         particleKaon->momentum(),
                                         particlePion->momentum() );
  return phi;
}


double MCBd2KstarMuMuAngleCalculator::calculateThetaL( const LHCb::MCParticle* particle )
{
  LHCb::MCParticle::ConstVector descendants;
  fillDescendants(particle, descendants ,2 );
  
  const LHCb::MCParticle* particleLepton1 = 0;
  const LHCb::MCParticle* particleLepton2 = 0;
  
  int motherID  = particle->particleID().pid() ;
  if ( particle->hasOscillated() ) motherID = -motherID;

  LHCb::ParticleID id;
  
  // loop over descendants and find leptons 
  for ( LHCb::MCParticle::ConstVector::const_iterator ipart = descendants.begin() ;
        ipart != descendants.end() ; ++ipart ){
    
    id = (*ipart)->particleID();
    if ( id.isLepton() && (*ipart)->mother()->particleID().abspid() != 13 ){
      if ( motherID*id.pid() < 0 ) particleLepton1 = (*ipart);
      else particleLepton2 = (*ipart);
    }
  }

  if ( !particleLepton1 || !particleLepton2 ) {
    warning() << "Could not find required particles !" << endmsg;
    return StatusCode::FAILURE ;
  }  

  double thetal = m_angle->calculatePolarAngle( particle->momentum(), 
                                         particleLepton1->momentum(),
                                         particleLepton2->momentum() ) ;

  return thetal;
}




double MCBd2KstarMuMuAngleCalculator::calculateMass( const LHCb::MCParticle* particle )
{  
  LHCb::MCParticle::ConstVector descendants ;
  fillDescendants( particle, descendants , 2 );  
  
  LHCb::ParticleID id ;

  const LHCb::MCParticle* leptonMinus = 0;
  const LHCb::MCParticle* leptonPlus = 0; 
  
  LHCb::MCParticle::ConstVector::const_iterator ipart;

  for ( ipart = descendants.begin() ; ipart != descendants.end() ; ++ipart ){  
    id = (*ipart)->particleID();
    if ( id.isLepton() && (*ipart)->mother()->particleID().abspid() != 13 ){
      if ( id.pid() > 0 ) leptonMinus = (*ipart) ;
      else leptonPlus = (*ipart) ; 
    } 
  }
  
  if ( !leptonMinus || !leptonPlus ) return StatusCode::FAILURE ;
  
  const Gaudi::LorentzVector leptonPair = ( (leptonPlus->momentum()) + 
                                            (leptonMinus->momentum()) );
  
  double mass = leptonPair.M(); 
  return mass ;
}
