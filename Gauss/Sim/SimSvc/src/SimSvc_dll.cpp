//====================================================================
//  SimSvc_dll.cpp
//--------------------------------------------------------------------
//
//  Package    : SimSvc
//
//  Description: Implementation of DllMain routine.
//               The DLL initialisation must be done seperately for 
//               each DLL. 
//
//  Author     : Sebastien Ponce
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

extern void SimSvc_load();
#include "GaudiKernel/FactoryTable.h"
extern "C" FactoryTable::EntryList* getFactoryEntries() {
  static bool first = true;
  if ( first ) {
    SimSvc_load();
    first = false;
  }
  return FactoryTable::instance()->getEntries();
} 

