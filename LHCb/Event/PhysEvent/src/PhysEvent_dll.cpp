// $Id: PhysEvent_dll.cpp,v 1.1.1.1 2001-07-09 09:23:58 gcorti Exp $

//--------------------------------------------------------------------
// Implementation file for PhysEvent dll routines
//
// 28/05/2001 : Gloria Corti
//--------------------------------------------------------------------

//====================================================================
// DllMain entry point
//====================================================================
#include "GaudiKernel/DllMain.icpp"
#include "GaudiKernel/FactoryTable.h"
extern void PhysEvent_load();

void GaudiDll::initialize(void* /*hinstDLL*/)    {
}

void GaudiDll::finalize(void* /*hinstDLL*/)    {
}

extern "C" FactoryTable::EntryList* getFactoryEntries() {
  static bool first = true;
  if ( first )    {
    PhysEvent_load();
    first = false;
  }
  return FactoryTable::instance()->getEntries();
} 
