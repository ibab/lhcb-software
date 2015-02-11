// $Id$
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Particles27.h"
#include "LoKi/Constants.h"
// ============================================================================
/** @file
 *  The implementation file for functors form the file LoKi/Particles27.h
 *
 *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
 *  @date   2009-04-30
 */
// ============================================================================
// constructor from the tool 
// ============================================================================
LoKi::Particles::ReFit::ReFit
( const IParticleReFitter* fitter ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , m_fitter ( fitter )
{}
// ============================================================================
// constructor from the tool 
// ============================================================================
LoKi::Particles::ReFit::ReFit
( const LoKi::Interface<IParticleReFitter>& fitter ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , m_fitter ( fitter )
{}
// ============================================================================
// destructor 
// ============================================================================
LoKi::Particles::ReFit::~ReFit() 
{
  if ( m_fitter && !gaudi() ) 
  {
    // Warning("Manual reset IParticleReFiter") ;
    m_fitter.reset() ;    
  }
}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::ReFit::result_type 
LoKi::Particles::ReFit::operator()
  ( LoKi::Particles::ReFit::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error( "Invalid Particle, return 'false'") ;
    return false ;
  }
  /// 
  const LHCb::Particle* _p  = p ;
  LHCb::Particle* __p = const_cast<LHCb::Particle*>( _p ) ;
  //
  StatusCode sc = reFit ( __p ) ;
  //
  return sc.isSuccess() ; 
}
// ============================================================================
// refit the particle 
// ============================================================================
StatusCode LoKi::Particles::ReFit::reFit ( LHCb::Particle* p ) const 
{
  Assert ( 0 != p       , "Argument is invaild!" ) ;
  // check the tool
  Assert ( !(!fitter()) , "IParticleReFitter is invalid!" ) ;
  //
  StatusCode sc = fitter()->reFit( *p ) ;
  if ( sc.isFailure() ) { return Warning( "Error from IParticleReFitter", sc ) ; }
  return sc ;
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Particles::ReFit::fillStream ( std::ostream& s ) const 
{ return s << "REFIT_" ; }
// ============================================================================



// ============================================================================
// constructor from the tool 
// ============================================================================
LoKi::Particles::MassFit::MassFit 
( const IMassFit*                  fitter ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , m_fitter ( fitter ) 
  , m_mass   ( LoKi::Constants::InvalidMass ) 
  , m_flag   ( false ) 
{}
// ============================================================================
// constructor from the tool 
// ============================================================================
LoKi::Particles::MassFit::MassFit 
( const LoKi::Interface<IMassFit>&   fitter ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , m_fitter ( fitter ) 
  , m_mass   ( LoKi::Constants::InvalidMass ) 
  , m_flag   ( false ) 
{}
// ============================================================================
// constructor from the tool & mass 
// ============================================================================
LoKi::Particles::MassFit::MassFit 
( const IMassFit*                  fitter , 
  const double                     mass   ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , m_fitter ( fitter ) 
  , m_mass   ( mass   ) 
  , m_flag   ( true   ) 
{}
// ============================================================================
// constructor from the tool  & mass 
// ============================================================================
LoKi::Particles::MassFit::MassFit 
( const LoKi::Interface<IMassFit>& fitter ,
  const double                     mass   ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , m_fitter ( fitter ) 
  , m_mass   ( mass   ) 
  , m_flag   ( true   ) 
{}
// ============================================================================
// constructor from the tool & mass 
// ============================================================================
LoKi::Particles::MassFit::MassFit 
( const double                     mass   ,
  const IMassFit*                  fitter )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , m_fitter ( fitter ) 
  , m_mass   ( mass   ) 
  , m_flag   ( true   ) 
{}
// ============================================================================
// constructor from the tool  & mass 
// ============================================================================
LoKi::Particles::MassFit::MassFit 
( const double                     mass   , 
  const LoKi::Interface<IMassFit>& fitter )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , m_fitter ( fitter ) 
  , m_mass   ( mass   ) 
  , m_flag   ( true   ) 
{}
// ============================================================================
// destructor 
// ============================================================================
LoKi::Particles::MassFit::~MassFit() 
{
  if ( m_fitter && !gaudi() ) 
  {
    // Warning("Manual reset IMassFit") ;
    m_fitter.reset() ;
  }
}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::MassFit::result_type 
LoKi::Particles::MassFit::operator()
  ( LoKi::Particles::MassFit::argument p ) const
{
  if ( 0 == p ) 
  {
    Error( "Invalid Particle, return 'false'") ;
    return false ;
  }
  /// 
  const LHCb::Particle* _p  = p ;
  LHCb::Particle* __p = const_cast<LHCb::Particle*>( _p ) ;
  //
  StatusCode sc = flag() ? fit ( __p , mass() ) : fit ( __p ) ;
  //
  return sc.isSuccess() ; 
}
// ============================================================================
// fit the particle 
// ============================================================================
StatusCode LoKi::Particles::MassFit::fit 
( LHCb::Particle* p ) const 
{
  Assert ( 0 != p       , "Argument is invaild!" ) ;
  Assert ( !(!fitter()) , "IMassFit is invalid!" ) ;
  StatusCode sc = fitter()->fit ( *p ) ;
  if ( sc.isFailure() ) { return Warning ( "Error (1) from IMassFit", sc ) ; }
  return sc ;
}
// ============================================================================
// fit the particle 
// ============================================================================
StatusCode LoKi::Particles::MassFit::fit 
( LHCb::Particle* p    ,
  const double    mass ) const
{
  Assert ( !(!fitter()) , "IMassFit is invalid!" ) ;
  StatusCode sc = fitter()->fit ( p , mass ) ;
  if ( sc.isFailure() ) { return Warning ( "Error (2) from IMassFit", sc ) ; }
  return sc ;
}
// ============================================================================
// OPTIONAL: nice printout
// ============================================================================
std::ostream& LoKi::Particles::MassFit::fillStream ( std::ostream& s ) const 
{ return s << "MFIT_" ; }
// ============================================================================







  


// ============================================================================
// The END 
// ============================================================================
