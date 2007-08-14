// $Id: LoKiArrayFunctorsDict.h,v 1.5 2007-08-14 18:27:27 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_LOKIARRAYDICT_H 
#define LOKI_LOKIARRAYDICT_H 1
// ============================================================================
// Include files
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/MoreFunctions.h"
#include "LoKi/Dicts.h"
// ============================================================================
#include "LoKi/ATypes.h"
#include "LoKi/LoKiArrayFunctors.h"
// ============================================================================
#include "LoKi/IHybridTool.h"
#include "LoKi/HybridEngine.h"
// ============================================================================
namespace LoKi
{
  namespace Dicts 
  {
    template <>
    class FunCalls<LoKi::ATypes::Combination> 
    {
    private:
      typedef LoKi::ATypes::Combination Type ;
      typedef LoKi::Function<Type>      Fun  ;
    public:
      static Fun::result_type __call__ 
      ( const Fun& fun  , const Type&           o ) { return fun ( o ) ; }
    } ;
    template <>
    class CutCalls<LoKi::ATypes::Combination> 
    {
    private:
      typedef LoKi::ATypes::Combination Type ;
      typedef LoKi::Predicate<Type>     Fun ;
    public:
      static Fun::result_type __call__ 
      ( const Fun& fun  , const Type&           o ) { return fun ( o ) ; }
    } ;
  } // end of namespace Dicts 
}
// ============================================================================
namespace
{
  struct _Instantiations 
  {
    /// fictive constructor 
    _Instantiations () ;
    // the basic functions 
    LoKi::Dicts::Funcs<LoKi::ATypes::Combination>          m_f1 ;
    // operators 
    LoKi::Dicts::FuncOps<LoKi::ATypes::Combination>        m_o1 ;
    /// mathematics:
    LoKi::Dicts::FunCalls<LoKi::ATypes::Combination>       m_c1 ;
    LoKi::Dicts::CutCalls<LoKi::ATypes::Combination>       m_c2 ;
    //
  } ;  
} // end of anonymous namespace 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_LOKIARRAYDICT_H
// ============================================================================
