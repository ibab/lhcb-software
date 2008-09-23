// $Id: OdinTypes.h,v 1.1 2008-09-23 13:13:35 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_ODINTYPES_H 
#define LOKI_ODINTYPES_H 1
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
namespace LHCb { class ODIN       ; }
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** @namespace OdinTypes LoKi/OdinTypes.h
   *
   *  The basic types defined for LoKiHlt package 
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date   2008-09-18
   */
  namespace OdinTypes 
  {
    // ========================================================================
    // O1 
    // ========================================================================
    /// type of 'cuts' for LHCb::ODIN        (interface)
    typedef LoKi::BasicFunctors<const LHCb::ODIN*>::Predicate ODIN_Cuts ;
    // ========================================================================
    /// type of 'functions' for LHCb::ODIN   (interface)
    typedef LoKi::BasicFunctors<const LHCb::ODIN*>::Function  ODIN_Func ;
    // ========================================================================
    /// type of 'cuts' for LHCb::ODIN        (assignable)
    typedef LoKi::BasicFunctors<const LHCb::ODIN*>::PredicateFromPredicate ODIN_Cut ;
    // ========================================================================
    /// type of 'functions' for HEpMC::GenParticle   (assignable)
    typedef LoKi::BasicFunctors<const LHCb::ODIN*>::FunctionFromFunction   ODIN_Fun ;
    // ========================================================================
  } // end of namespace LoKi::DAQTypes 
  // ==========================================================================
  namespace Types 
  {
    // ========================================================================
    // O1 
    // ========================================================================
    /// type of 'cuts' for LHCb::ODIN        (interface)
    typedef LoKi::OdinTypes::ODIN_Cuts                              ODIN_Cuts ;
    // ========================================================================
    /// type of 'functions' LHCb::ODIN       (interface)
    typedef LoKi::OdinTypes::ODIN_Func                              ODIN_Func ;
    // ========================================================================
    /// type of 'cuts' for LHCb::ODIN        (assignable)
    typedef LoKi::OdinTypes::ODIN_Cut                               ODIN_Cut  ;
    // ========================================================================
    /// type of 'functions' for LHCb::ODIN   (assignable)
    typedef LoKi::OdinTypes::ODIN_Fun                               ODIN_Fun  ;
    // ========================================================================
  }
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  // the specialized printout 
  // ==========================================================================
  template <>
  inline std::ostream& 
  Constant<const LHCb::ODIN*,bool>::fillStream( std::ostream& s ) const 
  { return s << ( this->m_value ? "ODIN_ALL" : "ODIN_NONE" ) ; }
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_DAQTYPES_H
// ============================================================================
