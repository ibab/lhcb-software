// $Id: LoKiCoreDict.h,v 1.13 2010-04-04 12:20:56 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_LOKICOREDICT_H 
#define LOKI_LOKICOREDICT_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/HistoDef.h"
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
#include "LoKi/Print.h"
#include "LoKi/HistoBook.h"
// ============================================================================
#include "LoKi/ICoreFactory.h"
#include "LoKi/ICoreAntiFactory.h"
#include "LoKi/CoreEngine.h"
#include "LoKi/iTree.h"
#include "LoKi/IDecayNode.h"
#include "LoKi/Listener.h"
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
  // ==========================================================================
  struct _Interfaces 
  { 
    // ========================================================================
    LoKi::Interface<LoKi::ILoKiSvc>   m_1 ;
    LoKi::Interface<LoKi::IReporter>  m_2 ;
    LoKi::Interface<GaudiTool>        m_3 ;
    LoKi::Interface<GaudiAlgorithm>   m_4 ;
    LoKi::Interface<IDataProviderSvc> m_5 ;
    // fictive constructor 
    _Interfaces() ;
    // ========================================================================
  } ;  
  // ==========================================================================
}
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_LOKICOREDICT_H
// ============================================================================
