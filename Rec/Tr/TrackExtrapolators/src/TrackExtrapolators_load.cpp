// $Id: TrackExtrapolators_load.cpp,v 1.2 2005-03-16 14:10:05 hernando Exp $
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
  DECLARE_TOOL( TrackParabolicExtrapolator );
  DECLARE_TOOL( TrackFastParabolicExtrapolator );
  DECLARE_TOOL( TrackHerabExtrapolator );
  DECLARE_TOOL( TrackFirstCleverExtrapolator );
                                                 
}
