// $Id: CaloDigit_load.cpp,v 1.7 2006-03-15 12:54:14 odescham Exp $
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
}
