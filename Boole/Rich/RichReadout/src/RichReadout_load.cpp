#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( RichReadout  ) {

  // Process the HPD MCHit signals
  DECLARE_ALGORITHM( RichHPDSignal );
  DECLARE_ALGORITHM( RichHPDSignalSICB );

  // Simulate the HPD frontend response
  DECLARE_ALGORITHM( RichSimpleHPDFrontEndResponse ); 
  DECLARE_ALGORITHM( RichDetailedHPDFrontEndResponse ); 

  // Process the MaPMT MCHit signals
  DECLARE_ALGORITHM( RichMaPMTSignal ); 
  DECLARE_ALGORITHM( RichMaPMTSignalSICB );

  // Simulate the HPD frontend response
  DECLARE_ALGORITHM( RichSimpleMaPMTFrontEndResponse ); 
  DECLARE_ALGORITHM( RichDetailedMaPMTFrontEndResponse ); 

  // Digit creator
  DECLARE_ALGORITHM( RichDigitCreator );

  // Monitors
  DECLARE_ALGORITHM( RichDigiAlgMoni );

}

