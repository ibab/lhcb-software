// $Id: RichRecTools_load.cpp,v 1.2 2002-11-14 13:54:25 jonrob Exp $

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( RichRecTools ) {

  // Declare Tools
  DECLARE_TOOL( RichRecPhotonTool );
  DECLARE_TOOL( RichRecSegmentTool );
  DECLARE_TOOL( RichRecTrackTool );
  DECLARE_TOOL( RichRecPixelTool );
  DECLARE_TOOL( RichRecMCTruthTool );

};
