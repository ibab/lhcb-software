// $Id: DAQEvent_load.cpp,v 1.5 2006-10-10 09:07:22 cattanem Exp $

// Include files 
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES(DAQEvent) {
  DECLARE_TOOL( OdinTimeDecoder );
  DECLARE_ALGORITHM( RawEventDump );
  DECLARE_ALGORITHM( bankKiller );
}
