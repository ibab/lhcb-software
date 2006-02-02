// $Id: OTDAQ_load.cpp,v 1.5 2006-02-02 15:31:50 janos Exp $
// Include files 
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES(OTDAQ) {
  DECLARE_TOOL( OTReadOutWindow );
  DECLARE_ALGORITHM( OTTimeCreator );
  DECLARE_ALGORITHM( OTFillEventFromOTTime );
}
