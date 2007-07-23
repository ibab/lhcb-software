// $Id: MCMoniDicts.cpp,v 1.1 2007-07-23 17:27:31 ibelyaev Exp $
// Include files 
// $Id: MCMoniDicts.cpp,v 1.1 2007-07-23 17:27:31 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Monitoring.h"
// ============================================================================
#include "LoKi/MCTypes.h"
#include "LoKi/MCMoniDicts.h"
// ========================================================================

// ========================================================================
LoKi::PredicateFromPredicate<const LHCb::MCParticle*>
LoKi::Dicts::monitor
( const LoKi::Predicate<const LHCb::MCParticle*>&   cut  , 
  StatEntity*                                       stat ) 
{ return LoKi::monitor ( cut , stat ) ; }
// ========================================================================
LoKi::FunctionFromFunction<const LHCb::MCParticle*>
LoKi::Dicts::monitor
( const LoKi::Function<const LHCb::MCParticle*>&    fun  , 
  StatEntity*                                       stat ) 
{ return LoKi::monitor ( fun , stat ) ; }
// ========================================================================
LoKi::FunctionFromFunction<const LHCb::MCParticle*>
LoKi::Dicts::monitor
( const LoKi::Function<const LHCb::MCParticle*>&    fun  , 
  AIDA::IHistogram1D*                               hist ) 
{ return LoKi::monitor ( fun , hist ) ; }
// ========================================================================
LoKi::PredicateFromPredicate<const LHCb::MCVertex*>
LoKi::Dicts::monitor
( const LoKi::Predicate<const LHCb::MCVertex*>&     cut  , 
  StatEntity*                                       stat ) 
{ return LoKi::monitor ( cut , stat ) ; }
// ========================================================================
LoKi::FunctionFromFunction<const LHCb::MCVertex*>
LoKi::Dicts::monitor
( const LoKi::Function<const LHCb::MCVertex*>&      fun  , 
  StatEntity*                                       stat ) 
{ return LoKi::monitor ( fun , stat ) ; }
// ========================================================================
LoKi::FunctionFromFunction<const LHCb::MCVertex*>
LoKi::Dicts::monitor
( const LoKi::Function<const LHCb::MCVertex*>&      fun  , 
  AIDA::IHistogram1D*                               hist ) 
{ return LoKi::monitor ( fun , hist ) ; }
// ========================================================================    

// ============================================================================
// The END 
// ============================================================================
