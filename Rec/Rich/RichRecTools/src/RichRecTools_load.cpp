// $Id: RichRecTools_load.cpp,v 1.1.1.1 2002-07-28 10:46:20 jonesc Exp $

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( RichRecTools ) {

  // Declare algorithms
  DECLARE_ALGORITHM( RichRecRecoTracks ); // Form tracks from pure reco information
  DECLARE_ALGORITHM( RichRecRecoPixels ); // Form pixels from pure reco information
  DECLARE_ALGORITHM( RichRecMCTracks ); // Form tracks from pure MC information
  DECLARE_ALGORITHM( RichRecMCPixels ); // Form pixels from pure MC information

  // Declare Tools
  DECLARE_TOOL( RichRecPhotonTool ); // Tool for forming photon candidates from tracks and pixels
  DECLARE_TOOL( RichRecSegmentTool ); // Tool for manipulating RichRecSegments
  DECLARE_TOOL( RichRecTrackTool ); // Tool which performs useful methods on RichRecTracks
  DECLARE_TOOL( RichRecDataTool ); // Tool for manipulating RichData in TDS

};
