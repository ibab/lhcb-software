// $Id: TrackExtrapolators_load.cpp,v 1.1 2005-03-10 14:41:03 hernando Exp $
// Include files 


#include "GaudiKernel/DeclareFactoryEntries.h"

// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_CONVERTER( MyConverter );
// DECLARE_OBJECT( DataObject );
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

DECLARE_FACTORY_ENTRIES(TrackExtrapolators) {

  DECLARE_TOOL( TrackLinearExtrapolator );
                                                 
}
