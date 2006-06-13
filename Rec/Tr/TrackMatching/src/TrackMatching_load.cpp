// $Id: TrackMatching_load.cpp,v 1.3 2006-06-13 15:33:26 jvantilb Exp $
// include files
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( TrackMatching ) {

  DECLARE_ALGORITHM( TrackMatchVeloSeed );
  DECLARE_ALGORITHM( TrackMatchChecker );
  DECLARE_TOOL( AddTTClusterTool );

}















