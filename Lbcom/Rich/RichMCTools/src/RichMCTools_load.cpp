// $Id: RichMCTools_load.cpp,v 1.2 2004-03-16 13:39:16 jonesc Exp $
// Include files

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES(RichMCTools) {

  // MC tracking information for the RICH
  DECLARE_TOOL( RichMCTrackInfoTool );

  // Builds association table for MCParticles to MCRichTracks
  DECLARE_ALGORITHM( MCPartToMCRichTrackAlg );

  // builds the association table for MCRichHits to MCRichOpticalPhotons
  DECLARE_ALGORITHM( MCRichHitToMCRichOpPhotAlg );

}
