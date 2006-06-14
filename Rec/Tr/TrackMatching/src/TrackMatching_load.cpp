// $Id: TrackMatching_load.cpp,v 1.4 2006-06-14 19:45:54 jvantilb Exp $
// include files
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( TrackMatching ) {

  DECLARE_ALGORITHM( TrackMatchVeloSeed );
  DECLARE_TOOL( AddTTClusterTool );

}















