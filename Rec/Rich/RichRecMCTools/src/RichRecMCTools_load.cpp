// $Id: RichRecMCTools_load.cpp,v 1.1.1.1 2003-10-13 16:21:51 jonesc Exp $

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( RichRecMCTools ) {

  // Rich reconstruction working object creators
  DECLARE_TOOL( RichTrackCreatorFromCheatedTrStoredTracks );
  DECLARE_TOOL( RichPixelCreatorFromCheatedRichDigits );
  DECLARE_TOOL( RichPixelCreatorFromMCRichHits );

  // MC truth tools
  DECLARE_TOOL( RichRecMCTruthTool );

};
