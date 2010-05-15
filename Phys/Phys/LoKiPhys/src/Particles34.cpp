// $Id: Particles34.cpp,v 1.1 2010-05-15 16:50:37 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// LoKiPhys
// ============================================================================
#include "LoKi/Constants.h"
#include "LoKi/Child.h"
#include "LoKi/Particles34.h"
#include "LoKi/GetTools.h"
// ============================================================================
/** @file
 *  Set of functions for Kazu Karvalho Akiba 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2010-05-15
 */
// ============================================================================
namespace 
{
  const ILifetimeFitter*  const s_FITTER = 0 ;
}
// ============================================================================
// constructor from the index & tool  
// ============================================================================
LoKi::Particles::DaughterParticleCTau::DaughterParticleCTau 
( const unsigned short   index  , 
  const ILifetimeFitter* fitter ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_index  ( index  ) 
  , m_fitter ( fitter ) 
{
  if ( !m_fitter ) 
  { m_fitter = LoKi::GetTools::lifetimeFitter ( *this ) ; }
  Assert ( !(!m_fitter) , "Unable to get ILifetimeFitter" ) ;
}
// ============================================================================
// constructor from the index & tool  
// ============================================================================
LoKi::Particles::DaughterParticleCTau::DaughterParticleCTau 
( const unsigned short                     index  , 
  const LoKi::Interface<ILifetimeFitter>&  fitter ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_index  ( index  ) 
  , m_fitter ( fitter ) 
{
  if ( !m_fitter ) 
  { m_fitter = LoKi::GetTools::lifetimeFitter ( *this ) ; }
  Assert ( !(!m_fitter), "Unable to get ILifetimeFitter" ) ;
}
// ============================================================================
// constructor from the index & tool  
// ============================================================================
LoKi::Particles::DaughterParticleCTau::DaughterParticleCTau 
( const unsigned short  index  , 
  const std::string&    fitter ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_index  ( index   ) 
  , m_fitter ( s_FITTER ) 
{
  if ( !m_fitter ) 
  { m_fitter = LoKi::GetTools::lifetimeFitter ( *this , fitter ) ; }
  Assert ( !(!m_fitter) , 
           "Unable to get ILifetimeFitter '" + fitter + "'") ;
}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Particles::DaughterParticleCTau::~DaughterParticleCTau(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::DaughterParticleCTau*
LoKi::Particles::DaughterParticleCTau::clone() const 
{ return new DaughterParticleCTau ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::DaughterParticleCTau::result_type
LoKi::Particles::DaughterParticleCTau::operator()
  ( LoKi::Particles::DaughterParticleCTau::argument p ) const 
{ return ctau ( p ) ; }
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Particles::DaughterParticleCTau::fillStream 
( std::ostream& s ) const
{ return s << " DPCTAU( " << index() << " ) " ; }
// ============================================================================
// get results from the fit 
// ============================================================================
StatusCode LoKi::Particles::DaughterParticleCTau::fit 
( const LHCb::Particle* p           , 
  double&               ctau        , 
  double&               ctauerr     , 
  double&               ctaufitchi2 ) const 
{
  if ( 0 == p ) 
  { return Error ("LHCb::Particle* points to NULL") ; }
  // get the vertex 
  const LHCb::VertexBase* v = p->endVertex() ;
  if ( 0 == v ) 
  { return Error ("LHCb::VertexBase* points to NULL") ; }
  // get the daughter
  const LHCb::Particle* c = LoKi::Child::child ( p , m_index ) ;
  if ( 0 == c ) 
  { return Error ("Child points to NULL, return InvalidDistance") ; }
  // check  the tool:
  if ( !m_fitter ) 
  { m_fitter = LoKi::GetTools::lifetimeFitter ( *this ) ; }
  Assert ( !(!m_fitter) , "Invalid ILifetimeFitter") ;
  // use the tool 
  //
  double ltime        = LoKi::Constants::InvalidTime ;
  double ltimeerr     = LoKi::Constants::InvalidTime ;
  double ltimefitchi2 = LoKi::Constants::InvalidChi2 ;
  //
  StatusCode sc = m_fitter -> fit ( *v , *c , ltime , ltimeerr , ltimefitchi2 ) ;
  if ( sc.isFailure() ) 
  { return Error ( "Error from ILifetimeFitter::fit" , sc ) ; }
  //
  ctau         = ltime    * Gaudi::Units::c_light ;
  ctauerr      = ltimeerr * Gaudi::Units::c_light ;
  ctaufitchi2  = ltimefitchi2 ;
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// get ctau
// ============================================================================
double LoKi::Particles::DaughterParticleCTau::ctau  
( const LHCb::Particle* p ) const 
{
  double _ctau        = LoKi::Constants::InvalidDistance ;
  double _ctauerr     = LoKi::Constants::InvalidDistance ;
  double _ctaufitchi2 = LoKi::Constants::InvalidChi2 ;
  //
  StatusCode sc = fit ( p , _ctau , _ctauerr , _ctaufitchi2 ) ;
  if ( sc.isFailure() ) 
  {
    Error ( "Error from fit, return InvalidDistance" , sc ) ;
    return LoKi::Constants::InvalidDistance ;
  }
  return  _ctau ;
}
// ============================================================================
// get ctau error 
// ============================================================================
double LoKi::Particles::DaughterParticleCTau::ctauErr  
( const LHCb::Particle* p ) const 
{
  double _ctau        = LoKi::Constants::InvalidDistance ;
  double _ctauerr     = LoKi::Constants::InvalidDistance ;
  double _ctaufitchi2 = LoKi::Constants::InvalidChi2 ;
  //
  StatusCode sc = fit ( p , _ctau , _ctauerr , _ctaufitchi2 ) ;
  if ( sc.isFailure() ) 
  {
    Error ( "Error from fit, return InvalidDistance" , sc ) ;
    return LoKi::Constants::InvalidDistance ;
  }
  return  _ctauerr ;
}
// ============================================================================
// get ctau fit chi2 
// ============================================================================
double LoKi::Particles::DaughterParticleCTau::ctauFitChi2 
( const LHCb::Particle* p ) const 
{
  double _ctau        = LoKi::Constants::InvalidDistance ;
  double _ctauerr     = LoKi::Constants::InvalidDistance ;
  double _ctaufitchi2 = LoKi::Constants::InvalidChi2 ;
  //
  StatusCode sc = fit ( p , _ctau , _ctauerr , _ctaufitchi2 ) ;
  if ( sc.isFailure() ) 
  {
    Error ( "Error from fit, return InvalidChi2" , sc ) ;
    return LoKi::Constants::InvalidChi2 ;
  }
  return  _ctaufitchi2  ;
}
// ============================================================================
// get ctau significance
// ============================================================================
double LoKi::Particles::DaughterParticleCTau::ctauSignificance
( const LHCb::Particle* p ) const 
{
  double _ctau        = LoKi::Constants::InvalidDistance ;
  double _ctauerr     = LoKi::Constants::InvalidDistance ;
  double _ctaufitchi2 = LoKi::Constants::InvalidChi2 ;
  //
  StatusCode sc = fit ( p , _ctau , _ctauerr , _ctaufitchi2 ) ;
  if ( sc.isFailure() ) 
  {
    Error ( "Error from fit, return NegativeInfinity" , sc ) ;
    return LoKi::Constants::NegativeInfinity ;
  }
  return  _ctau / _ctauerr ;
}

// ============================================================================
// constructor from the index & tool  
// ============================================================================
LoKi::Particles::DaughterParticleCTauErr::DaughterParticleCTauErr
( const unsigned short   index  , 
  const ILifetimeFitter* fitter ) 
  : LoKi::Particles::DaughterParticleCTau ( index , fitter ) 
{
}// ============================================================================
// constructor from the index & tool  
// ============================================================================
LoKi::Particles::DaughterParticleCTauErr::DaughterParticleCTauErr
( const unsigned short   index  , 
  const LoKi::Interface<ILifetimeFitter>&  fitter ) 
  : LoKi::Particles::DaughterParticleCTau ( index , fitter ) 
{}
// ============================================================================
// constructor from the index & tool  
// ============================================================================
LoKi::Particles::DaughterParticleCTauErr::DaughterParticleCTauErr
( const unsigned short   index  , 
  const std::string&     fitter ) 
  : LoKi::Particles::DaughterParticleCTau ( index , fitter ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Particles::DaughterParticleCTauErr::~DaughterParticleCTauErr () {}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::DaughterParticleCTauErr*
LoKi::Particles::DaughterParticleCTauErr::clone() const 
{ return new DaughterParticleCTauErr( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::DaughterParticleCTauErr::result_type
LoKi::Particles::DaughterParticleCTauErr::operator()
  ( LoKi::Particles::DaughterParticleCTauErr::argument p ) const 
{ return ctauErr( p ) ; }
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Particles::DaughterParticleCTauErr::fillStream 
( std::ostream& s ) const
{ return s << " DPCTAUERR( " << index() << " ) " ; }


// ============================================================================
// constructor from the index & tool  
// ============================================================================
LoKi::Particles::DaughterParticleCTauFitChi2::DaughterParticleCTauFitChi2
( const unsigned short   index  , 
  const ILifetimeFitter* fitter ) 
  : LoKi::Particles::DaughterParticleCTau ( index , fitter ) 
{
}// ============================================================================
// constructor from the index & tool  
// ============================================================================
LoKi::Particles::DaughterParticleCTauFitChi2::DaughterParticleCTauFitChi2
( const unsigned short   index  , 
  const LoKi::Interface<ILifetimeFitter>&  fitter ) 
  : LoKi::Particles::DaughterParticleCTau ( index , fitter ) 
{}
// ============================================================================
// constructor from the index & tool  
// ============================================================================
LoKi::Particles::DaughterParticleCTauFitChi2::DaughterParticleCTauFitChi2
( const unsigned short   index  , 
  const std::string&     fitter ) 
  : LoKi::Particles::DaughterParticleCTau ( index , fitter ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Particles::DaughterParticleCTauFitChi2::~DaughterParticleCTauFitChi2 () {}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::DaughterParticleCTauFitChi2*
LoKi::Particles::DaughterParticleCTauFitChi2::clone() const 
{ return new DaughterParticleCTauFitChi2( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::DaughterParticleCTauFitChi2::result_type
LoKi::Particles::DaughterParticleCTauFitChi2::operator()
  ( LoKi::Particles::DaughterParticleCTauFitChi2::argument p ) const 
{ return ctauFitChi2( p ) ; }
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Particles::DaughterParticleCTauFitChi2::fillStream 
( std::ostream& s ) const
{ return s << " DPCTAUFITCHI2( " << index() << " ) " ; }


// ============================================================================
// constructor from the index & tool  
// ============================================================================
LoKi::Particles::DaughterParticleCTauSignificance::DaughterParticleCTauSignificance
( const unsigned short   index  , 
  const ILifetimeFitter* fitter ) 
  : LoKi::Particles::DaughterParticleCTau ( index , fitter ) 
{
}// ============================================================================
// constructor from the index & tool  
// ============================================================================
LoKi::Particles::DaughterParticleCTauSignificance::DaughterParticleCTauSignificance
( const unsigned short   index  , 
  const LoKi::Interface<ILifetimeFitter>&  fitter ) 
  : LoKi::Particles::DaughterParticleCTau ( index , fitter ) 
{}
// ============================================================================
// constructor from the index & tool  
// ============================================================================
LoKi::Particles::DaughterParticleCTauSignificance::DaughterParticleCTauSignificance
( const unsigned short   index  , 
  const std::string&     fitter ) 
  : LoKi::Particles::DaughterParticleCTau ( index , fitter ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Particles::DaughterParticleCTauSignificance::~DaughterParticleCTauSignificance () {}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::DaughterParticleCTauSignificance*
LoKi::Particles::DaughterParticleCTauSignificance::clone() const 
{ return new DaughterParticleCTauSignificance( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::DaughterParticleCTauSignificance::result_type
LoKi::Particles::DaughterParticleCTauSignificance::operator()
  ( LoKi::Particles::DaughterParticleCTauSignificance::argument p ) const 
{ return ctauSignificance( p ) ; }
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Particles::DaughterParticleCTauSignificance::fillStream 
( std::ostream& s ) const
{ return s << " DPCTAUSIGNIFICANCE( " << index() << " ) " ; }


// ============================================================================
// The END 
// ============================================================================
