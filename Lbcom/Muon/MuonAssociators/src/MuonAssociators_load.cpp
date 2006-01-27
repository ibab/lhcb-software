// $Id: MuonAssociators_load.cpp,v 1.2 2006-01-27 17:15:59 asarti Exp $
// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/ContainerFactoryDefs.h"

// local
#include "MuonDigit2MCParticleAlg.h"
#include "Linker/LinkerWithKey.h"
#include "Event/IntLink.h"

DECLARE_FACTORY_ENTRIES( MuonAssociators ) 
{
  DECLARE_ALGORITHM( MuonDigit2MCParticleAlg );
  DECLARE_ALGORITHM( MuonTileDigitInfo );
}
