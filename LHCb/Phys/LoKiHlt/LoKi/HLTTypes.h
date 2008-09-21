// $Id: HLTTypes.h,v 1.1.1.1 2008-09-21 14:41:20 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_HLTTYPES_H 
#define LOKI_HLTTYPES_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/CoreTypes.h"
#include "LoKi/Functions.h"
// ============================================================================
// forward declarations 
// ============================================================================
namespace LHCb { class HltDecReports ; }
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** @class HLTTypes LoKi/HLTTypes.h
   *
   *  The basic types defined for LoKiHlt package 
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date   2008-09-18
   */  
  namespace HLTTypes 
  {
    // ========================================================================
    // L0 
    // ========================================================================
    /// type of 'cuts' for LHCb::HltDecReports (interface)
    typedef LoKi::BasicFunctors<const LHCb::HltDecReports*>::Predicate HLT_Cuts ;
    // ========================================================================
    /// type of 'functions' for LHCb::HltDecReports (interface)
    typedef LoKi::BasicFunctors<const LHCb::HltDecReports*>::Function  HLT_Func ;
    // ========================================================================
    /// type of 'cuts' for LHCb::HltDecReports      (assignable)
    typedef LoKi::BasicFunctors<const LHCb::HltDecReports*>::PredicateFromPredicate HLT_Cut ;
    // ========================================================================
    /// type of 'functions' for LHCb::HltDecReports (assignable)
    typedef LoKi::BasicFunctors<const LHCb::HltDecReports*>::FunctionFromFunction HLT_Fun ;
    // ========================================================================
  } // end of namespace LoKi::HLTTypes 
  // ==========================================================================
  namespace Types 
  {
    // ========================================================================
    /// type of 'cuts' for LHCb::HltDecReports      (interface)
    typedef LoKi::HLTTypes::HLT_Cuts                                 HLT_Cuts ;
    // ========================================================================
    /// type of 'functions' LHCb::HltDecReports     (interface)
    typedef LoKi::HLTTypes::HLT_Func                                 HLT_Func ;
    // ========================================================================
    /// type of 'cuts' for LHCb::HltDecReports      (assignable)
    typedef LoKi::HLTTypes::HLT_Cut                                  HLT_Cut  ;
    // ========================================================================
    /// type of 'functions' for LHCb::HltDecReports (assignable)
    typedef LoKi::HLTTypes::HLT_Fun                                  HLT_Fun  ;
    // ========================================================================
  } // end of namespace LoKi::Types 
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_HLTTYPES_H
// ============================================================================
