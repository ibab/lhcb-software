#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( RichReadout  ) {

  // Process the MCHit signals
  DECLARE_ALGORITHM( RichSignal );
  DECLARE_ALGORITHM( RichSignalSICB );

  // Simulate the frontend response
  DECLARE_ALGORITHM( RichSimpleFrontEndResponse ); 
  DECLARE_ALGORITHM( RichDetailedFrontEndResponse ); 

  // Digit creator
  DECLARE_ALGORITHM( RichDigitCreator );

  // Monitors
  DECLARE_ALGORITHM( RichDigiAlgMoni );

}

