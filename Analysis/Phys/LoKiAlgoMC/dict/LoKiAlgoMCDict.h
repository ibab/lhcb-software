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
#include "LoKi/LoKiAlgoMC.h"
#include "LoKi/MCMatchAlgoDicts.h"
#include "LoKi/MCTupleDicts.h"
#include "LoKi/GenTupleDicts.h"
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
