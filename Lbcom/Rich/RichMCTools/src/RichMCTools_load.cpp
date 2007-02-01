
//-----------------------------------------------------------------------------
/** @file RichMCTools_load.cpp
 *
 * Declaration of entries in the RichMCTools component library
 *
 * CVS Log :-
 * $Id: RichMCTools_load.cpp,v 1.9 2007-02-01 17:50:13 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 14/01/2002
 */
//-----------------------------------------------------------------------------

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES(RichMCTools)
{

  // MC tracking information for the RICH
  DECLARE_TOOL( MCTrackInfoTool );

  // MC truth information
  DECLARE_TOOL( MCTruthTool );

}
