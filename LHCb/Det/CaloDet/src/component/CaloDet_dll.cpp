// $Id: CaloDet_dll.cpp,v 1.4 2002-03-28 13:47:14 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2001/12/15 18:28:17  ibelyaev
//  update for new policy of versions and new test algorithm
//
// Revision 1.2  2001/06/29 10:26:18  ibelyaev
// update to use new features of DetDesc v7 package
// ============================================================================
// include 
#include "GaudiKernel/DllMain.icpp"
#include "GaudiKernel/FactoryTable.h"
// declarations 
extern void CaloDet_load();

//=============================================================================
/** @file  Calo_dll.cpp
 *  Implementation of DllMain routine.
 *  The DLL initialisation must be done seperately for each DLL. 
 */
//=============================================================================

void GaudiDll::initialize   ( void* /* hinstDLL */ ) {};
void GaudiDll::finalize     ( void* /* hinstDLL */ ) {};

extern "C" FactoryTable::EntryList* getFactoryEntries() 
{
  static bool first = true;
  if ( first ) 
    {
      CaloDet_load();
      first = false;
    }
  ///
  return FactoryTable::instance()->getEntries();
}


//=============================================================================
// The End 
//=============================================================================











