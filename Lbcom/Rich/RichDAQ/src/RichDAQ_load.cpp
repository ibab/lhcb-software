// $Id: RichDAQ_load.cpp,v 1.2 2003-11-09 12:39:29 jonrob Exp $
// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( RichDAQ ) {

  // Algorithm to convert RichDigits to HLT Buffer
  DECLARE_ALGORITHM( MCRichDigitsToHLTBuffAlg );

  // Algorithm to create RichDigits from HLT Buffer
  DECLARE_ALGORITHM( HLTBuffToRichDigitsAlg );

  // Converter for RichDigits
  DECLARE_CONVERTER( HLTBuffToRichDigitsCnv );
  
}
