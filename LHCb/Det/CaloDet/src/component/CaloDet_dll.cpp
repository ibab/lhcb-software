// $Id: CaloDet_dll.cpp,v 1.3 2001-12-15 18:28:17 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2001/06/29 10:26:18  ibelyaev
// update to use new features of DetDesc v7 package
//
#define CALODET_CALODETDLL_CPP 1 

//====================================================================
//  Calo_dll.cpp
//--------------------------------------------------------------------
//
//  Package    : Calo
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

extern void CaloDet_load();

#include "GaudiKernel/FactoryTable.h"

extern "C" FactoryTable::EntryList* getFactoryEntries() 
{
  ///
  static bool first = true;
  ///
  if ( first ) 
    {
      CaloDet_load();
      first = false;
    }
  ///
  return FactoryTable::instance()->getEntries();
}













