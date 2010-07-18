// $Id: LoKiTriggerDict.h,v 1.21 2010-06-06 08:33:35 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_LOKICOREDICT_H 
#define LOKI_LOKICOREDICT_H 1
// ============================================================================
// Include files
// ============================================================================
#include "HltBase/IHltRegister.h"
#include "HltBase/IHltData.h"
#include "HltBase/IHltInspector.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Interface.h"
#include "LoKi/HltTool.h"
// ============================================================================
namespace
{
  // ==========================================================================
  struct _Instantiations 
  {
    /// needed for various Hlt interafces 
    LoKi::Interface<Hlt::IRegister>          m_htl1 ;
    LoKi::Interface<Hlt::IData>              m_htl2 ;
    LoKi::Interface<Hlt::IInspector>         m_htl3 ;
    Hlt::IInspector::SelList                 m_hlt4 ;
    Hlt::IInspector::AlgList                 m_hlt5 ;
    /// fictive constructor 
    _Instantiations () ;
    // ========================================================================
  } ;  
  // ==========================================================================
} //                                                 end of anonymous namespace 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_LOKICOREDICT_H
// ============================================================================
