// $Id: MCBd2KstarMuMuAngleCalculator.cpp,v 1.5 2010-04-29 17:38:54 tblake Exp $
// Include files

// from Gaudi
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "Kernel/ParticleProperty.h"

#include "Event/MCParticle.h"
// local
#include "MCBd2KstarMuMuAngleCalculator.h"
#include "Kernel/DaVinciP2VVAngles.h"

#include <boost/assign/list_of.hpp>


//-----------------------------------------------------------------------------
// Implementation file for class : MCBd2KstarMuMuAngleCalculator
//
// 2007-08-13 : Thomas Blake
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( MCBd2KstarMuMuAngleCalculator )

  using namespace DaVinci::P2VVAngles;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCBd2KstarMuMuAngleCalculator::MCBd2KstarMuMuAngleCalculator( const std::string& type,
                                                              const std::string& name,
                                                              const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_KPiOrigin ( 1, "K*(892)0" )
{
  declareInterface<IP2VVMCPartAngleCalculator>(this);
  declareProperty("KaonPionOrigin",  m_KPiOrigin );
  const std::vector<std::string> tmp = boost::assign::list_of("J/psi(1S)")("psi(2S)")("B0")("B_s0");
  declareProperty("MuonOrigin", m_MuOrigin = tmp );
}

//=============================================================================
// Destructor
//=============================================================================

MCBd2KstarMuMuAngleCalculator::~MCBd2KstarMuMuAngleCalculator() {}

//=============================================================================

StatusCode MCBd2KstarMuMuAngleCalculator::initialize()
{

  StatusCode sc = GaudiTool::initialize() ;
  if (!sc) return sc ;

  info() << "Initializing Angle Calculator Tool" << endmsg ;

  m_mcDecayFinder = tool<IMCDecayFinder>("MCDecayFinder",this);
  m_ppSvc = svc<LHCb::IParticlePropertySvc>( "LHCb::ParticlePropertySvc", true );

  std::vector< std::string >::const_iterator istr;

  for ( istr = m_KPiOrigin.begin(); istr != m_KPiOrigin.end(); ++istr ){
    const LHCb::ParticleProperty* pp = m_ppSvc->find( *istr );
    if ( NULL != pp ) m_KPiOriginID.push_back( pp->particleID().abspid() );
  }

  for ( istr = m_MuOrigin.begin(); istr != m_MuOrigin.end(); ++istr ){
    const LHCb::ParticleProperty* pp = m_ppSvc->find( *istr );
    if ( NULL != pp ) m_MuOriginID.push_back( pp->particleID().abspid() );
  }

  return sc ;
}


bool MCBd2KstarMuMuAngleCalculator::hasMother( const LHCb::MCParticle* particle,
                                               const std::vector< unsigned int >& ids ) const
{
  const LHCb::MCParticle* mother = particle->mother();

  if ( NULL == mother ) return false ;
  std::vector< unsigned int >::const_iterator it;

  for ( it = ids.begin(); it != ids.end(); ++it ){
    if ( (*it) == (mother->particleID().abspid()) ) return true;
  }

  return false;
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

  for (iter = descendants.begin(); iter != descendants.end(); ++iter ){

    int pid   = (*iter)->particleID().pid() ;
    int absid = (*iter)->particleID().abspid() ;

    if ( 13 == pid || 11 == pid ) {
      if ( hasMother(*iter, m_MuOriginID) ) m_pMuMinus = (*iter);
    }
    else if ( -13 == pid || -11 == pid ) {
      if ( hasMother(*iter, m_MuOriginID) ) m_pMuPlus = (*iter);
    }
    else if ( 321 == absid ) {
      if ( hasMother(*iter, m_KPiOriginID) ) m_pK = (*iter);
    }
    else if ( 211 == absid ) {
      if ( hasMother(*iter, m_KPiOriginID) ) m_pPi = (*iter);
    }
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
