// $Id: LoKiAlgoDict.h,v 1.6 2008-06-12 08:28:54 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_LOKICOREDICT_H 
#define LOKI_LOKICOREDICT_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/LoKiAlgo.h"
#include "LoKi/ExtCalls.h"
#include "LoKi/LoopChild.h"
#include "LoKi/LoopDecorator.h"
// ============================================================================
// Trick to suppress _POSIX_C_SOURCE redefinition warning
#ifdef __GNUC__
#undef _POSIX_C_SOURCE
#endif
// ============================================================================
#include "LoKi/AlgoDecorator.h"
#include "LoKi/TupleDicts.h"
// ============================================================================
// Stefan Roiser's trick to allow compilation on Win32
// ============================================================================
#ifdef WIN32
#ifdef CONST
#undef CONST
#endif 
#endif
// ============================================================================
namespace 
{
  struct __Instantiations
  {
    LoKi::Dicts::ExtFunCalls<LHCb::Particle>   m_c1 ;
    LoKi::Dicts::ExtFunCalls<LHCb::VertexBase> m_c2 ;    
    LoKi::Dicts::ExtCutCalls<LHCb::Particle>   m_c3 ;
    LoKi::Dicts::ExtCutCalls<LHCb::VertexBase> m_c4 ;    
    //
    LoKi::Dicts::Alg<LoKi::Algo>               m_a1 ;
    // fictive constructor 
    __Instantiations();
    ~__Instantiations();
  } ;    
} // end of anonymous namespace 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_LOKICOREDICT_H
// ============================================================================
