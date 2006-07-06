#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( TrackCheckers ) {

  DECLARE_ALGORITHM( TrackChecker );
  DECLARE_ALGORITHM( TrackVeloTTChecker );
  DECLARE_ALGORITHM( TrackMatchChecker );
  DECLARE_ALGORITHM( ExtrapolatorChecker );

}
