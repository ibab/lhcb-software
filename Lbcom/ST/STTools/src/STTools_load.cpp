// $Id: STTools_load.cpp,v 1.3 2007-01-09 15:02:25 jvantilb Exp $
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( STTools ) {

  DECLARE_TOOL( STSignalToNoiseTool );
  DECLARE_TOOL( STOfflinePosition );
  DECLARE_TOOL( STOnlinePosition );

}
