// $Id: RichMCTools_load.cpp,v 1.4 2004-06-17 12:01:43 cattanem Exp $
// Include files

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES(RichMCTools) {

  // MC tracking information for the RICH
  DECLARE_TOOL( RichMCTrackInfoTool );

  // MC truth information
  DECLARE_TOOL( RichMCTruthTool );

  // Builds association table for MCParticles to MCRichTracks
  DECLARE_ALGORITHM( MCPartToMCRichTrackAlg );

  // builds the association table for MCRichHits to MCRichOpticalPhotons
  DECLARE_ALGORITHM( MCRichHitToMCRichOpPhotAlg );

  DECLARE_TOOL( RichTrSegMakerFromMCRichTracks ); 
}
