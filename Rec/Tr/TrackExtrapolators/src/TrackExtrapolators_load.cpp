// $Id: TrackExtrapolators_load.cpp,v 1.4 2005-11-09 14:43:46 erodrigu Exp $
// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( TrackExtrapolators ) {

  DECLARE_TOOL( TrackLinearExtrapolator        );
  DECLARE_TOOL( TrackParabolicExtrapolator     );
  DECLARE_TOOL( TrackFastParabolicExtrapolator );
  DECLARE_TOOL( TrackHerabExtrapolator         );
  DECLARE_TOOL( TrackMasterExtrapolator        );
  DECLARE_TOOL( TrackSimpleExtraSelector       );
  DECLARE_TOOL( TrackLongExtraSelector         );
  DECLARE_TOOL( TrackDistanceExtraSelector     );

}
