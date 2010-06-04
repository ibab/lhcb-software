// $Id: Particles36.cpp,v 1.1 2010-06-04 12:23:59 ibelyaev Exp $
// ========================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <set>
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/ParticleParams.h"
// ============================================================================
// DaVinciInterafces 
// ============================================================================
#include "Kernel/IDecayTreeFit.h"
// ============================================================================
// DaVinciTypes 
// ============================================================================
#include "Kernel/DecayTree.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Constants.h"
#include "LoKi/Particles36.h"
// ============================================================================
/** @file  Particles36.cpp
 *  Collection of functors dealing with DecayTreeFitter 
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2010-06-03
 */
// ============================================================================
// constructor with the functor
// ============================================================================
LoKi::Particles::DecayTreeFitterFun::DecayTreeFitterFun 
( const LoKi::PhysTypes::Func&         fun    , 
  const bool                           usePV  , 
  const IDecayTreeFit*                 fitter ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::AuxDTFBase ( fitter ) 
  , m_fun   ( fun   )
  , m_usePV ( usePV ) 
{}
// ============================================================================
// constructor with the functor
// ============================================================================
LoKi::Particles::DecayTreeFitterFun::DecayTreeFitterFun 
( const LoKi::PhysTypes::Func&         fun         , 
  const bool                           usePV       ,
  const std::vector<std::string>&      constraints ,
  const IDecayTreeFit*                 fitter      ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::AuxDTFBase ( constraints , fitter ) 
  , m_fun         ( fun    ) 
  , m_usePV       ( usePV  ) 
{}
// ======================================================================
// constructor with the functor
// ============================================================================
LoKi::Particles::DecayTreeFitterFun::DecayTreeFitterFun 
( const LoKi::PhysTypes::Func&         fun         , 
  const bool                           usePV       ,
  const std::string&                   constraint  ,
  const IDecayTreeFit*                 fitter      )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::AuxDTFBase ( std::vector<std::string>(1,constraint) , fitter ) 
  , m_fun         ( fun    ) 
  , m_usePV       ( usePV  ) 
{}
// ======================================================================
// constructor with the functor
// ============================================================================
LoKi::Particles::DecayTreeFitterFun::DecayTreeFitterFun 
( const LoKi::PhysTypes::Func&         fun         , 
  const bool                           usePV       ,
  const LoKi::AuxDTFBase&              base        )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::AuxDTFBase ( base   ) 
  , m_fun            ( fun    ) 
  , m_usePV          ( usePV  ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Particles::DecayTreeFitterFun::~DecayTreeFitterFun(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::DecayTreeFitterFun*
LoKi::Particles::DecayTreeFitterFun::clone() const 
{ return new LoKi::Particles::DecayTreeFitterFun ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::DecayTreeFitterFun::result_type 
LoKi::Particles::DecayTreeFitterFun::operator() 
  ( LoKi::Particles::DecayTreeFitterFun::argument p ) const 
{
  //
  if ( 0 == p ) 
  {
    Warning ( "LHCb::Particle* points to NULL, return NegativeInfinity" ) ;
    return LoKi::Constants::NegativeInfinity ;
  }
  //
  const LHCb::VertexBase* vertex = 0 ;
  if ( m_usePV ) 
  {
    vertex = bestVertex ( p ) ;
    if ( 0 == vertex ) 
    { Warning ( "``Best vertex'' points zero, constraits will be disabled!" ) ; } 
  }
  //
  // apply local mass constrains
  applyConstraints () ;
  //
  IDecayTreeFit* _fitter = fitter() ;
  //
  StatusCode sc = _fitter->fit ( p , vertex ) ;
  if ( sc.isFailure() ) 
  {
    Error ( "Error from IDecayTreeFit" , sc ) ;
    return LoKi::Constants::NegativeInfinity ;
  }
  // get the tree and evaluate the functor 
  return m_fun ( _fitter -> fittedTree () ) ;
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& 
LoKi::Particles::DecayTreeFitterFun::fillStream ( std::ostream& s ) const 
{
  //
  s << " DTF_FUN(" ;
  //
  s << " " << m_fun << " ," 
    << ( m_usePV ? " True " : " False " ) ; 
  //
  std::vector<std::string> ss = constraints() ;
  if      ( ss.empty()       ) {}
  else if ( 1 == ss.size  () ) { s << ", '" << ss[0] ; }
  else 
  { 
    s << ", " ;
    Gaudi::Utils::toStream ( ss ,s  ) ;
  }
  //
  return s << " ) " ;
}
// ============================================================================

// ============================================================================
// constructor with the functor
// ============================================================================
LoKi::Particles::DecayTreeFitterCut::DecayTreeFitterCut
( const LoKi::PhysTypes::Cuts&         fun         , 
  const bool                           usePV       ,
  const IDecayTreeFit*                 fitter      ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , LoKi::AuxDTFBase ( fitter ) 
  , m_fun         ( fun    ) 
  , m_usePV       ( usePV  ) 
{}
// ============================================================================
// constructor with the functor
// ============================================================================
LoKi::Particles::DecayTreeFitterCut::DecayTreeFitterCut
( const LoKi::PhysTypes::Cuts&         fun         , 
  const bool                           usePV       ,
  const std::vector<std::string>&      constraints ,
  const IDecayTreeFit*                 fitter      ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , LoKi::AuxDTFBase ( constraints , fitter ) 
  , m_fun         ( fun    ) 
  , m_usePV       ( usePV  ) 
{}
// ======================================================================
// constructor with the functor
// ============================================================================
LoKi::Particles::DecayTreeFitterCut::DecayTreeFitterCut
( const LoKi::PhysTypes::Cuts&         fun         , 
  const bool                           usePV       ,
  const std::string&                   constraint  ,
  const IDecayTreeFit*                 fitter      )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , LoKi::AuxDTFBase ( std::vector<std::string>(1,constraint) , fitter ) 
  , m_fun         ( fun    ) 
  , m_usePV       ( usePV  ) 
{}
// ======================================================================
// constructor with the functor
// ============================================================================
LoKi::Particles::DecayTreeFitterCut::DecayTreeFitterCut
( const LoKi::PhysTypes::Cuts&         fun         , 
  const bool                           usePV       ,
  const LoKi::AuxDTFBase&              base        )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , LoKi::AuxDTFBase ( base   ) 
  , m_fun            ( fun    ) 
  , m_usePV          ( usePV  ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Particles::DecayTreeFitterCut::~DecayTreeFitterCut(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::DecayTreeFitterCut*
LoKi::Particles::DecayTreeFitterCut::clone() const 
{ return new LoKi::Particles::DecayTreeFitterCut ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::DecayTreeFitterCut::result_type 
LoKi::Particles::DecayTreeFitterCut::operator() 
  ( LoKi::Particles::DecayTreeFitterCut::argument p ) const 
{
  //
  if ( 0 == p ) 
  {
    Warning ( "LHCb::Particle* points to NULL, return NegativeInfinity" ) ;
    return LoKi::Constants::NegativeInfinity ;
  }
  //
  const LHCb::VertexBase* vertex = 0 ;
  if ( m_usePV ) 
  {
    vertex = bestVertex ( p ) ;
    if ( 0 == vertex ) 
    { Warning ( "``Best vertex'' points zero, constraits will be disabled!" ) ; } 
  }
  //
  // apply local mass constrains
  applyConstraints () ;
  //
  IDecayTreeFit* _fitter = fitter() ;
  //
  StatusCode sc = _fitter->fit ( p , vertex ) ;
  if ( sc.isFailure() ) 
  {
    Error ( "Error from IDecayTreeFit" , sc ) ;
    return LoKi::Constants::NegativeInfinity ;
  }
  // get the tree and evaluate the functor 
  return m_fun ( _fitter -> fittedTree () ) ;
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& 
LoKi::Particles::DecayTreeFitterCut::fillStream ( std::ostream& s ) const 
{
  //
  //
  s << " DTF_FUN(" ;
  //
  s << " " << m_fun << " ," 
    << ( m_usePV ? " True " : " False " ) ; 
  //
  std::vector<std::string> ss = constraints() ;
  if      ( ss.empty()       ) {}
  else if ( 1 == ss.size  () ) { s << ", '" << ss[0] ; }
  else 
  { 
    s << ", " ;
    Gaudi::Utils::toStream ( ss ,s  ) ;
  }
  //
  return s << " ) " ;
}
// ============================================================================



// ============================================================================
// constructor from child selector & PV-flag & constraints 
// ============================================================================
LoKi::Particles::ChildCTau::ChildCTau
( const LoKi::Child::Selector&     child       , 
  const bool                       usePV       , 
  const std::vector<std::string>&  constraints ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , LoKi::AuxDTFBase ( constraints ) 
  , m_child ( child ) 
  , m_usePV ( usePV ) 
{}
// ============================================================================
// constructor from child selector & PV-flag & constraints 
// ============================================================================
LoKi::Particles::ChildCTau::ChildCTau
( const std::vector<unsigned int>& child       , 
  const bool                       usePV       , 
  const std::vector<std::string>&  constraints ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , LoKi::AuxDTFBase ( constraints ) 
  , m_child ( child ) 
  , m_usePV ( usePV ) 
{}
// ============================================================================
// constructor from child selector & PV-flag & constraints 
// ============================================================================
LoKi::Particles::ChildCTau::ChildCTau
( const unsigned int               child       , 
  const bool                       usePV       , 
  const std::vector<std::string>&  constraints ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , LoKi::AuxDTFBase ( constraints ) 
  , m_child ( child ) 
  , m_usePV ( usePV ) 
{}
// ============================================================================
// constructor from child selector & PV-flag & constraints 
// ============================================================================
LoKi::Particles::ChildCTau::ChildCTau
( const std::string&               child       , 
  const bool                       usePV       , 
  const std::vector<std::string>&  constraints ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , LoKi::AuxDTFBase ( constraints ) 
  , m_child ( child ) 
  , m_usePV ( usePV ) 
{}
// ============================================================================
// constructor from child selector & PV-flag & constraints 
// ============================================================================
LoKi::Particles::ChildCTau::ChildCTau
( const Decays::iNode&             child       , 
  const bool                       usePV       , 
  const std::vector<std::string>&  constraints ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , LoKi::AuxDTFBase ( constraints ) 
  , m_child ( child ) 
  , m_usePV ( usePV ) 
{}
// ============================================================================
// constructor from child selector & PV-flag & constraints 
// ============================================================================
LoKi::Particles::ChildCTau::ChildCTau
( const Decays::IDecay::iTree&     child       , 
  const bool                       usePV       , 
  const std::vector<std::string>&  constraints ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , LoKi::AuxDTFBase ( constraints ) 
  , m_child ( child ) 
  , m_usePV ( usePV ) 
{}
// ============================================================================
// constructor from child selector & PV-flag & constraints 
// ============================================================================
LoKi::Particles::ChildCTau::ChildCTau
( const Decays::IDecay::Finder&    child       , 
  const bool                       usePV       , 
  const std::vector<std::string>&  constraints ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , LoKi::AuxDTFBase ( constraints ) 
  , m_child ( child ) 
  , m_usePV ( usePV ) 
{}
// ============================================================================
// constructor from child selector & PV-flag & constraints 
// ============================================================================
LoKi::Particles::ChildCTau::ChildCTau
( const LoKi::PhysTypes::Cuts&     child       , 
  const bool                       usePV       , 
  const std::vector<std::string>&  constraints ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , LoKi::AuxDTFBase ( constraints ) 
  , m_child ( child ) 
  , m_usePV ( usePV ) 
{}
// ============================================================================
// MANDATORY: virtual descructor 
// ============================================================================
LoKi::Particles::ChildCTau::~ChildCTau(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::ChildCTau*
LoKi::Particles::ChildCTau::clone() const
{ return new LoKi::Particles::ChildCTau ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::ChildCTau::result_type
LoKi::Particles::ChildCTau::operator() 
  ( LoKi::Particles::ChildCTau::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, retrn InvalidDistance" );
    return LoKi::Constants::InvalidDistance ; 
  }
  //
  const IDecayTreeFit::Fitted* fitted  = params ( p ) ;
  if ( 0 == fitted ) 
  {
    Error ( "Gaudi::Math::ParticleParams points to NULL, retrn InvalidDistance" );
    return LoKi::Constants::InvalidDistance ; 
  }
  //
  return fitted->ctau().value() ;
} 
// ============================================================================
// get the fitted parameters
// ============================================================================
const IDecayTreeFit::Fitted*
LoKi::Particles::ChildCTau::params ( const LHCb::Particle* p ) const 
{
  if ( 0 == p ) { return 0 ; }                                      // REUTRN 
  //
  const LHCb::Particle* c = child( p ) ;
  if ( c == 0 ) 
  {
    Error ( "Unable to select he proper child particle ") ;
    return 0 ;                                                       // RETURN 
  }
  //
  IDecayTreeFit* _fitter = fitter() ;
  //
  const LHCb::VertexBase* vertex = 0 ;
  if ( usePV() ) 
  {
    vertex = bestVertex ( p ) ;
    if ( 0 == vertex ) 
    { Warning ( "``Best vertex'' points zero, constraits will be disabled!" ) ; } 
  }
  //
  // apply local mass constrains
  applyConstraints () ;
  //
  StatusCode sc = _fitter->fit ( p , vertex ) ;
  if ( sc.isFailure() ) 
  {
    Error ( "Error from IDecayTreeFit" , sc ) ;
    return 0 ;                                                       // RETURN
  }
  //
  return _fitter->fitted ( c ) ;
}
// =============================================================================
// OPTIONAL: nice printout
// =============================================================================
std::ostream& 
LoKi::Particles::ChildCTau::fillStream ( std::ostream& s ) const 
{
  return 
    s << " DTF_CTAU( " 
      << child ()
      << " , " <<  ( usePV() ? " True " : " False " ) << " ) " ;
}




// ============================================================================
// constructor from child selector & PV-flag & constraints 
// ============================================================================
LoKi::Particles::ChildCTauErr::ChildCTauErr
( const LoKi::Child::Selector&     child       , 
  const bool                       usePV       , 
  const std::vector<std::string>&  constraints ) 
  : LoKi::Particles::ChildCTau ( child , usePV , constraints ) 
{}
// ============================================================================
// constructor from child selector & PV-flag & constraints 
// ============================================================================
LoKi::Particles::ChildCTauErr::ChildCTauErr
( const std::vector<unsigned int>& child       , 
  const bool                       usePV       , 
  const std::vector<std::string>&  constraints ) 
  : LoKi::Particles::ChildCTau ( child , usePV , constraints ) 
{}
// ============================================================================
// constructor from child selector & PV-flag & constraints 
// ============================================================================
LoKi::Particles::ChildCTauErr::ChildCTauErr
( const unsigned int               child       , 
  const bool                       usePV       , 
  const std::vector<std::string>&  constraints ) 
  : LoKi::Particles::ChildCTau ( child , usePV , constraints ) 
{}
// ============================================================================
// constructor from child selector & PV-flag & constraints 
// ============================================================================
LoKi::Particles::ChildCTauErr::ChildCTauErr
( const std::string&               child       , 
  const bool                       usePV       , 
  const std::vector<std::string>&  constraints ) 
  : LoKi::Particles::ChildCTau ( child , usePV , constraints ) 
{}
// ============================================================================
// constructor from child selector & PV-flag & constraints 
// ============================================================================
LoKi::Particles::ChildCTauErr::ChildCTauErr
( const Decays::iNode&             child       , 
  const bool                       usePV       , 
  const std::vector<std::string>&  constraints ) 
  : LoKi::Particles::ChildCTau ( child , usePV , constraints ) 
{}
// ============================================================================
// constructor from child selector & PV-flag & constraints 
// ============================================================================
LoKi::Particles::ChildCTauErr::ChildCTauErr
( const Decays::IDecay::iTree&     child       , 
  const bool                       usePV       , 
  const std::vector<std::string>&  constraints ) 
  : LoKi::Particles::ChildCTau ( child , usePV , constraints ) 
{}
// ============================================================================
// constructor from child selector & PV-flag & constraints 
// ============================================================================
LoKi::Particles::ChildCTauErr::ChildCTauErr
( const Decays::IDecay::Finder&    child       , 
  const bool                       usePV       , 
  const std::vector<std::string>&  constraints ) 
  : LoKi::Particles::ChildCTau ( child , usePV , constraints ) 
{}
// ============================================================================
// constructor from child selector & PV-flag & constraints 
// ============================================================================
LoKi::Particles::ChildCTauErr::ChildCTauErr
( const LoKi::PhysTypes::Cuts&     child       , 
  const bool                       usePV       , 
  const std::vector<std::string>&  constraints ) 
  : LoKi::Particles::ChildCTau ( child , usePV , constraints ) 
{}
// ============================================================================
// MANDATORY: virtual descructor 
// ============================================================================
LoKi::Particles::ChildCTauErr::~ChildCTauErr(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::ChildCTauErr*
LoKi::Particles::ChildCTauErr::clone() const
{ return new LoKi::Particles::ChildCTauErr ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::ChildCTauErr::result_type
LoKi::Particles::ChildCTauErr::operator() 
  ( LoKi::Particles::ChildCTauErr::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, retrn InvalidDistance" );
    return LoKi::Constants::InvalidDistance ; 
  }
  //
  const IDecayTreeFit::Fitted* fitted  = params ( p ) ;
  if ( 0 == fitted ) 
  {
    Error ( "Gaudi::Math::ParticleParams points to NULL, return InvalidDistance" );
    return LoKi::Constants::InvalidDistance ; 
  }
  //
  return fitted->ctau().error() ;
} 
// =============================================================================
// OPTIONAL: nice printout
// =============================================================================
std::ostream& 
LoKi::Particles::ChildCTauErr::fillStream ( std::ostream& s ) const 
{
  return 
    s << " DTF_CTAUERR( " 
      << child () 
      << " , " <<  ( usePV() ? " True " : " False " ) << " ) " ;
}

  










// ============================================================================
// constructor from child selector & PV-flag & constraints 
// ============================================================================
LoKi::Particles::ChildCTauSignificance::ChildCTauSignificance
( const LoKi::Child::Selector&     child       , 
  const bool                       usePV       , 
  const std::vector<std::string>&  constraints ) 
  : LoKi::Particles::ChildCTauErr ( child , usePV , constraints ) 
{}
// ============================================================================
// constructor from child selector & PV-flag & constraints 
// ============================================================================
LoKi::Particles::ChildCTauSignificance::ChildCTauSignificance
( const std::vector<unsigned int>& child       , 
  const bool                       usePV       , 
  const std::vector<std::string>&  constraints ) 
  : LoKi::Particles::ChildCTauErr ( child , usePV , constraints ) 
{}
// ============================================================================
// constructor from child selector & PV-flag & constraints 
// ============================================================================
LoKi::Particles::ChildCTauSignificance::ChildCTauSignificance
( const unsigned int               child       , 
  const bool                       usePV       , 
  const std::vector<std::string>&  constraints ) 
  : LoKi::Particles::ChildCTauErr ( child , usePV , constraints ) 
{}
// ============================================================================
// constructor from child selector & PV-flag & constraints 
// ============================================================================
LoKi::Particles::ChildCTauSignificance::ChildCTauSignificance
( const std::string&               child       , 
  const bool                       usePV       , 
  const std::vector<std::string>&  constraints ) 
  : LoKi::Particles::ChildCTauErr ( child , usePV , constraints ) 
{}
// ============================================================================
// constructor from child selector & PV-flag & constraints 
// ============================================================================
LoKi::Particles::ChildCTauSignificance::ChildCTauSignificance
( const Decays::iNode&             child       , 
  const bool                       usePV       , 
  const std::vector<std::string>&  constraints ) 
  : LoKi::Particles::ChildCTauErr ( child , usePV , constraints ) 
{}
// ============================================================================
// constructor from child selector & PV-flag & constraints 
// ============================================================================
LoKi::Particles::ChildCTauSignificance::ChildCTauSignificance
( const Decays::IDecay::iTree&     child       , 
  const bool                       usePV       , 
  const std::vector<std::string>&  constraints ) 
  : LoKi::Particles::ChildCTauErr ( child , usePV , constraints ) 
{}
// ============================================================================
// constructor from child selector & PV-flag & constraints 
// ============================================================================
LoKi::Particles::ChildCTauSignificance::ChildCTauSignificance
( const Decays::IDecay::Finder&    child       , 
  const bool                       usePV       , 
  const std::vector<std::string>&  constraints ) 
  : LoKi::Particles::ChildCTauErr ( child , usePV , constraints ) 
{}
// ============================================================================
// constructor from child selector & PV-flag & constraints 
// ============================================================================
LoKi::Particles::ChildCTauSignificance::ChildCTauSignificance
( const LoKi::PhysTypes::Cuts&     child       , 
  const bool                       usePV       , 
  const std::vector<std::string>&  constraints ) 
  : LoKi::Particles::ChildCTauErr ( child , usePV , constraints ) 
{}
// ============================================================================
// MANDATORY: virtual descructor 
// ============================================================================
LoKi::Particles::ChildCTauSignificance::~ChildCTauSignificance(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::ChildCTauSignificance*
LoKi::Particles::ChildCTauSignificance::clone() const
{ return new LoKi::Particles::ChildCTauSignificance ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::ChildCTauSignificance::result_type
LoKi::Particles::ChildCTauSignificance::operator() 
  ( LoKi::Particles::ChildCTauSignificance::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, returnNegativeInfinity" );
    return LoKi::Constants::NegativeInfinity ;                     // RETURN 
  }
  //
  const IDecayTreeFit::Fitted* fitted  = params ( p ) ;
  if ( 0 == fitted ) 
  {
    Error ( "Gaudi::Math::ParticleParams points to NULL, return NegativeInfinity" );
    return LoKi::Constants::NegativeInfinity ;                     // RETURN 
  }
  //
  const Gaudi::Math::ValueWithError ctau = fitted->ctau () ;
  //
  if ( 0 >= ctau.error() ) 
  {
    Error ( "Non-positive error in ctau, return NegativeInfinity" );
    return LoKi::Constants::NegativeInfinity ;                     // RETURN 
  }
  //
  return ctau.value() / ctau.error() ;                             // RETURN 
} 
// =============================================================================
// OPTIONAL: nice printout
// =============================================================================
std::ostream& 
LoKi::Particles::ChildCTauSignificance::fillStream ( std::ostream& s ) const 
{
  return 
    s << " DTF_CTAUSIGNIFICANCE ( " 
      << child () 
      << " , " <<  ( usePV() ? " True " : " False " ) << " ) " ;
}

  







// ============================================================================
// The END 
// ============================================================================
