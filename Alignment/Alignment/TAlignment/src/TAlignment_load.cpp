#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( TAlignment ) {
   DECLARE_ALGORITHM(AlignAlgorithm);
   DECLARE_TOOL(AlignOTDetector);
}

