// $Id: OTDAQ_load.cpp,v 1.4 2004-09-17 10:13:05 cattanem Exp $
// Include files 
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES(OTDAQ) {
  DECLARE_TOOL( OTReadOutWindow );
  DECLARE_ALGORITHM( OTTimeCreator );
  DECLARE_ALGORITHM( OTFillBufferFromOTTime );
}
