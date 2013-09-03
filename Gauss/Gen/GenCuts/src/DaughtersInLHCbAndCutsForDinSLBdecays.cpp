// local
#include "DaughtersInLHCbAndCutsForDinSLBdecays.h"
#include <iostream>

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
// Implementation file for class : DaughtersInLHCbAndCutsForDinSLBdecays
//
// 2013-08-28 : Maurizio Martinelli
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( DaughtersInLHCbAndCutsForDinSLBdecays );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DaughtersInLHCbAndCutsForDinSLBdecays::DaughtersInLHCbAndCutsForDinSLBdecays( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : GaudiTool ( type, name , parent ) {
    declareInterface< IGenCutTool >( this ) ;
    declareProperty( "ChargedThetaMin" , m_chargedThetaMin = 10 * Gaudi::Units::mrad ) ;
    declareProperty( "ChargedThetaMax" , m_chargedThetaMax = 400 * Gaudi::Units::mrad ) ;
    declareProperty( "NeutralThetaMin" , m_neutralThetaMin = 5 * Gaudi::Units::mrad ) ;
    declareProperty( "NeutralThetaMax" , m_neutralThetaMax = 400 * Gaudi::Units::mrad ) ;
    declareProperty( "MassMin" , m_md0muMin = 0*Gaudi::Units::MeV ) ;
    declareProperty( "D0DaughtersPMin", m_daughtersPMin = 0 * Gaudi::Units::MeV );
    declareProperty( "D0DaughtersPtMin", m_daughtersPtMin = 0 * Gaudi::Units::MeV );
    declareProperty( "MuonPMin", m_muonPMin = 0 * Gaudi::Units::MeV );
    declareProperty( "MuonPtMin", m_muonPtMin = 0 * Gaudi::Units::MeV );
    declareProperty( "D0PtMin", m_d0PtMin = 0 * Gaudi::Units::MeV );
    declareProperty( "D0PropTimeMin", m_d0ctauMin = 0 * Gaudi::Units::mm );
}

//=============================================================================
// Destructor 
//=============================================================================
DaughtersInLHCbAndCutsForDinSLBdecays::~DaughtersInLHCbAndCutsForDinSLBdecays( ) { ; }

//=============================================================================
// Cut function
//=============================================================================
bool DaughtersInLHCbAndCutsForDinSLBdecays::applyCut( ParticleVector & theParticleVector ,
						      const HepMC::GenEvent * /* theEvent */ ,
						      const LHCb::GenCollision * /* theHardInfo */ )
  const {
  ParticleVector::iterator it ;
  
  for ( it = theParticleVector.begin() ; it != theParticleVector.end() ; ) {    
    // Check it is a B+/-
    if ( abs( (*it) -> pdg_id() ) != 521 )  {
      debug() << "Particle ID: " << (*it) -> pdg_id() << endmsg;
      Exception( "The signal is not a B+ or B-" ) ;
    }
    
    if ( ! passCuts( *it ) ) {
      it = theParticleVector.erase( it ) ;
    } else ++it ;
  }

  return ( ! theParticleVector.empty() ) ;
}

//=============================================================================
// Apply cuts Function
//=============================================================================
bool DaughtersInLHCbAndCutsForDinSLBdecays::passCuts( const HepMC::GenParticle * theSignal ) const {

  HepMC::GenVertex * EV = theSignal -> end_vertex() ;
  if ( 0 == EV ) return true ;

  typedef std::vector< HepMC::GenParticle * > Particles ;
  Particles stables ;
  HepMC::GenParticle * theMuon( 0 ) ;
  HepMC::GenParticle * theD0( 0 ) ;
  HepMC::GenParticle * theNeutrino( 0 ) ;
  Particles d0daughters ;

  HepMC::GenVertex::particle_iterator iter ;

  for ( iter = EV -> particles_begin( HepMC::descendants ) ; 
        iter != EV -> particles_end( HepMC::descendants ) ; ++iter ) {
    // Fill all daughters
    if ( 0 == (*iter) -> end_vertex() ) stables.push_back( *iter ) ;

    // The D0
    if ( abs( (*iter) -> pdg_id() ) == 421 ) theD0 = (*iter) ;

    // The muon
    if ( abs( (*iter) -> pdg_id() ) == 13 ) theMuon = (*iter) ;

    // The neutrino
    if ( abs( (*iter) -> pdg_id() ) == 14 ) theNeutrino = (*iter) ;
  }

  // check D0, mu, and nu are found
  if ( 0 == theD0 )
    Exception( "No D0 in the signal decay chain !" ) ;

  if ( stables.empty() )
    Exception( "Signal has no stable daughters !" ) ;

  if ( 0 == theMuon ) 
    Exception( "No muon in the decay chain !" ) ;  

  if ( 0 == theNeutrino ) 
    Exception( "No neutrino in the decay chain !" ) ;
  if( !(theD0 && theMuon && theNeutrino)) return false;

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
  
  debug() << "New event" << endmsg ;

  // ---------------------------------------------------------
  // Check all stable particles from decay chain in Acceptance
  //
  double firstpz = stables.front() -> momentum().pz() ;
  for ( Particles::const_iterator it = stables.begin() ; 
	it != stables.end() ; ++it )
    if (! InLHCbAcceptance( *it, firstpz ) ) return false;

  // ---------------------------------------------------------
  // Check (D0mu) invariant mass
  //
  debug() << "M(D0mu) = " << ( (Gaudi::LorentzVector)(theD0 -> momentum()) + (Gaudi::LorentzVector)(theMuon -> momentum())).mass() << endmsg ;
  double mD0mu = ((Gaudi::LorentzVector)(theD0->momentum()) + (Gaudi::LorentzVector)(theMuon->momentum())).mass();
  if (mD0mu < m_md0muMin) return false;

  // ---------------------------------------------------------
  // check D0 transverse momentum
  //
  debug() << "D0 pT = " << theD0 -> momentum().perp() << endmsg ;
  if ( theD0 -> momentum().perp() < m_d0PtMin ) return false ;
  
  // ---------------------------------------------------------
  // check D0 proper time
  //
  if ( m_d0ctauMin > 0. ) {
    // Apply ctau cut
    double ctau = ProperTime(theD0);
    if ( ctau < m_d0ctauMin ) return false ;
  }

  // ---------------------------------------------------------
  // check muon momentum
  //
  debug() << "Muon p = " << theMuon -> momentum().rho() << endmsg ;
  if ( theMuon -> momentum().rho() < m_muonPMin ) return false ;

  // ---------------------------------------------------------
  // check muon transverse momentum
  //
  debug() << "Muon pT = " << theMuon -> momentum().perp() << endmsg ;
  if ( theMuon -> momentum().perp() < m_muonPtMin ) return false ;

  // ---------------------------------------------------------
  // check D0 daughters transverse momentum and momentum
  //
  for ( Particles::const_iterator it = d0daughters.begin() ; it != d0daughters.end() ;
        ++it ) {  
    debug() << "Daughter pT = " << (*it) -> momentum().perp() 
            << " p = " << (*it) -> momentum().rho() << endmsg ;
    if ( (*it) -> momentum().perp() < m_daughtersPtMin ) return false;
    if ( (*it) -> momentum().rho() < m_daughtersPMin ) return false;
  }

  return true;

}

bool 
DaughtersInLHCbAndCutsForDinSLBdecays::InLHCbAcceptance( const HepMC::GenParticle * stable, const double firstpz ) const
{
  
  debug() << "Check particle " << stable -> pdg_id() << " with angle " 
            << stable -> momentum().theta() / Gaudi::Units::mrad 
            << " mrad." << endmsg ;
   
  // Don't care about neutrinos
  if ( ( 12 == abs( stable -> pdg_id() ) ) || 
       ( 14 == abs( stable -> pdg_id() ) ) || 
       ( 16 == abs( stable -> pdg_id() ) ) ) return true ;
 
  // Don't use daughters of Lambda and KS:
  HepMC::GenParticle * theParent = *( stable -> production_vertex() -> particles_in_const_begin() ) ;
  if ( 3122 == abs( theParent -> pdg_id() ) ||
       310 == theParent -> pdg_id() ) return true ;

  // Consider only gammas from pi0 and eta
  if ( 22 == stable -> pdg_id() ) {
    if ( ( 111 != theParent -> pdg_id() ) &&
	 ( 221 != theParent -> pdg_id() ) ) return true ;
  }

  // All particles in same direction
  if ( 0 > ( firstpz * ( stable -> momentum().pz() ) ) ) return false ;

  // Check angle
  double angle = stable -> momentum().theta() ;
  LHCb::ParticleID pid( stable -> pdg_id() ) ;
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
  
  // If ok
  return true;
}

double 
DaughtersInLHCbAndCutsForDinSLBdecays::ProperTime( const HepMC::GenParticle * resonance ) const
{
  Gaudi::LorentzVector pEnd , pBegin ;
  pEnd.SetXYZT( resonance -> end_vertex() -> position() . x() ,
                resonance -> end_vertex() -> position() . y() , 
                resonance -> end_vertex() -> position() . z() ,
                resonance -> end_vertex() -> position() . t() * Gaudi::Units::c_light ) ;

  pBegin.SetXYZT( resonance -> production_vertex() -> position() . x() ,
                  resonance -> production_vertex() -> position() . y() ,
                  resonance -> production_vertex() -> position() . z() ,
                  resonance -> production_vertex() -> position() . t() * 
		  Gaudi::Units::c_light ) ;  

  Gaudi::LorentzVector disp = pEnd - pBegin ;
    
  // Boost transformation
  Gaudi::LorentzVector fourM( resonance -> momentum() ) ;
  ROOT::Math::Boost theBoost( fourM.BoostToCM() ) ;
  Gaudi::LorentzVector restDisp = theBoost( disp ) ;
  double ctau = restDisp.t() ;

  debug() << "D0 ctau = " << ctau << endmsg ;
  return ctau ;
}
