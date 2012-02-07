// Include files 

// local
#include "DaughtersInLHCbAndCutsForDstar.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/Transform4DTypes.h"

// from Kernel
#include "Kernel/ParticleID.h"
#include "GaudiKernel/Vector4DTypes.h"

// from HepMC
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"

// from Generators
#include "GenEvent/HepMCUtils.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DaughtersInLHCbAndCutsForDstar
//
// 2012-02-07 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( DaughtersInLHCbAndCutsForDstar );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DaughtersInLHCbAndCutsForDstar::DaughtersInLHCbAndCutsForDstar( const std::string& type,
                                                                const std::string& name,
                                                                const IInterface* parent )
  : GaudiTool ( type, name , parent ) {
  declareInterface< IGenCutTool >( this ) ;
  declareProperty( "ChargedThetaMin"   , m_chargedThetaMin   = 10 * Gaudi::Units::mrad ) ;
  declareProperty( "ChargedThetaMax"   , m_chargedThetaMax   = 400 * Gaudi::Units::mrad ) ;
  declareProperty( "NeutralThetaMin"   , m_neutralThetaMin   = 5 * Gaudi::Units::mrad ) ;
  declareProperty( "NeutralThetaMax"   , m_neutralThetaMax   = 400 * Gaudi::Units::mrad ) ;
  declareProperty( "D0PtCuts"          , m_d0ptCut           = 1700 * Gaudi::Units::MeV ) ;
  declareProperty( "DaughtersPtMinCut" , m_daughtersptminCut = 700 * Gaudi::Units::MeV ) ;
  declareProperty( "DaughtersPtMaxCut" , m_daughtersptmaxCut = 1050 * Gaudi::Units::MeV ) ;
  declareProperty( "DaughtersPMinCut"  , m_daughterspminCut  = 4500 * Gaudi::Units::MeV ) ;
  declareProperty( "SoftPiPtCut"       , m_softpiptCut       = 100 * Gaudi::Units::MeV ) ;
  declareProperty( "D0CTauCut"         , m_d0ctauCut         = -1. ) ;  
}

//=============================================================================
// Destructor 
//=============================================================================
DaughtersInLHCbAndCutsForDstar::~DaughtersInLHCbAndCutsForDstar( ) { ; }

//=============================================================================
// AndWithMinP function
//=============================================================================
bool DaughtersInLHCbAndCutsForDstar::applyCut( ParticleVector & theParticleVector ,
                                               const HepMC::GenEvent * /* theEvent */ ,
                                               const LHCb::GenCollision * /* theHardInfo */ )
  const {
  ParticleVector::iterator it ;
  
  for ( it = theParticleVector.begin() ; it != theParticleVector.end() ; ) {    
    // Check it is a D*+/-
    if ( abs( (*it) -> pdg_id() ) != 413 ) 
      Exception( "The signal is not a D*+ or D*-" ) ;
    
    if ( ! passCuts( *it ) ) {
      it = theParticleVector.erase( it ) ;
    } else ++it ;
  }

  return ( ! theParticleVector.empty() ) ;
}

