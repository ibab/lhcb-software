// $Id: XmlTools_dll.cpp,v 1.1.1.1 2003-04-23 13:38:46 sponce Exp $

/**
 * Package    : XmlTools
 * Implementation of DllMain routine.
 * The DLL initialisation must be done seperately for each DLL. 
 *
 * @author Sebastien Ponce
 */

// DllMain entry point
#include "GaudiKernel/DllMain.icpp"

void GaudiDll::initialize(void* /*hinstDLL*/) {}

void GaudiDll::finalize(void* /*hinstDLL*/) {}

extern void XmlTools_load();
#include "GaudiKernel/FactoryTable.h"
extern "C" FactoryTable::EntryList* getFactoryEntries() {
  static bool first = true;
  if ( first ) {
    XmlTools_load();
    first = false;
  }
  return FactoryTable::instance()->getEntries();
} 

