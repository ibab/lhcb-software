// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h"

// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_CONVERTER( MyConverter );
// DECLARE_OBJECT( DataObject );
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

DECLARE_FACTORY_ENTRIES(VeloDAQ) {
  DECLARE_ALGORITHM( PrepareVeloRawBuffer );
  DECLARE_ALGORITHM( DecodeVeloRawBuffer );
  DECLARE_ALGORITHM( PrepareVeloL1BufferFPGA );
}

