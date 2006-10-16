// $Id: MuonDAQ_load.cpp,v 1.5 2006-10-16 08:12:10 cattanem Exp $
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

DECLARE_FACTORY_ENTRIES(MuonDAQ) {
   DECLARE_ALGORITHM(MuonDigitToRawBuffer)
   DECLARE_ALGORITHM(MuonRec)
   DECLARE_TOOL(MuonRawBuffer)
   DECLARE_ALGORITHM(MuonDAQTest)
   DECLARE_ALGORITHM(MuonPadTest)

}
