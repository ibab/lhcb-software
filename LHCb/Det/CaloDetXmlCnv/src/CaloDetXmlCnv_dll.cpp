// $Id: CaloDetXmlCnv_dll.cpp,v 1.1.1.1 2002-03-28 13:32:34 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// includes 
#include "GaudiKernel/DllMain.icpp"
#include "GaudiKernel/FactoryTable.h"
// declaration 
extern void CaloDetXmlCnv_load();

// ============================================================================
/** @file  CaloDetXmlCnv_dll.cpp
 *
 *  Implementation of DllMain routine.
 *  The DLL initialisation must be done seperately for each DLL. 
 */
// ============================================================================

void GaudiDll::initialize   ( void* /* hinstDLL */ ) {};
void GaudiDll::finalize     ( void* /* hinstDLL */ ) {};

extern "C" FactoryTable::EntryList* getFactoryEntries() 
{
  ///
  static bool first = true;
  ///
  if ( first ) 
    {
      CaloDetXmlCnv_load();
      first = false;
    }
  ///
  return FactoryTable::instance()->getEntries();
};

// ============================================================================
// The End 
// ============================================================================











