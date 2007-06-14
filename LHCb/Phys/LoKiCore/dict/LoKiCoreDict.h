// $Id: LoKiCoreDict.h,v 1.3 2007-06-14 14:54:17 cattanem Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.3 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2007/06/03 20:38:24  ibelyaev
//  update the generic algorithms
//
// Revision 1.1  2007/06/01 11:35:27  ibelyaev
//  prepare for v3r0
// 
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
    LoKi::Interface<LoKi::ILoKiSvc>  m_1 ;
    LoKi::Interface<LoKi::IReporter> m_2 ;
    LoKi::Interface<GaudiTool>       m_3 ;
    LoKi::Interface<GaudiAlgorithm>  m_4 ;
    /// fictive constructor 
    _Interfaces() ;
  } ;  
}
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_LOKICOREDICT_H
// ============================================================================
