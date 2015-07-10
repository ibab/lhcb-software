// $Id$
// ============================================================================
#ifndef LOKI_PHYSALGSDICTS_H 
#define LOKI_PHYSALGSDICTS_H 1
// ============================================================================
// Include files
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Constants.h"
#include "LoKi/PhysTypes.h"
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Dicts 
  {
    // ========================================================================
    namespace Algs 
    {
      // ======================================================================
      /// count the elements in the tree 
      std::size_t count_if 
      ( const LHCb::Particle*        particle , 
        const LoKi::PhysTypes::Cuts& cut      ) ;
      // ======================================================================
      /// check the presence in the tree 
      bool        found 
      ( const LHCb::Particle*        particle , 
        const LoKi::PhysTypes::Cuts& cut      ) ;
      // ======================================================================
      /// find the minimum over the tree 
      double      min_value 
      ( const LHCb::Particle*        particle , 
        const LoKi::PhysTypes::Func& fun      , 
        const LoKi::PhysTypes::Cuts& cut      ,
        double result = LoKi::Constants::PositiveInfinity ) ;
      // ======================================================================
      /// find the maximum over the tree 
      double      max_value 
      ( const LHCb::Particle*        particle , 
        const LoKi::PhysTypes::Func& fun      , 
        const LoKi::PhysTypes::Cuts& cut      ,
        double result = LoKi::Constants::NegativeInfinity ) ;
      // ======================================================================
      /// find the minimum element in  the tree 
      const LHCb::Particle* min_element
      ( const LHCb::Particle*        particle , 
        const LoKi::PhysTypes::Func& fun      , 
        const LoKi::PhysTypes::Cuts& cut      ) ;
      // ======================================================================
      /// find the maximum element in  the tree 
      const LHCb::Particle* max_element
      ( const LHCb::Particle*        particle , 
        const LoKi::PhysTypes::Func& fun      , 
        const LoKi::PhysTypes::Cuts& cut      ) ;
      // ======================================================================
      /// accumulate over the tree 
      double accumulate  
      ( const LHCb::Particle*        particle      , 
        const LoKi::PhysTypes::Func& fun           , 
        const LoKi::PhysTypes::Cuts& cut           ,    
        double                       result  = 0.0 ) ;
      // ======================================================================
    } // end of namespace LoKi::Dicts::Algs 
    // ========================================================================
  } // end of namespace LoKi::Dicts
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PHYSALGSDICTS_H
// ============================================================================
