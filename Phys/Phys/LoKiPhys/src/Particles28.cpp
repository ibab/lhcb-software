// $Id: Particles28.cpp,v 1.1 2009-04-30 12:58:29 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// GaudiuKernel
// ============================================================================
#include "GaudiKernel/SmartIF.h"
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/GetDVAlgorithm.h"
#include "Kernel/DVAlgorithm.h"
/// ===========================================================================
// LoKi
// ============================================================================
#include "LoKi/Particles28.h"
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
  const IParticleReFitter* const s_REFIT   = 0 ;
  /// the invalid tool
  const IMassFit*          const s_MFIT    = 0 ;
  // ==========================================================================
  const IParticleReFitter* getRF 
  ( const std::string&      nick ,
    const LoKi::AuxFunBase& base )
  {
    // get LoKi service
    const LoKi::Interface<LoKi::ILoKiSvc>& svc = base.lokiSvc() ;
    base.Assert( !(!svc) , "LoKi Service is not available!" ) ;
    // get DVAlgorithm
    DVAlgorithm* alg = Gaudi::Utils::getDVAlgorithm
      ( SmartIF<IAlgContextSvc>( svc ) ) ;
    base.Assert ( 0 != alg , "DVAlgorithm is not available" ) ;
    const IParticleReFitter* rf = alg->particleReFitter( nick ) ;
    if ( 0 == rf )
    { base.Error("IParticleReFitter('"+nick+"') is not available") ; }
    return rf ;
  }
  // ==========================================================================
  const IMassFit* getMF 
  ( const std::string&      nick ,
    const LoKi::AuxFunBase& base )
  {
    // get LoKi service
    const LoKi::Interface<LoKi::ILoKiSvc>& svc = base.lokiSvc() ;
    base.Assert( !(!svc) , "LoKi Service is not available!" ) ;
    // get DVAlgorithm
    DVAlgorithm* alg = Gaudi::Utils::getDVAlgorithm
      ( SmartIF<IAlgContextSvc>( svc ) ) ;
    base.Assert ( 0 != alg , "DVAlgorithm is not available" ) ;
    const IMassFit* mf = alg -> massFitter( nick ) ;
    if ( 0 == mf )
    { base.Error("IMassFit('"+nick+"') is not available") ; }
    return mf ;
  }
  // ==========================================================================
} // end of anonymous namespace 
// ============================================================================
// constructor from the tool name 
// ============================================================================
LoKi::Particles::ReFitter::ReFitter ( const std::string& name ) 
  : LoKi::Particles::ReFit ( s_REFIT ) 
  , m_name ( name ) 
{}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::ReFitter::result_type 
LoKi::Particles::ReFitter::operator() 
  ( LoKi::Particles::ReFitter::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ("LHCb::Particle* points to NULL, return 'False'") ;
    return false ;
  }
  //
  if ( !fitter() ) { setFitter ( getRF ( m_name , *this )  ) ; }
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
  : LoKi::Particles::MassFit ( s_MFIT ) 
  , m_name ( name ) 
{}
// ============================================================================
// constructor from the tool name 
// ============================================================================
LoKi::Particles::MassFitter::MassFitter
( const double       mass , 
  const std::string& name ) 
  : LoKi::Particles::MassFit ( s_MFIT , mass ) 
  , m_name ( name ) 
{}
// ============================================================================
// constructor from the tool name 
// ============================================================================
LoKi::Particles::MassFitter::MassFitter
( const std::string& name , 
  const double       mass ) 
  : LoKi::Particles::MassFit ( s_MFIT , mass ) 
  , m_name ( name ) 
{}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::MassFitter::result_type 
LoKi::Particles::MassFitter::operator()
  ( LoKi::Particles::MassFitter::argument p ) const
{
  if ( 0 == p ) 
  {
    Error( "Invalid Particle, return 'false'") ;
    return false ;
  }
  //
  if ( !fitter() ) { setFitter ( getMF ( m_name , *this )  ) ; }
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

