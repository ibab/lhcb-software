// $Id: DaVinciTools_load.cpp,v 1.44 2004-11-15 07:31:42 pkoppenb Exp $
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
  DECLARE_TOOL( PVLocator ) ;

  // Algorithms
  DECLARE_ALGORITHM( ResetPVLocation ) ;
  
}

