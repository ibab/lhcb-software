// $Id: RichDAQ_load.cpp,v 1.1.1.1 2003-11-08 14:26:20 jonesc Exp $
// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( RichDAQ ) {

  // Algorithm to convert RichDigits to HLT Buffer
  DECLARE_ALGORITHM( MCRichDigitsToHLTBuffAlg );

  // Algorithm to create RichDigits from HLT Buffer
  DECLARE_ALGORITHM( HLTBuffToRichDigitsAlg );
  
}
