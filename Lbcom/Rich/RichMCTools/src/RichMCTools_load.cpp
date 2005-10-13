
//-----------------------------------------------------------------------------
/** @file RichMCTools_load.cpp
 *
 * Declaration of entries in the RichMCTools component library
 *
 * CVS Log :-
 * $Id: RichMCTools_load.cpp,v 1.7 2005-10-13 15:23:04 jonrob Exp $
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

  // Builds RichTrackSegments from MC information
  DECLARE_TOOL( RichTrSegMakerFromMCRichTracks );

}
