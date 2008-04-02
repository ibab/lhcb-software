// $Id: ATypes.h,v 1.4 2008-04-02 21:10:47 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_ATYPES_H 
#define LOKI_ATYPES_H 1
// ============================================================================
// Include files
// ============================================================================
// Event
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Functions.h"
#include "LoKi/Range.h"
#include "LoKi/PhysTypes.h"
// ============================================================================
namespace LoKi 
{
  namespace ATypes
  {
    /** The actual argument type for Exclusive HLT processing, 
     *  as requested by Gerhard RAVEN and Patrick KOPPENBURG
     */
    typedef LoKi::Range_<LHCb::Particle::ConstVector>          Combination ;
    /// type of 'cuts' for the combination (interface)
    typedef LoKi::BasicFunctors<Combination>::Predicate              ACuts ;
    /// type of 'functions' for the combination (interface)
    typedef LoKi::BasicFunctors<Combination>::Function               AFunc ;
    /// type of 'cuts' for combination (assignable)
    typedef LoKi::BasicFunctors<Combination>::PredicateFromPredicate ACut  ;
    /// type of 'functions' for combination (assignable)
    typedef LoKi::BasicFunctors<Combination>::FunctionFromFunction   AFun  ;
  } // end of namespace LoKi::ATypes 
  namespace Types
  {
    /// type of 'cuts' for the combination (interface)
    typedef LoKi::ATypes::ACuts                                     ACuts ;
    /// type of 'functions' for the combination (interface)
    typedef LoKi::ATypes::AFunc                                     AFunc ;
    /// type of 'cuts' for combination (assignable)
    typedef LoKi::ATypes::ACut                                      ACut  ;
    /// type of 'functions' for combination (assignable)
    typedef LoKi::ATypes::AFun                                      AFun  ;
  } // end of namespace LoKi::Types 
} // end of namespace LoKi 
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  // the specialized printout 
  // ==========================================================================
  template <>
  inline std::ostream& 
  Constant<LoKi::ATypes::Combination,bool>::fillStream   ( std::ostream& s ) const 
  { return s << ( this->m_value ? "AALL" : "ANONE" ) ; }
  // ==========================================================================
}
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_ATYPES_H
// ============================================================================
