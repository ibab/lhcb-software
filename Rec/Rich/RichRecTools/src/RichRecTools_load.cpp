// $Id: RichRecTools_load.cpp,v 1.12 2004-02-02 14:27:02 jonesc Exp $

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( RichRecTools ) {

  // Rich reconstruction working object creators
  DECLARE_TOOL( RichTrackCreatorFromTrStoredTracks );
  DECLARE_TOOL( RichSegmentCreator );
  DECLARE_TOOL( RichPixelCreatorFromRichDigits );
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

  // Physical properties
  DECLARE_TOOL( RichSellmeirFunc );
  DECLARE_TOOL( RichParticleProperties );
  DECLARE_TOOL( RichFunctionalRayleighScatter );
  DECLARE_TOOL( RichTabulatedRayleighScatter );

  // Cherenkov angle tools
  DECLARE_TOOL( RichCherenkovAngle );
  DECLARE_TOOL( RichBinnedCherenkovResolution );
  DECLARE_TOOL( RichInterpCherenkovResolution );

};
