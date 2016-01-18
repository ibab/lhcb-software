// $Id: $
//==========================================================================
//  LHCb Online software suite
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see OnlineSys/LICENSE.
//
// Author     : M.Frank
//
//==========================================================================
#ifndef _MBM_INTERNALS_H
#define _MBM_INTERNALS_H
#include "RTL/rtl.h"
#include "RTL/que.h"
#include "RTL/bits.h"
#include "MBM/bmdef.h"

#ifdef MBM_IMPLEMENTATION
struct BMDESCRIPT;
struct CONTROL;
struct EVENT;
struct USER;
#else
typedef long BMDESCRIPT;
typedef long CONTROL;
typedef long EVENT;
typedef long USER;
typedef long TriggerMask;
typedef long UserMask;
#endif

#define _CHECK( x )  { int sc = x ; if ( !(sc&1) ) { lib_rtl_printf ( "Error in:%s, status=%d\n", #x , sc ); return sc; } }
static inline size_t mbm_section_length(size_t bytes) {
  static const int SECTION_ALIGNMENT=256;
  return size_t((bytes+SECTION_ALIGNMENT-1)/SECTION_ALIGNMENT)*SECTION_ALIGNMENT;
}

#endif // _MBM_INTERNALS_H
