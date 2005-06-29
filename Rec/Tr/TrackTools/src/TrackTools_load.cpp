#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( TrackTools ) {

  DECLARE_TOOL( BIntegrator );
  DECLARE_TOOL( TrackPtKick );                                                 
  DECLARE_TOOL( MeasurementProvider );
}
