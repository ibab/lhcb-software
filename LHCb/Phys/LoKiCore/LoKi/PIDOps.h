// $Id: PIDOps.h,v 1.3 2007-11-28 13:56:33 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_PIDOPS_H 
#define LOKI_PIDOPS_H 1
// ============================================================================
// Include files
// ============================================================================
// Kernel
// ============================================================================
#include "Kernel/ParticleID.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Functions.h"
#include "LoKi/Operators.h"
#include "LoKi/BasicFunctors.h"
// ============================================================================
namespace LoKi 
{
  namespace Dicts 
  {
    /** @class PIDOps PIDOps.h LoKi/PIDOps.h
     *  @author Vanya BELYAEV
     *  @date   2007-05-29
     */
    template <class FUNCTION>
    class PIDOps 
    {
    private:
      typedef FUNCTION                                Function ;
      typedef typename FUNCTION::Type                 Argument ;
      typedef typename LoKi::BasicFunctors<Argument>::PredicateFromPredicate Result ;
      typedef typename LoKi::BasicFunctors<Argument>::Function Other    ;
    public: 
      // ======================================================================
      static Result
      __eq__ ( const Function&         self  , 
               const long              other ) { return self == other ; } 
      static Result 
      __eq__ ( const Function&         self  , 
               const LHCb::ParticleID& other ) { return self == other ; } 
      static Result
      __eq__ ( const Function&         self  , 
               const std::string&      other ) { return self == other ; } 
      static Result
      __eq__ ( const Function&         self  , 
               const Other&            other ) { return self == other ; }
      // ======================================================================
      static Result
      __ne__ ( const Function&         self  , 
               const long              other ) { return self != other ; } 
      static Result 
      __ne__ ( const Function&         self  , 
               const LHCb::ParticleID& other ) { return self != other ; } 
      static Result
      __ne__ ( const Function&         self  , 
               const std::string&      other ) { return self != other ; } 
      static Result
      __ne__ ( const Function&         self  , 
               const Other&            other ) { return self != other ; }
      // ======================================================================
    } ;
  } // end of namespace LoKi::Dicts 
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PIDOPS_H
// ============================================================================
