// $Id: MCBd2KstarMuMuAngleCalculator.cpp,v 1.5 2010-04-29 17:38:54 tblake Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/ToolFactory.h" 
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "Event/MCParticle.h"
// local
#include "MCBd2KstarMuMuAngleCalculator.h"
#include "Kernel/DaVinciP2VVAngles.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MCBd2KstarMuMuAngleCalculator
//
// 2007-08-13 : Thomas Blake
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( MCBd2KstarMuMuAngleCalculator );

using namespace DaVinci::P2VVAngles;


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCBd2KstarMuMuAngleCalculator::MCBd2KstarMuMuAngleCalculator( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IP2VVMCPartAngleCalculator>(this);
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
  
  // use descendats method to get falttened list of daughters
  m_mcDecayFinder = tool<IMCDecayFinder>("MCDecayFinder",this);
  
  return sc ;
}


StatusCode MCBd2KstarMuMuAngleCalculator::daughters( const LHCb::MCParticle* mother )
{
  LHCb::MCParticle::ConstVector descendants;
  LHCb::MCParticle::ConstVector::const_iterator iter ;
  
  m_mcDecayFinder->descendants( mother, descendants ) ;
  
  m_pMuMinus = NULL;
  m_pMuPlus = NULL ;
  m_pK = NULL;
  m_pPi = NULL;

  if ( 4 > descendants.size() ) return StatusCode::FAILURE;
  
  for (iter = descendants.begin(); iter != descendants.end(); ++iter ){
    
     int pid = (*iter)->particleID().pid() ;
    int absid = (*iter)->particleID().abspid() ;

    if ( 13 == pid ) m_pMuMinus = (*iter);
    else if ( -13 == pid ) m_pMuPlus = (*iter);
    else if ( 321 == absid ) m_pK = (*iter);
    else if ( 211 == absid ) m_pPi = (*iter);
  }

  if ( NULL == m_pK || NULL == m_pMuPlus || 
       NULL == m_pMuMinus || NULL == m_pPi ){
    return StatusCode::FAILURE ;
  }
  
  return StatusCode::SUCCESS ;
}

StatusCode MCBd2KstarMuMuAngleCalculator::calculateAngles( 
   const LHCb::MCParticle* particle, double& thetal, 
   double& thetak, double& phi)
{
  StatusCode sc = daughters( particle ) ; 
  
  if ( sc.isFailure() ){
    error() << "Could not find required particles !" << endmsg;
    return sc; 
  }
  
  int motherID = particle->particleID().pid();
  if ( particle->hasOscillated() ) motherID = -motherID;
  bool isB0 = ( motherID > 0 );
  
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


StatusCode MCBd2KstarMuMuAngleCalculator::calculateTransversityAngles( 
   const LHCb::MCParticle* particle,
   double& Theta_tr, double& Phi_tr, double& Theta_V )
{

  StatusCode sc = daughters( particle ); 
    
  if ( sc.isFailure() ){
    error() << "Could not find required particles !" << endmsg;
    return sc; 
  }
  
  int motherID = particle->particleID().pid();
  if ( particle->hasOscillated() ) motherID = -motherID;
  bool isB0 = ( motherID > 0 );
  
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



double MCBd2KstarMuMuAngleCalculator::calculatePhi( const LHCb::MCParticle* particle )
{
  
  StatusCode sc = daughters( particle ) ; 
  
  if ( sc.isFailure() ){
     Exception("Could not find required particles !");
  }

  int motherID = particle->particleID().pid();
  if ( particle->hasOscillated() ) motherID = -motherID;
  bool isB0 = ( motherID > 0 );
  
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


double MCBd2KstarMuMuAngleCalculator::calculateThetaL( const LHCb::MCParticle* particle )
{
  
  StatusCode sc = daughters( particle ); 

  if ( sc.isFailure() ){
     Exception("Could not find required particles !");
  }
  
  int motherID = particle->particleID().pid();
  if ( particle->hasOscillated() ) motherID = -motherID;
  bool isB0 = ( motherID > 0 );
  
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


double MCBd2KstarMuMuAngleCalculator::calculateThetaK( const LHCb::MCParticle* particle ) 
{
  StatusCode sc = daughters( particle ); 
    
  if ( sc.isFailure() ){
     Exception("Could not find required particles !");
  }
  
  double theta = calculateHelicityPolarAngle( m_pK->momentum(), m_pPi->momentum(), 
                                              m_pMuPlus->momentum(), m_pMuMinus->momentum() );  
  return theta ;
}



double MCBd2KstarMuMuAngleCalculator::calculateTransThetaTr( 
   const LHCb::MCParticle* particle ) 
{
  StatusCode sc = daughters( particle );
  
  if ( sc.isFailure() ){
     Exception("Could not find required particles !");
  }

  int motherID = particle->particleID().pid();
  if ( particle->hasOscillated() ) motherID = -motherID;
  bool isB0 = ( motherID > 0 );
  
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

double MCBd2KstarMuMuAngleCalculator::calculateTransPhiTr( 
   const LHCb::MCParticle* particle ) 
{
  StatusCode sc = daughters( particle ) ;
    
  if ( sc.isFailure() ){
    Exception("Could not find required particles !");
  }

  int motherID = particle->particleID().pid();
  if ( particle->hasOscillated() ) motherID = -motherID;
  bool isB0 = ( motherID > 0 );

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

double MCBd2KstarMuMuAngleCalculator::calculateTransThetaV( const LHCb::MCParticle* particle ) 
{
  return calculateThetaK( particle ) ;
}

double  MCBd2KstarMuMuAngleCalculator::calculateMass( const LHCb::MCParticle* particle )
{
  StatusCode sc = daughters( particle ) ;
  
  if ( sc.isFailure() ){
    Exception("Could not find required particles !");
  }

  const Gaudi::LorentzVector& vecMuPlus = m_pMuPlus->momentum();
  const Gaudi::LorentzVector& vecMuMinus = m_pMuPlus->momentum();
  
  return ( vecMuPlus + vecMuMinus ).M();
}
