// $Id: RichDAQ_load.cpp,v 1.3 2003-11-10 14:59:59 jonrob Exp $
// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( RichDAQ ) {

  // Algorithm to convert RichDigits to HLT Buffer
  DECLARE_ALGORITHM( MCRichDigitsToHLTBuffAlg );

  // Algorithm to create RichDigits from HLT Buffer
  DECLARE_ALGORITHM( HLTBuffToRichDigitsAlg );

  // Create links between RichDigits and MCRichDigits
  DECLARE_ALGORITHM( BuildMCRichDigitLinks );

  // Converter for RichDigits
  //DECLARE_CONVERTER( HLTBuffToRichDigitsCnv );
  
}
