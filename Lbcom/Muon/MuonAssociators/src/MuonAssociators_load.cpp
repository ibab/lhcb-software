// $Id: MuonAssociators_load.cpp,v 1.5 2007-01-12 10:32:45 asatta Exp $
// Include files 
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( MuonAssociators ) 
{
  DECLARE_ALGORITHM( MuonDigit2MCParticleAlg );
  DECLARE_ALGORITHM( MuonTileDigitInfo );
  DECLARE_TOOL( MuonPad2MCTool );
  DECLARE_ALGORITHM( MuonCoord2MCParticleLink );

}
