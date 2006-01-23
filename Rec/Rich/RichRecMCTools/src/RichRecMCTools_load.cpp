
//-----------------------------------------------------------------------------
/** @file RichRecMCTools_load.cpp
 *
 *  Declaration of objects in component library RichRecMCTools
 *
 *  CVS Log :-
 *  $Id: RichRecMCTools_load.cpp,v 1.7 2006-01-23 14:09:59 jonrob Exp $
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
  DECLARE_TOOL( RichPixelCreatorFromCheatedRichDigits        );
  DECLARE_TOOL( RichPixelCreatorFromSignalRichDigits         );
  DECLARE_TOOL( RichPixelCreatorFromMCRichHits               );
  DECLARE_TOOL( RichPixelCreatorFromRichDigitsWithBg         );
  DECLARE_TOOL( RichPhotonCreatorFromMCRichOpticalPhotons    );
  DECLARE_TOOL( RichPhotonPredictorUsingMCRichOpticalPhotons );
  DECLARE_TOOL( RichPhotonCreatorWithGaussianCKSmear         );
  DECLARE_TOOL( RichPhotonCreatorWithMCSignal                );

  // MC truth interrogation tools
  DECLARE_TOOL( RichRecMCTruthTool );

  // Builds RichTrackSegments from MC information
  DECLARE_TOOL( RichTrSegMakerFromMCRichTracks );

};
