// $Id: TrackExtrapolators_load.cpp,v 1.3 2005-07-06 17:03:03 ebos Exp $
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
  DECLARE_TOOL( TrackMasterExtrapolator );
                                                 
}
