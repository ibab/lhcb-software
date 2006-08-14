
//=========================================================================
/** @file RichRecTools_load.cpp
 *
 *  Declaration of objects in the component library RichRecTools
 *
 *  CVS Log :-
 *  $Id: RichRecTools_load.cpp,v 1.28 2006-08-14 10:06:02 jonrob Exp $
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
  DECLARE_TOOL( RichFunctionalCKResVpForRecoTracks );
  DECLARE_TOOL( RichInterpCKResVthetaForRecoTracks );
  DECLARE_TOOL( RichInterpCKResVpForRecoTracks     );

  // ray tracing
  DECLARE_TOOL( RichRayTraceCherenkovCone );

  // track segment makers
  DECLARE_TOOL( RichDetailedTrSegMakerFromRecoTracks ); 
  DECLARE_TOOL( RichFastTrSegMakerFromRecoTracks ); 

  // photon reconstruction using Quartic solution
  DECLARE_TOOL( RichPhotonRecoUsingQuarticSoln ); 

  // Track selectors
  DECLARE_NAMESPACE_TOOL( Rich, RichTrackSelector      );
  DECLARE_NAMESPACE_TOOL( Rich, RichBaseTrackSelector  );
  DECLARE_NAMESPACE_TOOL( Rich, RichSeedTrackSelector  );
  DECLARE_NAMESPACE_TOOL( Rich, RichMatchTrackSelector );

};
