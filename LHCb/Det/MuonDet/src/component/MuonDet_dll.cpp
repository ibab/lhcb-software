// $Id: MuonDet_dll.cpp,v 1.2 2002-01-31 10:00:10 dhcroft Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $

#define MUONDET_MUONDETDLL_CPP 1 

//====================================================================
//  MUON_dll.cpp
//--------------------------------------------------------------------
//
//  Package    : MuonDet
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

#include "GaudiKernel/DllMain.icpp"

void GaudiDll::initialize   ( void* /* hinstDLL */ ) {};
void GaudiDll::finalize     ( void* /* hinstDLL */ ) {};

extern void MuonDet_load();

#include "GaudiKernel/FactoryTable.h"

extern "C" FactoryTable::EntryList* getFactoryEntries() 
{
  ///
  static bool first = true;
  ///
  if ( first ) 
    {
      MuonDet_load();
      first = false;
    }
  ///
  return FactoryTable::instance()->getEntries();
}













