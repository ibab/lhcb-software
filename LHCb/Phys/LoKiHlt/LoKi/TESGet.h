// $Id:$ 
// ============================================================================
#ifndef LOKI_TESGET_H 
#define LOKI_TESGET_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/TES.h"
#include "LoKi/L0Types.h"
#include "LoKi/HLTTypes.h"
#include "LoKi/OdinTypes.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace TES
  {
    // ========================================================================
    /** @class TESGet TESGet.h LoKi/TESGet.h
     *  
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2013-03-30
     */
    class GAUDI_API TESGet
    {
    public:
      // ======================================================================
      static LoKi::FunctorFromFunctor<void,bool>  
      get 
      ( const LoKi::TES::Get&                                   obj , 
        const LoKi::Functor<const LHCb::ODIN*,bool>&            cut );
      // ======================================================================
      static 
      LoKi::FunctorFromFunctor<void,double>  
      get 
      ( const LoKi::TES::Get&                                   obj , 
        const LoKi::Functor<const LHCb::ODIN*,double>&          fun );
      // ======================================================================
      static LoKi::FunctorFromFunctor<void,bool>  
      get 
      ( const LoKi::TES::Get&                                   obj , 
        const LoKi::Functor<const LHCb::L0DUReport*,bool>&      cut );
      // ======================================================================
      static 
      LoKi::FunctorFromFunctor<void,double>  
      get 
      ( const LoKi::TES::Get&                                   obj , 
        const LoKi::Functor<const LHCb::L0DUReport*,double>&    fun );
      // ======================================================================
      static LoKi::FunctorFromFunctor<void,bool>  
      get 
      ( const LoKi::TES::Get&                                   obj , 
        const LoKi::Functor<const LHCb::HltDecReports*,bool>&   cut );
      // ======================================================================
      static 
      LoKi::FunctorFromFunctor<void,double>  
      get 
      ( const LoKi::TES::Get&                                   obj , 
        const LoKi::Functor<const LHCb::HltDecReports*,double>& fun );
      // ======================================================================
    };
    // ========================================================================
  } // end of namespace                                               LoKi::TES
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_TESGET_H
// ============================================================================
