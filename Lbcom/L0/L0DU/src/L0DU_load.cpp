// $Id: L0DU_load.cpp,v 1.8 2006-05-25 22:37:17 odescham Exp $
// Include files 

#include "GaudiKernel/ContainerFactoryDefs.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_CONVERTER( MyConverter );


// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

DECLARE_FACTORY_ENTRIES(L0DU) {
  DECLARE_ALGORITHM( L0DU );
  DECLARE_ALGORITHM( L0Monitor );
  DECLARE_TOOL( L0DUMakerTools );
  DECLARE_TOOL( L0CandidateTool );
  DECLARE_ALGORITHM( L0DUFromRaw );
}
