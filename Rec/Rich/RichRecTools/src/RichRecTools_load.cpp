// $Id: RichRecTools_load.cpp,v 1.5 2003-08-06 11:08:13 jonrob Exp $

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
  DECLARE_TOOL( RichExpectedTrackSignal );
  DECLARE_TOOL( RichPhotonSignal );
  DECLARE_TOOL( RichPhotonPredictor );
  DECLARE_TOOL( RichRecMCTruthTool );
  DECLARE_TOOL( RichStatusCreator );
  DECLARE_TOOL( RichRefractiveIndex );
  DECLARE_TOOL( RichSellmeirFunc );
  DECLARE_TOOL( RichCherenkovAngle );
  DECLARE_TOOL( RichBinnedCherenkovResolution );
  DECLARE_TOOL( RichSignalDetectionEff );
  DECLARE_TOOL( RichParticleProperties );

};
