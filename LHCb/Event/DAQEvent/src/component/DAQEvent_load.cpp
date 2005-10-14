// $Id: DAQEvent_load.cpp,v 1.3 2005-10-14 12:48:50 cattanem Exp $

// Include files 
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES(DAQEvent) {
  DECLARE_ALGORITHM( RawEventDump );
}
