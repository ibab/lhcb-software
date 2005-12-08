 //  $Id: LbPythia_entries.cpp,v 1.4 2005-12-08 14:56:42 robbep Exp $
//====================================================================
//  LbPythia_entries.cxx
//--------------------------------------------------------------------
//
//  Package    : Gen/LbPythia
//
//  Description: Declaration of the components (factory entries)
//               specified by this component library.
//
//====================================================================

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( LbPythia ) {
  // New structure:

  // Production tools
  DECLARE_TOOL( PythiaProduction ) ;

  // Cut tools
  DECLARE_TOOL( PythiaHiggs ) ;
}
  
