// $Id: STDAQ_load.cpp,v 1.2 2006-12-18 10:49:46 cattanem Exp $
// Include files 


#include "GaudiKernel/DeclareFactoryEntries.h"

// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_ALGORITHM( MyAlgorithm );
// DECLARE_TOOL( MyTool );
// DECLARE_OBJECT( DataObject );
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

DECLARE_FACTORY_ENTRIES(STDAQ) {
  DECLARE_ALGORITHM( STClustersToRawBankAlg );
  DECLARE_ALGORITHM( RawBankToSTClusterAlg );
  DECLARE_ALGORITHM( RawBankToSTLiteClusterAlg );
  DECLARE_ALGORITHM( STRawBankMonitor );

  DECLARE_TOOL( ITReadoutTool );
  DECLARE_TOOL( TTReadoutTool );
}
