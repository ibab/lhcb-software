// $Id: OTDAQ_load.cpp,v 1.3 2004-09-10 13:14:58 cattanem Exp $
// Include files 
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES(OTDAQ) {
  DECLARE_TOOL( OTReadoutWindow );
  DECLARE_ALGORITHM( OTTimeCreator );
  DECLARE_ALGORITHM( OTFillBufferFromOTTime );
}
