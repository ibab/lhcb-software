// $Id: DetCondDict.h,v 1.4 2006-07-14 09:27:33 marcocle Exp $
// ============================================================================
#ifndef DETCOND_DETCONDDICT_H 
#define DETCOND_DETCONDDICT_H 1
// ============================================================================
// Hack to get round gccxml parsing problem (SEAL bug 9704)
// ============================================================================
#ifdef _WIN32
#define LONG_LONG_MAX  0x7fffffffffffffffLL  /*maximum   signed __int64 value */
#define LONG_LONG_MIN  0x8000000000000000LL  /*minimum   signed __int64 value */
#define ULONG_LONG_MAX 0xffffffffffffffffLL  /*maximum unsigned __int64 value */
#endif
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Time.h"
// ============================================================================
// DetCond 
// ============================================================================
#include "DetCond/ICondDBAccessSvc.h"
#include "DetCond/ICondDBEditor.h"
#include "DetCond/ICondDBReader.h"
// ============================================================================
#endif // DETCOND_DETCONDDICT_H
// ============================================================================

