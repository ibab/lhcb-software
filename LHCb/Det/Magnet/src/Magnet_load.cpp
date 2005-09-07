// $Id: Magnet_load.cpp,v 1.3 2005-09-07 12:20:25 cattanem Exp $

// Include Files
#include "GaudiKernel/DeclareFactoryEntries.h"

// Using the MACROS in the previous include file declare all "components" of
// the library.
DECLARE_FACTORY_ENTRIES(Magnet) {
  DECLARE_SERVICE( MagneticFieldSvc );
  DECLARE_ALGORITHM( MagFieldReader );
  DECLARE_TOOL( BdlTool );
  DECLARE_TOOL( BIntegrator );
}
