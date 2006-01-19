// $Id: HltTracking_load.cpp,v 1.1.1.1 2006-01-19 10:11:25 hernando Exp $
// Include files 


#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES(HltTracking) {

  DECLARE_ALGORITHM( HltVeloTT );
  DECLARE_ALGORITHM( HltForward );

}
