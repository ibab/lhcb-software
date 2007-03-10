
//=========================================================================
/** @file RichRecTools_load.cpp
 *
 *  Declaration of objects in the component library RichRecTools
 *
 *  CVS Log :-
 *  $Id: RichRecTools_load.cpp,v 1.32 2007-03-10 13:19:20 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//=========================================================================

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( RichRecTools )
{

  // Rich reconstruction working object creators
  DECLARE_TOOL( DelegatedTrackCreatorFromRecoTracks );
  DECLARE_TOOL( TrackCreatorFromRecoTracks );
  DECLARE_TOOL( SegmentCreator );
  DECLARE_TOOL( PixelCreatorFromRawBuffer );
  DECLARE_TOOL( PixelCreatorWithForcedIneffic );
  DECLARE_TOOL( PhotonCreator );
  DECLARE_TOOL( SimplePhotonPredictor );
  DECLARE_TOOL( SepVCKthetaPhotonPredictor );
  DECLARE_TOOL( PhotonPredictorUsingRings );
  DECLARE_TOOL( StatusCreator );
  DECLARE_TOOL( MassHypothesisRingCreator );

  // Detector geometry
  DECLARE_TOOL( GeomTool );

  // Geometrical efficiency calculators
  DECLARE_TOOL( GeomEffPhotonTracing );
  DECLARE_TOOL( GeomEffFixedValue    );

  // Signal calculators
  DECLARE_TOOL( ExpectedTrackSignal );
  DECLARE_TOOL( PhotonSignal );
  DECLARE_TOOL( TabulatedSignalDetectionEff );

  // Physical properties
  DECLARE_TOOL( SellmeirFunc );
  DECLARE_TOOL( FunctionalRayleighScatter );
  DECLARE_TOOL( TabulatedRayleighScatter );
  DECLARE_TOOL( TabulatedGasQuartzWindowAbs );

  // Cherenkov angle tools
  DECLARE_TOOL( CherenkovAngle );
  DECLARE_TOOL( BinnedCKResVthetaForRecoTracks );
  DECLARE_TOOL( FunctionalCKResForRecoTracks );
  DECLARE_TOOL( InterpCKResVthetaForRecoTracks );

  // ray tracing
  DECLARE_TOOL( RayTraceCherenkovCone );

  // track segment makers
  DECLARE_TOOL( DetailedTrSegMakerFromRecoTracks );
  DECLARE_TOOL( FastTrSegMakerFromRecoTracks );

  // photon reconstruction using Quartic solution
  DECLARE_TOOL( PhotonRecoUsingQuarticSoln );

  // Track selectors
  DECLARE_TOOL( TrackSelector        );
  DECLARE_TOOL( BaseTrackSelector    );
  DECLARE_TOOL( SeedTrackSelector    );
  DECLARE_TOOL( MatchTrackSelector   );
  DECLARE_TOOL( ForwardTrackSelector );
  DECLARE_TOOL( VeloTTTrackSelector  );
  DECLARE_TOOL( KsTrackSelector      );

};
