// $Id: MuonDAQ_load.cpp,v 1.2 2004-02-10 17:08:25 asatta Exp $
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
   DECLARE_ALGORITHM(MuonRawBuffer2Digit)
//   DECLARE_ALGORITHM(MuonDAQTest)

}
