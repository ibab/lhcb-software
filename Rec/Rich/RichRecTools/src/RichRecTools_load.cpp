// $Id: RichRecTools_load.cpp,v 1.3 2003-06-30 15:47:05 jonrob Exp $

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( RichRecTools ) {

  // Declare Tools
  DECLARE_TOOL( RichToolRegistry );
  DECLARE_TOOL( RichTrackCreatorFromTrStoredTracks );
  DECLARE_TOOL( RichSegmentCreator );
  DECLARE_TOOL( RichPixelCreatorFromRichDigits );
  DECLARE_TOOL( RichPixelCreatorFromSICB );
  DECLARE_TOOL( RichPhotonCreator );
  DECLARE_TOOL( RichGeomEffDetailed );
  DECLARE_TOOL( RichGeomEffFixedValue );
  DECLARE_TOOL( RichSegmentProperties );
  DECLARE_TOOL( RichPhotonSignal );
  DECLARE_TOOL( RichPhotonPredictor );
  DECLARE_TOOL( RichRecMCTruthTool );
  DECLARE_TOOL( RichTrackProperties );

};
