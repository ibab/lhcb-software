// $Id: GenMoniDicts.cpp,v 1.1 2007-07-23 17:23:37 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Monitoring.h"
// ============================================================================
#include "LoKi/GenTypes.h"
#include "LoKi/GenMoniDicts.h"
// ========================================================================

// ========================================================================
LoKi::PredicateFromPredicate<const HepMC::GenParticle*>
LoKi::Dicts::monitor
( const LoKi::Predicate<const HepMC::GenParticle*>& cut  , 
  StatEntity*                                       stat ) 
{ return LoKi::monitor ( cut , stat ) ; }
// ========================================================================
LoKi::FunctionFromFunction<const HepMC::GenParticle*>
LoKi::Dicts::monitor
( const LoKi::Function<const HepMC::GenParticle*>&  fun  , 
  StatEntity*                                       stat ) 
{ return LoKi::monitor ( fun , stat ) ; }
// ========================================================================
LoKi::FunctionFromFunction<const HepMC::GenParticle*>
LoKi::Dicts::monitor
( const LoKi::Function<const HepMC::GenParticle*>&  fun  , 
  AIDA::IHistogram1D*                               hist ) 
{ return LoKi::monitor ( fun , hist ) ; }
// ========================================================================
LoKi::PredicateFromPredicate<const HepMC::GenVertex*>
LoKi::Dicts::monitor
( const LoKi::Predicate<const HepMC::GenVertex*>&   cut  , 
  StatEntity*                                       stat ) 
{ return LoKi::monitor ( cut , stat ) ; }
// ========================================================================
LoKi::FunctionFromFunction<const HepMC::GenVertex*>
LoKi::Dicts::monitor
( const LoKi::Function<const HepMC::GenVertex*>&    fun  , 
  StatEntity*                                       stat ) 
{ return LoKi::monitor ( fun , stat ) ; }
// ========================================================================
LoKi::FunctionFromFunction<const HepMC::GenVertex*>
LoKi::Dicts::monitor
( const LoKi::Function<const HepMC::GenVertex*>&    fun  , 
  AIDA::IHistogram1D*                               hist ) 
{ return LoKi::monitor ( fun , hist ) ; }
// ========================================================================    

// ============================================================================
// The END 
// ============================================================================
