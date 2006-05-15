// $Id: TrackMatching_load.cpp,v 1.2 2006-05-15 13:45:28 jvantilb Exp $
// include files
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( TrackMatching ) {

  DECLARE_ALGORITHM( TrackMatchVeloSeed );
  DECLARE_TOOL( AddTTClusterTool );

}















