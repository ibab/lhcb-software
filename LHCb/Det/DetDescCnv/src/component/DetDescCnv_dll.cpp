//====================================================================
//  DetdescCnv_dll.cpp
//--------------------------------------------------------------------
//
//  Package    : DetDescCnv
//
//  Description: Implementation of DllMain routine.
//               The DLL initialisation must be done seperately for 
//               each DLL. 
//
//  Author     : S. Ponce
//
//====================================================================

// DllMain entry point
#include "GaudiKernel/DllMain.icpp"

void GaudiDll::initialize(void* /*hinstDLL*/) {}

void GaudiDll::finalize(void* /*hinstDLL*/) {}

extern void DetDescCnv_load();
#include "GaudiKernel/FactoryTable.h"
extern "C" FactoryTable::EntryList* getFactoryEntries() {
  static bool first = true;
  if ( first ) {
    DetDescCnv_load();
    first = false;
  }
  return FactoryTable::instance()->getEntries();
} 

