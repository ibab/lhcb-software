// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/Magnet/src/Magnet_load.cpp,v 1.1.1.1 2002-05-21 09:12:33 gcorti Exp $

// Include Files
#include "GaudiKernel/DeclareFactoryEntries.h"

// Using the MACROS in the previous include file declare all "components" of
// the library.
DECLARE_FACTORY_ENTRIES(Magnet) {
  DECLARE_SERVICE( MagneticFieldSvc );
}
