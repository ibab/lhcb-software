// $Id: RichRecTools_load.cpp,v 1.11 2003-11-25 14:06:41 jonrob Exp $

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( RichRecTools ) {

  // Rich reconstruction working object creators
  DECLARE_TOOL( RichTrackCreatorFromTrStoredTracks );
  DECLARE_TOOL( RichSegmentCreator );
  DECLARE_TOOL( RichPixelCreatorFromRichDigits );
  DECLARE_TOOL( RichPixelCreatorFromSICB ); // temporary
  DECLARE_TOOL( RichPhotonCreator );
  DECLARE_TOOL( RichPhotonPredictor );
  DECLARE_TOOL( RichStatusCreator );

  // Geometrical efficiency calculators
  DECLARE_TOOL( RichGeomEffPhotonTracing );
  DECLARE_TOOL( RichGeomEffFixedValue );

  // Signal calculators
  DECLARE_TOOL( RichExpectedTrackSignal );
  DECLARE_TOOL( RichPhotonSignal );
  DECLARE_TOOL( RichTabulatedSignalDetectionEff );
  DECLARE_TOOL( RichTabulatedSignalDetectionEffSICB );  // temporary
  DECLARE_TOOL( RichSignalDetectionEffSICB );

  // Physical properties
  DECLARE_TOOL( RichSellmeirFunc );
  DECLARE_TOOL( RichSellmeirFuncSICB );   // temporary
  DECLARE_TOOL( RichParticleProperties );
  DECLARE_TOOL( RichFunctionalRayleighScatter );
  DECLARE_TOOL( RichFunctionalRayleighScatterSICB );  // temporary
  DECLARE_TOOL( RichTabulatedRayleighScatter );

  // Cherenkov angle tools
  DECLARE_TOOL( RichCherenkovAngle );
  DECLARE_TOOL( RichBinnedCherenkovResolution );
  DECLARE_TOOL( RichInterpCherenkovResolution );

};
