// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/Magnet/src/Magnet_load.cpp,v 1.2 2004-04-07 15:24:34 cattanem Exp $

// Include Files
#include "GaudiKernel/DeclareFactoryEntries.h"

// Using the MACROS in the previous include file declare all "components" of
// the library.
DECLARE_FACTORY_ENTRIES(Magnet) {
  DECLARE_SERVICE( MagneticFieldSvc );
  DECLARE_ALGORITHM( MagFieldReader );
}
