// $Id: L0DU_load.cpp,v 1.5 2004-01-26 14:24:05 odescham Exp $
// Include files 


#include "GaudiKernel/DeclareFactoryEntries.h"

// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_CONVERTER( MyConverter );
// DECLARE_OBJECT( DataObject );
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

DECLARE_FACTORY_ENTRIES(L0DU) {
  DECLARE_ALGORITHM( DecisionUnit );
  DECLARE_ALGORITHM( PrepareL0DUL1Buffer );
  DECLARE_ALGORITHM( L0Monitor    );
}
