// $Id: L0DU_load.cpp,v 1.6 2006-02-18 11:29:37 ocallot Exp $
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
  DECLARE_TOOL( L0DUMakerTools );
  DECLARE_TOOL( L0CandidateTool );
  DECLARE_TOOL( L0Monitor );
}
