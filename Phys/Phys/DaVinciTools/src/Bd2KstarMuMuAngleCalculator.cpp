// $Id$
// Include files 
// from Gaudi

#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "Event/Particle.h" 
#include "Kernel/IParticleDescendants.h"


// local
#include "Bd2KstarMuMuAngleCalculator.h"
#include "Kernel/DaVinciP2VVAngles.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Bd2KstarMuMuAngleCalculator
//
// 2007-08-02 : Thomas Blake
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( Bd2KstarMuMuAngleCalculator )

using namespace DaVinci::P2VVAngles;

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


StatusCode Bd2KstarMuMuAngleCalculator::daughters( const LHCb::Particle* mother ){

  m_pMuMinus = NULL ;
  m_pMuPlus = NULL ;
  m_pK = NULL ;
  m_pPi = NULL ;

  LHCb::Particle::ConstVector descendants = m_descendants->finalStates( mother ) ; 
  LHCb::Particle::ConstVector::const_iterator iter;
  
    
  if ( 4 > descendants.size() ) return StatusCode::FAILURE;
  
  for ( iter = descendants.begin(); iter != descendants.end(); ++iter ){
    int pid = (*iter)->particleID().pid() ;
    int absid = (*iter)->particleID().abspid() ;

    if ( 13 == pid || 11 == pid ) m_pMuMinus = (*iter);
    else if ( -13 == pid || -11 == pid ) m_pMuPlus = (*iter);
    else if ( 321 == absid ) m_pK = (*iter);
    else if ( 211 == absid ) m_pPi = (*iter);
  }

  if ( NULL == m_pK || NULL == m_pMuPlus || 
       NULL == m_pMuMinus || NULL == m_pPi ){
    return StatusCode::FAILURE ;
  }
  
  return StatusCode::SUCCESS ;
}


StatusCode Bd2KstarMuMuAngleCalculator::initialize() {
  
  StatusCode sc = GaudiTool::initialize() ;
  if (!sc) return sc ;
  
  if (msgLevel(MSG::DEBUG)) debug() << "Initializing Angle Calculator Tool" << endmsg ;
  
  m_descendants = tool<IParticleDescendants>("ParticleDescendants", this);
  
  return sc ;
}



StatusCode Bd2KstarMuMuAngleCalculator::calculateAngles( 
   const LHCb::Particle* particle, double& thetal, 
   double& thetak, double& phi)
{
  StatusCode sc = daughters( particle ) ; 
  
  if ( sc.isFailure() ){
    error() << "Could not find required particles !" << endmsg;
    return sc; 
  }
  
  bool isB0 = (particle->particleID().pid() > 0);

  
  if ( isB0 ) {
      thetal = calculateHelicityPolarAngle( m_pK->momentum(), m_pPi->momentum(),
                                          m_pMuPlus->momentum(), m_pMuMinus->momentum() );  
    
    thetak = calculateHelicityPolarAngle( m_pMuPlus->momentum(), m_pMuMinus->momentum(),
                                          m_pK->momentum(), m_pPi->momentum() );
  
    phi = calculatePlaneAngle( m_pMuPlus->momentum(), m_pMuMinus->momentum(),
                               m_pK->momentum(), m_pPi->momentum() ); 
  } 
  else {
    thetal = calculateHelicityPolarAngle( m_pK->momentum(), m_pPi->momentum(), 
                                          m_pMuMinus->momentum(), m_pMuPlus->momentum() );
    
    thetak = calculateHelicityPolarAngle( m_pMuMinus->momentum(), m_pMuPlus->momentum(),
                                          m_pK->momentum(), m_pPi->momentum() );
    
    phi = calculatePlaneAngle( m_pMuMinus->momentum(), m_pMuPlus->momentum(),
                               m_pK->momentum(), m_pPi->momentum() );  
  }
  
  return sc ;
}


StatusCode Bd2KstarMuMuAngleCalculator::calculateTransversityAngles( 
   const LHCb::Particle* particle,
   double& Theta_tr, double& Phi_tr, double& Theta_V )
{

  StatusCode sc = daughters( particle ); 
    
  if ( sc.isFailure() ){
    error() << "Could not find required particles !" << endmsg;
    return sc; 
  }
 
  bool isB0 = (particle->particleID().pid() > 0);
  if ( isB0 ){
    Theta_V =  calculateHelicityPolarAngle( m_pMuPlus->momentum(), m_pMuMinus->momentum(), 
                                            m_pK->momentum(), m_pPi->momentum() );  
  
    Theta_tr = calculateThetaTr( m_pMuPlus->momentum(), m_pMuMinus->momentum(), 
                                 m_pK->momentum(), m_pPi->momentum() );
                                 
    
    Phi_tr = calculatePhiTr( m_pMuPlus->momentum(), m_pMuMinus->momentum(), 
                             m_pK->momentum(), m_pPi->momentum() ) ;  
  }
  else {
    
    Theta_V =  calculateHelicityPolarAngle( m_pMuMinus->momentum(), m_pMuPlus->momentum(),
                                            m_pK->momentum(), m_pPi->momentum() );  
    
    Theta_tr = calculateThetaTr( m_pMuMinus->momentum(), m_pMuPlus->momentum(),
                                 m_pK->momentum(), m_pPi->momentum() );
    
    
    Phi_tr = calculatePhiTr( m_pMuMinus->momentum(), m_pMuPlus->momentum(),
                             m_pK->momentum(), m_pPi->momentum() ) ;  
  }
  
  return StatusCode::SUCCESS ;
}



