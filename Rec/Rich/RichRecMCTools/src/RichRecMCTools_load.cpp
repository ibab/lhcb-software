
//-----------------------------------------------------------------------------
/** @file RichRecMCTools_load.cpp
 *
 *  Declaration of objects in component library RichRecMCTools
 *
 *  CVS Log :-
 *  $Id: RichRecMCTools_load.cpp,v 1.12 2007-02-02 10:06:27 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES ( RichRecMCTools )
{

  // Rich reconstruction working object creators that use cheated MC
  // information in one way or another
  DECLARE_TOOL( TrackCreatorFromMCRichTracks             );
  DECLARE_TOOL( PixelCreatorFromCheatedRawBuffer         );
  DECLARE_TOOL( PixelCreatorFromSignalRawBuffer          );
  DECLARE_TOOL( PixelCreatorFromRichDigitsWithBg         );
  DECLARE_TOOL( PhotonCreatorFromMCRichOpticalPhotons    );
  DECLARE_TOOL( PhotonPredictorUsingMCRichOpticalPhotons );
  DECLARE_TOOL( PhotonCreatorWithGaussianCKSmear         );
  DECLARE_TOOL( PhotonCreatorWithMCSignal                );
  DECLARE_TOOL( PhotonCreatorCheatedTrackDir             );

  // MC truth interrogation tools
  DECLARE_TOOL( MCTruthTool );

  // Builds RichTrackSegments from MC information
  DECLARE_TOOL( TrSegMakerFromMCRichTracks );

  // track selector
  DECLARE_TOOL( MCTrueTrackSelector );

};
