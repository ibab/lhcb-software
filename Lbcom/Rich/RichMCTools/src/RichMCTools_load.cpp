// $Id: RichMCTools_load.cpp,v 1.1.1.1 2004-01-29 16:39:24 jonesc Exp $
// Include files

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES(RichMCTools) {

  // MC tracking information for the RICH
  DECLARE_TOOL( RichMCTrackInfoTool );

}
