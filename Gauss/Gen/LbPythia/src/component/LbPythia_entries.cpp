 //  $Id: LbPythia_entries.cpp,v 1.3 2005-11-29 16:03:18 robbep Exp $
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
  // Old structure
  DECLARE_ALGORITHM( PythiaAlg );
  DECLARE_ALGORITHM( HiggsPythiaAlg );
  DECLARE_ALGORITHM( TTbarPythiaAlg );

  // New structure:

  // Production tools
  DECLARE_TOOL( PythiaProduction ) ;

  // Cut tools
  DECLARE_TOOL( PythiaHiggs ) ;
}
  
