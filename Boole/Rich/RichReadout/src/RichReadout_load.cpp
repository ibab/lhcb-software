#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( RichReadout  ) {

  DECLARE_ALGORITHM( RichSignal );
  DECLARE_ALGORITHM( RichReadout ); 
  DECLARE_ALGORITHM( RichDigitCreator );
}

