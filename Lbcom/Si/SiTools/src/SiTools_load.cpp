// $Id: SiTools_load.cpp,v 1.1.1.1 2006-05-16 08:33:54 mneedham Exp $
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

DECLARE_FACTORY_ENTRIES(SiTools) {
  DECLARE_TOOL( SiDepositedCharge );
  DECLARE_TOOL( SiGeantDepositedCharge );

  DECLARE_TOOL( SiAmplifierResponse );

}
