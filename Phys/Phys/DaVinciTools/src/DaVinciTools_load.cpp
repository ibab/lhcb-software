// $Id: DaVinciTools_load.cpp,v 1.40 2004-08-25 13:25:53 pkoppenb Exp $
// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/ObjectFactory.h"

// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_CONVERTER( MyConverter );
// DECLARE_OBJECT( DataObject );
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

DECLARE_FACTORY_ENTRIES( DaVinciTools ) {

  // Miscellaneous
  DECLARE_TOOL( CheckOverlap );
  DECLARE_TOOL( DecayFinder );
  DECLARE_TOOL( DecodeSimpleDecayString );

  // Algorithms
  DECLARE_ALGORITHM( PreDV );
  
}

