// Include files 

// local
#include "DaughtersInLHCbAndCutsForDACP.h"

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
// Implementation file for class : DaughtersInLHCbAndCutsForDACP
//
// 2012-05-14 : Chris Thomas, based on DaughtersInLHCbAndCutsForDstar
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( DaughtersInLHCbAndCutsForDACP );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DaughtersInLHCbAndCutsForDACP::DaughtersInLHCbAndCutsForDACP( const std::string& type,
                                                              const std::string& name,
                                                              const IInterface* parent )
  : GaudiTool ( type, name , parent ) {
  declareInterface< IGenCutTool >( this ) ;
  declareProperty( "ChargedThetaMin"   , m_chargedThetaMin   = 10 * Gaudi::Units::mrad ) ;
  declareProperty( "ChargedThetaMax"   , m_chargedThetaMax   = 400 * Gaudi::Units::mrad ) ;
  declareProperty( "NeutralThetaMin"   , m_neutralThetaMin   = 5 * Gaudi::Units::mrad ) ;
  declareProperty( "NeutralThetaMax"   , m_neutralThetaMax   = 400 * Gaudi::Units::mrad ) ;
  //
  declareProperty( "ScaleDst"          , m_scaleDst          = 0.1 ) ; //scaling between p and pT ranges
  declareProperty( "uDst"              , m_uDst              = 6000.0 * Gaudi::Units::MeV ) ; //x-coordinate of parabola base
  declareProperty( "vDst"              , m_vDst              = 60000.0 * Gaudi::Units::MeV ) ; //y-coordinate of parabola base
  //angle of rotation of parabola from vertical = thetaDst = 37.0 degrees
  declareProperty( "costhetaDst"       , m_costhetaDst       = 0.798636 ) ;
  declareProperty( "sinthetaDst"       , m_sinthetaDst       = 0.601815 ) ;
  declareProperty( "kDst"              , m_kDst              = 0.00009 ) ; //roughly proportional to inverse width of parabbola
  //
  declareProperty( "ScaleDiff"         , m_scaleDiff         = 0.1 ) ; //scaling between x and y axes
  declareProperty( "uDiff"             , m_uDiff             = 400.0 * Gaudi::Units::MeV ) ; //x-coordinate of parabola base
  declareProperty( "vDiff"             , m_vDiff             = 4500.0 * Gaudi::Units::MeV ) ; //y-coordinate of parabola base
  //angle of rotation of parabola from vertical = thetaDiff = 38.0 degrees
  declareProperty( "costhetaDiff"      , m_costhetaDiff      = 0.788011 ) ;
  declareProperty( "sinthetaDiff"      , m_sinthetaDiff      = 0.615661 ) ;
  declareProperty( "kDiff"             , m_kDiff             = 0.0014 ) ; //roughly proportional to inverse width of parabola
  //  
  declareProperty( "D0_TAU"            , m_D0_TAU            = 0.0005 * Gaudi::Units::ns ) ;
  declareProperty( "D0_FD_ORIVX"       , m_D0_FD_ORIVX       = 5.0 * Gaudi::Units::mm ) ;
  declareProperty( "D0_daug_HiPT"      , m_D0_daugHiPT       = 3000.0 * Gaudi::Units::MeV ) ;
  declareProperty( "D0_daug_LoPT"      , m_D0_daugLoPT       = 1000.0 * Gaudi::Units::MeV ) ;
  declareProperty( "D0_daug_LoP"       , m_D0_daugLoP        = 5000.0 * Gaudi::Units::MeV ) ;
}

//=============================================================================
// Destructor 
//=============================================================================
DaughtersInLHCbAndCutsForDACP::~DaughtersInLHCbAndCutsForDACP( ) { ; }

