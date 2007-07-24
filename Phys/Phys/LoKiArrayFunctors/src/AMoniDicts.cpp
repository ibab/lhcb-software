// $Id: AMoniDicts.cpp,v 1.1 2007-07-24 05:20:09 ibelyaev Exp $
// ============================================================================
// Include files
// ============================================================================
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Monitoring.h"
#include "LoKi/AMoniDicts.h"
// ============================================================================
/** @file 
 *  Implementation file for class functionds form the file LoKi/AMoniDicts.h
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2007-07-21 
 */
// ============================================================================
LoKi::PredicateFromPredicate<LoKi::ATypes::Combination>
LoKi::Dicts::monitor
( const LoKi::Predicate<LoKi::ATypes::Combination>& cut  , 
  StatEntity*                                       stat ) 
{ return LoKi::monitor ( cut , stat ) ; }
// ========================================================================
LoKi::FunctionFromFunction<LoKi::ATypes::Combination>
LoKi::Dicts::monitor
( const LoKi::Function<LoKi::ATypes::Combination>&  fun  , 
  StatEntity*                                       stat ) 
{ return LoKi::monitor ( fun  , stat ) ; }
// ========================================================================
LoKi::FunctionFromFunction<LoKi::ATypes::Combination>
LoKi::Dicts::monitor
( const LoKi::Function<LoKi::ATypes::Combination>&  fun   , 
  AIDA::IHistogram1D*                               histo ) 
{ return LoKi::monitor ( fun , histo ) ; }
// ============================================================================
// The END
// ============================================================================
