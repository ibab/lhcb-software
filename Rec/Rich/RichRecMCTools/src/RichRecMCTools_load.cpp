// $Id: RichRecMCTools_load.cpp,v 1.2 2004-06-10 14:40:52 jonesc Exp $

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( RichRecMCTools ) {

  // Rich reconstruction working object creators that use cheated MC 
  // information in one way or another
  DECLARE_TOOL( RichPixelCreatorFromCheatedRichDigits        );
  DECLARE_TOOL( RichPixelCreatorFromSignalRichDigits         );
  DECLARE_TOOL( RichPixelCreatorFromMCRichHits               );
  DECLARE_TOOL( RichPhotonCreatorFromMCRichOpticalPhotons    );
  DECLARE_TOOL( RichPhotonPredictorUsingMCRichOpticalPhotons );

  // MC truth interrogation tools
  DECLARE_TOOL( RichRecMCTruthTool );

};
