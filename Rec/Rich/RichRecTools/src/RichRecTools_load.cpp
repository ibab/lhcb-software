
//=========================================================================
/** @file RichRecTools_load.cpp
 *
 *  Declaration of objects in the component library RichRecTools
 *
 *  CVS Log :-
 *  $Id: RichRecTools_load.cpp,v 1.20 2005-05-13 15:20:38 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//=========================================================================

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( RichRecTools ) 
{

  // Rich reconstruction working object creators
  DECLARE_TOOL( RichDelegatedTrackCreatorFromTrStoredTracks );
  DECLARE_TOOL( RichTrackCreatorFromTrStoredTracks );
  DECLARE_TOOL( RichSegmentCreator );
  DECLARE_TOOL( RichPixelCreatorFromRichDigits );
  DECLARE_TOOL( RichPixelCreatorFromRawBuffer );
  DECLARE_TOOL( RichPhotonCreator );
  DECLARE_TOOL( RichPhotonPredictor );
  DECLARE_TOOL( RichPhotonPredictorUsingRings );
  DECLARE_TOOL( RichStatusCreator ); 
  DECLARE_TOOL( RichMassHypothesisRingCreator );

  // Detector geometry
  DECLARE_TOOL( RichRecGeomTool );

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
  DECLARE_TOOL( RichTabulatedGasQuartzWindowAbs );

  // Cherenkov angle tools
  DECLARE_TOOL( RichCherenkovAngle );
  DECLARE_TOOL( RichBinnedCKResVthetaForTrStoredTracks );
  DECLARE_TOOL( RichInterpCKResVthetaForTrStoredTracks );
  DECLARE_TOOL( RichFunctionalCKResVpForTrStoredTracks );
  DECLARE_TOOL( RichInterpCKResVpForTrStoredTracks );

  // ray tracing
  DECLARE_TOOL( RichRayTraceCherenkovCone );

};