double Bd2KstarMuMuAngleCalculator::calculatePhi( const LHCb::Particle* particle )
{
  
  StatusCode sc = daughters( particle ) ; 
  
  if ( sc.isFailure() ){
     Exception("Could not find required particles !");
  }

  bool isB0 = (particle->particleID().pid() > 0);
  
  double  phi = -999;

  if ( isB0 ){
    phi = calculatePlaneAngle( m_pMuPlus->momentum(), m_pMuMinus->momentum(),
                               m_pK->momentum(), m_pPi->momentum() );  
  }
  else {
    phi = calculatePlaneAngle( m_pMuMinus->momentum(), m_pMuPlus->momentum(),
                               m_pK->momentum(), m_pPi->momentum() );
  }

  return phi ;
}


// as far as here

double Bd2KstarMuMuAngleCalculator::calculateThetaL( const LHCb::Particle* particle )
{
  
  StatusCode sc = daughters( particle ); 

  if ( sc.isFailure() ){
     Exception("Could not find required particles !");
  }
  
  bool isB0 = (particle->particleID().pid() > 0);
  
  double theta = -999;
  
  if ( isB0 ){
    theta = calculateHelicityPolarAngle( m_pK->momentum(), m_pPi->momentum(), 
                                         m_pMuPlus->momentum(), m_pMuMinus->momentum() );  
  }
  else {
    
    theta = calculateHelicityPolarAngle( m_pK->momentum(), m_pPi->momentum(), 
                                         m_pMuMinus->momentum(), m_pMuPlus->momentum() );  
  }
  return theta ;
}


double Bd2KstarMuMuAngleCalculator::calculateThetaK( const LHCb::Particle* particle ) 
{
  StatusCode sc = daughters( particle ); 
    
  if ( sc.isFailure() ){
     Exception("Could not find required particles !");
  }
  
  double theta = calculateHelicityPolarAngle( m_pK->momentum(), m_pPi->momentum(), 
                                              m_pMuPlus->momentum(), m_pMuMinus->momentum() );  
  return theta ;
}



double Bd2KstarMuMuAngleCalculator::calculateTransThetaTr( 
   const LHCb::Particle* particle ) 
{
  StatusCode sc = daughters( particle );
  
  if ( sc.isFailure() ){
     Exception("Could not find required particles !");
  }

  bool isB0 = (particle->particleID().pid() > 0);
  double Theta_tr = -999.;
  
  if ( isB0 ){
    Theta_tr = calculateThetaTr( m_pMuPlus->momentum(), m_pMuMinus->momentum(),
                                 m_pK->momentum(), m_pPi->momentum() );
  }
  else {
    Theta_tr = calculateThetaTr( m_pMuMinus->momentum(), m_pMuPlus->momentum(),
                                 m_pK->momentum(), m_pPi->momentum() );  
  }  
  
  return Theta_tr;   
}

double Bd2KstarMuMuAngleCalculator::calculateTransPhiTr( 
   const LHCb::Particle* particle ) 
{
  StatusCode sc = daughters( particle ) ;
    
  if ( sc.isFailure() ){
    Exception("Could not find required particles !");
  }
  
  bool isB0 = (particle->particleID().pid() > 0);
  double Phi_tr = -999.;
  
  if ( isB0 ){
    Phi_tr = calculatePhiTr( m_pMuPlus->momentum(), m_pMuMinus->momentum(),
                             m_pK->momentum(), m_pPi->momentum() );
  }
  else {
    Phi_tr = calculatePhiTr( m_pMuMinus->momentum(), m_pMuPlus->momentum(),
                             m_pK->momentum(), m_pPi->momentum() );
  }
  
  return Phi_tr;
}

double Bd2KstarMuMuAngleCalculator::calculateTransThetaV( const LHCb::Particle* particle ) 
{
  return calculateThetaK( particle ) ;
}