//=============================================================================
// AndWithMinP function
//=============================================================================
bool DaughtersInLHCbAndCutsForDACP::applyCut( ParticleVector & theParticleVector ,
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
bool DaughtersInLHCbAndCutsForDACP::passCuts( const HepMC::GenParticle * theSignal ) const {
  HepMC::GenVertex * EV = theSignal -> end_vertex() ;
  if ( 0 == EV ) return true ;

  typedef std::vector< HepMC::GenParticle * > Particles ;
  Particles stables ;
  HepMC::GenParticle * theSoftPion( 0 ) ;
  HepMC::GenParticle * theD0( 0 ) ;
  HepMC::GenParticle * theDst( 0 ) ; //specifically want a Dst instead of just a "parent"
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

    // The Dst
    if ( 413 == abs( theParent -> pdg_id() ) )
      theDst = theParent ;
    
  }

  if ( 0 == theDst )
    Exception( "No Dst in the signal decay chain !" ) ;

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
  // Hopefully ordered so most time-expensive checks are performed at the end
  debug() << "Check other cuts" << endmsg ;

  //
  //Cut on D0 flight distance from origin vertex
  //- compare squared quantities to save time
  //- use of pEnd and pBegin (see below) somehow ends up with the wrong units
  //
  
  double FD_ORIVXsq = ( (theD0->end_vertex()->position().x() - theD0->production_vertex()->position().x()) *
                        (theD0->end_vertex()->position().x() - theD0->production_vertex()->position().x()) +
                        (theD0->end_vertex()->position().y() - theD0->production_vertex()->position().y()) *
                        (theD0->end_vertex()->position().y() - theD0->production_vertex()->position().y()) +
                        (theD0->end_vertex()->position().z() - theD0->production_vertex()->position().z()) *
                        (theD0->end_vertex()->position().z() - theD0->production_vertex()->position().z()) );

  debug() << "(D0 FD)^2 = " << FD_ORIVXsq << endmsg ;
  debug() << "(D0 FD cut)^2 = " << (m_D0_FD_ORIVX * m_D0_FD_ORIVX) << endmsg ;
  if ( FD_ORIVXsq < (m_D0_FD_ORIVX * m_D0_FD_ORIVX) ) return false ;

  //
  //Cuts on D0 daughter maximum pT, minimum pT and minimum p
  //- based on DaughtersInLHCbAndCutsForDstar
  //

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

  debug() << "Min Pt = "  << minpt
         << " Max Pt = " << maxpt 
         << " Min P = "  << minp << endmsg ;

  if ( maxpt < m_D0_daugHiPT ) return false ;
  if ( minpt < m_D0_daugLoPT ) return false ;
  if ( minp  < m_D0_daugLoP  ) return false ;
  
  //
  //Cut on (Dst p, Dst pT) plane
  //

  double Dstp = theDst -> momentum().rho() ;
  double DstpT = theDst -> momentum().perp() ;
  
  debug() << "Dst p = " << Dstp << endmsg ;
  debug() << "Dst pT = " << DstpT << endmsg ;

  debug() << "LHS = " << ((DstpT - m_uDst) * m_sinthetaDst + m_scaleDst * (Dstp - m_vDst) * m_costhetaDst) << endmsg;
  debug() << "RHS = " << (m_kDst *
                          ((DstpT - m_uDst) * m_costhetaDst - m_scaleDst * (Dstp - m_vDst) * m_sinthetaDst) *
                          ((DstpT - m_uDst) * m_costhetaDst - m_scaleDst * (Dstp - m_vDst) * m_sinthetaDst)) << endmsg;
  
  if ( ((DstpT - m_uDst) * m_sinthetaDst + m_scaleDst * (Dstp - m_vDst) * m_costhetaDst) <
       (m_kDst *
        ((DstpT - m_uDst) * m_costhetaDst - m_scaleDst * (Dstp - m_vDst) * m_sinthetaDst) *
        ((DstpT - m_uDst) * m_costhetaDst - m_scaleDst * (Dstp - m_vDst) * m_sinthetaDst)) )
    return false ;

  //
  //Cut on (Dst p - D0 p, Dst pT - D0 pT) plane
  //

  double D0p = theD0 -> momentum().rho() ;
  double D0pT = theD0 -> momentum().perp() ;

  debug() << "D0 p = " << D0p << endmsg ;
  debug() << "D0 pT = " << D0pT << endmsg ;
  
  double Diffp = Dstp - D0p ;
  double DiffpT = DstpT - D0pT ;

  debug() << "Diff p = " << Diffp << endmsg ;
  debug() << "Diff pT = " << DiffpT << endmsg ;

  debug() << "LHS = " << ((DiffpT - m_uDiff) * m_sinthetaDiff + m_scaleDiff * (Diffp - m_vDiff) * m_costhetaDiff) << endmsg;
  debug() << "RHS = " << (m_kDiff *
                          ((DiffpT - m_uDiff) * m_costhetaDiff - m_scaleDiff * (Diffp - m_vDiff) * m_sinthetaDiff) *
                          ((DiffpT - m_uDiff) * m_costhetaDiff - m_scaleDiff * (Diffp - m_vDiff) * m_sinthetaDiff)) << endmsg;
  
  if ( ((DiffpT - m_uDiff) * m_sinthetaDiff + m_scaleDiff * (Diffp - m_vDiff) * m_costhetaDiff) <
       (m_kDiff *
        ((DiffpT - m_uDiff) * m_costhetaDiff - m_scaleDiff * (Diffp - m_vDiff) * m_sinthetaDiff) *
        ((DiffpT - m_uDiff) * m_costhetaDiff - m_scaleDiff * (Diffp - m_vDiff) * m_sinthetaDiff)) )
    return false ;

  //
  //Cut on D0 proper time (tau)
  //- based on DaughtersInLHCbAndCutsForDstar
  //

  Gaudi::LorentzVector pEnd , pBegin ;
  pEnd.SetXYZT( theD0 -> end_vertex() -> position() . x() ,
                theD0 -> end_vertex() -> position() . y() ,
                theD0 -> end_vertex() -> position() . z() ,
                theD0 -> end_vertex() -> position() . t() * Gaudi::Units::c_light ) ;

  pBegin.SetXYZT( theD0 -> production_vertex() -> position() . x() ,
                  theD0 -> production_vertex() -> position() . y() ,
                  theD0 -> production_vertex() -> position() . z() ,
                  theD0 -> production_vertex() -> position() . t() * Gaudi::Units::c_light ) ;  

  Gaudi::LorentzVector disp = pEnd - pBegin ;
  
  // Boost displacement vector into D0 CoM frame
  Gaudi::LorentzVector fourM( theD0 -> momentum() ) ;
  ROOT::Math::Boost theBoost( fourM.BoostToCM() ) ;
  Gaudi::LorentzVector restDisp = theBoost( disp ) ;
  double tau = restDisp.t() / Gaudi::Units::c_light ;

  debug() << "D0 tau = " << tau << endmsg ;
  debug() << "D0 tau cut = " << m_D0_TAU << endmsg ;
  if ( tau < m_D0_TAU ) return false ;

  debug() << "Event passed !" << endmsg ;
  
  return true ;
}


