// $Id: PuVeto_load.cpp,v 1.1.1.1 2002-01-29 09:37:12 ocallot Exp $
// Include files 


#include "GaudiKernel/DeclareFactoryEntries.h"

// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_CONVERTER( MyConverter );
// DECLARE_OBJECT( DataObject );
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

DECLARE_FACTORY_ENTRIES(PuVeto) {
  DECLARE_ALGORITHM( PuVetoAlg );
}
