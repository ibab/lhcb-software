// $Id: L0DU_load.cpp,v 1.4 2002-06-23 13:37:38 ocallot Exp $
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
  DECLARE_ALGORITHM( L0Monitor    );
}
