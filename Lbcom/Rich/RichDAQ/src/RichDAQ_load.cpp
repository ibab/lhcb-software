// $Id: RichDAQ_load.cpp,v 1.4 2003-11-26 11:11:29 cattanem Exp $
// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( RichDAQ ) {

  // Algorithm to convert RichDigits to Raw Buffer
  DECLARE_ALGORITHM( MCRichDigitsToRawBufferAlg );

  // Algorithm to create RichDigits from Raw Buffer
  DECLARE_ALGORITHM( RawBufferToRichDigitsAlg );

  // Create links between RichDigits and MCRichDigits
  DECLARE_ALGORITHM( BuildMCRichDigitLinks );
  
}