//=============================================================================
// Functions to test if all daughters are in acceptance
//=============================================================================
bool DaughtersInLHCbAndCutsForDstar::passCuts( const HepMC::GenParticle * theSignal ) const {
  HepMC::GenVertex * EV = theSignal -> end_vertex() ;
  if ( 0 == EV ) return true ;

  typedef std::vector< HepMC::GenParticle * > Particles ;
  Particles stables ;
  HepMC::GenParticle * theSoftPion( 0 ) ;
  HepMC::GenParticle * theD0( 0 ) ;
  HepMC::GenParticle * theParent( 0 );
  Particles d0daughters ;
  
  HepMC::GenVertex::particle_iterator iter ;

  for ( iter = EV -> particles_begin( HepMC::descendants ) ; 
        iter != EV -> particles_end( HepMC::descendants ) ; ++iter ) {
    // Fill all daughters
    if ( 0 == (*iter) -> end_vertex() ) stables.push_back( *iter ) ;

    // The D0
    if ( abs( (*iter) -> pdg_id() ) == 421 ) theD0 = (*iter) ;

    // The soft pion
    theParent = *( (*iter) -> production_vertex() -> particles_in_const_begin() ) ;
    if ( ( 413 == abs( theParent -> pdg_id() ) ) && ( 211 == abs( (*iter) -> pdg_id() ) ) )
      theSoftPion = (*iter) ;
  }

  if ( 0 == theD0 )
    Exception( "No D0 in the signal decay chain !" ) ;

  if ( stables.empty() )
    Exception( "Signal has no stable daughters !" ) ;

  if ( 0 == theSoftPion ) 
    Exception( "No soft pion in the decay chain !" ) ;  

  // daughters of D0
  EV = theD0 -> end_vertex() ;
  if ( 0 == EV ) 
    Exception( "The D0 has no daughters" ) ;
  
  for ( iter = EV -> particles_begin( HepMC::descendants ) ; 
        iter != EV -> particles_end( HepMC::descendants ) ; ++iter ) {
    // Fill all daughters but exclude photons (can be radiative photons)
    if ( 0 == (*iter) -> end_vertex() )
      if ( 22 != (*iter) -> pdg_id() ) 
        d0daughters.push_back( *iter ) ;
  }

  if ( d0daughters.empty() )
    Exception( "No D0 daughters in signal chain !" );

  double angle( 0. ) ;
  double firstpz = stables.front() -> momentum().pz() ;

  debug() << "New event" << endmsg ;

  for ( Particles::const_iterator it = stables.begin() ; it != stables.end() ;
        ++it ) {

    debug() << "Check particle " << (*it) -> pdg_id() << " with angle " 
            << (*it) -> momentum().theta() / Gaudi::Units::mrad 
            << " mrad." << endmsg ;
   
    // Remove neutrinos
    if ( ( 12 == abs( (*it) -> pdg_id() ) ) || 
         ( 14 == abs( (*it) -> pdg_id() ) ) || 
         ( 16 == abs( (*it) -> pdg_id() ) ) ) continue ;
 
    // Don't use daughters of Lambda and KS:
    HepMC::GenParticle * theParent ;
    theParent = 
      *( (*it) -> production_vertex() -> particles_in_const_begin() ) ;
    if ( 3122 == abs( theParent -> pdg_id() ) ) continue ;
    if ( 310 == theParent -> pdg_id() ) continue ;

    // Consider only gammas from pi0 and eta
    if ( 22 == (*it) -> pdg_id() ) {
      if ( ( 111 != theParent -> pdg_id() ) &&
           ( 221 != theParent -> pdg_id() ) ) continue ;
    }

    // All particles in same direction
    if ( 0 > ( firstpz * ( (*it) -> momentum().pz() ) ) ) return false ;

    angle = (*it) -> momentum().theta() ;
    
    LHCb::ParticleID pid( (*it) -> pdg_id() ) ;
    if ( 0 == pid.threeCharge() ) {
      if ( fabs( sin( angle ) ) > fabs( sin( m_neutralThetaMax ) ) ) 
        return false ;
      if ( fabs( sin( angle ) ) < fabs( sin( m_neutralThetaMin ) ) ) 
        return false ;
    } else {
      if ( fabs( sin( angle ) ) > fabs( sin( m_chargedThetaMax ) ) ) 
        return false ;
      if ( fabs( sin( angle ) ) < fabs( sin( m_chargedThetaMin ) ) ) 
        return false ;
    }
  }

  // Now check other cuts
  debug() << "Check other cuts" << endmsg ;

  debug() << "D0 pT = " << theD0 -> momentum().perp() << endmsg ;
  if ( theD0 -> momentum().perp() < m_d0ptCut ) return false ;
  
  if ( m_d0ctauCut > 0. ) {
    // Apply ctau cut
    Gaudi::LorentzVector pEnd , pBegin ;
    pEnd.SetXYZT( theD0 -> end_vertex() -> position() . x() ,
                  theD0 -> end_vertex() -> position() . y() , 
                  theD0 -> end_vertex() -> position() . z() ,
                  theD0 -> end_vertex() -> position() . t() * Gaudi::Units::c_light ) ;

    pBegin.SetXYZT( theD0 -> production_vertex() -> position() . x() ,
                    theD0 -> production_vertex() -> position() . y() ,
                    theD0 -> production_vertex() -> position() . z() ,
                    theD0 -> production_vertex() -> position() . t() * 
                    Gaudi::Units::c_light ) ;  

    Gaudi::LorentzVector disp = pEnd - pBegin ;
    
    // Boost transformation
    Gaudi::LorentzVector fourM( theD0 -> momentum() ) ;
    ROOT::Math::Boost theBoost( fourM.BoostToCM() ) ;
    Gaudi::LorentzVector restDisp = theBoost( disp ) ;
    double ctau = restDisp.t() ;

    debug() << "D0 ctau = " << ctau << endmsg ;
    if ( ctau < m_d0ctauCut ) return false ;
  }

  debug() << "Soft pion pT = " << theSoftPion -> momentum().perp() << endmsg ;
  if ( theSoftPion -> momentum().perp() < m_softpiptCut ) return false ;
  
  double minpt = 14.*Gaudi::Units::TeV ;
  double maxpt = 0. ;
  double minp  = 14.*Gaudi::Units::TeV ;
  
  for ( Particles::const_iterator it = d0daughters.begin() ; it != d0daughters.end() ;
        ++it ) {  
    debug() << "Daughter pT = " << (*it) -> momentum().perp() 
            << " p = " << (*it) -> momentum().rho() << endmsg ;
    if ( (*it) -> momentum().perp() > maxpt ) 
      maxpt = (*it) -> momentum().perp() ;
    if ( (*it) -> momentum().perp() < minpt ) 
      minpt = (*it) -> momentum().perp() ;
    if ( (*it) -> momentum().rho() < minp ) 
      minp = (*it) -> momentum().rho() ;
  }
  
  debug() << "Min Pt = " << minpt
          << " Max Pt = " << maxpt 
          << " Min P = " << minp << endmsg ;

  if ( minpt < m_daughtersptminCut ) return false ;
  if ( maxpt < m_daughtersptmaxCut ) return false ;
  if ( minp  < m_daughterspminCut  ) return false ;

  debug() << "Event passed !" << endmsg ;
  
  return true ;
}

