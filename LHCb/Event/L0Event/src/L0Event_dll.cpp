//====================================================================
//  L0Event_dll.cpp
//--------------------------------------------------------------------
//
//  Package    : L0Event
//
//  Description: Implementation of DllMain routine.
//               The DLL initialisation must be done seperately for 
//               each DLL. 
//
//  Author     : Bruce Hay (after an original by M.Frank)
//  Created    : 29/05/00
//  Changes    : 
//
//====================================================================

// DllMain entry point
#include "GaudiKernel/DllMain.icpp"
extern void L0Event_load();

void GaudiDll::initialize(void* hinstDLL)    {
}

void GaudiDll::finalize(void* hinstDLL)    {
}

#include "GaudiKernel/FactoryTable.h"
extern "C" FactoryTable::EntryList* getFactoryEntries() {
  static bool first = true;
  if ( first )    {
    L0Event_load();
    first = false;
  }
  return FactoryTable::instance()->getEntries();
} 












