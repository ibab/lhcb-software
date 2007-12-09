// $Id: LoKiCoreDict.h,v 1.5 2007-12-09 19:17:32 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_LOKICOREDICT_H 
#define LOKI_LOKICOREDICT_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiAlg/GaudiTool.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/LoKiCore.h"
#include "LoKi/Services.h"
#include "LoKi/Dicts.h"
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
  struct _Interfaces 
  {
    // 
    LoKi::Interface<LoKi::ILoKiSvc>   m_1 ;
    LoKi::Interface<LoKi::IReporter>  m_2 ;
    LoKi::Interface<GaudiTool>        m_3 ;
    LoKi::Interface<GaudiAlgorithm>   m_4 ;
    LoKi::Interface<IDataProviderSvc> m_5 ;
    // fictive constructor 
    _Interfaces() ;
  } ;  
}
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_LOKICOREDICT_H
// ============================================================================
