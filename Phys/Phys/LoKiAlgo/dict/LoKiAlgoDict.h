// $Id: LoKiAlgoDict.h,v 1.1 2007-06-01 12:21:33 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $ 
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
// ============================================================================
namespace 
{
  struct __Instantiations
  {
    LoKi::Dicts::ExtFunCalls<LHCb::Particle>   m_c1 ;
    LoKi::Dicts::ExtFunCalls<LHCb::VertexBase> m_c2 ;    
    LoKi::Dicts::ExtCutCalls<LHCb::Particle>   m_c3 ;
    LoKi::Dicts::ExtCutCalls<LHCb::VertexBase> m_c4 ;    
  } ;    
} // end of anonymous namespace 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_LOKICOREDICT_H
// ============================================================================
