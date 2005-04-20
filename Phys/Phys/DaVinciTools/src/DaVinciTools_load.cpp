// $Id: DaVinciTools_load.cpp,v 1.49 2005-04-20 14:08:39 pkoppenb Exp $
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
  DECLARE_TOOL( RecursivePlotTool ) ;
  DECLARE_TOOL( SimplePlotTool ) ;

  DECLARE_TOOL( SetAlgorithmID ) ;

  // Algorithms
  DECLARE_ALGORITHM( CheckPV );  
  DECLARE_ALGORITHM( ResetPVLocation ) ;
  DECLARE_ALGORITHM( SimplePlots );  
  DECLARE_ALGORITHM( PIDFilter );  
  
}

