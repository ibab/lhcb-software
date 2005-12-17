
//-----------------------------------------------------------------------------
/** @file RichMCTools_load.cpp
 *
 * Declaration of entries in the RichMCTools component library
 *
 * CVS Log :-
 * $Id: RichMCTools_load.cpp,v 1.8 2005-12-17 14:18:15 jonrob Exp $
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

}
