// $Id: CaloDigit_load.cpp,v 1.8 2007-02-22 23:57:20 odescham Exp $
// Include files 


#include "GaudiKernel/DeclareFactoryEntries.h"
 
// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_CONVERTER( MyConverter );
// DECLARE_OBJECT( DataObject );
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

DECLARE_FACTORY_ENTRIES(CaloDigit) {
 
  DECLARE_ALGORITHM( CaloSignalAlg );
  DECLARE_ALGORITHM( CaloDigitAlg  );
  DECLARE_ALGORITHM( CaloPinDigitAlg  );
}
