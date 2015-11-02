// $Id$
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <set>
// ============================================================================
// GSL 
// ============================================================================
#include "gsl/gsl_cdf.h"
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
#include "LoKi/iTree.h"
#include "LoKi/DecayFinder.h"
#include "LoKi/Constants.h"
#include "LoKi/BasicFunctors.h"
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
namespace 
{
  // ==========================================================================
  /// static empty map 
  const LoKi::Particles::DecayTreeFitterFun::MASSES s_MASSES     ; 
  /// static empty vector 
  const std::vector<std::string>                    s_CONTRAINTS ;
  // ==========================================================================
}
// ============================================================================
// constructor with the functor
// ============================================================================
LoKi::Particles::DecayTreeFitterFun::DecayTreeFitterFun 
( const LoKi::PhysTypes::Func&                       fun    , 
  const bool                                         usePV  , 
  const IDecayTreeFit*                               fitter ) 
  : LoKi::AuxFunBase ( std::tie (  fun , usePV ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::AuxDTFBase ( fitter ) 
  , m_fun   ( fun   )
  , m_usePV ( usePV ) 
  , m_chi2  ( -1    ) 
  , m_bad   ( LoKi::Constants::NegativeInfinity ) 
{}
// ============================================================================
// constructor with the functor
// ============================================================================
LoKi::Particles::DecayTreeFitterFun::DecayTreeFitterFun 
( const LoKi::PhysTypes::Func&                       fun         , 
  const bool                                         usePV       ,
  const std::string&                                 constraints ,
  const LoKi::Particles::DecayTreeFitterFun::MASSES& masses      , 
  const IDecayTreeFit*                               fitter      ) 
  : LoKi::AuxFunBase ( std::tie (  fun , usePV , constraints , masses ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::AuxDTFBase ( std::vector<std::string>(1 , constraints ) , 
                       masses , 
                       fitter ) 
  , m_fun   ( fun    ) 
  , m_usePV ( usePV  ) 
  , m_chi2  ( -1     ) 
  , m_bad   ( LoKi::Constants::NegativeInfinity ) 
{}
// ============================================================================
// constructor with the functor
// ============================================================================
LoKi::Particles::DecayTreeFitterFun::DecayTreeFitterFun 
( const LoKi::PhysTypes::Func&                       fun         , 
  const bool                                         usePV       ,
  const std::vector<std::string>&                    constraints ,
  const LoKi::Particles::DecayTreeFitterFun::MASSES& masses      , 
  const IDecayTreeFit*                               fitter      ) 
  : LoKi::AuxFunBase ( std::tie (  fun , usePV , constraints , masses ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::AuxDTFBase ( constraints , masses , fitter ) 
  , m_fun   ( fun    ) 
  , m_usePV ( usePV  ) 
  , m_chi2  ( -1     ) 
  , m_bad   ( LoKi::Constants::NegativeInfinity ) 
{}
// ======================================================================
// constructor with the functor
// ============================================================================
LoKi::Particles::DecayTreeFitterFun::DecayTreeFitterFun 
( const LoKi::PhysTypes::Func&         fun         , 
  const bool                           usePV       ,
  const LoKi::AuxDTFBase&              base        )
  : LoKi::AuxFunBase ( std::tie (  fun , usePV , base ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::AuxDTFBase ( base   ) 
  , m_fun            ( fun    ) 
  , m_usePV          ( usePV  ) 
  , m_chi2           ( -1     ) 
  , m_bad            ( LoKi::Constants::NegativeInfinity ) 
{}
// ============================================================================
// constructor with the functor & chi2-cut 
// ============================================================================
LoKi::Particles::DecayTreeFitterFun::DecayTreeFitterFun 
( const LoKi::PhysTypes::Func&         fun        , 
  const bool                           usePV      , 
  const double                         maxChi2DoF , 
  const double                         bad        ,
  const IDecayTreeFit*                 fitter     ) 
  : LoKi::AuxFunBase ( std::tie (  fun , usePV , maxChi2DoF , bad ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::AuxDTFBase ( fitter ) 
  , m_fun   ( fun        )
  , m_usePV ( usePV      ) 
  , m_chi2  ( maxChi2DoF ) 
  , m_bad   ( bad        ) 
{}
// ============================================================================
// constructor with the functor & chi2-cut
// ============================================================================
LoKi::Particles::DecayTreeFitterFun::DecayTreeFitterFun 
( const LoKi::PhysTypes::Func&                       fun         , 
  const bool                                         usePV       ,
  const std::vector<std::string>&                    constraints ,
  const double                                       maxChi2DoF  , 
  const double                                       bad         ,
  const LoKi::Particles::DecayTreeFitterFun::MASSES& masses      , 
  const IDecayTreeFit*                               fitter      ) 
  : LoKi::AuxFunBase ( std::tie (  fun , usePV , constraints , 
                                   maxChi2DoF , bad , masses ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::AuxDTFBase ( constraints , masses , fitter ) 
  , m_fun   ( fun    ) 
  , m_usePV ( usePV  ) 
  , m_chi2  ( maxChi2DoF ) 
  , m_bad   ( bad        )
{}
// ======================================================================
// constructor with the functor & chi2-cut 
// ============================================================================
LoKi::Particles::DecayTreeFitterFun::DecayTreeFitterFun 
( const LoKi::PhysTypes::Func&                       fun         , 
  const bool                                         usePV       ,
  const std::string&                                 constraint  ,
  const double                                       maxChi2DoF  , 
  const double                                       bad         ,
  const LoKi::Particles::DecayTreeFitterFun::MASSES& masses      , 
  const IDecayTreeFit*                               fitter      )
  : LoKi::AuxFunBase ( std::tie (  fun , usePV , constraint  , 
                                   maxChi2DoF , bad , masses ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::AuxDTFBase ( std::vector<std::string>(1,constraint) , masses , fitter ) 
  , m_fun   ( fun        ) 
  , m_usePV ( usePV      ) 
  , m_chi2  ( maxChi2DoF ) 
  , m_bad   ( bad        )
{}
// ======================================================================
// constructor with the functor & chi2-cut 
// ============================================================================
LoKi::Particles::DecayTreeFitterFun::DecayTreeFitterFun 
( const LoKi::PhysTypes::Func&         fun         , 
  const bool                           usePV       ,
  const double                         maxChi2DoF  , 
  const double                         bad         ,
  const LoKi::AuxDTFBase&              base        )
  : LoKi::AuxFunBase ( std::tie (  fun , usePV , maxChi2DoF , bad , base  ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::AuxDTFBase ( base   ) 
  , m_fun   ( fun        ) 
  , m_usePV ( usePV      ) 
  , m_chi2  ( maxChi2DoF ) 
  , m_bad   ( bad        )
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
    Error ( "LHCb::Particle* points to NULL, return 'bad'" ) ;
    return m_bad ;
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
    Warning ( "Error from IDecayTreeFit" , sc ) ;
    return m_bad ;
  }
  //
  // check fit chi2/nDoF if needed 
  //
  if ( 0 < m_chi2  && ( m_chi2 * _fitter->nDoF() <= _fitter->chi2() ) )
  { return m_bad ; }                                                 // RETURN 
  //
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
  s << " " << m_fun << " ," << ( m_usePV ? " True " : " False " ) ; 
  //
  printConstraints ( s ) ;
  //
  if ( 0 < m_chi2 || LoKi::Constants::NegativeInfinity != m_bad ) 
  { s << "," << m_chi2 << "," << m_bad ; }
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
  : LoKi::AuxFunBase ( std::tie (  fun , usePV ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , LoKi::AuxDTFBase ( fitter ) 
  , m_fun         ( fun    ) 
  , m_usePV       ( usePV  ) 
  , m_chi2        ( -1     ) 
{}
// ============================================================================
// constructor with the functor
// ============================================================================
LoKi::Particles::DecayTreeFitterCut::DecayTreeFitterCut
( const LoKi::PhysTypes::Cuts&                       fun         , 
  const bool                                         usePV       ,
  const std::vector<std::string>&                    constraints ,
  const LoKi::Particles::DecayTreeFitterCut::MASSES& masses      , 
  const IDecayTreeFit*                               fitter      ) 
  : LoKi::AuxFunBase ( std::tie (  fun , usePV , constraints , masses ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , LoKi::AuxDTFBase ( constraints , masses , fitter ) 
  , m_fun         ( fun    ) 
  , m_usePV       ( usePV  ) 
  , m_chi2        ( -1     ) 
{}
// ======================================================================
// constructor with the functor
// ============================================================================
LoKi::Particles::DecayTreeFitterCut::DecayTreeFitterCut
( const LoKi::PhysTypes::Cuts&                       fun         , 
  const bool                                         usePV       ,
  const std::string&                                 constraint  ,
  const LoKi::Particles::DecayTreeFitterCut::MASSES& masses      , 
  const IDecayTreeFit*                               fitter      )
  : LoKi::AuxFunBase ( std::tie (  fun , usePV , constraint , masses ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , LoKi::AuxDTFBase ( std::vector<std::string>(1,constraint) , masses , fitter ) 
  , m_fun         ( fun    ) 
  , m_usePV       ( usePV  ) 
  , m_chi2        ( -1     ) 
{}
// ======================================================================
// constructor with the functor
// ============================================================================
LoKi::Particles::DecayTreeFitterCut::DecayTreeFitterCut
( const LoKi::PhysTypes::Cuts&         fun         , 
  const bool                           usePV       ,
  const LoKi::AuxDTFBase&              base        )
  : LoKi::AuxFunBase ( std::tie (  fun , usePV , base  ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , LoKi::AuxDTFBase ( base   ) 
  , m_fun            ( fun    ) 
  , m_usePV          ( usePV  ) 
  , m_chi2           ( -1     ) 
{}



// ============================================================================
// constructor with the functor & chi2-cut
// ============================================================================
LoKi::Particles::DecayTreeFitterCut::DecayTreeFitterCut
( const LoKi::PhysTypes::Cuts&         fun         , 
  const bool                           usePV       ,
  const double                         maxChi2DoF  , 
  const IDecayTreeFit*                 fitter      ) 
  : LoKi::AuxFunBase ( std::tie (  fun , usePV , maxChi2DoF  ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , LoKi::AuxDTFBase ( fitter ) 
  , m_fun         ( fun        ) 
  , m_usePV       ( usePV      ) 
  , m_chi2        ( maxChi2DoF )
{}
// ============================================================================
// constructor with the functor & chi2-cut
// ============================================================================
LoKi::Particles::DecayTreeFitterCut::DecayTreeFitterCut
( const LoKi::PhysTypes::Cuts&                       fun         , 
  const bool                                         usePV       ,
  const std::vector<std::string>&                    constraints ,
  const double                                       maxChi2DoF  , 
  const LoKi::Particles::DecayTreeFitterCut::MASSES& masses      , 
  const IDecayTreeFit*                               fitter      ) 
  : LoKi::AuxFunBase ( std::tie (  fun , usePV , constraints , 
                                   maxChi2DoF , masses ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , LoKi::AuxDTFBase ( constraints , masses , fitter ) 
  , m_fun         ( fun        ) 
  , m_usePV       ( usePV      ) 
  , m_chi2        ( maxChi2DoF )
{}
// ======================================================================
// constructor with the functor
// ============================================================================
LoKi::Particles::DecayTreeFitterCut::DecayTreeFitterCut
( const LoKi::PhysTypes::Cuts&                       fun         , 
  const bool                                         usePV       ,
  const std::string&                                 constraint  ,
  const double                                       maxChi2DoF  , 
  const LoKi::Particles::DecayTreeFitterCut::MASSES& masses      , 
  const IDecayTreeFit*                               fitter      )
  : LoKi::AuxFunBase ( std::tie (  fun , usePV , constraint , 
                                   maxChi2DoF  , masses ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , LoKi::AuxDTFBase ( std::vector<std::string>(1,constraint) , masses , fitter ) 
  , m_fun         ( fun        ) 
  , m_usePV       ( usePV      ) 
  , m_chi2        ( maxChi2DoF )
{}
// ======================================================================
// constructor with the functor
// ============================================================================
LoKi::Particles::DecayTreeFitterCut::DecayTreeFitterCut
( const LoKi::PhysTypes::Cuts&         fun         , 
  const bool                           usePV       ,
  const double                         maxChi2DoF  , 
  const LoKi::AuxDTFBase&              base        )
  : LoKi::AuxFunBase ( std::tie (  fun , usePV , 
                                   maxChi2DoF  , base ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , LoKi::AuxDTFBase ( base       ) 
  , m_fun            ( fun        ) 
  , m_usePV          ( usePV      ) 
  , m_chi2           ( maxChi2DoF )
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
    Error ( "LHCb::Particle* points to NULL, return false" ) ;
    return false ;
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
    Warning ( "Error from IDecayTreeFit", sc, 3 ) ;
    return false ;
  }
  //
  // check fit chi2/nDoF if needed 
  //
  if ( 0 < m_chi2  && ( m_chi2 * _fitter->nDoF() <= _fitter->chi2() ) ) 
  { return false ; }                                                 // RETURN 
  //
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
  s << " DTF_CUT(" ;
  //
  s << " " << m_fun << " ," << ( usePV() ? " True " : " False " ) ; 
  //
  printConstraints ( s ) ;
  // 
  if ( 0 < m_chi2 ) {  s << "," << m_chi2 ; }
  //
  return s << " ) " ;
}
// ============================================================================



// ============================================================================
// constructor from child selector & PV-flag & constraints 
// ============================================================================
LoKi::Particles::ChildCTau::ChildCTau
( const LoKi::Child::Selector&              child       , 
  const bool                                usePV       , 
  const std::vector<std::string>&           constraints , 
  const LoKi::Particles::ChildCTau::MASSES& masses      , 
  const double                              chi2MaxDoF  , 
  const IDecayTreeFit*                      fitter      ) 
  : LoKi::AuxFunBase ( std::tie ( child , usePV , constraints , masses , chi2MaxDoF) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , LoKi::AuxDTFBase ( constraints , masses , fitter ) 
  , m_child ( child      ) 
  , m_usePV ( usePV      ) 
  , m_chi2  ( chi2MaxDoF )
{}
// ============================================================================
// constructor from child selector & PV-flag & constraints 
// ============================================================================
LoKi::Particles::ChildCTau::ChildCTau
( const std::vector<unsigned int>&          child       , 
  const bool                                usePV       , 
  const std::vector<std::string>&           constraints ,
  const LoKi::Particles::ChildCTau::MASSES& masses      , 
  const double                              chi2MaxDoF  , 
  const IDecayTreeFit*                      fitter      ) 
  : LoKi::AuxFunBase ( std::tie ( child  , usePV , constraints , masses , chi2MaxDoF) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , LoKi::AuxDTFBase ( constraints , masses , fitter ) 
  , m_child ( child ) 
  , m_usePV ( usePV ) 
  , m_chi2  ( chi2MaxDoF )
{}
// ============================================================================
// constructor from child selector & PV-flag & constraints 
// ============================================================================
LoKi::Particles::ChildCTau::ChildCTau
( const unsigned int                        child       , 
  const bool                                usePV       , 
  const std::vector<std::string>&           constraints ,
  const LoKi::Particles::ChildCTau::MASSES& masses      , 
  const double                              chi2MaxDoF  , 
  const IDecayTreeFit*                      fitter      ) 
  : LoKi::AuxFunBase ( std::tie ( child  , usePV , constraints , masses , chi2MaxDoF) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , LoKi::AuxDTFBase ( constraints , masses , fitter ) 
  , m_child ( child ) 
  , m_usePV ( usePV ) 
  , m_chi2  ( chi2MaxDoF )
{}
// ============================================================================
// constructor from child selector & PV-flag & constraints 
// ============================================================================
LoKi::Particles::ChildCTau::ChildCTau
( const std::string&                        child       , 
  const bool                                usePV       , 
  const std::vector<std::string>&           constraints ,
  const LoKi::Particles::ChildCTau::MASSES& masses      , 
  const double                              chi2MaxDoF  , 
  const IDecayTreeFit*                      fitter      ) 
  : LoKi::AuxFunBase ( std::tie ( child  , usePV , constraints , masses , chi2MaxDoF) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , LoKi::AuxDTFBase ( constraints , masses , fitter ) 
  , m_child ( child ) 
  , m_usePV ( usePV ) 
  , m_chi2  ( chi2MaxDoF )
{}
// ============================================================================
// constructor from child selector & PV-flag & constraints 
// ============================================================================
LoKi::Particles::ChildCTau::ChildCTau
( const Decays::iNode&                      child       , 
  const bool                                usePV       , 
  const std::vector<std::string>&           constraints ,
  const LoKi::Particles::ChildCTau::MASSES& masses      , 
  const double                              chi2MaxDoF  , 
  const IDecayTreeFit*                      fitter      ) 
  : LoKi::AuxFunBase ( std::tie ( child  , usePV , constraints , masses , chi2MaxDoF) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , LoKi::AuxDTFBase ( constraints , masses , fitter ) 
  , m_child ( child ) 
  , m_usePV ( usePV ) 
  , m_chi2  ( chi2MaxDoF )
{}
// ============================================================================
// constructor from child selector & PV-flag & constraints 
// ============================================================================
LoKi::Particles::ChildCTau::ChildCTau
( const Decays::IDecay::iTree&              child       , 
  const bool                                usePV       , 
  const std::vector<std::string>&           constraints ,
  const LoKi::Particles::ChildCTau::MASSES& masses      , 
  const double                              chi2MaxDoF  , 
  const IDecayTreeFit*                      fitter      ) 
  : LoKi::AuxFunBase ( std::tie ( child  , usePV , constraints , masses , chi2MaxDoF) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , LoKi::AuxDTFBase ( constraints , masses , fitter )
  , m_child ( child ) 
  , m_usePV ( usePV ) 
  , m_chi2  ( chi2MaxDoF )
{}
// ============================================================================
// constructor from child selector & PV-flag & constraints 
// ============================================================================
LoKi::Particles::ChildCTau::ChildCTau
( const Decays::IDecay::Finder&             child       , 
  const bool                                usePV       , 
  const std::vector<std::string>&           constraints ,
  const LoKi::Particles::ChildCTau::MASSES& masses      , 
  const double                              chi2MaxDoF  , 
  const IDecayTreeFit*                      fitter      ) 
  : LoKi::AuxFunBase ( std::tie ( child  , usePV , constraints , masses , chi2MaxDoF) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , LoKi::AuxDTFBase ( constraints , masses , fitter )
  , m_child ( child ) 
  , m_usePV ( usePV ) 
  , m_chi2  ( chi2MaxDoF )
{}
// ============================================================================
// constructor from child selector & PV-flag & constraints 
// ============================================================================
LoKi::Particles::ChildCTau::ChildCTau
( const LoKi::PhysTypes::Cuts&              child       , 
  const bool                                usePV       , 
  const std::vector<std::string>&           constraints ,
  const LoKi::Particles::ChildCTau::MASSES& masses      , 
  const double                              chi2MaxDoF  , 
  const IDecayTreeFit*                      fitter      ) 
  : LoKi::AuxFunBase ( std::tie ( child  , usePV , constraints , masses , chi2MaxDoF) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , LoKi::AuxDTFBase ( constraints , masses , fitter )  
  , m_child ( child ) 
  , m_usePV ( usePV ) 
  , m_chi2  ( chi2MaxDoF )
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
  if ( !p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return InvalidDistance" );
    return LoKi::Constants::InvalidDistance ; 
  }
  //
  auto fitted  = params ( p ) ;
  if ( !fitted ) { return LoKi::Constants::InvalidDistance ; }
  //
  return fitted->ctau().value() ;
} 
// ============================================================================
// get the fitted parameters
// ============================================================================
const IDecayTreeFit::Fitted*
LoKi::Particles::ChildCTau::params ( const LHCb::Particle* p ) const 
{
  if ( !p ) { return nullptr ; }                                      // REUTRN 
  //
  const LHCb::Particle* c = child( p ) ;
  if ( !c ) 
  {
    Warning ( "Unable to select the proper child particle ") ;
    return nullptr ;                                                       // RETURN 
  }
  //
  IDecayTreeFit* _fitter = fitter() ;
  //
  const LHCb::VertexBase* vertex = nullptr ;
  if ( usePV() ) 
  {
    vertex = bestVertex ( p ) ;
    if ( !vertex ) 
    { Warning ( "``Best vertex'' points zero, constraints will be disabled!" ) ; } 
  }
  //
  // apply local mass constrains
  applyConstraints () ;
  //
  StatusCode sc = _fitter->fit ( p , vertex ) ;
  if ( sc.isFailure() ) 
  {
    Warning ( "Error from IDecayTreeFit" , sc ) ;
    return nullptr ;                                                       // RETURN
  }
  //
  // apply chi2/nDoF cut if needed
  //
  if ( 0 < m_chi2 && ( m_chi2 * _fitter->nDoF() <= _fitter -> chi2() ) ) 
  { return nullptr ; }                                                 // RETURN 
  //
  const IDecayTreeFit::Fitted* f = _fitter->fitted ( c ) ; 
  if ( !f ) { Warning ( "Gaudi::Math::ParticleParams points to NULL" ); }
  //
  return f ;
}
// =============================================================================
// OPTIONAL: nice printout
// =============================================================================
std::ostream& 
LoKi::Particles::ChildCTau::fillStream ( std::ostream& s ) const 
{
  s << " DTF_CTAU( " 
    << child ()
    << " , " <<  ( usePV() ? " True " : " False " ) ;
  //
  printConstraints ( s ) ;
  //
  if ( 0 < maxChi2() ) { s << "," << maxChi2() ; }
  //
  return s << " ) " ;
}




// ============================================================================
// constructor from child selector & PV-flag & constraints 
// ============================================================================
LoKi::Particles::ChildCTauErr::ChildCTauErr
( const LoKi::Child::Selector&              child       , 
  const bool                                usePV       , 
  const std::vector<std::string>&           constraints ,
  const LoKi::Particles::ChildCTau::MASSES& masses      , 
  const double                              chi2MaxDoF  , 
  const IDecayTreeFit*                      fitter      ) 
  : LoKi::AuxFunBase ( std::tie ( child  , usePV , constraints , masses , chi2MaxDoF) ) 
  , LoKi::Particles::ChildCTau ( child       , 
                                 usePV       , 
                                 constraints , 
                                 masses      ,
                                 chi2MaxDoF  , 
                                 fitter      ) 
{}
// ============================================================================
// constructor from child selector & PV-flag & constraints 
// ============================================================================
LoKi::Particles::ChildCTauErr::ChildCTauErr
( const std::vector<unsigned int>&          child       , 
  const bool                                usePV       , 
  const std::vector<std::string>&           constraints ,
  const LoKi::Particles::ChildCTau::MASSES& masses      , 
  const double                              chi2MaxDoF  , 
  const IDecayTreeFit*                      fitter      ) 
  : LoKi::AuxFunBase ( std::tie ( child  , usePV , constraints , masses , chi2MaxDoF) ) 
  , LoKi::Particles::ChildCTau ( child       , 
                                 usePV       , 
                                 constraints , 
                                 masses      , 
                                 chi2MaxDoF  , 
                                 fitter      ) 
{}
// ============================================================================
// constructor from child selector & PV-flag & constraints 
// ============================================================================
LoKi::Particles::ChildCTauErr::ChildCTauErr
( const unsigned int                        child       , 
  const bool                                usePV       , 
  const std::vector<std::string>&           constraints ,
  const LoKi::Particles::ChildCTau::MASSES& masses      , 
  const double                              chi2MaxDoF  , 
  const IDecayTreeFit*                      fitter      ) 
  : LoKi::AuxFunBase ( std::tie ( child  , usePV , constraints , masses , chi2MaxDoF) ) 
  , LoKi::Particles::ChildCTau ( child       , 
                                 usePV       , 
                                 constraints , 
                                 masses      , 
                                 chi2MaxDoF  , 
                                 fitter      ) 
{}
// ============================================================================
// constructor from child selector & PV-flag & constraints 
// ============================================================================
LoKi::Particles::ChildCTauErr::ChildCTauErr
( const std::string&                        child       , 
  const bool                                usePV       , 
  const std::vector<std::string>&           constraints ,
  const LoKi::Particles::ChildCTau::MASSES& masses      , 
  const double                              chi2MaxDoF  , 
  const IDecayTreeFit*                      fitter      ) 
  : LoKi::AuxFunBase ( std::tie ( child  , usePV , constraints , masses , chi2MaxDoF) ) 
  , LoKi::Particles::ChildCTau ( child       , 
                                 usePV       , 
                                 constraints , 
                                 masses      , 
                                 chi2MaxDoF  , 
                                 fitter      ) 
{}
// ============================================================================
// constructor from child selector & PV-flag & constraints 
// ============================================================================
LoKi::Particles::ChildCTauErr::ChildCTauErr
( const Decays::iNode&                      child       , 
  const bool                                usePV       , 
  const std::vector<std::string>&           constraints ,
  const LoKi::Particles::ChildCTau::MASSES& masses      , 
  const double                              chi2MaxDoF  , 
  const IDecayTreeFit*                      fitter      ) 
  : LoKi::AuxFunBase ( std::tie ( child  , usePV , constraints , masses , chi2MaxDoF) ) 
  , LoKi::Particles::ChildCTau ( child       , 
                                 usePV       , 
                                 constraints , 
                                 masses      , 
                                 chi2MaxDoF  , 
                                 fitter      ) 
{}
// ============================================================================
// constructor from child selector & PV-flag & constraints 
// ============================================================================
LoKi::Particles::ChildCTauErr::ChildCTauErr
( const Decays::IDecay::iTree&              child       , 
  const bool                                usePV       , 
  const std::vector<std::string>&           constraints ,
  const LoKi::Particles::ChildCTau::MASSES& masses      , 
  const double                              chi2MaxDoF  , 
  const IDecayTreeFit*                      fitter      ) 
  : LoKi::AuxFunBase ( std::tie ( child  , usePV , constraints , masses , chi2MaxDoF) ) 
  , LoKi::Particles::ChildCTau ( child       , 
                                 usePV       , 
                                 constraints , 
                                 masses      , 
                                 chi2MaxDoF  , 
                                 fitter      ) 
{}
// ============================================================================
// constructor from child selector & PV-flag & constraints 
// ============================================================================
LoKi::Particles::ChildCTauErr::ChildCTauErr
( const Decays::IDecay::Finder&             child       , 
  const bool                                usePV       , 
  const std::vector<std::string>&           constraints ,
  const LoKi::Particles::ChildCTau::MASSES& masses      , 
  const double                              chi2MaxDoF  , 
  const IDecayTreeFit*                      fitter      ) 
  : LoKi::AuxFunBase ( std::tie ( child  , usePV , constraints , masses , chi2MaxDoF) ) 
  , LoKi::Particles::ChildCTau ( child       , 
                                 usePV       , 
                                 constraints , 
                                 masses      , 
                                 chi2MaxDoF  , 
                                 fitter      ) 
{}
// ============================================================================
// constructor from child selector & PV-flag & constraints 
// ============================================================================
LoKi::Particles::ChildCTauErr::ChildCTauErr
( const LoKi::PhysTypes::Cuts&              child       , 
  const bool                                usePV       , 
  const std::vector<std::string>&           constraints ,
  const LoKi::Particles::ChildCTau::MASSES& masses      , 
  const double                              chi2MaxDoF  , 
  const IDecayTreeFit*                      fitter      ) 
  : LoKi::AuxFunBase ( std::tie ( child  , usePV , constraints , masses , chi2MaxDoF) ) 
  , LoKi::Particles::ChildCTau ( child       , 
                                 usePV       , 
                                 constraints , 
                                 masses      , 
                                 chi2MaxDoF  , 
                                 fitter      ) 
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
    Error ( "LHCb::Particle* points to NULL, return InvalidDistance" );
    return LoKi::Constants::InvalidDistance ; 
  }
  //
  const IDecayTreeFit::Fitted* fitted  = params ( p ) ;
  if ( 0 == fitted ) { return LoKi::Constants::InvalidDistance ; }
  //
  return fitted->ctau().error() ;
} 
// =============================================================================
// OPTIONAL: nice printout
// =============================================================================
std::ostream& 
LoKi::Particles::ChildCTauErr::fillStream ( std::ostream& s ) const 
{
  s << " DTF_CTAUERR( " 
    << child () 
    << " , " <<  ( usePV() ? " True " : " False " ) ;
  //
  printConstraints ( s ) ;
  //
  if ( 0 < maxChi2() ) { s << "," << maxChi2() ; }
  //
  return s << " ) " ;
}
// =============================================================================


// ============================================================================
// constructor from child selector & PV-flag & constraints 
// ============================================================================
LoKi::Particles::ChildCTauSignificance::ChildCTauSignificance
( const LoKi::Child::Selector&              child       , 
  const bool                                usePV       , 
  const std::vector<std::string>&           constraints ,
  const LoKi::Particles::ChildCTau::MASSES& masses      , 
  const double                              chi2MaxDoF  , 
  const IDecayTreeFit*                      fitter      ) 
  : LoKi::AuxFunBase ( std::tie ( child  , usePV , constraints , masses , chi2MaxDoF) ) 
  , LoKi::Particles::ChildCTauErr ( child       , 
                                    usePV       , 
                                    constraints , 
                                    masses      ,
                                    chi2MaxDoF  , 
                                    fitter      ) 
{}
// ============================================================================
// constructor from child selector & PV-flag & constraints 
// ============================================================================
LoKi::Particles::ChildCTauSignificance::ChildCTauSignificance
( const std::vector<unsigned int>&          child       , 
  const bool                                usePV       , 
  const std::vector<std::string>&           constraints ,
  const LoKi::Particles::ChildCTau::MASSES& masses      , 
  const double                              chi2MaxDoF  , 
  const IDecayTreeFit*                      fitter      ) 
  : LoKi::AuxFunBase ( std::tie ( child  , usePV , constraints , masses , chi2MaxDoF) ) 
  , LoKi::Particles::ChildCTauErr ( child       , 
                                    usePV       , 
                                    constraints , 
                                    masses      ,
                                    chi2MaxDoF  , 
                                    fitter      ) 
{}
// ============================================================================
// constructor from child selector & PV-flag & constraints 
// ============================================================================
LoKi::Particles::ChildCTauSignificance::ChildCTauSignificance
( const unsigned int                        child       , 
  const bool                                usePV       , 
  const std::vector<std::string>&           constraints ,
  const LoKi::Particles::ChildCTau::MASSES& masses      , 
  const double                              chi2MaxDoF  , 
  const IDecayTreeFit*                      fitter      ) 
  : LoKi::AuxFunBase ( std::tie ( child  , usePV , constraints , masses , chi2MaxDoF) ) 
  , LoKi::Particles::ChildCTauErr ( child       , 
                                    usePV       , 
                                    constraints , 
                                    masses      ,
                                    chi2MaxDoF  , 
                                    fitter      ) 
{}
// ============================================================================
// constructor from child selector & PV-flag & constraints 
// ============================================================================
LoKi::Particles::ChildCTauSignificance::ChildCTauSignificance
( const std::string&                        child       , 
  const bool                                usePV       , 
  const std::vector<std::string>&           constraints ,
  const LoKi::Particles::ChildCTau::MASSES& masses      , 
  const double                              chi2MaxDoF  , 
  const IDecayTreeFit*                      fitter      ) 
  : LoKi::AuxFunBase ( std::tie ( child  , usePV , constraints , masses , chi2MaxDoF) ) 
  , LoKi::Particles::ChildCTauErr ( child       , 
                                    usePV       , 
                                    constraints , 
                                    masses      ,
                                    chi2MaxDoF  , 
                                    fitter      ) 
{}
// ============================================================================
// constructor from child selector & PV-flag & constraints 
// ============================================================================
LoKi::Particles::ChildCTauSignificance::ChildCTauSignificance
( const Decays::iNode&             child       , 
  const bool                       usePV       , 
  const std::vector<std::string>&  constraints ,
  const LoKi::Particles::ChildCTau::MASSES& masses      , 
  const double                     chi2MaxDoF  , 
  const IDecayTreeFit*             fitter      ) 
  : LoKi::AuxFunBase ( std::tie ( child  , usePV , constraints , masses , chi2MaxDoF) ) 
  , LoKi::Particles::ChildCTauErr ( child       , 
                                    usePV       , 
                                    constraints , 
                                    masses      ,
                                    chi2MaxDoF  , 
                                    fitter      ) 
{}
// ============================================================================
// constructor from child selector & PV-flag & constraints 
// ============================================================================
LoKi::Particles::ChildCTauSignificance::ChildCTauSignificance
( const Decays::IDecay::iTree&     child       , 
  const bool                       usePV       , 
  const std::vector<std::string>&  constraints ,
  const LoKi::Particles::ChildCTau::MASSES& masses      , 
  const double                     chi2MaxDoF  , 
  const IDecayTreeFit*             fitter      ) 
  : LoKi::AuxFunBase ( std::tie ( child  , usePV , constraints , masses , chi2MaxDoF) ) 
  , LoKi::Particles::ChildCTauErr ( child       , 
                                    usePV       , 
                                    constraints , 
                                    masses      ,
                                    chi2MaxDoF  , 
                                    fitter      ) 
{}
// ============================================================================
// constructor from child selector & PV-flag & constraints 
// ============================================================================
LoKi::Particles::ChildCTauSignificance::ChildCTauSignificance
( const Decays::IDecay::Finder&    child       , 
  const bool                       usePV       , 
  const std::vector<std::string>&  constraints ,
  const LoKi::Particles::ChildCTau::MASSES& masses      , 
  const double                     chi2MaxDoF  , 
  const IDecayTreeFit*             fitter      ) 
  : LoKi::AuxFunBase ( std::tie ( child  , usePV , constraints , masses , chi2MaxDoF) ) 
  , LoKi::Particles::ChildCTauErr ( child       , 
                                    usePV       , 
                                    constraints , 
                                    masses      ,
                                    chi2MaxDoF  , 
                                    fitter      ) 
{}
// ============================================================================
// constructor from child selector & PV-flag & constraints 
// ============================================================================
LoKi::Particles::ChildCTauSignificance::ChildCTauSignificance
( const LoKi::PhysTypes::Cuts&     child       , 
  const bool                       usePV       , 
  const std::vector<std::string>&  constraints ,
  const LoKi::Particles::ChildCTau::MASSES& masses      , 
  const double                     chi2MaxDoF  , 
  const IDecayTreeFit*             fitter      ) 
  : LoKi::AuxFunBase ( std::tie ( child  , usePV , constraints , masses , chi2MaxDoF) ) 
  , LoKi::Particles::ChildCTauErr ( child       , 
                                    usePV       , 
                                    constraints , 
                                    masses      ,
                                    chi2MaxDoF  , 
                                    fitter      ) 
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
    Error ( "LHCb::Particle* points to NULL, return NegativeInfinity" );
    return LoKi::Constants::NegativeInfinity ;                     // RETURN 
  }
  //
  const IDecayTreeFit::Fitted* fitted  = params ( p ) ;
  if ( 0 == fitted ) { return LoKi::Constants::NegativeInfinity ; } // RETURN 
  //
  const Gaudi::Math::ValueWithError ctau = fitted->ctau () ;
  if ( 0 >= ctau.error() ) 
  {
    Warning ( "Non-positive error in ctau, return NegativeInfinity" );
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
  s << " DTF_CTAUSIGNIFICANCE ( " 
    << child () 
    << " , " <<  ( usePV() ? " True " : " False " ) ;
  //
  printConstraints ( s ) ;
  //
  if ( 0 < maxChi2() ) { s << "," << maxChi2() ; }
  //
  return s << " ) " ;
}
// ============================================================================



// ============================================================================
// constructor from PV-flag
// ============================================================================
LoKi::Particles::DecayTreeFitChi2::DecayTreeFitChi2 
( const bool                                         usePV       , 
  const std::vector<std::string>&                    constraints ,
  const LoKi::Particles::DecayTreeFitterFun::MASSES& masses      ) 
  : LoKi::AuxFunBase ( std::tie ( usePV , constraints , masses ) ) 
  , LoKi::Particles::DecayTreeFitterFun
( LoKi::BasicFunctors<const LHCb::Particle*>::Constant( 1.0 ) , 
  usePV       ,
  constraints , 
  masses      )
{}
// ============================================================================
// constructor from PV-flag
// ============================================================================
LoKi::Particles::DecayTreeFitChi2::DecayTreeFitChi2 
( const bool                                         usePV      , 
  const std::string&                                 constraint ,
  const LoKi::Particles::DecayTreeFitterFun::MASSES& masses     ) 
  : LoKi::AuxFunBase ( std::tie ( usePV , constraint , masses ) ) 
  , LoKi::Particles::DecayTreeFitterFun 
( LoKi::BasicFunctors<const LHCb::Particle*>::Constant( 1.0 ) , 
  usePV       ,
  constraint  , 
  masses      )
{}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::Particles::DecayTreeFitChi2::~DecayTreeFitChi2 (){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::DecayTreeFitChi2*
LoKi::Particles::DecayTreeFitChi2::clone() const 
{ return new LoKi::Particles::DecayTreeFitChi2 ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::DecayTreeFitChi2::result_type 
LoKi::Particles::DecayTreeFitChi2::operator() 
  ( LoKi::Particles::DecayTreeFitChi2::argument p ) const { return chi2 ( p ) ; }
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& 
LoKi::Particles::DecayTreeFitChi2::fillStream ( std::ostream& s ) const 
{
  s << " DTF_CHI2(" << ( usePV() ? " True " : " False " )  ;
  //
  printConstraints ( s ) ;
  //
  return s << ") " ;
}
// ============================================================================
// get chi2 of decay tree fit 
// ============================================================================
double LoKi::Particles::DecayTreeFitChi2::chi2 ( const LHCb::Particle* p ) const 
{
  // 1. check argument
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return InvalidChi2" );
    return LoKi::Constants::InvalidChi2 ;                          // RETURN 
  }
  // 2. get primary vertex (if required) 
  const LHCb::VertexBase* vertex = 0 ;
  if ( usePV() ) 
  {
    vertex = bestVertex ( p ) ;
    if ( 0 == vertex ) 
    { Warning ( "``Best vertex'' points zero, constraits will be disabled!" ) ; } 
  }
  //
  // 3. apply local mass constrains (if required) 
  applyConstraints () ;
  //
  // 4. get fitter 
  IDecayTreeFit* _fitter = fitter() ;
  //
  StatusCode sc = _fitter->fit ( p , vertex ) ;
  if ( sc.isFailure() ) 
  {
    Warning ( "Error from IDecayTreeFit, return InvalidChi2" , sc ) ;
    return LoKi::Constants::InvalidChi2 ;
  }
  // get chi2 
  return _fitter->chi2 () ;
}
// ============================================================================
// get nDoF of decay tree fit 
// ============================================================================
unsigned int LoKi::Particles::DecayTreeFitChi2::nDoF 
( const LHCb::Particle* p ) const 
{
  // 1. check argument
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return 0" );
    return 0 ;                                                        // RETURN 
  }
  // 2. get primary vertex (if required) 
  const LHCb::VertexBase* vertex = 0 ;
  if ( usePV() ) 
  {
    vertex = bestVertex ( p ) ;
    if ( 0 == vertex ) 
    { Warning ( "``Best vertex'' points zero, constraits will be disabled!" ) ; } 
  }
  //
  // 3. apply local mass constrains (if required) 
  applyConstraints () ;
  //
  // 4. get fitter 
  IDecayTreeFit* _fitter = fitter() ;
  //
  StatusCode sc = _fitter->fit ( p , vertex ) ;
  if ( sc.isFailure() ) 
  {
    Warning ( "Error from IDecayTreeFit, return 0 " , sc, 3 ) ;
    return 0  ;
  }
  // get nDoF 
  return _fitter->nDoF () ;
}
// ============================================================================
// get chi2/DoF of decay tree fit 
// ============================================================================
double LoKi::Particles::DecayTreeFitChi2::chi2PerDoF 
( const LHCb::Particle* p ) const 
{
  // 1. check argument
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return InvalidChi2" );
    return LoKi::Constants::InvalidChi2 ;                          // RETURN 
  }
  // 2. get primary vertex (if required) 
  const LHCb::VertexBase* vertex = 0 ;
  if ( usePV() ) 
  {
    vertex = bestVertex ( p ) ;
    if ( 0 == vertex ) 
    { Warning ( "``Best vertex'' points zero, constraits will be disabled!" ) ; } 
  }
  //
  // 3. apply local mass constrains (if required) 
  applyConstraints () ;
  //
  // 4. get fitter 
  IDecayTreeFit* _fitter = fitter() ;
  //
  StatusCode sc = _fitter->fit ( p , vertex ) ;
  if ( sc.isFailure() ) 
  {
    Warning ( "Error from IDecayTreeFit, return InvalidChi2", sc, 3 ) ;
    return LoKi::Constants::InvalidChi2 ;
  }
  /// get chi2 
  const double       _chi2 = _fitter->chi2 () ;
  /// get nDof 
  const unsigned int _nDoF = _fitter->nDoF () ;
  ///
  if ( 0 == _nDoF ) 
  { 
    Warning ("chi2PerDoF: invalid nDoF, rerurn InvalidChi2") ;
    return LoKi::Constants::InvalidChi2 ;                         // RETURN 
  }
  if ( 0  > _chi2 ) 
  { 
    Warning ("chi2PerDoF: invalid chi2, rerurn InvalidChi2") ;
    return LoKi::Constants::InvalidChi2 ;                         // RETURN 
  }
  //
  return _chi2/_nDoF ;
}
// ============================================================================
// get probablity of decay tree fit 
// ============================================================================
double LoKi::Particles::DecayTreeFitChi2::prob
( const LHCb::Particle* p ) const 
{
  // 1. check argument
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return NegativeInfinity" );
    return LoKi::Constants::NegativeInfinity ;                        // RETURN 
  }
  // 2. get primary vertex (if required) 
  const LHCb::VertexBase* vertex = 0 ;
  if ( usePV() ) 
  {
    vertex = bestVertex ( p ) ;
    if ( 0 == vertex ) 
    { Warning ( "``Best vertex'' points zero, constraits will be disabled!" ) ; } 
  }
  //
  // 3. apply local mass constrains (if required) 
  applyConstraints () ;
  //
  // 4. get fitter 
  IDecayTreeFit* _fitter = fitter() ;
  //
  StatusCode sc = _fitter->fit ( p , vertex ) ;
  if ( sc.isFailure() ) 
  {
    Warning ( "prob: Error from IDecayTreeFit, return NegativeInfinity" , sc, 3 ) ;
    return LoKi::Constants::NegativeInfinity ;                        // RETURN 
  }
  /// get chi2 
  const double       _chi2 = _fitter->chi2 () ;
  /// get nDof 
  const unsigned int _nDoF = _fitter->nDoF () ;
  ///
  if ( 0 == _nDoF ) 
  { 
    Warning ("prob: invalid nDoF, return NegativeInfinity") ;
    return LoKi::Constants::NegativeInfinity ;                        // RETURN 
  }
  if ( 0  > _chi2 ) 
  { 
    Warning ( "prob: invalid chi2, return NegativeInfiinty") ;
    return LoKi::Constants::NegativeInfinity ;                        // RETURN 
  }
  // use GSL
  return gsl_cdf_chisq_Q ( _chi2 , _nDoF ) ;
}
// ============================================================================



// ============================================================================
// constructor from PV-flag
// ============================================================================
LoKi::Particles::DecayTreeFitNDoF::DecayTreeFitNDoF
( const bool                                         usePV       , 
  const std::vector<std::string>&                    constraints ,
  const LoKi::Particles::DecayTreeFitterFun::MASSES& masses      ) 
  : LoKi::AuxFunBase ( std::tie ( usePV , constraints , masses ) ) 
  , LoKi::Particles::DecayTreeFitChi2 ( usePV , constraints , masses ) 
{}
// ============================================================================
// constructor from PV-flag
// ============================================================================
LoKi::Particles::DecayTreeFitNDoF::DecayTreeFitNDoF
( const bool                                         usePV       , 
  const std::string&                                 constraint  ,
  const LoKi::Particles::DecayTreeFitterFun::MASSES& masses      ) 
  : LoKi::AuxFunBase ( std::tie ( usePV , constraint , masses ) ) 
  , LoKi::Particles::DecayTreeFitChi2 ( usePV , constraint , masses ) 
{}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::Particles::DecayTreeFitNDoF::~DecayTreeFitNDoF (){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::DecayTreeFitNDoF*
LoKi::Particles::DecayTreeFitNDoF::clone() const 
{ return new LoKi::Particles::DecayTreeFitNDoF ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::DecayTreeFitNDoF::result_type 
LoKi::Particles::DecayTreeFitNDoF::operator() 
  ( LoKi::Particles::DecayTreeFitNDoF::argument p ) const { return nDoF ( p ) ; }
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& 
LoKi::Particles::DecayTreeFitNDoF::fillStream ( std::ostream& s ) const 
{
  s << " DTF_NDOF(" << ( usePV() ? " True " : " False " )  ;
  //
  printConstraints ( s ) ;
  //
  return s << ") " ;
}



// ============================================================================
// constructor from PV-flag
// ============================================================================
LoKi::Particles::DecayTreeFitChi2NDoF::DecayTreeFitChi2NDoF
( const bool                                         usePV       , 
  const std::vector<std::string>&                    constraints , 
  const LoKi::Particles::DecayTreeFitterFun::MASSES& masses      )
  : LoKi::AuxFunBase ( std::tie ( usePV , constraints , masses ) ) 
  , LoKi::Particles::DecayTreeFitNDoF ( usePV , constraints , masses ) 
{}
// ============================================================================
// constructor from PV-flag
// ============================================================================
LoKi::Particles::DecayTreeFitChi2NDoF::DecayTreeFitChi2NDoF
( const bool                                         usePV      , 
  const std::string&                                 constraint ,
  const LoKi::Particles::DecayTreeFitterFun::MASSES& masses      )
  : LoKi::AuxFunBase ( std::tie ( usePV , constraint , masses ) ) 
  , LoKi::Particles::DecayTreeFitNDoF ( usePV , constraint , masses ) 
{}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::Particles::DecayTreeFitChi2NDoF::~DecayTreeFitChi2NDoF (){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::DecayTreeFitChi2NDoF*
LoKi::Particles::DecayTreeFitChi2NDoF::clone() const 
{ return new LoKi::Particles::DecayTreeFitChi2NDoF ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::DecayTreeFitChi2NDoF::result_type 
LoKi::Particles::DecayTreeFitChi2NDoF::operator() 
  ( LoKi::Particles::DecayTreeFitChi2NDoF::argument p ) const 
{ return chi2PerDoF ( p ) ; }
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& 
LoKi::Particles::DecayTreeFitChi2NDoF::fillStream ( std::ostream& s ) const 
{
  s << " DTF_CHI2NDOF(" << ( usePV() ? " True " : " False " )  ;
  //
  printConstraints ( s ) ;
  //
  return s << ") " ;
}

// ============================================================================
// constructor from PV-flag
// ============================================================================
LoKi::Particles::DecayTreeFitProb::DecayTreeFitProb
( const bool                      usePV       , 
  const std::vector<std::string>& constraints ) 
  : LoKi::AuxFunBase ( std::tie ( usePV , constraints ) ) 
  , LoKi::Particles::DecayTreeFitChi2NDoF ( usePV , constraints ) 
{}
// ============================================================================
// constructor from PV-flag
// ============================================================================
LoKi::Particles::DecayTreeFitProb::DecayTreeFitProb
( const bool         usePV      , 
  const std::string& constraint ) 
  : LoKi::AuxFunBase ( std::tie ( usePV , constraint ) ) 
  , LoKi::Particles::DecayTreeFitChi2NDoF ( usePV , constraint ) 
{}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::Particles::DecayTreeFitProb::~DecayTreeFitProb (){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::DecayTreeFitProb*
LoKi::Particles::DecayTreeFitProb::clone() const 
{ return new LoKi::Particles::DecayTreeFitProb ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::DecayTreeFitProb::result_type 
LoKi::Particles::DecayTreeFitProb::operator() 
  ( LoKi::Particles::DecayTreeFitProb::argument p ) const 
{ return prob ( p ) ; }
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& 
LoKi::Particles::DecayTreeFitProb::fillStream ( std::ostream& s ) const 
{
  s << " DTF_PROB(" << ( usePV() ? " True " : " False " )  ;
  //
  printConstraints ( s ) ;
  //
  return s << ") " ;
}


// ============================================================================
// The END 
// ============================================================================
