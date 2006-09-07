// $Id: DAQEvent_load.cpp,v 1.4 2006-09-07 17:03:29 odescham Exp $

// Include files 
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES(DAQEvent) {
  DECLARE_ALGORITHM( RawEventDump );
  DECLARE_ALGORITHM( bankKiller );
}
