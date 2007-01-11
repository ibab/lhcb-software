// $Id: MuonAssociators_load.cpp,v 1.4 2007-01-11 13:12:42 asatta Exp $
// Include files 
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( MuonAssociators ) 
{
  DECLARE_ALGORITHM( MuonDigit2MCParticleAlg );
  DECLARE_ALGORITHM( MuonTileDigitInfo );
  DECLARE_TOOL( MuonPad2MCTool );

}
