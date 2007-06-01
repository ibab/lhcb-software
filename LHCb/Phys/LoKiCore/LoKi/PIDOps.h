// $Id: PIDOps.h,v 1.1 2007-06-01 11:35:26 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, verison $Revision: 1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $
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
      typedef FUNCTION                               Function ;
      typedef typename FUNCTION::Type                Argument ;
      typedef LoKi::PredicateFromPredicate<Argument> Result   ;
      typedef LoKi::Function<Argument>               Other    ;
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
