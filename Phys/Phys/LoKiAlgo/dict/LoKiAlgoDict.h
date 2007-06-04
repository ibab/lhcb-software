// $Id: LoKiAlgoDict.h,v 1.3 2007-06-04 14:31:17 cattanem Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.3 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2007/06/03 20:43:42  ibelyaev
//  update LoKi::Child &
//
// Revision 1.1  2007/06/01 12:21:33  ibelyaev
//  prepare for v4r0
// 
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
#include "LoKi/AlgoDecorator.h"
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
