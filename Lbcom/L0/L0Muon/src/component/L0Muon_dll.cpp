// $Header: /afs/cern.ch/project/cvs/reps/lhcb/L0/L0Muon/src/component/L0Muon_dll.cpp,v 1.1 2001-06-07 16:34:21 atsareg Exp $
//====================================================================
//  L0Muon_dll.cpp
//--------------------------------------------------------------------
//
//  Package    : L0Muon
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

///
/// DllMain entry point
///

///
///
///

#include "GaudiKernel/DllMain.icpp"

///
///
///

void GaudiDll::initialize   ( void* hinstDLL )    
{
  void * aux = 0 ; 
  aux = hinstDLL ; // trick to avoid compilationm warnings 
};

///
///
///

void GaudiDll::finalize     ( void* hinstDLL )  
{
  void * aux = 0 ; 
  aux = hinstDLL ; // trick to avoid compilationm warnings 
};

///
///
///

extern void L0Muon_load();

///
///
///

#include "GaudiKernel/FactoryTable.h"

///
///
///

extern "C" FactoryTable::EntryList* getFactoryEntries() 
{
  ///
  static bool first = true;
  ///
  if ( first ) 
    {
      L0Muon_load();
      first = false;
    }
  ///
  return FactoryTable::instance()->getEntries();
}


