// $Id: GiGa_load.cpp,v 1.15 2002-12-07 14:27:53 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.14  2002/09/26 18:05:37  ibelyaev
//  repackaging: remove all concrete implementations
//
// Revision 1.13  2002/05/07 12:21:37  ibelyaev
//  see $GIGAROOT/doc/release.notes  7 May 2002
//
// Revision 1.12  2002/05/04 20:20:12  ibelyaev
//  see $GIGAROOT/doc/release.notes (4 May 2002)
//
// Revision 1.11  2002/04/25 13:02:05  ibelyaev
//  small update
//
// ============================================================================
#define GIGA_GIGA_LOAD_CPP 1 
// ============================================================================
// include 
// GaudiKernel 
#include "GaudiKernel/DeclareFactoryEntries.h" 
// GiGa 
#include "GiGa/GiGaMACROs.h"

// ============================================================================
/** @file GiGa_load.cpp
 *  mandatry file for declaration of component library entries 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date xx/xx/xxxx
 */
// ============================================================================

void GiGa_load() 
{ 
  /// GiGa Service itself
  DECLARE_SERVICE         (    GiGa                    ) ; 
  /// Run manager  (GiGa)
  DECLARE_GiGaFactory     (    GiGaRunManager          ) ;
};

// ============================================================================
extern "C" void GiGa_loadRef() { GiGa_load(); } ; 
// ============================================================================

// ============================================================================
// The End 
// ============================================================================








