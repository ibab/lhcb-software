// $Id: HltTracking_load.cpp,v 1.3 2006-09-25 09:15:30 hernando Exp $
// Include files 


#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES(HltTracking) {

  DECLARE_TOOL( HltTrackSelector );

  DECLARE_ALGORITHM( HltTracking );

}
