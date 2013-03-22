// $Id$
// ============================================================================
#ifndef LOKI_LOKICOREDICT_H 
#define LOKI_LOKICOREDICT_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/LoKiAlgoMC.h"
#include "LoKi/MCMatchAlgoDicts.h"
#include "LoKi/MCTupleDicts.h"
#include "LoKi/GenTupleDicts.h"
// ============================================================================
// Trick to suppress redefinition warnings
#ifdef __GNUC__
#undef _POSIX_C_SOURCE
#undef _XOPEN_SOURCE
#endif
// ============================================================================
#include "LoKi/AlgoDecorator.h"
// ============================================================================
namespace
{
  struct __Instantiations
  {
    LoKi::Interface<LoKi::AlgoMC>  m_a1 ;
    LoKi::Dicts::Alg<LoKi::AlgoMC> m_a2 ;
    //
    __Instantiations();
  } ;
}
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_LOKICOREDICT_H
// ============================================================================
