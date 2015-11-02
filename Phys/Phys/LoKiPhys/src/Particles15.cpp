// $Id$
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IAlgContextSvc.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/SmartIF.h"
// ============================================================================
// GaudiAlg 
// ============================================================================
#include "GaudiAlg/GetAlgs.h"
#include "GaudiAlg/GaudiAlgorithm.h"
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/IParticleFilter.h"
// ============================================================================
// LoKiPhys
// ============================================================================
#include "LoKi/Particles15.h"
// ============================================================================
/** @file
 *
 *  Implementation file for functions from namespace  LoKi::Particles
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-03-20
 */
// ============================================================================
// constructor from the filter name 
// ============================================================================
LoKi::Particles::Filter::Filter
( const std::string& filter ) 
  : LoKi::AuxFunBase ( std::tie ( filter ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , m_filter ()
{
  // 1. get from the context 
  LoKi::ILoKiSvc* loki  = lokiSvc() ;
  //
  SmartIF<IAlgContextSvc> context ( loki ) ;
  GaudiAlgorithm* alg = Gaudi::Utils::getGaudiAlg ( context ) ;
  if ( alg ) 
  { m_filter = alg->tool<IParticleFilter> ( filter , alg , true ) ; }
  else 
  { 
    /// 2. use tool service 
    SmartIF<IToolSvc> tsvc ( loki ) ;
    if ( ! ( ! tsvc ) ) 
    {
      IParticleFilter* _fltr  = nullptr ;
      const IInterface* parent = nullptr ;
      StatusCode sc = tsvc->retrieveTool 
        ( filter , _fltr , parent , true ) ;
      Assert ( sc.isSuccess() , "Unable to retrieve the tool " + filter , sc ) ;
      Assert ( _fltr     , "Unable to retrieve the tool " + filter      ) ;
      m_filter = _fltr ;
    }
  }
  //
  Assert ( m_filter , "Unable to locate tool " + filter ) ;
} 
// ============================================================================
// constructor from the filter 
// ============================================================================
LoKi::Particles::Filter::Filter
( const IParticleFilter* filter ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , m_filter ( filter )
{}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::Particles::Filter::~Filter()
{
  if ( m_filter && !gaudi() )
  {
    // Warning("Manually reset  IParticleFilter") ;
    m_filter.reset() ;
  }
}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::Filter*
LoKi::Particles::Filter::clone() const { return new Filter(*this); }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::Filter::result_type
LoKi::Particles::Filter::operator() 
  ( LoKi::Particles::Filter::argument p ) const 
{
  if ( !p ) 
  {
    Error ( "Invalid argument, return 'false'");
    return false ;                                                 // RETURN 
  }
  if ( !m_filter ) 
  {
    Error ( "Invalid tool: IParticleFilter* points to NULL, return 'false'");
    return false ;                                                 // RETURN 
  }
  // use tool
  return m_filter->operator()( p ) ;
} 
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream& 
LoKi::Particles::Filter::fillStream( std::ostream& s ) const 
{
  s << " FILTER( ";
  if ( m_filter ) 
  { s << m_filter->type() << "/" << m_filter->name() ; }
  else 
  { s << "<invalid>" ; }
  return s << " ) " ;
}
// ============================================================================

// ============================================================================
// The END
// ============================================================================

