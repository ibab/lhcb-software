//====================================================================
//  Detdesc_dll.cpp
//--------------------------------------------------------------------
//
//  Package    : DetDesc
//
//  Description: Implementation of DllMain routine.
//               The DLL initialisation must be done seperately for 
//               each DLL. 
//
//  Author     : M.Frank
//  Created    : 13/1/99
//  Changes    : 
//
//====================================================================

// DllMain entry point
#include "GaudiKernel/DllMain.icpp"

void GaudiDll::initialize(
                           void* //hinstDLL
                         )
{
}

void GaudiDll::finalize(
                         void* //hinstDLL
                       )
{
}

extern void DetDesc_load();
#include "GaudiKernel/FactoryTable.h"
extern "C" FactoryTable::EntryList* getFactoryEntries() {
  static bool first = true;
  if ( first ) {
    DetDesc_load();
    first = false;
  }
  return FactoryTable::instance()->getEntries();
} 

