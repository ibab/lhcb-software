// $Id: MuonAssociators_load.cpp,v 1.3 2006-01-31 07:48:23 cattanem Exp $
// Include files 
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( MuonAssociators ) 
{
  DECLARE_ALGORITHM( MuonDigit2MCParticleAlg );
  DECLARE_ALGORITHM( MuonTileDigitInfo );
}
