// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/Gaucho/src/Gaucho_load.cpp,v 1.1.1.1 2003-06-05 09:34:46 vanphil Exp $

// Include Files
#include "GaudiKernel/DeclareFactoryEntries.h"

// Using the MACROS in the previous include file declare all "components" of
// the library.
DECLARE_FACTORY_ENTRIES(Gaucho) {
  DECLARE_SERVICE( MonitorSvc );
//! declare DimPropServer ???
}

