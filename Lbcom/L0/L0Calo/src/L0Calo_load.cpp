// $Id: L0Calo_load.cpp,v 1.5 2003-04-03 16:21:40 cattanem Exp $
// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h"

// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_CONVERTER( MyConverter );
// DECLARE_OBJECT( DataObject );
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

DECLARE_FACTORY_ENTRIES(L0Calo) {

  DECLARE_ALGORITHM( L0CaloDigit );
  DECLARE_ALGORITHM( L0CaloAlg   );
  DECLARE_ALGORITHM( L0CaloMonit );
  DECLARE_ALGORITHM( L0CaloToMCParticleAsct );
}
