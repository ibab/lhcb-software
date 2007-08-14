// $Id: InfoDicts.h,v 1.1 2007-08-14 20:32:32 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_INFODICTS_H 
#define LOKI_INFODICTS_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/TrackTypes.h"
// ============================================================================
namespace LoKi 
{
  namespace Dicts 
  {
    namespace Info
    {
      // ======================================================================
      LoKi::FunctionFromFunction<LHCb::Track>
      info 
      ( const int                              index          , 
        const LoKi::Function<LHCb::Track>&     fun            ,
        const bool                             update = false ) ;
      // ======================================================================
      LoKi::FunctionFromFunction<LHCb::RecVertex>
      info 
      ( const int                              index          , 
        const LoKi::Function<LHCb::RecVertex>& fun            ,
        const bool                             update = false ) ;
    }
  }  
}
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_INFODICTS_H
// ============================================================================
