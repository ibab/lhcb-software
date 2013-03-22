// $Id$
// ============================================================================
#ifndef LOKI_LOKICOREDICT_H 
#define LOKI_LOKICOREDICT_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/AlgoDecorator.h"
#include "LoKi/TupleDicts.h"
#include "LoKi/LoKiAlgo.h"
#include "LoKi/ExtCalls.h"
#include "LoKi/LoopChild.h"
#include "LoKi/LoopDecorator.h"
// ============================================================================
// Stefan Roiser's trick to allow compilation on Win32
// ============================================================================
#ifdef WIN32
#ifdef CONST
#undef CONST
#endif 
#endif
// ============================================================================
#ifdef __INTEL_COMPILER
#pragma warning(disable:177) //  variable ... was declared but never referenced
#pragma warning(disable:191) // type qualifier is meaningless on cast type
#endif
// ============================================================================
namespace 
{
  // ===========================================================================
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
  // ==========================================================================
} // end of anonymous namespace 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_LOKICOREDICT_H
// ============================================================================
