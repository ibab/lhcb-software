// $Id$
// ============================================================================
// Include files
// ============================================================================
// GaudiuKernel
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Particles28.h"
#include "LoKi/GetTools.h"
// ============================================================================
/** @file
 *  The file with functors for various "refit"
 *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
 *  @date   2009-04-30
 */
// ============================================================================
namespace
{
  // ==========================================================================
  /// the invalid tool
  const IParticleReFitter* const s_REFIT   = nullptr ;
  /// the invalid tool
  const IMassFit*          const s_MFIT    = nullptr ;
  // ==========================================================================
} // end of anonymous namespace
// ============================================================================
// constructor from the tool name
// ============================================================================
LoKi::Particles::ReFitter::ReFitter ( const std::string& name )
  : LoKi::AuxFunBase  ( std::tie ( name ) )
  , LoKi::Particles::ReFit ( s_REFIT )
  , m_name ( name )
{}
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::Particles::ReFitter::result_type
LoKi::Particles::ReFitter::operator()
  ( LoKi::Particles::ReFitter::argument p ) const
{
  if ( !p )
  {
    Error ("LHCb::Particle* points to NULL, return 'False'") ;
    return false ;
  }
  //
  if ( !fitter() )
  { setFitter ( LoKi::GetTools::particleReFitter ( *this , m_name )  ) ; }
  //
  const LHCb::Particle* _p  = p ;
  LHCb::Particle* __p = const_cast<LHCb::Particle*>( _p ) ;
  //
  StatusCode sc = reFit ( __p ) ;
  //
  return sc.isSuccess() ;
}
// ============================================================================
// OPTIONAL: nice printout
// ============================================================================
std::ostream& LoKi::Particles::ReFitter::fillStream ( std::ostream& s ) const
{ return s << "REFIT('" << m_name << "')" ; }
// ============================================================================



// ============================================================================
// constructor from the tool name
// ============================================================================
LoKi::Particles::MassFitter::MassFitter
( const std::string& name )
  : LoKi::AuxFunBase  ( std::tie ( name ) )
  , LoKi::Particles::MassFit ( s_MFIT )
  , m_name ( name )
{}
// ============================================================================
// constructor from the tool name
// ============================================================================
LoKi::Particles::MassFitter::MassFitter
( const double       mass ,
  const std::string& name )
  : LoKi::AuxFunBase  ( std::tie ( mass , name ) )
  , LoKi::Particles::MassFit ( s_MFIT , mass )
  , m_name ( name )
{}
// ============================================================================
// constructor from the tool name
// ============================================================================
LoKi::Particles::MassFitter::MassFitter
( const std::string& name ,
  const double       mass )
  : LoKi::AuxFunBase  ( std::tie ( name , mass  ) )
  , LoKi::Particles::MassFit ( s_MFIT , mass )
  , m_name ( name )
{}
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::Particles::MassFitter::result_type
LoKi::Particles::MassFitter::operator()
  ( LoKi::Particles::MassFitter::argument p ) const
{
  if ( !p )
  {
    Error( "Invalid Particle, return 'false'") ;
    return false ;
  }
  //
  if ( !fitter() )
  { setFitter ( LoKi::GetTools::massFitter ( *this , m_name ) ) ; }
  //
  const LHCb::Particle* _p  = p ;
  LHCb::Particle* __p = const_cast<LHCb::Particle*>( _p ) ;
  //
  StatusCode sc = flag() ? fit ( __p , mass() ) : fit ( __p ) ;
  //
  return sc.isSuccess() ;
}
// ============================================================================
// OPTIONAL: nice printout
// ============================================================================
std::ostream& LoKi::Particles::MassFitter::fillStream ( std::ostream& s ) const
{
  if ( !flag() ) { return s << "MFIT" ; }
  return s << "MFITTER('" << m_name  << "'," << mass() << ")";
}
// ============================================================================


// ============================================================================
// The END
// ============================================================================
