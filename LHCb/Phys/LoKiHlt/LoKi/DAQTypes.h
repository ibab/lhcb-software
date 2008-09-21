// $Id: DAQTypes.h,v 1.1.1.1 2008-09-21 14:41:20 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_DAQTYPES_H 
#define LOKI_DAQTYPES_H 1
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
  /** @class DAQTypes DAQTypes.h LoKi/DAQTypes.h
   *
   *  The basic types defined for LoKiHlt package 
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date   2008-09-18
   */
  namespace DAQTypes 
  {
    // ========================================================================
    // O1 
    // ========================================================================
    /// type of 'cuts' for LHCb::ODIN        (interface)
    typedef LoKi::BasicFunctors<const LHCb::ODIN*>::Predicate O1_Cuts ;
    // ========================================================================
    /// type of 'functions' for LHCb::ODIN   (interface)
    typedef LoKi::BasicFunctors<const LHCb::ODIN*>::Function  O1_Func ;
    // ========================================================================
    /// type of 'cuts' for LHCb::ODIN        (assignable)
    typedef LoKi::BasicFunctors<const LHCb::ODIN*>::PredicateFromPredicate O1_Cut ;
    // ========================================================================
    /// type of 'functions' for HEpMC::GenParticle   (assignable)
    typedef LoKi::BasicFunctors<const LHCb::ODIN*>::FunctionFromFunction   O1_Fun ;
    // ========================================================================
  } // end of namespace LoKi::DAQTypes 
  // ==========================================================================
  namespace Types 
  {
    // ========================================================================
    // O1 
    // ========================================================================
    /// type of 'cuts' for LHCb::ODIN        (interface)
    typedef LoKi::DAQTypes::O1_Cuts                                  O1_Cuts ;
    // ========================================================================
    /// type of 'functions' LHCb::ODIN       (interface)
    typedef LoKi::DAQTypes::O1_Func                                  O1_Func ;
    // ========================================================================
    /// type of 'cuts' for LHCb::ODIN        (assignable)
    typedef LoKi::DAQTypes::O1_Cut                                   O1_Cut  ;
    // ========================================================================
    /// type of 'functions' for LHCb::ODIN   (assignable)
    typedef LoKi::DAQTypes::O1_Fun                                   O1_Fun  ;
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
  { return s << ( this->m_value ? "O1_ALL" : "O1_NONE" ) ; }
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_DAQTYPES_H
// ============================================================================
