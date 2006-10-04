// $Id: LbPythia_load.cpp,v 1.3 2006-10-04 14:16:52 ibelyaev Exp $
// Include files 

//
//  Package    : Gen/LbPythia
//
//  Description: Declaration of the components (factory entries)
//               specified by this component library.
//

#include "GaudiKernel/DeclareFactoryEntries.h"

// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_ALGORITHM( MyAlgorithm );
// DECLARE_TOOL( MyTool );
// DECLARE_OBJECT( DataObject );
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

DECLARE_FACTORY_ENTRIES(LbPythia) {
  // New structure:
 
  // Production tools
  DECLARE_TOOL( PythiaProduction ) ;
  DECLARE_TOOL( ReadLHEfile      ) ;
 
  // Cut tools
  DECLARE_TOOL( PythiaHiggs ) ;
  
}
