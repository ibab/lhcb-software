
//=========================================================================
/** @file RichRecTools_load.cpp
 *
 *  Declaration of objects in the component library RichRecTools
 *
 *  CVS Log :-
 *  $Id: RichRecTools_load.cpp,v 1.30 2006-11-23 18:08:29 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//=========================================================================

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( RichRecTools ) 
{

  // Rich reconstruction working object creators
  DECLARE_TOOL( RichDelegatedTrackCreatorFromRecoTracks );
  DECLARE_TOOL( RichTrackCreatorFromRecoTracks );
  DECLARE_TOOL( RichSegmentCreator );
  DECLARE_TOOL( RichPixelCreatorFromRawBuffer );
  DECLARE_TOOL( RichPixelCreatorWithForcedIneffic );
  DECLARE_TOOL( RichPhotonCreator );
  DECLARE_TOOL( RichSimplePhotonPredictor );
  DECLARE_TOOL( RichSepVCKthetaPhotonPredictor );
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
  DECLARE_TOOL( RichFunctionalRayleighScatter );
  DECLARE_TOOL( RichTabulatedRayleighScatter );
  DECLARE_TOOL( RichTabulatedGasQuartzWindowAbs );

  // Cherenkov angle tools
  DECLARE_TOOL( RichCherenkovAngle );
  DECLARE_TOOL( RichBinnedCKResVthetaForRecoTracks );
  DECLARE_TOOL( RichFunctionalCKResForRecoTracks );
  DECLARE_TOOL( RichInterpCKResVthetaForRecoTracks );

  // ray tracing
  DECLARE_TOOL( RichRayTraceCherenkovCone );

  // track segment makers
  DECLARE_TOOL( RichDetailedTrSegMakerFromRecoTracks ); 
  DECLARE_TOOL( RichFastTrSegMakerFromRecoTracks ); 

  // photon reconstruction using Quartic solution
  DECLARE_TOOL( RichPhotonRecoUsingQuarticSoln ); 

  // Track selectors
  DECLARE_NAMESPACE_TOOL( Rich, RichTrackSelector        );
  DECLARE_NAMESPACE_TOOL( Rich, RichBaseTrackSelector    );
  DECLARE_NAMESPACE_TOOL( Rich, RichSeedTrackSelector    );
  DECLARE_NAMESPACE_TOOL( Rich, RichMatchTrackSelector   );
  DECLARE_NAMESPACE_TOOL( Rich, RichForwardTrackSelector );
  DECLARE_NAMESPACE_TOOL( Rich, RichVeloTTTrackSelector  );
  DECLARE_NAMESPACE_TOOL( Rich, RichKsTrackSelector      );

};
