//====================================================================
//  LHCbEventFactories_dll.cpp
//--------------------------------------------------------------------
//
//  Package    : Gaudi
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
extern void VeloEventFactories_load();

void GaudiDll::initialize(void* hinstDLL)    {
}

void GaudiDll::finalize(void* hinstDLL)    {
}

#include "GaudiKernel/FactoryTable.cpp"
extern "C" FactoryTable::EntryList* getFactoryEntries() {
  static bool first = true;
  if ( first )    {
    VeloEventFactories_load();
    first = false;
  }
  return FactoryTable::instance()->getEntries();
} 
