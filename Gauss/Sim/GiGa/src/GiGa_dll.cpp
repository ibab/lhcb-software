//====================================================================
//  GaudiSvc_dll.cpp
//--------------------------------------------------------------------
//
//  Package    : GaudiSvc
//
//  Description: Implementation of DllMain routine.
//               The DLL initialisation must be done seperately for 
//               each DLL. 
//
//  Author     : M.Frank
//  Created    : 13/1/99
//  Changes    : Pavel Binko, 15/02/2000
//               Version copied from Gaudi, and addopted for GaudiSvc
//
//====================================================================

// DllMain entry point

#include "GaudiKernel/DllMain.icpp"

void GaudiDll::initialize ( void* hinstDLL ) { void* aux = 0 ; aux = hinstDLL ; };

void GaudiDll::finalize   ( void* hinstDLL ) { void* aux = 0 ; aux = hinstDLL ; };

extern void GiGa_load();

#include "GaudiKernel/FactoryTable.h"

///
///
///

extern "C" FactoryTable::EntryList* getFactoryEntries() 
{

  static bool first = true;

  if ( first ) { GiGa_load() ; first = false; }

  return FactoryTable::instance()->getEntries();
  ///
};

///
///
///
 


