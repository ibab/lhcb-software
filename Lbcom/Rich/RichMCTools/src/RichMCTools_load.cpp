
//-----------------------------------------------------------------------------
/** @file RichMCTools_load.cpp
 *
 * Declaration of entries in the RichMCTools component library
 *
 * CVS Log :-
 * $Id: RichMCTools_load.cpp,v 1.5 2004-07-26 17:56:09 jonrob Exp $
 * $Log: not supported by cvs2svn $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 14/01/2002
 */
//-----------------------------------------------------------------------------

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES(RichMCTools)
{

  // MC tracking information for the RICH
  DECLARE_TOOL( RichMCTrackInfoTool );

  // MC truth information
  DECLARE_TOOL( RichMCTruthTool );

  // Builds association table for MCParticles to MCRichTracks
  DECLARE_ALGORITHM( MCPartToMCRichTrackAlg );

  // builds the association table for MCRichHits to MCRichOpticalPhotons
  DECLARE_ALGORITHM( MCRichHitToMCRichOpPhotAlg );

  DECLARE_TOOL( RichTrSegMakerFromMCRichTracks );

}
