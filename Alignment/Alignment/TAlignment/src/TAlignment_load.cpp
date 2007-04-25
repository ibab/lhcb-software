#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( TAlignment ) {
  DECLARE_ALGORITHM(AlignAlgorithm);
  DECLARE_ALGORITHM(AlignOTDetector);
  DECLARE_ALGORITHM( TStation );
  DECLARE_ALGORITHM( GAlign );
}

