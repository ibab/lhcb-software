
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/ObjectFactory.h"

DECLARE_FACTORY_ENTRIES( HltSelChecker ) {

  // Algorithms
  DECLARE_ALGORITHM( TrackNTuple );
  DECLARE_ALGORITHM( PlotTrackDeviation );
  DECLARE_ALGORITHM( TestHltScore );
}

